import tempfile
import unittest
from pathlib import Path
from unittest import mock

import exebuild

TABLE = """address,build,original,patched,note
0x00401002,steam,5153,b0c3,Steam: test
0x00401002,tfd,515356,909090,TFD: test
0x00402000,steam,aa,bb,Steam: outside the image
"""


class ExeBuildTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        root = Path(self.tmp.name)
        (root / "data").mkdir()
        (root / "data/exe_patches.csv").write_text(TABLE)
        patcher = mock.patch.object(exebuild, "ROOT", root)
        patcher.start()
        self.addCleanup(patcher.stop)
        self.addCleanup(self.tmp.cleanup)

    def test_restores_only_the_named_build(self):
        image = bytearray(bytes.fromhex("0000b0c30000"))
        exebuild.restore(image, 0x00401000, "steam")
        self.assertEqual(image.hex(), "000051530000")

    def test_other_build_rows_are_ignored(self):
        image = bytearray(bytes.fromhex("0000909090"))
        exebuild.restore(image, 0x00401000, "tfd")
        self.assertEqual(image.hex(), "0000515356")

    def test_retail_is_left_alone(self):
        image = bytearray(bytes.fromhex("0000515356"))
        exebuild.restore(image, 0x00401000, "retail")
        self.assertEqual(image.hex(), "0000515356")

    def test_unexpected_bytes_stop(self):
        image = bytearray(bytes.fromhex("0000515356"))
        with self.assertRaises(SystemExit):
            exebuild.restore(image, 0x00401000, "steam")

    def test_unknown_exe_names_the_accepted_builds(self):
        with self.assertRaises(SystemExit) as cm:
            exebuild.build_of(b"not an exe")
        self.assertIn("The First Decade", str(cm.exception))


if __name__ == "__main__":
    unittest.main()
