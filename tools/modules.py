"""Assign gamemd.exe's functions to source modules: data/modules.csv.

Usage: uv run tools/modules.py [OPENTS_CODE_DIR]

The linker placed modules in case-insensitive alphabetical order of file name, so each
module is one address range and the ranges are sorted by name; the VQA library's objects
follow the game's, in their own alphabetical order, named vqalib/X. Most modules end with
the same 23-byte initialiser, which zeroes the module's own copy of a static object a
common header defines; those initialisers cut the code into segments, and a segment holds
more than one module where the modules before its last one lack the header. Evidence
naming a function's module:
  F  the function references a __FILE__ path the exe still holds (D:\\ra2mdpost\\X.cpp)
  T  tools/tier0.py found TibSun's X.cpp function at this address by its bytes and
     relocation targets (data/tier0.csv), not by its module
  O  the function's class (its YRpp name) is defined in that file in OpenTS
  V  the function is a virtual of a class (from the RTTI vtables) defined in that file
     in OpenTS; a virtual belongs to the most basic class whose vtable holds it
Each function takes the name with the most evidence (F and T count ten times O or V), and
the names kept are the heaviest set that stays in alphabetical order with no module
crossing a segment end. A module not anchored by __FILE__ whose kept evidence is one
unbroken run, with labels of one other module of its segment on both sides, is an outlier
(identical code reused, or code folded into that module) and is rejected, as are labels
out of order; both are reported. Within a segment the module changes where its kept
labels change; code between two modules' labels is written as "?", as is a segment with
no name kept.
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
WEIGHT = {"F": 10, "T": 10, "O": 1, "V": 1}
LIBRARIES = ("vqalib",)
END_MARKER = re.compile(rb"\x33\xc0\xa3....\xa3....\xa3....\xa3....\xc3", re.S)


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


def sort_key(name: str) -> tuple:
    """Link-order key of a module: the game's objects by name, then each library's."""
    lib, _, stem = name.lower().rpartition("/")
    return (LIBRARIES.index(lib) + 1 if lib else 0, stem)


def tier0_anchors() -> dict[int, str]:
    """Function -> module, from tier 0's single-address finds that did not use the layout."""
    out = {}
    path = ROOT / "data/tier0.csv"
    if not path.exists():
        return out
    with path.open() as f:
        for r in csv.DictReader(f):
            if (r["status"] in ("landed", "conflict", "failed_in_src") and ";" not in r["address"]
                    and r["tibsun_file"].endswith(".cpp") and not r["note"].startswith("placed by module")):
                out[int(r["address"], 16)] = r["tibsun_file"].rsplit(".", 1)[0]
    return out


def islands(kept, points) -> set[int]:
    """Kept labels of a module not anchored by __FILE__ whose kept evidence is one unbroken
    run with the same other module on both sides in its segment: the nearest labels there
    of modules the segment keeps. Points are (segment, key, weight, address, kinds)."""
    order = [p[3] for p in points]
    in_segment = collections.defaultdict(set)
    for p in kept:
        in_segment[p[0]].add(p[1])
    runs = []
    for p in kept:
        if runs and runs[-1][0][1] == p[1]:
            runs[-1].append(p)
        else:
            runs.append([p])
    count = collections.Counter(run[0][1] for run in runs)

    def neighbour(k, step, seg, key):
        while 0 <= k < len(points) and points[k][0] == seg:
            if points[k][1] != key and points[k][1] in in_segment[seg]:
                return points[k][1]
            k += step
        return None

    out = set()
    for run in runs:
        seg, key = run[0][0], run[0][1]
        if count[key] != 1 or any("F" in p[4] for p in run):
            continue
        before = neighbour(bisect.bisect_left(order, run[0][3]) - 1, -1, seg, key)
        if before is not None and before == neighbour(bisect.bisect_right(order, run[-1][3]), 1, seg, key):
            out |= {p[3] for p in run}
    return out


def best_monotone(points):
    """The heaviest subsequence of (segment, key, weight) whose keys never decrease, and
    strictly increase from one segment to the next: a module never spans a segment end."""
    n = len(points)
    best, prev = [0] * n, [-1] * n
    for i, (seg, key, w, *_) in enumerate(points):
        best[i] = w
        for j in range(i):
            pseg, pkey = points[j][0], points[j][1]
            ok = pkey < key or (pkey == key and pseg == seg)
            if ok and best[j] + w > best[i]:
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
    kinds = collections.defaultdict(set)
    display = {}

    def label(fn, name, kind):
        labels[fn][name.lower()] += WEIGHT[kind]
        kinds[fn, name.lower()].add(kind)
        display.setdefault(name.lower(), name)

    for fn, name in file_anchors(pe, functions).items():
        label(fn, name, "F")
    for fn, name in tier0_anchors().items():
        label(fn, name, "T")
    classes = opents_classes(code_dir) if code_dir.exists() else {}
    for fn, cls in rtti_virtuals(pe, set(functions)).items():
        stem = classes.get(cls)
        if stem:
            label(fn, stem, "V")
    with (ROOT / "data/symbols.csv").open() as f:
        for r in csv.DictReader(f):
            if r["kind"] != "function" or "::" not in r["name"]:
                continue
            stem = classes.get(r["name"].split("::")[0])
            if stem:
                label(int(r["address"], 16), stem, "O")

    text = pe.sections[0]
    t0 = pe.OPTIONAL_HEADER.ImageBase + text.VirtualAddress
    game = bisect.bisect_left(functions, library_start)
    ends = sorted(bisect.bisect_left(functions, t0 + m.start())
                  for m in END_MARKER.finditer(text.get_data()) if t0 + m.start() < library_start)
    cuts = [0] + [e + 1 for e in ends] + ([game] if not ends or ends[-1] + 1 < game else [])

    segment_of = {}
    for k, (lo, hi) in enumerate(zip(cuts, cuts[1:])):
        for fn in functions[lo:hi]:
            segment_of[fn] = k

    points = []
    for a, c in sorted(labels.items()):
        if a in segment_of:
            name, w = c.most_common(1)[0]
            points.append((segment_of[a], sort_key(name), w, a, kinds[a, name], name))
    rejected = set()
    while True:
        live = [p for p in points if p[3] not in rejected]
        kept = best_monotone(live)
        new = islands(kept, live)
        if not new:
            break
        rejected |= new
    kept_at = {p[3]: p[5] for p in kept}

    # Within a segment the module changes only where its kept labels change; leading and
    # trailing functions belong to the segment's first and last module.
    size = {int(r["address"], 16): int(r["size"]) for r in rows}
    out = ROOT / "data/modules.csv"
    named = unknown = 0
    module_at = {}
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["module", "first", "last", "functions", "certain"])

        def emit(name, fns, certain):
            for fn in fns:
                module_at[fn] = name
            last = fns[-1]
            w.writerow([name, f"0x{fns[0]:08X}", f"0x{last + size[last] - 1:08X}", len(fns),
                        "yes" if certain else "no"])

        for lo, hi in zip(cuts, cuts[1:]):
            fns = functions[lo:hi]
            marks = [(k, kept_at[fn]) for k, fn in enumerate(fns) if fn in kept_at]
            if not marks:
                emit("?", fns, False)
                unknown += len(fns)
                continue
            runs = []
            for k, key in marks:
                if runs and runs[-1][0] == key:
                    runs[-1][2] = k
                else:
                    runs.append([key, k, k])
            start = 0
            for r, (key, first, last) in enumerate(runs):
                end = len(fns) if r == len(runs) - 1 else last + 1
                if start < first and r > 0:
                    emit("?", fns[start:first], False)
                    unknown += first - start
                    start = first
                emit(display[key], fns[start:end], r == 0 or start == first)
                named += end - start
                start = end
    print(f"{len(cuts) - 1} segments -> {out}: {named} of {game} game functions named, "
          f"{unknown} not; {len(points) - len(kept)} of {len(points)} labels overruled, "
          f"{len(rejected)} of them runs inside another module")
    kept_set = set(kept_at)
    for p in points:
        if p[3] in kept_set or not p[4] & {"F", "T"}:
            continue
        why = "run inside another module" if p[3] in rejected else "out of order"
        print(f"  rejected 0x{p[3]:08X} {display[p[5]]} ({''.join(sorted(p[4]))}): {why}, "
              f"placed in {module_at.get(p[3], '?')}")


if __name__ == "__main__":
    main()
