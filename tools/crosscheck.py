"""Write data/crosscheck/discrepancies.md and .csv: our MATCH rows against gagarin's matched rows.

Usage: uv run tools/crosscheck.py  (needs reference/gagarin and llvm-undname-18)
"""
import bisect
import collections
import csv
import json
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import capstone  # noqa: E402
from check import Original, known_names  # noqa: E402
from xmanifest import Rtti  # noqa: E402

orig = Original()
names = known_names()
rtti = Rtti(orig, names.get("operator delete"))
retail = orig.pe.get_memory_mapped_image()

G = [e for e in json.load(open(ROOT / "reference/gagarin/scripts/data/match_manifest.json"))]
gm = {int(e["addr"], 16): e for e in G if e["status"] == "matched"}
gp = {int(e["addr"], 16): e for e in G if e["status"] == "partial"}
ours = {}
for r in csv.DictReader(open(ROOT / "data/progress.csv")):
    if r["result"] == "MATCH":
        ours.setdefault(int(r["address"], 16), r)
attempted = collections.defaultdict(list)
for r in csv.DictReader(open(ROOT / "data/attempts.csv")):
    attempted[int(r["address"], 16)].append(r)
starts = sorted(orig.sizes)
ov = sorted(set(ours) & set(gm))

# ---- undecorate everything once
syms = sorted({ours[a]["symbol"] for a in ov} | {gm[a]["sym"] for a in ov})
syms = [s for s in syms if s.startswith("?")]
und = {}
out = subprocess.run(["llvm-undname-18"], input="\n".join(syms) + "\n", capture_output=True, text=True).stdout
lines = out.split("\n")
k = 0
while k < len(lines) - 1:
    if lines[k] in und or not lines[k]:
        k += 1
        continue
    und[lines[k]] = lines[k + 1]
    k += 3

CONV = re.compile(r"\b(__thiscall|__fastcall|__cdecl|__stdcall)\b")


def parse(sym):
    s = und.get(sym)
    if not s or s.startswith("error"):
        return None
    m = CONV.search(s)
    if not m:
        return None
    head, tail = s[:m.start()], s[m.end():].strip()
    access = re.match(r"(?:\[thunk\]:)?(public|private|protected):", head)
    virtual = " virtual " in " " + head
    static = " static " in " " + head
    ret = re.sub(r"^(?:\[thunk\]:)?(public|private|protected):\s*", "", head)
    ret = re.sub(r"\b(virtual|static)\b", "", ret).strip()
    depth, i = 0, len(tail) - 1
    # parameters: the last balanced (...) group
    end = tail.rfind(")")
    j = end
    while j >= 0:
        depth += {")": 1, "(": -1}.get(tail[j], 0)
        if depth == 0:
            break
        j -= 1
    qname, params, after = tail[:j], tail[j + 1:end], tail[end + 1:].strip()
    return dict(access=access.group(1) if access else None, virtual=virtual, static=static, ret=ret,
                conv=m.group(1), name=qname, params=params, const="const" in after, text=s)


def kw(t):
    return re.sub(r"\b(class|struct|union|enum)\s+", "", t or "")


PH_CLASS = re.compile(r"_[0-9A-Fa-f]{6,8}$")
PH_NAME = re.compile(r"^(FUN|Obj|DAT|sub|thunk|LAB)_[0-9A-Fa-f]+$|^Slot\d+$|_[0-9A-Fa-f]{8}$")


def split_name(q):
    """(class, member) of a qualified name, address suffixes on class names removed."""
    parts = q.split("::") if q else [""]
    cls = "::".join(PH_CLASS.sub("", p) for p in parts[:-1])
    return cls, parts[-1]


def is_placeholder_class(q):
    parts = q.split("::")[:-1] if q else []
    return any(p.startswith("Obj_") or re.fullmatch(r"\w+_[0-9A-Fa-f]{6,8}", p) and not PH_CLASS.sub("", p).endswith("Class") and False
               for p in parts) or any(p.startswith("Obj_") for p in parts)


def placeholder(sym, name=""):
    if re.search(r"(FUN|sub|DAT)_[0-9A-Fa-f]{6,8}", sym) or re.search(r"(FUN|sub)_[0-9A-Fa-f]{6,8}", name):
        return True
    p = parse(sym)
    if p:
        cls, mem = split_name(p["name"])
        return bool(PH_NAME.search(mem)) or any(c.startswith("Obj_") for c in cls.split("::"))
    return False


def norm_style(n):
    return n.replace("_", "").lower()


def deleting_name(sym, p):
    return sym.startswith(("??_G", "??_E")) or (p and split_name(p["name"])[1] in ("DeletingDestructor", "ScalarDeletingDestructor", "VectorDeletingDestructor"))


def dtor_name(sym, p):
    return sym.startswith("??1") or (p and split_name(p["name"])[1] in ("Dtor", "Destructor"))


def owner_of(address):
    return rtti.owner(address), sorted(set(rtti.slot_of.get(address, [])))


def type_to_name(t):
    """.?AVFoo@@ -> Foo (for nested, Inner@Outer -> Outer::Inner; templates left decorated)."""
    body = t[4:-2]
    if body.startswith("?$"):
        r = subprocess.run(["llvm-undname-18", "??_7" + body + "@@6B@"], capture_output=True, text=True).stdout.split("\n")
        if len(r) > 1 and "vftable" in r[1]:
            return re.sub(r"\b(class|struct)\s+", "", r[1].split("::`vftable'")[0].replace("const ", ""))
        return body
    return "::".join(reversed(body.split("@")))


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

# ================= (a) lengths
lens = []
for a in ov:
    ol, tl = int(ours[a]["size"]), int(gm[a]["len"], 16)
    if ol == tl:
        continue
    short, long_ = min(ol, tl), max(ol, tl)
    code = orig.read(a, long_)
    insns = list(md.disasm(code, a))
    bounds = {i.address - a for i in insns} | {sum(i.size for i in insns)}
    nxt = starts[bisect.bisect_right(starts, a)]
    tail = code[short:]
    reason = ""
    if short not in bounds:
        ins = next(i for i in insns if i.address - a < short < i.address - a + i.size)
        reason = (f"the shorter span ends inside `{ins.mnemonic} {ins.op_str}` at +0x{ins.address - a:X}, "
                  f"cutting {ins.address - a + ins.size - short} of its {ins.size} bytes")
    else:
        tailins = [i for i in insns if i.address - a >= short]
        # jump/index tables: dwords that point back into the function
        dwords = [struct.unpack_from("<I", code, o)[0] for o in range((short + 3) // 4 * 4, long_ - 3, 4)]
        table = sum(1 for d in dwords if a <= d < a + long_)
        targets = [int(i.op_str, 16) for i in insns if i.address - a < short and i.mnemonic.startswith("j")
                   and re.fullmatch(r"0x[0-9a-f]+", i.op_str)]
        reach = [t for t in targets if a + short <= t < a + long_]
        if table >= 2:
            reason = (f"the bytes after +0x{short:X} are the function's jump table ({table} entries point into "
                      f"the function) and alignment/index bytes")
        elif reach:
            reason = (f"the bytes after +0x{short:X} are code the function branches to "
                      f"(jump to 0x{min(reach):08X} at {len(reach)} site(s)), ending in "
                      f"`{tailins[-1].mnemonic} {tailins[-1].op_str}`")
        else:
            prev = next(i for i in insns if i.address - a + i.size == short)
            if prev.mnemonic not in ("jmp", "ret"):
                reason = (f"the bytes after +0x{short:X} are code reached by falling through from "
                          f"`{prev.mnemonic} {prev.op_str}`: `{tailins[0].mnemonic} {tailins[0].op_str}`"
                          + (f" .. `{tailins[-1].mnemonic} {tailins[-1].op_str}`" if len(tailins) > 1 else ""))
            else:
                reason = f"unclassified; tail {tail[:12].hex()}"
    longer = "ours" if ol > tl else "theirs"
    lens.append(dict(a=a, ours=ol, theirs=tl, longer=longer, reason=reason, next=nxt,
                     osym=ours[a]["symbol"], tsym=gm[a]["sym"]))

# ================= (b)/(c) names
cats = collections.defaultdict(list)
for a in ov:
    o, t = ours[a]["symbol"], gm[a]["sym"]
    if o == t:
        cats["identical"].append(a)
        continue
    po, pt = parse(o), parse(t)
    oph, tph = placeholder(o), placeholder(t, gm[a]["name"])
    if tph or oph:
        cats["placeholder (theirs)" if tph and not oph else "placeholder (ours)" if oph and not tph
             else "placeholder (both)"].append(a)
        continue
    if po and pt and po["name"] == pt["name"] and "`" not in po["name"]:
        cats["same name, signature differs"].append(a)
        continue
    if deleting_name(o, po) and deleting_name(t, pt):
        oc = split_name(po["name"])[0] if po else ""
        tc = split_name(pt["name"])[0] if pt else ""
        cats["deleting destructor: naming convention" if oc == tc else "deleting destructor: class differs"].append(a)
        continue
    if dtor_name(o, po) and dtor_name(t, pt):
        oc = split_name(po["name"])[0] if po else ""
        tc = split_name(pt["name"])[0] if pt else ""
        cats["destructor: naming convention" if oc == tc else "destructor: class differs"].append(a)
        continue
    if not po or not pt:
        cats["unparsed"].append(a)
        continue
    oc, om = split_name(po["name"])
    tc, tm = split_name(pt["name"])
    if oc == tc or norm_style(oc) == norm_style(tc):
        if norm_style(om) == norm_style(tm):
            cats["style only (case/underscores)"].append(a)
        else:
            cats["different member name, same class"].append(a)
    else:
        cats["different class"].append(a)


def sig_diffs(o, t):
    po, pt = parse(o), parse(t)
    d = []
    if po["virtual"] != pt["virtual"]:
        d.append("virtual")
    if po["static"] != pt["static"]:
        d.append("static")
    if po["access"] != pt["access"]:
        d.append("access")
    if po["const"] != pt["const"]:
        d.append("const")
    if po["conv"] != pt["conv"]:
        d.append("convention")
    if kw(po["ret"]) != kw(pt["ret"]):
        d.append("return")
    elif po["ret"] != pt["ret"]:
        d.append("return keyword")
    if kw(po["params"]) != kw(pt["params"]):
        d.append("parameters")
    elif po["params"] != pt["params"]:
        d.append("class/struct keyword")
    return d


# ================= (d) theirs matched, ours not
theirs_only = sorted(set(gm) - set(ours))

# ================= write the report data
report = dict(lens=lens, cats={k: v for k, v in cats.items()}, theirs_only=theirs_only)
json.dump({"lens": lens, "cats": {k: [f"0x{a:08X}" for a in v] for k, v in cats.items()},
           "theirs_only": [f"0x{a:08X}" for a in theirs_only]},
          open(ROOT / "build/crosscheck.json", "w"), indent=1)


def sha_file(path):
    import hashlib
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def class_verdicts(a):
    """(exe owner type, ours ok, theirs ok) for a vtable slot; None when the exe is silent."""
    owner, holders = owner_of(a)
    if not holders:
        return None
    oc = rtti.class_of(ours[a]["symbol"])[0]
    tc = rtti.class_of(gm[a]["sym"])[0]
    return owner, holders, oc, tc, (oc == owner) if owner else None, (tc == owner) if owner else None


def short(sym, n=70):
    return sym if len(sym) <= n else sym[:n - 1] + "…"


def main():
    sys.path.insert(0, str(ROOT / "tools"))
    from xmanifest import git_commit
    out_md = ROOT / "data/crosscheck/discrepancies.md"
    out_csv = ROOT / "data/crosscheck/discrepancies.csv"
    cat_of = {a: k for k, v in cats.items() for a in v}
    rows = []
    cv = {}
    for a in ov:
        v = class_verdicts(a)
        cv[a] = v
        vo = rtti.virtual_verdict(ours[a]["symbol"], a)
        vt = rtti.virtual_verdict(gm[a]["sym"], a)
        diffs = ",".join(sig_diffs(ours[a]["symbol"], gm[a]["sym"])) if cat_of[a] == "same name, signature differs" else ""
        rows.append([f"0x{a:08X}", cat_of[a], diffs, ours[a]["size"], int(gm[a]["len"], 16),
                     ours[a]["symbol"], gm[a]["sym"],
                     v[0] if v and v[0] else ("" if not v else "|".join(v[1])),
                     "" if not v or v[4] is None else ("yes" if v[4] else "no"),
                     "" if not v or v[5] is None else ("yes" if v[5] else "no"), vo, vt])
    with out_csv.open("w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["address", "category", "signature_differences", "our_size", "their_len", "our_symbol",
                    "their_symbol", "exe_slot_class", "our_class_per_exe", "their_class_per_exe",
                    "our_virtual_per_exe", "their_virtual_per_exe"])
        w.writerows(rows)

    L = []
    P = L.append
    P("# Crosscheck discrepancies: ra2-decomp and gagarin")
    P("")
    P("Every function both projects report as matched, compared by address. Generated from the inputs")
    P("below and checked against the executable; where the exe settles a difference, the side it")
    P("supports is named with the evidence. `discrepancies.csv` has one row per overlapping function.")
    P("")
    P("## Inputs")
    P("")
    P(f"- ra2-decomp: commit `{git_commit(ROOT)}` plus uncommitted work; `data/progress.csv` "
      f"sha256 `{sha_file(ROOT / 'data/progress.csv')}`, MATCH rows only.")
    P(f"- gagarin: commit `{git_commit(ROOT / 'reference/gagarin')}`, "
      f"`scripts/data/match_manifest.json` sha256 "
      f"`{sha_file(ROOT / 'reference/gagarin/scripts/data/match_manifest.json')}`, rows with status `matched`.")
    P("- Executable: ours is the Steam `gamemd.exe`, 5,286,208 bytes, sha256")
    P("  `3e81a61775d2745d1dabe397325ef663cd994ffc194da4e998e3bf5d2d308600`. gagarin pins sha256")
    P("  `1cdd1180e49024fbda8ad568caac2e86e856063ff67ab38f62b7d2c7bb84298c`, which is neither our Steam")
    P("  copy nor The First Decade's (`258aa247…`). All 1.001 builds share one link timestamp, and")
    P("  Steam's code differs from TFD's in 118 bytes across 12 functions (the CD check, recorded in")
    P("  `data/exe_patches.csv`), so per-function hashes should agree outside such sites. **Open")
    P("  question for gagarin: which build is pinned (file size, MD5, CRC-32)?** Every exe fact below")
    P("  was read from the Steam copy.")
    P("")
    P("## Counts")
    P("")
    P("| | functions |")
    P("|---|---:|")
    P(f"| ours, MATCH (distinct addresses) | {len(ours):,} |")
    P(f"| theirs, matched | {len(gm):,} |")
    P(f"| theirs, partial | {len(gp):,} |")
    P(f"| overlap: both matched | {len(ov):,} |")
    P(f"| theirs partial that we match | {len(set(gp) & set(ours)):,} |")
    P(f"| (a) length differs | {len(lens)} |")
    P(f"| (b) names or signatures differ, both real | "
      f"{sum(len(cats[k]) for k in cats if not k.startswith(('placeholder', 'identical')))} |")
    P(f"| (c) one side a placeholder | {sum(len(v) for k, v in cats.items() if k.startswith('placeholder'))} |")
    P(f"| identical decorated symbol | {len(cats['identical'])} |")
    P(f"| (d) theirs matched, ours not | {len(theirs_only):,} |")
    P("")

    # (a)
    kinds = collections.Counter("jump table" if "jump table" in l["reason"] else
                                "inside last instruction" if "ends inside" in l["reason"] else
                                "reachable code" for l in lens)
    P("## (a) Length differs")
    P("")
    P(f"{len(lens)} functions; in every one our span is the longer. Our size is the span our compiled")
    P("function covers, padding stripped, and the checker found every byte of it equal to the exe's,")
    P("jump-table entries included as relocations to labels inside the function. Read against the exe")
    P("bytes, each shorter span stops before the function ends:")
    P("")
    P(f"- {kinds['jump table']}: theirs stops where the jump table (and any byte index table) begins.")
    P("  VC6 emits these tables in the function's own section right after its code, so they belong to")
    P("  the function; their match covers the code only.")
    P(f"- {kinds['inside last instruction']}: theirs stops inside the final `ret imm16`, leaving out its")
    P("  last 2 bytes; the instruction, and the function, end 2 bytes later.")
    P(f"- {kinds['reachable code']}: theirs stops before code the function still reaches (a branch")
    P("  target, or a final `ret` reached by falling through).")
    P("")
    P("| address | ours | theirs | what follows the shorter span (exe bytes) |")
    P("|---|---:|---:|---|")
    for l in lens:
        P(f"| 0x{l['a']:08X} | {l['ours']} | {l['theirs']} | {l['reason']} |")
    P("")

    # (b)
    P("## (b) Different real names or signatures")
    P("")
    P("### Settled by the exe: virtual or not")
    P("")
    P("A virtual function is a slot of its class's vtable (found through the RTTI complete object")
    P("locator before each vtable); a class's destructor is virtual exactly when its vtable holds a")
    P("deleting destructor (tests bit 0 of its flag, calls `operator delete`).")
    P("")
    vrows = [r for r in rows if r[1] == "same name, signature differs" and "virtual" in r[2]]
    P(f"{len(vrows)} same-name pairs differ in `virtual`. The exe supports ours in "
      f"{sum(1 for r in vrows if r[10] == 'verified' and r[11] == 'mismatch')} and theirs in "
      f"{sum(1 for r in vrows if r[11] == 'verified' and r[10] == 'mismatch')}:")
    P("")
    P("| address | ours | theirs | evidence |")
    P("|---|---|---|---|")
    for r in vrows:
        a = int(r[0], 16)
        po, pt = parse(r[5]), parse(r[6])
        ev = ("slot of " + ", ".join(sorted(set(rtti.slot_of.get(a, []))))[:90] if a in rtti.slot_of
              else "class vtable holds a deleting destructor" if r[5].startswith("??1") else "")
        P(f"| {r[0]} | `{short(r[5], 60)}` | `{short(r[6], 60)}` | {ev} |")
    P("")
    P("### Settled by the exe: which class a virtual belongs to")
    P("")
    P("For a function that is a vtable slot, the owning class is the class whose vtable holds it and")
    P("from which every other holder derives (RTTI class hierarchy descriptors). Each side's class is")
    P("compared by its decorated name with the RTTI type name.")
    P("")
    tied = [a for a in ov if cv[a] and cv[a][0]]
    both = [a for a in tied if cv[a][4] and cv[a][5]]
    o_only = [a for a in tied if cv[a][4] and not cv[a][5]]
    t_only = [a for a in tied if cv[a][5] and not cv[a][4]]
    neither = [a for a in tied if not cv[a][4] and not cv[a][5]]
    t_wrong_real = [a for a in o_only if cv[a][3] is not None]
    o_wrong_real = [a for a in t_only if cv[a][2] is not None]
    P(f"- {len(tied):,} overlapping functions are slots with one owning class.")
    P(f"- Both sides name the owner: {len(both):,}.")
    P(f"- Only ours does: {len(o_only)}; of these, theirs names a different class that exists in the exe's")
    P(f"  RTTI in {len(t_wrong_real)} (listed below); the rest use a class name the RTTI does not have")
    P("  (a placeholder or another spelling).")
    P(f"- Only theirs does: {len(t_only)}; of these, ours names a different RTTI class in {len(o_wrong_real)}"
      " (listed below), nearly all template instances whose element type or")
    P("  `VectorClass`/`DynamicVectorClass` level we labelled wrongly; the rest use a class name the RTTI")
    P("  does not have (for example our `MPGameModeClass` where the exe's RTTI says `MultiplayerGameMode`).")
    P(f"- Neither does: {len(neither)}.")
    P("")
    P("Theirs names another RTTI class (the exe supports ours):")
    P("")
    P("| address | exe owner | ours | theirs |")
    P("|---|---|---|---|")
    for a in t_wrong_real:
        P(f"| 0x{a:08X} | `{cv[a][0]}` | `{short(ours[a]['symbol'], 55)}` | `{short(gm[a]['sym'], 55)}` |")
    P("")
    fam = collections.Counter("template" if "?$" in ours[a]["symbol"] else "other" for a in o_wrong_real)
    P(f"Ours names another RTTI class (the exe supports theirs): {len(o_wrong_real)}, of which "
      f"{fam['template']} are template instances (wrong element type, or `VectorClass` and")
    P("`DynamicVectorClass` swapped; the code is identical for any pointer type, so the bytes matched")
    P("under the wrong label). All are in the CSV (`our_class_per_exe` = `no`); the non-template ones")
    P("and the first 12 template ones:")
    P("")
    P("| address | exe owner | ours | theirs |")
    P("|---|---|---|---|")
    shown = [a for a in o_wrong_real if "?$" not in ours[a]["symbol"]] + \
        [a for a in o_wrong_real if "?$" in ours[a]["symbol"]][:12]
    for a in sorted(shown):
        P(f"| 0x{a:08X} | `{cv[a][0]}` | `{short(ours[a]['symbol'], 55)}` | `{short(gm[a]['sym'], 55)}` |")
    P("")
    P("Neither names the owner (first 40):")
    P("")
    P("| address | exe owner | ours | theirs |")
    P("|---|---|---|---|")
    for a in neither[:40]:
        P(f"| 0x{a:08X} | `{cv[a][0]}` | `{short(ours[a]['symbol'], 55)}` | `{short(gm[a]['sym'], 55)}` |")
    P("")
    def nonvirtual(sym):
        p = parse(sym)
        return p is None or not p["virtual"]
    o_nv = [r for r in rows if r[10] == "mismatch" and nonvirtual(r[5])]
    t_nv = [r for r in rows if r[11] == "mismatch" and nonvirtual(r[6])]
    P(f"A free or non-virtual symbol at a slot of one class hierarchy is settled too: a slot is a virtual. "
      f"Ours names {len(o_nv)} such functions,")
    P(f"theirs {len(t_nv)} (CSV column `*_virtual_per_exe` = `mismatch`). Ours:")
    P("")
    P("| address | exe slot owner | ours | theirs |")
    P("|---|---|---|---|")
    for r in o_nv:
        a = int(r[0], 16)
        P(f"| {r[0]} | `{rtti.owner(a)}` | `{short(r[5], 55)}` | `{short(r[6], 55)}` |")
    P("")
    P("### Not settled by bytes")
    P("")
    sd = collections.Counter(d for r in rows if r[2] for d in r[2].split(","))
    P(f"{len(cats['same name, signature differs'])} pairs share a qualified name and differ in the signature. "
      "Kinds (a pair can have several):")
    P("")
    for k, n in sd.most_common():
        P(f"- {k}: {n}")
    P("")
    P("Access (`private`/`public`), `const`, and `class` against `struct` change no code, and parameter")
    P("types of the same size and passing (`int` and `unsigned`, a pointer to one type or another) change")
    P("none either, so the bytes cannot choose; callers' use and RTTI names are the evidence to bring.")
    P("Examples:")
    P("")
    P("| address | differs in | ours | theirs |")
    P("|---|---|---|---|")
    for r in [r for r in rows if r[2] and "virtual" not in r[2]][:25]:
        P(f"| {r[0]} | {r[2]} | `{short(r[5], 55)}` | `{short(r[6], 55)}` |")
    P("")
    P("Different names for the same function, both real:")
    P("")
    for k in ("different member name, same class", "different class", "style only (case/underscores)",
              "deleting destructor: class differs", "deleting destructor: naming convention",
              "destructor: class differs", "destructor: naming convention", "unparsed"):
        P(f"- {k}: {len(cats.get(k, []))}")
    P("")
    P("Most are naming conventions: ours keeps Westwood's names from the TibSun source where it has them")
    P("(`INIClass::Put_Bool`, `LogicClass::Detach`), theirs follows YRpp (`WriteBool`,")
    P("`PointerGotInvalid`); deleting destructors are `??_G` in ours and `DeletingDestructor` members in")
    P("theirs. Class names that differ are listed by pair in the CSV. Where the RTTI has the class, it is")
    P("Westwood's own name and settles the spelling (section above).")
    P("")

    # (c)
    P("## (c) Placeholder against a name")
    P("")
    for k in ("placeholder (theirs)", "placeholder (ours)", "placeholder (both)"):
        P(f"- {k}: {len(cats[k])}")
    P("")
    P("Not discrepancies. A sample:")
    P("")
    P("| address | ours | theirs |")
    P("|---|---|---|")
    for k in ("placeholder (theirs)", "placeholder (ours)"):
        for a in cats[k][:6]:
            P(f"| 0x{a:08X} | `{short(ours[a]['symbol'], 60)}` | `{short(gm[a]['sym'], 60)}` |")
    P("")

    # EA's post-link patches
    patches = [(int(r["address"], 16), r["original"], r["patched"])
               for r in csv.DictReader(open(ROOT / "data/exe_patches.csv"))]
    hit = sorted({a for a, e in gm.items() for p, _, _ in patches
                  if a <= p < a + max(int(e["len"], 16), orig.sizes.get(a, 0))})
    P("## Rows that match EA's post-link patches")
    P("")
    P(f"{len(hit)} of their matched rows cover only a stub that EA wrote over Westwood's code after")
    P("linking (`data/exe_patches.csv`: the Steam copy's CD check, which The First Decade's exe holds")
    P("as compiler-shaped code). Their lengths are the stub's, so their pinned exe carries the same")
    P("patches, and these rows match EA's bytes rather than a compiled function:")
    P("")
    P("| address | their len | exe bytes in their span | function size in TFD's code | their symbol |")
    P("|---|---:|---|---:|---|")
    for a in hit:
        stub = retail[a - orig.base:a - orig.base + int(gm[a]["len"], 16)].hex()
        P(f"| 0x{a:08X} | {int(gm[a]['len'], 16)} | `{stub}` | {orig.sizes.get(a, '')} | `{short(gm[a]['sym'], 60)}` |")
    P("")

    # (d)
    P("## (d) Matched by them, not by us")
    P("")
    tried = sum(1 for a in theirs_only if a in attempted)
    P(f"{len(theirs_only):,} addresses; every one is a function start in our `data/functions.csv`. "
      f"{tried} have a recorded attempt of ours (`data/attempts.csv`). The 20 smallest:")
    P("")
    P("| address | their len | our size | their symbol | our attempts (best %) |")
    P("|---|---:|---:|---|---|")
    small = sorted(theirs_only, key=lambda a: (int(gm[a]["len"], 16), a))[:20]
    for a in small:
        at = attempted.get(a, [])
        best = max((float(r["best"]) for r in at if r["best"]), default=None)
        P(f"| 0x{a:08X} | {int(gm[a]['len'], 16)} | {orig.sizes.get(a, '')} | `{short(gm[a]['sym'], 60)}` | "
          + (f"{len(at)} ({best:g})" if at else "none") + " |")
    P("")
    out_md.write_text("\n".join(L) + "\n")
    print(f"wrote {out_md} and {out_csv}")


if __name__ == "__main__":
    main()
