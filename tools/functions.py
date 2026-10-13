"""Build the function map of gamemd.exe: data/functions.csv.

Usage: uv run tools/functions.py [EXE] [OUT_CSV]

A function start needs at least one piece of evidence, recorded as letters:
  P  the alignment padding the compiler puts between functions ends here
  C  a direct call targets it
  V  a pointer in .rdata or .data (vtables, callbacks) targets it
  Y  YRpp names it (data/symbols.csv)
  G  Ghidra's analysis found it (build/ghidra/functions.csv, when present)
  L  a prebuilt library function was found here (data/library.csv)
  F  code after a function's return or switch tables that no path through that function
     reaches, at an aligned address: a function nothing calls directly, placed with no
     padding before it
Every start must also be an instruction boundary of a linear sweep of .text. A
function's size runs to the next start, less the padding (nop or int3) before it.
"""

import bisect
import csv
import re
import struct
import sys
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parent.parent
NOP = 0x90
PADDING = (NOP, 0xCC)
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
                data_ends: set | None = None) -> int:
    """The end of the furthest instruction or switch table in [lo, hi) that flow from
    `lo` and `roots` reaches: jumps, branches, calls returning, switch tables, and offsets
    taken as operands. A switch table is data, never decoded as code; a byte index table
    runs while its entries index the function's jump tables. A pushed offset is a callback
    argument (atexit), not flow. With `code_only`, tables do not extend the end. `seen`
    collects every instruction reached, `data_ends` the end of each table."""
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
                if lo < int(v, 16) < hi and int(v, 16) != table:
                    todo.append(int(v, 16))
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
    ends = set(jump_tables.values())
    cases = max(((e - s) // 4 for s, e in jump_tables.items()), default=0)
    for entry in index_tables:
        while entry < hi and code[entry - base] < cases:
            entry += 1
        ends.add(entry)
    if data_ends is not None:
        data_ends |= ends
    return max(far, *ends) if ends else far


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


def main() -> None:
    exe = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "orig/gamemd.exe"
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "data/functions.csv"
    pe = pefile.PE(str(exe))
    text = pe.sections[0]
    base = pe.OPTIONAL_HEADER.ImageBase + text.VirtualAddress
    code = bytearray(text.get_data()[: text.Misc_VirtualSize])
    end = base + len(code)
    # EA's post-link patches leave stubs and nop runs that look like function boundaries;
    # the map is of the compiler's code, so their original bytes are put back first.
    with (ROOT / "data/exe_patches.csv").open() as f:
        for r in csv.DictReader(f):
            at = int(r["address"], 16)
            if base <= at < end:
                orig = bytes.fromhex(r["original"])
                code[at - base:at - base + len(orig)] = orig
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
            return i >= 0 and (j < 0 or table_list[i] > known[j])

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
        return i >= 0 and (j < 0 or table_list[i] > known[j])

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
    # A start that only a data pointer or Ghidra names, which flow from the function
    # before it reaches, is a label or the tail after a call inside that function.
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    ordered = sorted(evidence)
    case_labels = []
    for i, a in enumerate(ordered[1:-1], 1):
        if set(evidence[a]) <= {"V", "G"}:
            reached: set = set()
            reached_end(md, code, base, ordered[i - 1], ordered[i + 1], set(), reached, True)
            if a in reached:
                case_labels.append(a)
    for a in case_labels:
        del evidence[a]
    first = base
    if first not in evidence:
        evidence[first] = "P"

    # A function whose flow ends before the next start, with code after its last return
    # or switch table, holds a second function there when that code is aligned.
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    boundaries = after_end | padded
    pending = sorted(evidence)
    while pending:
        new = []
        ordered = sorted(evidence)
        for a in pending:
            i = bisect.bisect_right(ordered, a)
            nxt = ordered[i] if i < len(ordered) else end
            # Data pointers into the code before a candidate (unwind funclets) extend the
            # flow; those past it point into the second function.
            roots, data_ends = set(), set()
            while True:
                at = reached_end(md, code, base, a, nxt, roots, data_ends=data_ends)
                after_data = at in data_ends
                while at < nxt and code[at - base] == NOP:
                    at += 1
                more = {p for p in ptrs if a < p < at} - roots
                if not more:
                    break
                roots |= more
            if (a < at < nxt and at % 16 == 0 and at in starts and at not in tables
                    and (at in boundaries or after_data)):
                evidence[at] = "F"
                new.append(at)
        pending = new

    ordered = sorted(evidence)
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "evidence", "name"])
        for i, a in enumerate(ordered):
            nxt = ordered[i + 1] if i + 1 < len(ordered) else end
            stop = nxt
            while stop > a + 1 and code[stop - 1 - base] in PADDING:
                stop -= 1
            w.writerow([f"0x{a:08X}", stop - a, evidence[a], yrpp.get(a) or library.get(a, "")])

    print(f"{len(ordered)} functions -> {out}")
    print("labels named only by a data pointer or Ghidra, dropped:", " ".join(f"0x{a:08X}" for a in case_labels))
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
