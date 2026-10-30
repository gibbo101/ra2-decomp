"""List unmatched game functions whose best attempt is >= MIN percent, with module and banked file."""
import bisect, csv, glob, os, sys, collections
D = "data"
MIN = float(sys.argv[1]) if len(sys.argv) > 1 else 98
done = {int(r["address"], 16) for r in csv.DictReader(open(f"{D}/progress.csv"))}
lib0 = min(int(r["address"], 16) for r in csv.DictReader(open(f"{D}/library.csv")))
size = {int(r["address"], 16): int(r["size"]) for r in csv.DictReader(open(f"{D}/functions.csv"))}
mods = [(int(r["first"], 16), int(r["last"], 16), r["module"]) for r in csv.DictReader(open(f"{D}/modules.csv"))]
firsts = [m[0] for m in mods]
best = {}
for r in csv.DictReader(open(f"{D}/attempts.csv")):
    a = int(r["address"], 16)
    try: b = float(r["best"] or 0)
    except ValueError: continue
    if a not in best or b > best[a][0]:
        best[a] = (b, r["blocker"], r["notes"], r["function"])
banks = collections.defaultdict(list)
for p in glob.glob("build/batch/*/0x*.cpp") + glob.glob("build/batch/*/0X*.cpp"):
    try: banks[int(os.path.basename(p)[:-4], 16)].append(p)
    except ValueError: pass
for a in sorted(best):
    b = best[a]
    if a in done or a >= lib0 or b[0] < MIN or a not in size: continue
    i = bisect.bisect_right(firsts, a) - 1
    m = mods[i][2] if i >= 0 and a <= mods[i][1] else "?"
    print(f"0x{a:08X}\t{size[a]}\t{b[0]}\t{m}\t{';'.join(banks.get(a, [])) or '-'}\t{b[3]}\t{b[1]}")
