import struct, sys
sys.argv, args = sys.argv[:1], sys.argv[1:]
exec(open("tools/coord/vtowner.py").read().split("for t in sys.argv")[0])
for t in args:
    vt = int(t, 16); col = rd(vt - 4)
    print(f"0x{vt:08X}:", name(col) if col else "?")
