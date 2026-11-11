"""Assembly data evidence must come from independent, complete C++ matches."""
import copy
import csv
import hashlib
from pathlib import Path
import struct
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import assembly_anchors as anchors
from coff import CODE, REL_DIR32, STORAGE_EXTERNAL, CoffObject, Reloc, Section, Symbol


class AssemblyAnchorTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name).resolve()
        for name in ('src', 'data', 'build'):
            (self.root / name).mkdir()
        self.source = self.root / 'src/anchor.cpp'
        self.source.write_text('// FUNCTION: GAMEMD 0x00001000\nvoid a() {}\n'
                               '// FUNCTION: GAMEMD 0x00001016\nvoid b() {}\n')
        self.rows = [dict(address=f'0x{0x1000 + i * 22:08X}', size='22',
                          source='src/anchor.cpp', symbol='_' + name,
                          source_sha256=hashlib.sha256(self.source.read_bytes()).hexdigest(),
                          globals='_first;_second') for i, name in enumerate(('a', 'b'))]
        self.manifest = self.root / 'anchors.csv'
        self.write(self.root / 'data/progress.csv', [
            {**{k: r[k] for k in ('address', 'size', 'source', 'symbol')}, 'result': 'MATCH'}
            for r in self.rows])
        pieces = [b'ANCHOR_A' + bytes(4) + b'DATA_A' + bytes(4),
                  b'ANCHOR_B' + bytes(4) + b'DATA_B' + bytes(4)]
        data = b''.join(pieces)
        relocations = [Reloc(i * 22 + off, sym, REL_DIR32)
                       for i in range(2) for off, sym in ((8, 2), (18, 3))]
        self.obj = CoffObject.__new__(CoffObject)
        self.obj.sections = [Section(1, '.text', data, CODE, relocations)]
        self.obj.symbols = [Symbol(0, '_a', 0, 1, 0x20, STORAGE_EXTERNAL),
                            Symbol(1, '_b', 22, 1, 0x20, STORAGE_EXTERNAL),
                            Symbol(2, '_first', 0, 0, 0, STORAGE_EXTERNAL),
                            Symbol(3, '_second', 0, 0, 0, STORAGE_EXTERNAL)]
        self.obj.by_index = {s.index: s for s in self.obj.symbols}
        self.image = bytearray(data)
        for r in relocations:
            struct.pack_into('<I', self.image, r.offset, 0x2000 + (r.symbol - 2) * 4)
        self.original = SimpleNamespace(base=0, sizes={0x1000: 22, 0x1016: 22},
            pe=SimpleNamespace(sections=[
                SimpleNamespace(VirtualAddress=0x1000, Misc_VirtualSize=44, Characteristics=CODE),
                SimpleNamespace(VirtualAddress=0x2000, Misc_VirtualSize=16, Characteristics=0xC0)]),
            read=lambda at, size: bytes(self.image[at - 0x1000:at - 0x1000 + size]),
            u32=lambda at: struct.unpack_from('<I', self.image, at - 0x1000)[0])
        self.object_path = self.root / 'build/anchor.obj'
        self.object_path.write_bytes(b'synthetic COFF fixture')
        self.result = SimpleNamespace(matched=True, unverified=0, warnings=[], proven=['a', 'b'])
        for obj, attr, value in (
            (anchors, 'ROOT', self.root),
            (anchors.check, 'Original', lambda exe: self.original),
            (anchors.coff, 'CoffObject', lambda raw: self.obj),
            (anchors.check, 'known_decorated', lambda: {'_first': 0xDEAD}),
            (anchors.check, 'known_names', lambda: {'first': 0xDEAD}),
            (anchors.check, 'compare', self.compare)):
            patcher = patch.object(obj, attr, value)
            patcher.start(); self.addCleanup(patcher.stop)
        patcher = patch.object(anchors.check, 'compile_source', return_value=(self.object_path, ''))
        self.compile = patcher.start(); self.addCleanup(patcher.stop)

    @staticmethod
    def write(path, rows):
        with path.open('w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=list(rows[0]))
            writer.writeheader(); writer.writerows(rows)

    def compare(self, original, obj, raw, address, symbol, names, local):
        self.assertNotIn('first', names)
        self.assertEqual(local['_first'], 0x2000)
        self.assertEqual(local['_second'], 0x2004)
        return self.result

    def verify(self, rows=None):
        self.write(self.manifest, self.rows if rows is None else rows)
        return anchors.verified_data_symbols(self.root / 'original.exe', self.root / 'build', self.manifest)

    def test_two_complete_anchors_derive_bindings_and_compile_each_source_once(self):
        self.assertEqual(self.verify(), {'_first': 0x2000, '_second': 0x2004})
        self.compile.assert_called_once()

    def test_stale_or_duplicate_metadata_is_rejected_before_compilation(self):
        for key, value in [('source_sha256', '0' * 64), ('size', '23'), ('symbol', '_wrong')]:
            rows = copy.deepcopy(self.rows); rows[0][key] = value
            with self.subTest(key=key), self.assertRaises(ValueError):
                self.verify(rows)
        with self.assertRaisesRegex(ValueError, 'Duplicate'):
            self.verify(self.rows + [self.rows[0]])
        self.compile.assert_not_called()

    def test_single_or_disagreeing_anchors_are_rejected(self):
        with self.assertRaisesRegex(ValueError, 'two agreeing'):
            self.verify(self.rows[:1])
        struct.pack_into('<I', self.image, 22 + 8, 0x2008)
        with self.assertRaisesRegex(ValueError, 'two agreeing'):
            self.verify()

    def test_data_must_be_external_noncode_and_completely_verified(self):
        struct.pack_into('<I', self.image, 8, 0x1000)
        with self.assertRaisesRegex(ValueError, 'outside non-code'):
            self.verify()
        struct.pack_into('<I', self.image, 8, 0x2000)
        self.obj.symbols[2].type = 0x20
        with self.assertRaisesRegex(ValueError, 'external data'):
            self.verify()
        self.obj.symbols[2].type = 0
        self.result.unverified = 1
        with self.assertRaisesRegex(ValueError, 'complete comparator'):
            self.verify()
        self.result.unverified = 0
        self.result.proven = ['a']
        with self.assertRaisesRegex(ValueError, 'complete comparator'):
            self.verify()
