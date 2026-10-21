"""Write a cross-project manifest of matched functions as JSON (format in docs/MANIFEST.md).

    uv run tools/xmanifest.py 0x0040DCB0 0x004101F0          # these functions, to stdout
    uv run tools/xmanifest.py --from data/crosscheck/pilot_addresses.txt --out FILE  # '#' starts a comment
    uv run tools/xmanifest.py --all --out build/xmanifest.json

Each function is a MATCH row of data/progress.csv. Its source is compiled with check.py's
flags and compared as check.py compares it; the manifest records the original's bytes as
two hashes (raw, and with every relocation field zeroed), each relocation's target in the
original and whether it is ours, and what else the exe proves: the RTTI of every vtable
the function references, and whether a virtual symbol is a slot of its class's vtable.
The output depends only on the inputs the header pins, so two runs give identical files.
"""

import csv
import hashlib
import json
import shutil
import struct
import sys
from pathlib import Path

import capstone

sys.path.insert(0, str(Path(__file__).resolve().parent))
from check import (FLAGS, ROOT, Original, annotations, bind_annotations, compare, compile_source,  # noqa: E402
                   known_names, prepare_headers, ref_checks)
from coff import REL_DIR32, CoffObject  # noqa: E402

FORMAT_VERSION = 1
NORMALIZATION = "reloc-zero-v1"
COMPILER = "Visual C++ 6.0 SP3 (C2.DLL 12.00.8447)"
REPOSITORY = "https://github.com/gibbo101/ra2-decomp"
REL_NAMES = {0x00: "ABSOLUTE", 0x06: "DIR32", 0x07: "DIR32NB", 0x0A: "SECTION", 0x0B: "SECREL",
             0x14: "REL32"}
# Access codes of a decorated member function that make it virtual (thunks included).
VIRTUAL_CODES = set("EFGHMNOPUVWX")
DATA_FILES = ("data/functions.csv", "data/progress.csv", "data/names.csv", "data/symbols.csv",
              "data/library.csv", "data/exe_patches.csv")
REFERENCES = ("reference/tibsun", "reference/YRpp")


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def git_commit(repo: Path) -> str | None:
    """The commit HEAD names, read from the repository's files."""
    git = repo / ".git"
    try:
        head = (git / "HEAD").read_text().strip()
    except OSError:
        return None
    if not head.startswith("ref: "):
        return head
    ref = head[5:]
    if (git / ref).is_file():
        return (git / ref).read_text().strip()
    packed = git / "packed-refs"
    if packed.is_file():
        for line in packed.read_text().splitlines():
            if line.endswith(" " + ref) and not line.startswith(("#", "^")):
                return line.split()[0]
    return None


def tree_sha256(base: Path) -> str:
    """SHA-256 of `sha256sum`-style lines ("<hex>  <path>\\n", path relative to the
    repository, sorted by path) for every file under `base`."""
    files = sorted((p.relative_to(ROOT).as_posix(), p) for p in base.rglob("*") if p.is_file())
    return sha256("".join(f"{sha256(p.read_bytes())}  {rel}\n" for rel, p in files).encode())


class Rtti:
    """The exe's vtables, found through their complete object locators."""

    def __init__(self, orig: Original, operator_delete: int | None):
        self.orig = orig
        self.operator_delete = operator_delete
        pe = orig.pe
        text = next(s for s in pe.sections if s.Name.startswith(b".text"))
        rdata = next(s for s in pe.sections if s.Name.startswith(b".rdata"))
        self.text = range(orig.base + text.VirtualAddress, orig.base + text.VirtualAddress + text.Misc_VirtualSize)
        r0 = orig.base + rdata.VirtualAddress
        data = orig.image[r0 - orig.base:r0 - orig.base + rdata.Misc_VirtualSize]
        image = orig.image
        names = {}
        for tag in (b".?AV", b".?AU"):
            at = image.find(tag)
            while at >= 0:
                names[orig.base + at - 8] = image[at:image.index(b"\0", at)].decode("latin1")
                at = image.find(tag, at + 1)
        words = struct.unpack_from(f"<{len(data) // 4}I", data)
        self.cols = {}
        self.bases = {}
        for k in range(len(words) - 4):
            if words[k] == 0 and words[k + 3] in names:
                self.cols[r0 + 4 * k] = (names[words[k + 3]], words[k + 1])
                self.bases[names[words[k + 3]]] = self.base_classes(words[k + 4], names)
        self.vtables = {}
        for k, w in enumerate(words):
            if w in self.cols:
                vt = r0 + 4 * (k + 1)
                slots = []
                while orig.u32(vt + 4 * len(slots)) in self.text:
                    slots.append(orig.u32(vt + 4 * len(slots)))
                self.vtables[vt] = (*self.cols[w], slots)
        self.types = {name for name, _, _ in self.vtables.values()}
        self.bodies = {name[4:] for name in self.types}
        self.slot_of = {}
        for vt, (name, _, slots) in sorted(self.vtables.items()):
            for s in slots:
                self.slot_of.setdefault(s, []).append(name)
        # The linker folded every pure virtual's slot into _purecall, the most common slot.
        self.purecall = max(self.slot_of, key=lambda s: len(self.slot_of[s]))

    def base_classes(self, hierarchy: int, names: dict[int, str]) -> set[str]:
        """Every class in a class hierarchy descriptor's base class array, itself included."""
        count, array = self.orig.u32(hierarchy + 8), self.orig.u32(hierarchy + 12)
        return {names.get(self.orig.u32(self.orig.u32(array + 4 * i)), "") for i in range(min(count, 256))}

    def owner(self, address: int) -> str | None:
        """The class whose vtables hold `address` and from which every other class holding it
        derives; None where no one class does, as with code the linker shares."""
        holders = set(self.slot_of.get(address, ()))
        return next((c for c in sorted(holders) if all(c in self.bases.get(h, ()) for h in holders)), None)

    def type_of_vtable(self, vtable: int) -> str | None:
        col = self.orig.u32(vtable - 4)
        return self.cols[col][0] if col in self.cols else None

    def class_of(self, symbol: str) -> tuple[str | None, str | None]:
        """(RTTI type name, access code) of a decorated member symbol whose class has
        RTTI in the exe; (None, None) otherwise."""
        if symbol.startswith(("??_G", "??_E")):
            rest = symbol[4:]
        elif symbol.startswith(("??0", "??1")):
            rest = symbol[3:]
        elif symbol.startswith("??"):
            rest = symbol[3:] if symbol[2] != "_" else symbol[4:]
        elif symbol.startswith("?"):
            rest = symbol[1:].split("@", 1)[1] if "@" in symbol[1:] else ""
        else:
            return None, None
        best = None
        at = rest.find("@@")
        while at >= 0:
            if rest[:at + 2] in self.bodies:
                best = at + 2
            at = rest.find("@@", at + 1)
        if best is None or best >= len(rest):
            return None, None
        body = rest[:best]
        name = ".?AV" + body if ".?AV" + body in self.types else ".?AU" + body
        return name, rest[best]

    def class_vtables(self, name: str) -> list[list[int]]:
        return [slots for n, _, slots in self.vtables.values() if n == name]

    def calls(self, caller: int, callee: int | None) -> bool:
        size = self.orig.sizes.get(caller, 0)
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        return callee is not None and any(i.mnemonic == "call" and i.op_str == f"0x{callee:x}"
                                          for i in md.disasm(self.orig.read(caller, size), caller))

    def deleting(self, slot: int) -> bool:
        """Whether a slot is a deleting destructor: it tests bit 0 of its flag argument
        and calls operator delete."""
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        code = list(md.disasm(self.orig.read(slot, self.orig.sizes.get(slot, 0)), slot))
        return (any(i.mnemonic == "test" and i.op_str.endswith(", 1") for i in code)
                and self.calls(slot, self.operator_delete))

    def virtual_verdict(self, symbol: str, address: int) -> str:
        """Whether the exe agrees with the symbol about being a virtual of its class: a
        virtual is a slot of its class's vtable, or a pure virtual's body where it has _purecall."""
        name, access = self.class_of(symbol)
        free = symbol[:1] != "?" or (not symbol.startswith("??") and "@@Y" in symbol
                                     and symbol.index("@@") == symbol.index("@", 1))
        # A slot of one class hierarchy is a virtual, never a free or non-virtual function.
        slot = "mismatch" if self.owner(address) else "unchecked"
        if name is None:
            if address in self.slot_of:
                return slot if free else "unchecked"
            return "n-a" if free else "unchecked"
        is_dtor = symbol.startswith("??1")
        in_class = any(address in slots or (is_dtor and any(self.calls(s, address) or self.deleting(s)
                                                              for s in slots))
                       for slots in self.class_vtables(name))
        if access in VIRTUAL_CODES:
            if in_class:
                return "verified"
            pure = any(self.purecall in slots for slots in self.class_vtables(name))
            return "unchecked" if pure else "mismatch"
        if in_class:
            return "mismatch"
        return slot if address in self.slot_of else "verified"


def vftable_type(symbol: str) -> str | None:
    """The class body a `??_7...@@6B...` vftable symbol names, as '.?AV' + body."""
    if not symbol.startswith("??_7") or "@@6B" not in symbol:
        return None
    return symbol[4:symbol.index("@@6B") + 2]


def stack_only(code: bytes, address: int) -> bool:
    """Whether every memory operand of the function addresses its own stack frame."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    insns = list(md.disasm(code, address))
    frame = [i.mnemonic for i in insns[:2]] == ["push", "mov"] and insns[1].op_str == "ebp, esp"
    stack = {capstone.x86.X86_REG_ESP} | ({capstone.x86.X86_REG_EBP} if frame else set())
    for i in insns:
        if i.mnemonic == "lea":
            continue
        for op in i.operands:
            if op.type == capstone.x86.X86_OP_MEM and (op.mem.base not in stack or op.mem.index != 0):
                return False
    return sum(i.size for i in insns) == len(code)


def references_verdict(statuses: list[str]) -> str:
    if not statuses:
        return "n-a"
    if "mismatch" in statuses:
        return "mismatch"
    return "some-unverified" if "unverified" in statuses else "all-ok"


def describe(orig, rtti, retail, patch_sites, obj, raw, row, name, names, local) -> dict:
    address, size = int(row["address"], 16), int(row["size"])
    res = compare(orig, obj, raw, address, name, names, local, prefer=local)
    pick = next(f for f in obj.functions() if f[0].name == res.symbol)
    sym, sec, start, end = pick
    ours = sec.data[start:end]
    theirs = orig.read(address, size)
    fields = sorted((r.offset - start, r.type, obj.by_index[r.symbol]) for r in sec.relocs
                    if start <= r.offset < end and r.offset - start + 4 <= size)
    checked = {c.offset: c for c in ref_checks(orig, obj, raw, pick, ours, theirs, address, names, local)} \
        if res.bytes_match else {}
    bytes_retail = retail[address - orig.base:address - orig.base + size]
    normalized = bytearray(bytes_retail)
    relocations, statuses, vtables = [], [], []
    for off, typ, target in fields:
        normalized[off:off + 4] = b"\0\0\0\0"
        c = checked.get(off)
        local_label = target.section == sec.number and target.name.startswith(("$", "."))
        entry = {
            "offset": off,
            "type": REL_NAMES.get(typ, f"0x{typ:02X}"),
            "addend": c.addend if c else struct.unpack_from("<i", ours, off)[0],
            "target_symbol": None if local_label else target.name,
            "target_address": f"0x{c.got:08X}" if c else None,
            "binding": c.status if c else "unchecked",
        }
        body = vftable_type(target.name)
        if c and body is not None and typ == REL_DIR32:
            found = rtti.type_of_vtable(c.got)
            entry["rtti"] = found
            vtables.append(found in (".?AV" + body, ".?AU" + body))
        if c:
            statuses.append(c.status)
        relocations.append(entry)
    patches = [p for p in patch_sites if address <= p[0] < address + size]
    out = {
        "address": f"0x{address:08X}",
        "size": size,
        "symbol": res.symbol,
        "source": row["source"],
        "byte_match": res.bytes_match,
        "raw_sha256": sha256(bytes_retail),
        "normalized_sha256": sha256(bytes(normalized)),
        "relocations": relocations,
        "verification": {
            "bytes": res.bytes_match,
            "references": references_verdict(statuses) if res.bytes_match else "unchecked",
            "layout": "n-a" if stack_only(theirs, address) else "unchecked",
            "vtable": ("n-a" if not vtables else "verified" if all(vtables) else "mismatch"),
            "virtual": rtti.virtual_verdict(res.symbol, address),
        },
    }
    if patches:
        out["exe_patches"] = [{"offset": a - address, "retail": p, "restored": o} for a, p, o in patches]
    return out


def load_patch_sites() -> list[tuple[int, str, str]]:
    path = ROOT / "data/exe_patches.csv"
    if not path.exists():
        return []
    with path.open() as f:
        return [(int(r["address"], 16), r["patched"], r["original"]) for r in csv.DictReader(f)]


def main() -> None:
    argv = sys.argv[1:]
    out_path = None
    if "--out" in argv:
        k = argv.index("--out")
        out_path = Path(argv[k + 1])
        del argv[k:k + 2]
    if "--from" in argv:
        k = argv.index("--from")
        lines = Path(argv[k + 1]).read_text().splitlines()
        argv += [t for line in lines for t in line.split("#", 1)[0].split()]
        del argv[k:k + 2]
    everything = "--all" in argv
    if everything:
        argv.remove("--all")
    with (ROOT / "data/progress.csv").open() as f:
        rows = {}
        for r in csv.DictReader(f):
            if r["result"] == "MATCH":
                rows.setdefault(int(r["address"], 16), r)
    wanted = sorted(rows) if everything else sorted({int(a, 16) for a in argv})
    if not wanted:
        raise SystemExit(__doc__)
    missing = [a for a in wanted if a not in rows]
    if missing:
        raise SystemExit("not a MATCH in data/progress.csv: " + ", ".join(f"0x{a:08X}" for a in missing))

    orig = Original()
    patch_sites = load_patch_sites()
    retail = orig.pe.get_memory_mapped_image()
    names = known_names()
    rtti = Rtti(orig, names.get("operator delete"))
    exe = (ROOT / "orig/gamemd.exe").read_bytes()
    by_source: dict[str, list[int]] = {}
    for a in wanted:
        by_source.setdefault(rows[a]["source"], []).append(a)

    functions = {}
    headers = prepare_headers(None)
    work = ROOT / "build/xmanifest" / headers.name
    try:
        for source, addresses in sorted(by_source.items()):
            src = ROOT / source
            obj_path = work / (src.stem + ".obj")
            compiled, log = compile_source(src, headers=headers, out=obj_path)
            if compiled is None:
                for a in addresses:
                    functions[a] = {"address": f"0x{a:08X}", "size": int(rows[a]["size"]),
                                    "symbol": rows[a]["symbol"], "source": source, "byte_match": False,
                                    "compile_error": True}
                print(f"{source}: does not compile", file=sys.stderr)
                continue
            raw = compiled.read_bytes()
            compiled.unlink()
            obj = CoffObject(raw)
            local, _ = bind_annotations(orig, obj, raw, src)
            marked = dict(annotations(src))
            for a in addresses:
                if a not in marked:
                    raise SystemExit(f"0x{a:08X}: no marker in {source}")
                functions[a] = describe(orig, rtti, retail, patch_sites, obj, raw, rows[a], marked[a], names, local)
    finally:
        shutil.rmtree(headers, ignore_errors=True)
        if work.is_dir() and not any(work.iterdir()):
            work.rmdir()

    sources = {s: sha256((ROOT / s).read_bytes()) for s in sorted(by_source)}
    manifest = {
        "format": "ra2-decomp crosscheck manifest",
        "format_version": FORMAT_VERSION,
        "normalization": NORMALIZATION,
        "project": "ra2-decomp",
        "repository": REPOSITORY,
        "commit": git_commit(ROOT),
        "exe": {"file": "gamemd.exe", "version": "1.001", "md5": hashlib.md5(exe).hexdigest(),
                "sha256": sha256(exe), "size": len(exe)},
        "compiler": COMPILER,
        "flags": FLAGS[1:],
        "inputs": {
            "include_tree_sha256": tree_sha256(ROOT / "include"),
            **{p: sha256((ROOT / p).read_bytes()) for p in DATA_FILES if (ROOT / p).exists()},
            **{p: git_commit(ROOT / p) for p in REFERENCES},
        },
        "sources": sources,
        "functions": [functions[a] for a in wanted],
    }
    text = json.dumps(manifest, indent=1) + "\n"
    if out_path:
        out_path.write_text(text)
        good = sum(1 for f in manifest["functions"] if f.get("byte_match"))
        print(f"{len(wanted)} functions ({good} byte matches) -> {out_path}")
    else:
        sys.stdout.write(text)


if __name__ == "__main__":
    main()
