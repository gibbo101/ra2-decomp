"""Write batchN.js/json for a near-miss round: unmatched game functions whose best attempt is >= MIN%.

Usage: python3 tools/coord/mknear.py TEMPLATE.js FIRST_BATCH NBATCHES MIN [PER_JOB [MAX]]
MAX, if given, keeps only near misses whose best is below it.
One Opus job per module with PER_JOB or more near misses, smaller modules grouped to about PER_JOB.
"""
import bisect, collections, csv, glob, json, os, re, sys
from pathlib import Path

D = "data"
COORD = Path(__file__).resolve().parent
OUT = Path("build/coord")
OUT.mkdir(parents=True, exist_ok=True)
SKIP = {"vqalib/unvq", "_alpha", "?"}
template = Path(sys.argv[1]).read_text()
first, nb, MIN = int(sys.argv[2]), int(sys.argv[3]), float(sys.argv[4])
PER = int(sys.argv[5]) if len(sys.argv) > 5 else 6
MAX = float(sys.argv[6]) if len(sys.argv) > 6 else 1000.0
done = {int(r["address"], 16) for r in csv.DictReader(open(f"{D}/progress.csv"))}
lib0 = min(int(r["address"], 16) for r in csv.DictReader(open(f"{D}/library.csv")))
size = {int(r["address"], 16): int(r["size"]) for r in csv.DictReader(open(f"{D}/functions.csv"))}
mods = [(int(r["first"], 16), int(r["last"], 16), r["module"]) for r in csv.DictReader(open(f"{D}/modules.csv"))]
segfile = json.loads((COORD / "segfiles.json").read_text())
spelling = {m[2].lower(): m[2] for m in mods if not m[2].startswith("seg_")}
for i, m in enumerate(mods):
    if m[2] in segfile:
        mods[i] = (m[0], m[1], spelling.get(segfile[m[2]]["file"], segfile[m[2]]["file"]))
firsts = [m[0] for m in mods]
best = {}
for r in csv.DictReader(open(f"{D}/attempts.csv")):
    a = int(r["address"], 16)
    try: b = float(r["best"] or 0)
    except ValueError: continue
    if a not in best or b > best[a][0]:
        best[a] = (b, r["blocker"].strip(), r["notes"].strip())
banks = collections.defaultdict(list)
for p in glob.glob("build/batch/*/0x*.cpp"):
    try: banks[int(os.path.basename(p)[:-4], 16)].append(p)
    except ValueError: pass
hx = lambda a: f"0x{a:08X}"
per = collections.defaultdict(list)
desc = {}
for a, (b, blk, note) in best.items():
    if a in done or a >= lib0 or b < MIN or b >= MAX or a not in size:
        continue
    i = bisect.bisect_right(firsts, a) - 1
    m = mods[i][2] if i >= 0 and a <= mods[i][1] else "?"
    if m in SKIP or m.startswith("seg_"):
        continue
    per[m].append(a)
    d = f"{hx(a)} ({size[a]} B, best {b:g}%"
    if blk: d += f", blocker: {blk[:160]}"
    if note: d += f"; {note[:240]}"
    d += f"; banked: {', '.join(banks[a])}" if banks[a] else "; no banked attempt"
    if b >= 100: d += "; it reached 100% without landing: settle why (label, twin or overlay) with evidence"
    desc[a] = d + ")"
WONLINE = (" wonline 0x00788180 is sensitive to the compiler's heap: any declaration added to or removed from wonline.cpp "
           "or a header it includes can flip it, so check it after every such change and keep it MATCH.")
jobs, cur, n = [], {}, 0
for m in sorted(per, key=lambda m: (-len(per[m]), m.lower())):
    v = sorted(per[m])
    if len(v) >= PER:
        jobs.append({"name": m, "modules": {m: [hx(a) for a in v]}})
        continue
    cur[m] = [hx(a) for a in v]; n += len(v)
    if n >= PER:
        jobs.append({"name": "near_" + re.sub(r"[^a-z0-9]", "", list(cur)[0].lower()), "modules": cur}); cur, n = {}, 0
if cur:
    jobs.append({"name": "near_" + re.sub(r"[^a-z0-9]", "", list(cur)[0].lower()), "modules": cur})
for j in jobs:
    addrs = [int(a, 16) for v in j["modules"].values() for a in v]
    j.update(model="opus", effort="high",
             note="These are near misses from earlier rounds: " + "; ".join(desc[a] for a in sorted(addrs)) +
                  ". Start from the banked attempt where there is one, keep what it proved, and find the last difference. "
                  "One that matches only when forced out of line (a probe, an explicit instantiation, a pragma) or through a stand-in "
                  "does not land: name the real code it waits for, and land that code too if it is small and in a module you own."
                  + (WONLINE if "wonline" in j["modules"] else ""))
sz = lambda j: sum(len(v) for v in j["modules"].values())
batches = [[] for _ in range(nb)]
for j in sorted(jobs, key=sz, reverse=True):
    min(batches, key=lambda b: (sum(map(sz, b)), len(b))).append(j)
for k, b in enumerate(batches):
    n = first + k
    others = [m for kk, bb in enumerate(batches) if kk != k for j in bb for m in j["modules"]]
    other_batches = ", ".join(str(first + kk) for kk in range(nb) if kk != k)
    total = sum(map(sz, b))
    s = re.sub(r"name: 'batch-\d+-small-functions'", f"name: 'batch-{n}-near-misses'", template)
    s = re.sub(r"description: '[^']*'",
               f"description: 'Finish {total} near misses (best {MIN:g}% or more{f" and under {MAX:g}%" if MAX < 1000 else ""}) in {len(b)} Opus jobs over "
               f"{sum(len(j['modules']) for j in b)} modules, alongside batch {other_batches} into src/, then a header merge'", s)
    s = s.replace("REPO_PATH", os.getcwd()).replace("OTHERS", ", ".join(others))
    (OUT / f"batch{n}.js").write_text(s)
    (OUT / f"batch{n}.json").write_text(json.dumps({"jobs": b}))
    print(n, len(b), "jobs", total, "functions:", ", ".join(f"{j['name']}({sz(j)})" for j in b))
