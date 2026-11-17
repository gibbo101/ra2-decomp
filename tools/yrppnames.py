"""List the names whose only evidence is YRpp, as lower-confidence: data/yrpp_names.csv.

    uv run tools/yrppnames.py           # rewrite data/yrpp_names.csv
    uv run tools/yrppnames.py --check   # fail if it is out of date

A matched function whose name is YRpp's name for its address (data/symbols.csv), and a
global whose names.csv evidence cites YRpp, is listed unless the TibSun archive defines the
same name: a function as Class::Name or a free function, a global by its bare name. A
listed name is YRpp's guess, not Westwood's; a better-evidenced name may replace it.
"""
import csv
import io
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TIBSUN = ROOT / "reference/tibsun/code"
OUT = ROOT / "data/yrpp_names.csv"
# A definition at the start of a line: `Type Class::Name(` or `Type Name(`.
DEFINITION = re.compile(r"^[A-Za-z_][\w\s\*&<>,:]*?\b((?:\w+::)*~?\w+)\s*\(", re.M)


def tibsun_names(root: Path = TIBSUN) -> tuple[set[str], set[str]]:
    """Qualified function names TibSun defines, and every identifier its code uses."""
    functions, words = set(), set()
    for path in [*root.glob("*.cpp"), *root.glob("*.h")]:
        text = path.read_text(errors="replace")
        functions.update(DEFINITION.findall(text))
        words.update(re.findall(r"\b[A-Za-z_]\w*\b", text))
    return functions, words


def yrpp_only(progress, symbols, names, functions, words) -> list[tuple[str, str, str]]:
    """(address, name, source file) for each YRpp-only name, by address."""
    yrpp = {}
    for r in symbols:
        yrpp.setdefault(int(r["address"], 16), set()).add(r["name"])
    out = []
    for r in progress:
        if r["result"] != "MATCH":
            continue
        address = int(r["address"], 16)
        if r["name"] in yrpp.get(address, ()) and r["name"] not in functions:
            out.append((f"0x{address:08X}", r["name"], "symbols.csv"))
    for r in names:
        bare = r["name"].split("::")[-1]
        if "YRpp" in r.get("evidence", "") and r["name"] not in words and bare not in words:
            out.append((f"0x{int(r['address'], 16):08X}", r["name"], "names.csv"))
    return sorted(set(out))


def render(rows) -> str:
    buf = io.StringIO()
    w = csv.writer(buf, lineterminator="\n")
    w.writerow(["address", "name", "source"])
    w.writerows(rows)
    return buf.getvalue()


def main() -> None:
    def read(name):
        with (ROOT / "data" / name).open(newline="") as f:
            return list(csv.DictReader(f))
    functions, words = tibsun_names()
    text = render(yrpp_only(read("progress.csv"), read("symbols.csv"), read("names.csv"), functions, words))
    if "--check" in sys.argv[1:]:
        if not OUT.exists() or OUT.read_text() != text:
            raise SystemExit("data/yrpp_names.csv is out of date: run tools/yrppnames.py")
        return
    OUT.write_text(text)
    print(f"{text.count(chr(10)) - 1} YRpp-only names written to {OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
