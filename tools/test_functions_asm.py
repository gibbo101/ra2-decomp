"""MASM evidence must identify a section independently of alignment and of source annotations."""
import struct
import unittest

from coff import CODE, REL_DIR32, STORAGE_EXTERNAL, CoffObject, Reloc, Section, Symbol
from functions import assembly_bounds


def archive(data, entries, relocs=()):
    obj = CoffObject.__new__(CoffObject)
    obj.sections = [Section(1, ".text", data, CODE, list(relocs))]
    obj.symbols = [Symbol(i, name, off, 1, 0x20, STORAGE_EXTERNAL)
                   for i, (off, name) in enumerate(entries)]
    return obj


class AssemblyBoundsTest(unittest.TestCase):
    def test_unique_relocated_section_places_short_entry_and_removes_only_inferred_starts(self):
        image = b"long section head....second body end"
        obj = archive(image.replace(b"....", bytes(4)), [(0, "a"), (4, "b")],
                      [Reloc(image.index(b"...."), 0, REL_DIR32)])
        evidence = {4: "CG", 16: "P", 23: "F"}
        self.assertEqual(assembly_bounds(obj, image, 0, evidence, set()), {0: 4, 4: len(image)})
        self.assertEqual(evidence, {0: "A", 4: "CG"})
        evidence[16] = "CG"
        self.assertNotIn(4, assembly_bounds(obj, image, 0, evidence, set()))
        self.assertEqual(evidence[16], "CG")

    def test_repeated_and_uncorroborated_sections_do_not_displace_inferred_starts(self):
        body = b"0123456789"
        obj = archive(body, [(0, "proc")])
        for image, flags in [(body * 2, "CG"), (body, "P"), (body, "A")]:
            evidence = {0: flags, 8: "P"}
            self.assertEqual(assembly_bounds(obj, image, 0, evidence, set()), {})
            self.assertEqual(evidence, {0: flags, 8: "P"})

    def test_last_procedure_of_split_section_needs_two_import_jumps(self):
        proc = b"1234567\xc3"
        obj = archive(b"absent" + proc, [(0, "other"), (6, "final")])
        jump = b"\xff\x25" + struct.pack("<I", 0x9000)
        for table, slots, result in [(jump * 2, {0x9000}, {10: 18}), (jump, {0x9000}, {}),
                                     (jump * 2, {0x9010}, {})]:
            self.assertEqual(assembly_bounds(obj, proc + table, 10, {10: "CG"}, slots), result)
