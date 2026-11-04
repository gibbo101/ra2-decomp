"""Plan evidence-driven jobs or run a bounded product of explicit source alternatives.

    uv run tools/coord/choices.py plan jobs.json
    uv run tools/coord/choices.py run jobs.json JOB build/trials/JOB --budget 30

See tools/coord/README.md for the input format.
"""
import argparse
from collections import defaultdict
import csv
import hashlib
import itertools
import json
import re
from pathlib import Path
import sys
import tempfile

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import check
from coord.claims import claimed
from tier0 import module_lookup


def addresses(job):
    return [int(job["address"], 16), *(int(a, 16) for a in job.get("helpers", {}))]


def variants(source, choices):
    # Resolve all sites against the starting text. One alternative cannot accidentally
    # introduce a new occurrence for a later replacement.
    sites = []
    for site in choices:
        text = site["find"]
        if not text or source.count(text) != 1:
            raise ValueError("each find string must occur exactly once")
        start = source.index(text)
        sites.append((start, start + len(text), [text, *site["alternatives"]], site.get("group")))
    sites.sort()
    if any(first[1] > second[0] for first, second in zip(sites, sites[1:])):
        raise ValueError("overlapping choice sites")
    groups = defaultdict(list)
    for index, (_, _, alternatives, group) in enumerate(sites):
        if group:
            groups[group].append(index)
    baseline = tuple(site[2][0] for site in sites)
    coherent = []
    for group, indices in groups.items():
        width = len(sites[indices[0]][2])
        if any(len(sites[i][2]) != width for i in indices):
            raise ValueError(f"{group}: grouped sites need corresponding alternatives")
        for choice in range(1, width):
            seed = list(baseline)
            for i in indices:
                seed[i] = sites[i][2][choice]
            coherent.append(tuple(seed))
    seen = set()
    mixed = itertools.product(*(dict.fromkeys(site[2]) for site in sites))
    for replacements in itertools.chain([baseline], coherent, mixed):
        text = source
        for (begin, end, _, _), replacement in reversed(list(zip(sites, replacements))):
            text = text[:begin] + replacement + text[end:]
        digest = hashlib.sha256(text.encode("latin-1")).hexdigest()
        if digest not in seen:
            seen.add(digest)
            yield digest, text


def plan(jobs, taken, done, known, owner):
    scheduled = set()
    out = []
    for job in sorted(jobs, key=lambda j: -len(set(addresses(j)) - done)):
        if not job["hypothesis"].strip() or not job["evidence"].strip():
            raise ValueError(f"{job['name']}: give a hypothesis and the evidence for retrying it")
        group = addresses(job)
        if any(a not in known for a in group) or len(set(group)) != len(group):
            raise ValueError(f"{job['name']}: unknown or repeated address")
        if group[0] in done or taken.intersection(group):
            continue
        modules = defaultdict(list)
        for a in group:
            if a not in done:
                modules[owner(a)].append(f"0x{a:08X}")
        if scheduled.intersection(group):
            raise ValueError("two jobs would work on the same function")
        scheduled.update(group)
        out.append(dict(name=job["name"], modules=modules, model="opus", effort="high",
                        note=job["evidence"] + "\n" + job["hypothesis"]))
    return {"jobs": out}


def waiting_groups(attempts, taken, done, known):
    """Use only the latest recorded blocker and an explicitly named dependency."""
    latest = {int(r["address"], 16): r for r in attempts}
    groups = defaultdict(list)
    for addr, row in sorted(latest.items()):
        blocker = row["blocker"]
        match = re.search(r"waits (?:on|for) (?:caller )?(0x[0-9a-f]{8})\b", blocker, re.I)
        if not match or addr not in known or addr in taken or addr in done:
            continue
        caller = int(match[1], 16)
        if caller == addr or caller not in known or caller in taken or caller in done:
            continue
        groups[caller].append((addr, blocker))
    return sorted(groups.items(), key=lambda item: (-len(item[1]), item[0]))


def code_digest(obj, pick):
    sym, sec, lo, hi = pick
    refs = [(r.offset - lo, r.type, obj.by_index[r.symbol].name) for r in sec.relocs if lo <= r.offset < hi]
    return hashlib.sha256(sec.data[lo:hi] + json.dumps([sym.name, refs]).encode()).hexdigest()


def observations(src, raw, job, orig, names):
    obj = check.CoffObject(raw)
    local, conflicts = check.bind_annotations(orig, obj, raw, src)
    if conflicts:
        raise ValueError("\n".join(conflicts))
    caller = int(job["address"], 16)
    target_names = {caller: dict(check.annotations(src))[caller]}
    target_names.update({int(a, 16): n for a, n in job.get("helpers", {}).items()})
    functions = list(obj.functions())
    picks, results = {}, {}
    for addr, name in target_names.items():
        candidates = [f for f in functions if f[0].name.startswith(check.mangled(name))]
        bound = [f for f in candidates if local.get(f[0].name) == addr]
        eligible = bound or [f for f in candidates if f[0].name not in local]
        if len(eligible) != 1:
            results[f"0x{addr:08X}"] = {"status": "absent" if not candidates else "ambiguous"}
            continue
        picks[addr] = eligible[0]
        local[eligible[0][0].name] = addr
    held = check.vftable_holders(obj)
    for addr, pick in picks.items():
        r = check.compare_one(orig, obj, raw, addr, target_names[addr], names, local, pick, True)
        problem = check.class_problem(orig.rtti(), held, r.symbol, addr) if r.bytes_match else None
        exact = r.matched and not (problem and problem[1])
        results[f"0x{addr:08X}"] = dict(status="match" if exact else "partial", code=code_digest(obj, pick),
                                       raw=1.0 if exact else r.ratio, normalized=r.norm_ratio,
                                       unverified=r.unverified, size=r.ours)
    return results


def run(job, dest, budget):
    if not job["hypothesis"].strip() or not job["evidence"].strip():
        raise ValueError("a trial requires evidence and a hypothesis")
    source = Path(job["source"])
    overlay = Path(job["overlay"]) if job.get("overlay") else None
    if budget < 1 or (overlay and not overlay.is_dir()):
        raise ValueError("invalid budget or overlay")
    dest = dest.resolve()
    if (check.ROOT / "build") not in dest.parents:
        raise ValueError("trial output must be in a new directory under build/")
    dest.mkdir(parents=True)
    orig, names = check.Original(), check.known_names()
    calls, outputs, wins = 0, set(), 0
    (dest / "job.json").write_text(json.dumps(job, indent=2) + "\n")
    with tempfile.TemporaryDirectory(dir=dest) as tmp, (dest / "results.jsonl").open("w") as log:
        candidate = Path(tmp) / source.name
        for digest, text in itertools.islice(variants(source.read_bytes().decode("latin-1"), job["choices"]), budget):
            candidate.write_bytes(text.encode("latin-1"))
            (dest / (digest + ".cpp")).write_bytes(candidate.read_bytes())
            calls += 1
            p, message = check.compile_source(candidate, overlay, out=Path(tmp) / "trial.obj")
            row = {"compile": calls, "source": digest}
            if p is None:
                row["error"] = message.strip()
            else:
                try:
                    results = observations(candidate, p.read_bytes(), job, orig, names)
                    row["targets"] = results
                    outputs.add(tuple((a, r["status"], r.get("code")) for a, r in sorted(results.items())))
                    wins += all(r["status"] == "match" for r in results.values())
                except (ValueError, KeyError, SystemExit) as exc:
                    row["error"] = str(exc)
            log.write(json.dumps(row) + "\n")
            log.flush()
    print(f"{calls} compiler invocations, {len(outputs)} distinct target outputs, {wins} exact combinations")


def main():
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="command", required=True)
    sub.add_parser("plan").add_argument("jobs", type=Path)
    sub.add_parser("suggest", help="list explicitly blocked groups for review")
    run_parser = sub.add_parser("run")
    run_parser.add_argument("jobs", type=Path)
    run_parser.add_argument("job")
    run_parser.add_argument("dest", type=Path)
    run_parser.add_argument("--budget", type=int, required=True)
    args = p.parse_args()
    try:
        jobs = json.loads(args.jobs.read_text()) if args.command != "suggest" else []
        if len({job["name"] for job in jobs}) != len(jobs):
            raise ValueError("job names must be unique")
        if args.command in ("suggest", "plan"):
            with (check.ROOT / "data/progress.csv").open() as f:
                done = {int(r["address"], 16) for r in csv.DictReader(f) if r["result"] == "MATCH"}
            with (check.ROOT / "data/functions.csv").open() as f:
                known = {int(r["address"], 16) for r in csv.DictReader(f)}
            taken = claimed()
            if args.command == "suggest":
                with (check.ROOT / "data/attempts.csv").open() as f:
                    groups = waiting_groups(csv.DictReader(f), taken, done, known)
                for caller, helpers in groups:
                    print(f"0x{caller:08X}: {len(helpers)} waiting")
                    for addr, evidence in helpers:
                        print(f"  0x{addr:08X} {evidence}")
            else:
                print(json.dumps(plan(jobs, taken, done, known, module_lookup()), indent=2))
        else:
            selected = next((j for j in jobs if j["name"] == args.job), None)
            if selected is None:
                raise ValueError("unknown job")
            run(selected, args.dest, args.budget)
    except (OSError, KeyError, ValueError) as exc:
        p.exit(1, str(exc) + "\n")


if __name__ == "__main__":
    main()
