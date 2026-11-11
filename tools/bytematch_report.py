"""Write the progress report ByteMatch reads: objdiff's report format, version 2, as JSON.

Usage: python3 tools/bytematch_report.py [OUT]   (default build/GAMEMD_1.001/report.json)

Reads only tracked data, never the game or the toolchain. Each game module in
data/modules.csv is a unit, and Westwood's assembly after the CRT is the unit winasm; each
library in data/library.csv is a library unit, with the span's unidentified functions in
library/unidentified. Game functions match as tools/gamecode.py says; a library function
matches when data/library.csv identifies it, since that is a byte match against the
toolchain's own library objects. Fuzzy scores weigh each function by its size; an unmatched
function whose best attempt matched outside src/ scores 99.99. A unit is complete when every
function in it matches.
"""

import bisect
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path

import gamecode

VERSION = 2
CATEGORIES = (("game", "Game"), ("library", "Library"))
DEFAULT_OUT = gamecode.ROOT / "build/GAMEMD_1.001/report.json"
# objdiff counts a score of 100 as matched; a banked match outside src/ is not.
UNLANDED_CAP = 99.99


def measures(functions: list[dict], complete: bool) -> dict:
    total = sum(f["size"] for f in functions)
    done = sum(f["size"] for f in functions if f["fuzzy_match_percent"] == 100)
    fuzzy = sum(f["size"] * f["fuzzy_match_percent"] / 100 for f in functions)
    count = len(functions)
    hits = sum(1 for f in functions if f["fuzzy_match_percent"] == 100)
    return {
        "fuzzy_match_percent": pct(fuzzy, total),
        "total_code": total, "matched_code": done, "matched_code_percent": pct(done, total),
        "total_data": 0, "matched_data": 0, "matched_data_percent": 0.0,
        "total_functions": count, "matched_functions": hits,
        "matched_functions_percent": pct(hits, count),
        "complete_code": total if complete else 0,
        "complete_code_percent": 100.0 if complete and total else 0.0,
        "complete_data": 0, "complete_data_percent": 0.0,
        "total_units": 1, "complete_units": 1 if complete else 0,
    }


def pct(part: float, whole: float) -> float:
    return part * 100.0 / whole if whole else 0.0


def combine(units: list[dict]) -> dict:
    m = {k: 0 for k in units[0]["measures"]} if units else {}
    for u in units:
        for k, v in u["measures"].items():
            if not k.endswith("_percent"):
                m[k] += v
    fuzzy = sum(u["measures"]["fuzzy_match_percent"] * u["measures"]["total_code"] for u in units)
    m["fuzzy_match_percent"] = fuzzy / m["total_code"] if m.get("total_code") else 0.0
    for part, whole in (("matched_code", "total_code"), ("matched_data", "total_data"),
                        ("matched_functions", "total_functions"), ("complete_code", "total_code"),
                        ("complete_data", "total_data")):
        m[part + "_percent"] = pct(m[part], m[whole])
    return m


def build() -> dict:
    game, library = gamecode.split_functions()
    names = {int(r["address"], 16): r["name"] for r in gamecode.rows("functions.csv")}
    manifest = {int(r["address"], 16): r for r in gamecode.rows("manifest.csv")}
    best = gamecode.best_scores(game, set(manifest))
    libs = {int(r["address"], 16): r for r in gamecode.rows("library.csv")}
    modules = sorted((int(r["first"], 16), int(r["last"], 16), r["module"])
                     for r in gamecode.rows("modules.csv"))
    starts = [m[0] for m in modules]
    _, last = gamecode.library_span()

    grouped = defaultdict(list)
    sources = defaultdict(Counter)
    for a, size in sorted(game.items()):
        i = bisect.bisect_right(starts, a) - 1
        if i >= 0 and a <= modules[i][1]:
            unit = "game/" + modules[i][2]
        else:
            unit = "game/winasm" if a > last else "game/unassigned"
        row = manifest.get(a)
        if row:
            sources[unit][row["source"]] += 1
        grouped[unit].append({
            "name": row["symbol"] if row else (names.get(a) or f"FUN_{a:08X}"),
            "size": size,
            "fuzzy_match_percent": 100 if row else min(best.get(a, 0.0), UNLANDED_CAP),
            "metadata": {"virtual_address": a},
        })
    for a, size in sorted(library.items()):
        row = libs.get(a)
        unit = "library/" + (row["library"].rsplit(".", 1)[0].lower() if row else "unidentified")
        grouped[unit].append({
            "name": row["symbol"] if row else (names.get(a) or f"FUN_{a:08X}"),
            "size": size,
            "fuzzy_match_percent": 100 if row else 0,
            "metadata": {"virtual_address": a},
        })

    units = []
    for name, functions in sorted(grouped.items(), key=lambda kv: kv[1][0]["metadata"]["virtual_address"]):
        category = name.split("/", 1)[0]
        complete = all(f["fuzzy_match_percent"] == 100 for f in functions)
        meta = {"complete": complete, "progress_categories": [category]}
        if sources[name]:
            meta["source_path"] = sources[name].most_common(1)[0][0]
        units.append({"name": name, "measures": measures(functions, complete), "sections": [],
                      "functions": functions, "metadata": meta})
    categories = [{"id": cid, "name": cname,
                   "measures": combine([u for u in units if cid in u["metadata"]["progress_categories"]])}
                  for cid, cname in CATEGORIES]
    return {"measures": combine(units), "units": units, "version": VERSION, "categories": categories}


def main(argv: list[str]) -> int:
    out = Path(argv[1]) if len(argv) > 1 else DEFAULT_OUT
    report = build()
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=1) + "\n")
    for c in report["categories"]:
        m = c["measures"]
        print(f"{c['id']}: {m['matched_functions']:,} of {m['total_functions']:,} functions, "
              f"{m['matched_code_percent']:.1f}% of code, fuzzy {m['fuzzy_match_percent']:.1f}%, "
              f"{m['complete_units']} of {m['total_units']} units complete")
    print(f"-> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
