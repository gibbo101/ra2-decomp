import csv, bisect, collections
D="data"
fn=[(int(r["address"],16),int(r["size"])) for r in csv.DictReader(open(f"{D}/functions.csv"))]
done={int(r["address"],16) for r in csv.DictReader(open(f"{D}/progress.csv"))}
lib={int(r["address"],16) for r in csv.DictReader(open(f"{D}/library.csv"))}
tried={int(r["address"],16) for r in csv.DictReader(open(f"{D}/attempts.csv"))}
mods=[(int(r["first"],16),int(r["last"],16),r["module"]) for r in csv.DictReader(open(f"{D}/modules.csv"))]
firsts=[m[0] for m in mods]
c=collections.Counter(); cs=collections.Counter(); seg=collections.Counter(); tot=collections.Counter()
for a,s in fn:
    if a in lib: continue
    i=bisect.bisect_right(firsts,a)-1
    if i<0 or a>mods[i][1]: m="(none)"
    else: m=mods[i][2]
    k="seg" if m.startswith("seg_") else ("?" if m=="?" else ("none" if m=="(none)" else "named"))
    tot[k]+=1
    if a in done: c[k]+=1; continue
    if a in tried: continue
    seg[k]+=1
    if s<1536: cs[k]+=1
print("total",tot); print("done",c); print("untried",seg); print("untried<1536",cs)
import sys
b=collections.Counter(); bm=collections.Counter()
for a,s in fn:
    if a in lib or a in done or a in tried: continue
    i=bisect.bisect_right(firsts,a)-1
    m=mods[i][2] if i>=0 and a<=mods[i][1] else "(none)"
    if m.startswith("seg_") or m in("?","(none)"): continue
    b[min(s//256,8)]+=1; bm[m]+=1
print(sorted(b.items())); print(len(bm), bm.most_common(30))
