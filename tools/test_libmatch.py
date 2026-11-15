from pathlib import Path
import struct
import tempfile
import unittest

from types import SimpleNamespace

from coff import REL_DIR32, REL_REL32
from libmatch import import_fields, imports_agree, match_local_calls, short_imports


def short(symbol, dll, hint, how):
    data = symbol.encode() + b"\0" + dll.encode() + b"\0"
    return struct.pack("<HHHHIIHH", 0, 0xFFFF, 0, 0x14C, 0, len(data), hint, how << 2) + data


def archive(bodies):
    out = b"!<arch>\n"
    for k, body in enumerate(bodies):
        head = f"M{k}/".ljust(16) + "0".ljust(12) + "".ljust(6) + "".ljust(6) + "0".ljust(8) + str(len(body)).ljust(10) + "`\n"
        out += head.encode() + body + (b"\n" if len(body) & 1 else b"")
    return out


class LocalCallTests(unittest.TestCase):
    def test_same_shape_calls_different_local_functions(self):
        # Both relative calls have the same displacement at different image addresses.
        code = b"\xe8" + struct.pack("<i", -5) + b"\xe8" + struct.pack("<i", -5)
        calls = [(1, 7, 2)]
        self.assertEqual(match_local_calls(code, 0x1000, [0, 5], calls, {7: 0x1003}), [5])
        self.assertEqual(match_local_calls(code, 0x1000, [0, 5], calls, {7: 0x2000}), [])
        self.assertEqual(match_local_calls(code, 0x1000, [0, 5], calls, {8: 0x1003}), [0, 5])

    def test_multiple_calls_to_same_anchor_stay_ambiguous(self):
        code = b"\xe8" + struct.pack("<i", 11) + b"\xe8" + struct.pack("<i", 6)
        self.assertEqual(match_local_calls(code, 0x2000, [0, 5], [(1, 1, 0)], {1: 0x2010}),
                         [0, 5])


class ImportSlotTests(unittest.TestCase):
    def test_same_instructions_with_different_imports(self):
        obj = SimpleNamespace(by_index={1: SimpleNamespace(name="__imp__DeleteFileA@4")})
        data = b"\x90" * 4 + b"\xff\x15" + struct.pack("<I", 4)
        reloc = SimpleNamespace(offset=6, symbol=1, type=REL_DIR32)
        section = SimpleNamespace(data=data, relocs=[reloc])
        fields = import_fields(obj, section, 4, 10)
        self.assertEqual(fields, [(2, "DeleteFileA", 4)])

        code = b"\xff\x15" + struct.pack("<I", 0x1004) + b"\xff\x15" + struct.pack("<I", 0x2004)
        slots = {0x1000: "RemoveDirectoryA", 0x2000: "DeleteFileA"}
        self.assertFalse(imports_agree(code, 0, fields, slots))
        self.assertTrue(imports_agree(code, 6, fields, slots))
        reloc.type = REL_REL32
        self.assertEqual(import_fields(obj, section, 4, 10), [])


class ShortImportTests(unittest.TestCase):
    def test_import_names_follow_the_name_type(self):
        with tempfile.TemporaryDirectory() as tmp:
            Path(tmp, "TEST.LIB").write_bytes(archive([
                short("_htonl@4", "WSOCK32.dll", 8, 3),
                short("_BinkOpen@8", "binkw32.dll", 0, 1),
                short("_GetACP@0", "KERNEL32.dll", 0, 2),
                short("_InitCommonControls@0", "COMCTL32.dll", 17, 0),
            ]))
            index = short_imports(Path(tmp))
        self.assertEqual(index[("wsock32.dll", "htonl")][0], "_htonl@4")
        self.assertEqual(index[("binkw32.dll", "_BinkOpen@8")][0], "_BinkOpen@8")
        self.assertEqual(index[("kernel32.dll", "GetACP@0")][0], "_GetACP@0")
        self.assertEqual(index[("comctl32.dll", 17)], ("_InitCommonControls@0", "TEST.LIB", "M3"))

    def test_unreadable_library_is_skipped(self):
        with tempfile.TemporaryDirectory() as tmp:
            Path(tmp, "BAD.LIB").write_bytes(b"not an archive")
            self.assertEqual(short_imports(Path(tmp)), {})


if __name__ == "__main__":
    unittest.main()
