"""Capture a successful cold gate, then verify its objects with the current address tables.

    uv run tools/build_receipt.py capture build/gates/batch
    uv run tools/build_receipt.py recheck build/gates/batch --record
    uv run tools/build_receipt.py recheck build/gates/batch

Use capture NEW --reuse OLD after edits. Every module is verified. An unchanged
source can reuse its verified object if all shared inputs still agree. Symbol and progress tables are
read again on every recheck. Receipts contain local objects and belong under build/.
"""

import argparse
import contextlib
import hashlib
import io
import json
import os
import re
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
    shared = hashlib.sha256()
    units = {}
    independent = True
    for name in sorted(files):
        data = (root / name).read_bytes()
        content = hashlib.sha256(data).digest()
        entry = name.encode() + b"\0" + content
        h.update(entry)
        path = Path(name)
        if name.startswith("src/") and path.suffix in (".cpp", ".asm"):
            units[name + ".obj"] = content.hex()
        else:
            shared.update(entry)
        if b"#" in data and b"\0" not in data:
            # A compiled source may itself be included. Fall back to a whole-tree
            # key for source or computed includes rather than guessing dependencies.
            logical = data.replace(b"\\\r\n", b"").replace(b"\\\n", b"")
            for directive in re.findall(rb"(?m)^\s*#\s*include\b([^\n]*)", logical):
                literal = re.match(rb'\s*[<"]([^>"\n]+)[>"]', directive)
                if not literal or literal[1].lower().endswith((b".cpp", b".asm")):
                    independent = False
    env = {key: os.environ.get(key) for key in ("CL", "_CL_", "ML", "_ML_")}
    return {"units": units, "shared": shared.hexdigest() if independent and not any(env.values()) else h.hexdigest(),
            "tree": h.hexdigest(), "cpp": check.FLAGS, "asm": check.ASM_FLAGS, "environment": env}


def object_name(src):
    return src.relative_to(check.ROOT).as_posix() + ".obj"


def can_reuse(previous, current, name):
    return ("units" in previous and previous["units"].get(name) == current["units"][name]
            and all(previous.get(key) == current[key] for key in ("shared", "cpp", "asm", "environment")))


def gate(directory, capture, manifest, reuse=None):
    original, names = check.Original(), check.known_names()

    def run(src):
        name = object_name(src)
        path = directory / name
        if reuse and can_reuse(reuse[1]["inputs"], manifest["inputs"], name):
            old = (reuse[0] / name).read_bytes()
            if digest(old) != reuse[1]["objects"].get(name):
                raise ValueError(f"{name}: cached object changed")
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(old)
        elif capture:
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
        current = inputs(check.ROOT)
        if "units" not in manifest["inputs"]:  # Receipts from before per-source keys.
            current.pop("units")
            current.pop("shared")
        if current != manifest["inputs"]:
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
    parser.add_argument("--reuse", type=Path, metavar="OLD", help="reuse eligible objects in a new capture")
    args, gate_args = parser.parse_known_args()
    sys.argv = ["checkall.py", *gate_args]
    dest = args.directory.resolve()
    if (check.ROOT / "build") not in dest.parents:
        parser.error("choose a directory under build/")
    try:
        reuse = None
        if args.reuse:
            old = args.reuse.resolve()
            if args.mode != "capture" or (check.ROOT / "build") not in old.parents:
                parser.error("--reuse requires capture and a receipt below build/")
            prior = json.loads((old / "receipt.json").read_text())
            if prior["version"] != 1:
                raise ValueError("unsupported receipt version")
            reuse = old, prior
        if args.mode == "capture":
            dest.parent.mkdir(parents=True, exist_ok=True)
            if dest.exists():
                parser.error("capture requires a new directory")
            manifest = {"version": 1, "inputs": inputs(check.ROOT), "objects": {}}
            with tempfile.TemporaryDirectory(dir=dest.parent) as tmp:
                staging = Path(tmp) / "objects"
                staging.mkdir()
                gate(staging, True, manifest, reuse)
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
