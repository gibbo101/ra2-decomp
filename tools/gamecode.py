"""What counts as game code, shared by the status totals and the progress report.

Game code is every function outside the library's span (the CRT and the import thunks):
everything below the first library function, and Westwood's assembly after the last one.
Unidentified functions inside the span are library code. A function matches when
data/manifest.csv lists it; its fuzzy score is 100 when it matches, else its best score
in data/attempts.csv, and 0 when it was never tried.
"""

import csv
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"


def rows(name: str) -> list[dict]:
    with open(DATA / name, newline="") as f:
        return list(csv.DictReader(f))


def library_span() -> tuple[int, int]:
    libs = [int(r["address"], 16) for r in rows("library.csv")]
    return min(libs), max(libs)


def split_functions() -> tuple[dict[int, int], dict[int, int]]:
    """({address: size} of game functions, {address: size} of library functions)."""
    first, last = library_span()
    game, library = {}, {}
    for r in rows("functions.csv"):
        a = int(r["address"], 16)
        (library if first <= a <= last else game)[a] = int(r["size"])
    return game, library


def matched() -> set[int]:
    return {int(r["address"], 16) for r in rows("manifest.csv")}


def best_scores(addresses, done: set[int]) -> dict[int, float]:
    """Best attempt score per address not in `done`, from data/attempts.csv."""
    best = {}
    for r in rows("attempts.csv"):
        score = re.match(r"\s*(\d+(?:\.\d+)?)", r["best"] or "")
        try:
            a = int(r["address"], 16)
        except ValueError:
            continue
        if score and a in addresses and a not in done and float(score.group(1)) <= 100:
            best[a] = max(best.get(a, 0.0), float(score.group(1)))
    return best
