"""Reconcile a round's claimed matches with the tree: python3 tools/coord/tally.py [--gate GATE.txt] JOURNAL...

Lists every address a job or merge reported as MATCH and sorts it into: new (annotated in
src/ and not MATCH in HEAD's progress.csv), already recorded, or not in src/ (banked only).
With --gate, a new address the gate output does not print as MATCH is flagged.
"""
import csv, io, json, re, subprocess, sys
from pathlib import Path

norm = lambda a: "0x" + a[2:].upper()

args = sys.argv[1:]
gate = None
if args[:1] == ["--gate"]:
    gate, args = Path(args[1]).read_text(), args[2:]
head = subprocess.run(["git", "show", "HEAD:data/progress.csv"], capture_output=True, text=True).stdout
before = {norm(r["address"]) for r in csv.DictReader(io.StringIO(head)) if r["result"] == "MATCH"}
marked = set()
for p in Path("src").rglob("*.cpp"):
    marked |= {norm(a) for a in re.findall(r"// (?:FUNCTION|TEMPLATE|SYNTHETIC): GAMEMD (0x[0-9A-Fa-f]+)", p.read_text(errors="replace"))}

claims = {}
for path in args:
    labels = {}
    for line in open(path):
        d = json.loads(line)
        if d.get("type") == "started":
            labels[d.get("key")] = d.get("label", "")
        if d.get("type") != "result":
            continue
        v = d.get("result") or {}
        if isinstance(v, str):
            try: v = json.loads(v)
            except ValueError: continue
        who = "merge" if "landed" in v else labels.get(d.get("key"), "?")
        for f in v.get("functions", []):
            if f.get("result") == "MATCH":
                claims.setdefault(norm(f["address"]), who)
        for item in v.get("landed", []):
            m = re.match(r"(0x[0-9A-Fa-f]{6,8})", str(item))
            if m:
                claims.setdefault(norm(m.group(1)), who)

groups = {"new": [], "already recorded": [], "not in src/": []}
for a, who in sorted(claims.items()):
    if a in before:
        groups["already recorded"].append((a, who))
    elif a in marked:
        groups["new"].append((a, who))
    else:
        groups["not in src/"].append((a, who))
for g, items in groups.items():
    print(f"{g}: {len(items)}")
    for a, who in items:
        flag = ""
        if g == "new" and gate is not None and not re.search(rf"^{a}\s[^\n]*: MATCH", gate, re.M | re.I):
            flag = "  <- not MATCH in the gate"
        print(f"  {a}  {who}{flag}")
