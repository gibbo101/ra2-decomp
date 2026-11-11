# SPDX-License-Identifier: MIT
# Copyright 2026 ra2-decomp contributors
"""Derive assembly data bindings from independently matched existing C++ bodies.

This does not use assembly annotations or proposed assembly procedure addresses.
Every anchor is freshly compiled with check.py's flags and compared in full.
"""

import csv
import hashlib
import json
import re
import struct
from collections import defaultdict
from pathlib import Path

import check
import coff
import libmatch

ROOT = Path(__file__).resolve().parent.parent
FIELDS = {"address", "size", "source", "symbol", "source_sha256", "globals"}


def require(condition, message):
    if not condition:
        raise ValueError(message)


def verified_data_symbols(exe: Path, work: Path,
                          manifest: Path = ROOT / "data/assembly_anchors.csv") -> dict[str, int]:
    if not manifest.exists():
        return {}
    with manifest.open(newline="") as stream:
        reader = csv.DictReader(stream)
        require(set(reader.fieldnames or ()) == FIELDS
                and len(reader.fieldnames or ()) == len(FIELDS), "Invalid assembly-anchor columns")
        rows = list(reader)
    require(rows, "Empty assembly-anchor manifest")
    keys = [(r["address"], r["source"], r["symbol"]) for r in rows]
    require(len(set(keys)) == len(rows) == len({r["address"] for r in rows}),
            "Duplicate assembly-anchor record or address")
    globals_ = {name for row in rows for name in row["globals"].split(";")}
    require("" not in globals_, "Empty assembly-anchor data name")
    with (ROOT / "data/progress.csv").open() as stream:
        progress = list(csv.DictReader(stream))
    original = check.Original(exe)
    text_section = original.pe.sections[0]
    base = original.base + text_section.VirtualAddress
    image = original.read(base, text_section.Misc_VirtualSize)
    ranges = [(original.base + sec.VirtualAddress,
               original.base + sec.VirtualAddress + sec.Misc_VirtualSize)
              for sec in original.pe.sections
              if not sec.Characteristics & 0x20000020 and sec.Characteristics & 0xC0]
    # These names are the conclusion, so remove them from the initial evidence.
    local = {name: address for name, address in check.known_decorated().items()
             if name not in globals_}
    qualified = {check.qualified(name) for name in globals_}
    names = {name: address for name, address in check.known_names().items()
             if name not in qualified}
    objects, selected, observations = {}, [], defaultdict(dict)
    for row in rows:
        address, size = int(row["address"], 16), int(row["size"])
        require(row["address"] == f"0x{address:08X}" and size >= libmatch.MIN_SIZE,
                "Invalid or insufficient assembly-anchor extent")
        records = [r for r in progress if r["address"] == row["address"]
                   and r["source"] == row["source"] and r["symbol"] == row["symbol"]]
        require(len(records) == 1 and records[0]["result"] == "MATCH"
                and int(records[0]["size"]) == size == original.sizes.get(address),
                "Assembly anchor is not an existing complete source match")
        source = (ROOT / row["source"]).resolve()
        require(source.is_relative_to(ROOT / "src") and source.suffix == ".cpp"
                and hashlib.sha256(source.read_bytes()).hexdigest() == row["source_sha256"],
                "Assembly-anchor source or hash differs")
        require(address in dict(check.annotations(source)), "Assembly anchor lacks its source marker")
        if source not in objects:
            path, log = check.compile_source(source, out=work / (source.stem + ".anchor.obj"))
            require(path is not None, f"Cannot compile assembly anchor {source}: {log}")
            raw = path.read_bytes()
            objects[source] = raw, coff.CoffObject(raw)
        raw, obj = objects[source]
        picks = [p for p in obj.functions() if p[0].name == row["symbol"]]
        require(len(picks) == 1, "Assembly-anchor function symbol is ambiguous")
        pick = picks[0]
        data, masked = check.function_bytes(pick)
        require(len(data) == size, "Assembly-anchor compiled extent differs")
        pattern = re.compile(libmatch.pattern(data, masked), re.S)
        first = pattern.search(image)
        require(first is not None and base + first.start() == address
                and pattern.search(image, first.start() + 1) is None,
                "Assembly-anchor complete body is not uniquely placed")
        wanted = row["globals"].split(";")
        require(len(wanted) == len(set(wanted)), "Duplicate data names in an assembly anchor")
        found = defaultdict(set)
        _, sec, lo, hi = pick
        relocs = [r for r in sec.relocs if lo <= r.offset < hi]
        require(all(r.type in (coff.REL_DIR32, coff.REL_REL32)
                    and r.offset + 4 <= lo + len(data) for r in relocs),
                "Unsupported or truncated assembly-anchor relocation")
        for rel in relocs:
            symbol = obj.by_index[rel.symbol]
            if symbol.name not in wanted:
                continue
            data_definition = (symbol.section == 0 or 0 < symbol.section <= len(obj.sections)
                               and not obj.sections[symbol.section - 1].is_code)
            require(rel.type == coff.REL_DIR32 and data_definition
                    and symbol.storage == coff.STORAGE_EXTERNAL and symbol.type != 0x20,
                    "Assembly anchor must reference external data")
            off = rel.offset - lo
            addend = struct.unpack_from("<i", data, off)[0]
            target = (original.u32(address + off) - addend) & 0xFFFFFFFF
            require(any(a <= target < b for a, b in ranges),
                    "Assembly-anchor data target is outside non-code data")
            found[symbol.name].add(target)
        require(set(found) == set(wanted) and all(len(v) == 1 for v in found.values()),
                "Assembly-anchor data references are missing or inconsistent")
        for name, targets in found.items():
            observations[name][address] = next(iter(targets))
        selected.append((row, address, raw, obj, pick, len(relocs)))

    require(set(observations) == globals_, "Assembly-anchor globals are incomplete")
    require(all(len(by_anchor) >= 2 and len(set(by_anchor.values())) == 1
                for by_anchor in observations.values()),
            "Each assembly data name needs two agreeing independent function anchors")
    bindings = {name: next(iter(by_anchor.values())) for name, by_anchor in observations.items()}
    proof = []
    for row, address, raw, obj, pick, count in selected:
        complete_local = {**local, **bindings}
        result = check.compare(original, obj, raw, address, "=" + row["symbol"],
                               check.ReferenceNames(names, complete_local, check.function_symbols(obj)),
                               complete_local)
        require(result.matched and not result.unverified and not result.warnings
                and len(result.proven) == count,
                f"Assembly anchor fails the unchanged complete comparator: {row['address']}")
        proof.append({**row, "object_sha256": hashlib.sha256(raw).hexdigest(),
                      "unique_complete_body": True, "all_relocations_verified": True})
    (ROOT / "build/assembly_anchor_proof.json").write_text(json.dumps({
        "result": "PASS", "anchors": proof,
        "bindings": {name: f"0x{address:08X}" for name, address in bindings.items()},
    }, indent=2) + "\n")
    return bindings
