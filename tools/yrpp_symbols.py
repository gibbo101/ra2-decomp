"""Extract addresses and names from YRpp's headers into data/symbols.csv.

Usage: python3 tools/yrpp_symbols.py [YRPP_DIR] [OUT_CSV]

Records facts only: address, kind, qualified name, calling convention and the header
it came from. Functions come from YRpp's call wrappers, data from its reference macros,
vtables from its address constants and imports from ALIAS.
"""

import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

FUNC_MACROS = {"JMP_THIS", "JMP_STD", "JUMP_STD", "JMP", "CALL", "THISCALL"}
ADDR = r"(0x[0-9A-Fa-f]+)u?"
SCOPE_HEAD = re.compile(
    r"\b(class|struct|union|namespace)\s+(?:(?:NOVTABLE|alignas\(\d+\)|__declspec\([^)]*\))\s+)*"
    r"(\w+)[^()]*$")
CONVENTIONS = ("__fastcall", "__stdcall", "__cdecl", "__thiscall")
LEADING_MACROS = re.compile(
    r"^(?:\s*\b(?:DEFINE_\w+|ABSTRACTTYPE_ARRAY|DECLARE_\w+|ALIAS)\s*\((?:[^()]|\([^()]*\))*\))+")


def strip(text: str) -> str:
    """Blanks comments, string and char literals and preprocessor lines, keeping offsets."""
    out = list(text)
    i, n = 0, len(text)

    def blank(a, b):
        for k in range(a, b):
            if out[k] != "\n":
                out[k] = " "

    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            blank(i, j)
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            blank(i, j)
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            blank(i, j + 1)
            i = j + 1
        elif c == "#" and not text[text.rfind("\n", 0, i) + 1:i].strip():
            j = i
            while True:
                e = text.find("\n", j)
                if e < 0:
                    e = n
                    break
                if text[e - 1] != "\\":
                    break
                j = e + 1
            blank(i, e)
            i = e
        else:
            i += 1
    return "".join(out)


def scopes(text: str):
    """Yields (offset of each '{', offset of matching '}', header text, scope name or None)."""
    stack, last = [], 0
    for i, c in enumerate(text):
        if c == "{":
            head = text[last:i]
            m = SCOPE_HEAD.search(" ".join(head.split()))
            stack.append((i, head, m.group(2) if m else None))
            last = i + 1
        elif c == "}":
            if stack:
                start, head, name = stack.pop()
                yield start, i, head, name
            last = i + 1
        elif c == ";":
            last = i + 1


def declared_name(head: str) -> tuple[str, str]:
    """The function name and calling convention in a declaration ending before its body."""
    flat = " ".join(head.split())
    flat = re.sub(r"^.*\b(public|protected|private)\s*:", "", flat)
    flat = LEADING_MACROS.sub("", flat)
    flat = re.sub(r"__declspec\s*\((?:[^()]|\([^()]*\))*\)", "", flat).strip()
    depth, cut = 0, None
    for k, c in enumerate(flat):
        if c == "(":
            if depth == 0 and cut is None:
                cut = k
            depth += 1
        elif c == ")":
            depth -= 1
    before = flat[:cut] if cut is not None else flat
    m = re.search(r"(operator\s*(?:\(\)|[^\s(]+(?:\s*\[\])?)|~?\w+)\s*$", before)
    name = m.group(1).replace(" ", "") if m else "?"
    conv = next((c for c in CONVENTIONS if c in before.split()), "")
    return name, conv


def extract(path: Path, rel: str):
    raw = path.read_text(errors="replace")
    text = strip(raw)
    blocks = sorted(scopes(text))
    named = [(s, e, n) for s, e, _, n in blocks if n]

    def owner(pos: int) -> str:
        inside = [n for s, e, n in named if s < pos < e]
        return "::".join(inside)

    rows = []
    prev_end = 0
    for m in re.finditer(r"\b(\w+)\s*\(\s*" + ADDR + r"[^)]*\)", text):
        macro, addr = m.group(1), int(m.group(2), 16)
        if macro not in FUNC_MACROS:
            continue
        inner = [(s, e, h, n) for s, e, h, n in blocks if s < m.start() < e]
        if inner and not inner[-1][3]:
            s, _, head, _ = inner[-1]
        else:
            # A wrapper standing in for the body: the declaration runs up to the macro,
            # from the previous statement or the previous wrapper, whichever is later.
            s = max(max(text.rfind(c, 0, m.start()) for c in ";{}") + 1, prev_end)
            head = text[s:m.start()]
        name, conv = declared_name(head)
        if macro in ("JMP_THIS", "THISCALL"):
            conv = "__thiscall"
        cls = owner(s)
        rows.append((addr, "function", f"{cls}::{name}" if cls else name, conv, rel))
        prev_end = m.end()
    for m in re.finditer(r"\bDEFINE_REFERENCE\s*\(([^;]*?),\s*(\w+)\s*,\s*" + ADDR + r"\s*\)", text):
        cls = owner(m.start())
        rows.append((int(m.group(3), 16), "data", f"{cls}::{m.group(2)}" if cls else m.group(2), "", rel))
    for m in re.finditer(r"\bDEFINE_ARRAY_REFERENCE\s*\(([^;]*?),\s*(\[[^;]*?\]|\w+)\s*,\s*(\w+)\s*,\s*" + ADDR + r"\s*\)", text):
        cls = owner(m.start())
        rows.append((int(m.group(4), 16), "data", f"{cls}::{m.group(3)}" if cls else m.group(3), "", rel))
    for m in re.finditer(r"\bABSTRACTTYPE_ARRAY\s*\(\s*(\w+)\s*,\s*" + ADDR + r"\s*\)", text):
        rows.append((int(m.group(2), 16), "data", f"{m.group(1)}::Array", "", rel))
    for m in re.finditer(r"\bstatic\s+constexpr\s+uintptr_t\s+(\w*VTable\w*)\s*=\s*" + ADDR, text):
        cls = owner(m.start())
        rows.append((int(m.group(2), 16), "vtable", f"{cls}::{m.group(1)}" if cls else m.group(1), "", rel))
    for m in re.finditer(r"\bALIAS\s*\(\s*(\w+)\s*,\s*" + ADDR + r"\s*\)", text):
        rows.append((int(m.group(2), 16), "import", m.group(1), "", rel))
    return rows


def main() -> None:
    src = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "reference/YRpp"
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "data/symbols.csv"
    rows = []
    for path in sorted(src.rglob("*.h")):
        rows += extract(path, path.relative_to(src).as_posix())
    rows = sorted(set(rows))
    out.parent.mkdir(parents=True, exist_ok=True)
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "kind", "name", "convention", "source"])
        for addr, kind, name, conv, rel in rows:
            w.writerow([f"0x{addr:08X}", kind, name, conv, f"yrpp:{rel}"])
    print(f"{len(rows)} rows -> {out}")


if __name__ == "__main__":
    main()
