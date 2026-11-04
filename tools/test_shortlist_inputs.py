import hashlib
import json
from pathlib import Path
import tempfile
import unittest

from coord.shortlist_inputs import banked_paths, finished_addresses


class ShortlistTests(unittest.TestCase):
    def test_only_matches_finish_and_only_intact_replays_are_shown(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            progress = root / "progress.csv"
            progress.write_text("address,result\n0x10,MATCH\n0x20,99.5%\n0x30,WRONG TARGETS\n")
            self.assertEqual(finished_addresses(progress), {16})
            loose = root / "build/batch/one/0X30.cpp"
            loose.parent.mkdir(parents=True)
            loose.touch()
            raw = json.dumps(dict(address="0x00000020", version=2, expected={"result": "partial"})).encode()
            (root / "trial.json").write_bytes(raw)
            link = ["trial.json", hashlib.sha256(raw).hexdigest()]
            rows = [dict(address="0x20", replays=json.dumps({"good": link, "bad": ["trial.json", "wrong"],
                                                               "outside": ["../trial.json", link[1]]})),
                    dict(address="0x30", replays=json.dumps({"wrong_address": link})),
                    dict(address="0x30")]
            self.assertEqual(banked_paths(rows, root), {32: ["trial.json"], 48: ["build/batch/one/0X30.cpp"]})
            (root / "trial.json").write_bytes(raw + b" ")
            self.assertNotIn(32, banked_paths(rows, root))
