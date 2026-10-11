"""Copy TibSun archive headers into include/, ready to be edited into YR's versions.

Usage: uv run tools/adopt_header.py team.h [more.h ...]

Each copy gets the repository's licence header and loses TibSun's comments, which the
repository's comment rules do not allow; the code is unchanged. A header already in
include/ is left alone.
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TIBSUN = ROOT / "reference/tibsun/code"
HEADER = """// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
"""


def strip_comments(text: str) -> str:
    """Removes comments, leaving string and character literals alone."""
    out, i, n = [], 0, len(text)
    while i < n:
        if text.startswith("//", i):
            while i < n and text[i] != "\n":
                i += 1
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            i = n if end < 0 else end + 2
        elif text[i] in "\"'":
            quote, j = text[i], i + 1
            while j < n and text[j] != quote:
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        else:
            out.append(text[i])
            i += 1
    body = re.sub(r"[ \t]+\n", "\n", "".join(out))
    return re.sub(r"\n{3,}", "\n\n", body)


def main() -> None:
    for name in sys.argv[1:]:
        dest = ROOT / "include" / name
        if dest.exists():
            print(f"{name}: already in include/, left alone")
            continue
        text = (TIBSUN / name).read_text(errors="replace")
        start = text.find("#pragma once")
        body = strip_comments(text[start if start >= 0 else 0:])
        dest.write_text(HEADER + body.lstrip("\n"))
        print(f"{name}: copied to include/")


if __name__ == "__main__":
    main()
