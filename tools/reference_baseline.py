"""Store comparable, proven relocation sites from a successful full gate."""
import hashlib
import json
import os
from pathlib import Path
import tempfile


class ReferenceBaseline:
    def __init__(self, root, record):
        self.output = os.environ.get("RA2_SAVE_REFERENCES")
        baseline = os.environ.get("RA2_REQUIRE_REFERENCES")
        self.enabled = bool(self.output or baseline)
        self.sites = set()
        self.expected = set()
        if not self.enabled:
            return
        self.exe = hashlib.sha256((root / "orig/gamemd.exe").read_bytes()).hexdigest()
        if baseline:
            data = json.loads(Path(baseline).read_text())
            if data["version"] != 1 or data["exe"] != self.exe or not isinstance(data["sites"], list):
                raise SystemExit("incompatible reference baseline")
            self.expected = set(data["sites"])
        if self.output and (record or Path(self.output).exists()):
            raise SystemExit("capture a reference baseline in a new file after recording progress")

    def save(self):
        if not self.output:
            return
        path = Path(self.output)
        with tempfile.NamedTemporaryFile(dir=path.parent, mode="w", delete=False) as tmp:
            try:
                json.dump(dict(version=1, exe=self.exe, sites=sorted(self.sites)), tmp)
                tmp.flush()
                os.link(tmp.name, path)
            finally:
                os.unlink(tmp.name)
