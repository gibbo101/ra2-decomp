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



done_by=collections.Counter()
for a in done:
    i=bisect.bisect_right(firsts,a)-1
    if i>=0 and a<=mods[i][1]: done_by[mods[i][2]]+=1
r=sorted(cnt.items(),key=lambda kv:-len(kv[1]))
print(len(done), sum(len(v) for v in cnt.values()), len(cnt))
for m,v in r[:int(sys.argv[1]) if len(sys.argv)>1 else 60]: print(f"{m:20} left={len(v):4} done={done_by[m]:4} small12={sum(s for a,s in sorted(v)[:12])}")
