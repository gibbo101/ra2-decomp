"""Re-check every annotated function under src/: the merge gate.

Usage: uv run tools/checkall.py

Prints one line per function and exits non-zero if any annotated function no longer
matches, since a header edit can change the code of functions it never touches.
"""

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main() -> None:
    sources = sorted((ROOT / "src").rglob("*.cpp")) if (ROOT / "src").exists() else []
    matched = failed = 0
    for src in sources:
        proc = subprocess.run([sys.executable, str(ROOT / "tools/check.py"), str(src)],
                              capture_output=True, text=True, cwd=ROOT)
        for line in proc.stdout.splitlines():
            if line.startswith("0x"):
                print(line)
                if ": MATCH (" in line:
                    matched += 1
                else:
                    failed += 1
        if proc.returncode and not proc.stdout.startswith("0x"):
            print(f"{src.relative_to(ROOT)}: {proc.stdout.strip() or proc.stderr.strip()}")
            failed += 1
    print(f"{matched} matched, {failed} not")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
