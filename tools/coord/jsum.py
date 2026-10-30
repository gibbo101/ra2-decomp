import json,sys
for path in sys.argv[1:]:
    tot=m=0; miss=[]
    for line in open(path):
        r=json.loads(line)
        if r.get("type")!="result": continue
        v=r.get("result") or r.get("value") or {}
        if isinstance(v,str):
            try: v=json.loads(v)
            except: continue
        if "functions" in v and "module" in v:
            for f in v["functions"]:
                print(r.get("label",""), f["address"], f["result"], f["best_percent"], (f.get("blocker") or "")[:60])
