"""Find whether one function's near miss follows its count of named locals, as a target for natural source.

Usage: uv run tools/localprobe.py SRC ADDR [--max 15]

Copies SRC to build/localprobe/ and, for n = 0..MAX, adds n unused initialised `int` locals
at the start of the target's body, printing the score each count gives. VC6 numbers a
function's named, initialised locals ahead of its own temporaries, so register and slot
ties between temporaries can follow that count (by its value mod 8, or by which run of
eight it falls in) whatever the locals' names, places or scopes. A count that matches is a
target: the original had that many more named locals, or 8 minus that many fewer. The
probe never lands; find natural source that changes the count the same way.
"""
import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import check  # noqa: E402

ROOT = check.ROOT
OUT = ROOT / "build/localprobe"
MARKER = re.compile(r"^\s*//\s*(?:FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY):\s*GAMEMD\s+(0x[0-9A-Fa-f]+)")


def body_start(lines, addr):
    """Index of the line after the target's opening brace."""
    for i, line in enumerate(lines):
        m = MARKER.match(line)
        if m and int(m.group(1), 16) == addr:
            for j in range(i, len(lines)):
                if "{" in re.sub(r"//.*", "", lines[j]):
                    return j + 1
    raise SystemExit(f"no marker for 0x{addr:08X}")


def checked(path, addr, orig, names):
    """The checker's result for the probe copy at `path`; `orig` and `names` load once per run."""
    out = check.output(path, {addr}, orig=orig, names=names, out=path.with_suffix(".obj"))
    head = next((l for l in out.splitlines() if l.startswith("0x")), "")
    m = re.search(r": (MATCH|[0-9.]+%) \((\d+)/(\d+)", head)
    return f"{m.group(1)} {m.group(2)}/{m.group(3)}" if m else "build failed"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("addr")
    ap.add_argument("--max", type=int, default=15)
    a = ap.parse_args()
    src = (ROOT / a.src) if not Path(a.src).is_absolute() else Path(a.src)
    addr = int(a.addr, 16)
    lines = src.read_text(errors="replace").split("\n")
    at = body_start(lines, addr)
    OUT.mkdir(parents=True, exist_ok=True)
    probe = OUT / src.name
    orig, names = check.Original(), check.known_names()
    for n in range(a.max + 1):
        extra = [f"\tint local_probe_{k} = {k};" for k in range(n)]
        probe.write_text("\n".join(lines[:at] + extra + lines[at:]))
        print(f"+{n:2} locals: {checked(probe, addr, orig, names)}", flush=True)
    probe.unlink(missing_ok=True)
    probe.with_suffix(".obj").unlink(missing_ok=True)


if __name__ == "__main__":
    sys.exit(main())
