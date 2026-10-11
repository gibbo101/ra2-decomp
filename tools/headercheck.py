"""Check that our header gives a class exactly Yuri's Revenge's layout and virtual table.

    uv run tools/headercheck.py AbstractTypeClass
    uv run tools/headercheck.py ObjectClass --overlay build/spike/0x005F3E70/include

The headers are check.py's: the TibSun archive's, include/ over them, then the overlay.
The original compiler answers every question about our side, through a probe file that
includes the class's header; the exe and data/layouts.csv (YRpp) are the authority.

  1. Size: sizeof(Class) equals YRpp's <size> row.
  2. Members: each data member the class declares directly (static members, functions,
     typedefs, enums and bitfields are skipped) is placed by offsetof and matched with
     YRpp's members by offset, not name. A YRpp member must start where one of ours starts;
     one of ours must start where a YRpp member starts. A YRpp member lying wholly inside
     an array or class-type member of ours (a string's terminator byte, say) is a note.
  3. Virtual table: a probe class derived from Class that overrides nothing makes the
     compiler emit Class's slots. They are compared with the exe's primary vtable, found
     through RTTI: the slot count, whether each slot is inherited from the primary base
     (in the exe: the same function as the base's slot), and the slot's address wherever
     data/progress.csv, data/names.csv or data/symbols.csv knows our symbol's. A pure
     slot must hold the exe's _purecall, the most common vtable entry (the linker folded
     every `return 0` function into it). Secondary vtables compare slot counts only.

Exits 0 only when every check passes.
"""

import collections
import csv
import hashlib
import os
import re
import shutil
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from check import ROOT, Original, known_decorated, prepare_headers, qualified, run_compiler, winpath  # noqa: E402
from coff import CoffObject  # noqa: E402

PROBE = "HeaderProbe"
UNINITIALIZED = 0x80
PADDING_NAME = re.compile(r"^(padding|align|pad)", re.I)
SCALARS = {"bool", "char", "short", "int", "long", "float", "double", "unsigned", "signed",
           "__int64", "wchar_t", "BYTE", "WORD", "DWORD", "LONG", "BOOL", "UINT", "ULONG",
           "HRESULT", "size_t"}


# --- our side: the header, parsed and measured by the compiler ---------------------------

def find_header(headers: Path, cls: str) -> str:
    """The header (relative to `headers`) that defines `cls`."""
    pattern = re.compile(rf"^\s*(?:class|struct)\s+{cls}\s*(?::[^;{{]*)?\{{", re.M)
    found = sorted(h for h in headers.rglob("*.h") if pattern.search(h.read_text(errors="replace")))
    if not found:
        raise SystemExit(f"{cls}: no header defines it")
    return found[0].relative_to(headers).as_posix()


def preamble(header: str) -> str:
    return ("#define private public\n#define protected public\n"
            f'#include "always.h"\n#include "{header}"\n')


def class_body(text: str, cls: str) -> str:
    """The text between the braces of `cls`'s definition."""
    m = re.search(rf"\b(?:class|struct)\s+{cls}\s*(?::[^;{{]*)?\{{", text)
    if not m:
        raise SystemExit(f"{cls}: definition not found in the preprocessed header")
    return text[m.end():matching(text, m.end() - 1)]


def matching(text: str, at: int) -> int:
    """Index of the brace closing the one at `at`."""
    depth = 0
    for i in range(at, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return i
    raise SystemExit("unbalanced braces in the preprocessed header")


def statements(body: str) -> list[str]:
    """The class body's declarations, with function bodies dropped and labels removed."""
    out, cur, parens, i = [], "", 0, 0
    while i < len(body):
        c = body[i]
        if c == "{" and parens == 0:
            end = matching(body, i)
            if "(" in cur:
                out.append(cur + "{}")
                cur = ""
            else:
                cur += body[i:end + 1]
            i = end + 1
            continue
        if c == "(":
            parens += 1
        elif c == ")":
            parens -= 1
        if c == ";" and parens == 0:
            out.append(cur)
            cur = ""
        elif c == ":" and cur.strip() in ("public", "protected", "private") and body[i + 1] != ":":
            cur = ""
        else:
            cur += c
        i += 1
    return [" ".join(s.split()) for s in out if s.strip()]


def split_top(text: str) -> list[str]:
    """`text` split at the commas outside any brackets."""
    parts, cur, depth = [], "", 0
    for c in text:
        depth += c in "([{<"
        depth -= c in ")]}>"
        if c == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += c
    return parts + [cur]


def data_members(body: str) -> tuple[list[tuple[str, bool]], list[str]]:
    """(name, could hold a YRpp member inside it) for each data member, and skipped bitfields."""
    members, bitfields = [], []
    for s in statements(body):
        s = re.sub(r"^(mutable|volatile)\s+", "", s)
        first = s.split()[0]
        if first in ("static", "typedef", "enum", "friend", "using", "template", "virtual"):
            continue
        if "__declspec" in s and "property" in s:
            continue
        if re.fullmatch(r"(class|struct|union)\s+\w+(\s*\{.*\})?", s):
            continue
        if "(" in s:
            fp = re.match(r"[^(]*\(\s*(?:__\w+\s+)?(?:\w+\s*::\s*)*\*\s*(\w+)\s*\)\s*\(", s)
            if fp:
                members.append((fp.group(1), False))
            continue
        s = re.sub(r"\{.*\}", "", s)
        if ":" in s.replace("::", ""):
            bitfields.append(s)
            continue
        pieces = split_top(s)
        base_type = re.sub(r"\w+\s*(\[[^\]]*\]\s*)*$", "", pieces[0]).strip()
        aggregate = base_type.split()[-1] not in SCALARS if base_type else False
        for piece in pieces:
            m = re.search(r"(\w+)\s*((?:\[[^\]]*\]\s*)*)$", piece.strip())
            if m:
                pointer = "*" in piece or "&" in piece
                members.append((m.group(1), bool(m.group(2)) or (aggregate and not pointer)))
    return members, bitfields


def noinit_ctor(body: str, cls: str) -> bool:
    return re.search(rf"\b{cls}\s*\(\s*(?:const\s+)?NoInitClass\b", body) is not None


def probe_definitions(cls: str, members: list[tuple[str, bool]], noinit: bool) -> str:
    # offsetof that a member's own operator& cannot intercept.
    lines = ["#define PROBE_OFFSET(c, m) ((unsigned int)&reinterpret_cast<char const volatile &>(((c *)0)->m))",
             f'extern "C" unsigned int probe_size = sizeof({cls});']
    for k, (name, _) in enumerate(members):
        lines.append(f'extern "C" unsigned int probe_{k}_at = PROBE_OFFSET({cls}, {name});')
        lines.append(f'extern "C" unsigned int probe_{k}_size = sizeof((({cls} *)0)->{name});')
    init = f"{cls}(NoInitClass())" if noinit else f"{cls}()"
    lines.append(f"class {PROBE} : public {cls} {{ public: {PROBE}(); }};")
    lines.append(f"{PROBE}::{PROBE}() : {init} {{}}")
    return "\n".join(lines) + "\n"


def compile_probe(cls: str, overlay: Path | None, work: Path):
    """Our side: (size, [(name, offset, size, aggregate)], bitfields, {vtable offset: [symbol]})."""
    headers = prepare_headers(overlay)
    try:
        header = find_header(headers, cls)
        src = work / "probe.cpp"
        src.write_text(preamble(header))
        proc = run_compiler(headers, "/E", winpath(src))
        if proc.returncode != 0:
            raise SystemExit(proc.stdout + proc.stderr)
        body = class_body(proc.stdout, cls)
        members, bitfields = data_members(body)
        src.write_text(preamble(header) + probe_definitions(cls, members, noinit_ctor(body, cls)))
        out = work / "probe.obj"
        proc = run_compiler(headers, f"/Fo{winpath(out)}", winpath(src))
        if proc.returncode != 0 or not out.exists():
            raise SystemExit(proc.stdout + proc.stderr)
    finally:
        shutil.rmtree(headers, ignore_errors=True)
    obj = CoffObject(out.read_bytes())
    # A value the compiler could not fold is stored by code at startup instead.
    written = {r.symbol for sec in obj.sections if sec.is_code for r in sec.relocs}
    values = {}
    for s in obj.symbols:
        if s.name.startswith("_probe_") and s.section > 0:
            sec = obj.sections[s.section - 1]
            if s.index in written:
                raise SystemExit(f"{s.name[1:]}: the compiler did not fold it to a constant")
            values[s.name[1:]] = 0 if sec.flags & UNINITIALIZED else struct.unpack_from("<I", sec.data, s.value)[0]
    placed = [(name, values[f"probe_{k}_at"], values[f"probe_{k}_size"], agg)
              for k, (name, agg) in enumerate(members)]
    return values["probe_size"], placed, bitfields, our_vtables(obj, cls)


def our_vtables(obj: CoffObject, cls: str) -> dict[int, list[str]]:
    """The probe class's vtables by their offset in the object, as slot symbols."""
    out = {}
    for s in obj.symbols:
        if not s.name.startswith(f"??_7{PROBE}@@6B") or s.section <= 0:
            continue
        sec = obj.sections[s.section - 1]
        relocs = {r.offset: obj.by_index[r.symbol].name for r in sec.relocs}
        locator = next(x for x in obj.symbols if x.name == relocs[s.value - 4] and x.section > 0)
        offset = struct.unpack_from("<I", obj.sections[locator.section - 1].data, locator.value + 4)[0]
        slots = [relocs[o] for o in range(s.value, len(sec.data), 4)]
        out[offset] = [sym.replace(f"{PROBE}@@", f"{cls}@@") for sym in slots]
    return out


# --- the exe's side ---------------------------------------------------------------------

class Exe:
    def __init__(self):
        self.orig = Original()
        pe = self.orig.pe
        text = next(s for s in pe.sections if s.Name.startswith(b".text"))
        rdata = next(s for s in pe.sections if s.Name.startswith(b".rdata"))
        self.text = range(self.orig.base + text.VirtualAddress,
                          self.orig.base + text.VirtualAddress + text.Misc_VirtualSize)
        self.rdata = (self.orig.base + rdata.VirtualAddress, rdata.get_data())

    def vtables(self, cls: str) -> dict[int, list[int]]:
        """The slots of each of `cls`'s vtables by its offset in the object, through RTTI."""
        image, base = self.orig.image, self.orig.base
        at = max(image.find(b".?AV" + cls.encode() + b"@@\0"), image.find(b".?AU" + cls.encode() + b"@@\0"))
        if at < 0:
            return {}
        r0, data = self.rdata
        out = {}
        for col in occurrences(data, struct.pack("<I", base + at - 8)):
            signature, offset = struct.unpack_from("<II", data, col - 12)
            if signature != 0:
                continue
            for holder in occurrences(data, struct.pack("<I", r0 + col - 12)):
                out[offset] = self.slots(r0 + holder + 4)
        return out

    def slots(self, vtable: int) -> list[int]:
        """Entries from `vtable` up to the first that is not code."""
        out = []
        while self.orig.u32(vtable + 4 * len(out)) in self.text:
            out.append(self.orig.u32(vtable + 4 * len(out)))
        return out

    def purecall(self) -> int:
        """The function most vtable entries point at: _purecall, which returns 0 here."""
        _, data = self.rdata
        counts = collections.Counter(v for (v,) in struct.iter_unpack("<I", data[:len(data) // 4 * 4])
                                     if v in self.text)
        return counts.most_common(1)[0][0]


def occurrences(data: bytes, needle: bytes):
    """Dword-aligned offsets of `needle` in `data`."""
    at = data.find(needle)
    while at >= 0:
        if at % 4 == 0:
            yield at
        at = data.find(needle, at + 1)


# --- YRpp's side --------------------------------------------------------------------------

def yrpp_layout(cls: str):
    """(size, primary base, [(member, offset, span, bitfield)], [(start, end) of padding])
    from data/layouts.csv."""
    rows = [r for r in csv.DictReader((ROOT / "data/layouts.csv").open()) if r["class"] == cls]
    if not rows:
        raise SystemExit(f"{cls}: not in data/layouts.csv")
    size = int(rows[0]["size"])
    base = next((r["type"] for r in rows if r["member"] == "<primary base>"), None)
    fields = [r for r in rows if not r["member"].startswith("<")]
    starts = sorted({int(r["offset"]) for r in fields} | {size})
    members, padding = [], []
    for r in fields:
        at = int(r["offset"])
        span = next(s for s in starts if s > at) - at
        if PADDING_NAME.match(r["member"]):
            padding.append((at, at + span))
        else:
            members.append((r["member"], at, span, bool(r["bits"])))
    return size, base, members, padding


# --- the checks ---------------------------------------------------------------------------

def check_members(ours, yrpp, padding_spans) -> int:
    print(f"{'offset':>8}  {'ours':<28} {'YRpp':<28} verdict")
    problems = 0
    offsets = sorted({o for _, o, _, _ in ours} | {o for _, o, _, _ in yrpp})
    for at in offsets:
        mine = [m for m in ours if m[1] == at]
        theirs = [m for m in yrpp if m[1] == at]
        if mine and theirs:
            verdict = "ok"
        elif mine:
            inside = [y for y in yrpp if y[1] < at < y[1] + y[2]]
            if inside:
                verdict = f"MISMATCH: inside YRpp {inside[0][0]}"
            elif any(p[0] <= at < p[1] for p in padding_spans):
                verdict = "MISMATCH: in YRpp's padding"
            else:
                verdict = "MISMATCH: YRpp has no member here"
        else:
            holder = [m for m in ours if m[1] < at < m[1] + m[2]]
            if all(t[3] for t in theirs):
                verdict = "note: bitfield, not checked"
            elif holder and holder[0][3] and all(at + t[2] <= holder[0][1] + holder[0][2] for t in theirs):
                verdict = f"note: inside our {holder[0][0]}"
            elif holder:
                verdict = f"MISMATCH: inside our {holder[0][0]}"
            else:
                verdict = "MISMATCH: we have no member here"
        problems += verdict.startswith("MISMATCH")
        print(f"{at:>#8x}  {', '.join(m[0] for m in mine) or '-':<28} "
              f"{', '.join(t[0] for t in theirs) or '-':<28} {verdict}")
    return problems


def scope_of(sym: str) -> str:
    """The class a decorated member symbol belongs to ('' for a free function)."""
    if sym.startswith(("??_E", "??_G")):
        return sym[4:].split("@", 1)[0]
    if sym.startswith("??"):
        return sym[3:].split("@", 1)[0]
    if sym.startswith("?"):
        parts = sym[1:].split("@@", 1)[0].split("@")
        return parts[1] if len(parts) > 1 else ""
    return ""


def pretty(sym: str) -> str:
    if sym == "__purecall":
        return "(pure)"
    if sym.startswith(("??_E", "??_G")):
        return f"{scope_of(sym)}::~{scope_of(sym)} (deleting)"
    return qualified(sym)


def address_book():
    """Decorated symbol -> address, and unambiguous qualified name -> address."""
    named = collections.defaultdict(set)
    yrpp = collections.defaultdict(list)
    with (ROOT / "data/symbols.csv").open() as f:
        for r in csv.DictReader(f):
            named[r["name"]].add(int(r["address"], 16))
            yrpp[int(r["address"], 16)].append(r["name"])
    unique = {n: next(iter(a)) for n, a in named.items() if len(a) == 1}
    return known_decorated(), unique, yrpp


def check_vtable(cls: str, ours: list[str], theirs: list[int], parent: list[int] | None,
                 purecall: int) -> int:
    decorated, unique, yrpp = address_book()
    print(f"{'slot':>4} {'offset':>6}  {'ours':<44} {'exe':<10} {'YRpp':<32} verdict")
    problems = 0
    for k in range(max(len(ours), len(theirs))):
        sym = ours[k] if k < len(ours) else ""
        addr = theirs[k] if k < len(theirs) else None
        verdict = slot_verdict(cls, k, sym, addr, parent, purecall, decorated, unique)
        problems += verdict != "ok"
        names = ", ".join(yrpp.get(addr, [])) if addr else ""
        shown = f"0x{addr:08X}" if addr else "-"
        print(f"{k:>4} {4 * k:>#6x}  {pretty(sym) if sym else '-':<44} {shown:<10} {names:<32} {verdict}")
    return problems


def slot_verdict(cls, k, sym, addr, parent, purecall, decorated, unique) -> str:
    if not sym:
        return "MISMATCH: we have no slot"
    if addr is None:
        return "MISMATCH: the exe has no slot"
    if sym == "__purecall":
        return "ok" if addr == purecall else "MISMATCH: pure in ours, not in the exe"
    known = decorated.get(sym, unique.get(qualified(sym)))
    if known is not None and known != addr:
        return f"MISMATCH: ours is at 0x{known:08X}"
    exe_inherited = parent is not None and k < len(parent) and parent[k] == addr
    our_inherited = scope_of(sym) != cls
    if exe_inherited != our_inherited:
        return "MISMATCH: " + ("inherited in the exe, overridden in ours" if exe_inherited
                               else "overridden in the exe, inherited in ours")
    return "ok"


def check_vtables(cls: str, parent_name: str | None, ours: dict[int, list[str]]) -> int:
    exe = Exe()
    theirs = exe.vtables(cls)
    if not theirs and not ours:
        print("\nno virtual table in ours or the exe")
        return 0
    parent = exe.vtables(parent_name).get(0) if parent_name else None
    print(f"\nprimary vtable: ours {len(ours.get(0, []))} slots, exe {len(theirs.get(0, []))}; "
          f"{parent_name}'s: {len(parent) if parent else 'none in the exe'}")
    problems = check_vtable(cls, ours.get(0, []), theirs.get(0, []), parent, exe.purecall())
    for offset in sorted((set(ours) | set(theirs)) - {0}):
        n_ours, n_exe = len(ours.get(offset, [])), len(theirs.get(offset, []))
        problems += n_ours != n_exe
        print(f"vtable at +0x{offset:X}: ours {n_ours} slots, exe {n_exe}"
              + ("" if n_ours == n_exe else "  MISMATCH"))
    return problems


def main() -> None:
    argv = sys.argv[1:]
    overlay = None
    if "--overlay" in argv:
        k = argv.index("--overlay")
        overlay = Path(argv[k + 1]).resolve()
        del argv[k:k + 2]
    if len(argv) != 1:
        raise SystemExit(__doc__)
    cls = argv[0]
    work = ROOT / "build/headercheck" / hashlib.sha256(f"{os.getpid()} {cls}".encode()).hexdigest()[:8]
    work.mkdir(parents=True, exist_ok=True)
    try:
        size, members, bitfields, vtables = compile_probe(cls, overlay, work)
    finally:
        shutil.rmtree(work, ignore_errors=True)
    ysize, ybase, ymembers, padding = yrpp_layout(cls)
    problems = 0

    print(f"size: ours {size} (0x{size:X}), YRpp {ysize} (0x{ysize:X})"
          + ("" if size == ysize else "  MISMATCH"))
    problems += size != ysize

    print(f"\nmembers of {cls}:")
    problems += check_members(members, ymembers, padding)
    for b in bitfields:
        print(f"  not checked (bitfield): {b}")

    problems += check_vtables(cls, ybase, vtables)
    print(f"\n{cls}: PASS" if not problems else f"\n{cls}: FAIL ({problems} problems)")
    raise SystemExit(1 if problems else 0)


if __name__ == "__main__":
    main()
