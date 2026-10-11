"""Compile a source file and compare its annotated functions with gamemd.exe.

    uv run tools/check.py src/rawfile.cpp            # every annotated function in the file
    uv run tools/check.py src/rawfile.cpp 0x65CF00   # one of them

A function is tied to its address by a marker on the line before its definition:

    // FUNCTION: 0x0065CF00
    int RawFileClass::Seek(int pos, int dir)

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
"""

import csv
import difflib
import re
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
FLAGS = ["/c", "/O2", "/Ob2", "/Gr", "/GR", "/MT"]
ANNOTATION = re.compile(r"^\s*//\s*FUNCTION:\s*(0x[0-9A-Fa-f]+)\s*$")
FORBIDDEN = re.compile(r"\b(__asm|_asm|__emit|_emit)\b|#\s*pragma\s+(optimize|code_seg)")
PADDING = (0x90, 0xCC)


class Original:
    def __init__(self, path: Path = ROOT / "orig/gamemd.exe"):
        self.pe = pefile.PE(str(path), fast_load=True)
        self._imports = None
        self.base = self.pe.OPTIONAL_HEADER.ImageBase
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


def winpath(p: Path) -> str:
    """The path as the compiler sees it: the checkout is drive Y: (tools/wineenv.sh)."""
    p = p.resolve()
    if p == ROOT or ROOT in p.parents:
        return "Y:\\" + "\\".join(p.relative_to(ROOT).parts)
    return "Z:" + str(p).replace("/", "\\")


def annotations(src: Path) -> list[tuple[int, str]]:
    """(address, qualified name) for each annotated definition in a source file."""
    lines = src.read_text(errors="replace").splitlines()
    out = []
    for i, line in enumerate(lines):
        m = ANNOTATION.match(line)
        if not m:
            continue
        decl = " ".join(lines[i + 1:i + 4])
        n = re.search(r"((?:\w+::)*(?:operator\s*\S+?|~?\w+))\s*\(", decl)
        out.append((int(m.group(1), 16), n.group(1).replace(" ", "") if n else ""))
    return out


def known_names() -> dict[str, int]:
    """Qualified name -> address, from data/symbols.csv and every annotated source."""
    names = {}
    with (ROOT / "data/symbols.csv").open() as f:
        for r in csv.DictReader(f):
            names.setdefault(r["name"], int(r["address"], 16))
    with (ROOT / "data/library.csv").open() as f:
        for r in csv.DictReader(f):
            names.setdefault(qualified(r["symbol"]), int(r["address"], 16))
    for src in (ROOT / "src").rglob("*.cpp") if (ROOT / "src").exists() else []:
        for addr, name in annotations(src):
            names[name] = addr
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
    return sym.lstrip("_@").split("@", 1)[0]


def compile_source(src: Path) -> tuple[Path | None, str]:
    text = src.read_text(errors="replace")
    bad = FORBIDDEN.search(text)
    if bad:
        return None, f"{src}: '{bad.group(0)}' is not allowed"
    rel = src.resolve().relative_to(ROOT) if ROOT in src.resolve().parents else Path(src.name)
    out = ROOT / "build/obj" / rel.with_suffix(".obj")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.unlink(missing_ok=True)
    cmd = [str(ROOT / "tools/wcl"), *FLAGS, f"/I{winpath(ROOT / 'include')}",
           f"/Fo{winpath(out)}", winpath(src)]
    proc = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    log = proc.stdout + proc.stderr
    return (out if proc.returncode == 0 and out.exists() else None), log


@dataclass
class Result:
    address: int
    name: str
    size: int
    ours: int
    bytes_match: bool
    refs: list[str] = field(default_factory=list)
    mismatched_refs: int = 0
    unverified: int = 0
    ratio: float = 0.0
    diff: list[str] = field(default_factory=list)

    @property
    def matched(self) -> bool:
        return self.bytes_match and self.mismatched_refs == 0


def disasm(code: bytes, va: int) -> list[str]:
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [f"{ins.mnemonic} {ins.op_str}".strip() for ins in md.disasm(code, va)]


def compare(orig: Original, obj: CoffObject, raw: bytes, address: int, name: str,
            names: dict[str, int], local: dict[str, int]) -> Result:
    prefixes = mangled(name)
    picks = [f for f in obj.functions() if f[0].name.startswith(prefixes)]
    if len(picks) != 1:
        cands = ", ".join(f[0].name for f in obj.functions())
        raise SystemExit(f"{name}: expected one compiled function, found {len(picks)} ({cands})")
    sym, sec, start, end = picks[0]
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
    res = Result(address, name, size, len(ours), same)

    for r in relocs:
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
        if status == "mismatch":
            res.mismatched_refs += 1
        if status == "unverified":
            res.unverified += 1
        if status != "ok":
            res.refs.append(f"  +0x{off:X} {qualified(target_sym.name)}: original 0x{got:08X}"
                            + (f", expected 0x{want:08X}" if want is not None else "") + f" [{status}]")

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
    return res


def check_ref(orig, obj, raw, target, sec, start, address, got, names, local):
    """Whether a linker-filled field points where the original's does."""
    if target.section == sec.number:
        want = address - start + target.value
        return ("ok" if got == want else "mismatch"), want
    name = qualified(target.name)
    if target.section > 0:
        if name in local:
            want = local[name]
            return ("ok" if got == want else "mismatch"), want
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
    if name in names:
        want = names[name]
        return ("ok" if got == want else "mismatch"), want
    return "unverified", None


def mangled(name: str) -> tuple[str, ...]:
    parts = name.split("::")
    last, scopes = parts[-1], parts[:-1]
    if scopes and last == scopes[-1]:
        return (f"??0{last}@{''.join(s + '@' for s in reversed(scopes[:-1]))}@",)
    if last.startswith("~"):
        return (f"??1{last[1:]}@{''.join(s + '@' for s in reversed(scopes[:-1]))}@",)
    scope = "".join(s + "@" for s in reversed(scopes))
    return (f"?{last}@{scope}@", f"_{last}@", f"@{last}@", f"_{last}")


def main() -> None:
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    src = Path(sys.argv[1])
    only = {int(a, 16) for a in sys.argv[2:]}
    targets = [(a, n) for a, n in annotations(src) if not only or a in only]
    if not targets:
        raise SystemExit(f"{src}: no annotated functions to check")
    objpath, log = compile_source(src)
    if objpath is None:
        print(log)
        raise SystemExit(1)
    raw = objpath.read_bytes()
    obj, orig, names = CoffObject(raw), Original(), known_names()
    local = {n: a for a, n in annotations(src)}
    failed = 0
    for address, name in targets:
        res = compare(orig, obj, raw, address, name, names, local)
        if res.matched:
            verdict = "MATCH"
        elif res.bytes_match:
            verdict = "WRONG TARGETS"
        else:
            verdict = f"{res.ratio * 100:.1f}%"
        extra = f", {res.unverified} unverified" if res.unverified else ""
        print(f"0x{address:08X} {name}: {verdict} ({res.ours}/{res.size} bytes{extra})")
        for line in res.refs:
            print(line)
        if not res.matched:
            failed += 1
            print("\n".join(res.diff[:60]))
    raise SystemExit(1 if failed else 0)


if __name__ == "__main__":
    main()
