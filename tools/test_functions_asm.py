"""MASM evidence must identify a section independently of alignment and of source annotations."""
import struct
import unittest

from coff import CODE, REL_DIR32, REL_REL32, STORAGE_EXTERNAL, CoffObject, Reloc, Section, Symbol
from functions import assembly_bounds


def archive(data, entries, relocs=()):
    obj = CoffObject.__new__(CoffObject)
    obj.sections = [Section(1, ".text", data, CODE, list(relocs))]
    obj.symbols = [Symbol(i, name, off, 1, 0x20, STORAGE_EXTERNAL)
                   for i, (off, name) in enumerate(entries)]
    obj.by_index = {s.index: s for s in obj.symbols}
    return obj


class AssemblyBoundsTest(unittest.TestCase):
    def test_complete_contiguous_run_can_follow_a_changed_procedure(self):
        a, b = b'first complete body', b'second complete body'
        obj = archive(b'old prefix' + a + b, [(0, 'changed'), (10, 'a'), (10 + len(a), 'b')])
        evidence = {100: 'CG', 105: 'P', 100 + len(a): 'P'}
        self.assertEqual(assembly_bounds(obj, a + b, 100, evidence, set()),
                         {100: 100 + len(a), 100 + len(a): 100 + len(a + b)})
        self.assertNotIn(105, evidence)
        evidence = {100: 'CG', 105: 'V'}
        self.assertNotIn(100, assembly_bounds(obj, a + b, 100, evidence, set()))
        self.assertEqual(evidence[105], 'V')
        self.assertEqual(assembly_bounds(obj, a + b'gap' + b, 100, {100: 'CG'}, set()), {})

    def test_unanchored_run_requires_two_verified_external_data_locations(self):
        a = b'first code' + bytes(8)
        b = b'second code' + bytes(8)
        prefix = b'changed prefix'
        relocs = [Reloc(len(prefix) + off, 3 + i, REL_DIR32)
                  for offset in (len(a) - 8, len(a) + len(b) - 8)
                  for i, off in enumerate((offset, offset + 4))]
        obj = archive(prefix + a + b, [(0, 'changed'), (len(prefix), 'a'),
                      (len(prefix) + len(a), 'b')], relocs)
        for i, name in enumerate(('_one', '_two'), 3):
            sym = Symbol(i, name, 0, 0, 0, STORAGE_EXTERNAL)
            obj.symbols.append(sym); obj.by_index[i] = sym
        image = bytearray(a + b)
        for r in relocs:
            struct.pack_into('<I', image, r.offset - len(prefix), 0x2000 + (r.symbol - 3) * 4)
        expected = {100: 100 + len(a), 100 + len(a): 100 + len(a + b)}
        bindings = {'_one': 0x2000, '_two': 0x2004}
        self.assertEqual(assembly_bounds(obj, bytes(image), 100, {}, set(), bindings), expected)
        for bad in ({}, {'_one': 0x2000}, {'_one': 0x2000, '_two': 0x2008}):
            self.assertEqual(assembly_bounds(obj, bytes(image), 100, {}, set(), bad), {})
        obj.symbols[3].storage = 3
        self.assertEqual(assembly_bounds(obj, bytes(image), 100, {}, set(), bindings), {})
        obj.symbols[3].storage = STORAGE_EXTERNAL
        obj.sections[0].relocs[0].type = REL_REL32
        self.assertEqual(assembly_bounds(obj, bytes(image), 100, {}, set(), bindings), {})

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
