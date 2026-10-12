"""List unverified references whose implied base addresses disagree.

Usage: uv run tools/refaudit.py

check.py verifies a reference only when it knows the target's address, so a wrong member
offset into a global object of unknown address passes as unverified. For one object,
(original target - our addend) is the same base at every reference; a symbol with more
than one base is either several objects sharing a name (a constant each file copies, an
overload, a template instance, a function-local static) or an offset error to fix. The
fix for a real object is its layout plus a data/names.csv row, which makes check.py
verify it from then on.
"""

import collections
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FUNCTION = re.compile(r"^(0x[0-9A-F]{8}) ")
REF = re.compile(r"^\s+\+0x[0-9A-F]+ (.+?): original (0x[0-9A-F]+) \[unverified\]")


def main() -> None:
    bases = collections.defaultdict(lambda: collections.defaultdict(set))
    for src in sorted((ROOT / "src").rglob("*.cpp")):
        out = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), str(src)],
                             capture_output=True, text=True).stdout
        current = None
        for line in out.splitlines():
            if m := FUNCTION.match(line):
                current = m.group(1)
            elif (m := REF.match(line)) and current:
                bases[m.group(1)][m.group(2)].add(current)
    split = {k: v for k, v in bases.items() if len(v) > 1}
    print(f"{len(bases)} unverified symbols, {len(split)} with more than one base")
    for name, by_base in sorted(split.items(), key=lambda kv: -sum(map(len, kv[1].values()))):
        print(name)
        for base, functions in sorted(by_base.items()):
            print(f"  {base}: {', '.join(sorted(functions)[:6])}"
                  + (f" and {len(functions) - 6} more" if len(functions) > 6 else ""))


if __name__ == "__main__":
    main()
