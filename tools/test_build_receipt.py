from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import build_receipt as receipt


class ReceiptTests(unittest.TestCase):
    def test_inputs_include_headers_but_not_progress(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for name in ("src", "include", "reference/tibsun/code", "toolchain/vc6sp3", "tools", "data"):
                (root / name).mkdir(parents=True)
            for name in ("tools/check.py", "tools/wcl", "tools/wml", "tools/wineenv.sh"):
                (root / name).write_bytes(b"")
            first = receipt.inputs(root)
            (root / "data/progress.csv").write_text("changed")
            self.assertEqual(receipt.inputs(root), first)
            (root / "include/types.h").write_text("changed")
            self.assertNotEqual(receipt.inputs(root), first)

    def test_recheck_rejects_changed_object_before_scoring(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            obj = root / "src/unit.cpp.obj"
            obj.parent.mkdir()
            obj.write_bytes(b"stale")
            with patch.object(receipt.check, "ROOT", root), patch.object(receipt.check, "Original"), \
                    patch.object(receipt.check, "known_names"), patch.object(receipt, "inputs", return_value={}), \
                    patch.object(receipt.checkall, "main", side_effect=lambda run, validate: run(root / "src/unit.cpp")), \
                    patch.object(receipt.check, "report") as report:
                with self.assertRaisesRegex(ValueError, "object missing from receipt or changed"):
                    receipt.gate(root, False, {"inputs": {}, "objects": {"src/unit.cpp.obj": "old"}})
                report.assert_not_called()
