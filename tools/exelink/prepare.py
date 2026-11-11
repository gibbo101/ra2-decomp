#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Derive the link pipeline's inputs from data/ and the player's exe.

Writes, under build/exelink/ (ignored by git):
  gamemd_1001.exe              orig/gamemd.exe with the build's post-link changes put back
                               (tools/exebuild.py), the image every later step compares with
  inputs/progress_totals.json  its SHA-256, which every module checks before reading it
  inputs/match_manifest.json   data/manifest.csv as manifest rows (all matched)
  inputs/symbol_bindings.json  decorated symbol -> address from data/names.csv
  inputs/rtti_classes.json     the exe's RTTI graph: type descriptors, locators, vtables
  inputs/crt_tail_layout.json  the library tail of .text (tail_layout.py over the toolchain's LIB)
  inputs/function_extents.json data/functions.csv as slots: size, the gap to the next
                               function and what fills it; engine functions are the ones
                               before the library tail

Usage: uv run tools/exelink/prepare.py [--objdir build/exelink/obj]
With --objdir (after build.py) the call targets of matched code settle library sections
that match in more than one place.
"""
import argparse
import csv
import hashlib
import json
import os
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent))
import paths  # noqa: E402
from exebuild import build_of, restore  # noqa: E402

DATA = paths.ROOT / "data"


def hx(s):
    return int(s, 16)


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=1) + "\n")


def pe_sections(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    count = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 52)[0]
    out = {}
    for i in range(count):
        name, vsize, va, rawsize, rawptr = struct.unpack_from("<8sIIII", data, pe + 24 + opt + 40 * i)
        out[name.rstrip(b"\0").decode()] = (base + va, vsize, rawptr, rawsize)
    return base, out


def restored_image(source):
    """orig/gamemd.exe with the sites its build changed after linking put back (file offsets)."""
    data = bytearray(source.read_bytes())
    build = build_of(bytes(data))
    _, secs = pe_sections(data)
    start, _, rawptr, rawsize = secs[".text"]
    text = bytearray(data[rawptr:rawptr + rawsize])
    restore(text, start, build)
    data[rawptr:rawptr + rawsize] = text
    for name in (".rdata", ".data"):   # sites outside .text (a few builds patch data too)
        start, _, rawptr, rawsize = secs[name]
        chunk = bytearray(data[rawptr:rawptr + rawsize])
        restore(chunk, start, build)
        data[rawptr:rawptr + rawsize] = chunk
    return bytes(data), build


def manifest_rows():
    rows = []
    with (DATA / "manifest.csv").open() as f:
        for r in csv.DictReader(f):
            rows.append({"tu": r["source"], "addr": f"0x{hx(r['address']):X}", "sym": r["symbol"],
                         "len": f"0x{int(r['size']):X}", "status": "matched", "name": r["symbol"]})
    return rows


def bindings():
    out = {}
    with (DATA / "names.csv").open() as f:
        for r in csv.DictReader(f):
            if r.get("symbol"):
                out.setdefault(r["symbol"], f"0x{hx(r['address']):08X}")
    return out


def rtti_classes(data):
    """{mangled type name: type descriptor, complete object locators and their vtables}, found by
    walking every .rdata dword that points at a locator whose type descriptor names a class."""
    base, secs = pe_sections(data)

    def where(va):
        for start, vsize, rawptr, rawsize in secs.values():
            if start <= va < start + min(vsize, rawsize):
                return rawptr + va - start
        return None

    def dword(va):
        off = where(va)
        return None if off is None else struct.unpack_from("<I", data, off)[0]

    def type_name(td):
        off = where(td + 8)
        if off is None or data[off:off + 3] != b".?A":
            return None
        end = data.index(b"\0", off)
        return data[off:end].decode("latin-1")

    classes, located = {}, set()
    start, vsize, rawptr, rawsize = secs[".rdata"]
    for va in range(start, start + min(vsize, rawsize) - 4, 4):
        col = struct.unpack_from("<I", data, rawptr + va - start)[0]
        if where(col) is None or col % 4:
            continue
        head = [dword(col + 4 * k) for k in range(5)]
        if None in head or head[0] != 0:
            continue
        _, offset, cd_offset, td, chd = head
        name = type_name(td) if where(td) is not None else None
        if name is None or where(chd) is None:
            continue
        info = classes.setdefault(name, {"mangled_name": name, "type_descriptor_va": f"0x{td:08X}", "locators": []})
        info["locators"].append({"locator_va": f"0x{col:08X}", "base_offset": offset, "cd_offset": cd_offset,
                                 "class_hierarchy_va": f"0x{chd:08X}", "vtable_va": f"0x{va + 4:08X}"})
        located.add(name)
    bare = set()
    start, vsize, rawptr, rawsize = secs[".data"]
    blob = data[rawptr:rawptr + min(vsize, rawsize)]
    pos = blob.find(b".?A")
    while pos >= 0:
        end = blob.find(b"\0", pos)
        name = blob[pos:end].decode("latin-1")
        if name not in located and (pos - 8) % 4 == 0:
            bare.add(name)
        pos = blob.find(b".?A", end)
    return {"_meta": {"source": "the player's gamemd.exe, read by tools/exelink/prepare.py"},
            "classes": classes, "type_descriptors_without_locator": sorted(bare)}


def extents(data, layout):
    """function_extents.json rows [addr, len, gap, gap kind, kind, flags] from data/functions.csv."""
    base, secs = pe_sections(data)
    text_start, text_vsize, rawptr, rawsize = secs[".text"]
    library = set()
    with (DATA / "library.csv").open() as f:
        for r in csv.DictReader(f):
            library.add(hx(r["address"]))
    with (DATA / "functions.csv").open() as f:
        funcs = sorted((hx(r["address"]), int(r["size"])) for r in csv.DictReader(f))
    tail = hx(layout["tail_start"])
    first_thunk = min([hx(t["addr"]) for t in layout["thunks"]] + [tail])
    with (DATA / "manifest.csv").open() as f:
        matched = {hx(r["address"]) for r in csv.DictReader(f)}
    padding = [(va, size) for va, size in funcs if va not in matched and set(
        data[rawptr + va - text_start:rawptr + va + size - text_start]) <= {0x90, 0xCC}]
    funcs = [f for f in funcs if f not in padding]   # padding listed as a function would shrink a real slot
    rows = []
    for i, (va, size) in enumerate(funcs):
        nxt = funcs[i + 1][0] if i + 1 < len(funcs) else text_start + text_vsize
        if va < first_thunk:
            nxt = min(nxt, first_thunk)
        gap = max(nxt - va - size, 0)
        raw = bytes(data[rawptr + va + size - text_start:rawptr + va + size + gap - text_start])
        if not raw:
            kind = "none"
        elif raw == b"\xcc" * gap:
            kind = "cc"
        elif raw == b"\x90" * gap:
            kind = "nop"
        elif raw == b"\0" * gap:
            kind = "zero"
        else:
            kind = "mixed"
        owner = "crt" if va in library or va >= first_thunk else "engine"
        rows.append([f"0x{va:X}", f"0x{size:X}", f"0x{gap:X}", kind, owner, []])
    return {"schema": "exelink/function_extents/1", "fields": ["addr", "len", "gap"],
            "padding_entries_dropped": [f"0x{va:08X}" for va, _ in padding],
            "provenance": "data/functions.csv; gaps read from the player's exe", "entries": rows}


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--exe", type=Path, default=paths.ROOT / "orig/gamemd.exe")
    ap.add_argument("--objdir", type=Path, help="build.py output, to settle ambiguous library sections")
    args = ap.parse_args(argv)
    image, build = restored_image(args.exe)
    paths.RETAIL.parent.mkdir(parents=True, exist_ok=True)
    paths.RETAIL.write_bytes(image)
    os.environ["GAMEMD_BINARY"] = str(paths.RETAIL)
    write_json(paths.TOTALS, {"_meta": {"sha256": hashlib.sha256(image).hexdigest(), "build": build}})
    manifest = manifest_rows()
    write_json(paths.MANIFEST, manifest)
    write_json(paths.BINDINGS, bindings())
    write_json(paths.RTTI_CLASSES, rtti_classes(image))
    import tail_layout
    hints = tail_layout.call_hints(args.objdir) if args.objdir else None
    layout = tail_layout.build(paths.LIBDIR, hints)
    write_json(paths.TAIL_LAYOUT, layout)
    write_json(paths.EXTENTS, extents(image, layout))
    print(f"{args.exe} ({build}) restored to 1.001 -> {paths.RETAIL}")
    dropped = json.loads(paths.EXTENTS.read_text())["padding_entries_dropped"]
    if dropped:
        print(f"data/functions.csv entries that are only padding (left out of the layout): {', '.join(dropped)}")
    print(f"{len(manifest)} matched rows; {len(layout['sections'])} library sections and "
          f"{len(layout['thunks'])} import thunks in the tail from {layout['tail_start']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
