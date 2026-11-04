import json
import subprocess
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

import attempt


class ReplayTests(unittest.TestCase):
    def test_replay_patch_overlay_and_verdict_without_overwriting_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            subprocess.run(["git", "init", "-q", tmp], check=True)
            (root / "src").mkdir()
            tracked = root / "src/unit.cpp"
            tracked.write_bytes(b"int unit() { return 1; }\n")
            subprocess.run(["git", "add", "."], cwd=root, check=True)
            subprocess.run(["git", "-c", "user.email=1+test@users.noreply.github.com", "-c", "user.name=test",
                            "commit", "-qm", "base"], cwd=root, check=True)
            commit = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
            src = root / "unit.cpp"
            src.write_bytes(b"int unit() { return 3; }")  # no trailing newline
            inc = root / "overlay"
            (inc / "sub").mkdir(parents=True)
            (inc / "sub/layout.h").write_bytes(b"// \xff\n")
            args = SimpleNamespace(source=src, address=0x401000, bundle=root / "attempt.json", overlay=inc, module=None)

            def verify(source, address, overlay, module=None):
                self.assertEqual(source.read_bytes(), src.read_bytes())
                self.assertEqual((overlay / "sub/layout.h").read_bytes(), b"// \xff\n")
                return {"result": "partial", "score": "99.0%"}

            git = attempt.git
            with patch.object(attempt, "ROOT", root), patch.object(attempt, "git", lambda *a: git(*a, cwd=root)), \
                    patch.object(attempt, "inputs", return_value={"commit": commit}), patch.object(attempt, "verdict", verify):
                attempt.save(args)
                saved = json.loads(args.bundle.read_text())
                self.assertEqual(attempt.reproduce(saved), saved["expected"])
                with self.assertRaisesRegex(ValueError, "new output"):
                    attempt.save(args)
                with patch.object(attempt, "verdict", return_value={}):
                    with self.assertRaisesRegex(ValueError, "did not replay"):
                        attempt.reproduce(saved)
                saved["overlay"]["../outside.h"] = ""
                with self.assertRaisesRegex(ValueError, "overlay path"):
                    attempt.reproduce(saved)
            self.assertEqual(tracked.read_bytes(), b"int unit() { return 1; }\n")
            self.assertEqual(list((root / "build/attempt-replay").iterdir()), [])


if __name__ == "__main__":
    unittest.main()
