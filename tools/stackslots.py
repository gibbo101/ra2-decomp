"""Compare frame-relative stack accesses and identify the candidate's named locals.

uv run tools/stackslots.py SOURCE ADDRESS [--overlay DIR]
Use permute.py --stack-focus to prefer declarations identified here. Unreachable
instructions, conflicting stack depths and unknown call cleanup cannot identify a slot.
"""
import argparse
from collections import deque
import difflib
from functools import lru_cache
from pathlib import Path
import re
import struct
import tempfile

import capstone
from capstone.x86 import X86_OP_IMM, X86_OP_MEM
import check
from coff import CoffObject


def decode(code, address):
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    return list(decoder.disasm(code, address))


def successors(ins):
    if ins.group(capstone.CS_GRP_RET):
        return []
    fall = ins.address + ins.size
    if ins.group(capstone.CS_GRP_JUMP):
        target = ins.operands[0]
        dest = target.imm if target.type == X86_OP_IMM else None
        return [dest] if ins.mnemonic == "jmp" else [dest, fall]
    return [fall]


def return_cleanup(orig, addr):
    size = orig.sizes.get(addr)
    if size is None:
        return None
    code = {i.address: i for i in decode(orig.read(addr, size), addr)}
    pending, seen, pops = [addr], set(), set()
    while pending:
        at = pending.pop()
        if at in seen:
            continue
        seen.add(at)
        ins = code.get(at)
        if ins is None:
            return None
        if ins.mnemonic == "ret":
            pops.add(ins.operands[0].imm if ins.operands else 0)
        else:
            pending.extend(successors(ins))
    return pops.pop() if len(pops) == 1 else None


def shift(slot, delta):
    return (slot[0], slot[1] + delta) if slot is not None and delta is not None else None


def transfer(ins, state, pop):
    sp, bp = state
    op = ins.mnemonic
    if op in ("push", "pop") and ins.op_str not in ("sp", "esp"):
        width = ins.operands[0].size
        return shift(sp, -width if op == "push" else width), None if ins.op_str in ("bp", "ebp") else bp
    if op == "mov" and ins.op_str == "ebp, esp":
        return sp, sp
    if op == "mov" and ins.op_str == "esp, ebp":
        return bp, bp
    if op == "leave":
        return shift(bp, 4), None
    if op == "call":
        return shift(sp, pop(ins)), bp
    if op in ("add", "sub", "and") and ins.op_str.startswith("esp, ") and ins.operands[1].type == X86_OP_IMM:
        imm = ins.operands[1].imm
        if op == "and":
            # Only compare aligned frames when the pre-alignment expressions were identical.
            aligned = ((sp, imm), 0) if sp is not None and sp[0] == "entry" else None
            return aligned, bp
        return shift(sp, imm if op == "add" else -imm), bp
    written = {ins.reg_name(r) for r in ins.regs_access()[1]}
    return (None if {"esp", "sp"} & written else sp, None if {"ebp", "bp"} & written else bp)


def slots(insns, pop):
    if not insns:
        return {}
    code = {i.address: i for i in insns}
    entry = insns[0].address
    states = {entry: (("entry", 0), None)}
    pending = deque([entry])
    while pending:
        at = pending.popleft()
        ins = code[at]
        after = transfer(ins, states[at], pop)
        for dest in successors(ins):
            if dest not in code:
                continue
            merged = after if dest not in states else tuple(a if a == b else None for a, b in zip(states[dest], after))
            if states.get(dest) != merged:
                states[dest] = merged
                pending.append(dest)
    out = {}
    for at, (sp, bp) in states.items():
        for operand in code[at].operands:
            if operand.type == X86_OP_MEM and not operand.mem.index:
                reg = code[at].reg_name(operand.mem.base)
                if reg in ("esp", "ebp"):
                    out[at] = shift(sp if reg == "esp" else bp, operand.mem.disp)
    return out


def listing_names(text, symbol):
    names, offset, active = {}, None, False
    for line in text.splitlines():
        if not active:
            active = bool(re.match(re.escape(symbol) + r"\s+PROC\b", line))
            continue
        if re.match(re.escape(symbol) + r"\s+ENDP\b", line):
            break
        start = re.match(r"^\s{2}([0-9a-fA-F]{5,8})\s", line)
        if start:
            offset = int(start[1], 16)
        if offset is not None and not line.lstrip().startswith(";"):
            local = re.search(r"\b_([A-Za-z_]\w*)\$\d*(?:[+-]\d+)?\[(?:e[bs]p)", line, re.I)
            if local:
                names[offset] = local[1]
    return names


def fingerprint(obj):
    return [(sym.name, sec.data[lo:hi], [(r.offset-lo, r.type, obj.by_index[r.symbol].name)
            for r in sec.relocs if lo <= r.offset < hi]) for sym, sec, lo, hi in obj.functions()]


def diagnose(src, address, overlay, baseline):
    orig = check.Original()
    base = CoffObject(baseline)
    with tempfile.TemporaryDirectory(dir=check.ROOT / "build") as tmp:
        listing = Path(tmp) / "source.asm"
        obj, log = check.compile_source(src, overlay, out=Path(tmp) / "source.obj",
                                        extra=("/FAcs", "/Fa" + check.winpath(listing)))
        if not obj:
            raise ValueError(log)
        if fingerprint(base) != fingerprint(CoffObject(obj.read_bytes())):
            raise ValueError("listing changed code or relocations; stack focus unavailable")
        text = listing.read_text(encoding="latin-1")
    local, conflicts = check.bind_annotations(orig, base, baseline, src)
    if conflicts:
        raise ValueError("\n".join(conflicts))
    name = dict(check.annotations(src))[address]
    res = check.compare(orig, base, baseline, address, name, check.known_names(), local, prefer=local)
    sym, sec, lo, _ = next(f for f in base.functions() if f[0].name == res.symbol)
    ours = sec.data[lo:lo + res.ours]
    theirs = orig.read(address, res.size)
    linked = check.placeholder_linked(orig, base, sec, lo, ours, address, res.size)
    left, right = decode(theirs, address), decode(linked, address)
    a, b = check.normalised_listings(orig, base, sec, lo, ours, address, theirs)
    labels = listing_names(text, sym.name)
    targets = {}
    known = check.known_names()
    for rel in sec.relocs:
        off = rel.offset - lo
        if rel.type != check.REL_REL32 or not 0 <= off <= len(ours) - 4:
            continue
        target = base.by_index[rel.symbol]
        dest = local.get(target.name)
        if target.section == sec.number:
            dest = address - lo + target.value
        elif dest is None and check.qualified(target.name) in known and not check.overloaded(
                target.name, local, check.function_symbols(base)):
            dest = known[check.qualified(target.name)]
        if dest is not None:
            targets[address + off] = dest + struct.unpack_from("<i", ours, off)[0]
    cleanup = lru_cache(None)(lambda addr: return_cleanup(orig, addr))
    def pop(ins):
        return cleanup(ins.operands[0].imm) if ins.operands[0].type == X86_OP_IMM else None
    ls = slots(left, pop)
    rs = slots(right, lambda ins: cleanup(targets.get(ins.address + ins.imm_offset)))
    differences = []
    for block in difflib.SequenceMatcher(None, a, b, autojunk=False).get_matching_blocks():
        for i in range(block.size):
            old, new = left[block.a+i], right[block.b+i]
            l, r = ls.get(old.address), rs.get(new.address)
            label = labels.get(new.address-address)
            if label and l and r and l[0] == r[0] and l[1] != r[1]:
                differences.append((label, old.address, l[1], r[1]))
    return differences


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("source", type=Path)
    p.add_argument("address", type=lambda s: int(s, 16))
    p.add_argument("--overlay", type=Path)
    args = p.parse_args()
    path, log = check.compile_source(args.source, args.overlay)
    if not path:
        p.exit(1, log)
    for name, addr, expected, actual in diagnose(args.source, args.address, args.overlay, path.read_bytes()):
        print(f"0x{addr:08X} {name}: frame offset {actual:+d}, retail {expected:+d}")


if __name__ == "__main__":
    main()
