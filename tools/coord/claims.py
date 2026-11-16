"""Function addresses that outside work has claimed, so a round never picks them.

Usage: python3 tools/coord/claims.py   (lists the claims; the generators import claimed())
A claim is a function an open pull request adds a marker or a MATCH progress row for, or an
address or range (0xAAAAAAAA-0xBBBBBBBB) in the body of an open issue labelled "claims" or in a
contributor's comment on one. The maintainer's comments never claim; one that says "taking back"
releases the addresses it lists from every earlier claim, and a later comment can claim them again.
A contributor releases only their own claims, with a sentence that says "releas...": the addresses
that sentence lists; or, if it lists none, every claim comment of theirs that the comment links
(#issuecomment-N); or, if it links none, their latest claim. The rest of a releasing comment is a
status note and claims only if it also says it is taking, claiming, picking up or checking.
Fails rather than returning nothing when GitHub cannot be read.
"""
import csv, json, re, subprocess, sys

MARKER = re.compile(r"^\+\s*// (?:FUNCTION|SYNTHETIC|TEMPLATE|LIBRARY): GAMEMD (0x[0-9A-Fa-f]{8})", re.M)
ROW = re.compile(r"^\+(0x[0-9A-Fa-f]{8}),[^\n]*,MATCH,", re.M)
SPAN = re.compile(r"\b(0x[0-9A-Fa-f]{8})(?:\s*(?:[-\u2013\u2014]|\.\.)\s*(0x[0-9A-Fa-f]{8}))?\b")
LABEL = "claims"
TAKE_BACK = re.compile(r"\btaking back\b", re.I)
RELEASE = re.compile(r"\breleas", re.I)
SENTENCE = re.compile(r"(?<=[.!?])\s+|\n+")
LINK = re.compile(r"issuecomment-(\d+)")
CLAIM = re.compile(r"\b(?:taking|claiming|picking up|checking)\b", re.I)


def gh(*args, fail=True):
    out = subprocess.run(["gh", *args], capture_output=True, text=True)
    if out.returncode:
        if not fail:
            return None
        sys.exit(f"claims.py: gh {' '.join(args[:2])} failed: {out.stderr.strip()}")
    return out.stdout


def pr_diff(number):
    """The pull request's diff; GitHub refuses whole diffs it is slow to build, so fall back to per-file patches."""
    diff = gh("pr", "diff", str(number), fail=False)
    if diff is None:
        diff = gh("api", "--paginate", f"repos/{{owner}}/{{repo}}/pulls/{number}/files", "--jq", ".[].patch // empty")
    return diff


def sources():
    """(label, set of addresses) for each open pull request and claims issue."""
    starts = sorted(int(r["address"], 16) for r in csv.DictReader(open("data/functions.csv")))
    out = []
    for pr in json.loads(gh("pr", "list", "--state", "open", "--limit", "200", "--json", "number,author")):
        diff = pr_diff(pr["number"])
        found = {int(a, 16) for a in MARKER.findall(diff) + ROW.findall(diff)}
        out.append((f"PR #{pr['number']} ({pr['author']['login']})", found))
    owner = gh("repo", "view", "--json", "owner", "-q", ".owner.login").strip()
    numbers = json.loads(gh("issue", "list", "--state", "open", "--label", LABEL, "--limit", "100", "--json", "number"))
    for number in numbers:
        # issue list returns at most 100 comments per issue; issue view returns them all
        issue = json.loads(gh("issue", "view", str(number["number"]), "--json", "number,body,comments"))
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
    body = addresses(issue["body"], starts)
    claims = []  # [comment number, author, addresses], oldest first
    for comment in sorted(issue["comments"], key=lambda c: c["createdAt"]):
        text = comment["body"]
        author = (comment.get("author") or {}).get("login")
        if author == owner:
            if TAKE_BACK.search(text):
                listed = addresses(text, starts)
                body -= listed
                for claim in claims:
                    claim[2] -= listed
            continue
        sentences = [s for s in SENTENCE.split(text) if s.strip()]
        released = [s for s in sentences if RELEASE.search(s)]
        own = [claim for claim in claims if claim[1] == author]
        if released:
            listed = addresses(" ".join(released), starts)
            linked = set(LINK.findall(text))
            if listed:
                for claim in own:
                    claim[2] -= listed
            elif linked:
                for claim in own:
                    if claim[0] in linked:
                        claim[2].clear()
            elif own:
                own[-1][2].clear()
        kept = " ".join(s for s in sentences if not RELEASE.search(s))
        if released and not CLAIM.search(kept):
            kept = ""
        number = (LINK.findall(comment.get("url") or "") or [None])[-1]
        claims.append([number, author, addresses(kept, starts)])
    return body.union(*(claim[2] for claim in claims))


def claimed():
    return set().union(*(found for _, found in sources()))


if __name__ == "__main__":
    total = set()
    for label, found in sources():
        total |= found
        print(f"{label}: {len(found)}")
    print(f"{len(total)} claimed functions")
