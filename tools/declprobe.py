"""Map how one function's match changes with the count of declarations before it.

    uv run tools/declprobe.py src/queue.cpp 0x0064DEA0 --counts 0,1,2,3
    uv run tools/declprobe.py src/rules.cpp 0x0066D530 --into include/mouse.h --pos line:60 --counts 0,2,4

Some functions' register allocation depends on the compiler's heap, which grows with the
declarations seen before them. Their names and lengths make no difference, and those after
the function make none at all; each kind counts with a weight:

    0  #define, a redeclaration of a name already declared
    1  variable or extern, typedef, forward declaration, data member (named or unnamed)
    2  function or member function declaration, enum
    3  inline function definition
    8  struct or class definition, plus its members

The outcome for a given weighted count is fixed but follows no pattern, and the map
changes at some #include boundaries. A change that keeps the net weight before a
function at zero leaves it as it was; for any other, this sweeps unused declarations
of one kind into SRC (or --into a header) and prints the result at each count. The
file is edited in place, so the compiler sees its usual path, and restored afterwards.

kind: var (extern int), def (int), func (function declaration), sfunc (static function
      declaration), type (struct forward declaration), tdef (typedef), inl (inline
      definition), enum, ns (namespace with one extern), macro (#define), mem (data members
      of one struct), mfn (member function declarations of one struct)
pos:  head (line 1), before (above the function's marker), after (above the next
      marker), line:N
"""
import argparse
import hashlib
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MARKER = re.compile(r"^\s*//\s*(?:FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY):\s*GAMEMD\s+(0x[0-9A-Fa-f]+)")
FORMS = {"var": "extern int {};", "def": "int {};", "func": "void {}(void);",
         "sfunc": "static void {}(void);", "type": "struct {};", "tdef": "typedef int {};",
         "inl": "inline int {}(void) {{ return 0; }}", "enum": "enum {{ {} }};",
         "ns": "namespace {} {{ extern int v; }}", "macro": "#define {} 1"}


def decls(kind: str, n: int, prefix: str = "probe") -> list[str]:
    """n unused declarations of one kind, with names nothing else uses."""
    names = [f"{prefix}{k}" for k in range(n)]
    if kind in ("mem", "mfn"):
        if not n:
            return []
        member = "\tint {};" if kind == "mem" else "\tvoid {}(void);"
        return [f"struct {prefix}Struct {{"] + [member.format(x) for x in names] + ["};"]
    return [FORMS[kind].format(x) for x in names]


def position(lines: list[str], addr: int, pos: str) -> int:
    """The line index the declarations go before."""
    if pos == "head":
        return 0
    if pos.startswith("line:"):
        return int(pos[5:]) - 1
    for i, line in enumerate(lines):
        m = MARKER.match(line)
        if m and int(m.group(1), 16) == addr:
            if pos == "before":
                return i
            return next((j for j in range(i + 1, len(lines)) if MARKER.match(lines[j])), len(lines))
    raise SystemExit(f"no marker for 0x{addr:08X}")


def outcome(stdout: str) -> str:
    """MATCH or the score, with a short fingerprint telling different diffs apart."""
    lines = stdout.splitlines()
    head = next((line for line in lines if line.startswith("0x")), "")
    m = re.search(r": (MATCH|[0-9.]+%) \(", head)
    if not m:
        return "FAIL"
    body = "\n".join(line for line in lines if line[:1] in "+-" and not line.startswith(("+++", "---")))
    return m.group(1) + ("" if m.group(1) == "MATCH" else " " + hashlib.md5(body.encode()).hexdigest()[:6])


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("src")
    ap.add_argument("addr")
    ap.add_argument("--kind", default="var", choices=[*FORMS, "mem", "mfn"])
    ap.add_argument("--pos", default="before")
    ap.add_argument("--counts", default="0,1,2,3")
    ap.add_argument("--into", help="insert into this file (a header) instead of SRC; use --pos head or line:N")
    a = ap.parse_args()
    path = ROOT / (a.into or a.src)
    original = path.read_text(errors="replace")
    lines = original.split("\n")
    addr = int(a.addr, 16)
    at = position(lines, addr, a.pos)
    try:
        for n in [int(c) for c in a.counts.split(",")]:
            path.write_text("\n".join(lines[:at] + decls(a.kind, n) + lines[at:]))
            proc = subprocess.run(["uv", "run", "tools/check.py", a.src, a.addr],
                                  capture_output=True, text=True, cwd=ROOT)
            print(f"{a.kind} x{n}\t{outcome(proc.stdout)}", flush=True)
    finally:
        path.write_text(original)


if __name__ == "__main__":
    main()
