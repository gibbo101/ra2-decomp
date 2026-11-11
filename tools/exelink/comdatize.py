#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""COFF post-pass for objects written by the toolchain's ml.exe (MASM 6.15).

ml emits one non-COMDAT .text section per object, and LINK cannot place such a section through
/ORDER (warning LNK4065). The original hand-written assembly objects are one COMDAT-like unit
each, so this pass marks every non-empty executable .text or .text$ section
IMAGE_SCN_LNK_COMDAT and sets the selection byte of its section-symbol aux record to ANY. LINK keys a COMDAT on the first symbol
the symbol table defines in it; where that is a file-local label (the archived objects
define helpers before the public routine at offset 0), the public routine's entry is swapped
ahead of it and every relocation's symbol index follows. The bytes are left alone.
split() first cuts a code section where the original began a new object at one of its
routines (build.py reads those from data/asm_objects.csv), so each part is placed on its own.

Usage: comdatize.py IN.obj [OUT.obj]    (in place when OUT is omitted)
"""
import struct
import sys

import coffgen

CNT_CODE = coffgen.SCN_CNT_CODE


def comdatize(data):
    """Return (object bytes, number of sections converted)."""
    data = bytearray(data)
    if struct.unpack_from("<H", data, 0)[0] != coffgen.I386:
        raise ValueError("not an i386 COFF object")
    nsec = struct.unpack_from("<H", data, 2)[0]
    symptr = struct.unpack_from("<I", data, 8)[0]
    converted = 0
    strtab = symptr + 18 * struct.unpack_from("<I", data, 12)[0]
    for idx in range(1, nsec + 1):
        off = 20 + 40 * (idx - 1)
        name = section_name(data, off, strtab)
        flags = struct.unpack_from("<I", data, off + 36)[0]
        size = struct.unpack_from("<I", data, off + 16)[0]
        if (name != ".text" and not name.startswith(".text$")) or not size \
                or not flags & CNT_CODE or flags & coffgen.SCN_LNK_COMDAT:
            continue
        aux = None
        for sidx, sname, value, sec, scl, naux in coffgen.read_symbols(bytes(data)):
            if sec == idx and scl == coffgen.CLASS_STATIC and value == 0 and naux and sname == name:
                aux = symptr + 18 * (sidx + 1)
                break
        if aux is None:
            raise ValueError(f"section symbol for {name} not found")
        struct.pack_into("<I", data, off + 36, flags | coffgen.SCN_LNK_COMDAT)
        data[aux + 14] = coffgen.COMDAT_ANY
        key_first(data, idx)
        converted += 1
    return bytes(data), converted


def split(data, symbol):
    """Copy of the object whose code section holding `symbol` ends where `symbol` starts; `symbol`
    and everything after it move to a new last section with the same header."""
    data = bytes(data)
    nsec, symptr, nsyms = struct.unpack_from("<H", data, 2)[0], *struct.unpack_from("<II", data, 8)
    if struct.unpack_from("<H", data, 16)[0]:
        raise ValueError("objects with an optional header are not split")
    strtab = data[symptr + 18 * nsyms:]
    symbols = list(coffgen.read_symbols(data))
    at = next((s for s in symbols if s[1] == symbol and s[3] > 0), None)
    if at is None:
        raise ValueError(f"{symbol} is not defined")
    section, cut = at[3], at[2]
    heads = [bytearray(data[20 + 40 * i:20 + 40 * i + 40]) for i in range(nsec)]
    flags = struct.unpack_from("<I", heads[section - 1], 36)[0]
    if not cut or struct.unpack_from("<H", heads[section - 1], 34)[0] or flags & coffgen.SCN_LNK_NRELOC_OVFL:
        raise ValueError(f"{symbol} cannot start a section")
    sec_sym = next(s for s in symbols if s[3] == section and s[4] == coffgen.CLASS_STATIC
                   and s[2] == 0 and s[5] and s[1].startswith("."))
    moved = {s[0] for s in symbols if s[3] == section and s[2] >= cut and s[0] != sec_sym[0]}
    newsec = nsec + 1
    record = lambda i: bytearray(data[symptr + 18 * i:symptr + 18 * i + 18])  # noqa: E731

    # The new section symbol and its aux go just before the first moved symbol, so the new
    # section's first symbol is its section symbol and its COMDAT key can follow.
    table, remap, new_sym = [], {}, None
    for s in symbols:
        if s[0] == min(moved):
            new_sym = len(table)
            sym = record(sec_sym[0])
            struct.pack_into("<h", sym, 12, newsec)
            table += [sym, record(sec_sym[0] + 1)]
        remap[s[0]] = len(table)
        recs = [record(i) for i in range(s[0], s[0] + 1 + s[5])]
        if s[0] in moved:
            struct.pack_into("<Ih", recs[0], 8, s[2] - cut, newsec)
        table += recs

    def relocations(h):
        ptr, n = struct.unpack_from("<I", h, 24)[0], struct.unpack_from("<H", h, 32)[0]
        return [struct.unpack_from("<IIH", data, ptr + 10 * r) for r in range(n)]

    def contents(h):
        size, ptr = struct.unpack_from("<II", h, 16)
        return data[ptr:ptr + size] if ptr else None

    relocs = {i: relocations(h) for i, h in enumerate(heads, 1)}
    bodies = {i: contents(h) for i, h in enumerate(heads, 1)}
    relocs[newsec] = [(va - cut, idx, kind) for va, idx, kind in relocs[section] if va >= cut]
    if any(va < cut < va + 4 for va, _, _ in relocs[section]):
        raise ValueError(f"a relocation straddles {symbol}")
    relocs[section] = [r for r in relocs[section] if r[0] < cut]
    bodies[newsec], bodies[section] = bodies[section][cut:], bodies[section][:cut]
    heads.append(bytearray(heads[section - 1]))
    for sec, idx in ((section, remap[sec_sym[0]]), (newsec, new_sym)):
        struct.pack_into("<IH", table[idx + 1], 0, len(bodies[sec]), len(relocs[sec]))

    pos, blob = 20 + 40 * len(heads), bytearray()
    for i, h in enumerate(heads, 1):
        body = bodies[i]
        if body is not None:
            struct.pack_into("<II", h, 16, len(body), pos + len(blob) if body else 0)
            blob += body
        rel = b"".join(struct.pack("<IIH", va, remap[idx], kind) for va, idx, kind in relocs[i])
        struct.pack_into("<I", h, 24, pos + len(blob) if rel else 0)
        struct.pack_into("<H", h, 32, len(relocs[i]))
        blob += rel
    header = bytearray(data[:20])
    struct.pack_into("<H", header, 2, len(heads))
    struct.pack_into("<II", header, 8, pos + len(blob), len(table))
    return bytes(header) + b"".join(heads) + bytes(blob) + b"".join(bytes(r) for r in table) + strtab


def section_name(data, off, strtab):
    """A section header's name, following a /offset into the string table for long names."""
    raw = bytes(data[off:off + 8]).rstrip(b"\0")
    if raw.startswith(b"/") and raw[1:].isdigit():
        at = strtab + int(raw[1:])
        return bytes(data[at:data.index(b"\0", at)]).decode("latin-1")
    return raw.decode("latin-1")


def key_first(data, section):
    """Make the external symbol at offset 0 of `section` the first one defined in it (its COMDAT key)."""
    symbols = list(coffgen.read_symbols(bytes(data)))
    defined = [s for s in symbols if s[3] == section and s[1] != ".text" and not (s[4] == coffgen.CLASS_STATIC and s[5])]
    if not defined or defined[0][4] == coffgen.CLASS_EXTERNAL and defined[0][2] == 0:
        return
    head = next((s for s in defined if s[4] == coffgen.CLASS_EXTERNAL and s[2] == 0), None)
    first = defined[0]
    if head is None or first[5] or head[5]:
        return
    a, b = first[0], head[0]
    symptr = struct.unpack_from("<I", data, 8)[0]
    ra, rb = bytes(data[symptr + 18 * a:symptr + 18 * a + 18]), bytes(data[symptr + 18 * b:symptr + 18 * b + 18])
    data[symptr + 18 * a:symptr + 18 * a + 18] = rb
    data[symptr + 18 * b:symptr + 18 * b + 18] = ra
    for i in range(struct.unpack_from("<H", data, 2)[0]):
        off = 20 + 40 * i
        relptr, nrel = struct.unpack_from("<I", data, off + 24)[0], struct.unpack_from("<H", data, off + 32)[0]
        for r in range(nrel):
            at = relptr + 10 * r + 4
            idx = struct.unpack_from("<I", data, at)[0]
            if idx in (a, b):
                struct.pack_into("<I", data, at, b if idx == a else a)


def main(argv):
    if len(argv) not in (2, 3):
        print(__doc__.strip().splitlines()[-1], file=sys.stderr)
        return 2
    out = argv[-1]
    with open(argv[1], "rb") as stream:
        result, count = comdatize(stream.read())
    if not count:
        print("comdatize: no non-COMDAT .text section in " + argv[1], file=sys.stderr)
        return 1
    with open(out, "wb") as stream:
        stream.write(result)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
