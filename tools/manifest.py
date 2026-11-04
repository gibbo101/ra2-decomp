"""Write data/manifest.csv: one row per matched function, for anyone to verify the claim.

Usage: uv run tools/manifest.py

Each row names a function of gamemd.exe 1.001 (MD5 2a1307f6f57c9c492f985b4e3afb88e6) that
src/ rebuilds byte for byte: its address and size, the decorated symbol our object gives
it, the source file, and the SHA-256 of the original's bytes at that address, so a
verifier knows exactly which bytes the claim covers without the file holding any of them.
C++ uses Visual C++ 6.0 SP3 (C2.DLL 12.00.8447) and check.FLAGS; assembly uses MASM
and check.ASM_FLAGS. The checker compares linker-filled fields (relocations) by target.
"""

import csv
import hashlib
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from check import Original  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent


def main(output=None) -> None:
    orig = Original()
    with (ROOT / "data/progress.csv").open() as f:
        rows = [r for r in csv.DictReader(f) if r["result"] == "MATCH"]
    rows.sort(key=lambda r: int(r["address"], 16))
    out = output or ROOT / "data/manifest.csv"
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "size", "symbol", "source", "sha256"])
        for r in rows:
            address, size = int(r["address"], 16), int(r["size"])
            digest = hashlib.sha256(orig.read(address, size)).hexdigest()
            w.writerow([f"0x{address:08X}", size, r["symbol"], r["source"], digest])
    print(f"{len(rows)} functions -> {out}")


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out", type=Path)
    main(parser.parse_args().out)
