"""Assign gamemd.exe's functions to source modules: data/modules.csv.

Usage: uv run tools/modules.py [OPENTS_CODE_DIR]

The linker placed modules in case-insensitive alphabetical order of file name, so each
module is one address range and the ranges are sorted by name. Two kinds of evidence
name a function's module:
  F  the function references a __FILE__ path the exe still holds (D:\\ra2mdpost\\X.cpp)
  O  the function's class (its YRpp name) is defined in that file in OpenTS
  V  the function is a virtual of a class (from the RTTI vtables) defined in that file
     in OpenTS; a virtual belongs to the most basic class whose vtable holds it
The labelling kept is the one that agrees with the most evidence (F counts ten times O)
while staying in alphabetical order. Functions between two modules' last and first
evidence are left unassigned and listed as gaps.
"""

import bisect
import collections
import csv
import re
import struct
import sys
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parent.parent
WEIGHT = {"F": 10, "O": 1, "V": 1}


def file_anchors(pe, functions):
    """Function -> module name, from the __FILE__ strings each function references."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    text = pe.sections[0]
    code, t0 = text.get_data(), base + text.VirtualAddress
    out = {}
    for m in re.finditer(rb"[A-Za-z]:\\[\x20-\x7e]{3,120}?\.(?:cpp|c|asm)\x00", img, re.I):
        name = m.group()[:-1].decode().rsplit("\\", 1)[-1].rsplit(".", 1)[0]
        for ref in re.finditer(re.escape(struct.pack("<I", base + m.start())), code):
            fn = functions[bisect.bisect_right(functions, t0 + ref.start()) - 1]
            out[fn] = name
    return out


def rtti_virtuals(pe, functions: set[int]) -> dict[int, str]:
    """Function -> the class that defines it, from the RTTI vtables."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()

    def u32(va):
        return struct.unpack_from("<I", img, va - base)[0]

    names = {}
    for m in re.finditer(rb"\.\?AV(\w+)@@\x00", img):
        names[base + m.start() - 8] = m.group(1).decode()
    rdata = next(s for s in pe.sections if s.Name.startswith(b".rdata"))
    r0 = base + rdata.VirtualAddress
    data = rdata.get_data()
    locators = {}
    for off in range(0, len(data) - 20, 4):
        sig, _, _, td, chd = struct.unpack_from("<IIIII", data, off)
        if sig == 0 and td in names and r0 <= chd < r0 + len(data):
            locators[r0 + off] = (names[td], u32(chd + 8))
    holders = collections.defaultdict(list)
    for off in range(0, len(data) - 4, 4):
        col = struct.unpack_from("<I", data, off)[0]
        if col not in locators:
            continue
        cls, nbases = locators[col]
        at = r0 + off + 4
        while u32(at) in functions:
            holders[u32(at)].append((nbases, cls))
            at += 4
    return {fn: min(owners)[1] for fn, owners in holders.items()}


def opents_classes(code_dir: Path):
    """Class -> the OpenTS file defining most of its methods."""
    count = collections.defaultdict(collections.Counter)
    for f in code_dir.glob("*.cpp"):
        for m in re.finditer(r"^[\w\s\*&<>:,]*?\b(\w+)::~?\w+\s*\(", f.read_text(errors="replace"), re.M):
            count[m.group(1)][f.stem] += 1
    return {c: files.most_common(1)[0][0] for c, files in count.items()}


def best_monotone(points):
    """The heaviest subsequence of (address, key, weight) whose keys never decrease."""
    n = len(points)
    best, prev = [0] * n, [-1] * n
    for i, (_, key, w) in enumerate(points):
        best[i] = w
        for j in range(i):
            if points[j][1] <= key and best[j] + w > best[i]:
                best[i], prev[i] = best[j] + w, j
    i = max(range(n), key=best.__getitem__)
    keep = []
    while i >= 0:
        keep.append(points[i])
        i = prev[i]
    return keep[::-1]


def main() -> None:
    code_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "../cnc-remastered-mods/reference/OpenTS/code"
    pe = pefile.PE(str(ROOT / "orig/gamemd.exe"), fast_load=True)
    with (ROOT / "data/functions.csv").open() as f:
        rows = list(csv.DictReader(f))
    functions = [int(r["address"], 16) for r in rows]
    with (ROOT / "data/library.csv").open() as f:
        library_start = min(int(r["address"], 16) for r in csv.DictReader(f))

    labels = collections.defaultdict(collections.Counter)
    display = {}
    for fn, name in file_anchors(pe, functions).items():
        labels[fn][name.lower()] += WEIGHT["F"]
        display[name.lower()] = name
    classes = opents_classes(code_dir) if code_dir.exists() else {}
    for fn, cls in rtti_virtuals(pe, set(functions)).items():
        stem = classes.get(cls)
        if stem:
            labels[fn][stem.lower()] += WEIGHT["V"]
            display.setdefault(stem.lower(), stem)
    with (ROOT / "data/symbols.csv").open() as f:
        for r in csv.DictReader(f):
            if r["kind"] != "function" or "::" not in r["name"]:
                continue
            stem = classes.get(r["name"].split("::")[0])
            if stem:
                labels[int(r["address"], 16)][stem.lower()] += WEIGHT["O"]
                display.setdefault(stem.lower(), stem)

    points = [(a, c.most_common(1)[0][0], c.most_common(1)[0][1]) for a, c in sorted(labels.items())
              if a < library_start]
    kept = best_monotone(points)
    module_of = {a: key for a, key, _ in kept}

    spans = []
    for a, key, _ in kept:
        if spans and spans[-1][0] == key:
            spans[-1][2] = a
            spans[-1][3] += 1
        else:
            spans.append([key, a, a, 1])

    size = {int(r["address"], 16): int(r["size"]) for r in rows}
    game = bisect.bisect_left(functions, library_start)
    out = ROOT / "data/modules.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["module", "first", "last", "evidence", "functions"])

        def gap(lo: int, hi: int) -> None:
            """Functions [lo, hi) of the map that no module's evidence covers."""
            if hi > lo:
                last = functions[hi - 1]
                w.writerow(["?", f"0x{functions[lo]:08X}", f"0x{last + size[last] - 1:08X}", 0, hi - lo])

        nxt = 0
        for key, first, last, n in spans:
            lo, hi = bisect.bisect_left(functions, first), bisect.bisect_right(functions, last)
            gap(nxt, lo)
            w.writerow([display[key], f"0x{first:08X}", f"0x{last + size[last] - 1:08X}", n, hi - lo])
            nxt = hi
        gap(nxt, game)
    dropped = len(points) - len(kept)
    assigned = sum(bisect.bisect_right(functions, s[2]) - bisect.bisect_left(functions, s[1]) for s in spans)
    print(f"{len(spans)} modules -> {out}: {assigned} of {game} game functions inside a module's "
          f"evidence span; {dropped} of {len(points)} labels overruled by the order")


if __name__ == "__main__":
    main()
