"""Locate prebuilt library functions in gamemd.exe: data/library.csv.

Usage: uv run tools/libmatch.py [LIB ...]   (default: toolchain/vc6sp3/LIB/LIBCMT.LIB)

Every function in every member of each library is searched for in .text, with the
fields its relocations fill in left as wildcards. A function found exactly once is
recorded with its library and member. Functions under 8 bytes are skipped: too short
to place on their own.

data/library_identified.json supplements those matches with explicit identities
corroborated by incoming relocations, referenced globals, or named RTTI vtables.
library_identified.py verifies the linked archive bodies before the map is written.
Run python tools/library_identified.py to check just these entries.

The linker's import thunks (`jmp dword ptr [IAT]`) come from the import libraries too: each
run of at least THUNK_RUN of them outside a located function is recorded, named from the
short import record of the DLL's import library under toolchain/vc6sp3/LIB, or from the
exe's import name when the toolchain has no library for that DLL.
"""

import csv
import re
import struct
import sys
from pathlib import Path

import pefile

sys.path.insert(0, str(Path(__file__).resolve().parent))
from coff import CoffObject  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
MIN_SIZE = 8
THUNK_RUN = 8
LIBDIR = ROOT / "toolchain/vc6sp3/LIB"


def members(lib: bytes):
    """(member name, object bytes) for each object in an archive."""
    assert lib[:8] == b"!<arch>\n"
    at, longnames = 8, b""
    while at < len(lib):
        head = lib[at:at + 60]
        name, size = head[:16].decode("latin1").rstrip(), int(head[48:58])
        body = lib[at + 60:at + 60 + size]
        if name == "//":
            longnames = body
        elif name.startswith("/") and name[1:].isdigit():
            k = int(name[1:])
            yield longnames[k:longnames.index(b"\0", k)].decode("latin1"), body
        elif name not in ("/", ""):
            yield name.rstrip("/"), body
        at += 60 + size + (size & 1)


def short_imports(libdir: Path):
    """{(dll, import name or ordinal): (symbol, library, member)} from the short import records."""
    index = {}
    for lib in sorted(libdir.glob("*.LIB")):
        try:
            records = [(m, b) for m, b in members(lib.read_bytes()) if b[:4] == b"\0\0\xff\xff"]
        except (AssertionError, ValueError):
            continue
        for member, body in records:
            size, hint, kind = struct.unpack_from("<IHH", body, 12)
            symbol, dll = (x.decode("latin1") for x in body[20:20 + size].split(b"\0")[:2])
            if kind & 3 != 0:
                continue
            how = (kind >> 2) & 7
            name = (hint if how == 0 else symbol if how == 1 else symbol.lstrip("?@_") if how == 2
                    else symbol.lstrip("?@_").split("@")[0])
            index.setdefault((dll.lower(), name), (symbol, lib.name, member))
    return index


def import_thunks(pe, code: bytes, base: int, located: dict[int, tuple]) -> dict[int, tuple]:
    """Each run of THUNK_RUN or more import thunks outside the located functions, by address."""
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
    iat = {}
    for entry in pe.DIRECTORY_ENTRY_IMPORT:
        for imp in entry.imports:
            iat[imp.address] = (entry.dll.decode("latin1"), imp.name.decode("latin1") if imp.name else imp.ordinal)
    inside = [(a, a + v[0]) for a, v in located.items()]
    hits = [base + m.start() for m in re.finditer(rb"\xff\x25", code)
            if struct.unpack_from("<I", code, m.start() + 2)[0] in iat
            and not any(lo <= base + m.start() < hi for lo, hi in inside)]
    runs, run = [], []
    for at in hits:
        if run and at != run[-1] + 6:
            runs.append(run)
            run = []
        run.append(at)
    runs.append(run)
    index = short_imports(LIBDIR)
    found = {}
    for run in (r for r in runs if len(r) >= THUNK_RUN):
        for at in run:
            dll, name = iat[struct.unpack_from("<I", code, at - base + 2)[0]]
            symbol, lib, member = index.get((dll.lower(), name),
                                            (name if isinstance(name, str) else f"#{name}",
                                             dll.rsplit(".", 1)[0].upper() + ".LIB", dll))
            found[at] = (6, symbol, lib, member)
    return found


def pattern(code: bytes, masked: set[int]) -> bytes:
    return b"".join(b"." if k in masked else re.escape(code[k:k + 1]) for k in range(len(code)))


def main() -> None:
    libs = [Path(a) for a in sys.argv[1:]] or [ROOT / "toolchain/vc6sp3/LIB/LIBCMT.LIB"]
    pe = pefile.PE(str(ROOT / "orig/gamemd.exe"), fast_load=True)
    text = pe.sections[0]
    code = text.get_data()
    base = pe.OPTIONAL_HEADER.ImageBase + text.VirtualAddress
    found: dict[int, tuple] = {}
    ambiguous = 0
    for lib in libs:
        for member, body in members(lib.read_bytes()):
            if body[:2] != b"\x4c\x01":
                continue
            obj = CoffObject(body)
            for sym, sec, start, end in obj.functions():
                if end - start < MIN_SIZE:
                    continue
                masked = {r.offset - start + k for r in sec.relocs
                          if start <= r.offset < end for k in range(4)}
                hits = [m.start() for m in re.finditer(pattern(sec.data[start:end], masked), code, re.S)]
                if len(hits) == 1:
                    found.setdefault(base + hits[0], (end - start, sym.name, lib.name,
                                                      member.split("\\")[-1]))
                elif len(hits) > 1:
                    ambiguous += 1
    thunks = import_thunks(pe, code, base, found)
    found.update(thunks)
    identified = ROOT / "data/library_identified.json"
    if identified.exists():
        from library_identified import merge, verify
        found = merge(found, verify(identified))
    out = ROOT / "data/library.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "symbol", "library", "object"])
        for addr in sorted(found):
            w.writerow([f"0x{addr:08X}", *found[addr]])
    print(f"{len(found)} library functions -> {out} ({ambiguous} found more than once, skipped; "
          f"{len(thunks)} import thunks)")


if __name__ == "__main__":
    main()
