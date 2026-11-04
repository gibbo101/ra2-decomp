"""Save or reproduce one C++ matching attempt using the normal checker.

    uv run tools/attempt.py save build/spike/0x005B20F0/missile.cpp 0x005B20F0 missile.json
    uv run tools/attempt.py replay missile.json

--module selects the tracked source to diff against (default src/<filename>).
--overlay saves the header overrides used by the candidate. The JSON contains source
and hashes only. Replay requires the recorded commit, toolchain and retail executable.
"""

import argparse
import csv
import hashlib
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

from check import ROOT, FLAGS
from checkall import LINE, SUMMARY

HEADERS = {".h", ".hpp", ".hh", ".inl"}


def git(*args, cwd=ROOT):
    return subprocess.check_output(["git", *args], cwd=cwd)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def header_digest(directory):
    entries = ((p.relative_to(directory).as_posix(), digest(p.read_bytes())) for p in sorted(directory.rglob("*"))
               if p.is_file() and p.suffix.lower() in HEADERS)
    return digest(json.dumps(list(entries)).encode())


def inputs():
    if git("diff", "HEAD", "--name-only", "--", "data", "tools/check.py", "tools/coff.py", "tools/rtti.py",
           "tools/wcl", "tools/wineenv.sh"):
        raise ValueError("recorded checker inputs must match the base commit")
    files = ["orig/gamemd.exe", "data/symbols.csv", *("toolchain/vc6sp3/BIN/" + name for name in
                                  ("CL.EXE", "C1XX.DLL", "C2.DLL"))]
    return {"commit": git("rev-parse", "HEAD").decode().strip(), "flags": FLAGS,
            "files": {name: digest((ROOT / name).read_bytes()) for name in files},
            "headers": {name: header_digest(ROOT / name) for name in
                        ("include", "reference/tibsun/code", "toolchain/vc6sp3/INCLUDE",
                         "toolchain/vc6sp3/MFC/INCLUDE", "toolchain/vc6sp3/ATL/INCLUDE")}}


def source_patch(original, candidate):
    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp)
        for name, data in (("a", original), ("b", candidate)):
            (root / name).mkdir()
            (root / name / "candidate.cpp").write_bytes(data)
        diff = subprocess.run(["git", "diff", "--no-index", "--no-prefix", "--no-ext-diff",
                               "--no-textconv", "--", "a/candidate.cpp", "b/candidate.cpp"],
                              cwd=root, capture_output=True)
        if diff.returncode not in (0, 1):
            raise ValueError(diff.stderr.decode(errors="replace"))
        return diff.stdout.decode("latin-1")


def verdict(src, address, overlay, module=None):
    cmd = [sys.executable, str(ROOT / "tools/check.py"), str(src), address, "--score", "--summary"]
    if module is not None:
        cmd.remove(address)
    if overlay is not None:
        cmd += ["--overlay", str(overlay)]
    run = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    lines = run.stdout.splitlines()
    rows = [m for line in lines if (m := LINE.match(line))]
    if module is not None:
        summary = SUMMARY.fullmatch(lines[-1]) if lines else None
        targets = [r for r in rows if r[1] == address]
        if (not summary or int(summary[1]) != len(rows) or len(targets) != 1
                or run.returncode != int(int(summary[2]) > 0)):
            raise ValueError("incomplete module verification:\n" + run.stdout + run.stderr)
        pos = next(i for i, line in enumerate(lines) if line == targets[0].string)
        score = lines[pos + 1]
        if not score.startswith("normalised score:"):
            raise ValueError("missing candidate score")
        matches = sorted(r[1] for r in rows if r[3] == "MATCH")
        with (ROOT / "data/progress.csv").open() as f:
            prior = {r["address"] for r in csv.DictReader(f) if r["source"] == module and r["result"] == "MATCH"}
        return {"result": targets[0][0], "score": score, "matched_annotations": matches,
                "regressed": sorted(prior - set(matches)), "module_failures": int(summary[2]),
                "output_sha256": digest(run.stdout.encode())}
    if (len(rows) != 1 or rows[0][1] != address or not lines or not SUMMARY.fullmatch(lines[-1])
            or lines[-1] != f"CHECK-SUMMARY: checked=1 failed={run.returncode}"
            or run.returncode not in (0, 1) or (run.returncode == 0) != (rows[0][3] == "MATCH")):
        raise ValueError("incomplete or inconsistent checker result:\n" + run.stdout + run.stderr)
    return {"result": rows[0][0], "score": next(line for line in lines if line.startswith("normalised score:")),
            "output_sha256": digest(run.stdout.encode())}


def module_bytes(module, commit):
    path = Path(module)
    if path.is_absolute() or ".." in path.parts or path.parts[:1] != ("src",) or path.suffix != ".cpp":
        raise ValueError("module must be a C++ source under src/")
    if not git("ls-tree", "--name-only", commit, "--", path.as_posix()):
        return b""
    return git("show", f"{commit}:{path.as_posix()}")


def reproduce(data):
    if data["version"] not in (1, 2) or data["inputs"] != inputs():
        raise ValueError("base commit, headers, symbols, compiler, flags or executable differ from the recorded inputs")
    address = data["address"]
    if not re.fullmatch(r"0x[0-9A-F]{8}", address):
        raise ValueError("invalid target address")
    original = module_bytes(data["module"], data["inputs"]["commit"])
    # Stable compiler-visible path. Concurrent replays cannot write each other's inputs.
    root = ROOT / "build/attempt-replay"
    root.mkdir(parents=True, exist_ok=True)
    work = root / digest(json.dumps(data, sort_keys=True).encode())[:16]
    work.mkdir()
    try:
        src = work / "candidate.cpp"
        src.write_bytes(original)
        if data["patch"]:
            subprocess.run(["git", "apply", "--no-index", "--include=candidate.cpp", "-p1", "-"],
                           cwd=work, input=data["patch"].encode("latin-1"), check=True,
                           env={**os.environ, "GIT_CEILING_DIRECTORIES": str(root)})
        if src.is_symlink() or digest(src.read_bytes()) != data["source_sha256"]:
            raise ValueError("restored source differs from the recorded candidate")
        src = src.rename(work / Path(data["module"]).name)
        overlay = work / "include"
        for name, content in data["overlay"].items():
            path = Path(name)
            if (path.is_absolute() or ".." in path.parts or "\\" in name or ":" in name
                    or path.suffix.lower() not in HEADERS):
                raise ValueError("invalid overlay path")
            dest = overlay / path
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(content.encode("latin-1"))
        actual = verdict(src, address, overlay if data["overlay"] else None,
                         data["module"] if data["version"] >= 2 else None)
        if actual != data["expected"]:
            raise ValueError(f"candidate did not replay: expected {data['expected']}, got {actual}")
        return actual
    finally:
        # Remove only the directory created by this invocation.
        for p in sorted(work.rglob("*"), reverse=True):
            if p.is_dir():
                p.rmdir()
            else:
                p.unlink()
        work.rmdir()


def save(args):
    src = args.source.resolve()
    if args.bundle.exists() or src.suffix != ".cpp":
        raise ValueError("save requires a C++ candidate and a new output file")
    module = args.module or "src/" + src.name
    pinned = inputs()
    candidate = src.read_bytes()
    original = module_bytes(module, pinned["commit"])
    overlay = {}
    if args.overlay is not None:
        if not args.overlay.is_dir():
            raise ValueError("overlay directory does not exist")
        overlay = {p.relative_to(args.overlay).as_posix(): p.read_bytes().decode("latin-1")
                   for p in sorted(args.overlay.rglob("*")) if p.is_file()
                   and p.suffix.lower() in HEADERS}
    address = f"0x{args.address:08X}"
    data = {"version": 2, "address": address, "module": module, "inputs": pinned,
            "source_sha256": digest(candidate), "patch": source_patch(original, candidate),
            "overlay": overlay, "expected": verdict(src, address, args.overlay, module)}
    reproduce(data)
    with tempfile.NamedTemporaryFile(dir=args.bundle.parent, delete=False) as tmp:
        try:
            tmp.write((json.dumps(data, indent=2) + "\n").encode())
            tmp.flush()
            os.link(tmp.name, args.bundle)
        finally:
            os.unlink(tmp.name)
    return data["expected"]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    saving = commands.add_parser("save")
    saving.add_argument("source", type=Path)
    saving.add_argument("address", type=lambda s: int(s, 16))
    saving.add_argument("bundle", type=Path)
    saving.add_argument("--module")
    saving.add_argument("--overlay", type=Path)
    commands.add_parser("replay").add_argument("bundle", type=Path)
    args = parser.parse_args()
    try:
        result = save(args) if args.command == "save" else reproduce(json.loads(args.bundle.read_text()))
    except (ValueError, KeyError, OSError, subprocess.CalledProcessError) as exc:
        parser.exit(1, str(exc) + "\n")
    print(result["result"])
    print(result["score"])


if __name__ == "__main__":
    main()
