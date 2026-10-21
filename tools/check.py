"""Compile a source file and compare its annotated functions with gamemd.exe.

    uv run tools/check.py src/rawfile.cpp            # every annotated function in the file
    uv run tools/check.py src/rawfile.cpp 0x65CF00   # one of them
    uv run tools/check.py work.cpp --overlay hdrs/    # headers in hdrs/ override include/
    uv run tools/check.py work.cpp 0x65CF00 --score   # also print the normalised score

A function is tied to its address by a marker on the line before its definition, in
reccmp's annotation format (module GAMEMD):

    // FUNCTION: GAMEMD 0x0065CF00
    int RawFileClass::Seek(int pos, int dir)

A function with no definition in the file (a header inline the object emits out of line, a
template instance, a compiler-generated destructor or initialiser) is named on the comment
line after its marker: a qualified name, or an exact decorated symbol, which starts with `?`
or carries the SYMBOL modifier:

    // FUNCTION: GAMEMD 0x005FE020
    // XSurface::XSurface
    // TEMPLATE: GAMEMD 0x0063E540
    // ?Add@?$DynamicVectorClass@H@@QAE_NABH@Z
    // SYNTHETIC: GAMEMD 0x0048E050 SYMBOL
    // _$E12

An .asm file is assembled with MASM instead, and must be a file of Westwood's TibSun
archive copied unchanged but for its markers, `; FUNCTION: GAMEMD 0x007C5DB0` on the line before
a PROC (the PROC's name is the function's).

A function MATCHES when it is as long as the original (data/functions.csv), every byte
the compiler emits is identical, and every field the linker fills in points at the
right target:
  - a jump or table inside the function: the same place in the original;
  - a string literal or constant the file defines: the original's target holds the
    same bytes;
  - a function annotated in the same file: its annotated address;
  - any other name: its address in data/symbols.csv, or in another annotated file.
A name with no known address is reported as unverified, not as a mismatch.
Bytes EA patched after linking are first put back from data/exe_patches.csv.

--score adds a line with a normalised similarity: the instruction lists compared with every
address in the image, every jump target inside the function and every stack-slot
displacement ([esp+N], [ebp-N], and [ebp+N] in a function with a frame) masked, so a
changed frame size or a shifted jump does not hide progress.
"""

import csv
import difflib
import hashlib
import os
import re
import shutil
import struct
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

import capstone
import pefile

sys.path.insert(0, str(Path(__file__).resolve().parent))
from coff import REL_DIR32, REL_REL32, CoffObject  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
FLAGS = ["/c", "/O2", "/Ob2", "/Gr", "/GR", "/MT", "/DWIN32"]
ANNOTATION = re.compile(r"^\s*//\s*(?:FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY):\s*GAMEMD\s+(0x[0-9A-Fa-f]+)"
                        r"((?:\s+(?:FOLDED|SYMBOL))*)\s*$")
ASM_ANNOTATION = re.compile(r"^\s*;\s*FUNCTION:\s*GAMEMD\s+(0x[0-9A-Fa-f]+)\s*$")
# The name comment after a marker: one token, a qualified name or a decorated symbol.
NAME_COMMENT = re.compile(r"^\s*//\s*([^\s:][^\s]*(?:::[^\s]+)*|(?:[\w?$@]+::)*operator\s*\S+)\s*$")
PROC = re.compile(r"^\s*([\w?$@]+)\s+PROC\b", re.I | re.M)
ASM_FLAGS = ["/c", "/coff", "/Cx"]
FORBIDDEN = re.compile(r"\b(__asm|_asm|__emit|_emit)\b|#\s*pragma\s+(optimize|code_seg)")
PADDING = (0x90, 0xCC)
TIBSUN = ROOT / "reference/tibsun/code"


class Original:
    def __init__(self, path: Path = ROOT / "orig/gamemd.exe"):
        self.pe = pefile.PE(str(path), fast_load=True)
        self._imports = None
        self.base = self.pe.OPTIONAL_HEADER.ImageBase
        self.end = self.base + self.pe.OPTIONAL_HEADER.SizeOfImage
        self.image = self.pe.get_memory_mapped_image()
        self.sizes = {}
        with (ROOT / "data/functions.csv").open() as f:
            for r in csv.DictReader(f):
                self.sizes[int(r["address"], 16)] = int(r["size"])
        patches = ROOT / "data/exe_patches.csv"
        if patches.exists():
            img = bytearray(self.image)
            with patches.open() as f:
                for r in csv.DictReader(f):
                    at = int(r["address"], 16) - self.base
                    orig = bytes.fromhex(r["original"])
                    if img[at:at + len(orig)] != bytes.fromhex(r["patched"]):
                        raise SystemExit(f"exe_patches.csv: bytes at {r['address']} are not EA's patch")
                    img[at:at + len(orig)] = orig
            self.image = bytes(img)

    def imports(self) -> dict[str, int]:
        """Import-table slot address of each imported function, by name."""
        if self._imports is not None:
            return self._imports
        self.pe.parse_data_directories(directories=[
            pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
        self._imports = {imp.name.decode(): imp.address
                         for dll in self.pe.DIRECTORY_ENTRY_IMPORT for imp in dll.imports if imp.name}
        return self._imports

    def read(self, va: int, size: int) -> bytes:
        return self.image[va - self.base:va - self.base + size]

    def u32(self, va: int) -> int:
        return struct.unpack_from("<I", self.image, va - self.base)[0]


ASM_START = re.compile(r"\b(__asm|_asm)\b")
_archive_text: str | None = None


def normalised(text: str) -> str:
    """Code with comments removed and every run of whitespace made one space."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    return re.sub(r"\s+", " ", text).strip()


def archive_text() -> str:
    global _archive_text
    if _archive_text is None:
        files = sorted(TIBSUN.glob("*.cpp")) + sorted(TIBSUN.glob("*.h")) if TIBSUN.exists() else []
        _archive_text = "\n".join(normalised(f.read_text(errors="replace")) for f in files)
    return _archive_text


def asm_blocks(text: str) -> list[tuple[int, int]]:
    """Spans of each `_asm { ... }` block, or of a one-line `_asm` statement."""
    spans, pos = [], 0
    while m := ASM_START.search(text, pos):
        i = m.end()
        while i < len(text) and text[i] in " \t":
            i += 1
        if i < len(text) and text[i] == "{":
            depth, j = 0, i
            while j < len(text):
                depth += {"{": 1, "}": -1}.get(text[j], 0)
                j += 1
                if depth == 0:
                    break
        else:
            j = text.find("\n", i)
            j = len(text) if j < 0 else j
        spans.append((m.start(), j))
        pos = j
    return spans


def without_archived_asm(text: str) -> str:
    """The source minus every asm block found unchanged in Westwood's TibSun archive, which
    is the original's own code; asm written or altered here stays and is refused."""
    archive, out, last = None, [], 0
    for start, end in asm_blocks(text):
        archive = archive or archive_text()
        block = normalised(text[start:end])
        if block and block in archive:
            out.append(text[last:start])
            last = end
    out.append(text[last:])
    return "".join(out)


def unmarked(text: str) -> str:
    """Assembly source with its marker lines removed, LF line endings and none at the end."""
    lines = text.replace("\r\n", "\n").splitlines(keepends=True)
    return "".join(line for line in lines if not ASM_ANNOTATION.match(line)).rstrip("\n")


def archived_asm(src: Path) -> Path | None:
    """The TibSun archive's .asm file that `src`, markers aside, copies byte for byte."""
    text = unmarked(src.read_text(encoding="latin-1"))
    for f in sorted(TIBSUN.rglob("*.asm")) if TIBSUN.exists() else []:
        if unmarked(f.read_text(encoding="latin-1")) == text:
            return f
    return None


def is_asm(src: Path) -> bool:
    return src.suffix.lower() == ".asm"


def winpath(p: Path) -> str:
    """The path as the compiler sees it: the checkout is drive Y: (tools/wineenv.sh)."""
    p = p.resolve()
    if p == ROOT or ROOT in p.parents:
        return "Y:\\" + "\\".join(p.relative_to(ROOT).parts)
    return "Z:" + str(p).replace("/", "\\")


def annotations(src: Path) -> list[tuple[int, str]]:
    """(address, qualified name) for each annotated definition in a source file."""
    lines = src.read_text(errors="replace").splitlines()
    asm = is_asm(src)
    out = []
    for i, line in enumerate(lines):
        m = (ASM_ANNOTATION if asm else ANNOTATION).match(line)
        if not m:
            continue
        nxt = lines[i + 1] if i + 1 < len(lines) else ""
        named = None if asm or ANNOTATION.match(nxt) else NAME_COMMENT.match(nxt)
        if named:
            name = named.group(1).replace(" ", "")
            # An exact decorated symbol is marked with a leading "=".
            exact = (name.startswith("?") and "::" not in name) or "SYMBOL" in m.group(2)
            out.append((int(m.group(1), 16), "=" + name if exact else name))
            continue
        if asm:
            n = PROC.search("\n".join(lines[i + 1:i + 4]))
            out.append((int(m.group(1), 16), n.group(1) if n else ""))
            continue
        decl = " ".join(lines[i + 1:i + 4])
        n = re.search(r"((?:\w+::)*(?:operator\s*\S+?|~?\w+))\s*\(", decl)
        out.append((int(m.group(1), 16), n.group(1).replace(" ", "") if n else ""))
    return out


def known_decorated() -> dict[str, int]:
    """Decorated symbol -> address, from data/progress.csv and data/names.csv."""
    out = {}
    for path in (ROOT / "data/progress.csv", ROOT / "data/names.csv"):
        if path.exists():
            with path.open() as f:
                for r in csv.DictReader(f):
                    # Initialiser and static-guard names are local to their own source file.
                    if r.get("symbol") and not re.match(r"^_?\$[ES]\d|^\?\$S\d", r["symbol"]):
                        out[r["symbol"]] = int(r["address"], 16)
    return out


def known_names() -> dict[str, int]:
    """Qualified name -> address, from Westwood's proven names, YRpp and the CRT."""
    names = {}
    # Westwood's names, proven by matches, before YRpp's.
    with (ROOT / "data/names.csv").open() as f:
        for r in csv.DictReader(f):
            names.setdefault(r["name"], int(r["address"], 16))
    with (ROOT / "data/symbols.csv").open() as f:
        for r in csv.DictReader(f):
            names.setdefault(r["name"], int(r["address"], 16))
    with (ROOT / "data/library.csv").open() as f:
        for r in csv.DictReader(f):
            names.setdefault(qualified(r["symbol"]), int(r["address"], 16))
    return names


def qualified(sym: str) -> str:
    """Enough undecoration to look a symbol up by name: '?Seek@RawFileClass@@...' -> 'RawFileClass::Seek'."""
    if sym.startswith(("??0", "??1")):
        scopes = sym[3:].split("@@", 1)[0].split("@")
        return "::".join(reversed(scopes)) + "::" + ("~" if sym[2] == "1" else "") + scopes[0]
    if sym.startswith("??"):
        return sym
    if sym.startswith("?"):
        parts = sym[1:].split("@@", 1)[0].split("@")
        return "::".join(reversed(parts[1:])) + ("::" if len(parts) > 1 else "") + parts[0]
    # A C symbol carries one prefix character: _toupper is toupper, __toupper is _toupper.
    return (sym[1:] if sym[:1] in ("_", "@") else sym).split("@", 1)[0]


def prepare_headers(overlay: Path | None) -> Path:
    """One directory of headers for this run: the TibSun archive's, include/ over them,
    then `overlay`'s. One directory, because a quoted #include searches the including
    file's own first; one per run, so parallel checks never share it. Its name has a
    fixed length, since the paths the compiler sees can move its output."""
    if not TIBSUN.is_dir():
        raise SystemExit(f"{TIBSUN} is missing: run tools/setup_toolchain.sh")
    tag = hashlib.sha256(f"{os.getpid()} {overlay}".encode()).hexdigest()[:8]
    headers = ROOT / "build/hdr" / tag
    if headers.exists():
        shutil.rmtree(headers)
    for base in (TIBSUN, ROOT / "include", *([overlay] if overlay else [])):
        for h in base.rglob("*"):
            if h.is_file() and h.suffix.lower() in (".h", ".hh", ".hpp", ".inl"):
                dest = headers / h.relative_to(base)
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(h, dest)
    return headers


def compile_source(src: Path, overlay: Path | None = None, headers: Path | None = None,
                   out: Path | None = None, extra: tuple[str, ...] = ()) -> tuple[Path | None, str]:
    """Compile src to an object. `headers` is a directory prepare_headers already filled,
    which the caller keeps; `out` is the object's path; `extra` adds compiler options.
    An .asm file is assembled, only if it is a copy of an archive file."""
    if out is None:
        rel = src.resolve().relative_to(ROOT) if ROOT in src.resolve().parents else Path(src.name)
        out = ROOT / "build/obj" / rel.with_suffix(".obj")
    if is_asm(src):
        return assemble_source(src, out)
    text = src.read_text(errors="replace")
    bad = FORBIDDEN.search(without_archived_asm(text))
    if bad:
        return None, f"{src}: '{bad.group(0)}' is not allowed"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.unlink(missing_ok=True)
    own = headers is None
    if own:
        headers = prepare_headers(overlay)
    try:
        proc = run_compiler(headers, *extra, f"/Fo{winpath(out)}", winpath(src))
    finally:
        if own:
            shutil.rmtree(headers, ignore_errors=True)
    log = proc.stdout + proc.stderr
    return (out if proc.returncode == 0 and out.exists() else None), log


def assemble_source(src: Path, out: Path) -> tuple[Path | None, str]:
    """Assemble src to `out` with MASM, if it copies a file of the TibSun archive."""
    if archived_asm(src) is None:
        return None, f"{src}: not a copy of an .asm file in {TIBSUN.relative_to(ROOT)}"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.unlink(missing_ok=True)
    cmd = [str(ROOT / "tools/wml"), *ASM_FLAGS, f"/Fo{winpath(out)}", winpath(src)]
    proc = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    log = proc.stdout + proc.stderr
    return (out if proc.returncode == 0 and out.exists() else None), log


def run_compiler(headers: Path, *args: str) -> subprocess.CompletedProcess:
    """The original compiler with the fixed flags and `headers` (from prepare_headers)."""
    extra = [f"/I{winpath(headers / 'vqalib')}"] if (headers / "vqalib").is_dir() else []
    cmd = [str(ROOT / "tools/wcl"), *FLAGS, f"/I{winpath(headers)}", *extra, *args]
    return subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)


@dataclass
class Result:
    address: int
    name: str
    size: int
    ours: int
    bytes_match: bool
    symbol: str = ""
    refs: list[str] = field(default_factory=list)
    mismatched_refs: int = 0
    unverified: int = 0
    ratio: float = 0.0
    norm_ratio: float = 0.0
    diff: list[str] = field(default_factory=list)

    @property
    def matched(self) -> bool:
        return self.bytes_match and self.mismatched_refs == 0


def disasm(code: bytes, va: int) -> list[str]:
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [f"{ins.mnemonic} {ins.op_str}".strip() for ins in md.disasm(code, va)]


def encoding_diff(theirs: bytes, ours: bytes, va: int) -> list[str]:
    """The instructions that read the same but are encoded differently, as hex."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [f"0x{x.address:08X} {x.mnemonic} {x.op_str}: original {x.bytes.hex()}, ours {y.bytes.hex()}"
            for x, y in zip(md.disasm(theirs, va), md.disasm(ours, va)) if x.bytes != y.bytes]


HEX = re.compile(r"0x[0-9a-f]+")
ESP_SLOT = re.compile(r"\[esp(?: [+-] (?:0x[0-9a-f]+|\d+))?\]")
EBP_SLOT = re.compile(r"\[ebp(?: [+-] (?:0x[0-9a-f]+|\d+))?\]")
EBP_LOCAL = re.compile(r"\[ebp - (?:0x[0-9a-f]+|\d+)\]")


def mask_addresses(listing: list[str], lo: int, hi: int, image: tuple[int, int]) -> list[str]:
    """The listing with targets inside [lo, hi) as <local> and other image addresses as <addr>."""
    def sub(m):
        v = int(m.group(), 16)
        if lo <= v < hi:
            return "<local>"
        return "<addr>" if image[0] <= v < image[1] else m.group()
    return [HEX.sub(sub, line) for line in listing]


def mask_stack(listing: list[str]) -> list[str]:
    """The listing with stack-slot displacements masked; [ebp+N] only under a frame."""
    frame = listing[:2] == ["push ebp", "mov ebp, esp"]
    ebp = EBP_SLOT if frame else EBP_LOCAL
    return [ebp.sub("[ebp+N]", ESP_SLOT.sub("[esp+N]", line)) for line in listing]


def placeholder_linked(orig: Original, obj: CoffObject, sec, start: int, ours: bytes, address: int,
                       size: int) -> bytes:
    """Our bytes with each linker-filled field pointing where its own target would land:
    a label in this section at its place relative to `address`, anything else at an
    address in the image outside the function."""
    placeholder = orig.base + 0x1000
    if address <= placeholder < address + max(size, len(ours)):
        placeholder = orig.end - 0x10
    out = bytearray(ours)
    for r in sec.relocs:
        off = r.offset - start
        if not 0 <= off <= len(out) - 4:
            continue
        addend = struct.unpack_from("<i", out, off)[0]
        target = obj.by_index[r.symbol]
        dest = address - start + target.value if target.section == sec.number else placeholder
        if r.type == REL_DIR32:
            struct.pack_into("<I", out, off, (dest + addend) & 0xFFFFFFFF)
        elif r.type == REL_REL32:
            struct.pack_into("<I", out, off, (dest + addend - (address + off + 4)) & 0xFFFFFFFF)
    return bytes(out)


def normalised_listings(orig: Original, obj: CoffObject, sec, start: int, ours: bytes, address: int,
                        theirs: bytes, stack: bool = True) -> tuple[list[str], list[str]]:
    """(theirs, ours) as instruction lists with addresses masked, and stack slots too if `stack`."""
    image = (orig.base, orig.end)
    a = mask_addresses(disasm(theirs, address), address, address + len(theirs), image)
    linked = placeholder_linked(orig, obj, sec, start, ours, address, len(theirs))
    b = mask_addresses(disasm(linked, address), address, address + len(ours), image)
    return (mask_stack(a), mask_stack(b)) if stack else (a, b)


def similarity(a: list[str], b: list[str]) -> float:
    return difflib.SequenceMatcher(None, a, b, autojunk=False).ratio()


def compare(orig: Original, obj: CoffObject, raw: bytes, address: int, name: str,
            names: dict[str, int], local: dict[str, int], score: bool = False,
            prefer: dict[str, int] | None = None, taken: set[str] = frozenset()) -> Result:
    """The best result among the compiled functions the name could be: overloads share one.
    `local` maps decorated symbols to addresses; `names` maps undecorated names. `score`
    fills in the normalised similarity too. Among equally good results, a symbol `prefer`
    ties to this address wins, then one not in `taken`: byte-identical overloads differ."""
    prefixes = mangled(name)
    if name.startswith("="):
        picks = [f for f in obj.functions() if f[0].name == prefixes[0]]
    else:
        picks = [f for f in obj.functions() if f[0].name.startswith(prefixes)]
    if not picks:
        cands = ", ".join(f[0].name for f in obj.functions())
        raise SystemExit(f"{name.lstrip('=')}: no compiled function has this name ({cands})")
    results = [compare_one(orig, obj, raw, address, name, names, local, p, score) for p in picks]
    prefer = prefer or {}
    return max(results, key=lambda r: (r.matched, r.bytes_match, r.ratio,
                                       prefer.get(r.symbol) == address, r.symbol not in taken))


def compare_one(orig, obj, raw, address, name, names, local, pick, score=False) -> Result:
    sym, sec, start, end = pick
    relocs = [r for r in sec.relocs if start <= r.offset < end]
    masked = {r.offset - start + k for r in relocs for k in range(4)}
    ours = sec.data[start:end]
    while ours and ours[-1] in PADDING and len(ours) - 1 not in masked:
        ours = ours[:-1]
    size = orig.sizes.get(address)
    if size is None:
        raise SystemExit(f"0x{address:08X} is not a function start in data/functions.csv")
    theirs = orig.read(address, size)
    same = len(ours) == len(theirs) and all(
        k in masked or a == b for k, (a, b) in enumerate(zip(ours, theirs)))
    res = Result(address, name, size, len(ours), same, sym.name)

    # Field offsets line up with the original's only when the bytes do.
    for ref in ref_checks(orig, obj, raw, pick, ours, theirs, address, names, local) if same else []:
        if ref.status == "mismatch":
            res.mismatched_refs += 1
        if ref.status == "unverified":
            res.unverified += 1
        if ref.status != "ok":
            res.refs.append(f"  +0x{ref.offset:X} {qualified(ref.target.name)}: original 0x{ref.got:08X}"
                            + (f", expected 0x{ref.want:08X}" if ref.want is not None else "")
                            + f" [{ref.status}]")

    if not res.matched:
        # Our linker-filled fields take the original's values, so only real differences show.
        shown = bytearray(ours)
        for k in masked:
            if k < len(shown) and k < len(theirs):
                shown[k] = theirs[k]
        a = disasm(theirs, address)
        b = disasm(bytes(shown), address)
        res.ratio = difflib.SequenceMatcher(None, a, b, autojunk=False).ratio()
        for line in difflib.unified_diff(a, b, "original", "ours", n=2, lineterm=""):
            res.diff.append(line)
        if a == b:
            res.diff += encoding_diff(theirs, bytes(shown), address)
        if score:
            res.norm_ratio = similarity(*normalised_listings(orig, obj, sec, start, ours, address, theirs))
    else:
        res.norm_ratio = 1.0
    return res


@dataclass
class RefCheck:
    offset: int
    type: int
    addend: int
    target: object
    got: int
    status: str
    want: int | None


def ref_checks(orig, obj, raw, pick, ours: bytes, theirs: bytes, address: int, names, local) -> list[RefCheck]:
    """Each DIR32 or REL32 field of the compiled function `pick` that lies inside the
    original's bytes: the target the original's field holds, and whether it is ours."""
    sym, sec, start, end = pick
    out = []
    for r in sec.relocs:
        if not start <= r.offset < end:
            continue
        off = r.offset - start
        if off + 4 > len(theirs):
            continue
        target_sym = obj.by_index[r.symbol]
        addend = struct.unpack_from("<i", ours, off)[0] if off + 4 <= len(ours) else 0
        if r.type == REL_DIR32:
            got = orig.u32(address + off) - addend
        elif r.type == REL_REL32:
            got = (address + off + 4 + struct.unpack_from("<i", theirs, off)[0] - addend) & 0xFFFFFFFF
        else:
            continue
        status, want = check_ref(orig, obj, raw, target_sym, sec, start, address, got, names, local)
        out.append(RefCheck(off, r.type, addend, target_sym, got, status, want))
    return out


def bind_annotations(orig, obj, raw, src: Path) -> tuple[dict[str, int], list[str]]:
    """Decorated symbol -> address for every known match, with each annotated function of
    `src` bound to the compiled function its bytes pick; and a line per double claim."""
    local = known_decorated()
    known = dict(local)
    claimed: dict[str, int] = {}
    duplicates = []
    for address, name in annotations(src):
        symbol = compare(orig, obj, raw, address, name, {}, {}, prefer=known, taken=set(claimed)).symbol
        if symbol in claimed and claimed[symbol] != address:
            duplicates.append(f"0x{address:08X} {name.lstrip('=') or symbol}: same compiled function as "
                              f"0x{claimed[symbol]:08X} ({symbol})")
        claimed[symbol] = address
        local[symbol] = address
    return local, duplicates


def function_symbols(obj: CoffObject) -> set[str]:
    """Every function symbol the object defines or references."""
    if not hasattr(obj, "function_names"):
        obj.function_names = {s.name for s in obj.symbols if s.type == 0x20}
    return obj.function_names


def overloaded(sym: str, decorated, declared=()) -> bool:
    """Whether the undecorated name of `sym` is ambiguous: another overload of it is known
    by its decorated symbol, or is in `declared` too."""
    name = qualified(sym)
    return any(d != sym and qualified(d) == name for d in (*decorated, *declared))


def check_ref(orig, obj, raw, target, sec, start, address, got, names, local):
    """Whether a linker-filled field points where the original's does."""
    if target.section == sec.number:
        want = address - start + target.value
        return ("ok" if got == want else "mismatch"), want
    name = qualified(target.name)
    if target.name in local:
        want = local[target.name]
        return ("ok" if got == want else "mismatch"), want
    if target.section > 0:
        tsec = obj.sections[target.section - 1]
        if not tsec.is_code:
            content = tsec.data[target.value:] if target.name.startswith(("??_C@", "__real@")) else b""
            if content:
                return ("ok" if orig.read(got, len(content)) == content else "mismatch"), None
    if target.name.startswith("__imp_"):
        imported = target.name[len("__imp_"):].lstrip("_").split("@", 1)[0]
        want = orig.imports().get(imported)
        if want is not None:
            return ("ok" if got == want else "mismatch"), want
    # A placeholder name carries its own address.
    placeholder = re.search(r"(?:^|::)(?:FUN|DAT)_([0-9A-Fa-f]{8})$", name)
    if placeholder:
        want = int(placeholder.group(1), 16)
        return ("ok" if got == want else "mismatch"), want
    if name in names and not overloaded(target.name, local, function_symbols(obj)):
        want = names[name]
        return ("ok" if got == want else "mismatch"), want
    return "unverified", None


OPERATORS = {"==": "8", "!=": "9", "=": "4", "[]": "A", "()": "R", "<": "M", ">": "O",
             "<=": "N", ">=": "P", "+": "H", "-": "G", "*": "D", "/": "K", "new": "2",
             "delete": "3", "->": "C", "+=": "Y", "-=": "Z"}


def mangled(name: str) -> tuple[str, ...]:
    if name.startswith("="):
        return (name[1:],)
    parts = name.split("::")
    last, scopes = parts[-1], parts[:-1]
    if last.startswith("operator"):
        rest = last[len("operator"):].strip()
        # A name after "operator" that is no operator symbol is a conversion operator (??B).
        code = OPERATORS.get(rest) or ("B" if rest[:1].isalpha() else None)
        return (f"??{code}{''.join(s + '@' for s in reversed(scopes))}@",) if code else (last,)
    if scopes and last == scopes[-1]:
        return (f"??0{last}@{''.join(s + '@' for s in reversed(scopes[:-1]))}@",)
    if last.startswith("~"):
        # The compiler also generates the scalar and vector deleting destructors from it.
        scope = "".join(s + "@" for s in reversed(scopes[:-1]))
        return tuple(f"{kind}{last[1:]}@{scope}@" for kind in ("??1", "??_G", "??_E"))
    scope = "".join(s + "@" for s in reversed(scopes))
    return (f"?{last}@{scope}@", f"_{last}@", f"@{last}@", f"_{last}")


def main() -> None:
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    argv = sys.argv[1:]
    overlay = None
    score = "--score" in argv
    if score:
        argv.remove("--score")
    if "--overlay" in argv:
        k = argv.index("--overlay")
        overlay = Path(argv[k + 1]).resolve()
        del argv[k:k + 2]
    src = Path(argv[0])
    only = {int(a, 16) for a in argv[1:]}
    targets = [(a, n) for a, n in annotations(src) if not only or a in only]
    if not targets:
        raise SystemExit(f"{src}: no annotated functions to check")
    objpath, log = compile_source(src, overlay)
    if objpath is None:
        print(log)
        raise SystemExit(1)
    raw = objpath.read_bytes()
    obj, orig, names = CoffObject(raw), Original(), known_names()
    # Which compiled function each annotation is decides what its own symbol resolves to,
    # so pick them first, by bytes alone.
    local, duplicates = bind_annotations(orig, obj, raw, src)
    for line in duplicates:
        print(line)
    failed = len(duplicates)
    for address, name in targets:
        res = compare(orig, obj, raw, address, name, names, local, score, prefer=local)
        if res.matched:
            verdict = "MATCH"
        elif res.bytes_match:
            verdict = "WRONG TARGETS"
        else:
            verdict = f"{res.ratio * 100:.1f}%"
        extra = f", {res.unverified} unverified" if res.unverified else ""
        print(f"0x{address:08X} {name.lstrip('=') or res.symbol}: {verdict} ({res.ours}/{res.size} bytes{extra}) "
              f"{res.symbol}")
        if score:
            raw_ratio = 1.0 if res.matched else res.ratio
            print(f"normalised score: {res.norm_ratio * 100:.1f}% (raw {raw_ratio * 100:.1f}%)")
        for line in res.refs:
            print(line)
        if not res.matched:
            failed += 1
            print("\n".join(res.diff[:400]))
    raise SystemExit(1 if failed else 0)


if __name__ == "__main__":
    main()
