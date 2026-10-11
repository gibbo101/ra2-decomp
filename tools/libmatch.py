"""Locate prebuilt library functions in gamemd.exe: data/library.csv.

Usage: uv run tools/libmatch.py [LIB ...]   (default: toolchain/vc6sp3/LIB/LIBCMT.LIB)

Every function in every member of each library is searched for in .text, with the
fields its relocations fill in left as wildcards. A function found exactly once is
recorded with its library and member. Functions under 8 bytes are skipped: too short
to place on their own.
"""

import csv
import re
import sys
from pathlib import Path

import pefile

sys.path.insert(0, str(Path(__file__).resolve().parent))
from coff import CoffObject  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
MIN_SIZE = 8


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
    out = ROOT / "data/library.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "symbol", "library", "object"])
        for addr in sorted(found):
            w.writerow([f"0x{addr:08X}", *found[addr]])
    print(f"{len(found)} library functions -> {out} ({ambiguous} found more than once, skipped)")


if __name__ == "__main__":
    main()
