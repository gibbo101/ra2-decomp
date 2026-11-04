"""Read a module object from a cold gate's object directory or hashed build receipt."""
import hashlib
import json
from pathlib import Path


class SavedObjects:
    def __init__(self, directory):
        self.directory = directory
        receipt = directory / "receipt.json"
        self.hashes = None
        if receipt.is_file():
            manifest = json.loads(receipt.read_text())
            if manifest.get("version") != 1 or not isinstance(manifest.get("objects"), dict):
                raise ValueError(f"{receipt}: invalid build receipt")
            self.hashes = manifest["objects"]

    def read(self, source):
        path = Path(source)
        if path.is_absolute() or ".." in path.parts or path.parts[:1] != ("src",):
            raise ValueError(f"expected a source path relative to the checkout: {source}")
        if self.hashes is None:
            return (self.directory / path.with_suffix(".obj")).read_bytes()
        key = path.as_posix() + ".obj"
        raw = (self.directory / key).read_bytes()
        if self.hashes.get(key) != hashlib.sha256(raw).hexdigest():
            raise ValueError(f"{key}: object differs from the build receipt")
        return raw
