"""Identify the inputs of a completed local landing gate."""
import hashlib
import os
import subprocess


def fingerprint(root):
    h = hashlib.sha256(subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root))
    for key in ("CL", "_CL_", "ML", "_ML_"):
        h.update(key.encode() + b"\0" + os.environ.get(key, "").encode() + b"\0")
    paths = [root / n for n in ("orig/gamemd.exe", "data/functions.csv", "data/progress.csv", "data/names.csv",
                                 "data/symbols.csv", "data/library.csv", "data/exe_patches.csv",
                                 "tools/wcl", "tools/wml", "tools/wineenv.sh")]
    for name in ("src", "include", "reference/tibsun/code", "toolchain/vc6sp3"):
        paths.extend(p for p in (root / name).rglob("*") if p.is_file())
    paths.extend((root / "tools").glob("*.py"))
    for p in sorted(paths):
        h.update(p.relative_to(root).as_posix().encode() + b"\0" + hashlib.sha256(p.read_bytes()).digest())
    return h.hexdigest()
