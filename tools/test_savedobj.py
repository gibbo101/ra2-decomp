import hashlib
import json
from pathlib import Path
import tempfile
import unittest

from savedobj import SavedObjects


class SavedObjectTests(unittest.TestCase):
    def test_legacy_object_and_receipt_integrity(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "src").mkdir()
            (root / "src/unit.obj").write_bytes(b"legacy")
            self.assertEqual(SavedObjects(root).read("src/unit.asm"), b"legacy")
            (root / "src/unit.asm.obj").write_bytes(b"verified")
            hashes = {"src/unit.asm.obj": hashlib.sha256(b"verified").hexdigest()}
            (root / "receipt.json").write_text(json.dumps(dict(version=1, objects=hashes)))
            objects = SavedObjects(root)
            self.assertEqual(objects.read("src/unit.asm"), b"verified")
            (root / "src/unit.asm.obj").write_bytes(b"changed")
            with self.assertRaises(ValueError):
                objects.read("src/unit.asm")
