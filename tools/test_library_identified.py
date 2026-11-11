import copy
import json
from pathlib import Path
import struct
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch

from coff import CODE, REL_DIR32, REL_REL32, STORAGE_EXTERNAL, CoffObject, Reloc, Section, Symbol
from library_identified import Audit, identity_records, merge, verify
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


def block_fixture():
    codes = [bytes([65 + i]) * 19 for i in range(4)] + [b"\xc3"]
    audit = audit_fixture(b"", b"".join(codes))
    obj = CoffObject.__new__(CoffObject)
    obj.sections = [Section(i + 1, ".text", code, CODE) for i, code in enumerate(codes)]
    obj.symbols = [Symbol(i, f"_f{i}", 0, i + 1, 0x20, STORAGE_EXTERNAL) for i in range(5)]
    obj.by_index = {s.index: s for s in obj.symbols}
    picks = {p[0].name: p for p in obj.functions()}
    audit.object = lambda entry: ("fixture.obj", b"", obj, picks[entry["symbol"]])
    entry = {**ENTRY, "symbol": "_f4", "size": 1, "address": "0x0000104C",
             "kind": "object_block", "block": {
                 "address": "0x00001000", "size": 77, "symbols": list(picks),
                 "anchors": [{**ENTRY, "symbol": f"_f{i}",
                              "address": f"0x{0x1000 + i * 19:08X}"} for i in range(4)]}}
    return audit, entry


def table_fixture():
    audit = audit_fixture(b"\xc3", b"\xc3")
    table = b"DATA" + bytes(12) + b"END!"
    linked = b"DATA" + struct.pack("<III", 0x1000, 0x1000, 0x3000) + b"END!"
    section = Section(1, ".data", table, 0, [Reloc(4, 0, REL_DIR32),
                      Reloc(8, 0, REL_DIR32), Reloc(12, 1, REL_DIR32)])
    code = Section(2, ".text", b"\xc3", CODE)
    obj = CoffObject.__new__(CoffObject)
    obj.sections = [section, code]
    obj.symbols = [Symbol(0, "_tiny", 0, 2, 0x20, STORAGE_EXTERNAL),
                   Symbol(1, "_init", 0, 0, 0x20, STORAGE_EXTERNAL),
                   Symbol(2, "_pointer1", 4, 1, 0, STORAGE_EXTERNAL),
                   Symbol(3, "_pointer2", 8, 1, 0, STORAGE_EXTERNAL),
                   Symbol(4, "_table_anchor", 16, 1, 0, STORAGE_EXTERNAL)]
    obj.by_index = {s.index: s for s in obj.symbols}
    audit.object = lambda entry: ("fixture.obj", b"", obj, (obj.symbols[0], code, 0, 1))
    def read(address, size):
        base, data = (0x1000, b"\xc3") if address < 0x2000 else (0x2000, linked)
        return data[address - base:address - base + size]
    audit.original.read = read
    audit.original.base = 0
    audit.original.pe = SimpleNamespace(sections=[
        SimpleNamespace(VirtualAddress=0x1000, SizeOfRawData=1),
        SimpleNamespace(VirtualAddress=0x2000, SizeOfRawData=20)])
    audit.unique_anchor = lambda entry: {
        "address": entry["address"],
        "references": [{"symbol": "_table_anchor", "target": "0x00002010"}]}
    entry = {**ENTRY, "symbol": "_tiny", "size": 1, "kind": "data_table", "table": {
        "address": "0x00002000", "section": 1, "anchor_symbol": "_table_anchor",
        "anchor_address": "0x00002010", "anchors": [{**ENTRY, "symbol": "_caller"}],
        "target_pointers": ["_pointer1", "_pointer2"],
        "function_anchors": [{**ENTRY, "symbol": "_init", "address": "0x00003000"}]}}
    return audit, entry, obj


class LibraryIdentityTests(unittest.TestCase):
    def test_prerequisites_are_verified_first_and_cycles_are_rejected(self):
        rows = [{**ENTRY, 'symbol': '_first', 'kind': 'incoming',
                 'requires': ['0x00002000']},
                {**ENTRY, 'symbol': '_second', 'kind': 'incoming', 'address': '0x00002000'}]
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'identities.json'
            path.write_text(json.dumps({'targets': rows}))
            audit = Mock(local={})
            with patch('library_identified.Audit', return_value=audit):
                verify(path)
                self.assertEqual([c.args[0]['symbol'] for c in audit.target.call_args_list],
                                 ['_second', '_first'])
                self.assertEqual(audit.local, {'_second': 0x2000, '_first': 0x1000})
                rows[1]['requires'] = ['0x00001000']
                path.write_text(json.dumps({'targets': rows}))
                with self.assertRaisesRegex(ValueError, 'Cyclic'):
                    verify(path)
                rows[1]['requires'] = ['0x00003000']
                path.write_text(json.dumps({'targets': rows}))
                with self.assertRaisesRegex(ValueError, 'Unknown'):
                    verify(path)

    def test_block_accepts_static_functions_but_rejects_rebinding_members(self):
        audit, entry = block_fixture()
        obj = audit.object(entry)[2]
        obj.symbols[0].storage = 3
        audit.target(entry)
        entry['block']['external_anchors'] = [{**ENTRY, 'symbol': '_f4'}]
        with self.assertRaisesRegex(ValueError, 'Duplicate external anchor'):
            audit.target(entry)
        entry['block']['external_anchors'] = []
        obj.symbols[0].storage = 1
        with self.assertRaisesRegex(ValueError, 'entire code section'):
            audit.target(entry)

    def test_literal_relocations_require_the_complete_original_string(self):
        code = b'\x68' + bytes(4) + b'\xc3'
        image = b'\x68' + struct.pack('<I', 0x2000) + b'\xc3'
        audit = audit_fixture(code, image, [Reloc(1, 1, REL_DIR32)])
        obj = audit.object(ENTRY)[2]
        obj.symbols[1].name = '??_C@fixture'
        obj.symbols[1].section = 2
        obj.symbols[1].type = 0
        obj.sections.append(Section(2, '.rdata', b'hello\0', 0))
        read = audit.original.read
        audit.original.read = lambda at, size: b'hello\0'[:size] if at == 0x2000 else read(at, size)
        audit.linked_body(ENTRY, {})
        audit.original.read = lambda at, size: b'other\0'[:size] if at == 0x2000 else read(at, size)
        with self.assertRaisesRegex(ValueError, 'Unproved reference'):
            audit.linked_body(ENTRY, {})

    def test_block_data_requires_a_consistent_reference_and_full_definition(self):
        code = b'\xff\x05' + struct.pack('<i', 4) + b'\xc3'
        image = b'\xff\x05' + struct.pack('<I', 0x2004) + b'\xc3'
        audit = audit_fixture(code, image, [Reloc(2, 1, REL_DIR32)])
        obj = audit.object(ENTRY)[2]
        obj.symbols[1].type = 0
        definition = CoffObject.__new__(CoffObject)
        definition.sections = [Section(1, '.data', bytes(range(12)), 0)]
        definition.symbols = [Symbol(0, '_target', 4, 1, 0, STORAGE_EXTERNAL)]
        audit.object = lambda entry: ('definition.obj', b'', definition, None)
        read = audit.original.read
        audit.original.read = lambda at, size: bytes(range(12))[:size] if at == 0x1FFC else read(at, size)
        audit.original.base = 0
        audit.original.pe = SimpleNamespace(sections=[SimpleNamespace(
            VirtualAddress=0x1FFC, Misc_VirtualSize=12, Characteristics=0x40)])
        item = {'symbol': '_target', 'function': '_caller', 'definition': {}}
        self.assertEqual(audit.block_data(obj, {'_caller': 0x1000}, item), 0x2000)
        definition.sections[0].data = b'wrong data!!'
        with self.assertRaisesRegex(ValueError, 'definition differs'):
            audit.block_data(obj, {'_caller': 0x1000}, item)
        definition.sections[0].data = bytes(range(12))
        obj.sections[0].relocs[0].type = REL_REL32
        with self.assertRaisesRegex(ValueError, 'external data'):
            audit.block_data(obj, {'_caller': 0x1000}, item)

    def test_object_block_requires_complete_ordered_member_and_target_position(self):
        audit, entry = block_fixture()
        audit.target(entry)
        mutations = [lambda e: e.update(address="0x0000104B"),
                     lambda e: e["block"]["symbols"].pop(),
                     lambda e: e["block"]["symbols"].reverse(),
                     lambda e: e["block"]["anchors"].pop(),
                     lambda e: e["block"]["anchors"][0].update(object="wrong.obj")]
        for change in mutations:
            bad = copy.deepcopy(entry)
            change(bad)
            with self.subTest(entry=bad), self.assertRaises(ValueError):
                audit.target(bad)

    def test_data_table_requires_distinct_pointers_and_complete_relocations(self):
        audit, entry, obj = table_fixture()
        audit.target(entry)
        mutations = [lambda e: e.update(address="0x00001001"),
                     lambda e: e["table"].update(target_pointers=["_pointer1", "_pointer1"]),
                     lambda e: e["table"].update(target_pointers=["_pointer1"]),
                     lambda e: e["table"].update(function_anchors=[]),
                     lambda e: e["table"].update(anchor_address="0x00002011")]
        for change in mutations:
            bad = copy.deepcopy(entry)
            change(bad)
            with self.subTest(entry=bad), self.assertRaises(ValueError):
                audit.target(bad)
        obj.sections[0].relocs.append(Reloc(18, 0, REL_DIR32))
        with self.assertRaisesRegex(ValueError, "truncated"):
            audit.target(entry)

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
