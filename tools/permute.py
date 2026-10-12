# /// script
# requires-python = ">=3.11"
# dependencies = [
#     "capstone>=5",
#     "pefile>=2023.2.7",
#     "tree-sitter>=0.23",
#     "tree-sitter-cpp>=0.23",
# ]
# ///
# Adapted from Byte Tactics (tools/permute.py), whose tools/ are under this licence:
#
# MIT License
#
# Copyright (c) 2026 Hector Bailey
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
"""Search meaning-preserving rewrites of a near-miss function for one that matches.

    uv run tools/permute.py build/spike/0x00421EA0/anim.cpp 0x00421EA0 \\
        --overlay build/spike/0x00421EA0/include --minutes 30
    uv run tools/permute.py build/x/abstype.cpp 0x00410A60 --jobs 8 --seed 7

Arguments are check.py's: the source, the annotated address and an optional header
overlay. Each candidate applies one to four random rewrites from tools/permute_mutate.py
(statement and declaration order, commutative operands, temporaries, if/else polarity,
loop forms, casts, cached pointers, `x[i]` and `*(x + i)`, inline helpers, ...) to the
annotated function and the small inline helpers it calls, compiles it with check.py's
compile path in a parallel worker (each with its own header and build directory), and
scores it against the original. A hill climb with random restarts keeps the best
candidates and accepts equal scores, since near misses sit on plateaus. It stops at
MATCH or at the time limit.

The score is computed on check.py's normalised listing (addresses, jump targets and
stack-slot displacements masked): an instruction with other registers costs 5, other
operands 10, a moved one 60, an inserted or deleted one 100, each byte of length 2, and
each instruction that differs only in its stack slot 0.01. Lower is better; 0 is MATCH.

The input is never written. The result goes next to it:
  <name>.best.cpp    the whole file with the best version found, after cleanup
  <name>.best.json   start and best scores (normalised and raw), status, compiles, rewrites
Logs go to build/permute/<ADDR>/: log.txt (each gain), stats.json (per rewrite kind),
matches/ (every MATCHing candidate). A source under src/ or include/ is refused: copy it
to build/ first. Confirm a result with check.py before using it.
"""

from __future__ import annotations

import argparse
import bisect
import difflib
import hashlib
import json
import os
import random
import re
import shutil
import struct
import sys
import time
from collections import Counter
from concurrent.futures import FIRST_COMPLETED, ProcessPoolExecutor, wait
from dataclasses import dataclass, field
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).resolve().parent))

from check import (FORBIDDEN, PADDING, ROOT, Original, annotations, compare, compile_source,  # noqa: E402
                   known_decorated, known_names, normalised_listings, prepare_headers, similarity)
from coff import CoffObject  # noqa: E402
from permute_mutate import MUTATIONS, SIMPLIFY, Ctx, Mutator, TargetSpec, helper_names  # noqa: E402

OUT = ROOT / "build" / "permute"
INF = 10 ** 9

# --- scoring ---------------------------------------------------------------------

REG = re.compile(r"\b(?:e?[abcd]x|[abcd][lh]|e?[sd]i|e?bp)\b")
STACK = re.compile(r"\[(?:esp|ebp)[^\]]*\]")

PENALTY_REG = 5         # same instruction, different registers
PENALTY_OPERAND = 10    # same mnemonic, other operands
PENALTY_REORDER = 60    # an instruction that is in both, at different places
PENALTY_INSDEL = 100    # an instruction only one side has
PENALTY_BYTE = 2        # per byte of size difference
PENALTY_SLOT = 0.01     # an aligned instruction whose only difference is its stack slot


def instruction_penalty(x: str, y: str) -> int:
    return PENALTY_REG if REG.sub("R", x) == REG.sub("R", y) else PENALTY_OPERAND


def fine_score(theirs: list[str], ours: list[str], hot: set | None = None) -> int:
    """decomp-permuter style distance between two normalised instruction lists.
    `hot`, if given, collects the indexes of our instructions in differing blocks."""
    score = 0
    deleted: Counter = Counter()
    inserted: Counter = Counter()
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, theirs, ours, autojunk=False).get_opcodes():
        if tag == "equal":
            continue
        if hot is not None:
            hot.update(range(j1, j2))
            if j1 == j2 and ours:
                hot.add(min(j1, len(ours) - 1))
        a, b = theirs[i1:i2], ours[j1:j2]
        ma = [t.split(" ", 1)[0] for t in a]
        mb = [t.split(" ", 1)[0] for t in b]
        for t2, x1, x2, y1, y2 in difflib.SequenceMatcher(None, ma, mb, autojunk=False).get_opcodes():
            if t2 == "equal":
                for x, y in zip(a[x1:x2], b[y1:y2]):
                    if x != y:
                        score += instruction_penalty(x, y)
            else:
                deleted.update(a[x1:x2])
                inserted.update(b[y1:y2])
    # An unaligned instruction found on the other side is a move, not an insertion plus
    # a deletion; one moved with a register change costs a little more.
    for extra, key in ((0, lambda s: s), (PENALTY_REG, lambda s: REG.sub("R", s))):
        both = Counter(key(x) for x in deleted.elements()) & Counter(key(y) for y in inserted.elements())
        score += (PENALTY_REORDER + extra) * sum(both.values())
        deleted, inserted = unmatched_after(deleted, both, key), unmatched_after(inserted, both, key)
    score += PENALTY_INSDEL * (sum(deleted.values()) + sum(inserted.values()))
    return score


def unmatched_after(side: Counter, matched: Counter, key) -> Counter:
    budget = Counter(matched)
    out: Counter = Counter()
    for x in side.elements():
        if budget[key(x)] > 0:
            budget[key(x)] -= 1
        else:
            out[x] += 1
    return out


def slot_differences(norm_a, norm_b, semi_a, semi_b) -> int:
    """Instructions equal once stack slots are masked but not before."""
    n = 0
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, norm_a, norm_b, autojunk=False).get_opcodes():
        if tag == "equal":
            n += sum(1 for k in range(i2 - i1) if semi_a[i1 + k] != semi_b[j1 + k])
    return n


def line_table(raw: bytes, sec_number: int, symbol: str) -> list[tuple[int, int]]:
    """(section offset, source line) for one function, from the COFF line numbers /Zd adds."""
    try:
        _, _, _, symptr, nsym, opt, _ = struct.unpack_from("<HHIIIHH", raw, 0)
        hdr = 20 + opt + 40 * (sec_number - 1)
        (plines,) = struct.unpack_from("<I", raw, hdr + 28)
        (nlines,) = struct.unpack_from("<H", raw, hdr + 34)
        strtab = symptr + 18 * nsym

        def sym(i):
            name, _, _, _, _, naux = struct.unpack_from("<8sIhHBB", raw, symptr + 18 * i)
            if name[:4] == b"\0\0\0\0":
                (o,) = struct.unpack_from("<I", name, 4)
                name = raw[strtab + o:raw.index(b"\0", strtab + o)]
            return name.rstrip(b"\0").decode("latin-1"), naux

        out, base = [], None
        for k in range(nlines):
            addr, line = struct.unpack_from("<IH", raw, plines + 6 * k)
            if line == 0:
                base = None
                name, naux = sym(addr)
                if name != symbol:
                    continue
                j = addr + 1 + naux
                while j < nsym:
                    n2, na2 = sym(j)
                    if n2 == ".bf":
                        (base,) = struct.unpack_from("<H", raw, symptr + 18 * (j + 1) + 4)
                        break
                    j += 1 + na2
            elif base is not None:
                out.append((addr, base + line))
        return sorted(out)
    except (struct.error, ValueError, IndexError):
        return []


@dataclass
class Score:
    value: float          # lower is better, 0 is a MATCH
    norm: float           # normalised similarity
    ratio: float          # check.py's raw similarity
    status: str           # match | bytes | partial | compile | error | guard
    size: int = 0
    note: str = ""
    hot: tuple = ()       # source lines (1-based) behind differing instructions

    @property
    def pct(self) -> str:
        return f"{self.norm * 100:.1f}% (raw {self.ratio * 100:.1f}%)"


def picked_function(obj: CoffObject, symbol: str):
    for sym, sec, start, end in obj.functions():
        if sym.name == symbol:
            ours = sec.data[start:end]
            relocated = {r.offset - start + k for r in sec.relocs if start <= r.offset < end for k in range(4)}
            while ours and ours[-1] in PADDING and len(ours) - 1 not in relocated:
                ours = ours[:-1]
            return sec, start, ours
    return None


class Scorer:
    """Scores compiled objects for one address, the way check.py compares."""

    def __init__(self, address: int, qualname: str, local: dict[str, int], guards: list[tuple[int, str]]):
        self.address, self.qualname, self.local, self.guards = address, qualname, local, guards
        self.orig = Original()
        self.names = known_names()
        self.theirs = self.orig.read(address, self.orig.sizes[address])

    def score(self, raw: bytes) -> Score:
        obj = CoffObject(raw)
        for g_addr, g_name in self.guards:
            if not compare(self.orig, obj, raw, g_addr, g_name, {}, {}).bytes_match:
                return Score(INF, 0.0, 0.0, "guard", note=f"0x{g_addr:08X} no longer matches")
        res = compare(self.orig, obj, raw, self.address, self.qualname, self.names, self.local)
        if res.matched:
            return Score(0, 1.0, 1.0, "match", res.ours)
        if res.bytes_match:
            return Score(0.5, 1.0, 1.0, "bytes", res.ours, "; ".join(res.refs)[:300])
        sec, start, ours = picked_function(obj, res.symbol)
        semi_a, semi_b = normalised_listings(self.orig, obj, sec, start, ours, self.address, self.theirs, False)
        norm_a, norm_b = normalised_listings(self.orig, obj, sec, start, ours, self.address, self.theirs)
        hot_idx: set = set()
        value = (fine_score(norm_a, norm_b, hot_idx) + PENALTY_SLOT * slot_differences(norm_a, norm_b, semi_a, semi_b)
                 + PENALTY_BYTE * abs(len(ours) - len(self.theirs)))
        hot: tuple = ()
        if hot_idx:
            table = line_table(raw, sec.number, res.symbol)
            if table:
                md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
                offs_ins = [ins.address for ins in md.disasm(ours, 0)]
                offs = [a for a, _ in table]
                lines = set()
                for i in hot_idx:
                    if i < len(offs_ins):
                        k = bisect.bisect_right(offs, start + offs_ins[i]) - 1
                        if k >= 0:
                            lines.add(table[k][1])
                hot = tuple(sorted(lines))
        return Score(max(value, 1), similarity(norm_a, norm_b), res.ratio, "partial", len(ours), hot=hot)


# --- worker processes ---------------------------------------------------------------

_W: dict = {}


def worker_init(address: int, qualname: str, local, guards, spec: TargetSpec, base_text: str, basename: str,
                overlay: Path | None, weights: dict | None, focus: bool):
    try:
        os.nice(10)
    except OSError:
        pass
    work = OUT / f"0x{address:08X}" / "work" / str(os.getpid())
    work.mkdir(parents=True, exist_ok=True)
    headers = prepare_headers(overlay)
    (work / "headers.txt").write_text(str(headers))
    simplifier = Mutator(base_text, spec, {k: SIMPLIFY.get(k, 0) for k in MUTATIONS})
    simplifier.simplify = True
    _W.update(scorer=Scorer(address, qualname, local, guards), mutator=Mutator(base_text, spec, weights),
              simplifier=simplifier, base_lines=base_text.splitlines(), path=work / basename,
              obj=work / (Path(basename).stem + ".obj"), headers=headers, cache={},
              extra=("/Zd",) if focus else (), volatiles=base_text.count("volatile"))


WORD = re.compile(r"\w+|->|::|\+\+|--|&&|\|\||<<=|>>=|[-+*/%&|^!<>=]=|<<|>>|\S")


def distance(base_lines: list[str], text: str) -> int:
    """How far text is from the starting file: tokens changed, ignoring spacing."""
    b = text.splitlines()
    d = 0
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, base_lines, b, autojunk=False).get_opcodes():
        if tag == "equal":
            continue
        ta = WORD.findall("\n".join(base_lines[i1:i2]))
        tb = WORD.findall("\n".join(b[j1:j2]))
        for t, x1, x2, y1, y2 in difflib.SequenceMatcher(None, ta, tb, autojunk=False).get_opcodes():
            if t != "equal":
                d += max(x2 - x1, y2 - y1)
    return d


def compile_and_score(text: str) -> Score:
    if FORBIDDEN.search(text) or text.count("volatile") > _W["volatiles"]:
        return Score(INF, 0.0, 0.0, "compile", note="breaks a source rule")
    path: Path = _W["path"]
    path.write_text(text, encoding="latin-1")
    obj_path, log = compile_source(path, headers=_W["headers"], out=_W["obj"], extra=_W["extra"])
    if obj_path is None:
        err = next((ln for ln in log.splitlines() if "error" in ln), log.strip()[-200:])
        return Score(INF, 0.0, 0.0, "compile", note=err[-200:])
    try:
        return _W["scorer"].score(obj_path.read_bytes())
    except (Exception, SystemExit) as exc:  # a broken object must not stop the search
        return Score(INF, 0.0, 0.0, "error", note=repr(exc)[:200])


def evaluate(parent: str, n_mut: int, seed: int, simplify: bool = False, hot: tuple = ()) -> dict:
    """Mutate parent n_mut times (0: score it as is), compile and score."""
    rng = random.Random(seed)
    cache: dict = _W["cache"]
    mutator = _W["simplifier"] if simplify else _W["mutator"]
    text, names = parent, []
    if n_mut:
        for _ in range(4):
            text, names = mutator.mutate(parent, rng, n_mut, hot=hot)
            if names and digest(text) not in cache:
                break
        if not names:
            return {"text": None, "names": [], "score": None}
    h = digest(text)
    dist = distance(_W["base_lines"], text)
    if h in cache:
        return {"text": text, "names": names, "score": cache[h], "dup": True, "dist": dist}
    score = compile_and_score(text)
    cache[h] = score
    return {"text": text, "names": names, "score": score, "dup": False, "dist": dist}


def digest(text: str) -> str:
    return hashlib.sha1(text.encode("latin-1", "replace")).hexdigest()


# --- the search ------------------------------------------------------------------------


@dataclass
class Entry:
    text: str
    score: Score
    serial: int
    lineage: list[str] = field(default_factory=list)

    def key(self):
        # lower score first; on a tie the better normalised ratio, then the newest
        return (self.score.value, -self.score.norm, -self.serial)


def acceptable(s: Score, target: Score) -> bool:
    """Is s at least as good as target, by the score and by the normalised similarity?"""
    if target.status == "match":
        return s.status == "match"
    if target.status == "bytes":
        return s.status in ("match", "bytes")
    return s.status in ("partial", "match", "bytes") and s.value <= target.value and s.norm >= target.norm - 1e-9


def simplify(pool, text: str, target: Score, jobs: int, seconds: float, counter: Counter,
             say=print) -> tuple[str, Score]:
    """Undo the rewrites the score does not need: keep each meaning-preserving rewrite
    that brings the text closer to the start without losing score."""
    cur, cur_score = text, target
    cur_dist = pool.submit(evaluate, text, 0, 0).result()["dist"]
    start_dist = cur_dist
    rng = random.Random(0x5eed)
    t0 = time.time()
    fails = 0
    inflight: set = set()
    while time.time() - t0 < seconds and fails < 2000 and cur_dist > 0:
        while len(inflight) < jobs * 2:
            n = 1 if rng.random() < 0.7 else 2
            inflight.add(pool.submit(evaluate, cur, n, rng.getrandbits(62), True))
        done, _ = wait(list(inflight), timeout=5, return_when=FIRST_COMPLETED)
        for fut in done:
            inflight.discard(fut)
            r = fut.result()
            if r["text"] is not None and not r.get("dup"):
                counter["compiles"] += 1
            if r["text"] is not None and r["dist"] < cur_dist and acceptable(r["score"], target):
                cur, cur_score, cur_dist = r["text"], r["score"], r["dist"]
                fails = 0
            else:
                fails += 1
    for fut in inflight:
        fut.cancel()
    for fut in wait(list(inflight)).done:
        if not fut.cancelled() and fut.result()["text"] is not None and not fut.result().get("dup"):
            counter["compiles"] += 1
    say(f"  cleanup: {start_dist} -> {cur_dist} tokens changed from the start, in {time.time() - t0:.0f} s")
    return cur, cur_score


def same_code_with_lines(src: Path, overlay: Path | None, symbols: list[str]) -> bool:
    """Does /Zd leave the scored functions' bytes alone in this file?"""
    objs = []
    for extra in ((), ("/Zd",)):
        out = OUT / "focus" / f"{os.getpid()}{len(extra)}.obj"
        path, _ = compile_source(src, overlay, out=out, extra=extra)
        if path is None:
            return False
        objs.append({sym.name: sec.data[start:end] for sym, sec, start, end in CoffObject(path.read_bytes()).functions()})
        path.unlink(missing_ok=True)
    return all(s in objs[0] and objs[0][s] == objs[1].get(s) for s in symbols)


def pick_mutation_count(rng: random.Random) -> int:
    r = rng.random()
    return 1 if r < 0.55 else 2 if r < 0.8 else 3 if r < 0.93 else 4


def permute(src: Path, address: int, overlay: Path | None, minutes: float, jobs: int, seed: int | None,
            elite_size: int = 12, keep_going: bool = False, weights: dict | None = None,
            cleanup: float = 2.0, stall: float = 0.0, focus: bool = True) -> int:
    say = lambda *a: print(*a, flush=True)  # noqa: E731
    tag = f"0x{address:08X}"
    out = OUT / tag
    (out / "matches").mkdir(parents=True, exist_ok=True)
    best_path = src.with_name(src.stem + ".best.cpp")
    json_path = src.with_name(src.stem + ".best.json")
    base_text = src.read_text(encoding="latin-1")
    ann = annotations(src)
    qualname = next((q for a, q in ann if a == address), None)
    if qualname is None:
        raise SystemExit(f"{src}: no // FUNCTION: {tag}")
    spec = TargetSpec(address)
    spec.helpers = helper_names(base_text, spec)

    # Other annotated functions in the file whose bytes match now must keep matching.
    orig = Original()
    base_obj_path, log = compile_source(src, overlay, out=out / "base.obj")
    if base_obj_path is None:
        raise SystemExit("the starting file does not compile:\n" + log)
    raw = base_obj_path.read_bytes()
    base_obj = CoffObject(raw)
    guards, symbols = [], []
    local = known_decorated()
    for a, q in ann:
        res = compare(orig, base_obj, raw, a, q, {}, {})
        local[res.symbol] = a
        if a == address or res.bytes_match:
            symbols.append(res.symbol)
        if a != address and res.bytes_match:
            guards.append((a, q))
    # #line renumbers the COFF line table, so hot lines would point at the wrong statements.
    focus = focus and not re.search(r"^\s*#\s*line\b", base_text, re.M) \
        and same_code_with_lines(src, overlay, symbols)

    rng = random.Random(seed)
    mutator = Mutator(base_text, spec, weights)
    ctx = Ctx(base_text, spec, mutator.fi, rng)
    if ctx.error or not ctx.funcs:
        raise SystemExit(ctx.error or "no function to mutate")
    say(f"{tag}: mutating {', '.join(f.name for f in ctx.funcs)} in {src}"
        + (f"; guarding {', '.join(f'0x{a:08X}' for a, _ in guards)}" if guards else "")
        + ("" if focus else "; no line numbers, so no focus"))

    stats: dict[str, Counter] = {k: Counter() for k in MUTATIONS}
    counter: Counter = Counter()
    t0 = time.time()
    serial = 0
    seen: set[str] = set()
    elite: list[Entry] = []
    matches = 0
    last_gain = t0
    lowest = INF

    def summary(final: bool, start: Score, best: Entry) -> dict:
        return {
            "address": tag, "source": str(src), "status": best.score.status,
            "start": {"score": start.value, "normalised": round(start.norm * 100, 2),
                      "raw": round(start.ratio * 100, 2), "bytes": start.size},
            "best": {"score": best.score.value, "normalised": round(best.score.norm * 100, 2),
                     "raw": round(best.score.ratio * 100, 2), "bytes": best.score.size, "note": best.score.note},
            "compiles": counter["compiles"], "compile_errors": counter["compile_errors"],
            "duplicates": counter["duplicates"], "minutes": round((time.time() - t0) / 60, 2),
            "matches": matches, "final": final, "focus": focus, "helpers": spec.helpers,
            "guards": [f"0x{a:08X}" for a, _ in guards], "lineage": best.lineage[-200:],
        }

    def write_best(e: Entry, start: Score, final: bool):
        best_path.write_text(e.text, encoding="latin-1")
        json_path.write_text(json.dumps(summary(final, start, e), indent=1) + "\n")

    def add_elite(e: Entry):
        elite.append(e)
        elite.sort(key=Entry.key)
        del elite[elite_size:]

    with ProcessPoolExecutor(max_workers=jobs, initializer=worker_init,
                             initargs=(address, qualname, local, guards, spec, base_text, src.name, overlay,
                                       weights, focus)) as pool:
        first = pool.submit(evaluate, base_text, 0, 0).result()
        counter["compiles"] += 1
        start = first["score"]
        if start.status in ("compile", "error", "guard"):
            raise SystemExit(f"cannot score the starting file: {start.note}")
        best = Entry(base_text, start, serial)
        lowest = start.value
        elite.append(best)
        seen.add(digest(base_text))
        with (out / "log.txt").open("a") as fh:
            fh.write(f"# {time.strftime('%Y-%m-%d %H:%M')} start {src} at {start.pct}, score {start.value:g}\n")
        say(f"{tag}: start {start.pct}, score {start.value:g}, {start.size} bytes")
        if start.status in ("match", "bytes"):
            write_best(best, start, True)
            return 0

        inflight: dict = {}

        def submit():
            r = rng.random()
            if r < 0.06:
                parent = Entry(base_text, start, 0, [])
                n = pick_mutation_count(rng) + 1
            elif r < 0.5:
                parent = elite[0]
                n = pick_mutation_count(rng)
            else:
                parent = rng.choices(elite, [1.0 / (i + 1) for i in range(len(elite))])[0]
                n = pick_mutation_count(rng)
            fut = pool.submit(evaluate, parent.text, n, rng.getrandbits(62), False, parent.score.hot)
            inflight[fut] = parent

        def time_left() -> bool:
            idle = (time.time() - last_gain) / 60
            return (time.time() - t0) / 60 < minutes and not (stall > 0 and idle >= stall)

        done_matching = False
        while time_left() and not done_matching:
            while len(inflight) < jobs * 2:
                submit()
            done, _ = wait(list(inflight), timeout=5, return_when=FIRST_COMPLETED)
            for fut in done:
                parent = inflight.pop(fut)
                try:
                    res = fut.result()
                except Exception as exc:  # a crashed worker task: note it and go on
                    say(f"{tag}: worker error {exc!r}")
                    continue
                if res["text"] is None:
                    continue
                names = res["names"]
                sc: Score = res["score"]
                h = digest(res["text"])
                if res.get("dup") or h in seen:
                    counter["duplicates"] += 1
                    continue
                seen.add(h)
                counter["compiles"] += 1
                for k in set(names):
                    stats[k]["tried"] += 1
                if sc.status in ("compile", "guard", "error"):
                    counter["compile_errors" if sc.status == "compile" else sc.status] += 1
                    for k in set(names):
                        stats[k][sc.status] += 1
                    continue
                for k in set(names):
                    stats[k]["compiled"] += 1
                    if sc.value < parent.score.value:
                        stats[k]["improved"] += 1
                    elif sc.value == parent.score.value:
                        stats[k]["equal"] += 1
                serial += 1
                if sc.value < lowest:
                    lowest = sc.value
                    last_gain = time.time()
                e = Entry(res["text"], sc, serial, parent.lineage + names)
                worst = elite[-1].key() if len(elite) >= elite_size else None
                if sc.value <= parent.score.value or worst is None or e.key() < worst:
                    add_elite(e)
                if sc.status == "match":
                    matches += 1
                    (out / "matches" / f"match_{matches:03d}.cpp").write_text(res["text"], encoding="latin-1")
                # best.cpp never falls below the start's normalised similarity
                if e.key() < best.key() and (sc.value < best.score.value or sc.norm > best.score.norm) \
                        and sc.norm >= start.norm - 1e-9:
                    for k in set(names):
                        stats[k]["new_best"] += 1
                    line = (f"{(time.time() - t0) / 60:6.2f} min  score {parent.score.value:g} -> {sc.value:g}"
                            f"  (best was {best.score.value:g}) {sc.pct}  {sc.size} bytes  {'+'.join(names)}")
                    with (out / "log.txt").open("a") as fh:
                        fh.write(line + "\n")
                    best = e
                    write_best(best, start, False)
                    say(f"{tag}: {line}")
                    if sc.status in ("match", "bytes") and not keep_going:
                        done_matching = True
        for fut in inflight:
            fut.cancel()
        wait(list(inflight))
        search_minutes = (time.time() - t0) / 60
        search_compiles = counter["compiles"]
        if best.text != base_text and cleanup > 0:
            text, sc = simplify(pool, best.text, best.score, jobs, cleanup * 60, counter, say)
            best = Entry(text, sc, best.serial, best.lineage)
        pool.shutdown(wait=True, cancel_futures=True)
    for note in (out / "work").glob("*/headers.txt"):
        shutil.rmtree(note.read_text(), ignore_errors=True)
        note.unlink()

    write_best(best, start, True)
    (out / "stats.json").write_text(json.dumps({k: dict(v) for k, v in stats.items() if v}, indent=1) + "\n")
    rate = search_compiles / search_minutes if search_minutes else 0
    say(f"{tag}: done after {(time.time() - t0) / 60:.1f} min, {counter['compiles']} compiles "
        f"({counter['compile_errors']} failed, {counter['duplicates']} duplicates skipped, {rate:.0f} a minute "
        f"while searching): {start.pct} -> {best.score.pct}, score {start.value:g} -> {best.score.value:g}"
        + (" MATCH" if best.score.status == "match" else ""))
    say(f"{tag}: wrote {best_path}")
    return 0 if best.score.status == "match" else 1


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", type=Path)
    ap.add_argument("address", type=lambda s: int(s, 16))
    ap.add_argument("--overlay", type=Path, help="header directory over include/, as for check.py")
    ap.add_argument("--minutes", type=float, default=15.0, help="time limit (default 15)")
    ap.add_argument("--stall", type=float, default=0.0, help="stop once this many minutes pass without a gain")
    ap.add_argument("--jobs", type=int, default=min(12, os.cpu_count() or 1), help="parallel compiles (default 12)")
    ap.add_argument("--seed", type=int)
    ap.add_argument("--no-focus", action="store_true",
                    help="do not aim rewrites at the source lines behind differing instructions")
    ap.add_argument("--keep-going", action="store_true", help="do not stop at the first MATCH")
    ap.add_argument("--only", help="comma-separated rewrite kinds to use (tools/permute_mutate.py MUTATIONS)")
    ap.add_argument("--cleanup", type=float, default=2.0,
                    help="minutes spent undoing rewrites the best version does not need (default 2, 0: none)")
    args = ap.parse_args()

    src = args.source.resolve()
    if any(d in src.parents for d in (ROOT / "src", ROOT / "include", ROOT / "reference")):
        ap.error(f"{args.source} is a tracked source: copy it to build/ and permute the copy")
    weights = None
    if args.only:
        kinds = set(args.only.split(","))
        if kinds - set(MUTATIONS):
            ap.error(f"unknown rewrite kinds: {', '.join(sorted(kinds - set(MUTATIONS)))}")
        weights = {k: (w if k in kinds else 0) for k, (_, w) in MUTATIONS.items()}
    try:
        os.nice(10)
    except OSError:
        pass
    overlay = args.overlay.resolve() if args.overlay else None
    raise SystemExit(permute(src, args.address, overlay, args.minutes, args.jobs, args.seed,
                             keep_going=args.keep_going, weights=weights, cleanup=args.cleanup,
                             stall=args.stall, focus=not args.no_focus))


if __name__ == "__main__":
    main()
