"""Write batchN.js/json for a round of grouped jobs over the named modules' untried small functions.

Usage: uv run python tools/coord/mkround.py TEMPLATE.js FIRST_BATCH NBATCHES [module=cap ...] [big=K]
A module with a cap gets one agent with that many functions; the rest are grouped, in name order,
about GROUP functions to an agent. Skips SKIP modules. An unnamed seg_ range joins the module
tools/coord/segfiles.json gives it (its src/ file), with that file's note. big=K adds last batches of K Opus jobs, eight to a batch, each
the smallest untried function of 1,536 bytes or more in a module no other job touches.
"""
import os, bisect, collections, csv, json, re, sys
from pathlib import Path

D = "data"
SKIP = {"vqalib/unvq", "_alpha"}
GROUP = 12
COORD = Path(__file__).resolve().parent
OUT = Path("build/coord")
OUT.mkdir(parents=True, exist_ok=True)

fn = [(int(r["address"], 16), int(r["size"])) for r in csv.DictReader(open(f"{D}/functions.csv"))]
done = {int(r["address"], 16) for r in csv.DictReader(open(f"{D}/progress.csv"))}
lib = {int(r["address"], 16) for r in csv.DictReader(open(f"{D}/library.csv"))}
tried = {int(r["address"], 16) for r in csv.DictReader(open(f"{D}/attempts.csv"))}
patch = [int(r["address"], 16) for r in csv.DictReader(open(f"{D}/exe_patches.csv"))]
mods = [(int(r["first"], 16), int(r["last"], 16), r["module"]) for r in csv.DictReader(open(f"{D}/modules.csv"))]
firsts = [m[0] for m in mods]
segfile = json.loads((Path(__file__).resolve().parent / "segfiles.json").read_text())
spelling = {m[2].lower(): m[2] for m in mods if not m[2].startswith("seg_")}
notes = collections.defaultdict(set)
for seg, v in segfile.items():
    for i, m in enumerate(mods):
        if m[2] == seg:
            mods[i] = (m[0], m[1], spelling.get(v["file"], v["file"]))
            notes[mods[i][2]].add(v["note"])
cnt = collections.defaultdict(list)
large = collections.defaultdict(list)
for a, s in fn:
    if a in done or a in lib or a in tried:
        continue
    if s >= 1536:
        i = bisect.bisect_right(firsts, a) - 1
        if i >= 0 and a <= mods[i][1] and mods[i][2] != "?" and not mods[i][2].startswith("seg_") and mods[i][2] not in SKIP:
            large[mods[i][2]].append((s, a))
        continue
    if any(a <= p < a + s for p in patch):
        continue
    i = bisect.bisect_right(firsts, a) - 1
    if i < 0 or a > mods[i][1] or mods[i][2] == "?" or mods[i][2].startswith("seg_") or mods[i][2] in SKIP:
        continue
    cnt[mods[i][2]].append(a)

template = Path(sys.argv[1]).read_text()
first, nb = int(sys.argv[2]), int(sys.argv[3])
caps = dict((k, int(v)) for k, v in (x.split("=") for x in sys.argv[4:]))
nbig = caps.pop("big", 0)
hx = lambda a: f"0x{a:08X}"
jobs = [{"name": m, "modules": {m: [hx(a) for a in sorted(cnt[m])[:c]]}} for m, c in caps.items() if cnt[m]]
rest = sorted((m for m in cnt if m not in caps), key=str.lower)
cur, n = {}, 0
for m in rest:
    cur[m] = [hx(a) for a in sorted(cnt[m])[:GROUP]]
    n += len(cur[m])
    if n >= GROUP:
        jobs.append({"name": "grp_" + re.sub(r"[^a-z0-9]", "", list(cur)[0].lower()), "modules": cur})
        cur, n = {}, 0
if cur:
    jobs.append({"name": "grp_" + re.sub(r"[^a-z0-9]", "", list(cur)[0].lower()), "modules": cur})

size = lambda j: sum(len(v) for v in j["modules"].values())
batches = [[] for _ in range(nb)]
for j in sorted(jobs, key=size, reverse=True):
    min(batches, key=lambda b: sum(map(size, b))).append(j)
for j in jobs:
    extra = " ".join(sorted(n for m in j["modules"] for n in notes[m]))
    if extra:
        j["note"] = extra
if nbig:
    taken = {m for j in jobs for m in j["modules"]}
    picks = sorted(min(v) + (m,) for m, v in large.items() if m not in taken)[:nbig]
    bigjobs = [{"name": m, "modules": {m: [hx(a)]}, "model": "opus", "effort": "high",
                "note": f"{hx(a)} is {s:,} bytes, larger than the small functions other batches take; "
                        "land it and any helpers it keeps out of line, and report them too."}
               for s, a, m in sorted(picks, key=lambda p: p[2].lower())]
    HEAP = (" wonline 0x00788180 and 0x007A3100, and preview's DrawMap 0x00640A40, are sensitive to the compiler's heap: "
            "any declaration added to or removed from a header they include (ownrdraw.h above all) or from wonline.cpp can "
            "flip them, so check all three after every such change and keep them MATCH.")
    for j in bigjobs:
        if set(j["modules"]) & {"wonline", "ownrdraw", "preview"}:
            j["note"] += HEAP
    for k in range(0, len(bigjobs), 8):
        batches.append(bigjobs[k:k + 8])
        nb += 1
for k, b in enumerate(batches):
    n = first + k
    others = [m for kk, bb in enumerate(batches) if kk != k for j in bb for m in j["modules"]]
    other_batches = ", ".join(str(first + kk) for kk in range(nb) if kk != k)
    total = sum(map(size, b))
    s = re.sub(r"name: 'batch-\d+-small-functions'", f"name: 'batch-{n}-small-functions'", template)
    together = f"alongside batch {other_batches}" if other_batches else "alone"
    if all(j.get("model") == "opus" for j in b):
        desc = (f"Opus on {total} functions over 1.5 KB in {len(b)} named modules, {together} into src/: "
                "Opus, a header merge, then Opus")
    else:
        desc = (f"Match {total} small functions in {len(b)} jobs over {sum(len(j['modules']) for j in b)} named modules, "
                f"{together} into src/: Sonnet, a header merge, then Opus")
    s = re.sub(r"description: '[^']*'", f"description: '{desc}'", s)
    s = s.replace("REPO_PATH", os.getcwd()).replace("OTHERS", ", ".join(others))
    s = s.replace("one agent per module, its 12 small unmatched functions", "one agent per module or group of small modules")
    (OUT / f"batch{n}.js").write_text(s)
    (OUT / f"batch{n}.json").write_text(json.dumps({"jobs": b}))
    print(n, len(b), "jobs", total, "functions:", ", ".join(f"{j['name']}({size(j)})" for j in b))
