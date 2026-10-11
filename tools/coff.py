"""Read the parts of an i386 COFF object the checker needs: sections, relocations, symbols."""

import struct
from dataclasses import dataclass, field

REL_DIR32 = 0x06
REL_REL32 = 0x14
CODE = 0x00000020


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

    def functions(self):
        """(symbol, section, start, end) for every function defined in a code section."""
        for sec in self.sections:
            if not sec.is_code:
                continue
            defs = sorted((s for s in self.symbols if s.section == sec.number and s.type == 0x20),
                          key=lambda s: s.value)
            for k, s in enumerate(defs):
                end = defs[k + 1].value if k + 1 < len(defs) else len(sec.data)
                yield s, sec, s.value, end
