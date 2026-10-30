import struct, sys
data = open("orig/gamemd.exe", "rb").read()
pe = struct.unpack_from("<I", data, 0x3C)[0]
nsec = struct.unpack_from("<H", data, pe + 6)[0]
optsz = struct.unpack_from("<H", data, pe + 20)[0]
base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + optsz + 40 * i
    va, rsz, raw = struct.unpack_from("<I", data, o + 12)[0], struct.unpack_from("<I", data, o + 16)[0], struct.unpack_from("<I", data, o + 20)[0]
    secs.append((base + va, rsz, raw))
def off(v):
    for va, sz, raw in secs:
        if va <= v < va + sz: return raw + v - va
def rd(v): o = off(v); return struct.unpack_from("<I", data, o)[0] if o is not None else None
def name(col):
    td = rd(col + 12)
    if td is None: return "?"
    o = off(td + 8); return data[o:data.index(b"\0", o)].decode()
for t in sys.argv[1:]:
    t = int(t, 16); pat = struct.pack("<I", t); res = []
    i = data.find(pat)
    while i >= 0:
        # walk back to vtable start: the slot preceded by a COL pointer
        for va, sz, raw in secs:
            if raw <= i < raw + sz: v = va + i - raw
        s = v
        for _ in range(64):
            col = rd(s - 4)
            if col and rd(col) == 0 and off(col + 12) is not None and rd(col+12) and off(rd(col+12)+8):
                n = name(col)
                if n.startswith(".?A"): res.append(f"{n} vt=0x{s:08X} slot={(v-s)//4}"); break
            s -= 4
        i = data.find(pat, i + 1)
    print(f"0x{t:08X}:", "; ".join(res) or "no vtable")
