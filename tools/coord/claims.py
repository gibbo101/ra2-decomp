"""Function addresses that outside work has claimed, so a round never picks them.

Usage: python3 tools/coord/claims.py   (lists the claims; the generators import claimed())
A claim is a function an open pull request adds a marker or a MATCH progress row for, or an
address or range (0xAAAAAAAA-0xBBBBBBBB) in the body of an open issue labelled "claims" or in a
contributor's comment on one. The maintainer's comments never claim; one that says "taking back"
releases the addresses it lists from every earlier claim, and a later comment can claim them again.
Fails rather than returning nothing when GitHub cannot be read.
"""
import csv, json, re, subprocess, sys

MARKER = re.compile(r"^\+\s*// (?:FUNCTION|SYNTHETIC|TEMPLATE|LIBRARY): GAMEMD (0x[0-9A-Fa-f]{8})", re.M)
ROW = re.compile(r"^\+(0x[0-9A-Fa-f]{8}),[^\n]*,MATCH,", re.M)
SPAN = re.compile(r"\b(0x[0-9A-Fa-f]{8})(?:\s*(?:[-\u2013\u2014]|\.\.)\s*(0x[0-9A-Fa-f]{8}))?\b")
LABEL = "claims"
TAKE_BACK = re.compile(r"\btaking back\b", re.I)


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
    owner = gh("repo", "view", "--json", "owner", "-q", ".owner.login").strip()
    issues = json.loads(gh("issue", "list", "--state", "open", "--label", LABEL, "--limit", "100",
                           "--json", "number,body,comments"))
    for issue in issues:
        out.append((f"issue #{issue['number']}", issue_claims(issue, owner, starts)))
    return out


def addresses(text, starts):
    found = set()
    for lo, hi in SPAN.findall(text):
        lo = int(lo, 16)
        hi = int(hi, 16) if hi else lo
        found |= {a for a in starts if lo <= a <= hi}
    return found


def issue_claims(issue, owner, starts):
    """The functions an issue still claims, reading its comments in the order they were posted."""
    found = addresses(issue["body"], starts)
    for comment in sorted(issue["comments"], key=lambda c: c["createdAt"]):
        listed = addresses(comment["body"], starts)
        if (comment.get("author") or {}).get("login") != owner:
            found |= listed
        elif TAKE_BACK.search(comment["body"]):
            found -= listed
    return found


def claimed():
    return set().union(*(found for _, found in sources()))


if __name__ == "__main__":
    total = set()
    for label, found in sources():
        total |= found
        print(f"{label}: {len(found)}")
    print(f"{len(total)} claimed functions")
