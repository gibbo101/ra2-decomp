from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

from coord import land


class LandingTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.data = self.root / "data"
        self.data.mkdir()
        for name in ["attempts.csv", "modules.csv", "manifest.csv"]:
            (self.data / name).write_text("previous")
        (self.data / "progress.csv").write_text("address,name,result,size,source,symbol\n0x00401000,F,MATCH,1,src/f.cpp,_F\n")
        self.gate = self.root / "gate"
        self.gate.write_text("0x00401000 F: MATCH (1/1 bytes) _F\n1 of 1 functions match (1 of 1 bytes)\nLANDING-GATE valid\n")
        self.journal = self.root / "journal"
        self.journal.touch()

    def test_requires_complete_gate_for_current_inputs(self):
        with patch.object(land, "fingerprint", return_value="valid"):
            self.assertEqual(land.check_log(self.root, self.gate), "LANDING-GATE valid")
            self.gate.write_text("LANDING-GATE valid\n")
            with self.assertRaisesRegex(ValueError, "cover"):
                land.check_log(self.root, self.gate)
            self.gate.write_text("")
            with self.assertRaisesRegex(ValueError, "completed"):
                land.check_log(self.root, self.gate)

    def test_generator_failure_never_publishes_or_cleans(self):
        with patch.object(land, "ROOT", self.root), patch.object(land, "fingerprint", return_value="valid"), \
                patch.object(land.sys, "argv", ["land.py", "date", str(self.gate), str(self.journal)]), \
                patch.object(land.subprocess, "run") as run:
            def command(args, **kwargs):
                if str(args[1]).endswith("manifest.py"):
                    raise subprocess.CalledProcessError(1, args)
            run.side_effect = command
            with self.assertRaises(subprocess.CalledProcessError):
                land.main()
            self.assertFalse(any("stale.py" in str(call.args) for call in run.call_args_list))
            for name in ("attempts.csv", "modules.csv", "manifest.csv"):
                self.assertEqual((self.data / name).read_text(), "previous")
