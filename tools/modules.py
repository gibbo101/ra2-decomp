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
     relocation targets (data/tier0.csv), not by its module; or a static initialiser of
     TibSun's X.obj, which is private to its object
  O  the function's class (its YRpp name) is defined in that file in OpenTS
  V  the function is a virtual of a class (from the RTTI vtables) defined in that file
     in OpenTS; a virtual belongs to the most basic class whose vtable holds it
Each function takes the name with the most evidence (F and T count ten times O or V), and
the names kept are the heaviest set that stays in alphabetical order with no module
crossing a segment end. A module not anchored by __FILE__ whose kept evidence is one
unbroken run, with labels of one other module of its segment on both sides, is an outlier
(identical code reused, or code folded into that module) and is rejected, as are labels
out of order; both are reported. Within a segment the module changes where its kept
labels change, and a function landed in a neighbouring module's src/ file
(data/progress.csv) extends that module.

Code that leaves unnamed takes weaker evidence, counted like O or V, where the name sorts
strictly between the kept labels around it and no module or src/ file has it yet:
  Y  the function's class (RTTI or YRpp name) has no OpenTS file; the name is made from
     the class name (FooClass foo, FooTypeClass footype, FooLocomotionClass foo), and two
     functions there must agree on it
  L  a TibSun file shares a distinctive string literal or function name with the function,
     defines its class, or gave tier 0 a find there
Modules named only by Y or L are not certain. An unnamed range then takes the one TibSun
file sorting between its named neighbours that it has evidence for, or, as a whole
segment alone between them with no class OpenTS lacks, the one TibSun file sorting there
at all. A game range still unnamed takes the provisional name seg_<first address>; the
VQA library's unnamed ranges stay "?".
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
WEIGHT = {"F": 10, "T": 10, "O": 1, "V": 1, "Y": 1, "L": 1}
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
    """Function -> module, from tier 0's single-address finds that did not use the layout:
    TibSun functions, and static initialisers, which are private to the object they are in."""
    out = {}
    path = ROOT / "data/tier0.csv"
    if not path.exists():
        return out
    with path.open() as f:
        for r in csv.DictReader(f):
            if ";" in r["address"] or r["note"].startswith("placed by module"):
                continue
            src = r["tibsun_file"]
            if r["status"] in ("landed", "conflict", "failed_in_src") and src.endswith(".cpp"):
                out[int(r["address"], 16)] = src.rsplit(".", 1)[0]
            elif r["status"] == "not_in_cpp" and r["symbol"].startswith("_$E") and src.endswith(".obj)"):
                stem = src[1:].rsplit(".", 1)[0]
                lib = next((lib for lib in LIBRARIES if stem.startswith(lib + "_")), None)
                out[int(r["address"], 16)] = f"{lib}/{stem[len(lib) + 1:]}" if lib else stem
    return out


def landed_modules() -> dict[int, str]:
    """Function -> the module whose src/ file holds it (data/progress.csv), lowercase."""
    out = {}
    path = ROOT / "data/progress.csv"
    if path.exists():
        with path.open() as f:
            for r in csv.DictReader(f):
                if r["source"].startswith("src/"):
                    out[int(r["address"], 16)] = r["source"][4:].rsplit(".", 1)[0].lower()
    return out


def yr_stems(cls: str) -> list[str]:
    """Candidate file names for a class OpenTS lacks, Westwood's way first: FooClass is
    foo, FooTypeClass footype, FooLocomotionClass foo."""
    stem = cls[:-5] if cls.endswith("Class") and len(cls) > 5 else cls
    out = [stem[:-10]] if stem.endswith("Locomotion") and len(stem) > 10 else []
    return [s.lower() for s in out + [stem]]


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


def tibsun_index(code_dir: Path):
    """TibSun's files, with the distinctive string literals and function names each defines
    (names lowercased without underscores, as YRpp spells TibSun's) and its classes."""
    literals = collections.defaultdict(set)
    names = collections.defaultdict(set)
    files = sorted(f.stem.lower() for f in code_dir.glob("*.cpp"))
    for f in code_dir.glob("*.cpp"):
        text = f.read_text(errors="replace")
        for m in re.finditer(r'"((?:[^"\\\n]|\\.){4,})"', text):
            literals[m.group(1).replace("\\\\", "\\")].add(f.stem.lower())
        for m in re.finditer(r"^\w[\w\s\*&<>:,]*?\b((?:\w+::)?~?\w+)\s*\([^;]*$", text, re.M):
            names[m.group(1).replace("_", "").lower()].add(f.stem.lower())
    literals = {k: v for k, v in literals.items() if len(v) <= 3}
    names = {k: v for k, v in names.items() if len(v) <= 2}
    return files, literals, names, opents_classes(code_dir)


def referenced_strings(img: bytes, base: int, start: int, size: int) -> set[str]:
    """The NUL-terminated ASCII strings outside the code whose start a function's bytes hold."""
    out = set()
    body = img[start - base:start - base + size]
    for i in range(len(body) - 3):
        o = struct.unpack_from("<I", body, i)[0] - base
        if 0x1000 < o < len(img) and img[o - 1] == 0:
            m = re.match(rb"[\x20-\x7e]{4,120}\x00", img[o:o + 121])
            if m:
                out.add(m.group()[:-1].decode())
    return out


def tibsun_files(ranges, index, evidence, contrary, used):
    """{range index: (file, how)}: the one TibSun file between a range's named neighbours
    it has evidence for, else the only one there for a lone whole segment with no YR class."""
    files = index[0]
    named = {}
    while True:
        for k, rng in enumerate(ranges):
            if rng[0] != "?" or k in named:
                continue
            lo = next((j for j in range(k - 1, -1, -1) if ranges[j][0] != "?" or j in named), None)
            hi = next((j for j in range(k + 1, len(ranges)) if ranges[j][0] != "?" or j in named), None)
            if lo is None or hi is None:
                continue
            lo_key, hi_key = (sort_key(named[j][0] if j in named else ranges[j][0]) for j in (lo, hi))
            cands = [f for f in files if lo_key < sort_key(f) < hi_key and f not in used]
            backed = [f for f in cands if evidence(rng[1], f)]
            if len(backed) == 1:
                named[k] = (backed[0], "evidence")
                used.add(backed[0])
                break
            if len(cands) == 1 and hi - lo == 2 and rng[4] and not backed and not contrary(rng[1]):
                named[k] = (cands[0], "order")
                used.add(cands[0])
                break
        else:
            return named


def assign(labels, kinds, segment_of):
    """Each labelled function's point (segment, key, weight, address, kinds, name), the
    kept points, and the addresses of rejected runs inside another module."""
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
    return points, kept, rejected


def around(kept, fn):
    """The link-order keys of the kept labels before and after a function."""
    k = bisect.bisect_left([p[3] for p in kept], fn)
    return (kept[k - 1][1] if k else (-1, ""),
            kept[k][1] if k < len(kept) else (len(LIBRARIES) + 1, ""))


def yr_labels(kept, yr_classes, labels):
    """Function -> the file name made from its class that sorts strictly between the kept
    labels around it, where at least two functions there agree on it."""
    picks = {}
    for fn, names in yr_classes.items():
        if fn in labels:
            continue
        lo, hi = around(kept, fn)
        stem = next((s for c in names for s in yr_stems(c) if lo < sort_key(s) < hi), None)
        if stem:
            picks[fn] = (lo, hi, stem)
    support = collections.Counter(picks.values())
    return {fn: pick[2] for fn, pick in picks.items() if support[pick] > 1}


def build_ranges(cuts, functions, kept_at, kinds, display, landed):
    """[module, functions, certain, how, whole segment] per range; leading and trailing
    functions of a segment belong to its first and last module."""
    ranges = []
    for lo, hi in zip(cuts, cuts[1:]):
        fns = functions[lo:hi]
        marks = [(k, kept_at[fn]) for k, fn in enumerate(fns) if fn in kept_at]
        if not marks:
            ranges.append(["?", fns, False, "", True])
            continue
        # A function landed in a neighbouring module's src/ file extends that module.
        at = [k for k, _ in marks]
        extra = []
        for k, fn in enumerate(fns):
            if fn in kept_at or fn not in landed:
                continue
            j = bisect.bisect_left(at, k)
            near = [marks[i][1] for i in (j - 1, j) if 0 <= i < len(marks)]
            extra += [(k, m) for m in near if m.lower() == landed[fn]][:1]
        runs = []
        for k, key in sorted(marks + extra):
            if runs and runs[-1][0] == key:
                runs[-1][2] = k
            else:
                runs.append([key, k, k])
        start = 0
        for r, (key, first, last) in enumerate(runs):
            end = len(fns) if r == len(runs) - 1 else last + 1
            if start < first and r > 0:
                ranges.append(["?", fns[start:first], False, "", False])
                start = first
            got = set().union(*(kinds[fn, key.lower()] for fn in fns[start:end]
                                 if kept_at.get(fn) == key))
            weak = "tibsun" if got <= {"Y", "L"} and "L" in got else "class" if got == {"Y"} else ""
            certain = (r == 0 or start == first) and not weak
            ranges.append([display[key], fns[start:end], certain, weak, False])
            start = end
    return ranges


def main() -> None:
    if len(sys.argv) > 1 and (sys.argv[1].startswith("-") or not Path(sys.argv[1]).is_dir()):
        raise SystemExit(__doc__)
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
    yr_classes = collections.defaultdict(list)
    for fn, cls in rtti_virtuals(pe, set(functions)).items():
        stem = classes.get(cls)
        if stem:
            label(fn, stem, "V")
        else:
            yr_classes[fn].append(cls)
    with (ROOT / "data/symbols.csv").open() as f:
        for r in csv.DictReader(f):
            if r["kind"] != "function" or "::" not in r["name"]:
                continue
            cls = r["name"].split("::")[0]
            stem = classes.get(cls)
            if stem:
                label(int(r["address"], 16), stem, "O")
            elif cls not in yr_classes[int(r["address"], 16)]:
                yr_classes[int(r["address"], 16)].append(cls)

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

    landed = landed_modules()
    index = tibsun_index(ROOT / "reference/tibsun/code")
    img, base = pe.get_memory_mapped_image(), pe.OPTIONAL_HEADER.ImageBase
    size = {int(r["address"], 16): int(r["size"]) for r in rows}
    fn_names = collections.defaultdict(set)
    for name in ("symbols.csv", "names.csv"):
        with (ROOT / "data" / name).open() as f:
            for r in csv.DictReader(f):
                fn_names[int(r["address"], 16)].add(r["name"])
    tier0_files = collections.defaultdict(set)
    with (ROOT / "data/tier0.csv").open() as f:
        for r in csv.DictReader(f):
            if ";" not in r["address"]:
                stem = r["tibsun_file"].strip("()").rsplit(".", 1)[0].lower()
                tier0_files[int(r["address"], 16)].add(stem)
    tie_cache = {}

    def ties(fn):
        """TibSun file -> what ties this function to it: the file's distinctive literals or
        function names, its class, or a tier 0 find."""
        if fn not in tie_cache:
            out = collections.defaultdict(list)
            for text in referenced_strings(img, base, fn, size[fn]):
                for stem in index[1].get(text, ()):
                    out[stem].append(f"string {text!r}")
            for name in fn_names.get(fn, ()):
                for stem in index[2].get(name.replace("_", "").lower(), ()):
                    out[stem].append(name)
                if "::" in name and name.split("::")[0] in index[3]:
                    out[index[3][name.split("::")[0]].lower()].append(name)
            for stem in tier0_files.get(fn, ()):
                out[stem].append(f"tier 0 0x{fn:08X}")
            tie_cache[fn] = out
        return tie_cache[fn]

    points, kept, rejected = assign(labels, kinds, segment_of)
    ranges = build_ranges(cuts, functions, {p[3]: p[5] for p in kept}, kinds, display, landed)
    used = {r[0].lower() for r in ranges if r[0] != "?"}
    src = ROOT / "src"
    used |= {p.relative_to(src).with_suffix("").as_posix().lower() for p in src.rglob("*.cpp")}
    unknown = {fn for r in ranges if r[0] == "?" for fn in r[1]}
    yr = {fn: c for fn, c in yr_classes.items() if fn in unknown}
    for fn, stem in yr_labels(kept, yr, labels).items():
        label(fn, stem, "Y")
    for fn in sorted(unknown - set(labels)):
        lo, hi = around(kept, fn)
        files = [f for f in ties(fn) if lo < sort_key(f) < hi and f not in used]
        if len(files) == 1:
            label(fn, files[0], "L")
    points, kept, rejected = assign(labels, kinds, segment_of)
    kept_at = {p[3]: p[5] for p in kept}
    ranges = build_ranges(cuts, functions, kept_at, kinds, display, landed)

    def evidence(fns, stem):
        return [why for fn in fns for why in ties(fn).get(stem, ())]

    def contrary(fns):
        return any(yr_classes.get(fn) and not labels.get(fn) for fn in fns)

    used |= {r[0].lower() for r in ranges if r[0] != "?"}
    for k, (stem, how) in tibsun_files(ranges, index, evidence, contrary, used).items():
        ranges[k][0], ranges[k][3] = stem, how
    for name, fns, _, how, _ in ranges:
        if how in ("tibsun", "evidence", "order"):
            why = sorted(set(evidence(fns, name.lower())))
            print(f"  {name} 0x{fns[0]:08X} ({len(fns)}): by {how}"
                  + (f": {', '.join(why)[:160]}" if why else ""))

    # A range still unnamed in the game's own code takes a provisional name from its address.
    libraries = min((r[1][0] for r in ranges if sort_key(r[0])[0]), default=library_start)
    for rng in ranges:
        if rng[0] == "?" and rng[1][0] < libraries:
            rng[0], rng[3] = f"seg_{rng[1][0]:08X}", "seg"

    out = ROOT / "data/modules.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["module", "first", "last", "functions", "certain"])
        for name, fns, certain, *_ in ranges:
            last = fns[-1]
            w.writerow([name, f"0x{fns[0]:08X}", f"0x{last + size[last] - 1:08X}", len(fns),
                        "yes" if certain else "no"])
    module_at = {fn: name for name, fns, *_ in ranges for fn in fns}
    named = sum(len(r[1]) for r in ranges if r[0] != "?" and r[3] != "seg")
    how = collections.Counter()
    for r in ranges:
        how[r[3]] += len(r[1])
    print(f"{len(cuts) - 1} segments -> {out}: {named} of {game} game functions named by evidence"
          f" ({how['class']} by class names only, {how['tibsun'] + how['evidence'] + how['order']}"
          f" by TibSun files),"
          f" {how['seg']} provisional (seg_), "
          f"{sum(len(r[1]) for r in ranges if r[0] == '?')} not; {len(points) - len(kept)} of "
          f"{len(points)} labels overruled, {len(rejected)} of them runs inside another module")
    kept_set = set(kept_at)
    for p in points:
        if p[3] in kept_set or not p[4] & {"F", "T"}:
            continue
        why = "run inside another module" if p[3] in rejected else "out of order"
        print(f"  rejected 0x{p[3]:08X} {display[p[5]]} ({''.join(sorted(p[4]))}): {why}, "
              f"placed in {module_at.get(p[3], '?')}")


if __name__ == "__main__":
    main()
