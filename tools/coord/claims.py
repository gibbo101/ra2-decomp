"""Function addresses that outside work has claimed, so a round never picks them.

Usage: python3 tools/coord/claims.py   (lists the claims; the generators import claimed())
A claim is a function an open pull request adds a marker or a MATCH progress row for, or an
address or range (0xAAAAAAAA-0xBBBBBBBB) in a comment on an open issue labelled "claims".
Fails rather than returning nothing when GitHub cannot be read.
"""
import csv, json, re, subprocess, sys

MARKER = re.compile(r"^\+\s*// (?:FUNCTION|SYNTHETIC|TEMPLATE|LIBRARY): GAMEMD (0x[0-9A-Fa-f]{8})", re.M)
ROW = re.compile(r"^\+(0x[0-9A-Fa-f]{8}),[^\n]*,MATCH,", re.M)
SPAN = re.compile(r"\b(0x[0-9A-Fa-f]{8})(?:\s*(?:[-\u2013\u2014]|\.\.)\s*(0x[0-9A-Fa-f]{8}))?\b")
LABEL = "claims"


def gh(*args):
    out = subprocess.run(["gh", *args], capture_output=True, text=True)
    if out.returncode:
        sys.exit(f"claims.py: gh {' '.join(args[:2])} failed: {out.stderr.strip()}")
    return out.stdout


def sources():
    """(label, set of addresses) for each open pull request and claims issue."""
    starts = sorted(int(r["address"], 16) for r in csv.DictReader(open("data/functions.csv")))
    out = []
    for pr in json.loads(gh("pr", "list", "--state", "open", "--limit", "200", "--json", "number,author")):
        diff = gh("pr", "diff", str(pr["number"]))
        found = {int(a, 16) for a in MARKER.findall(diff) + ROW.findall(diff)}
        out.append((f"PR #{pr['number']} ({pr['author']['login']})", found))
    issues = json.loads(gh("issue", "list", "--state", "open", "--label", LABEL, "--limit", "100",
                           "--json", "number,body,comments"))
    for issue in issues:
        found = set()
        for text in [issue["body"]] + [c["body"] for c in issue["comments"]]:
            for lo, hi in SPAN.findall(text):
                lo = int(lo, 16)
                hi = int(hi, 16) if hi else lo
                found |= {a for a in starts if lo <= a <= hi}
        out.append((f"issue #{issue['number']}", found))
    return out


def claimed():
    return set().union(*(found for _, found in sources()))


if __name__ == "__main__":
    total = set()
    for label, found in sources():
        total |= found
        print(f"{label}: {len(found)}")
    print(f"{len(total)} claimed functions")
