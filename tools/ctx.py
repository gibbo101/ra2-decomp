"""Everything known about one function of gamemd.exe, for whoever reconstructs it.

Usage: uv run tools/ctx.py 0x00410A60

Prints its extent, module and names, its disassembly with call targets, globals and
string literals named where data/ knows them, its direct callers, the YRpp lines that
mention it, and the TibSun archive file to start from when the module exists in TS.
"""

import bisect
import csv
import re
import struct
import sys
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parent.parent


def load_csv(name):
    with (ROOT / "data" / name).open() as f:
        return list(csv.DictReader(f))


def main() -> None:
    address = int(sys.argv[1], 16)
    pe = pefile.PE(str(ROOT / "orig/gamemd.exe"), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    text = pe.sections[0]
    t0, code = base + text.VirtualAddress, text.get_data()

    functions = {int(r["address"], 16): r for r in load_csv("functions.csv")}
    starts = sorted(functions)
    if address not in functions:
        raise SystemExit(f"0x{address:08X} is not a function start in data/functions.csv")
    size = int(functions[address]["size"])

    names: dict[int, str] = {}
    for r in load_csv("names.csv"):
        names.setdefault(int(r["address"], 16), r["name"])
    for r in load_csv("symbols.csv"):
        names.setdefault(int(r["address"], 16), r["name"])
    for r in load_csv("library.csv"):
        names.setdefault(int(r["address"], 16), f"{r['symbol']} [CRT, prebuilt]")
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
    for dll in pe.DIRECTORY_ENTRY_IMPORT:
        for imp in dll.imports:
            if imp.name:
                names[imp.address] = f"[import] {dll.dll.decode()}!{imp.name.decode()}"
    modules = [(int(r["first"], 16), int(r["last"], 16), r["module"]) for r in load_csv("modules.csv")]

    def module_of(a: int) -> str:
        for lo, hi, m in modules:
            if lo <= a <= hi:
                return m
        return "?"

    def describe(v: int) -> str:
        if v in names:
            return names[v]
        if v in functions:
            return f"FUN_{v:08X} (module {module_of(v)})"
        if base + 0x3E1000 <= v < base + 0x77A000:
            raw = img[v - base:v - base + 80]
            s = raw.split(b"\0", 1)[0]
            if len(s) >= 2 and all(32 <= c < 127 for c in s):
                return f'"{s.decode()}"'
            return f"DAT_{v:08X}"
        return ""

    print(f"0x{address:08X}  size {size}  module {module_of(address)}  "
          f"evidence {functions[address]['evidence']}  name {names.get(address, '(none)')}")
    body = code[address - t0:address - t0 + size]
    if re.search(rb"[^\x90]\x90{1,8}[^\x90]", body[:-1]):
        print("NOTE: nop bytes inside the body; this may be one of EA's patch sites")

    print("\n-- disassembly --")
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for ins in md.disasm(body, address):
        notes = []
        for h in re.findall(r"0x[0-9a-f]{6,}", ins.op_str):
            d = describe(int(h, 16))
            if d:
                notes.append(f"{h} = {d}")
        print(f"  {ins.address:08X}  {ins.mnemonic:6} {ins.op_str:40} {'; ' + ', '.join(notes) if notes else ''}")

    print("\n-- direct callers --")
    callers = set()
    for m in re.finditer(rb"\xe8", code):
        at = t0 + m.start()
        if at + 5 <= t0 + len(code) and at + 5 + struct.unpack_from("<i", code, m.start() + 1)[0] == address:
            k = bisect.bisect_right(starts, at) - 1
            if k >= 0 and at < starts[k] + int(functions[starts[k]]["size"]):
                callers.add(starts[k])
    for c in sorted(callers)[:20]:
        print(f"  0x{c:08X} {names.get(c, '')} (module {module_of(c)})")
    if len(callers) > 20:
        print(f"  ... {len(callers) - 20} more")

    print("\n-- YRpp lines naming it --")
    pattern = re.compile(rf"0x0*{address:X}\b", re.I)
    for h in sorted((ROOT / "reference/YRpp").rglob("*.h")):
        lines = h.read_text(errors="replace").splitlines()
        for i, line in enumerate(lines):
            if pattern.search(line):
                print(f"  {h.name}:{i}: {lines[i - 1].strip() if i else ''} {line.strip()}")

    module = module_of(address)
    tibsun = ROOT / "reference/tibsun/code"
    match = [p for p in tibsun.glob("*.cpp") if p.stem.lower() == module.lower()]
    print("\n-- TibSun archive --")
    print(f"  {match[0].relative_to(ROOT)}" if match else "  no file of this module's name: likely RA2-only")


if __name__ == "__main__":
    main()
