from pathlib import Path
import struct
import tempfile
import unittest

from libmatch import short_imports


def short(symbol, dll, hint, how):
    data = symbol.encode() + b"\0" + dll.encode() + b"\0"
    return struct.pack("<HHHHIIHH", 0, 0xFFFF, 0, 0x14C, 0, len(data), hint, how << 2) + data


def archive(bodies):
    out = b"!<arch>\n"
    for k, body in enumerate(bodies):
        head = f"M{k}/".ljust(16) + "0".ljust(12) + "".ljust(6) + "".ljust(6) + "0".ljust(8) + str(len(body)).ljust(10) + "`\n"
        out += head.encode() + body + (b"\n" if len(body) & 1 else b"")
    return out


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
