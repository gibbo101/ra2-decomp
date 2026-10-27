"""The exe's vtables, found through RTTI: which classes' vtables hold an address, and the
class a vtable or a decorated member symbol names."""

from __future__ import annotations

import struct

import capstone

# Access codes of a decorated member function that make it virtual (thunks included).
VIRTUAL_CODES = set("EFGHMNOPUVWX")


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

    def type_of_body(self, body: str) -> str | None:
        """The RTTI type name of a decorated class body ('?$VectorClass@H@@'), if the exe has one."""
        for tag in (".?AV", ".?AU"):
            if tag + body in self.types:
                return tag + body
        return None

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
