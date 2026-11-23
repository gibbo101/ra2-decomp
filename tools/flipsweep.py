"""Find the count of one diagnostic statement at which a function's near miss flips, as a target for natural source.

Usage: uv run tools/flipsweep.py SRC ADDR [--kind KIND] [--stmt TEXT] [--at POS] [--max K]
           [--kind2 KIND --stmt2 TEXT --at2 POS --max2 K2] [--decl TEXT] [--ignore REGEX] [--overlay DIR]

Copies SRC to build/flipsweep/ and, for k = 0..K, inserts k copies of one statement at one
place in the target's body, then compiles and checks each copy. Per k it prints the score,
the bytes, and a letter for the set of differing lines; the first k whose set differs from
k = 0's is the flip point. Some ties are thresholds on a count (named locals, instructions
in a block before scheduling, distinct expressions, inline sites): the score stays flat
until the flip, so this finds it in one run. --kind2 sweeps a second count against the first
as a grid. A flip is a target, not a fix: the probes never land; find natural source that
changes the count the same way.

kinds (each copy's index replaces {k}):
  local  a dead named, initialised local          int flip_local_{k} = {k};
  store  a one-instruction store to an extern int  Flip_Probe_Store_[{k}] = 0x7F00 + {k};
  call   a call of a trivial inline that folds     Inline_Probe_({k});
  expr   a dead distinct expression term           (void)(Flip_Probe_Value_ * ({k} + 3));
  stmt   --stmt TEXT (or --stmt2), declarations it needs from --decl
positions: start, end (before the body's last return), line:N (before SRC's line N),
  before:REGEX or after:REGEX (the first line of the target's body that matches).
The declarations a kind needs go above the first marker at every k, k = 0 included, so the
count of declarations before the target stays fixed; the `source` row has none of them.
Differing lines the probe itself adds (the store kind's 0x7Fxx constants, --ignore) are
left out of the sets.
"""
import argparse
import os
import re
import sys
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check  # noqa: E402
from coff import CoffObject  # noqa: E402
from inlineprobe import MARKER, helper, insertion, target_span  # noqa: E402

ROOT = check.ROOT
OUT = ROOT / "build/flipsweep"


@dataclass(frozen=True)
class Kind:
    stmt: str
    decls: tuple[str, ...] = ()
    ignore: str = ""


KINDS = {
    "local": Kind("int flip_local_{k} = {k};"),
    "store": Kind("Flip_Probe_Store_[{k}] = 0x7F00 + {k};", ("extern int Flip_Probe_Store_[];",),
                  r"\b0x7f[0-9a-f]{2}\b"),
    "call": Kind("Inline_Probe_({k});", (helper(0),)),
    "expr": Kind("(void)(Flip_Probe_Value_ * ({k} + 3));", ("extern int Flip_Probe_Value_;",)),
}


def kind_of(name: str, stmt: str | None, decls=()) -> Kind:
    """The kind a sweep inserts: a built-in one, or --stmt with --decl's declarations."""
    if name == "stmt":
        if not stmt:
            raise SystemExit("--kind stmt needs --stmt TEXT")
        return Kind(stmt, tuple(decls))
    return KINDS[name]


def copies(kind: Kind, n: int, indent: str = "\t") -> list[str]:
    """n copies of the kind's statement, numbered from 0."""
    return [indent + kind.stmt.replace("{k}", str(k)) for k in range(n)]


def with_decls(lines: list[str], decls) -> tuple[list[str], int]:
    """lines with each distinct declaration added above the first marker, and how many."""
    unique = list(dict.fromkeys(decls))
    first = next((i for i, line in enumerate(lines) if MARKER.match(line)), 0)
    return lines[:first] + unique + lines[first:], len(unique)


def position(lines: list[str], span: tuple[int, int], pos: str, shift: int = 0) -> int:
    """The line index the copies go before; line:N counts SRC's lines, `shift` the ones added above."""
    o, c = span
    if pos in ("start", "end"):
        return insertion(lines, span, pos)
    if pos.startswith("line:"):
        return int(pos[5:]) - 1 + shift
    where, _, pattern = pos.partition(":")
    if where in ("before", "after") and pattern:
        for i in range(o + 1, c):
            if re.search(pattern, lines[i]):
                return i if where == "before" else i + 1
        raise SystemExit(f"no line of the target's body matches {pattern!r}")
    raise SystemExit(f"unknown position {pos!r}")


def insert_all(lines: list[str], blocks) -> list[str]:
    """lines with each (index, block) inserted; equal indexes keep the blocks' order."""
    out = list(lines)
    for at, _, block in sorted(((at, n, block) for n, (at, block) in enumerate(blocks)), reverse=True):
        out[at:at] = block
    return out


def diff_set(diff: list[str], ignore: str = "") -> tuple:
    """The differing lines of a zero-context diff as a multiset, less added lines matching `ignore`."""
    pattern = re.compile(ignore, re.I) if ignore else None
    lines = Counter()
    for line in diff:
        if line[:1] not in "+-" or line.startswith(("+++", "---")):
            continue
        if line[0] == "+" and pattern and pattern.search(line):
            continue
        lines[line] += 1
    return tuple(sorted(lines.items()))


@dataclass
class Outcome:
    verdict: str
    ours: int = 0
    size: int = 0
    norm: float = 0.0
    lines: tuple = ()

    def __str__(self) -> str:
        if self.verdict == "FAIL":
            return "build failed"
        count = sum(n for _, n in self.lines)
        return f"{self.verdict} ({self.ours}/{self.size})  norm {self.norm * 100:.1f}%  {count} lines differ"


def verdict(res) -> str:
    """The checker's verdict for one compared function, as check.py prints it."""
    if res.matched:
        return "MATCH"
    return "WRONG TARGETS" if res.bytes_match else f"{res.ratio * 100:.1f}%"


@dataclass
class Build:
    path: Path
    obj: CoffObject
    raw: bytes
    local: dict
    names: dict
    log: str = ""


class Scorer:
    """Compiles probe copies and compares them as check.py does; its objects go when it closes."""

    def __init__(self, overlay: Path | None = None, out: Path = OUT):
        self.overlay = overlay
        self.out = out
        self.orig = check.Original()
        self.known = check.known_names()
        self.objs = set()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        for obj in self.objs:
            obj.unlink(missing_ok=True)

    def build(self, path: Path, extra: tuple[str, ...] = ()) -> Build | None:
        obj_path = self.out / (path.stem + ".obj")
        self.objs.add(obj_path)
        obj_file, log = check.compile_source(path, self.overlay, out=obj_path, extra=extra)
        if obj_file is None:
            print(log[-2000:], file=sys.stderr)
            return None
        raw = obj_file.read_bytes()
        obj = CoffObject(raw)
        try:
            local, _ = check.bind_annotations(self.orig, obj, raw, path)
        except SystemExit as gone:
            # A probe can stop another marked function being emitted; bind by known matches only.
            log += f"\n{gone}"
            local = check.known_decorated(self.orig.decorated_addresses)
        names = check.ReferenceNames(self.known, local, check.function_symbols(obj))
        return Build(path, obj, raw, local, names, log)

    def compare(self, build: Build, addr: int):
        name = dict(check.annotations(build.path)).get(addr)
        if name is None:
            raise SystemExit(f"{build.path}: no marker for 0x{addr:08X}")
        os.environ["CHECK_CTX"] = "0"
        return check.compare(self.orig, build.obj, build.raw, addr, name, build.names, build.local,
                             score=True, prefer=build.local)

    def score(self, path: Path, addr: int, ignore: str = "") -> Outcome:
        build = self.build(path)
        if build is None:
            return Outcome("FAIL")
        res = self.compare(build, addr)
        return Outcome(verdict(res), res.ours, res.size, res.norm_ratio, diff_set(res.diff, ignore))


def letters(sets: dict) -> dict:
    """A letter per distinct differing-line set, in order of first appearance."""
    names = {}
    for lines in sets.values():
        names.setdefault(lines, chr(ord("A") + len(names)) if len(names) < 26 else f"#{len(names)}")
    return {key: names[lines] for key, lines in sets.items()}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("addr")
    ap.add_argument("--kind", default="local", choices=[*KINDS, "stmt"])
    ap.add_argument("--stmt")
    ap.add_argument("--at", default="start")
    ap.add_argument("--max", type=int, default=16)
    ap.add_argument("--kind2", choices=[*KINDS, "stmt"])
    ap.add_argument("--stmt2")
    ap.add_argument("--at2")
    ap.add_argument("--max2", type=int, default=8)
    ap.add_argument("--decl", action="append", default=[], help="a declaration --stmt needs (repeatable)")
    ap.add_argument("--ignore", default="", help="leave added lines matching this out of the sets")
    ap.add_argument("--overlay", type=Path)
    a = ap.parse_args()
    src = Path(a.src) if Path(a.src).is_absolute() else ROOT / a.src
    addr = int(a.addr, 16)
    first = kind_of(a.kind, a.stmt, a.decl)
    second = kind_of(a.kind2, a.stmt2, a.decl) if a.kind2 else None
    ignore = "|".join(p for p in (first.ignore, second.ignore if second else "", a.ignore) if p)

    plain = src.read_text(errors="replace").split("\n")
    lines, shift = with_decls(plain, first.decls + (second.decls if second else ()))
    span = target_span(lines, addr)
    at1 = position(lines, span, a.at, shift)
    at2 = position(lines, span, a.at2 or a.at, shift) if second else None
    OUT.mkdir(parents=True, exist_ok=True)
    probe = OUT / src.name
    grid = [(k1, k2) for k1 in range(a.max + 1) for k2 in (range(a.max2 + 1) if second else [None])]
    results = {}
    with Scorer(a.overlay.resolve() if a.overlay else None) as scorer:
        try:
            probe.write_text("\n".join(plain))
            print(f"source       {scorer.score(probe, addr, ignore)}", flush=True)
            for k1, k2 in grid:
                blocks = [(at1, copies(first, k1))] + ([(at2, copies(second, k2))] if second else [])
                probe.write_text("\n".join(insert_all(lines, blocks)))
                results[(k1, k2)] = scorer.score(probe, addr, ignore)
                label = f"k={k1:2}" if not second else f"k={k1:2} k2={k2:2}"
                print(f"{label:12} {results[(k1, k2)]}", flush=True)
        finally:
            probe.unlink(missing_ok=True)
    report(results, second is not None)


def report(results: dict, is_grid: bool) -> None:
    tags = letters({key: (o.verdict == "FAIL", o.lines) for key, o in results.items()})
    clean = [key for key, o in results.items() if o.verdict not in ("FAIL", "MATCH") and not o.lines]
    if clean:
        print("only the probe's own lines differ at " + ", ".join(
            f"k={k1}" + (f" k2={k2}" if is_grid else "") for k1, k2 in clean))
    if not is_grid:
        print("sets: " + " ".join(f"{k}{tags[(k, None)]}" for k, _ in results))
        base = tags[(0, None)]
        flip = next((k for k, _ in results if tags[(k, None)] != base), None)
        print(f"flip point: k={flip} ({base} -> {tags[(flip, None)]})" if flip is not None
              else "no flip: every k gives k=0's set")
        return
    cols = sorted({k2 for _, k2 in results})
    print("sets   " + " ".join(f"k2={c:<3}" for c in cols))
    for k1 in sorted({k1 for k1, _ in results}):
        print(f"k={k1:<4} " + " ".join(f"{tags[(k1, c)]:6}" for c in cols))
    base = tags[(0, 0)]
    flips = [key for key in sorted(results) if tags[key] != base]
    print(f"first change from (0,0): k={flips[0][0]} k2={flips[0][1]} ({base} -> {tags[flips[0]]})"
          if flips else "no flip: every cell gives (0,0)'s set")


if __name__ == "__main__":
    sys.exit(main())
