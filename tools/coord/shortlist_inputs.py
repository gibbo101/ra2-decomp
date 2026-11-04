"""Inputs shared by retry selectors. Paths are relative to the repository root."""
from collections import defaultdict
import csv
import hashlib
import json
from pathlib import Path


def finished_addresses(path):
    with open(path, newline="") as f:
        return {int(r["address"], 16) for r in csv.DictReader(f) if r["result"] == "MATCH"}


def banked_paths(attempts, root=Path(".")):
    banks = defaultdict(set)
    for path in (root / "build/batch").glob("*/*.cpp"):
        if path.stem.lower().startswith("0x"):
            try:
                banks[int(path.stem, 16)].add(path.relative_to(root).as_posix())
            except ValueError:
                pass
    for row in attempts:
        addr = int(row["address"], 16)
        try:
            links = json.loads(row.get("replays") or "{}")
        except ValueError:
            continue
        if not isinstance(links, dict):
            continue
        for link in links.values():
            if not isinstance(link, list) or len(link) != 2 or not all(isinstance(x, str) for x in link):
                continue
            name, checksum = link
            path = Path(name)
            if path.is_absolute() or ".." in path.parts:
                continue
            try:
                full = root / path
                if not full.resolve().is_relative_to(root.resolve()):
                    continue
                raw = full.read_bytes()
                if hashlib.sha256(raw).hexdigest() != checksum:
                    continue
                bundle = json.loads(raw)
                if (isinstance(bundle, dict) and bundle.get("address") == f"0x{addr:08X}"
                        and bundle.get("version") in (1, 2) and bundle.get("expected")):
                    banks[addr].add(path.as_posix())
            except (OSError, ValueError, RuntimeError):
                continue
    return {addr: sorted(paths) for addr, paths in banks.items()}
