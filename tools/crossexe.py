"""Search RA2 1.006, TS 2.03 and YR for out-of-line copies of functions compiled from our source.

Usage: uv run tools/crossexe.py SRC ... [--only NAME ...] [--extern] [--sweep] [--overlay DIR]
           [--exe game ts gamemd] [--max-hits 8] [--asm] [--near [--floor F]] [--twins] [--keep]
       uv run tools/crossexe.py --bytes HEX ...
       uv run tools/crossexe.py --twin-of ADDR ... | --dis EXE:ADDR[:N] ...

Each SRC is compiled with check.py's compiler and fixed flags into build/crossexe/. Every
function whose decorated or undecorated name contains one of the --only strings (default:
all of them) becomes a pattern, searched for in the .text of orig/game.exe (RA2 1.006),
orig/ts-game.exe (TS 2.03) and orig/gamemd.exe (YR 1.001, its build's patches put back).
A helper YR inlines everywhere is often out of line in the older exes, where its callers
name the function it belongs to.

Three pattern strengths, each a superset of the one before:
  exact   relocated fields (calls, globals) are wildcards, every other byte must agree;
  disp    memory-operand displacements are wildcards too (field offsets move between games);
  loose   immediates are wildcards too (sizes, counts, enum values move as well).
Each hit lists its address, its strongest strength, whether it starts a function (in YR from
data/functions.csv; elsewhere 16-byte aligned after padding or a return), the rel32 calls and
jumps that reach it and the absolute references to it (vtables, callback tables, imm32
operands), each with the function holding the referrer. game.exe addresses are named from
reference/redalert2-decomp's recovered files and reference/ra2-help's 1006_check.csv.

  --extern    compile a copy in which top-level `static` and `inline` functions are plain
              extern, so the object holds helpers the compiler would otherwise inline;
  --sweep     --extern, searching every function defined without a checker marker (unless
              --only names some) and reporting only those that game or ts holds at a start;
  --near      for an exe with no hit at a start, the functions closest in instruction shape
              (mnemonics and registers, numbers masked), at least --floor alike;
  --twins     name game/ts callers by their closest YR function too (shape index cached in
              build/crossexe/; the first run builds it);
  --twin-of   for YR functions, their closest game/ts functions by shape and their callers;
  --dis       disassemble EXE:ADDR (to the next function start, or N bytes);
  --bytes     search raw hex bodies, nothing compiled.
"""

import argparse
import bisect
import csv
import difflib
import functools
import hashlib
import pickle
import re
import struct
import subprocess
import sys
from pathlib import Path

import capstone
import pefile

sys.path.insert(0, str(Path(__file__).resolve().parent))
from coff import CoffObject  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "build/crossexe"
EXES = {"game": ROOT / "orig/game.exe", "ts": ROOT / "orig/ts-game.exe", "gamemd": ROOT / "orig/gamemd.exe"}
RECOVERED = ROOT / "reference/redalert2-decomp/src/recovered"
CHECK_1006 = ROOT / "reference/ra2-help/for-mischa85-RedAlert2/1006_check.csv"
LEVELS = ("exact", "disp", "loose")
PAD = (0x90, 0xCC)
MAX_REFS = 12
MARKER = re.compile(r"^\s*//\s*(?:FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY):\s*GAMEMD\s+0x[0-9A-Fa-f]+")
HELPER_PREFIX = re.compile(r"^(static inline |inline static |static __inline |__inline |static |inline )"
                           r"(?=[A-Za-z_][\w:<>,*& ]*\()")
STATIC_DATA = re.compile(r"^static [\w:<>*& ]+\s+\w+\s*(\[|=)")
DEFINITION = re.compile(r"^(?:static inline |inline |static )?[A-Za-z_][\w:<>,*& ]*?\b([A-Za-z_]\w*(?:::~?\w+)?)\s*\(")


def load_csv(path: Path) -> list[dict]:
    with path.open() as f:
        return list(csv.DictReader(f))


def rel_refs(code: bytes, t0: int) -> dict[int, list[tuple[int, str]]]:
    """Target -> [(site, 'call' | 'jmp')] for every E8/E9 rel32 in code that lands inside it."""
    out, t1 = {}, t0 + len(code)
    for op, kind in ((0xE8, "call"), (0xE9, "jmp")):
        at = code.find(bytes([op]))
        while at >= 0:
            if at + 5 <= len(code):
                target = (t0 + at + 5 + struct.unpack_from("<i", code, at + 1)[0]) & 0xFFFFFFFF
                if t0 <= target < t1:
                    out.setdefault(target, []).append((t0 + at, kind))
            at = code.find(bytes([op]), at + 1)
    return out


def abs_refs(image: bytes, base: int, t0: int, t1: int, va: int) -> list[tuple[int, str]]:
    """[(address, 'code-imm' | 'data')] of every little-endian copy of va in the image."""
    needle, out = struct.pack("<I", va), []
    at = image.find(needle)
    while at >= 0:
        a = base + at
        out.append((a, "code-imm" if t0 <= a < t1 else "data"))
        at = image.find(needle, at + 1)
    return out


def follows_padding(code: bytes, t0: int, va: int) -> bool:
    """True when va is 16-byte aligned in code and is its first byte or follows padding or a return."""
    off = va - t0
    if va % 16 or not 0 <= off < len(code):
        return False
    if off == 0:
        return True
    return code[off - 1] in PAD or code[off - 1] == 0xC3 or (off >= 3 and code[off - 3] == 0xC2)


def masked_offsets(body: bytes, relocs: set[int], level: str) -> set[int]:
    """The offsets of body that a pattern of this strength leaves as wildcards."""
    masked = set(relocs)
    if level == "exact":
        return masked
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    for ins in md.disasm(body, 0):
        branch = ins.mnemonic.startswith(("j", "call", "loop"))
        if ins.disp_size and not branch:
            masked |= set(range(ins.disp_offset + ins.address, ins.disp_offset + ins.address + ins.disp_size))
        if level == "loose" and ins.imm_size and not branch:
            masked |= set(range(ins.imm_offset + ins.address, ins.imm_offset + ins.address + ins.imm_size))
    return masked


def pattern(body: bytes, masked: set[int]) -> re.Pattern:
    """A regex matching body at every position, overlapping ones included, with masked bytes free."""
    parts = (b"." if k in masked else re.escape(body[k:k + 1]) for k in range(len(body)))
    return re.compile(b"(?=" + b"".join(parts) + b")", re.S)


def grade(code: bytes, t0: int, body: bytes, relocs: set[int]) -> dict[int, str]:
    """{address: strongest level} of every place in code that body matches at some strength."""
    seen: dict[int, str] = {}
    for level in LEVELS:
        for m in pattern(body, masked_offsets(body, relocs, level)).finditer(code):
            seen.setdefault(t0 + m.start(), level)
    return seen


def mnemonics(body: bytes, va: int = 0) -> list[str]:
    """Each instruction as mnemonic and operands, with every number replaced by N."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [f"{i.mnemonic} {re.sub(r'0x[0-9a-f]+|\b\d+\b', 'N', i.op_str)}" for i in md.disasm(body, va)]


def shape_ratio(want: list[str], got: list[str], floor: float) -> float | None:
    """How alike two instruction shapes are, or None when below floor."""
    sm = difflib.SequenceMatcher(None, want, got, autojunk=False)
    if sm.real_quick_ratio() < floor or sm.quick_ratio() < floor:
        return None
    r = sm.ratio()
    return r if r >= floor else None


class Exe:
    """One exe's image and .text, with function starts either known or found by heuristics."""

    def __init__(self, key: str, base: int, image: bytes, text_rva: int, text_size: int,
                 known: dict[int, int] | None = None, names: dict[int, str] | None = None):
        self.key, self.base, self.image = key, base, image
        self.t0 = base + text_rva
        self.code = image[text_rva:text_rva + text_size]
        self.t1 = self.t0 + len(self.code)
        self.known = known
        self.names = names or {}
        self._rel_refs = None
        self._starts = None

    @classmethod
    def load(cls, key: str) -> "Exe":
        data = EXES[key].read_bytes()
        pe = pefile.PE(data=data, fast_load=True)
        base = pe.OPTIONAL_HEADER.ImageBase
        image = bytearray(pe.get_memory_mapped_image())
        text = pe.sections[0]
        known = None
        if key == "gamemd":
            from exebuild import build_of, restore
            restore(image, base, build_of(data))
            known = {int(r["address"], 16): int(r["size"]) for r in load_csv(ROOT / "data/functions.csv")}
        return cls(key, base, bytes(image), text.VirtualAddress, text.Misc_VirtualSize, known, exe_names(key))

    @property
    def rel_refs(self) -> dict[int, list[tuple[int, str]]]:
        if self._rel_refs is None:
            self._rel_refs = rel_refs(self.code, self.t0)
        return self._rel_refs

    @property
    def starts(self) -> list[int]:
        if self._starts is None:
            self._starts = sorted(self.known) if self.known is not None else self.found_starts()
        return self._starts

    def found_starts(self) -> list[int]:
        """Aligned starts reached by a call, a data pointer or an imm32 operand."""
        found = {t for t, sites in self.rel_refs.items() if any(k == "call" for _, k in sites) and t % 16 == 0}
        data = self.image[(self.t1 - self.base + 3) & ~3:]
        for k in range(0, len(data) - 3, 4):
            found.add(struct.unpack_from("<I", data, k)[0])
        for m in re.finditer(rb"[\x68\xb8-\xbf]", self.code):
            if m.start() + 5 <= len(self.code):
                found.add(struct.unpack_from("<I", self.code, m.start() + 1)[0])
        return sorted(v for v in found if follows_padding(self.code, self.t0, v))

    def is_start(self, va: int) -> bool:
        if self.known is not None:
            return va in self.known
        return follows_padding(self.code, self.t0, va)

    def size(self, start: int) -> int:
        """A start's size: known, or the distance to the next start."""
        if self.known is not None and start in self.known:
            return self.known[start]
        k = bisect.bisect_right(self.starts, start)
        return (self.starts[k] if k < len(self.starts) else self.t1) - start

    def containing(self, at: int) -> int | None:
        k = bisect.bisect_right(self.starts, at) - 1
        if k < 0:
            return None
        start = self.starts[k]
        if self.known is not None and at >= start + self.known[start]:
            return None
        return start

    def body(self, start: int, n: int | None = None) -> bytes:
        return self.code[start - self.t0:start - self.t0 + (self.size(start) if n is None else n)]

    def refs(self, va: int) -> list[tuple[int, str]]:
        return self.rel_refs.get(va, []) + abs_refs(self.image, self.base, self.t0, self.t1, va)

    def name(self, va: int) -> str:
        return self.names.get(va, f"FUN_{va:08X}")


def exe_names(key: str) -> dict[int, str]:
    """Names for an exe's addresses: ours for YR, the recovered files and 1006_check.csv for game."""
    names: dict[int, str] = {}
    if key == "gamemd":
        for src in ("progress.csv", "names.csv", "symbols.csv"):
            for r in load_csv(ROOT / "data" / src):
                names.setdefault(int(r["address"], 16), r["name"])
    elif key == "game":
        if RECOVERED.is_dir():
            for f in RECOVERED.rglob("*.cpp"):
                m = re.fullmatch(r"(.*)_([0-9a-f]{8})", f.stem)
                if m:
                    names.setdefault(int(m.group(2), 16), recovered_name(f.parent, m.group(1), f.stem))
        if CHECK_1006.exists():
            for r in load_csv(CHECK_1006):
                symbol = r["m85_symbol_at_va"] or r["our_symbol"]
                if symbol:
                    names.setdefault(int(r["va_1006"], 16), symbol)
    return names


def recovered_name(folder: Path, name: str, stem: str) -> str:
    """A recovered file's function name, qualified by its class folder where the name repeats it."""
    cls = folder.name
    if folder.parent.name == "classes" and name.startswith(cls + "_"):
        name = f"{cls}::{name[len(cls) + 1:]}"
    if name.lower() == "unknown":
        name = stem
    return f"{name} [{cls}]"


@functools.cache
def exe(key: str) -> Exe:
    return Exe.load(key)


def unstatic(lines: list[str]) -> list[str]:
    """The lines with `static` and `inline` dropped from top-level function definitions."""
    out = []
    for line in lines:
        m = HELPER_PREFIX.match(line)
        if m and not STATIC_DATA.match(line):
            line = line[m.end(1):]
        out.append(line)
    return out


def unmarked_names(lines: list[str]) -> set[str]:
    """Names of the top-level functions defined without a checker marker above them."""
    names, marked, pending = set(), set(), False
    for line in lines:
        if MARKER.match(line):
            pending = True
            continue
        m = DEFINITION.match(line)
        if m and not line.rstrip().endswith(";"):
            (marked if pending else names).add(m.group(1).split("::")[-1])
            pending = False
        elif line.strip() and not line.lstrip().startswith("//"):
            pending = False
    return names - marked


def undname(symbols: list[str]) -> dict[str, str]:
    """Decorated -> undecorated names, by llvm-undname-18 (empty when it is missing)."""
    try:
        out = subprocess.run(["llvm-undname-18"], input="\n".join(symbols) + "\n",
                             capture_output=True, text=True).stdout
    except FileNotFoundError:
        return {}
    res, lines, k = {}, out.split("\n"), 0
    while k + 1 < len(lines):
        if lines[k] in symbols and lines[k] not in res:
            res[lines[k]] = lines[k + 1]
            k += 2
        else:
            k += 1
    return res


def object_bodies(obj: CoffObject, wanted) -> list[tuple[str, str, bytes, set[int]]]:
    """(symbol, undecorated name, body without trailing padding, relocated offsets) per wanted function."""
    funcs = list(obj.functions())
    und = undname([s.name for s, *_ in funcs if s.name.startswith("?")])
    bodies = []
    for sym, sec, start, end in funcs:
        full = und.get(sym.name, sym.name)
        if not wanted(sym.name, full):
            continue
        body = sec.data[start:end]
        while len(body) > 1 and body[-1] in PAD:
            body = body[:-1]
        relocs = {r.offset - start + k for r in sec.relocs if start <= r.offset < end for k in range(4)}
        bodies.append((sym.name, full, body, relocs))
    return bodies


def compile_bodies(src: Path, wanted, overlay: Path | None, extern: bool, keep: bool):
    """Compile src (or its --extern copy) in build/crossexe/ and return its wanted bodies."""
    from check import compile_source
    OUT.mkdir(parents=True, exist_ok=True)
    source = src
    if extern:
        source = OUT / src.name
        text = src.read_text(encoding="latin-1").splitlines(keepends=True)
        source.write_text("".join(unstatic(text)), encoding="latin-1")
    obj_path, log = compile_source(source, overlay=overlay, out=OUT / (src.stem + ".obj"))
    try:
        if obj_path is None:
            return None, log
        return object_bodies(CoffObject(obj_path.read_bytes()), wanted), log
    finally:
        if not keep:
            if obj_path is not None:
                obj_path.unlink(missing_ok=True)
            if source != src:
                source.unlink(missing_ok=True)


def search(exes: list[Exe], body: bytes, relocs: set[int]) -> dict[str, list[tuple[int, str, bool, list]]]:
    """{exe key: [(address, level, at a start, references)]}, strongest level first."""
    result = {}
    for e in exes:
        seen = grade(e.code, e.t0, body, relocs)
        order = sorted(seen.items(), key=lambda kv: (LEVELS.index(kv[1]), kv[0]))
        result[e.key] = [(va, level, e.is_start(va), e.refs(va)) for va, level in order]
    return result


def shapes_stamp() -> str:
    h = hashlib.sha256((ROOT / "data/functions.csv").read_bytes())
    h.update(EXES["gamemd"].read_bytes())
    return h.hexdigest()


@functools.cache
def yr_shapes() -> list[tuple[int, int, list[str]]]:
    """(start, size, shape) of every YR function up to 20000 bytes, cached under build/crossexe/."""
    cache = OUT / "yr_shapes.pickle"
    stamp = shapes_stamp()
    if cache.exists():
        saved = pickle.loads(cache.read_bytes())
        if saved.get("stamp") == stamp:
            return saved["shapes"]
    y = exe("gamemd")
    shapes = [(s, n, mnemonics(y.body(s), s)) for s, n in sorted(y.known.items()) if n <= 20000]
    OUT.mkdir(parents=True, exist_ok=True)
    cache.write_bytes(pickle.dumps({"stamp": stamp, "shapes": shapes}))
    return shapes


@functools.cache
def yr_twin(key: str, va: int, floor: float = 0.7) -> tuple[float, int] | None:
    """The YR function most like a game/ts function: (ratio, address) or None."""
    e = exe(key)
    size = e.size(va)
    got = mnemonics(e.body(va, size), va)
    best = None
    for s, n, want in yr_shapes():
        if 0.75 * size <= n <= 1.33 * size + 16:
            r = shape_ratio(got, want, floor)
            if r is not None and (best is None or r > best[0]):
                best = (r, s)
    return best


def near(e: Exe, body: bytes, top: int = 3, floor: float = 0.75) -> list[tuple[float, int, int]]:
    """[(ratio, start, size)] of e's functions closest to body in instruction shape."""
    want, n, out = mnemonics(body), len(body), []
    for s in e.starts:
        size = e.size(s)
        if 0.7 * n <= size <= 1.6 * n + 16:
            r = shape_ratio(want, mnemonics(e.body(s, size), s), floor)
            if r is not None:
                out.append((r, s, size))
    return sorted(out, reverse=True)[:top]


def label(e: Exe, va: int | None, twins: bool) -> str:
    if va is None:
        return "?"
    text = e.name(va)
    if twins and e.key != "gamemd":
        t = yr_twin(e.key, va)
        if t:
            text += f" [~YR 0x{t[1]:08X} {exe('gamemd').names.get(t[1], '')} {t[0]:.2f}]"
    return text


def callers(e: Exe, va: int) -> list[int]:
    """The functions holding a rel32 call or jump to va."""
    return sorted({h for h in (e.containing(x) for x, _ in e.rel_refs.get(va, [])) if h is not None})


def describe(e: Exe, hits, max_hits: int, twins: bool = False) -> list[str]:
    """Report lines for one exe's hits: those at a function start first, each with its references."""
    starts = [h for h in hits if h[2]]
    lines = [f"  {e.key}: {len(hits)} hit(s), {len(starts)} at a function start"]
    for va, level, start, refs in (starts + [h for h in hits if not h[2]])[:max_hits]:
        tag = "start" if start else f"inside {label(e, e.containing(va), False)}"
        lines.append(f"    0x{va:08X} {level:<5} {tag} {e.names.get(va, '')}".rstrip())
        for site, kind in refs[:MAX_REFS]:
            holder = e.containing(site) if kind != "data" else None
            lines.append(f"        {kind:<8} from 0x{site:08X}" + (f" in {label(e, holder, twins)}" if holder else ""))
        if len(refs) > MAX_REFS:
            lines.append(f"        ... {len(refs) - MAX_REFS} more refs")
    return lines


def near_lines(e: Exe, body: bytes, floor: float) -> list[str]:
    lines = []
    for r, va, size in near(e, body, floor=floor):
        holders = callers(e, va)
        more = " ..." if len(holders) > 6 else ""
        lines.append(f"    near {r:.3f} 0x{va:08X} ({size} bytes) {e.names.get(va, '')}"
                     f" callers: {', '.join(e.name(h) for h in holders[:6])}{more}")
    return lines


def report(exes: list[Exe], bodies, args) -> None:
    for sym, full, body, relocs in bodies:
        print(f"== {full}  [{sym}] {len(body)} bytes, {len(relocs) // 4} relocated fields")
        if args.asm:
            md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
            for i in md.disasm(body, 0):
                print(f"  {i.address:08X}: {i.bytes.hex():<20} {i.mnemonic} {i.op_str}")
        if len(body) < 8:
            print("  (short body: hits are weak evidence)")
        res = search(exes, body, relocs)
        for e in exes:
            print("\n".join(describe(e, res[e.key], args.max_hits, args.twins)))
            if args.near and not any(h[2] for h in res[e.key]):
                for line in near_lines(e, body, args.floor):
                    print(line)


def sweep(exes: list[Exe], src: Path, args) -> None:
    """One line per unmarked function, with details where game or ts holds it at a start."""
    names = set(args.only) or unmarked_names(src.read_text(encoding="latin-1").splitlines())
    if not names:
        print(f"## {src.name}: no unmarked functions")
        return
    called = re.compile(r"\b(" + "|".join(map(re.escape, sorted(names))) + r")\(")
    bodies, log = compile_bodies(src, lambda s, full: bool(called.search(full)) or s in names, args.overlay,
                                 True, args.keep)
    if bodies is None:
        print(f"## {src.name}: compile failed\n{log}")
        return
    print(f"## {src.name}: {len(bodies)} bodies ({', '.join(sorted(names))})")
    for sym, full, body, relocs in bodies:
        starts = {k: [h for h in v if h[2]] for k, v in search(exes, body, relocs).items()}
        found = any(starts.get(k) for k in ("game", "ts"))
        tally = " ".join(f"{k}:{len(v)}" for k, v in starts.items())
        print(f"  {'*' if found else ' '} {full} ({len(body)} B) starts {tally}")
        if found:
            for e in exes:
                for va, level, _, refs in starts[e.key][:4]:
                    holders = callers(e, va)
                    print(f"      {e.key} 0x{va:08X} {level} {e.names.get(va, '')}; {len(refs)} refs, callers "
                          f"{', '.join(label(e, h, args.twins) for h in holders[:5])}")


def twin_of(exes: list[Exe], addresses: list[str], floor: float) -> None:
    y = exe("gamemd")
    for t in addresses:
        va = int(t, 16)
        body = y.body(va)
        print(f"== twins of YR 0x{va:08X} {y.name(va)} ({len(body)} bytes)")
        for e in exes:
            if e.key != "gamemd":
                for r, s, size in near(e, body, top=3, floor=floor):
                    print(f"  {e.key} {r:.3f} 0x{s:08X} ({size} bytes) {e.names.get(s, '')}"
                          f" callers: {', '.join(f'0x{h:08X}' for h in callers(e, s)[:8])}")


def dis(spec: str, twins: bool) -> None:
    parts = spec.split(":")
    e = exe(parts[0])
    va = int(parts[1], 16)
    n = int(parts[2], 0) if len(parts) > 2 else e.size(va)
    print(f"== {e.key} 0x{va:08X} {n} bytes")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for i in md.disasm(e.body(va, n), va):
        extra = ""
        if i.mnemonic in ("call", "jmp") and i.op_str.startswith("0x"):
            target = int(i.op_str, 16)
            if not va <= target < va + n:
                extra = f"   ; {label(e, target, twins)}"
        print(f"  {i.address:08X}: {i.bytes.hex():<16} {i.mnemonic} {i.op_str}{extra}")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("src", nargs="*", type=Path)
    ap.add_argument("--only", nargs="*", default=[], help="name substrings of the functions to search")
    ap.add_argument("--extern", action="store_true", help="make top-level static/inline functions extern")
    ap.add_argument("--sweep", action="store_true", help="search every unmarked function, report start hits")
    ap.add_argument("--overlay", type=Path)
    ap.add_argument("--exe", nargs="*", choices=list(EXES), help="exes to search (default: those present)")
    ap.add_argument("--max-hits", type=int, default=8)
    ap.add_argument("--asm", action="store_true", help="print each body's disassembly")
    ap.add_argument("--near", action="store_true", help="with no hit at a start, list the closest functions")
    ap.add_argument("--floor", type=float, default=0.6, help="lowest shape ratio for --near and --twin-of")
    ap.add_argument("--twins", action="store_true", help="name game/ts callers by their closest YR function")
    ap.add_argument("--twin-of", nargs="*", default=[], help="YR addresses: their closest game/ts functions")
    ap.add_argument("--dis", nargs="*", default=[], help="EXE:0xADDR[:N]: disassemble")
    ap.add_argument("--bytes", nargs="*", default=[], help="raw hex bodies to search")
    ap.add_argument("--keep", action="store_true", help="keep the object and the --extern copy")
    args = ap.parse_args()

    keys = args.exe or [k for k, p in EXES.items() if p.exists()]
    missing = [k for k in keys if not EXES[k].exists()]
    if missing:
        raise SystemExit(f"missing: {', '.join(str(EXES[k].relative_to(ROOT)) for k in missing)}")
    if args.twin_of or args.dis or args.twins:
        if not EXES["gamemd"].exists():
            raise SystemExit("--twin-of, --dis and --twins need orig/gamemd.exe")
    exes = [exe(k) for k in keys]

    if args.twin_of or args.dis:
        twin_of(exes, args.twin_of, args.floor)
        for spec in args.dis:
            dis(spec, args.twins)
        return 0
    if args.bytes:
        report(exes, [(f"bytes{i}", f"bytes{i}", bytes.fromhex(h), set()) for i, h in enumerate(args.bytes)], args)
        return 0
    if not args.src:
        ap.error("give SRC, --bytes, --twin-of or --dis")
    status = 0
    for src in args.src:
        src = src.resolve()
        if args.sweep:
            sweep(exes, src, args)
            continue
        only = args.only
        bodies, log = compile_bodies(src, lambda s, full: not only or any(o in s or o in full for o in only),
                                     args.overlay, args.extern, args.keep)
        if bodies is None:
            print(f"{src.name}: compile failed\n{log}", file=sys.stderr)
            status = 1
            continue
        if not bodies:
            print(f"{src.name}: no function matches --only"
                  + ("" if args.extern else " (an inlined helper needs --extern)"))
        report(exes, bodies, args)
    return status


if __name__ == "__main__":
    sys.exit(main())
