"""Write data/manifest.csv: one row per matched function, for anyone to verify the claim.

Usage: uv run tools/manifest.py

Each row names a function of gamemd.exe 1.001 (MD5 2a1307f6f57c9c492f985b4e3afb88e6) that
src/ rebuilds byte for byte: its address and size, the decorated symbol our object gives
it, the source file, and the SHA-256 of the original's bytes at that address, so a
verifier knows exactly which bytes the claim covers without the file holding any of them.
Every file compiles with Visual C++ 6.0 SP3 (C2.DLL 12.00.8447) and the flags in
check.FLAGS; bytes the linker fills in (relocations) are compared by target, not value.
"""

import csv
import hashlib
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from check import FLAGS, Original  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent


def main() -> None:
    orig = Original()
    with (ROOT / "data/progress.csv").open() as f:
        rows = [r for r in csv.DictReader(f) if r["result"] == "MATCH"]
    rows.sort(key=lambda r: int(r["address"], 16))
    out = ROOT / "data/manifest.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "symbol", "source", "sha256"])
        for r in rows:
            address, size = int(r["address"], 16), int(r["size"])
            digest = hashlib.sha256(orig.read(address, size)).hexdigest()
            w.writerow([f"0x{address:08X}", size, r["symbol"], r["source"], digest])
    print(f"{len(rows)} functions -> {out.relative_to(ROOT)} (flags: {' '.join(FLAGS[1:])})")


if __name__ == "__main__":
    main()
