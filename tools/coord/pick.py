import csv, bisect, collections, json, sys
D="data"
fn=[(int(r["address"],16),int(r["size"])) for r in csv.DictReader(open(f"{D}/functions.csv"))]
done={int(r["address"],16) for r in csv.DictReader(open(f"{D}/progress.csv"))}
lib={int(r["address"],16) for r in csv.DictReader(open(f"{D}/library.csv"))}
patch=[int(r["address"],16) for r in csv.DictReader(open(f"{D}/exe_patches.csv"))]
tried={int(r["address"],16) for r in csv.DictReader(open(f"{D}/attempts.csv"))}
mods=[(int(r["first"],16),int(r["last"],16),r["module"]) for r in csv.DictReader(open(f"{D}/modules.csv"))]
firsts=[m[0] for m in mods]
cnt=collections.defaultdict(list)
for a,s in fn:
    if a in done or a in lib or a in tried or s>=1536: continue
    if any(a<=p<a+s for p in patch): continue
    i=bisect.bisect_right(firsts,a)-1
    if i<0 or a>mods[i][1] or mods[i][2]=="?": continue
    cnt[mods[i][2]].append((a,s))
want=sys.argv[1].split(",")
out={m:[f"0x{a:08X}" for a,s in sorted(cnt[m])[:12]] for m in want}
print(json.dumps(out))
