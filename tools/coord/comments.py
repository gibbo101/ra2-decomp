import subprocess, re, sys
files = subprocess.run("(git diff HEAD --name-only -- src include; git ls-files --others --exclude-standard -- src include) | sort -u", shell=True, capture_output=True, text=True).stdout.split()
MARK = re.compile(r"^\s*// (FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY|GLOBAL|VTABLE|STRING|SYMBOL)|^\s*// [?_]")
def runs(text):
    out, cur = [], []
    for i, l in enumerate(text.splitlines()):
        if i < 4: continue
        if l.strip().startswith("//") and not MARK.match(l): cur.append(l.strip()); continue
        if len(cur) >= 3: out.append(tuple(cur))
        cur = []
    if len(cur) >= 3: out.append(tuple(cur))
    return out
for f in files:
    try: new = open(f).read()
    except FileNotFoundError: continue
    old = subprocess.run(["git", "show", f"HEAD:{f}"], capture_output=True, text=True).stdout
    o = set(runs(old))
    for r in runs(new):
        if r not in o: print(f, "|", " / ".join(r)[:300])
