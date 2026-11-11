#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# Copyright 2026 ra2-decomp contributors
"""Verify small library identities using relocations, RTTI and complete object blocks.

Usage: python tools/library_identified.py [IDENTITIES_JSON]
The linked archive bodies must agree at every byte, including relocations.
This audit also runs before libmatch.py writes the library map.
Named blocks in the manifest share evidence for functions from one archive member;
data-table evidence requires distinct named pointers and all table relocations.
"""

import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import check  # noqa: E402
import coff  # noqa: E402
import libmatch  # noqa: E402


def require(condition, message):
    if not condition:
        raise ValueError(message)


def identity_records(rows):
    records = {}
    for row in rows:
        address = int(row["address"], 16)
        require(row["address"] == f"0x{address:08X}" and
                type(row["size"]) is int and row["size"] > 0,
                f"Noncanonical address or invalid size: {row}")
        require(address not in records, f"Duplicate identity: {row['address']}")
        records[address] = row
    return records


class Audit:
    def __init__(self):
        self.original = check.Original()
        section = self.original.pe.sections[0]
        self.base = self.original.base + section.VirtualAddress
        self.text = self.original.read(self.base, section.SizeOfRawData)
        self.objects = {}
        self.anchors = {}
        self.names = check.known_names()
        self.local = check.known_decorated(self.original.decorated_addresses)

    def object(self, entry):
        key = entry["library"], entry["object"]
        if key not in self.objects:
            require(Path(key[0]).name == key[0] and "\\" not in key[0],
                    "Library must be a basename in the pinned library directory")
            archive = (libmatch.LIBDIR / key[0]).read_bytes()
            members = [(name, raw) for name, raw in libmatch.members(archive)
                       if name.split("\\")[-1] == key[1] and raw[:2] == b"\x4c\x01"]
            require(len(members) == 1, f"Archive member is ambiguous: {key}")
            name, raw = members[0]
            self.objects[key] = name, raw, coff.CoffObject(raw)
        member, raw, obj = self.objects[key]
        picks = [pick for pick in obj.functions() if pick[0].name == entry["symbol"]]
        require(len(picks) == 1, f"Object definition is ambiguous: {entry['symbol']}")
        return member, raw, obj, picks[0]

    def unique_anchor(self, entry):
        key = entry["library"], entry["object"], entry["address"], entry["symbol"]
        if key in self.anchors:
            return self.anchors[key]
        member, raw, obj, pick = self.object(entry)
        _, section, start, end = pick
        code, masked = check.function_bytes(pick)
        hits = [self.base + match.start() for match in
                re.finditer(b"(?=" + libmatch.pattern(code, masked) + b")", self.text, re.S)]
        address = int(entry["address"], 16)
        require(hits == [address], f"Anchor does not have one independent location: {entry}")
        references = []
        for relocation in section.relocs:
            if not start <= relocation.offset < end:
                continue
            offset = relocation.offset - start
            require(offset + 4 <= len(code), f"Truncated anchor relocation: {entry}")
            require(relocation.type in (coff.REL_DIR32, coff.REL_REL32),
                    f"Unsupported anchor relocation: {entry}")
            addend = struct.unpack_from("<i", code, offset)[0]
            target = self.original.u32(address + offset)
            if relocation.type == coff.REL_REL32:
                target = (target + address + offset + 4) & 0xFFFFFFFF
            references.append({"offset": offset,
                               "symbol": obj.by_index[relocation.symbol].name,
                               "target": f"0x{(target - addend) & 0xFFFFFFFF:08X}"})
        result = {"address": entry["address"], "size": len(code), "references": references}
        self.anchors[key] = result
        return result

    def linked_body(self, entry, bindings):
        """Resolve every archive relocation, then compare all linked bytes."""
        member, raw, obj, pick = self.object(entry)
        _, section, start, end = pick
        code, _ = check.function_bytes(pick)
        address = int(entry["address"], 16)
        local = {**self.local, **bindings}
        references = check.ref_checks(self.original, obj, raw, pick, code,
                                      self.original.read(address, len(code)), address,
                                      check.ReferenceNames(self.names, local,
                                                           check.function_symbols(obj)), local)
        relocations = [r for r in section.relocs if start <= r.offset < end]
        require(len(references) == len(relocations), f"Unchecked relocation: {entry}")
        linked = bytearray(code)
        for reference in references:
            require(reference.status == "ok" and reference.want is not None,
                    f"Unproved reference: {entry}: {reference}")
            value = reference.want + reference.addend
            if reference.type == coff.REL_REL32:
                value -= address + reference.offset + 4
            struct.pack_into("<I", linked, reference.offset, value & 0xFFFFFFFF)
        require(bytes(linked) == self.original.read(address, len(code)),
                f"Fully relocated archive body differs: {entry}")
        hits = [self.base + match.start() for match in
                re.finditer(b"(?=" + re.escape(bytes(linked)) + b")", self.text)]
        return {"size": len(code),
                "fully_linked_locations": [f"0x{hit:08X}" for hit in hits]}

    def object_block(self, entry):
        """Locate every code section of a library member, in its original order."""
        block = entry["block"]
        _, _, obj, _ = self.object(entry)
        picks = list(obj.functions())
        symbols = [pick[0].name for pick in picks]
        require(symbols == block["symbols"] and len(set(symbols)) == len(symbols),
                "Object block must cover every function, without aliases, in section order")
        require({p[1].number for p in picks} ==
                {s.number for s in obj.sections if s.is_code and s.data},
                "Object block omits a code section")
        address = int(block["address"], 16)
        combined, masked, addresses = bytearray(), set(), {}
        for pick in picks:
            symbol, section, start, end = pick
            code, mask = check.function_bytes(pick)
            require(start == 0 and end == len(section.data) == len(code) and code and
                    symbol.storage == coff.STORAGE_EXTERNAL and symbol.type == 0x20,
                    "Object-block function must occupy its entire code section")
            addresses[symbol.name] = address + len(combined)
            masked.update(len(combined) + offset for offset in mask)
            combined.extend(code)
        require(len(combined) == block["size"] and
                addresses[entry["symbol"]] == int(entry["address"], 16),
                "Object-block size or target position differs")
        hits = [self.base + m.start() for m in re.finditer(
            b"(?=" + libmatch.pattern(bytes(combined), masked) + b")", self.text, re.S)]
        require(hits == [address], "Complete object block is not independently unique")
        anchors = {}
        for anchor in block["anchors"]:
            require((anchor["library"], anchor["object"]) ==
                    (entry["library"], entry["object"]) and
                    addresses[anchor["symbol"]] == int(anchor["address"], 16),
                    "Wrong object-block anchor member or position")
            result = self.unique_anchor(anchor)
            require(result["size"] >= 19, "Object-block anchor is too small")
            anchors[anchor["symbol"]] = result
        require(len(anchors) >= 4, "Object block requires four independent unique anchors")
        bindings = dict(addresses)
        for item in block.get("globals", []):
            require(len(set(item["anchor_symbols"])) >= 2,
                    "Object-block global requires two distinct anchors")
            for symbol in item["anchor_symbols"]:
                refs = [r for r in anchors[symbol]["references"] if r["symbol"] == item["symbol"]]
                require(refs and {r["target"] for r in refs} == {item["address"]},
                        "Object-block anchors disagree on a global")
            bindings[item["symbol"]] = int(item["address"], 16)
        for item in block.get("rtti", []):
            rtti, vtable = self.original.rtti(), int(item["address"], 16)
            deleting, destructor = addresses[item["deleting_destructor"]], addresses[item["destructor"]]
            require(rtti.class_of(item["destructor"])[0] == item["type"] and
                    rtti.type_of_vtable(vtable) == item["type"] and
                    self.original.u32(vtable) == deleting and
                    rtti.owner(deleting) == item["type"] and rtti.calls(deleting, destructor),
                    "Object-block RTTI destructor chain differs")
            refs = [r for r in anchors[item["destructor"]]["references"]
                    if r["symbol"] == item["symbol"]]
            require(refs and {r["target"] for r in refs} == {item["address"]},
                    "Unique destructor does not corroborate the RTTI vtable")
            bindings[item["symbol"]] = vtable
        for symbol in symbols:
            self.linked_body({**entry, "symbol": symbol,
                              "address": f"0x{addresses[symbol]:08X}"}, bindings)
        return bindings

    def data_table(self, entry):
        """Identify a tiny body through named pointers in a uniquely located table."""
        table = entry["table"]
        _, _, obj, _ = self.object(entry)
        symbols = {s.name: s for s in obj.symbols}
        anchor = symbols[table["anchor_symbol"]]
        require(anchor.section == table["section"] > 0, "Wrong anchored data section")
        section = obj.sections[anchor.section - 1]
        address = int(table["address"], 16)
        require(not section.is_code and section.name == ".data" and
                address + anchor.value == int(table["anchor_address"], 16),
                "Wrong data table or anchor offset")
        require(table["anchors"], "No incoming reference to the data table")
        for caller in table["anchors"]:
            refs = [r for r in self.unique_anchor(caller)["references"] if r["symbol"] == anchor.name]
            require(refs and {r["target"] for r in refs} == {table["anchor_address"]},
                    "Independent caller does not locate the data table")
        for relocation in section.relocs:
            require(relocation.type == coff.REL_DIR32 and
                    0 <= relocation.offset <= len(section.data) - 4,
                    "Unsupported or truncated data table relocation")
        mask = {r.offset + k for r in section.relocs for k in range(4)}
        pattern = b"(?=" + libmatch.pattern(section.data, mask) + b")"
        hits = []
        for pe_section in self.original.pe.sections:
            base = self.original.base + pe_section.VirtualAddress
            data = self.original.read(base, pe_section.SizeOfRawData)
            hits.extend(base + m.start() for m in re.finditer(pattern, data, re.S))
        require(hits == [address], "Complete data table is not independently unique")
        functions = {a["symbol"]: self.unique_anchor(a) for a in table["function_anchors"]}
        pointers = table["target_pointers"]
        require(len(set(pointers)) == len(pointers) >= 2,
                "Require two distinct named incoming pointers")
        offsets = set()
        for name in pointers:
            symbol = symbols[name]
            require(symbol.section == section.number and symbol.storage == coff.STORAGE_EXTERNAL,
                    "Target pointer is not defined in this data section")
            offsets.add(symbol.value)
        require(len(offsets) == len(pointers), "Named pointers must occupy distinct slots")
        linked, seen = bytearray(section.data), set()
        for relocation in section.relocs:
            symbol = obj.by_index[relocation.symbol].name
            addend = struct.unpack_from("<i", section.data, relocation.offset)[0]
            if symbol == entry["symbol"]:
                require(relocation.offset in offsets and addend == 0,
                        "Target relocation lacks a distinct named pointer")
                target = int(entry["address"], 16)
                seen.add(relocation.offset)
            else:
                require(symbol in functions, "Unproved auxiliary data table reference")
                target = int(functions[symbol]["address"], 16)
            struct.pack_into("<I", linked, relocation.offset, (target + addend) & 0xFFFFFFFF)
        require(seen == offsets, "Missing target data pointer")
        require(bytes(linked) == self.original.read(address, len(linked)),
                "Fully relocated data table differs")
        return {}

    def target(self, entry):
        address = int(entry["address"], 16)
        bindings = {}
        if entry["kind"] == "object_block":
            bindings = self.object_block(entry)
        elif entry["kind"] == "data_table":
            bindings = self.data_table(entry)
        elif entry["kind"] in ("incoming", "global"):
            symbol = entry["symbol"] if entry["kind"] == "incoming" else entry["global_symbol"]
            target = entry["address"] if entry["kind"] == "incoming" else entry["global_address"]
            require(entry["anchors"], f"No independent anchors: {entry}")
            for anchor in entry["anchors"]:
                result = self.unique_anchor(anchor)
                references = [r for r in result["references"] if r["symbol"] == symbol]
                require(references and {r["target"] for r in references} == {target},
                        f"Anchor does not resolve {symbol} to {target}: {anchor}")
            bindings[symbol] = int(target, 16)
        else:
            require(entry["kind"] == "rtti", f"Unknown identity kind: {entry}")
            rtti = self.original.rtti()
            require(entry["symbol"].startswith("??1") and
                    rtti.class_of(entry["symbol"])[0] == entry["rtti_type"],
                    "Destructor symbol does not name the RTTI class")
            vtable = int(entry["vtable"], 16)
            deleting = int(entry["deleting_destructor"], 16)
            require(rtti.type_of_vtable(vtable) == entry["rtti_type"], "Wrong RTTI type")
            require(self.original.u32(vtable) == deleting, "Wrong deleting-destructor slot")
            require(rtti.owner(deleting) == entry["rtti_type"], "Wrong destructor owner")
            require(rtti.calls(deleting, address), "RTTI-selected wrapper does not call this body")
            bindings[entry["symbol"]] = address
            # The wrapper is an identity anchor, not an additional recovered function.
            wrapper = {**entry, "address": entry["deleting_destructor"],
                       "symbol": entry["symbol"].replace("??1", "??_G", 1)
                                 .replace("@@UAE@XZ", "@@UAEPAXI@Z")}
            self.linked_body(wrapper, bindings)

        result = self.linked_body(entry, bindings)
        require(result["size"] == entry["size"], f"Archive extent differs: {entry}")
        if entry["kind"] == "global":
            require(result["fully_linked_locations"] == [entry["address"]],
                    f"Relocated getter body has more than one location: {entry}")


def verify(path=ROOT / "data/library_identified.json"):
    document = json.loads(Path(path).read_text())
    entries = identity_records(document["targets"])
    audit = Audit()
    for entry in entries.values():
        if entry["kind"] == "object_block":
            entry = {**entry, "block": document["blocks"][entry["block"]]}
        audit.target(entry)
    return {address: (row["size"], row["symbol"], row["library"], row["object"])
            for address, row in entries.items()}


def merge(automatic, identified):
    combined = dict(automatic)
    for address, row in identified.items():
        require(address not in combined or combined[address] == row,
                f"Conflicting library identity at 0x{address:08X}")
        for other, old in combined.items():
            require(other == address or
                    address >= other + old[0] or other >= address + row[0],
                    f"Overlapping library extents at 0x{address:08X} and 0x{other:08X}")
        combined[address] = row
    return combined


if __name__ == "__main__":
    records = verify(Path(sys.argv[1])) if len(sys.argv) > 1 else verify()
    for address, (size, symbol, library, member) in records.items():
        print(f"0x{address:08X} {symbol}: verified {size} bytes from {library}/{member}")
    print(f"{len(records)} library identities verified")
