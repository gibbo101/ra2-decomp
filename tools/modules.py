"""Assign gamemd.exe's functions to source modules: data/modules.csv.

Usage: uv run tools/modules.py [OPENTS_CODE_DIR]

The linker placed modules in case-insensitive alphabetical order of file name, so each
module is one address range and the ranges are sorted by name; the VQA library's objects
follow the game's, in their own alphabetical order, named vqalib/X. Common headers define
static objects, so every module including them begins with the initialisers of its own
copies: a 23-byte one zeroing a 16-byte object, a run of 13 from a second header, or both
side by side. Each such set starts a segment, and a segment holds more than one module where
the modules after its first lack those headers. Data is laid out in module order too, so a
23-byte initialiser whose object lies among those of the initialisers before it, not after,
is the end of a module and starts no segment. Evidence naming a function's module:
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
strictly between the kept labels around it and no module, nor src/ file holding none of
its range, has it yet; Y and L may also name the module of a kept label beside it in its
segment, and G either module around it:
  Y  the function's class (RTTI, YRpp or data/names.csv name, the class whose pointers
     a static vector it builds holds, the class of a vtable only code of its segment holds,
     or, in a segment naming no other class, the class whose pointers the vector template
     holding it is over) has no OpenTS file; the name is made from the class name (FooClass
     foo, FooTypeClass footype, FooLocomotionClass foo), and two functions there must agree
     on it unless it names a neighbour
  L  a TibSun file shares a distinctive string literal (or one of a table of strings the
     function holds) or function name with the function, defines its class (or that of an
     RTTI vtable holding it) or a file-scope object it initialises, or gave tier 0 a find
     there
  G  a static initialiser (one that registers an atexit function), or the function it
     registers, builds at a fixed address an object of a type that only one TibSun file
     defines a file-scope object of
Then code still unnamed takes the name of a module beside it in its segment:
  B  a static initialiser whose object lies in memory among the objects of that module's
     initialisers
  C  code the module before it instantiated, which the linker keeps with its first user:
     a virtual that a vtable referenced by the module's code holds (unless the class's own
     file sorts in between), or a class template instance the module calls
G, B and C replace labels of a function that were all overruled. Code no TibSun file
claims below then takes the name of a module beside it in its segment that alone uses it:
  U  every function calling it or holding its address is in that module, and no data
     holds its address
  D  else it holds a data address that, of other code, only that module's holds (an
     address just past another the same code holds may end that object, and is skipped)
Modules named only by Y, L or G (with B, C, U or D) are not certain. An unnamed range
then takes the one TibSun file sorting between its named neighbours that it has evidence
for, or, as a whole segment alone between them with no class OpenTS lacks and no object
whose type TibSun defines elsewhere, the one TibSun file sorting there at all. A game
range still unnamed takes the name of the one src/ file its landed functions are in, if
no module has it, else the provisional name seg_<first address>; the VQA library's
unnamed ranges stay "?".
"""

import bisect
import collections
import csv
import re
import struct
import sys
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parent.parent
WEIGHT = {"F": 10, "T": 10, "O": 1, "V": 1, "Y": 1, "L": 1, "G": 1, "C": 1, "B": 1, "U": 1, "D": 1}
LIBRARIES = ("vqalib",)
END_MARKER = re.compile(rb"\x33\xc0\xa3....\xa3....\xa3....\xa3....\xc3", re.S)
HEADER_MARKER = re.compile(rb"\xdd\x05....\xdd\x05....\xe8....\xdc\xc0\x83\xec\x08\xdd\x1c\x24"
                           rb"\xe8....\xdd\x1d....\x83\xc4\x08\xc3", re.S)
HEADER_RUN = 13
ATEXIT = 0x007C978A


def initialiser_statics(pe, functions, size) -> dict[int, set[int]]:
    """Each static initialiser (the C++ initialiser table lists them) -> the addresses in .data
    it writes or loads into a register: the objects it builds."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    fset = set(functions)
    data = next(s for s in pe.sections if s.Name.startswith(b".data"))
    words = struct.unpack_from(f"<{data.SizeOfRawData // 4}I", data.get_data())
    best, run = [], []
    for w in words:
        if w in fset and (not run or w > run[-1]):
            run.append(w)
        elif w:
            best, run = max(best, run, key=len), []
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    lo = base + data.VirtualAddress
    hi = lo + data.Misc_VirtualSize
    out = {}
    for fn in max(best, run, key=len):
        written = set()
        for i in md.disasm(img[fn - base:fn - base + size[fn]], fn):
            ops = i.operands
            stores = i.mnemonic.startswith(("mov", "fst", "fist", "and", "or", "inc"))
            if stores and ops and ops[0].type == capstone.x86.X86_OP_MEM and ops[0].mem.base == 0 \
                    and ops[0].mem.index == 0:
                written.add(ops[0].mem.disp & 0xFFFFFFFF)
            elif i.mnemonic == "mov" and len(ops) == 2 and ops[0].type == capstone.x86.X86_OP_REG \
                    and ops[1].type == capstone.x86.X86_OP_IMM:
                written.add(ops[1].imm & 0xFFFFFFFF)
        out[fn] = {a for a in written if lo <= a < hi}
    return out


def module_starts(pe, functions, statics, game) -> list[int]:
    """Indexes in functions that start a segment, then game. Each set of header initialisers
    (a run of 13 with up to one 23-byte one before it and two after, or a 23-byte one alone)
    begins a module. A 23-byte one goes with the initialisers whose objects lie either side
    of its own in memory."""
    text = pe.sections[0]
    t0 = pe.OPTIONAL_HEADER.ImageBase + text.VirtualAddress
    code = text.get_data()

    def at(marker):
        found = (bisect.bisect_left(functions, t0 + m.start()) for m in marker.finditer(code))
        return {k for k in found if k < game}

    ends, runs = at(END_MARKER), at(HEADER_MARKER)
    writer = {a: fn for fn, written in statics.items() for a in written}
    order = sorted(writer)

    def side(k):
        """Where the initialisers of the objects either side of this one's in memory lie."""
        own = statics.get(functions[k], set())
        if not own:
            return set()
        j = bisect.bisect_left(order, min(own))
        near = [next((order[x] for x in steps if order[x] not in own), None)
                for steps in (range(j - 1, -1, -1), range(j, len(order)))]
        return {"before" if writer[a] < functions[k] else "after" for a in near if a is not None}

    def run_before(k):
        """The first index of a header run ending just before this one, or one 23-byte
        initialiser before it."""
        return next((j for j in (k - HEADER_RUN, k - HEADER_RUN - 1) if j in runs and
                     all(x in ends for x in range(j + HEADER_RUN, k))), None)

    def begins(k):
        j = run_before(k)
        if j is None:
            return side(k) != {"before"}
        run = set().union(*(statics.get(functions[x], set()) for x in range(j, j + HEADER_RUN)))
        own = statics.get(functions[k], set())
        inside = bool(run and own) and min(run) < min(own) < max(run)
        return side(k) == {"after"} and not inside

    starts = {k for k in ends if begins(k)}
    starts |= {k for k in runs if k - 1 not in starts}
    return sorted(starts | {0}) + ([game] if game not in starts else [])


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


def vtable_slots(pe, functions):
    """({function: the vtables holding it}, {vtable: its class's RTTI name})."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    fset = set(functions)

    def u32(va):
        return struct.unpack_from("<I", img, va - base)[0]

    names = {base + m.start() - 8: m.group(1).decode(errors="replace")
             for m in re.finditer(rb"\.\?A[VU]([^\x00]+)\x00", img)}
    rdata = next(s for s in pe.sections if s.Name.startswith(b".rdata"))
    r0, data = base + rdata.VirtualAddress, rdata.get_data()
    held, owner = collections.defaultdict(set), {}
    for off in range(0, len(data) - 20, 4):
        col = struct.unpack_from("<I", data, off)[0]
        if not r0 <= col < r0 + len(data) - 16 or u32(col) != 0 or u32(col + 12) not in names:
            continue
        at = r0 + off + 4
        owner[at] = names[u32(col + 12)]
        while at < r0 + len(data) and u32(at) in fset:
            held[u32(at)].add(r0 + off + 4)
            at += 4
    return held, owner


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


def type_key(text: str, typedefs=None) -> tuple:
    """A class type as (name, classes among its template arguments), from source text
    (`DynamicVectorClass<MFCD *>`) or an RTTI name (`.?AV?$DynamicVectorClass@PAVMixFileClass@@@@`)."""
    typedefs = typedefs or {}
    if text.startswith(".?A"):
        body = text[4:]
        if not body.startswith("?$"):
            return (body.split("@")[0], frozenset())
        name = body[2:body.index("@")]
        return (name, frozenset(re.findall(r"(?:[VU]|\?\$)([A-Za-z_]\w*)@", body[len(name) + 3:])))
    m = re.match(r"(\w+)\s*<(.*)>$", text.strip())
    if not m:
        return (re.match(r"\w+", typedefs.get(text.strip(), text.strip())).group(), frozenset())
    args = (re.match(r"\w+", typedefs.get(i, i)).group() for i in re.findall(r"[A-Za-z_]\w*", m.group(2))
            if i not in ("const", "volatile", "unsigned", "signed", "struct", "class"))
    return (m.group(1), frozenset(args))


def tibsun_globals(code_dir: Path) -> tuple[dict[tuple, set[str]], dict[str, set[str]]]:
    """(type key -> the TibSun files defining a file-scope object of that type, object name ->
    the TibSun files defining it)."""
    typedefs = {}
    for f in code_dir.glob("*.h*"):
        for m in re.finditer(r"^typedef\s+([\w<>\s,\*:]+?)\s+(\w+)\s*;", f.read_text(errors="replace"), re.M):
            typedefs[m.group(2)] = m.group(1).strip()
    found = re.compile(r"^(?:static\s+)?(?:const\s+)?([A-Za-z_]\w*(?:\s*<[^;()]*>)?)\s+([A-Za-z_][\w:]*)"
                       r"\s*(?:\[[^\]]*\]\s*)?(?:;|=|\(([^;]*)\)\s*;)", re.M)
    out = collections.defaultdict(set)
    named = collections.defaultdict(set)
    for f in code_dir.glob("*.cpp"):
        for m in found.finditer(f.read_text(errors="replace")):
            typ, name, args = m.groups()
            # A parameter list (a type then a name) makes it a prototype, not an object.
            if typ in ("return", "typedef", "using", "class", "struct", "enum", "extern", "friend",
                       "delete", "goto", "const", "static") or (args and re.search(r"\w[\s\*&]+\w", args)):
                continue
            out[type_key(typ, typedefs)].add(f.stem.lower())
            named[name].add(f.stem.lower())
    return out, named


def initialiser_types(pe, functions, size, names) -> dict[int, set[tuple]]:
    """Static initialiser (it registers an atexit function), and the function it registers ->
    the type keys of the objects it builds at fixed addresses: vtables stored there, and
    named constructors called with one in ecx."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    fset = set(functions)

    def code(fn):
        return list(md.disasm(img[fn - base:fn - base + size[fn]], fn)) if size[fn] < 400 else []

    def u32(va):
        return struct.unpack_from("<I", img, va - base)[0] if base <= va < base + len(img) - 4 else 0

    def rtti(vt):
        td = u32(u32(vt - 4) + 12) + 8
        text = img[td - base:td - base + 400] if td > base else b""
        return text[:text.find(0)].decode(errors="replace") if text.startswith(b".?A") else None

    def imm(op):
        return op.imm & 0xFFFFFFFF if op.type == capstone.x86.X86_OP_IMM else None

    def calls_atexit(fn):
        body = img[fn - base:fn - base + size[fn]]
        return any(body[k] in (0xE8, 0xE9) and fn + k + 5 + struct.unpack_from("<i", body, k + 1)[0] == ATEXIT
                   for k in range(len(body) - 4))

    out = {}
    for fn in functions:
        if size[fn] >= 400 or not calls_atexit(fn):
            continue
        types, this, registered = set(), None, []
        for i in code(fn):
            ops = i.operands
            if i.mnemonic == "push" and imm(ops[0]) in fset:
                registered.append(imm(ops[0]))
            elif i.mnemonic == "mov" and len(ops) == 2 and ops[0].type == capstone.x86.X86_OP_REG:
                this = imm(ops[1]) if i.reg_name(ops[0].reg) == "ecx" else this
            elif i.mnemonic == "mov" and len(ops) == 2 and ops[0].type == capstone.x86.X86_OP_MEM \
                    and ops[0].mem.base == 0 and imm(ops[1]) and rtti(imm(ops[1])):
                types.add(type_key(rtti(imm(ops[1]))))
            elif i.mnemonic in ("call", "jmp") and this and imm(ops[0]) is not None:
                for name in names.get(imm(ops[0]), ()):
                    m = re.match(r"(?:\w+::)*?(\w+)::~?(\w+)$", name)
                    if m and m.group(1) == m.group(2):
                        types.add((m.group(1), frozenset()))
        if types:
            for f in [fn] + registered:
                out.setdefault(f, set()).update(types)
    return out


def referenced_strings(img: bytes, base: int, start: int, size: int) -> set[str]:
    """The NUL-terminated ASCII strings outside the code whose start a function's bytes hold,
    or that a table of two or more string addresses at an address its bytes hold lists."""

    def text(o):
        if not 0x1000 < o < len(img) or img[o - 1] != 0:
            return None
        m = re.match(rb"[\x20-\x7e]{3,120}\x00", img[o:o + 121])
        return m.group()[:-1].decode() if m else None

    out = set()
    body = img[start - base:start - base + size]
    for i in range(len(body) - 3):
        o = struct.unpack_from("<I", body, i)[0] - base
        if text(o) and len(text(o)) >= 4:
            out.add(text(o))
        elif 0x1000 < o < len(img) - 8:
            table = []
            while o + 4 * len(table) < len(img) - 4 and len(table) < 64:
                entry = text(struct.unpack_from("<I", img, o + 4 * len(table))[0] - base)
                if entry is None:
                    break
                table.append(entry)
            out |= set(table) if len(table) > 1 else set()
    return out


def tibsun_files(ranges, index, evidence, contrary, taken):
    """{range index: (file, how)}: the one TibSun file between a range's named neighbours
    it has evidence for, else the only one there for a lone whole segment with no YR class.
    taken(file, functions) says another module or src/ file has the name."""
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
            cands = [f for f in files if lo_key < sort_key(f) < hi_key and not taken(f, rng[1])
                     and all(f != n[0] for n in named.values())]
            backed = [f for f in cands if evidence(rng[1], f)]
            if len(backed) == 1:
                named[k] = (backed[0], "evidence")
                break
            if len(cands) == 1 and hi - lo == 2 and rng[4] and not backed and not contrary(rng[1]):
                named[k] = (cands[0], "order")
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
            weak = ("tibsun" if got <= {"Y", "L", "G", "B", "C", "U", "D"} and got & {"L", "G"}
                    else "class" if got <= {"Y", "B", "C", "U", "D"} and "Y" in got else "")
            certain = (r == 0 or start == first) and not weak
            ranges.append([display[key], fns[start:end], certain, weak, False])
            start = end
    return ranges


def neighbour_statics(statics, kept, kept_at, module_at, segment_of) -> dict[int, str]:
    """Unnamed static initialiser -> the module beside it in its segment whose initialisers'
    objects lie either side of its own in memory (data is laid out in module order)."""
    writer = {a: fn for fn, written in statics.items() for a in written}
    order = sorted(writer)
    at = [p[3] for p in kept]
    placed = dict(module_at)
    out = {}
    pending = sorted(set(statics) - set(kept_at) - set(module_at))
    while pending:
        left = []
        for fn in pending:
            k, own = bisect.bisect_left(at, fn), statics[fn]
            if not own or not k or k == len(kept) or segment_of.get(fn) is None:
                continue
            j = bisect.bisect_left(order, min(own))
            near = [next((order[x] for x in steps if order[x] not in own and writer[order[x]] in placed), None)
                    for steps in (range(j - 1, -1, -1), range(j, len(order)))]
            mods = {placed[writer[a]] if a is not None else None for a in near}
            sides = {p[5] for p in (kept[k - 1], kept[k]) if p[0] == segment_of[fn]}
            if len(mods) == 1 and mods <= sides:
                out[fn] = placed[fn] = mods.pop()
            else:
                left.append(fn)
        pending = left if len(left) < len(pending) else []
    return out


def instantiated(pe, size, slots, templates, classes, kept, module_at, segment_of) -> dict[int, str]:
    """Unnamed code after a kept module in its segment -> that module, where the module's code
    references a vtable holding it or calls it as a class template instance."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    held, owner = slots
    members = collections.defaultdict(list)
    for fn, name in module_at.items():
        members[name].append(fn)
    cache = {}

    def used(fn):
        """The vtables a function's bytes hold and the functions it calls directly."""
        if fn not in cache:
            body = img[fn - base:fn - base + size[fn]]
            words = [struct.unpack_from("<I", body, k)[0] for k in range(len(body) - 3)]
            cache[fn] = ({v for v in words if v in owner},
                         {fn + k + 5 + struct.unpack_from("<i", body, k + 1)[0]
                          for k in range(len(body) - 4) if body[k] == 0xE8})
        return cache[fn]

    at = [p[3] for p in kept]
    out = {}
    for fn in sorted((set(held) | templates) - set(at) - set(module_at)):
        k = bisect.bisect_left(at, fn)
        if not k or kept[k - 1][0] != segment_of.get(fn):
            continue
        mod, lo = kept[k - 1][5], kept[k - 1][1]
        hi = kept[k][1] if k < len(kept) else (len(LIBRARIES) + 1, "")
        homes = [s for vt in held.get(fn, ()) if not owner[vt].startswith("?$")
                 for c in [owner[vt].split("@")[0]] for s in ([classes[c]] if c in classes else yr_stems(c))]
        if any(lo < sort_key(s) < hi for s in homes):
            continue
        if any(held.get(fn, set()) & used(u)[0] or fn in templates and fn in used(u)[1] for u in members[mod]):
            out[fn] = mod
    return out


def references(pe, functions, size):
    """({function: the functions whose code calls it or holds its address}, the functions
    whose address data outside the code holds, {function: the data addresses its code holds})."""
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    fset = set(functions)
    data = [s for s in pe.sections[1:] if s.Name.startswith((b".rdata", b".data"))]
    lo = min(base + s.VirtualAddress for s in data)
    hi = max(base + s.VirtualAddress + s.Misc_VirtualSize for s in data)
    users = collections.defaultdict(set)
    reads = collections.defaultdict(set)
    for fn in functions:
        body = img[fn - base:fn - base + size[fn]]
        for k in range(len(body) - 3):
            word = struct.unpack_from("<I", body, k)[0]
            near = fn + k + 4 + struct.unpack_from("<i", body, k)[0] if k and body[k - 1] in (0xE8, 0xE9) else None
            for target in (word, near):
                if target in fset and target != fn:
                    users[target].add(fn)
            if lo <= word < hi:
                reads[fn].add(word)
    held = set()
    for section in pe.sections[1:]:
        raw = section.get_data()
        held |= set(struct.unpack_from(f"<{len(raw) // 4}I", raw)) & fset
    return users, held, reads


def sole_users(refs, kept, module_at, segment_of, open_fns) -> dict[int, tuple[str, str]]:
    """Unnamed code -> (the module beside it in its segment that alone uses it, U), where every
    function calling it or holding its address is there and no data holds it; else (that
    module, D), where it shares data with that module's code and no other's."""
    users, from_data, held = refs
    # An address just past another the same code holds may be the end of that object's table.
    reads = {fn: {w for w in words if not any(w - 256 <= v < w for v in words)} for fn, words in held.items()}
    readers = collections.defaultdict(set)
    for fn, words in reads.items():
        for word in words:
            readers[word].add(fn)
    at = [p[3] for p in kept]
    placed = dict(module_at)
    out = {}
    pending = sorted(open_fns)
    while pending:
        left = []
        for fn in pending:
            k = bisect.bisect_left(at, fn)
            sides = {kept[j][5] for j in (k - 1, k) if 0 <= j < len(kept) and kept[j][0] == segment_of.get(fn)}
            mods = {placed.get(u) for u in users.get(fn, ())}
            shared = [{placed.get(u) for u in readers[w] - {fn}} for w in reads.get(fn, ())]
            private = {m.pop() for m in shared if len(m) == 1 and None not in m}
            if fn not in from_data and len(mods) == 1 and mods <= sides:
                out[fn] = (mods.pop(), "U")
            elif len(private) == 1 and private <= sides:
                out[fn] = (private.pop(), "D")
            else:
                left.append(fn)
                continue
            placed[fn] = out[fn][0]
        pending = left if len(left) < len(pending) else []
    return out


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
    with (ROOT / "data/names.csv").open() as f:
        for r in csv.DictReader(f):
            cls = r["name"].split("::")[0]
            if "::" in r["name"] and cls not in classes and cls not in yr_classes[int(r["address"], 16)]:
                yr_classes[int(r["address"], 16)].append(cls)

    game = bisect.bisect_left(functions, library_start)
    size = {int(r["address"], 16): int(r["size"]) for r in rows}
    statics = initialiser_statics(pe, functions, size)
    cuts = module_starts(pe, functions, statics, game)

    segment_of = {}
    for k, (lo, hi) in enumerate(zip(cuts, cuts[1:])):
        for fn in functions[lo:hi]:
            segment_of[fn] = k

    landed = landed_modules()
    index = tibsun_index(ROOT / "reference/tibsun/code")
    img, base = pe.get_memory_mapped_image(), pe.OPTIONAL_HEADER.ImageBase
    fn_names = collections.defaultdict(set)
    for name in ("symbols.csv", "names.csv"):
        with (ROOT / "data" / name).open() as f:
            for r in csv.DictReader(f):
                fn_names[int(r["address"], 16)].add(r["name"])
    landed_names = collections.defaultdict(set, {a: set(n) for a, n in fn_names.items()})
    with (ROOT / "data/progress.csv").open() as f:
        for r in csv.DictReader(f):
            landed_names[int(r["address"], 16)].add(r["name"])
    globals_in, objects_in = tibsun_globals(ROOT / "reference/tibsun/code")
    held, owner = vtable_slots(pe, functions)
    tier0_files = collections.defaultdict(set)
    tier0_symbols = collections.defaultdict(set)
    with (ROOT / "data/tier0.csv").open() as f:
        for r in csv.DictReader(f):
            if ";" not in r["address"]:
                stem = r["tibsun_file"].strip("()").rsplit(".", 1)[0].lower()
                tier0_files[int(r["address"], 16)].add(stem)
                tier0_symbols[int(r["address"], 16)].add(r["symbol"])
    tie_cache = {}

    def ties(fn):
        """TibSun file -> what ties this function to it: the file's distinctive literals or
        function names, its class, an object it initialises, or a tier 0 find."""
        if fn not in tie_cache:
            out = collections.defaultdict(list)
            vt_classes = {owner[vt].split("@")[0] for vt in held.get(fn, ()) if not owner[vt].startswith("?$")}
            vt_files = {index[3][c].lower() for c in vt_classes if c in index[3]}
            if len(vt_files) == 1:
                out[vt_files.pop()].append(f"vtable of {', '.join(sorted(vt_classes))}")
            for name in (n for a in statics.get(fn, ()) for n in fn_names.get(a, ())):
                files = objects_in.get(name.rsplit("::", 1)[-1], ())
                if len(files) == 1:
                    out[next(iter(files))].append(f"initialises {name}")
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
    src_files = collections.defaultdict(set)
    for fn, stem in landed.items():
        src_files[stem].add(fn)
    src = ROOT / "src"
    for p in src.rglob("*.cpp"):
        src_files.setdefault(p.relative_to(src).with_suffix("").as_posix().lower(), set())
    names_now = {r[0].lower() for r in ranges if r[0] != "?"}

    def taken(stem, fns):
        """Another module has the name, or a src/ file holding none of these functions."""
        return stem in names_now or (stem in src_files and not src_files[stem] & set(fns))

    gap_of = {fn: r[1] for r in ranges if r[0] == "?" for fn in r[1]}
    unknown = set(gap_of)
    built = initialiser_types(pe, functions, size, landed_names)
    for fn, types in built.items():
        # A static vector of a class's pointers is that class's own array.
        for name, args in types:
            if name == "DynamicVectorClass" and len(args) == 1 and not args <= set(classes):
                yr_classes[fn] += [c for c in args if c not in yr_classes[fn]]
    # A class's vtable that only code of one segment holds is that code's class.
    refs = references(pe, functions, size)
    holders = collections.defaultdict(set)
    vtables = {vt for vt, name in owner.items() if re.fullmatch(r"\w+@@", name)}
    for fn, words in refs[2].items():
        for vt in words & vtables:
            holders[vt].add(fn)
    for vt, fns in holders.items():
        cls = owner[vt][:-2]
        if len({segment_of.get(fn) for fn in fns}) == 1 and cls not in classes:
            for fn in fns:
                yr_classes[fn] += [cls] if cls not in yr_classes[fn] else []
    # So is a vector template's code over a class's pointers, in a segment naming no other class.
    named_segments = {segment_of.get(fn) for fn, c in yr_classes.items() if c and fn in unknown}
    for fn, vts in held.items():
        args = {m.group(1) if m else None for vt in vts
                for m in [re.fullmatch(r"\?\$(?:Dynamic)?VectorClass@PA[VU](\w+)@@@@", owner[vt])]}
        if len(args) == 1 and None not in args and not args <= set(classes) \
                and segment_of.get(fn) not in named_segments:
            yr_classes[fn] += [c for c in args if c not in yr_classes[fn]]
    yr = {fn: c for fn, c in yr_classes.items() if fn in unknown}

    def beside(fn, stem):
        """The stem is the module of a kept label next to the function in its segment."""
        k = bisect.bisect_left([p[3] for p in kept], fn)
        return any(0 <= j < len(kept) and kept[j][0] == segment_of.get(fn) and kept[j][1] == sort_key(stem)
                   for j in (k - 1, k))

    for fn, stem in yr_labels(kept, yr, labels).items():
        label(fn, stem, "Y")
    for fn, names in yr.items():
        stems = {s for c in names for s in yr_stems(c) if beside(fn, s)}
        if fn not in labels and len(stems) == 1:
            label(fn, stems.pop(), "Y")
    overruled = {}

    def relabel(fn, name, kind):
        """Label a function whose own labels were all overruled, in their place."""
        old = labels.pop(fn, None)
        if old and any(kinds[fn, n] & {"F", "T"} for n in old):
            overruled[fn] = (old.most_common(1)[0][0], kind)
        label(fn, name, kind)

    for fn, types in built.items():
        lo, hi = around(kept, fn)
        files = {f for t in types for f in globals_in.get(t, ())}
        files = [f for f in files if lo <= sort_key(f) <= hi
                 and (sort_key(f) in (lo, hi) or not taken(f, gap_of.get(fn, [fn])))]
        if fn in unknown and len(files) == 1:
            relabel(fn, files[0], "G")
    for fn in sorted(unknown - set(labels)):
        lo, hi = around(kept, fn)
        files = [f for f in ties(fn) if lo < sort_key(f) < hi and not taken(f, gap_of[fn]) or beside(fn, f)]
        if len(files) == 1:
            label(fn, files[0], "L")
    points, kept, rejected = assign(labels, kinds, segment_of)
    kept_at = {p[3]: p[5] for p in kept}
    ranges = build_ranges(cuts, functions, kept_at, kinds, display, landed)
    module_at = {fn: rng[0].lower() for rng in ranges if rng[0] != "?" for fn in rng[1]}
    for fn, name in neighbour_statics(statics, kept, kept_at, module_at, segment_of).items():
        relabel(fn, name, "B")
        module_at[fn] = name
    templates = {fn for fn, names in fn_names.items() if any("<" in n for n in names)}
    templates |= {fn for fn, symbols in tier0_symbols.items() if any("?$" in n for n in symbols)}
    for fn, name in instantiated(pe, size, (held, owner), templates, classes, kept, module_at, segment_of).items():
        relabel(fn, name, "C")
    points, kept, rejected = assign(labels, kinds, segment_of)
    kept_at = {p[3]: p[5] for p in kept}
    ranges = build_ranges(cuts, functions, kept_at, kinds, display, landed)

    def evidence(fns, stem):
        found = [why for fn in fns for why in ties(fn).get(stem, ())]
        return found + [f"builds a {name}" + (f"<{','.join(sorted(args))}>" if args else "")
                        for fn in fns for name, args in built.get(fn, ()) if stem in globals_in.get((name, args), ())]

    def contrary(fns):
        return any(yr_classes.get(fn) and not labels.get(fn)
                   or any(globals_in.get(t) for t in built.get(fn, ())) for fn in fns)

    names_now |= {r[0].lower() for r in ranges if r[0] != "?"}
    # Code no TibSun file claims goes to the only module beside it that uses it.
    claimed = tibsun_files(ranges, index, evidence, contrary, taken)
    open_fns = {fn for k, rng in enumerate(ranges) if rng[0] == "?" and k not in claimed for fn in rng[1]}
    module_at = {fn: rng[0].lower() for rng in ranges if rng[0] != "?" for fn in rng[1]}
    for fn, (name, kind) in sole_users(refs, kept, module_at, segment_of, open_fns).items():
        relabel(fn, name, kind)
    points, kept, rejected = assign(labels, kinds, segment_of)
    kept_at = {p[3]: p[5] for p in kept}
    ranges = build_ranges(cuts, functions, kept_at, kinds, display, landed)
    names_now |= {r[0].lower() for r in ranges if r[0] != "?"}
    for k, (stem, how) in tibsun_files(ranges, index, evidence, contrary, taken).items():
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
            stems = {landed[fn] for fn in rng[1] if fn in landed}
            stem = stems.pop() if len(stems) == 1 else None
            if stem and stem not in names_now:
                rng[0], rng[3] = stem, "src"
                names_now.add(stem)
            else:
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
    named = sum(len(r[1]) for r in ranges if r[0] != "?" and r[3] not in ("seg", "src"))
    how = collections.Counter()
    for r in ranges:
        how[r[3]] += len(r[1])
    print(f"{len(cuts) - 1} segments -> {out}: {named} of {game} game functions named by evidence"
          f" ({how['class']} by class names only, {how['tibsun'] + how['evidence'] + how['order']}"
          f" by TibSun files),"
          f" {how['seg']} provisional (seg_), {how['src']} by their src/ file only, "
          f"{sum(len(r[1]) for r in ranges if r[0] == '?')} not; {len(points) - len(kept)} of "
          f"{len(points)} labels overruled, {len(rejected)} of them runs inside another module")
    kept_set = set(kept_at)
    for p in points:
        if p[3] in kept_set or not p[4] & {"F", "T"}:
            continue
        why = "run inside another module" if p[3] in rejected else "out of order"
        print(f"  rejected 0x{p[3]:08X} {display[p[5]]} ({''.join(sorted(p[4]))}): {why}, "
              f"placed in {module_at.get(p[3], '?')}")
    for fn, (name, kind) in sorted(overruled.items()):
        print(f"  rejected 0x{fn:08X} {display[name]}: out of order, placed in {module_at.get(fn, '?')}"
              f" by {kind} evidence")


if __name__ == "__main__":
    main()
