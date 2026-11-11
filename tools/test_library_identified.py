import copy
import struct
from types import SimpleNamespace
import unittest
from unittest.mock import Mock

from coff import CODE, REL_REL32, STORAGE_EXTERNAL, CoffObject, Reloc, Section, Symbol
from library_identified import Audit, identity_records, merge
from rtti import Rtti


def audit_fixture(code, image, relocations=()):
    audit = Audit.__new__(Audit)
    audit.base, audit.text = 0x1000, image
    audit.anchors, audit.names, audit.local = {}, {}, {}
    read = lambda address, size: image[address - audit.base:address - audit.base + size]
    audit.original = SimpleNamespace(
        read=read, u32=lambda address: struct.unpack("<I", read(address, 4))[0],
        decorated_addresses={})
    function = Symbol(0, "_caller", 0, 1, 0x20, STORAGE_EXTERNAL)
    target = Symbol(1, "_target", 0, 0, 0x20, STORAGE_EXTERNAL)
    section = Section(1, ".text", code, CODE, list(relocations))
    obj = CoffObject.__new__(CoffObject)
    obj.sections, obj.symbols = [section], [function, target]
    obj.by_index = {0: function, 1: target}
    audit.object = Mock(return_value=("fixture.obj", b"", obj,
                                     (function, section, 0, len(code))))
    return audit


ENTRY = {"address": "0x00001000", "size": 6, "symbol": "_caller",
         "library": "FIXTURE.LIB", "object": "fixture.obj"}


class LibraryIdentityTests(unittest.TestCase):
    def test_overlapping_anchor_matches_are_ambiguous(self):
        audit = audit_fixture(b"AA", b"AAA")
        with self.assertRaisesRegex(ValueError, "one independent location"):
            audit.unique_anchor(ENTRY)

    def test_incoming_relocation_includes_addend_and_place(self):
        code = b"\xe8" + struct.pack("<i", 3) + b"\xc3"
        image = b"\xe8" + struct.pack("<i", 0x2000 + 3 - 0x1005) + b"\xc3"
        audit = audit_fixture(code, image, [Reloc(1, 1, REL_REL32)])
        anchor = audit.unique_anchor(ENTRY)
        self.assertEqual(anchor["references"][0]["target"], "0x00002000")
        audit.linked_body(ENTRY, {"_target": 0x2000})
        with self.assertRaisesRegex(ValueError, "Unproved reference"):
            audit.linked_body(ENTRY, {"_target": 0x2001})
        with self.assertRaisesRegex(ValueError, "Unproved reference"):
            audit.linked_body(ENTRY, {})

    def test_linked_body_rejects_byte_changes_and_unsupported_relocations(self):
        code = b"\xe8" + struct.pack("<i", 0) + b"\xc3"
        image = b"\xe8" + struct.pack("<i", 0x2000 - 0x1005) + b"\xc2"
        audit = audit_fixture(code, image, [Reloc(1, 1, REL_REL32)])
        with self.assertRaisesRegex(ValueError, "body differs"):
            audit.linked_body(ENTRY, {"_target": 0x2000})
        audit = audit_fixture(code, image, [Reloc(1, 1, 0x10)])
        with self.assertRaisesRegex(ValueError, "Unchecked relocation"):
            audit.linked_body(ENTRY, {"_target": 0x2000})

    def test_same_anchor_address_in_different_member_is_rechecked(self):
        audit = audit_fixture(b"AA", b"AA")
        audit.unique_anchor(ENTRY)
        audit.unique_anchor({**ENTRY, "object": "other.obj"})
        self.assertEqual(audit.object.call_count, 2)

    def test_rtti_name_must_agree_with_destructor_symbol(self):
        rtti = Rtti.__new__(Rtti)
        rtti.types = {".?AVbad_cast@@", ".?AVbad_typeid@@"}
        rtti.bodies = {name[4:] for name in rtti.types}
        rtti.type_of_vtable = lambda address: ".?AVbad_cast@@"
        rtti.owner = lambda address: ".?AVbad_cast@@"
        rtti.calls = lambda caller, target: True
        audit = audit_fixture(b"", b"")
        audit.original.rtti = lambda: rtti
        audit.original.u32 = lambda address: 0x2000
        audit.linked_body = Mock(return_value={"size": 5})
        entry = {**ENTRY, "size": 5, "symbol": "??1bad_cast@@UAE@XZ",
                 "kind": "rtti", "rtti_type": ".?AVbad_cast@@",
                 "vtable": "0x00003000", "deleting_destructor": "0x00002000"}
        audit.target(entry)
        with self.assertRaisesRegex(ValueError, "does not name the RTTI class"):
            audit.target({**entry, "symbol": "??1bad_typeid@@UAE@XZ"})

    def test_record_validation_and_merging_preserve_existing_entries(self):
        with self.assertRaisesRegex(ValueError, "Duplicate"):
            identity_records([ENTRY, copy.deepcopy(ENTRY)])
        for size in (0, -1, True):
            with self.subTest(size=size), self.assertRaises(ValueError):
                identity_records([{**ENTRY, "size": size}])
        row = (6, "_f", "LIB.LIB", "f.obj")
        old = {0x1000: row}
        self.assertEqual(merge(old, {0x1006: row}), {0x1000: row, 0x1006: row})
        self.assertEqual(merge(old, old), old)
        with self.assertRaisesRegex(ValueError, "Conflicting"):
            merge(old, {0x1000: (5, *row[1:])})
        for address in (0xFFF, 0x1001):
            with self.subTest(address=address), self.assertRaisesRegex(ValueError, "Overlapping"):
                merge(old, {address: row})
        self.assertEqual(old, {0x1000: row})


if __name__ == "__main__":
    unittest.main()
