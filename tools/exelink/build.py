#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Compile every source file under src/ into build/exelink/obj with the checker's compiler path.

Each .cpp is compiled as tools/check.py compiles it (the fixed flags, the header
directory it prepares) plus /Gy, which puts every function in its own COMDAT so /ORDER can place it at its
address; it changes how the code is packaged, not the code. Each archived .asm is assembled the
same way as the checker does, split where data/asm_objects.csv says the original began a new object
at one of its routines, and then comdatize.py marks its .text a COMDAT for the same reason. Writes
build/exelink/obj/objects.json, the object index the link reads:
{"objects": [{"source": "src/x.cpp", "object": "src/x.obj"}, ...]}.

Usage: uv run tools/exelink/build.py [--jobs N] [--only src/a.cpp ...]
"""
import argparse
import csv
import json
import shutil
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent))
import paths  # noqa: E402
import check  # noqa: E402
import comdatize  # noqa: E402


def object_starts():
    """{source: [routines the original assembled as the first of a new object]} from data/asm_objects.csv."""
    with open(paths.ROOT / "data/asm_objects.csv", newline="") as f:
        starts = {}
        for row in csv.DictReader(f):
            starts.setdefault(row["source"], []).append(row["symbol"])
        return starts


def sources():
    return sorted(p for ext in ("*.cpp", "*.asm") for p in (paths.ROOT / "src").rglob(ext))


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", nargs="*", type=Path, help="rebuild just these sources (the index keeps the rest)")
    args = ap.parse_args(argv)
    objdir = paths.OBJDIR
    objdir.mkdir(parents=True, exist_ok=True)
    todo = [p.resolve() for p in args.only] if args.only else sources()
    headers = check.prepare_headers(None)
    starts = object_starts()
    failed = []

    def one(src):
        rel = src.relative_to(paths.ROOT)
        obj, log = check.compile_source(src, headers=headers, out=objdir / rel.with_suffix(".obj"),
                                        extra=() if src.suffix == ".asm" else ("/Gy",))
        if obj is None:
            failed.append((rel.as_posix(), log.strip().splitlines()[-3:]))
        elif src.suffix == ".asm":   # MASM writes one plain .text; make it a COMDAT /ORDER can place
            data = obj.read_bytes()
            for symbol in starts.get(rel.as_posix(), ()):
                data = comdatize.split(data, symbol)
            obj.write_bytes(comdatize.comdatize(data)[0])
        return rel

    try:
        with ThreadPoolExecutor(args.jobs) as pool:
            list(pool.map(one, todo))
    finally:
        shutil.rmtree(headers, ignore_errors=True)
    objects = []
    for src in sources():
        rel = src.relative_to(paths.ROOT)
        if (objdir / rel.with_suffix(".obj")).is_file():
            objects.append({"source": rel.as_posix(), "object": rel.with_suffix(".obj").as_posix()})
    (objdir / "objects.json").write_text(json.dumps({"objects": objects}, indent=1) + "\n")
    print(f"{len(objects)} objects in {objdir}; {len(failed)} sources failed")
    for rel, tail in failed:
        print(f"  {rel}: {' | '.join(tail)}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
