"""Shared reading of gamemd.exe for the agent helpers: function extents, names, the
direct calls and data references each function makes, and where matched code lives."""

import bisect
import csv
import re
import struct
from functools import cached_property
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parent.parent
ADDRESS = re.compile(r"0x[0-9a-f]{6,8}")


def load_csv(name: str) -> list[dict]:
    with (ROOT / "data" / name).open() as f:
        return list(csv.DictReader(f))


class ExeIndex:
    def __init__(self, exe: Path = ROOT / "orig/gamemd.exe"):
        pe = pefile.PE(str(exe), fast_load=True)
        self.base = pe.OPTIONAL_HEADER.ImageBase
        text = pe.sections[0]
        self.t0 = self.base + text.VirtualAddress
        self.code = text.get_data()
        self.functions = {int(r["address"], 16): int(r["size"]) for r in load_csv("functions.csv")}
        self.starts = sorted(self.functions)
        self.progress = {int(r["address"], 16): r for r in load_csv("progress.csv")}
        names: dict[int, str] = {}
        for source in ("names.csv", "symbols.csv"):
            for r in load_csv(source):
                names.setdefault(int(r["address"], 16), r["name"])
        for a, r in self.progress.items():
            names.setdefault(a, r["name"])
        for r in load_csv("library.csv"):
            names.setdefault(int(r["address"], 16), f"{r['symbol']} [CRT]")
        self.names = names

    def body(self, address: int) -> bytes:
        start = address - self.t0
        return self.code[start:start + self.functions[address]]

    def name(self, address: int) -> str:
        return self.names.get(address, f"FUN_{address:08X}")

    def matched(self, address: int) -> bool:
        row = self.progress.get(address)
        return bool(row) and row["result"] == "MATCH"

    def containing(self, at: int) -> int | None:
        k = bisect.bisect_right(self.starts, at) - 1
        if k >= 0 and at < self.starts[k] + self.functions[self.starts[k]]:
            return self.starts[k]
        return None

    def instructions(self, address: int):
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        return list(md.disasm(self.body(address), address))

    def calls(self, address: int) -> list[int]:
        """Function starts this function reaches by a direct call or tail jump, in order."""
        out, end = [], address + self.functions[address]
        for ins in self.instructions(address):
            if ins.mnemonic in ("call", "jmp") and ins.op_str.startswith("0x"):
                target = int(ins.op_str, 16)
                if target in self.functions and not address <= target < end:
                    out.append(target)
        return out

    def data_refs(self, address: int) -> set[int]:
        """Absolute addresses outside .text that the function's instructions name."""
        refs = set()
        for ins in self.instructions(address):
            for h in ADDRESS.findall(ins.op_str):
                v = int(h, 16)
                if v >= self.t0 + len(self.code) and v < self.base + 0x1000000:
                    refs.add(v)
        return refs

    @cached_property
    def callers(self) -> dict[int, set[int]]:
        """Callee start -> starts of the functions holding a direct call to it."""
        out: dict[int, set[int]] = {}
        code, t0 = self.code, self.t0
        for m in re.finditer(rb"\xe8", code):
            if m.start() + 5 > len(code):
                continue
            target = t0 + m.start() + 5 + struct.unpack_from("<i", code, m.start() + 1)[0]
            if target in self.functions:
                holder = self.containing(t0 + m.start())
                if holder is not None and holder != target:
                    out.setdefault(target, set()).add(holder)
        return out

    def source_line(self, address: int) -> tuple[str, str] | None:
        """(file, definition line) for a matched function, from its marker in src/."""
        row = self.progress.get(address)
        if not row or not row.get("source"):
            return None
        path = ROOT / row["source"]
        if not path.exists():
            return None
        lines = path.read_text(encoding="latin-1").splitlines()
        marker = re.compile(rf"//\s*(FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY): GAMEMD 0x{address:08X}\b", re.I)
        for i, line in enumerate(lines):
            if marker.search(line):
                for follow in lines[i + 1:i + 6]:
                    if follow.strip() and not follow.lstrip().startswith("//"):
                        return row["source"], follow.strip()
                return row["source"], ""
        return row["source"], ""
