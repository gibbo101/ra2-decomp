"""Locate where the original spent VC6's inline budget for one function, as a target for natural source.

Usage: uv run tools/inlineprobe.py SRC ADDR [--n 1,2,4,8] [--only start,end,NAME...] [--spend K]

Copies SRC to build/inlineprobe/, adds N unused calls of a trivial inline at one place at a
time, and prints the target's score and how many call lines still differ from the exe.
The default probe body folds away, so each call adds a statement and raises the budget;
--spend K gives the body K terms, so each call spends budget instead.
Places: the start and end of the target's body (its own expansions) and the start and end
of each function defined in SRC that the target calls (the next nesting level).
A place and count where no call line differs is the original's spending pattern. The probe
is a diagnostic: it never lands; find natural source that spends the same.
"""
import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check  # noqa: E402

ROOT = check.ROOT
OUT = ROOT / "build/inlineprobe"
MARKER = re.compile(r"^\s*//\s*(?:FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY):\s*GAMEMD\s+(0x[0-9A-Fa-f]+)")


def helper(spend: int = 0) -> str:
    """The probe inline: a body that folds away, or one of `spend` terms that VC6 must expand."""
    body = "a & 0" if spend <= 0 else " + ".join(f"(a ^ {k + 1})" for k in range(spend))
    return f"static inline int Inline_Probe_(int a) {{ return {body}; }}"
KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "catch", "delete", "new"}


def body_span(lines, start):
    """(open, close) line indexes of the first brace-delimited body at or after `start`."""
    depth, first = 0, None
    for i in range(start, len(lines)):
        text = re.sub(r"//.*|\"(?:\\.|[^\"])*\"|'(?:\\.|[^'])*'", "", lines[i])
        for ch in text:
            if ch == "{":
                depth += 1
                if first is None:
                    first = i
            elif ch == "}":
                depth -= 1
                if first is not None and depth == 0:
                    return first, i
    raise SystemExit("unbalanced braces")


def target_span(lines, addr):
    for i, line in enumerate(lines):
        m = MARKER.match(line)
        if m and int(m.group(1), 16) == addr:
            return body_span(lines, i)
    raise SystemExit(f"no marker for 0x{addr:08X}")


def callee_spans(lines, body, target):
    """Bodies of functions defined in the file whose name the target's body calls."""
    names = {n for n in re.findall(r"\b([A-Za-z_]\w*)\s*\(", "\n".join(body)) if n not in KEYWORDS}
    spans = {}
    for name in sorted(names):
        pat = re.compile(r"(?:^|[\s:*&])" + re.escape(name) + r"\s*\([^;]*\)\s*(?:const)?\s*(?:\{|$)")
        for i, line in enumerate(lines):
            if pat.search(line) and not line.rstrip().endswith(";") and not line.lstrip().startswith("//"):
                try:
                    o, c = body_span(lines, i)
                except SystemExit:
                    continue
                if o - i > 3 or target[0] <= o <= target[1]:
                    continue
                spans[name] = (o, c)
                break
    return spans


def insertion(lines, span, where):
    """Line index to insert before: just inside the body, or before its last return / brace."""
    o, c = span
    if where == "start":
        return o + 1
    for i in range(c - 1, o, -1):
        if re.match(r"\s*return\b", lines[i]):
            return i
        if lines[i].strip():
            break
    return c


def checked(path, addr, orig, names):
    """The checker's result for the probe copy at `path`; `orig` and `names` load once per run."""
    out = check.output(path, {addr}, orig=orig, names=names, out=path.with_suffix(".obj"))
    head = next((l for l in out.splitlines() if l.startswith("0x")), "")
    m = re.search(r": (MATCH|[0-9.]+%) \((\d+)/(\d+)", head)
    calls = sum(1 for l in out.splitlines() if l[:1] in "+-" and not l.startswith(("+++", "---")) and "call" in l)
    return (m.group(1), f"{m.group(2)}/{m.group(3)}", calls) if m else ("build failed", "", -1)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("addr")
    ap.add_argument("--n", default="1,2,4,8")
    ap.add_argument("--only", default="")
    ap.add_argument("--spend", type=int, default=0)
    a = ap.parse_args()
    src = (ROOT / a.src) if not Path(a.src).is_absolute() else Path(a.src)
    addr = int(a.addr, 16)
    lines = src.read_text(errors="replace").split("\n")
    first = next(i for i, l in enumerate(lines) if MARKER.match(l))
    lines.insert(first, helper(a.spend))
    tgt = target_span(lines, addr)
    places = {"start": (tgt, "start"), "end": (tgt, "end")}
    for name, span in callee_spans(lines, lines[tgt[0]:tgt[1] + 1], tgt).items():
        places[f"{name}:start"] = (span, "start")
        places[f"{name}:end"] = (span, "end")
    only = {s for s in a.only.split(",") if s}
    if only:
        places = {k: v for k, v in places.items() if k in only or k.split(":")[0] in only}
    OUT.mkdir(parents=True, exist_ok=True)
    probe = OUT / src.name
    orig, names = check.Original(), check.known_names()
    probe.write_text("\n".join(lines))
    print(f"baseline: {' '.join(map(str, checked(probe, addr, orig, names)))} call lines differ")
    for label, (span, where) in places.items():
        row = []
        for n in [int(x) for x in a.n.split(",")]:
            at = insertion(lines, span, where)
            calls = [f"\tint inline_probe_{k} = Inline_Probe_({k});" for k in range(n)]
            probe.write_text("\n".join(lines[:at] + calls + lines[at:]))
            score, size, diff = checked(probe, addr, orig, names)
            row.append(f"n={n}: {score} {size} calls~{diff}")
            print(f"{label:28} " + " | ".join(row[-1:]), flush=True)
    probe.unlink(missing_ok=True)
    probe.with_suffix(".obj").unlink(missing_ok=True)


if __name__ == "__main__":
    sys.exit(main())
