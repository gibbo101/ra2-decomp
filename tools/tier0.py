"""Find TibSun functions that Yuri's Revenge kept byte for byte, and land them in src/.

Usage: uv run tools/tier0.py [--files abstype.cpp,base.cpp] [--jobs N] [--no-land]

1. Compiles every TibSun .cpp (TibSun headers only, check.py's flags) into build/tier0/,
   with a source listing that ties each function to its line in the .cpp.
2. Looks for every compiled function of 8 bytes or more at the function starts of the
   same size in data/functions.csv, relocation fields as wildcards, EA's patches undone.
3. Rules out addresses where a relocation target is known and differs, as check.py
   resolves them, also using the other candidates' unique addresses. Of several
   survivors, the one lying in the module named after the TibSun file is kept when it is
   the only one there; one address keeps one function.
4. Copies each survivor's definition, comments removed, into src/<module>.cpp (module from
   data/modules.csv, or the TibSun file's module where that is "?"; the VQA library's are
   vqalib/X), with the file-scope items it needs, and keeps it only if check.py prints
   MATCH there.
5. Writes data/tier0.csv. Functions already annotated in src/ are skipped, so it re-runs.
"""

import argparse
import bisect
import csv
import os
import re
import struct
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, field
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check  # noqa: E402
from coff import CoffObject  # noqa: E402
from modules import sort_key  # noqa: E402

ROOT = check.ROOT
TIBSUN = check.TIBSUN
OUT = ROOT / "build/tier0"
CSV_OUT = ROOT / "data/tier0.csv"
MIN_SIZE = 8
LICENCE = """\
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
"""
KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "__declspec", "defined",
            "operator", "catch", "throw", "new", "delete", "static_cast", "const_cast",
            "reinterpret_cast", "dynamic_cast", "__stdcall", "__cdecl", "__fastcall"}
STATIC = {"static", "_STATIC"}    # _STATIC is the VQA library's macro for static
PROC = re.compile(r"^(\S+)\s+PROC NEAR")
LISTED = re.compile(r"^; (\d+)\s*: ?(.*)$")
ERROR = re.compile(r"^(.*?)\((\d+)\) : (?:fatal )?error (C\d+): (.*)$")
RESULT = re.compile(r"^(0x[0-9A-F]{8}) (.+?): (MATCH|WRONG TARGETS|[\d.]+%) "
                    r"\((\d+)/(\d+) bytes(?:, (\d+) unverified)?\) (\S+)")


# ---------------------------------------------------------------- source parsing

def strip_comments(text: str) -> str:
    """The text with every comment blanked out; line numbers and string literals kept."""
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        elif c == "/" and text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join("\n" if ch == "\n" else " " for ch in text[i:j]))
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] not in (c, "\n"):
                j += 2 if text[j] == "\\" else 1
            j = min(j + 1, n)
            out.append(text[i:j])
            i = j
        else:
            out.append(c)
            i += 1
    return "".join(out)


def without_strings(text: str) -> str:
    return re.sub(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'', '""', text)


@dataclass
class Item:
    start: int                 # first line, 0-based
    end: int                   # last line, inclusive
    kind: str                  # pp, function, decl
    head: str = ""             # text before the body or the end
    name: str = ""             # a function's (qualified) name
    names: set = field(default_factory=set)
    helper: bool = False       # static or inline function, or a file-local declaration
    proto: bool = False        # a function prototype
    static: bool = False       # declared static
    idents: set = field(default_factory=set)


def classify(head: str) -> str:
    h = " ".join(head.split())
    if re.match(r'(extern\s+"C"|namespace\b)', h):
        return "container"
    h2 = re.sub(r"operator\s*(\(\)|[^\s(]+)", "operator", h)
    if re.match(r"(template\s*<[^>]*>\s*)?(typedef\s+)?(static\s+)?(const\s+)?(class|struct|union|enum)\b", h2) \
            and "(" not in h2:
        return "type"
    eq, par = h2.find("="), h2.find("(")
    if eq >= 0 and (par < 0 or eq < par):
        return "init"
    return "function" if par >= 0 else "type"


def parse_items(stripped: str) -> list[Item]:
    """Top-level constructs of a comment-stripped C++ file, by brace and preprocessor structure."""
    lines = stripped.split("\n")
    items: list[Item] = []
    cur = None           # [start line, mode, head chars]
    depth = paren = 0
    pp_stack = []        # [depth at #if, depth after first branch or None]
    k = 0
    while k < len(lines):
        line = lines[k]
        s = line.lstrip()
        if s.startswith("#"):
            end = k
            while lines[end].rstrip().endswith("\\") and end + 1 < len(lines):
                end += 1
            d = s[1:].lstrip()
            if re.match(r"if", d):
                pp_stack.append([depth, None])
            elif re.match(r"el", d) and pp_stack:
                if pp_stack[-1][1] is None:
                    pp_stack[-1][1] = depth
                depth = pp_stack[-1][0]
            elif re.match(r"endif", d) and pp_stack:
                base, first = pp_stack.pop()
                if first is not None:
                    depth = first
            if cur is None and depth == 0:
                items.append(Item(k, end, "pp", head=" ".join(lines[k:end + 1])))
            k = end + 1
            continue
        i = 0
        while i < len(line):
            c = line[i]
            if c in "\"'":
                j = i + 1
                while j < len(line) and line[j] != c:
                    j += 2 if line[j] == "\\" else 1
                if cur is not None and cur[1] == "head":
                    cur[2].append(line[i:j + 1])
                i = j + 1
                continue
            if cur is None:
                if c.isspace():
                    i += 1
                    continue
                if c == ";" and depth == 0:
                    i += 1
                    continue
                cur = [k, "head", []]
            if c == "(":
                paren += 1
            elif c == ")":
                paren -= 1
            elif c == "{":
                if depth == 0 and paren == 0 and cur[1] == "head":
                    kind = classify("".join(cur[2]))
                    cur[1] = "body" if kind in ("function", "container") else "agg"
                    cur.append(kind)
                    cur.append("".join(cur[2]))
                depth += 1
            elif c == "}":
                depth -= 1
                if depth == 0 and cur[1] == "body":
                    kind = cur[3]
                    items.append(Item(cur[0], k, "function" if kind == "function" else "decl",
                                      head=cur[4]))
                    cur = None
                    i += 1
                    continue
            elif c == ";" and depth == 0 and paren == 0:
                items.append(Item(cur[0], k, "decl", head="".join(cur[2])))
                cur = None
                i += 1
                continue
            if cur is not None and cur[1] == "head":
                cur[2].append(c)
            i += 1
        if cur is not None and cur[1] == "head":
            cur[2].append("\n")
        k += 1
    for it in items:
        describe(it, lines)
    return items


NAME = re.compile(r"((?:\w+\s*(?:<[^<>()]*>)?\s*::\s*)*(?:operator\s*(?:\(\)|[^\s(]+)|~\s*\w+|\w+))\s*\(")


def describe(it: Item, lines: list[str]) -> None:
    text = "\n".join(lines[it.start:it.end + 1])
    plain = without_strings(text)
    it.idents = set(re.findall(r"[A-Za-z_]\w*", plain))
    head = " ".join(it.head.split())
    if it.kind == "pp":
        m = re.match(r"#\s*define\s+(\w+)", head)
        if m:
            it.names = {m.group(1)}
            it.helper = True
        return
    if it.kind == "function":
        for m in NAME.finditer(head):
            name = re.sub(r"\s+", "", m.group(1))
            if name.split("::")[-1] not in KEYWORDS:
                it.name = name
                break
        before = head[:head.find(it.name.split("::")[0])] if it.name else head
        words = set(re.findall(r"\w+", before))
        it.names = {it.name.split("::")[-1]} if it.name else set()
        it.helper = bool(words & {"static", "inline", "__inline"}) and ("::" not in it.name
                                                                         or "inline" in words)
        return
    # A declaration: variables, prototypes, types, typedefs, enums.
    body = without_strings(text)
    names = set()
    for m in re.finditer(r"\b(?:class|struct|union|enum)\s+(\w+)\s*(?::[^{;]*)?\{", body):
        names.add(m.group(1))
    for m in re.finditer(r"\benum\b[^{;]*\{([^}]*)\}", body):
        for part in m.group(1).split(","):
            e = re.match(r"\s*(\w+)", part)
            if e:
                names.add(e.group(1))
    flat = body
    while True:
        new = re.sub(r"\{[^{}]*\}", " ", flat)
        if new == flat:
            break
        flat = new
    flat = re.sub(r"=[^,;]*", "", flat)
    proto = re.search(r"(\w+)\s*\([^*]", flat)
    if proto and not re.search(r"\(\s*\*", flat) and "typedef" not in flat:
        names.add(proto.group(1))
        is_proto = True
    else:
        is_proto = False
        for m in re.finditer(r"(?<!::)(?<![\w:])(\w+)\s*(?:\[[^\]]*\]\s*)*(?=[,;)])", flat):
            names.add(m.group(1))
        for m in re.finditer(r"\(\s*\*\s*(\w+)\s*\)", flat):
            names.add(m.group(1))
    qualified_def = bool(re.search(r"\w::\w", flat.split("(")[0]))
    names -= KEYWORDS | {"void", "int", "char", "const", "long", "short", "unsigned", "bool"}
    it.names = names
    if re.match(r'\s*extern\s+"[^"]*"\s*\{', body):
        inner = body[body.index("{") + 1:body.rindex("}")]
        names = {m.group(1) for m in re.finditer(r"(\w+)\s*(?:\[[^\]]*\]\s*)*(?=[,;)])",
                                                 re.sub(r"\([^()]*\)", "", inner))} - KEYWORDS
        is_proto = False
        it.names = names
    words = set(re.findall(r"\w+", flat))
    local = bool(words & STATIC) or "typedef" in words or "extern" in words or bool(
        re.match(r"\s*(?:typedef\s+)?(?:class|struct|union|enum)\b", flat))
    it.helper = not qualified_def and (local or is_proto)
    it.proto = is_proto
    it.static = bool(words & STATIC)


def item_text(src: "Source", it: Item) -> str:
    """The item's lines with comments removed: comment-only lines go, blank runs collapse,
    and no blank line opens or closes a block."""
    out = []
    for k in range(it.start, it.end + 1):
        line = src.lines[k].rstrip()
        if not line and src.raw[k].strip():
            continue
        if not line and (not out or not out[-1] or out[-1].endswith("{")):
            continue
        if line.strip() == "}" and out and not out[-1]:
            out.pop()
        out.append(line)
    while out and not out[-1]:
        out.pop()
    return "\n".join(out)


@dataclass
class Source:
    path: Path
    raw: list[str]
    lines: list[str]           # comment-stripped
    items: list[Item]
    starts: list[int]

    def item_at(self, line: int) -> Item | None:
        k = bisect.bisect_right(self.starts, line) - 1
        if k >= 0 and self.items[k].start <= line <= self.items[k].end:
            return self.items[k]
        return None


SOURCES: dict[Path, Source] = {}


def source(path: Path) -> Source:
    if path not in SOURCES:
        raw = path.read_text(encoding="latin-1")
        stripped = strip_comments(raw)
        items = parse_items(stripped)
        SOURCES[path] = Source(path, raw.split("\n"), stripped.split("\n"), items,
                               [it.start for it in items])
    return SOURCES[path]


# ---------------------------------------------------------------- discovery

def cpp_files(only: set[str] | None) -> list[Path]:
    files = sorted(TIBSUN.glob("*.cpp")) + sorted((TIBSUN / "vqalib").glob("*.cpp"))
    return [p for p in files if not only or p.name.lower() in only]


def obj_stem(cpp: Path) -> str:
    return cpp.stem if cpp.parent == TIBSUN else f"{cpp.parent.name}_{cpp.stem}"


def module_name(cpp: Path) -> str:
    """The module a TibSun file's code links as, as data/modules.csv names it."""
    return cpp.stem if cpp.parent == TIBSUN else f"{cpp.parent.name}/{cpp.stem}"


def compile_one(cpp: Path) -> tuple[Path, bool]:
    obj = OUT / "obj" / f"{obj_stem(cpp)}.obj"
    asm = OUT / "asm" / f"{obj_stem(cpp)}.asm"
    if obj.exists() and asm.exists() and obj.stat().st_mtime > cpp.stat().st_mtime:
        return cpp, True
    obj.parent.mkdir(parents=True, exist_ok=True)
    asm.parent.mkdir(parents=True, exist_ok=True)
    cmd = [str(ROOT / "tools/wcl"), *check.FLAGS, f"/I{check.winpath(TIBSUN)}",
           f"/I{check.winpath(TIBSUN / 'vqalib')}", "/FAs", f"/Fa{check.winpath(asm)}",
           f"/Fo{check.winpath(obj)}", check.winpath(cpp)]
    proc = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
    (OUT / "obj" / f"{obj_stem(cpp)}.log").write_text(proc.stdout + proc.stderr)
    ok = proc.returncode == 0 and obj.exists()
    if not ok:
        obj.unlink(missing_ok=True)
    return cpp, ok


def listing(asm: Path) -> dict[str, list[tuple[int, str]]]:
    """Decorated name -> the (line, text) source lines the listing shows inside it."""
    out, cur = {}, None
    for line in asm.read_text(errors="replace").splitlines():
        m = PROC.match(line)
        if m:
            cur = out.setdefault(m.group(1), [])
            continue
        if cur is not None and " ENDP" in line:
            cur = None
            continue
        if cur is not None:
            m = LISTED.match(line)
            if m:
                cur.append((int(m.group(1)) - 1, m.group(2)))
    return out


def short_name(sym: str) -> str:
    """The unqualified name a decorated symbol's definition is written with."""
    m = re.match(r"\?\?(0|1|_G|_E)(\w+)@", sym)
    if m:
        return ("" if m.group(1) == "0" else "~") + m.group(2)
    if sym.startswith("??"):
        return "operator"
    if sym.startswith("?"):
        return sym[1:].split("@", 1)[0]
    return sym.lstrip("_@").split("@", 1)[0]


def definition(src: Source, sym: str, shown: list[tuple[int, str]]) -> Item | None:
    """The function item in `src` the listing places this symbol's code in, if any."""
    want = short_name(sym)
    norm = lambda s: " ".join(s.split())  # noqa: E731
    for line, text in shown:
        if line >= len(src.raw) or norm(src.raw[line]) != norm(text):
            continue
        it = src.item_at(line)
        if it is None or it.kind != "function" or not it.name:
            continue
        last = it.name.split("::")[-1].replace(" ", "")
        if last == want or (want == "operator" and last.startswith("operator")):
            return it
    return None


@dataclass
class Fn:
    symbol: str
    obj: str                   # object stem
    cpp: Path | None           # the .cpp holding its definition, if any
    item: Item | None
    data: bytes
    ranges: list               # unmasked byte ranges
    pick: tuple                # (symbol, section, start, end) in its object
    hits: list = field(default_factory=list)
    survivors: list = field(default_factory=list)
    scores: dict = field(default_factory=dict)
    status: str = ""
    address: int = 0
    unverified: str = ""
    note: str = ""
    placed: bool = False       # chosen from several survivors by its module


OBJECTS: dict[str, tuple[bytes, CoffObject]] = {}


def discover(files: list[Path], orig: check.Original, jobs: int) -> list[Fn]:
    with ThreadPoolExecutor(jobs) as pool:
        results = list(pool.map(compile_one, files))
    failed = [p.name for p, ok in results if not ok]
    if failed:
        print(f"{len(failed)} TibSun files do not compile: {', '.join(failed)}")
    by_size: dict[int, list[int]] = {}
    for a, s in orig.sizes.items():
        by_size.setdefault(s, []).append(a)
    fns: dict[str, Fn] = {}
    for cpp, ok in results:
        if not ok:
            continue
        stem = obj_stem(cpp)
        raw = (OUT / "obj" / f"{stem}.obj").read_bytes()
        obj = CoffObject(raw)
        OBJECTS[stem] = (raw, obj)
        shown = listing(OUT / "asm" / f"{stem}.asm")
        src = source(cpp)
        for pick in obj.functions():
            sym, sec, start, end = pick
            relocs = [r for r in sec.relocs if start <= r.offset < end]
            masked = {r.offset - start + k for r in relocs for k in range(4)}
            data = sec.data[start:end]
            while data and data[-1] in check.PADDING and len(data) - 1 not in masked:
                data = data[:-1]
            if len(data) < MIN_SIZE:
                continue
            item = definition(src, sym.name, shown.get(sym.name, []))
            prev = fns.get(sym.name)
            if prev is not None and (prev.item is not None or item is None):
                continue
            ranges, a = [], 0
            for k in sorted(m for m in masked if m < len(data)) + [len(data)]:
                if k > a:
                    ranges.append((a, k))
                a = max(a, k + 1)
            fns[sym.name] = Fn(sym.name, stem, cpp if item else None, item, data, ranges, pick)
    found = []
    for fn in fns.values():
        n = len(fn.data)
        for a in by_size.get(n, []):
            theirs = orig.read(a, n)
            if all(theirs[x:y] == fn.data[x:y] for x, y in fn.ranges):
                fn.hits.append(a)
        if fn.hits:
            fn.hits.sort()
            found.append(fn)
    return found


# ---------------------------------------------------------------- disambiguation

class Resolver:
    """check.check_ref's verdicts for one state of the known names, without its per-field
    scan of every decorated name."""

    def __init__(self, orig, names: dict[str, int], local: dict[str, int]):
        self.orig, self.names, self.local = orig, names, local
        self.overloaded = {check.qualified(d) for d in local}

    def status(self, obj, sec, start, address, target, got) -> str:
        if target.section == sec.number:
            return "ok" if got == address - start + target.value else "mismatch"
        if target.name in self.local:
            return "ok" if got == self.local[target.name] else "mismatch"
        if target.section > 0:
            tsec = obj.sections[target.section - 1]
            if not tsec.is_code and target.name.startswith(("??_C@", "__real@")):
                content = tsec.data[target.value:]
                if content:
                    return "ok" if self.orig.read(got, len(content)) == content else "mismatch"
        if target.name.startswith("__imp_"):
            want = self.orig.imports().get(target.name[len("__imp_"):].lstrip("_").split("@", 1)[0])
            if want is not None:
                return "ok" if got == want else "mismatch"
        name = check.qualified(target.name)
        placeholder = re.search(r"(?:^|::)(?:FUN|DAT)_([0-9A-Fa-f]{8})$", name)
        if placeholder:
            return "ok" if got == int(placeholder.group(1), 16) else "mismatch"
        if name in self.names and name not in self.overloaded:
            return "ok" if got == self.names[name] else "mismatch"
        return "unverified"


def evaluate(fn: Fn, a: int, res: Resolver) -> tuple[int, int, int]:
    """(mismatched, unverified, verified) relocation fields of `fn` placed at `a`."""
    _, obj = OBJECTS[fn.obj]
    sym, sec, start, end = fn.pick
    theirs = res.orig.read(a, len(fn.data))
    count = {"ok": 0, "mismatch": 0, "unverified": 0}
    for r in sec.relocs:
        off = r.offset - start
        if not (start <= r.offset < end) or off + 4 > len(fn.data):
            continue
        addend = struct.unpack_from("<i", fn.data, off)[0]
        if r.type == check.REL_DIR32:
            got = (struct.unpack_from("<I", theirs, off)[0] - addend) & 0xFFFFFFFF
        elif r.type == check.REL_REL32:
            got = (a + off + 4 + struct.unpack_from("<i", theirs, off)[0] - addend) & 0xFFFFFFFF
        else:
            continue
        count[res.status(obj, sec, start, a, obj.by_index[r.symbol], got)] += 1
    return count["mismatch"], count["unverified"], count["ok"]


def disambiguate(fns: list[Fn], orig, modules) -> None:
    names = check.known_names()
    base_local = check.known_decorated()
    unique = {fn.symbol: fn.hits[0] for fn in fns if len(fn.hits) == 1}
    while True:
        local = dict(base_local)
        for s, a in unique.items():
            local.setdefault(s, a)
        res = Resolver(orig, names, local)
        for fn in fns:
            fn.survivors, fn.scores = [], {}
            for a in fn.hits:
                bad, unver, ok = evaluate(fn, a, res)
                if not bad:
                    fn.survivors.append(a)
                    fn.scores[a] = (unver, ok)
        new = {fn.symbol: fn.survivors[0] for fn in fns if len(fn.survivors) == 1}
        if new == unique:
            break
        unique = new
    # Several survivors: the one in the TibSun file's own module, if it is the only one.
    for fn in fns:
        if len(fn.survivors) > 1 and fn.cpp is not None:
            home = [a for a in fn.survivors if modules(a).lower() == module_name(fn.cpp).lower()]
            if len(home) == 1:
                fn.placed, fn.survivors = True, home
    # One address, one function: the claim whose file is the address's module, then the
    # one with more proven references, wins; a tie leaves the address to nobody.
    claims: dict[int, list[Fn]] = {}
    for fn in fns:
        if len(fn.survivors) == 1:
            claims.setdefault(fn.survivors[0], []).append(fn)
        else:
            fn.status = "ambiguous"
            fn.note = f"{len(fn.survivors)} of {len(fn.hits)} starts survive"
    for a, cl in claims.items():
        mod = modules(a)

        def rank(fn):
            stem = module_name(fn.cpp).lower() if fn.cpp else ""
            unver, ok = fn.scores[a]
            return (mod.lower() == stem, fn.item is not None, ok, -unver, len(fn.hits) == 1)
        cl.sort(key=rank, reverse=True)
        if len(cl) > 1 and rank(cl[0]) == rank(cl[1]):
            for fn in cl:
                fn.status, fn.note = "ambiguous", f"0x{a:08X} also fits " + " ".join(
                    f.symbol for f in cl if f is not fn)[:200]
            continue
        cl[0].address = a
        for fn in cl[1:]:
            fn.status, fn.note = "ambiguous", f"0x{a:08X} claimed by {cl[0].symbol}"


# ---------------------------------------------------------------- landing

def module_lookup():
    rows = []
    with (ROOT / "data/modules.csv").open() as f:
        for r in csv.DictReader(f):
            rows.append((int(r["first"], 16), int(r["last"], 16), r["module"]))
    rows.sort()
    firsts = [r[0] for r in rows]

    def module(a: int) -> str:
        k = bisect.bisect_right(firsts, a) - 1
        if k >= 0 and rows[k][0] <= a <= rows[k][1]:
            return rows[k][2]
        return "?"

    def window(a: int) -> tuple[str, str]:
        """The named modules before and after an address; the linker placed modules in
        case-insensitive alphabetical order, so an unnamed one sorts between them."""
        k = bisect.bisect_right(firsts, a) - 1
        before = next((rows[j][2] for j in range(k, -1, -1) if rows[j][2] != "?"), "")
        after = next((rows[j][2] for j in range(k + 1, len(rows)) if rows[j][2] != "?"), "~")
        return before, after
    module.window = window
    return module


def dependencies(src: Source, fn_items: list[Item], extra: set[str]) -> list[Item]:
    """File-scope items of `src` the given functions use and the headers lack: statics,
    types, macros and inline or static helpers, followed transitively."""
    landed = {id(it) for it in fn_items}
    by_name: dict[str, list[Item]] = {}
    for it in src.items:
        for n in it.names:
            by_name.setdefault(n, []).append(it)
    chosen, todo, seen = {}, [], set()
    for it in fn_items:
        todo.extend(it.idents)
    todo.extend(extra)
    while todo:
        n = todo.pop()
        if n in seen:
            continue
        seen.add(n)
        for it in by_name.get(n, []):
            if id(it) in landed or id(it) in chosen:
                continue
            if not (it.helper or n in extra):
                continue
            if it.kind == "function" and "::" in it.name and n not in extra and not it.helper:
                continue
            chosen[id(it)] = it
            todo.extend(it.idents)
            # A static function's prototype needs its definition too.
            if it.proto and it.static:
                for d in by_name.get(n, []):
                    if d.kind == "function" and "::" not in d.name and id(d) not in landed:
                        chosen[id(d)] = d
                        todo.extend(d.idents)
    return sorted(chosen.values(), key=lambda it: it.start)


def preamble(src: Source) -> list[str]:
    """The TibSun file's leading preprocessor lines (#define switches and #includes), up to
    the last point where every conditional is closed."""
    out, keep, depth = [], 0, 0
    for it in src.items:
        if it.kind != "pp":
            break
        d = it.head.lstrip("# \t")
        depth += 1 if d.startswith("if") else -1 if d.startswith("endif") else 0
        for k in range(it.start, it.end + 1):
            if src.lines[k].strip():
                out.append(src.lines[k].rstrip())
        if depth == 0:
            keep = len(out)
    return out[:keep]


@dataclass
class Unit:
    """One src file being extended."""
    module: str
    path: Path
    src: Source                # the TibSun file
    fns: list                  # Fn, in address order
    original: str | None       # the file's content before this run
    extra: dict = field(default_factory=dict)      # symbol -> names forced in by errors
    add_includes: bool = True
    blocks: list = field(default_factory=list)     # (first line, last line, owner)
    text: str = ""


def render(u: Unit) -> str:
    groups: dict[int, list[Fn]] = {}
    for fn in u.fns:
        groups.setdefault(id(fn.item), []).append(fn)
    ordered = sorted(groups.values(), key=lambda g: min(f.address for f in g))
    extra = set()
    for fn in u.fns:
        extra |= u.extra.get(fn.symbol, set())
    deps, seen_text = [], set()
    for d in dependencies(u.src, [g[0].item for g in ordered], extra):
        key = " ".join(item_text(u.src, d).split())
        if key not in seen_text:
            seen_text.add(key)
            deps.append(d)
    pre = preamble(u.src)
    out: list[str] = []
    blocks = []

    def emit(text: str, owner) -> None:
        first = len(out) + 1
        out.extend(text.split("\n"))
        blocks.append((first, len(out), owner))

    def emit_deps(deps):
        for k, d in enumerate(deps):
            emit(item_text(u.src, d), ("dep", d))
            if not (d.kind == "pp" and k + 1 < len(deps) and deps[k + 1].kind == "pp"):
                out.append("")

    def fn_block(g):
        marks = "\n".join(f"// FUNCTION: 0x{f.address:08X}" for f in sorted(g, key=lambda f: f.address))
        return marks + "\n" + item_text(u.src, g[0].item)

    if u.original is None:
        out.extend(LICENCE.rstrip("\n").split("\n"))
        out.append("")
        out.extend(pre)
        out.append("")
        emit_deps(deps)
        for g in ordered:
            emit(fn_block(g), ("fn", g))
            out.append("")
        while out and not out[-1]:
            out.pop()
    else:
        old = u.original.rstrip("\n").split("\n")
        have = {" ".join(l.split()) for l in strip_comments(u.original).split("\n")}
        inc_end = max((k for k, l in enumerate(old) if l.lstrip().startswith("#include")), default=-1)
        old_items = parse_items(strip_comments(u.original))
        defined = set()
        for it in old_items:
            defined |= it.names
        marks = []
        for k, l in enumerate(old):
            m = check.ANNOTATION.match(l)
            if m and not (k > 0 and check.ANNOTATION.match(old[k - 1])):
                marks.append((int(m.group(1), 16), k))
        new_inc = [l for l in pre if l.lstrip().startswith("#include")
                   and " ".join(l.split()) not in have] if u.add_includes else []
        deps = [d for d in deps if not (d.names & defined) and
                item_text(u.src, d).strip() not in u.original]
        inserts: dict[int, list] = {}
        for g in ordered:
            a = min(f.address for f in g)
            at = next((k for addr, k in marks if addr > a), len(old))
            inserts.setdefault(at, []).append(g)
        for k in range(len(old) + 1):
            for g in inserts.get(k, []):
                if out and out[-1]:
                    out.append("")
                emit(fn_block(g), ("fn", g))
                out.append("")
            if k == len(old):
                break
            out.append(old[k])
            if k == inc_end:
                out.extend(new_inc)
                if deps:
                    out.append("")
                emit_deps(deps)
                if deps and k + 1 < len(old) and not old[k + 1].strip():
                    out.pop()
        while out and not out[-1]:
            out.pop()
    u.blocks = blocks
    u.text = "\n".join(out) + "\n"
    return u.text


def annotations_text(text: str) -> list[tuple[int, str]]:
    """check.annotations() for text not yet written to a file."""
    lines = text.splitlines()
    out = []
    for i, line in enumerate(lines):
        m = check.ANNOTATION.match(line)
        if m and m.group(2):
            out.append((int(m.group(1), 16), m.group(2)))
        elif m:
            n = re.search(r"((?:\w+::)*(?:operator\s*\S+?|~?\w+))\s*\(", " ".join(lines[i + 1:i + 4]))
            out.append((int(m.group(1), 16), n.group(1).replace(" ", "") if n else ""))
    return out


def drop(u: Unit, fn: Fn, note: str) -> None:
    if fn in u.fns:
        u.fns.remove(fn)
        fn.status, fn.note = "failed_in_src", note[:200]


def land(u: Unit, matched_before: set[int]) -> None:
    """Grow u.path with u.fns, removing each function that does not compile or match."""
    for fn in list(u.fns):
        if check.FORBIDDEN.search(item_text(u.src, fn.item)):
            drop(u, fn, "inline assembly")
    for _ in range(200):
        if not u.fns:
            break
        text = render(u)
        # The checker must find each landed function by the name its marker reads.
        names = dict(annotations_text(text))
        bad = [fn for fn in u.fns if not fn.symbol.startswith(check.mangled(names.get(fn.address, "?")))]
        for fn in bad:
            drop(u, fn, f"checker reads the name as '{names.get(fn.address)}'")
        if bad:
            continue
        if any(check.FORBIDDEN.search(item_text(u.src, b[2][1])) for b in u.blocks if b[2][0] == "dep"):
            for b in u.blocks:
                if b[2][0] == "dep" and check.FORBIDDEN.search(item_text(u.src, b[2][1])):
                    users = [fn for fn in u.fns if b[2][1].names & fn.item.idents]
                    for fn in users or list(u.fns):
                        drop(u, fn, "needs an item with inline assembly")
            continue
        u.path.parent.mkdir(exist_ok=True)
        u.path.write_text(text, encoding="latin-1")
        proc = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), str(u.path)],
                              capture_output=True, text=True, cwd=ROOT)
        output = proc.stdout + proc.stderr
        results = [RESULT.match(l) for l in proc.stdout.splitlines()]
        results = [m for m in results if m]
        if not results:
            if not handle_errors(u, output):
                for fn in list(u.fns):
                    drop(u, fn, "file does not compile: " + first_error(output))
            continue
        by_addr = {int(m.group(1), 16): m for m in results}
        broken = [a for a in matched_before if a in by_addr and by_addr[a].group(3) != "MATCH"]
        if broken:
            if u.add_includes and u.original is not None:
                u.add_includes = False
                continue
            for fn in list(u.fns):
                drop(u, fn, "breaks existing matches " + " ".join(f"0x{a:08X}" for a in broken))
            continue
        changed = False
        for fn in list(u.fns):
            m = by_addr.get(fn.address)
            if m is None:
                drop(u, fn, "checker printed no result")
                changed = True
            elif m.group(3) != "MATCH":
                drop(u, fn, m.group(3))
                changed = True
            else:
                fn.unverified = m.group(6) or "0"
        if not changed:
            for fn in u.fns:
                fn.status = "landed"
            return
    restore(u)


def first_error(output: str) -> str:
    for line in output.splitlines():
        m = ERROR.match(line)
        if m:
            return f"{m.group(3)} {m.group(4)}"
    return output.strip().splitlines()[-1] if output.strip() else "no output"


def handle_errors(u: Unit, output: str) -> bool:
    """Act on compile errors: add the TibSun item defining a name on the failing line, or
    drop the functions at fault. False when the errors cannot be tied to a landed function."""
    if "no compiled function has this name" in output:
        name = output.strip().splitlines()[-1].split(":", 1)[0]
        hit = [fn for fn in u.fns if check.qualified(fn.symbol) == name or fn.item.name == name]
        for fn in hit:
            drop(u, fn, "checker cannot find it by name")
        return bool(hit)
    here = u.path.name.lower()
    text = u.text.split("\n")
    rendered = {id(b[2][1]) for b in u.blocks if b[2][0] == "dep"}
    adds, drops = {}, {}
    for line in output.splitlines():
        m = ERROR.match(line)
        if not m:
            continue
        if not m.group(1).replace("\\", "/").lower().endswith(here):
            if u.add_includes and u.original is not None:
                u.add_includes = False
                return True
            continue
        ln = int(m.group(2))
        owner = next((b[2] for b in u.blocks if b[0] <= ln <= b[1]), None)
        if owner is None:
            continue
        if owner[0] == "dep":
            users = [fn for fn in u.fns if any(d is owner[1] for d in dependencies(
                u.src, [fn.item], u.extra.get(fn.symbol, set())))]
        else:
            users = list(owner[1])
        users = users or list(u.fns)
        wanted = set(re.findall(r"[A-Za-z_]\w*", without_strings(text[ln - 1]) if ln <= len(text) else ""))
        wanted |= set(re.findall(r"'(\w+)'", m.group(4)))
        known = set().union(*(u.extra.get(fn.symbol, set()) for fn in users))
        add = {n for n in wanted - known
               for it in u.src.items
               if n in it.names and (it.kind != "function" or "::" not in it.name)
               and id(it) not in rendered and not it.proto and "::" not in n}
        for fn in users:
            if add:
                adds.setdefault(fn.symbol, set()).update(add)
            else:
                drops.setdefault(id(fn), (fn, ("needs a file-scope item that fails: "
                                               if owner[0] == "dep" else "") + f"{m.group(3)} {m.group(4)}"))
    for sym, names in adds.items():
        u.extra.setdefault(sym, set()).update(names)
    for fn, note in drops.values():
        if fn.symbol not in adds:
            drop(u, fn, note)
    return bool(adds or drops)


def restore(u: Unit) -> None:
    for fn in list(u.fns):
        drop(u, fn, "did not settle")
    if u.original is None:
        u.path.unlink(missing_ok=True)
    else:
        u.path.write_text(u.original, encoding="latin-1")


def finish(u: Unit) -> None:
    if not u.fns:
        if u.original is None:
            u.path.unlink(missing_ok=True)
        else:
            u.path.write_text(u.original, encoding="latin-1")
    if u.path.parent != ROOT / "src" and u.path.parent.is_dir() and not any(u.path.parent.iterdir()):
        u.path.parent.rmdir()


# ---------------------------------------------------------------- main

def annotated_in_src() -> set[int]:
    out = set()
    for p in (ROOT / "src").rglob("*.cpp"):
        out |= {a for a, _ in check.annotations(p)}
    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--files", help="comma-separated TibSun .cpp names (default: all)")
    ap.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    ap.add_argument("--no-land", action="store_true")
    args = ap.parse_args()
    only = {f.strip().lower() for f in args.files.split(",")} if args.files else None

    orig = check.Original()
    module = module_lookup()
    fns = discover(cpp_files(only), orig, args.jobs)
    print(f"{len(fns)} TibSun functions of {MIN_SIZE}+ bytes sit at a function start of their size; "
          f"{sum(1 for f in fns if len(f.hits) == 1)} at exactly one")
    disambiguate(fns, orig, module)
    winners = [fn for fn in fns if fn.address]
    print(f"{len(winners)} survive disambiguation "
          f"({sum(1 for f in winners if len(f.hits) > 1)} resolved from several starts)")

    done = annotated_in_src()
    progress = {}
    if (ROOT / "data/progress.csv").exists():
        with (ROOT / "data/progress.csv").open() as f:
            progress = {int(r["address"], 16): r for r in csv.DictReader(f)}
    previous = {}
    if CSV_OUT.exists():
        with CSV_OUT.open() as f:
            previous = {r["symbol"]: r for r in csv.DictReader(f)}

    units: dict[str, Unit] = {}
    for fn in winners:
        if fn.address in done:
            prev = previous.get(fn.symbol, {})
            fn.status = "landed" if prev.get("status") == "landed" else "annotated"
            fn.unverified = prev.get("unverified_refs", "")
            continue
        if fn.item is None:
            fn.status, fn.note = "not_in_cpp", "defined in a header or generated by the compiler"
            continue
        mod = module(fn.address)
        stem = module_name(fn.cpp)
        if mod == "?":
            lo, hi = module.window(fn.address)
            key = sort_key(stem)
            fits = (not lo or sort_key(lo) <= key) and (hi == "~" or key <= sort_key(hi))
            if not fits:
                fn.status, fn.note = "conflict", f"modules.csv says ? between {lo} and {hi}"
                continue
            mod = stem
        elif mod.lower() != stem.lower():
            fn.status, fn.note = "conflict", f"modules.csv says {mod}"
            continue
        name = mod.lower()
        path = ROOT / "src" / f"{name}.cpp"
        if name not in units:
            units[name] = Unit(name, path, source(fn.cpp), [],
                               path.read_text(encoding="latin-1") if path.exists() else None)
        elif units[name].src.path != fn.cpp:
            fn.status, fn.note = "conflict", f"src/{name}.cpp already takes {units[name].src.path.name}"
            continue
        units[name].fns.append(fn)
    for u in units.values():
        u.fns.sort(key=lambda f: f.address)

    if args.no_land:
        for name, u in sorted(units.items()):
            print(f"src/{name}.cpp: {len(u.fns)} functions from {u.src.path.name}")
        return
    matched_before = {a for a, r in progress.items() if r["result"] == "MATCH"}
    with ThreadPoolExecutor(args.jobs) as pool:
        list(pool.map(lambda u: land(u, matched_before), units.values()))
    for u in units.values():
        finish(u)

    rows = {}
    if only:
        rows = {sym: list(r.values()) for sym, r in previous.items()}
    for fn in fns:
        addr = f"0x{fn.address:08X}" if fn.address else ";".join(f"0x{a:08X}" for a in fn.hits)
        tib = fn.cpp.relative_to(TIBSUN).as_posix() if fn.cpp else f"({fn.obj}.obj)"
        note = "; ".join(n for n in ("placed by module" if fn.placed else "", fn.note) if n)
        rows[fn.symbol] = [addr, len(fn.data), tib, fn.symbol, fn.status, fn.unverified, note]
    rows = sorted(rows.values(), key=lambda r: (r[0], r[3]))
    with CSV_OUT.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "tibsun_file", "symbol", "status", "unverified_refs", "note"])
        w.writerows(rows)
    counts = {}
    for r in rows:
        counts[r[4]] = counts.get(r[4], 0) + 1
    landed = [fn for fn in fns if fn.status == "landed"]
    print("status counts:", counts)
    print(f"landed by tier0, all runs: {len(landed)} functions, {sum(len(f.data) for f in landed)} bytes")


if __name__ == "__main__":
    main()
