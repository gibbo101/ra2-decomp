"""Read the parts of an i386 COFF object the checker needs: sections, relocations, symbols."""

import struct
from functools import cached_property
from dataclasses import dataclass, field

REL_DIR32 = 0x06
REL_REL32 = 0x14
CODE = 0x00000020
STORAGE_EXTERNAL = 2


@dataclass
class Reloc:
    offset: int
    symbol: int
    type: int


@dataclass
class Section:
    number: int
    name: str
    data: bytes
    flags: int
    relocs: list[Reloc] = field(default_factory=list)
    # Offsets where the original began a new object: alignment filler before one is not code.
    object_starts: set[int] = field(default_factory=set)

    @property
    def is_code(self) -> bool:
        return bool(self.flags & CODE)


@dataclass
class Symbol:
    index: int
    name: str
    value: int
    section: int
    type: int
    storage: int


class CoffObject:
    def __init__(self, data: bytes):
        nsec, _, symoff, nsym, opt = struct.unpack_from("<HIIIH", data, 2)
        strtab = symoff + nsym * 18

        def name_of(raw: bytes) -> str:
            if raw[:4] == b"\0\0\0\0":
                at = strtab + struct.unpack_from("<I", raw, 4)[0]
                return data[at:data.index(b"\0", at)].decode("latin1")
            return raw.rstrip(b"\0").decode("latin1")

        self.sections: list[Section] = []
        for i in range(nsec):
            h = 20 + opt + i * 40
            raw_name = data[h:h + 8]
            if raw_name.startswith(b"/"):
                at = strtab + int(raw_name[1:].rstrip(b"\0"))
                sname = data[at:data.index(b"\0", at)].decode("latin1")
            else:
                sname = raw_name.rstrip(b"\0").decode("latin1")
            size, ptr, relptr, _, nrel, _, flags = struct.unpack_from("<IIIIHHI", data, h + 16)
            sec = Section(i + 1, sname, data[ptr:ptr + size] if ptr else bytes(size), flags)
            for k in range(nrel):
                sec.relocs.append(Reloc(*struct.unpack_from("<IIH", data, relptr + k * 10)))
            self.sections.append(sec)

        self.symbols: list[Symbol] = []
        self.by_index: dict[int, Symbol] = {}
        i = 0
        while i < nsym:
            e = data[symoff + i * 18:symoff + i * 18 + 18]
            value, secnum, typ, storage, naux = struct.unpack_from("<IhHBB", e, 8)
            sym = Symbol(i, name_of(e[:8]), value, secnum, typ, storage)
            self.symbols.append(sym)
            self.by_index[i] = sym
            i += 1 + naux

    @cached_property
    def _functions(self):
        by_section = {}
        for sym in self.symbols:
            if sym.type == 0x20 or sym.storage == STORAGE_EXTERNAL:
                by_section.setdefault(sym.section, []).append(sym)

        functions = []
        for sec in self.sections:
            if not sec.is_code:
                continue
            defs = sorted(by_section.get(sec.number, ()), key=lambda sym: sym.value)
            for i, sym in enumerate(defs):
                end = defs[i + 1].value if i + 1 < len(defs) else len(sec.data)
                functions.append((sym, sec, sym.value, end))
        return tuple(functions)

    def functions(self):
        """Iterate the object's function extents in section and symbol-offset order."""
        return iter(self._functions)
