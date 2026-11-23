"""List where one function's inlining differs from the exe's, and measure the budget slack at each site.

Usage: uv run tools/inlinebudget.py ADDR [--src SRC] [--overlay DIR] [--max 12] [--spend 8]
           [--site-line N] [--no-measure]

Compiles SRC (default: the file holding ADDR's marker) and lines up the exe's and ours'
out-of-line calls, resolved to their targets as check.py does. A callee called a different
number of times on the two sides at an unmatched call is a differing decision: the exe calls
it where ours inlines it, or the reverse. Each is tied to the statement of the target's body
it expands under, from the compiler's listing (--site-line N overrides it with SRC's line N).

For each site, copies of SRC in build/inlinebudget/ add probes until that decision flips:
  ours inlines, exe calls:  k trivial inline calls after the statement (k more sites to the
    end of the body), k at the end of each body in SRC that the statement calls, and j spend
    probes (a call of an inline whose body is S one-unit calls) just before the statement:
    the slack is (j-1)*S to j*S units of the pool left at the site;
  ours calls, exe inlines:  k trivial inline calls before the statement, each one unit of own
    size: the shortfall is 2(k-1) to 2k units while the pool is above its 166-unit floor.
Each line gives the probe count and how many decisions still differ there. Every probe is
also a site for the calls before it, so a count at which other decisions changed too is a
rough measure. The probes are diagnostics: they never land.
"""
import argparse
import csv
import difflib
import re
import sys
from collections import Counter
from dataclasses import dataclass, replace
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check  # noqa: E402
from exeindex import ExeIndex  # noqa: E402
from flipsweep import KINDS, Kind, Scorer, copies, insert_all, with_decls  # noqa: E402
from inlineprobe import callee_spans, insertion, target_span  # noqa: E402

ROOT = check.ROOT
OUT = ROOT / "build/inlinebudget"
CALL = re.compile(r"^(call|jmp) (?:0x([0-9a-f]+)|<unresolved:([^>]+)>)", re.I)
PROBE = "Inline_Probe"
LISTING_LINE = re.compile(r"^; (\d+)\s*:")
LISTING_CODE = re.compile(r"^\s{2}([0-9a-fA-F]{5,8})\s")


def spend_kind(units: int) -> Kind:
    """A call of an inline whose body is `units` one-unit calls of an extern."""
    body = " ".join("Inline_Probe_Sink_(1);" for _ in range(units))
    return Kind("Inline_Probe_Spend_({k});",
                ("void Inline_Probe_Sink_(int a);",
                 f"static inline void Inline_Probe_Spend_(int a) {{ {body} }}"))


@dataclass(frozen=True)
class Call:
    index: int
    at: int
    key: object
    text: str
    defined: bool = False


@dataclass(frozen=True)
class Site:
    side: str
    callee: object
    call: Call
    anchor: int | None

    @property
    def ident(self):
        return (self.side, self.call.at) if self.side == "exe" else (self.side, self.callee, self.anchor)


def calls(listing: list[str], insns, resolve=None, defined=frozenset()) -> list[Call]:
    """The direct calls and tail jumps out of the function, keyed by target address or symbol;
    `defined` names the functions the object has bodies for."""
    out = []
    for n, (line, ins) in enumerate(zip(listing, insns)):
        m = CALL.match(line)
        if not m:
            continue
        if m[2]:
            label = re.search(r"\(([^()]*)\)$", line)
            key, symbols = int(m[2], 16), (label[1].split(", ") if label else ())
        else:
            key, symbols = (resolve(m[3]) if resolve else None) or m[3], (m[3],)
        out.append(Call(n, ins.address, key, line, any(s in defined for s in symbols)))
    return out


def paired(theirs: list[Call], ours: list[Call]) -> list[Call]:
    """ours with each unresolved callee keyed by the exe target it lines up with one to one."""
    matcher = difflib.SequenceMatcher(None, [c.key for c in theirs], [c.key for c in ours], autojunk=False)
    mapping = {}
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "replace" and i2 - i1 == j2 - j1:
            for e, o in zip(theirs[i1:i2], ours[j1:j2]):
                if isinstance(o.key, str) and isinstance(e.key, int):
                    mapping.setdefault(o.key, e.key)
    return [replace(c, key=mapping.get(c.key, c.key)) for c in ours]


def differing(theirs: list[Call], ours: list[Call]) -> list[Site]:
    """Unmatched calls whose callee the two sides call a different number of times. A call of
    ours counts only to a function the object defines: an extern has no body to inline."""
    ours = paired(theirs, [c for c in ours if PROBE not in str(c.key)])
    count_e, count_o = Counter(c.key for c in theirs), Counter(c.key for c in ours)
    matcher = difflib.SequenceMatcher(None, [c.key for c in theirs], [c.key for c in ours], autojunk=False)
    sites, anchor = [], None
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            anchor = theirs[i2 - 1].at
            continue
        for c in theirs[i1:i2]:
            if count_e[c.key] > count_o[c.key]:
                sites.append(Site("exe", c.key, c, anchor))
        for c in ours[j1:j2]:
            if c.defined and count_o[c.key] > count_e[c.key]:
                sites.append(Site("ours", c.key, c, anchor))
    return sites


def listing_lines(text: str, symbol: str) -> dict[int, int]:
    """Instruction offset -> the source line the listing shows above it, for one function."""
    out, line, active = {}, None, False
    for row in text.splitlines():
        if not active:
            active = bool(re.match(re.escape(symbol) + r"\s+PROC\b", row))
            continue
        if re.match(re.escape(symbol) + r"\s+ENDP\b", row):
            break
        m = LISTING_LINE.match(row)
        if m:
            line = int(m[1])
            continue
        m = LISTING_CODE.match(row)
        if m and line is not None:
            out[int(m[1], 16)] = line
    return out


def line_at(lines: dict[int, int], offset: int) -> int | None:
    before = [o for o in lines if o <= offset]
    return lines[max(before)] if before else None


def statement_end(lines: list[str], start: int) -> int:
    """Index of the last line of the statement that starts on line `start`."""
    paren = brace = 0
    opened = False
    for i in range(start, len(lines)):
        text = re.sub(r"//.*|\"(?:\\.|[^\"])*\"|'(?:\\.|[^'])*'", "", lines[i])
        for ch in text:
            paren += {"(": 1, ")": -1}.get(ch, 0)
            if ch == "{":
                brace += 1
                opened = True
            elif ch == "}":
                brace -= 1
                if brace <= 0 and opened:
                    return i
            elif ch == ";" and paren == 0 and brace == 0:
                return i
    return start


def site_line(site: Site, theirs: list[str], ours: list[str], ours_insns, lines: dict[int, int], addr: int):
    """The source line ours' code for a site sits under: the call itself, or the code ours
    has where the exe's call is (between the matched instructions around it)."""
    if site.side == "ours":
        return line_at(lines, site.call.at - addr)
    matched = {}
    for a, b, n in difflib.SequenceMatcher(None, check.mask_stack(theirs), check.mask_stack(ours),
                                           autojunk=False).get_matching_blocks():
        matched.update({a + k: b + k for k in range(n)})
    i = site.call.index
    prev = max((e for e in matched if e < i), default=None)
    nxt = min((e for e in matched if e > i), default=None)
    lo = matched[prev] + 1 if prev is not None else 0
    hi = matched[nxt] if nxt is not None else len(ours)
    j = lo if lo < hi else max(lo - 1, 0)
    return line_at(lines, ours_insns[j].address - addr)


class Analysis:
    """The differing decisions of one compiled copy."""

    def __init__(self, scorer: Scorer, build, addr: int, resolve):
        res = scorer.compare(build, addr)
        self.res = res
        pick = next(f for f in build.obj.functions() if f[0].name == res.symbol)
        self.symbol = res.symbol
        _, sec, lo, _ = pick
        ours, _ = check.function_bytes(pick)
        theirs = scorer.orig.read(addr, res.size)
        self.theirs, self.ours = check.aligned_listings(scorer.orig, build.obj, sec, lo, ours, addr, theirs,
                                                        build.local, build.names)
        linked = check.placeholder_linked(scorer.orig, build.obj, sec, lo, ours, addr, len(theirs))
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.theirs_insns = list(md.disasm(theirs, addr))
        self.ours_insns = list(md.disasm(linked, addr))
        self.exe_calls = calls(self.theirs, self.theirs_insns)
        self.our_calls = calls(self.ours, self.ours_insns, resolve, {f[0].name for f in build.obj.functions()})
        self.sites = differing(self.exe_calls, self.our_calls)
        self.probe_calls = sum(1 for c in self.our_calls if PROBE in str(c.key) and "Sink" not in str(c.key))


def default_source(addr: int) -> Path:
    with (ROOT / "data/progress.csv").open() as f:
        for r in csv.DictReader(f):
            if int(r["address"], 16) == addr and r.get("source"):
                return ROOT / r["source"]
    pattern = re.compile(rf"//\s*(?:FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY):\s*GAMEMD\s+0x0*{addr:X}\b", re.I)
    for path in sorted((ROOT / "src").rglob("*.cpp")):
        if pattern.search(path.read_text(errors="replace")):
            return path
    raise SystemExit(f"no source holds 0x{addr:08X}'s marker: pass --src")


def plural(n: int, noun: str) -> str:
    return f"{n} {noun}" + ("" if n == 1 else "s")


def describe(index: ExeIndex, key) -> str:
    return f"0x{key:08X} {index.name(key)}" if isinstance(key, int) else str(key)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("addr")
    ap.add_argument("--src")
    ap.add_argument("--overlay", type=Path)
    ap.add_argument("--max", type=int, default=12)
    ap.add_argument("--spend", type=int, default=8)
    ap.add_argument("--site-line", type=int)
    ap.add_argument("--no-measure", action="store_true")
    a = ap.parse_args()
    addr = int(a.addr, 16)
    src = (Path(a.src) if Path(a.src).is_absolute() else ROOT / a.src) if a.src else default_source(addr)
    OUT.mkdir(parents=True, exist_ok=True)
    with Scorer(a.overlay.resolve() if a.overlay else None, OUT) as scorer:
        budget = Budget(scorer, src, addr, a)
        try:
            budget.run()
        finally:
            budget.probe.unlink(missing_ok=True)


class Budget:
    """One function's differing decisions, and the probe sweeps that measure each one's slack."""

    def __init__(self, scorer: Scorer, src: Path, addr: int, args):
        self.scorer, self.addr, self.args = scorer, addr, args
        self.index = ExeIndex()
        by_name = Counter(self.index.names.values())
        self.lookup = {n: at for at, n in self.index.names.items() if by_name[n] == 1}
        self.fold, self.spend = KINDS["call"], spend_kind(args.spend)
        self.plain = src.read_text(errors="replace").split("\n")
        self.lines, self.shift = with_decls(self.plain, self.fold.decls + self.spend.decls)
        self.probe = OUT / src.name

    def resolve(self, symbol: str):
        return self.lookup.get(check.qualified(symbol))

    def analyse(self, text: list[str], extra: tuple[str, ...] = ()) -> Analysis | None:
        self.probe.write_text("\n".join(text))
        build = self.scorer.build(self.probe, extra)
        return Analysis(self.scorer, build, self.addr, self.resolve) if build else None

    def run(self) -> None:
        base = self.analyse(self.plain)
        if base is None:
            raise SystemExit("the source does not compile")
        ours = len([c for c in base.our_calls if PROBE not in str(c.key)])
        print(f"0x{self.addr:08X} {base.symbol}: {base.res.ours}/{base.res.size} bytes, "
              f"{len(base.exe_calls)} exe calls, {ours} ours")
        if not base.sites:
            print("no differing inlining decisions")
            return
        print(f"{plural(len(base.sites), 'differing decision')}:")
        for s in base.sites:
            where = f"exe 0x{s.call.at:08X}" if s.side == "exe" else f"ours +0x{s.call.at - self.addr:X}"
            print(f"  {describe(self.index, s.callee)} at {where}: {direction(s)}")
        if self.args.no_measure:
            return
        ref = self.analyse(self.lines)
        asm = OUT / (self.probe.stem + ".asm")
        listed = self.analyse(self.lines, ("/FAcs", "/Fa" + check.winpath(asm)))
        if ref is None or listed is None:
            raise SystemExit("the source does not compile with the probes' declarations")
        if listed.ours != ref.ours:
            print("note: the listing build's code differs, so its line numbers may be off")
        if [s.ident for s in ref.sites] != [s.ident for s in base.sites]:
            print("note: with the probes' declarations added, the differing decisions are:"
                  + "".join(f"\n  {describe(self.index, s.callee)}" for s in ref.sites))
        offsets = listing_lines(asm.read_text(encoding="latin-1"), ref.symbol)
        asm.unlink(missing_ok=True)
        for s in ref.sites:
            self.measure(s, ref, offsets)

    def sweep(self, site: Site, label: str, kind: Kind, at: int) -> int | None:
        """The fewest copies of `kind` at line index `at` that flip the site's decision."""
        for k in range(1, self.args.max + 1):
            now = self.analyse(insert_all(self.lines, [(at, copies(kind, k))]))
            if now is None:
                print(f"  {label}: build failed at {k}")
                return None
            if kind is self.spend and now.probe_calls:
                print(f"  {label}: the probes stopped expanding at {k}, before the decision flipped")
                return None
            if site.ident not in {s.ident for s in now.sites}:
                left = len(now.sites)
                print(f"  {label}: flips at {k} ({plural(left, 'decision')} still differ{'s' if left == 1 else ''})")
                return k
        print(f"  {label}: no flip up to {self.args.max}")
        return None

    def measure(self, site: Site, ref: Analysis, offsets: dict[int, int]) -> None:
        lines, a = self.lines, self.args
        print(f"\n{describe(self.index, site.callee)} ({direction(site)}):")
        number = (a.site_line + self.shift) if a.site_line else site_line(site, ref.theirs, ref.ours,
                                                                          ref.ours_insns, offsets, self.addr)
        tgt = target_span(lines, self.addr)
        if number is None or not tgt[0] < number - 1 < tgt[1]:
            print(f"  its statement (listing line {number}) is not inside the target's body: pass --site-line")
            return
        start = number - 1
        end = statement_end(lines, start)
        print(f"  under SRC line {start + 1 - self.shift}: {lines[start].strip()}")
        if site.side == "ours":
            k = self.sweep(site, "trivial inline calls before it", self.fold, start)
            if k:
                print(f"    = shortfall {2 * (k - 1)}..{2 * k} units of pool (above the 166-unit floor)")
            return
        k = self.sweep(site, "trivial inline calls after it", self.fold, end + 1)
        if k:
            print(f"    = {plural(k, 'more inline site')} to the end of the body")
        j = self.sweep(site, f"spend probes ({a.spend} units each) before it", self.spend, start)
        if j:
            print(f"    = slack {(j - 1) * a.spend}..{j * a.spend} units of the pool left at the site")
        for name, span in callee_spans(lines, lines[start:end + 1], tgt).items():
            k = self.sweep(site, f"trivial inline calls at the end of {name}", self.fold, insertion(lines, span, "end"))
            if k:
                print(f"    = {plural(k, 'more inline site')} in {name}'s body")


def direction(site: Site) -> str:
    return "exe calls, ours inlines" if site.side == "exe" else "ours calls, exe inlines"


if __name__ == "__main__":
    sys.exit(main())
