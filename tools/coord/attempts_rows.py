"""Append attempts.csv rows for a round's misses: python3 tools/coord/attempts_rows.py DATE JOURNAL...

Each journal is a workflow journal.jsonl. The last result per address wins (Opus over
Sonnet); addresses that progress.csv records as MATCH are skipped. The model comes from
each agent's label in the journal ("opus ...", "sonnet ...").
"""
import csv, json, re, sys

date, journals = sys.argv[1], sys.argv[2:]
done = {r["address"].upper() for r in csv.DictReader(open("data/progress.csv")) if r["result"] == "MATCH"}
known = {r["address"].upper() for r in csv.DictReader(open("data/functions.csv"))}
seen = set()
for r in csv.DictReader(open("data/attempts.csv")):
    if r["date"] == date:
        seen.add(r["address"].upper())

def clean(s):
    s = re.sub(r"/home/[^\s,;)]*/ra2-decomp/", "", s or "")
    s = re.sub(r"/(home|tmp)/[^\s,;)]*", "<path>", s)
    return s.replace("\n", " ")[:300]

MODEL = {"opus": "opus-5.5 high", "sonnet": "sonnet-5.5 medium"}

final = {}
for path in journals:
    labels = {}
    for line in open(path):
        d = json.loads(line)
        if d.get("type") == "started":
            labels[d.get("key")] = d.get("label", "")
        if d.get("type") != "result" or not isinstance(d.get("result"), dict):
            continue
        res = d["result"]
        agent = MODEL.get(labels.get(d.get("key"), "").split(" ")[0], "sonnet-5.5 medium")
        model = "opus-5.5 high (header merge)" if "landed" in res else None
        for f in res.get("functions", []):
            a = f["address"].upper().replace("0X", "0x")
            prev = final.get(a)
            m = model or agent
            if prev and prev["model"].startswith("sonnet") and agent.startswith("opus"):
                m = "sonnet-5.5 + opus-5.5"
            if prev and f["result"] != "MATCH" and f.get("checks", 0) == 0 and f["best_percent"] <= prev["best"]:
                continue
            final[a] = dict(model=m, result=f["result"], best=max(f["best_percent"], prev["best"] if prev else 0),
                            checks=f.get("checks", 0) + (prev["checks"] if prev else 0),
                            blocker=clean(f.get("blocker")), notes=clean(f.get("note")))

rows = []
for a, f in sorted(final.items()):
    if a.upper() in done or a.upper() in seen or a.upper() not in known:
        continue
    result = "partial" if f["result"] == "MATCH" else f["result"]
    blocker = f["blocker"] or ("matched in a banked file but not landed" if f["result"] == "MATCH" else "")
    rows.append([date, a, "", f["model"], result, f["best"], f["checks"], "", blocker, f["notes"]])
with open("data/attempts.csv", "a", newline="") as out:
    csv.writer(out, lineterminator="\n").writerows(rows)
print(len(rows), "rows appended")
