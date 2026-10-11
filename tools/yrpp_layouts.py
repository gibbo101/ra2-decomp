"""Extract YRpp's class layouts into data/layouts.csv.

Usage: uv run tools/yrpp_layouts.py [YRPP_DIR] [OUT_CSV]

Compiles every YRpp header with clang for 32-bit MSVC, so classes get Microsoft's record
layout, and records each YRpp class's direct members and bases: offset, bit range, type and
name, plus the class size. Needs clang and the i686 MinGW-w64 headers, the only Windows
headers at hand: the compile sees the MinGW target's include path and predefined macros.
YRpp is MSVC-only, so a scratch copy under build/ gets a few syntax fix-ups first; they
change no layout.
"""

import csv
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
WORK = ROOT / "build/layouts"

# YRpp's call wrappers become empty bodies; bodies do not affect layout.
WRAPPERS = ["JMP_THIS", "JMP_STD", "JMP"]
STATEMENT_MACROS = [
    "GET_REG32", "SET_REG32", "ZERO_REG", "GET_REG8", "SET_REG8", "ADD_ESP", "SUB_ESP",
    "PUSH_IMM", "PUSH_REG", "POP_REG", "PUSH_PTR", "PUSH_VAR32", "PUSH_VAR16", "PUSH_VAR8",
    "PUSH_VAR64", "CALL", "THISCALL", "THISCALL_EX", "THISCALL_VT", "CALL_VT",
    "THISCALL_EX_VT", "MEM_READ8", "MEM_WRITE8", "MEM_READ16", "MEM_WRITE16", "MEM_READ32",
    "MEM_WRITE32", "MEM_WRITEIMM8", "MEM_WRITEIMM16", "MEM_WRITEIMM32", "VAR32_REG",
    "VAR8_REG",
]

# MSVC-only spellings clang rejects, as (file, pattern, replacement).
FIXUPS = [
    ("Interfaces.h", r"\b__interface\b", "struct"),
    ("WOLAPIInterfaces.h", r"\b__out\b", ""),
    ("DiscreteSelectionClass.h", r"(T |&|return )default\b", r"\1fallback"),
    ("DiscreteDistributionClass.h", r"(T |&|return )default\b", r"\1fallback"),
    ("DiscreteDistributionClass.h", r"(class DiscreteDistributionClass\s*\{\s*public:\s*)template <typename T>", r"\1template <typename U>"),
    ("IsometricTileClass.h", r"(Draw\(Point2D\* pLocation, RectangleStruct\* pBounds\) const) override", r"\1"),
    ("BeaconClass.h", r"\A", "#pragma once\n"),
]

TARGET = "i686-pc-windows-msvc"
HEADERS_TARGET = "i686-w64-mingw32"
STD = ["-std=c++20", "-fms-extensions"]
CLANG = [
    "clang++", "-target", TARGET, *STD, "-fms-compatibility", "-fdelayed-template-parsing",
    "-fasm-blocks", "-fsyntax-only", "-Wno-everything", "-ferror-limit=0", "-Dnaked=",
    "-Xclang", "-fdump-record-layouts",
]

LINE = re.compile(r"^\s*(\d+)(?::(\d+)-(\d+))? \|( +)(.*)$")
SIZE = re.compile(r"\[sizeof=(\d+)")


def predefined(target: str) -> dict[str, str]:
    """The macros clang predefines for `target`, by name, as #define lines."""
    out = subprocess.run(["clang++", "-target", target, *STD, "-dM", "-E", "-x", "c++",
                          "/dev/null"], capture_output=True, text=True, check=True).stdout
    return {m.group(1): m.group(0) for m in re.finditer(r"^#define (\w+).*$", out, re.M)}


def mingw_view() -> list[str]:
    """Flags that make the MSVC-target compile include and preprocess as the MinGW one does."""
    mingw, msvc = predefined(HEADERS_TARGET), predefined(TARGET)
    macros = WORK / "mingw.h"
    macros.write_text("".join(f"{line}\n" for name, line in mingw.items() if name not in msvc)
                      + "".join(f"#undef {name}\n" for name in msvc if name not in mingw))
    verbose = subprocess.run(["clang++", "-target", HEADERS_TARGET, *STD, "-E", "-v", "-x", "c++",
                              "/dev/null"], capture_output=True, text=True, check=True).stderr
    dirs = re.search(r"#include <\.\.\.> search starts here:\n(.*?)\nEnd of search list",
                     verbose, re.S).group(1).split()
    return ["-nostdinc", *(a for d in dirs for a in ("-isystem", d)), "-include", str(macros)]


def prepare(src: Path, records: set[str]) -> Path:
    copy = WORK / "yrpp"
    if copy.exists():
        shutil.rmtree(copy)
    shutil.copytree(src, copy, ignore=shutil.ignore_patterns(".git"))
    for name, pattern, repl in FIXUPS:
        p = copy / name
        text = p.read_text(errors="replace")
        new = re.sub(pattern, repl, text, flags=re.S)
        if new == text:
            raise SystemExit(f"fix-up for {name} no longer applies: {pattern}")
        p.write_text(new)
    shim = WORK / "shim.h"
    shim.write_text("".join(f"#define {m}(...) {{}}\n" for m in WRAPPERS)
                    + "".join(f"#define {m}(...)\n" for m in STATEMENT_MACROS)
                    + "#define EPILOG_THISCALL\n#define EPILOG_STDCALL\n")
    unit = WORK / "all.cpp"
    # Only layouts the compile computes are dumped, so each class's size is asked for.
    unit.write_text("".join(f"#include <{h.name}>\n" for h in sorted(copy.glob("*.h")))
                    + "".join(f"__if_exists(::{r}) {{ extern char layout_of_{r}[sizeof(::{r})]; }}\n"
                              for r in sorted(records)))
    return copy


def yrpp_records(src: Path) -> set[str]:
    """Names of the non-template classes YRpp defines."""
    names = set()
    for h in src.rglob("*.h"):
        text = h.read_text(errors="replace")
        for m in re.finditer(r"^(.*?)\b(?:class|struct|union)\s+(?:(?:NOVTABLE|alignas\(\d+\)|"
                             r"__declspec\([^)]*\))\s+)*(\w+)\s*(?:final\s*)?(?::[^;{]*)?\{",
                             text, re.M):
            before = text[max(0, m.start() - 200):m.start()].rstrip()
            if "template" in m.group(1) or before.endswith(">"):
                continue
            names.add(m.group(2))
    return names


def parse(dump: str, wanted: set[str]):
    for block in dump.split("*** Dumping AST Record Layout")[1:]:
        lines = block.strip("\n").splitlines()
        head = LINE.match(lines[0])
        if not head:
            continue
        m = re.match(r"(?:class|struct|union) (\w+)$", head.group(5))
        if not m or m.group(1) not in wanted:
            continue
        cls = m.group(1)
        size = next((int(s.group(1)) for ln in lines if (s := SIZE.search(ln))), None)
        members = []
        # Members of an anonymous union or struct belong to the class itself; the
        # stack holds the indent of each anonymous scope still open.
        anonymous = []
        for ln in lines[1:]:
            e = LINE.match(ln)
            if not e:
                continue
            indent = len(e.group(4))
            while anonymous and indent <= anonymous[-1]:
                anonymous.pop()
            if indent != 3 + 2 * len(anonymous):
                continue
            off, b0, b1, text = int(e.group(1)), e.group(2), e.group(3), e.group(5).rstrip()
            if re.match(r"(?:union|struct) .*\(anonymous at [^)]*\)$", text):
                anonymous.append(indent)
                continue
            bits = f"{b0}-{b1}" if b0 is not None else ""
            if text.startswith("("):
                members.append((off, bits, "", text.strip("()")))
                continue
            base = re.match(r"(class|struct) (\w+) \((.*)\)$", text)
            if base:
                members.append((off, bits, base.group(2), f"<{base.group(3)}>"))
                continue
            t = re.match(r"(.*?)\s*(\w+)$", text)
            members.append((off, bits, t.group(1), t.group(2)) if t else (off, bits, "", text))
        yield cls, size, members


def main() -> None:
    src = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "reference/YRpp"
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "data/layouts.csv"
    WORK.mkdir(parents=True, exist_ok=True)
    records = yrpp_records(src)
    # A name that is only forward-declared, or names a template, has no layout of its
    # own; clang reports each one, and the run is repeated without them.
    not_a_class = re.compile(r"incomplete type '::(\w+)'|class template '(\w+)' requires")
    view = mingw_view()
    for _ in range(5):
        copy = prepare(src, records)
        run = subprocess.run(CLANG + view + ["-include", "stddef.h", "-I", str(copy), "-include",
                                             str(WORK / "shim.h"), str(WORK / "all.cpp")],
                             capture_output=True, text=True)
        errors = [ln for ln in run.stderr.splitlines() if "error:" in ln]
        drop = {a or b for ln in errors for a, b in not_a_class.findall(ln)}
        if not drop or len(drop) < len(errors):
            break
        records -= drop
    if errors:
        print("\n".join(errors[:20]), file=sys.stderr)
        raise SystemExit(f"{len(errors)} compile errors; no layouts written")
    seen = set()
    rows = []
    for cls, size, members in parse(run.stdout, records):
        if cls in seen:
            continue
        seen.add(cls)
        rows.append((cls, size, "", "", "", "<size>"))
        rows += [(cls, size, off, bits, typ, name) for off, bits, typ, name in members]
    # Unnamed types are named after where they are declared: keep the header, not the path.
    rows = [r[:4] + (re.sub(r" at [^)]*/([^/)]+\.h):(\d+):\d+\)", r" at \1:\2)", r[4]),) + r[5:]
            for r in rows]
    with out.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["class", "size", "offset", "bits", "type", "member"])
        w.writerows(rows)
    print(f"{len(seen)} classes, {len(rows) - len(seen)} members -> {out}")


if __name__ == "__main__":
    main()
