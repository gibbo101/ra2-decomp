"""Capture a successful cold gate, then verify its objects with the current address tables.

    uv run tools/build_receipt.py capture build/gates/batch
    uv run tools/build_receipt.py recheck build/gates/batch --record
    uv run tools/build_receipt.py recheck build/gates/batch

Receipts cover the whole checkout's compilation inputs. Changed sources, headers,
toolchain files or flags require a fresh capture. Symbol and progress tables are
read again on every recheck. Receipts contain local objects and belong under build/.
"""

import argparse
import contextlib
import hashlib
import io
import json
import os
import subprocess
import sys
import tempfile
from pathlib import Path

import check
import checkall


def digest(data):
    return hashlib.sha256(data).hexdigest()


def inputs(root):
    dirs = ["src", "include", "reference/tibsun/code", "toolchain/vc6sp3"]
    files = ["tools/check.py", "tools/wcl", "tools/wml", "tools/wineenv.sh"]
    for directory in dirs:
        base = root / directory
        if not base.is_dir():
            raise ValueError(f"missing compilation input: {directory}")
        files.extend(p.relative_to(root).as_posix() for p in base.rglob("*") if p.is_file())
    h = hashlib.sha256()
    for name in sorted(files):
        h.update(name.encode() + b"\0" + hashlib.sha256((root / name).read_bytes()).digest())
    env = {key: os.environ.get(key) for key in ("CL", "_CL_", "ML", "_ML_")}
    return {"tree": h.hexdigest(), "cpp": check.FLAGS, "asm": check.ASM_FLAGS, "environment": env}


def object_name(src):
    return src.relative_to(check.ROOT).as_posix() + ".obj"


def gate(directory, capture, manifest):
    original, names = check.Original(), check.known_names()

    def run(src):
        name = object_name(src)
        path = directory / name
        if capture:
            path, log = check.compile_source(src, out=path)
            if path is None:
                return subprocess.CompletedProcess([], 1, "", log)
        raw = path.read_bytes()
        actual = digest(raw)
        if capture:
            manifest["objects"][name] = actual
        elif manifest["objects"].get(name) != actual:
            raise ValueError(f"{name}: saved object missing from receipt or changed")
        text = io.StringIO()
        with contextlib.redirect_stdout(text):
            code = check.report(src, raw, original, names, check.annotations(src), summary=True)
        return subprocess.CompletedProcess([], code, text.getvalue(), "")

    def validate():
        if inputs(check.ROOT) != manifest["inputs"]:
            raise ValueError("compilation inputs changed; run a fresh capture")

    if not capture:
        validate()
    try:
        checkall.main(run, validate)
    except SystemExit as exc:
        if exc.code:
            raise


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=["capture", "recheck"])
    parser.add_argument("directory", type=Path)
    args, gate_args = parser.parse_known_args()
    sys.argv = ["checkall.py", *gate_args]
    dest = args.directory.resolve()
    if (check.ROOT / "build") not in dest.parents:
        parser.error("choose a directory under build/")
    try:
        if args.mode == "capture":
            dest.parent.mkdir(parents=True, exist_ok=True)
            if dest.exists():
                parser.error("capture requires a new directory")
            manifest = {"version": 1, "inputs": inputs(check.ROOT), "objects": {}}
            with tempfile.TemporaryDirectory(dir=dest.parent) as tmp:
                staging = Path(tmp) / "objects"
                staging.mkdir()
                gate(staging, True, manifest)
                (staging / "receipt.json").write_text(json.dumps(manifest, indent=2) + "\n")
                staging.rename(dest)
        else:
            manifest = json.loads((dest / "receipt.json").read_text())
            if manifest["version"] != 1:
                parser.error("unsupported receipt version")
            gate(dest, False, manifest)
    except (KeyError, OSError, ValueError) as exc:
        parser.exit(1, str(exc) + "\n")


if __name__ == "__main__":
    main()
