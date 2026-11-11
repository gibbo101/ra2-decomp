"""Build the function map of gamemd.exe: data/functions.csv.

Usage: uv run tools/functions.py [EXE] [OUT_CSV] [--no-asm]

Canonical regeneration uses MASM and the pinned archive for any .asm files in src/.
Use --no-asm with a separate output path for a map without the archive evidence.

A function start needs at least one piece of evidence, recorded as letters:
  P  the alignment padding the compiler puts between functions ends here
  C  a direct call targets it
  V  a pointer in .rdata or .data (vtables, callbacks) targets it
  Y  YRpp names it (data/symbols.csv)
  G  Ghidra's analysis found it (build/ghidra/functions.csv, when present)
  L  a prebuilt library function was found here (data/library.csv)
  A  a public entry in a uniquely located archived assembly section or procedure run
  F  code after a function's return or switch tables that no path through that function
     reaches, at an aligned address: a function nothing calls directly, placed with no
     padding before it
Every start must also be an instruction boundary of a linear sweep of .text. A
function's size runs to the next start, less the padding (nop or int3) before it.
"""

import argparse
import bisect
import csv
import re
import struct
import tempfile
from pathlib import Path

import capstone
import pefile

from exebuild import build_of, restore

ROOT = Path(__file__).resolve().parent.parent
NOP = 0x90
PADDING = (NOP, 0xCC)
REGISTER_IMMEDIATE = re.compile(r"e\w+, 0x[0-9a-f]+")
TABLE_OPERAND = re.compile(r"(?:dword ptr \[\w+\*4 \+ |byte ptr \[\w+ \+ )(0x[0-9a-f]+)\]")


def sweep(code: bytes, base: int, starts: set, padded: set, calls: set, tables: set,
          after_end: set, at: int | None = None):
    """Linear sweep from `at` (default: the start of .text), adding instruction starts,
    padded boundaries, direct call targets, switch tables kept in .text, and the
    addresses right after an instruction that can end a function. A sweep from `at`
    stops where it rejoins instructions already found."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.skipdata = True
    origin = base if at is None else at
    prev = None
    nop_run_from = None
    for addr, size, mnem, ops in md.disasm_lite(code[origin - base:], origin):
        if at is not None and addr != at and addr in starts:
            break
        starts.add(addr)
        if mnem in ("ret", "jmp", "call", "int3"):
            after_end.add(addr + size)
        if mnem == "nop" and size == 1:
            if nop_run_from is None:
                nop_run_from = prev
        else:
            if nop_run_from is not None and addr % 16 == 0:
                padded.add(addr)
            nop_run_from = None
            if mnem == "call" and code[addr - base] == 0xE8:
                calls.add(int(ops, 16))
            elif mnem in ("jmp", "mov", "movzx"):
                t = TABLE_OPERAND.search(ops)
                if t and base <= int(t.group(1), 16) < base + len(code):
                    tables.add(int(t.group(1), 16))
        prev = mnem


def reached_end(md, code: bytes, base: int, lo: int, hi: int, roots: set,
                seen: set | None = None, code_only: bool = False,
                data: set | None = None, callbacks: set | None = None,
                callback: int | None = None) -> int:
    """The end of the furthest instruction or switch table in [lo, hi) that flow from
    `lo` and `roots` reaches: jumps, branches, calls returning, switch tables, and offsets
    taken as operands. A switch table is data, never decoded as code; a byte index table
    runs while its entries index the function's jump tables. A pushed offset is a callback
    argument (atexit), not flow. When testing a particular callback, a register-immediate
    load of that address is not flow either. With `code_only`, tables do not extend the end.
    `seen` collects every instruction reached, `data` the (start, end) of each table."""
    seen = set() if seen is None else seen
    todo, far = [lo, *roots], lo
    jump_tables, index_tables = {}, set()
    while todo:
        at = todo.pop()
        while lo <= at < hi and at not in seen:
            seen.add(at)
            insn = next(md.disasm_lite(code[at - base:at - base + 16], at, 1), None)
            if insn is None:
                break
            _, size, mnem, ops = insn
            far = max(far, at + size)
            t = TABLE_OPERAND.search(ops)
            table = int(t.group(1), 16) if t else None
            for v in re.findall(r"0x[0-9a-f]+", ops) if mnem != "push" else ():
                target = int(v, 16)
                if lo < target < hi and target != table:
                    if mnem == "mov" and REGISTER_IMMEDIATE.fullmatch(ops):
                        if target == callback:
                            continue
                        if callbacks is not None and at < target and target % 16 == 0:
                            callbacks.add((target, at))
                    todo.append(target)
            if t and mnem == "jmp" and lo <= table < hi:
                entry = table
                while entry < hi and lo <= struct.unpack_from("<I", code, entry - base)[0] < hi:
                    todo.append(struct.unpack_from("<I", code, entry - base)[0])
                    entry += 4
                jump_tables[table] = entry
            elif t and lo <= table < hi:
                index_tables.add(table)
            if mnem in ("ret", "jmp", "int3"):
                break
            at += size
    if code_only:
        return far
    spans = set(jump_tables.items())
    cases = max(((e - s) // 4 for s, e in spans), default=0)
    for start in index_tables:
        entry = start
        while entry < hi and code[entry - base] < cases:
            entry += 1
        spans.add((start, entry))
    if data is not None:
        data |= spans
    return max(far, *(e for _, e in spans)) if spans else far


def pointers(pe: pefile.PE, lo: int, hi: int):
    """Every aligned dword in the data sections whose value lies in [lo, hi)."""
    found = set()
    for sec in pe.sections:
        if sec.Name.rstrip(b"\0") not in (b".rdata", b".data"):
            continue
        data = sec.get_data()
        for off in range(0, len(data) - 3, 4):
            v = struct.unpack_from("<I", data, off)[0]
            if lo <= v < hi:
                found.add(v)
    return found


def read_starts(path: Path, kind: str | None = None) -> dict[int, str]:
    if not path.exists():
        return {}
    out = {}
    with path.open() as f:
        for r in csv.DictReader(f):
            if kind and r.get("kind") != kind:
                continue
            out.setdefault(int(r["address"], 16), r.get("name", ""))
    return out


def assembly_bounds(obj, code: bytes, base: int, evidence: dict[int, str],
                    iat: set[int], data_symbols: dict[str, int] | None = None) -> dict[int, int]:
    """Locate archived MASM procedures independently of the addresses in source annotations."""
    from coff import REL_DIR32, REL_REL32, STORAGE_EXTERNAL
    from libmatch import MIN_SIZE, pattern

    def unique(data: bytes, masked: set[int]) -> int | None:
        if len(data) - len(masked) < MIN_SIZE:
            return None
        regex = re.compile(pattern(data, masked), re.DOTALL)
        first = regex.search(code)
        if first and not regex.search(code, first.start() + 1):
            return base + first.start()
        return None

    def import_jump(addr: int) -> bool:
        offset = addr - base
        return (0 <= offset <= len(code) - 6 and code[offset:offset + 2] == b"\xff\x25"
                and struct.unpack_from("<I", code, offset + 2)[0] in iat)

    def bound_data(sec, lo: int, hi: int, at: int) -> bool:
        """Every relocation binds independently named external data, at two locations."""
        targets = {}
        for rel in sec.relocs:
            if not lo <= rel.offset < hi:
                continue
            sym = obj.by_index[rel.symbol]
            if (rel.type != REL_DIR32 or rel.offset + 4 > hi or sym.section != 0
                    or sym.storage != STORAGE_EXTERNAL
                    or sym.type == 0x20 or not data_symbols or sym.name not in data_symbols):
                return False
            target = data_symbols[sym.name]
            addend = struct.unpack_from("<i", sec.data, rel.offset)[0]
            linked = struct.unpack_from("<I", code, at - base + rel.offset - lo)[0]
            if linked != (target + addend) & 0xFFFFFFFF:
                return False
            targets[sym.name] = target
        return len(targets) >= 2 and len(set(targets.values())) >= 2

    anchors = {a for a, flags in evidence.items() if set(flags) - {"P", "F", "A"}}
    stops = {}
    for sec in obj.sections:
        if not sec.is_code or any(r.type not in (REL_DIR32, REL_REL32) for r in sec.relocs):
            continue
        procs = [(lo, hi) for sym, section, lo, hi in obj.functions() if section is sec
                 and sym.type == 0x20 and sym.storage == STORAGE_EXTERNAL and lo < hi]
        if not procs:
            continue
        masked = {r.offset + k for r in sec.relocs for k in range(4)}
        addr = unique(sec.data, masked)
        # An independent entry in the image corroborates the section's placement.
        if addr is not None and any(addr + lo in anchors for lo, _ in procs):
            for lo, hi in procs:
                start, stop = addr + lo, addr + hi
                interior = [a for a in evidence if start < a < stop]
                if any(set(evidence[a]) - {"P", "F"} for a in interior):
                    continue
                for a in interior:
                    del evidence[a]
                evidence.setdefault(start, "A")
                stops[start] = stop
        else:
            # A changed procedure can divide an otherwise unchanged archive section.
            # Preserve COFF order and spacing across at least two complete public
            # procedures. Either an existing entry anchors the run, or every body
            # binds all references to independently named non-code data locations.
            runs, run = [], []
            for lo, hi in procs:
                at = unique(sec.data[lo:hi], {k - lo for k in masked if lo <= k < hi})
                if (at is None or (run and
                        (run[-1][1] != lo or run[-1][2] + run[-1][1] - run[-1][0] != at))):
                    runs.append(run)
                    run = []
                if at is not None:
                    run.append((lo, hi, at))
            runs.append(run)
            for run in runs:
                if len(run) < 2:
                    continue
                if not (any(at in anchors for _, _, at in run)
                        or all(bound_data(sec, lo, hi, at) for lo, hi, at in run)):
                    continue
                for lo, hi, at in run:
                    stop = at + hi - lo
                    interior = [a for a in evidence if at < a < stop]
                    if any(set(evidence[a]) - {"P", "F"} for a in interior):
                        continue
                    for a in interior:
                        del evidence[a]
                    evidence.setdefault(at, "A")
                    stops[at] = stop
            # A split section can still end at a run of linker import thunks.
            lo, hi = procs[-1]
            if hi != len(sec.data):
                continue
            at = unique(sec.data[lo:hi], {k - lo for k in masked if lo <= k < hi})
            if at in anchors:
                stop = at + hi - lo
                if import_jump(stop) and import_jump(stop + 6):
                    stops[at] = stop
    return stops


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("exe", nargs="?", type=Path, default=ROOT / "orig/gamemd.exe")
    parser.add_argument("out", nargs="?", type=Path, default=ROOT / "data/functions.csv")
    parser.add_argument("--no-asm", action="store_true", help="omit MASM evidence in a separate map")
    args = parser.parse_args()
    exe, out = args.exe, args.out
    if args.no_asm and out.resolve() == (ROOT / "data/functions.csv").resolve():
        parser.error("--no-asm requires a separate output path")
    data = exe.read_bytes()
    build = build_of(data)
    pe = pefile.PE(data=data)
    text = pe.sections[0]
    base = pe.OPTIONAL_HEADER.ImageBase + text.VirtualAddress
    code = bytearray(text.get_data()[: text.Misc_VirtualSize])
    end = base + len(code)
    # Post-link patches leave stubs and nop runs that look like function boundaries;
    # the map is of the compiler's code, so the retail bytes are put back first.
    restore(code, base, build)
    code = bytes(code)

    starts, padded, calls, tables, after_end = set(), set(), set(), set(), set()
    sweep(code, base, starts, padded, calls, tables, after_end)
    ptrs = pointers(pe, base, end)
    yrpp = read_starts(ROOT / "data/symbols.csv")
    ghidra = read_starts(ROOT / "build/ghidra/functions.csv")
    library = {}
    if (ROOT / "data/library.csv").exists():
        with (ROOT / "data/library.csv").open() as f:
            library = {int(r["address"], 16): r["symbol"] for r in csv.DictReader(f)}

    # A switch table stored in .text throws the sweep out of step until it happens to
    # resynchronise. An aligned start that other evidence names is taken as a boundary
    # and swept from again when padding precedes it, or when it lies past a switch
    # table of the function before it.
    named = {a for s_ in (calls, yrpp, ghidra, library) for a in s_}
    while True:
        known = sorted(padded | (named & starts))
        table_list = sorted(tables)

        def after_table(a: int) -> bool:
            i = bisect.bisect_left(table_list, a) - 1
            j = bisect.bisect_left(known, a) - 1
            return i >= 0 and (j < 0 or table_list[i] >= known[j])

        def table_ends_at(a: int) -> bool:
            lo = known[bisect.bisect_left(known, a) - 1] if known and known[0] < a else base
            in_fn = lambda at: lo <= int.from_bytes(code[at - base:at - base + 4], "little") < a
            index = lambda at: all(x < 0x40 for x in code[at - base:at - base + 4])
            return (in_fn(a - 4) or index(a - 4)) and not (in_fn(a) or index(a))

        resync = [a for a in named | {p for p in ptrs if p % 16 == 0}
                  if base < a < end and a not in starts and a % 16 == 0
                  and (code[a - 1 - base] == NOP or a in library
                       or (after_table(a) and (a in named or table_ends_at(a))))]
        if not resync:
            break
        for a in resync:
            sweep(code, base, starts, padded, calls, tables, after_end, a)

    known = sorted(padded)
    table_list = sorted(tables)

    def after_table(a: int) -> bool:
        i = bisect.bisect_left(table_list, a) - 1
        j = bisect.bisect_left(known, a) - 1
        return i >= 0 and (j < 0 or table_list[i] >= known[j])

    evidence: dict[int, str] = {}
    for letter, addrs in (("P", padded), ("C", calls), ("V", ptrs), ("Y", yrpp), ("G", ghidra),
                          ("L", library)):
        for a in addrs:
            if base <= a < end and a in starts and (a not in tables or letter == "C"):
                # A pointer into the middle of game code is a jump-table entry or a
                # stray constant, not a function, unless something else agrees.
                if letter == "V" and a % 16 and a not in calls:
                    continue
                evidence[a] = evidence.get(a, "") + letter
    # Without padding, a function can only begin where the code before it cannot run
    # on into it: after a return, jump, no-return call or switch table. A direct call
    # target is a function start wherever it lies.
    mid = {a: e for a, e in evidence.items()
           if "P" not in e and "C" not in e and "L" not in e and a != base
           and a not in after_end and not after_table(a)}
    for a in mid:
        del evidence[a]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

    def labels(after: set | None) -> list[int]:
        """Starts that only a data pointer, YRpp or Ghidra names, which flow from the start
        before them reaches: labels or tails inside that function. `after` limits the
        check to starts whose previous start is in it."""
        ordered = sorted(evidence)
        found = []
        for i, a in enumerate(ordered[1:-1], 1):
            if set(evidence[a]) <= {"V", "Y", "G"} and (after is None or ordered[i - 1] in after):
                reached: set = set()
                reached_end(md, code, base, ordered[i - 1], ordered[i + 1], set(), reached, True)
                if a in reached:
                    found.append(a)
        return found

    case_labels = labels(None)
    for a in case_labels:
        del evidence[a]
    first = base
    if first not in evidence:
        evidence[first] = "P"

    callbacks = set()

    def flow_end(a: int, nxt: int, callback: int | None = None) -> tuple[int, bool]:
        # Data pointers to instructions before a candidate (unwind funclets) extend the
        # flow; those past it point into the second function, those into a table at nothing.
        roots, data = set(), set()
        while True:
            at = reached_end(md, code, base, a, nxt, roots, data=data,
                             callbacks=callbacks if callback is None else None, callback=callback)
            after_data = any(e == at for _, e in data)
            while at < nxt and code[at - base] == NOP:
                at += 1
            more = {p for p in ptrs if a < p < at and p in starts
                    and not any(s <= p < e for s, e in data)} - roots
            if not more:
                return at, after_data
            roots |= more

    # Aligned code past a function's last return or switch table that its flow never
    # reaches is a second function, even where the linear sweep ran through it.
    boundaries = after_end | padded
    pending = sorted(evidence)
    while pending:
        new = []
        ordered = sorted(evidence)
        for a in pending:
            i = bisect.bisect_right(ordered, a)
            nxt = ordered[i] if i < len(ordered) else end
            at, after_data = flow_end(a, nxt)
            if (a < at < nxt and at % 16 == 0 and at not in tables
                    and ((at in starts and at in boundaries) or after_data)):
                if at not in starts:
                    sweep(code, base, starts, set(), set(), set(), set(), at)
                evidence[at] = "F"
                new.append(at)
        # Flow from a new start can reach a label that the start before it did not.
        dropped = labels(set(new))
        for a in dropped:
            del evidence[a]
            new.append(max(p for p in evidence if p < a))
        case_labels += dropped
        pending = new

    # Look for callbacks inside the remaining intervals. Existing boundaries are now
    # fixed. Ignore only the immediate load of the proposed entry: a branch or table
    # reaching that address still keeps it inside the preceding function.
    ordered = sorted(evidence)
    for target, site in sorted(callbacks):
        if target in evidence or target in tables or target not in starts or target not in boundaries:
            continue
        i = bisect.bisect_left(ordered, target)
        lo = ordered[i - 1]
        if site < lo:
            continue
        hi = ordered[i] if i < len(ordered) else end
        at, _ = flow_end(lo, hi, callback=target)
        if at == target:
            evidence[target] = "F"
            ordered.insert(i, target)

    sources = sorted((ROOT / "src").rglob("*.asm"))
    stops = {}
    if sources and not args.no_asm:
        from check import assemble_source
        from assembly_anchors import verified_data_symbols
        from coff import CoffObject
        iat = {imp.address for dll in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []) for imp in dll.imports}
        (ROOT / "build").mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=ROOT / "build") as tmp:
            data_symbols = verified_data_symbols(exe, Path(tmp))
            for src in sources:
                obj, log = assemble_source(src, Path(tmp) / "archive.obj")
                if obj is None:
                    raise SystemExit(f"Cannot assemble {src.name} for function discovery: {log}")
                stops.update(assembly_bounds(CoffObject(obj.read_bytes()), code, base, evidence,
                                             iat, data_symbols))
        ordered = sorted(evidence)

    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "evidence", "name"])
        for i, a in enumerate(ordered):
            nxt = ordered[i + 1] if i + 1 < len(ordered) else end
            stop = min(nxt, stops.get(a, nxt))
            while stop > a + 1 and code[stop - 1 - base] in PADDING:
                stop -= 1
            w.writerow([f"0x{a:08X}", stop - a, evidence[a], yrpp.get(a) or library.get(a, "")])

    print(f"{len(ordered)} functions -> {out}")
    print("labels named only by a data pointer, YRpp or Ghidra, dropped:", " ".join(f"0x{a:08X}" for a in case_labels))
    by = {}
    for a, e in mid.items():
        by[e] = by.get(e, 0) + 1
    print("starts inside a function's flow, dropped:", by)
    for a, e in sorted(mid.items()):
        if "Y" in e:
            print(f"  0x{a:08X} {yrpp[a]}")
    for k, s_ in (("C", calls), ("Y", yrpp), ("G", ghidra)):
        bad = sorted(a for a in s_ if base <= a < end and a not in starts)
        print(f"{k} starts inside another instruction, dropped: {len(bad)}")
        if k == "Y":
            for a in bad:
                print(f"  0x{a:08X} {yrpp[a]}")


if __name__ == "__main__":
    main()
