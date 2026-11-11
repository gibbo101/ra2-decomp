#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""comdatize: an ml-shaped object gets a COMDAT ANY .text; everything else is untouched."""

import struct
import unittest

import coffgen
import comdatize


def ml_shaped_object(code=b"\x90\xc3"):
    """Header, one plain .text, one section symbol with aux, one external label."""
    text_flags = coffgen.SCN_CNT_CODE | coffgen.SCN_MEM_EXECUTE | coffgen.SCN_MEM_READ | coffgen.ALIGN[16]
    nsyms = 3
    raw = 20 + 40
    symptr = raw + len(code)
    head = struct.pack("<HHIIIHH", coffgen.I386, 1, 0, symptr, nsyms, 0, 0)
    sect = struct.pack("<8sIIIIIIHHI", b".text", 0, 0, len(code), raw, 0, 0, 0, 0, text_flags)
    sym_text = struct.pack("<8sIhHBB", b".text", 0, 1, 0, coffgen.CLASS_STATIC, 1)
    aux = struct.pack("<IHHIHBBBB", len(code), 0, 0, 0, 0, 0, 0, 0, 0)
    sym_label = struct.pack("<8sIhHBB", b"_label", 0, 1, 0x20, coffgen.CLASS_EXTERNAL, 0)
    return head + sect + code + sym_text + aux + sym_label + struct.pack("<I", 4)


def archive_shaped_object():
    """A file-local label (offset 0x10) defined before the public routine at offset 0, and a DIR32
    relocation against the label: the shape of the archived objects."""
    code = bytes(0x20)
    text_flags = coffgen.SCN_CNT_CODE | coffgen.SCN_MEM_EXECUTE | coffgen.SCN_MEM_READ | coffgen.ALIGN[16]
    raw = 20 + 40
    relptr = raw + len(code)
    symptr = relptr + 10
    head = struct.pack("<HHIIIHH", coffgen.I386, 1, 0, symptr, 4, 0, 0)
    sect = struct.pack("<8sIIIIIIHHI", b".text", 0, 0, len(code), raw, relptr, 0, 1, 0, text_flags)
    reloc = struct.pack("<IIH", 4, 2, coffgen.REL_DIR32)
    sym_text = struct.pack("<8sIhHBB", b".text", 0, 1, 0, coffgen.CLASS_STATIC, 1)
    aux = struct.pack("<IHHIHBBBB", len(code), 1, 0, 0, 0, 0, 0, 0, 0)
    helper = struct.pack("<8sIhHBB", b"_helper", 0x10, 1, 0x20, coffgen.CLASS_STATIC, 0)
    public = struct.pack("<8sIhHBB", b"_public", 0, 1, 0x20, coffgen.CLASS_EXTERNAL, 0)
    return head + sect + code + reloc + sym_text + aux + helper + public + struct.pack("<I", 4)


def grouped_object(code=b"\x90\xc3"):
    """An empty .text, then the code in .text$mmx (a long name, in the string table): ml's
    output for a file that only uses named code segments."""
    text_flags = coffgen.SCN_CNT_CODE | coffgen.SCN_MEM_EXECUTE | coffgen.SCN_MEM_READ | coffgen.ALIGN[16]
    raw = 20 + 2 * 40
    symptr = raw + len(code)
    head = struct.pack("<HHIIIHH", coffgen.I386, 2, 0, symptr, 5, 0, 0)
    empty = struct.pack("<8sIIIIIIHHI", b".text", 0, 0, 0, 0, 0, 0, 0, 0, text_flags)
    mmx = struct.pack("<8sIIIIIIHHI", b"/4", 0, 0, len(code), raw, 0, 0, 0, 0, text_flags)
    sym_text = struct.pack("<8sIhHBB", b".text", 0, 1, 0, coffgen.CLASS_STATIC, 1)
    aux_text = bytes(18)
    sym_mmx = struct.pack("<4sIIhHBB", b"\0\0\0\0", 4, 0, 2, 0, coffgen.CLASS_STATIC, 1)
    aux_mmx = struct.pack("<IHHIHBBBB", len(code), 0, 0, 0, 0, 0, 0, 0, 0)
    routine = struct.pack("<8sIhHBB", b"_routine", 0, 2, 0x20, coffgen.CLASS_EXTERNAL, 0)
    strings = b".text$mmx\0"
    return (head + empty + mmx + code + sym_text + aux_text + sym_mmx + aux_mmx + routine
            + struct.pack("<I", 4 + len(strings)) + strings)


def two_routine_object():
    """_first at 0 and _second at 0x10 in one .text, with a DIR32 at 0x14 against _first."""
    code = bytes(range(0x20))
    text_flags = coffgen.SCN_CNT_CODE | coffgen.SCN_MEM_EXECUTE | coffgen.SCN_MEM_READ | coffgen.ALIGN[4]
    raw = 20 + 40
    relptr = raw + len(code)
    symptr = relptr + 10
    head = struct.pack("<HHIIIHH", coffgen.I386, 1, 0, symptr, 4, 0, 0)
    sect = struct.pack("<8sIIIIIIHHI", b".text", 0, 0, len(code), raw, relptr, 0, 1, 0, text_flags)
    reloc = struct.pack("<IIH", 0x14, 2, coffgen.REL_DIR32)
    sym_text = struct.pack("<8sIhHBB", b".text", 0, 1, 0, coffgen.CLASS_STATIC, 1)
    aux = struct.pack("<IHHIHBBBB", len(code), 1, 0, 0, 0, 0, 0, 0, 0)
    first = struct.pack("<8sIhHBB", b"_first", 0, 1, 0x20, coffgen.CLASS_EXTERNAL, 0)
    second = struct.pack("<8sIhHBB", b"_second", 0x10, 1, 0x20, coffgen.CLASS_EXTERNAL, 0)
    return head + sect + code + reloc + sym_text + aux + first + second + struct.pack("<I", 4)


class ComdatizeTests(unittest.TestCase):
    def test_split_moves_the_routine_and_what_follows_to_a_keyed_section(self):
        out, count = comdatize.comdatize(comdatize.split(two_routine_object(), "_second"))
        self.assertEqual(count, 2)
        sections = [struct.unpack_from("<IIIIH", out, 20 + 40 * i + 16) for i in range(2)]
        self.assertEqual([(size, nrel) for size, _, _, _, nrel in sections], [(0x10, 0), (0x10, 1)])
        self.assertEqual(out[sections[0][1]:sections[0][1] + 0x10] + out[sections[1][1]:sections[1][1] + 0x10],
                         bytes(range(0x20)))
        symbols = list(coffgen.read_symbols(out))
        in_second = [s for s in symbols if s[3] == 2]
        self.assertEqual([(s[1], s[2]) for s in in_second], [(".text", 0), ("_second", 0)])
        self.assertEqual(out[struct.unpack_from("<I", out, 8)[0] + 18 * (in_second[0][0] + 1) + 14], coffgen.COMDAT_ANY)
        va, idx, _ = struct.unpack_from("<IIH", out, sections[1][2])
        self.assertEqual((va, next(s[1] for s in symbols if s[0] == idx)), (4, "_first"))

    def test_split_refuses_a_routine_at_the_section_start(self):
        with self.assertRaises(ValueError):
            comdatize.split(two_routine_object(), "_first")

    def test_public_routine_becomes_the_comdat_key(self):
        out, _ = comdatize.comdatize(archive_shaped_object())
        symbols = list(coffgen.read_symbols(out))
        first = next(s for s in symbols if s[3] == 1 and s[1] != ".text")
        self.assertEqual((first[1], first[2], first[4]), ("_public", 0, coffgen.CLASS_EXTERNAL))
        relptr = struct.unpack_from("<I", out, 20 + 24)[0]
        target = struct.unpack_from("<I", out, relptr + 4)[0]
        self.assertEqual(next(s[1] for s in symbols if s[0] == target), "_helper")   # the relocation follows its symbol

    def test_marks_text_comdat_any(self):
        src = ml_shaped_object()
        out, count = comdatize.comdatize(src)
        self.assertEqual(count, 1)
        flags = struct.unpack_from("<I", out, 20 + 36)[0]
        self.assertTrue(flags & coffgen.SCN_LNK_COMDAT)
        symptr = struct.unpack_from("<I", out, 8)[0]
        self.assertEqual(out[symptr + 18 + 14], coffgen.COMDAT_ANY)
        self.assertEqual(len(out), len(src))
        diff = [i for i in range(len(src)) if src[i] != out[i]]
        self.assertEqual(set(diff), {20 + 36 + 1, symptr + 18 + 14})

    def test_grouped_code_section_converts_and_empty_text_stays(self):
        out, count = comdatize.comdatize(grouped_object())
        self.assertEqual(count, 1)
        self.assertFalse(struct.unpack_from("<I", out, 20 + 36)[0] & coffgen.SCN_LNK_COMDAT)
        self.assertTrue(struct.unpack_from("<I", out, 20 + 40 + 36)[0] & coffgen.SCN_LNK_COMDAT)
        symptr = struct.unpack_from("<I", out, 8)[0]
        self.assertEqual(out[symptr + 18 * 3 + 14], coffgen.COMDAT_ANY)

    def test_idempotent(self):
        once, _ = comdatize.comdatize(ml_shaped_object())
        twice, count = comdatize.comdatize(once)
        self.assertEqual(count, 0)
        self.assertEqual(once, twice)

    def test_rejects_non_i386(self):
        with self.assertRaises(ValueError):
            comdatize.comdatize(b"\x64\x86" + bytes(40))


if __name__ == "__main__":
    unittest.main()
