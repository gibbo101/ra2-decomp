"""Import distinct journal observations into the daily attempts table."""
import argparse
from collections import defaultdict
import csv
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
import sys
from tempfile import NamedTemporaryFile


def clean(s):
    s = re.sub(r"/home/[^\s,;)]*/ra2-decomp/", "", s or "")
    s = re.sub(r"/(home|tmp)/[^\s,;)]*", "<path>", s)
    return s.replace("\n", " ")[:300]


def observations(paths, date):
    for path in paths:
        labels = {}
        for line in path.read_text().splitlines():
            record = json.loads(line)
            key = record.get("key")
            if record.get("type") == "started":
                labels[key] = record.get("label", "")
            result = record.get("result")
            if record.get("type") != "result" or not isinstance(result, dict):
                continue
            model = "opus-5.5 high" if labels.get(key, "").startswith("opus") else "sonnet-5.5 medium"
            if "landed" in result:
                model = "opus-5.5 high (header merge)"
            for item in result.get("functions", []):
                addr = f"0x{int(item['address'], 16):08X}"
                identity = [date, key, model, addr, item.get("attempt_id", item)]
                token = hashlib.sha256(json.dumps(identity, sort_keys=True).encode()).hexdigest()
                yield addr, token, model, item


def legacy_total_matches(row, observations):
    # The old importer summed checks and retained the maximum score, but
    # saved the result, blocker and notes from the last contributing entry.
    total, best, last = 0, None, None
    for _, _, item in observations:
        score = float(item["best_percent"])
        checks = int(item.get("checks", 0))
        if best is not None and not checks and item["result"] != "MATCH" and score <= best:
            continue
        total += checks
        best = max(best, score) if best is not None else score
        last = item
    if last is None or not row["best"]:
        return False
    exact = last["result"] == "MATCH"
    blocker = clean(last.get("blocker")) or ("matched in a banked file but not landed" if exact else "")
    return (total == int(row["checks"] or 0) and best == float(row["best"])
            and row["result"] == ("partial" if exact else last["result"])
            and row["blocker"] == blocker and row["notes"] == clean(last.get("note")))


def merge(table, date, paths, known, done):
    with table.open(newline="") as f:
        reader = csv.DictReader(f, strict=True)
        fields = list(reader.fieldnames or ())
        if not fields or len(set(fields)) != len(fields):
            raise ValueError(f"{table}: missing or duplicate column names")
        rows = []
        for row in reader:
            if None in row or None in row.values():
                raise ValueError(f"{table}:{reader.line_num}: expected {len(fields)} columns")
            rows.append(row)
    for col in ("attempt_ids", "replays"):
        if col not in fields:
            fields.append(col)
    today = {r["address"]: r for r in rows if r["date"] == date}
    by_address = defaultdict(dict)
    for addr, token, model, item in observations(paths, date):
        if addr in known and addr not in done:
            by_address[addr].setdefault(token, (token, model, item))

    added = 0
    for addr, items in by_address.items():
        row = today.get(addr)
        adopt = row is not None and not row.get("attempt_ids")
        if row is None:
            row = dict.fromkeys(fields, "")
            row.update(address=addr, date=date)
            rows.append(row)
            today[addr] = row
        if adopt and not legacy_total_matches(row, items.values()):
            print(f"{addr}: preserving legacy total; import its complete original journals first", file=sys.stderr)
            continue
        for token, model, item in items.values():
            ids = (row.get("attempt_ids") or "").split()
            if token in ids:
                continue
            score = float(item["best_percent"])
            checks = int(item.get("checks", 0))
            row["attempt_ids"] = " ".join([*ids, token])
            row["checks"] = int(row["checks"] or 0) + (0 if adopt else checks)
            if item.get("replay"):
                bundle = Path(item["replay"])
                if bundle.is_absolute() or ".." in bundle.parts:
                    raise ValueError("replay must be a repository-relative bundle")
                raw = bundle.read_bytes()
                data = json.loads(raw)
                if data["address"] != addr or data["version"] not in (1, 2) or not data.get("expected"):
                    raise ValueError(f"{bundle}: wrong replay bundle for {addr}")
                links = json.loads(row.get("replays") or "{}")
                links[token] = [bundle.as_posix(), hashlib.sha256(raw).hexdigest()]
                row["replays"] = json.dumps(links, sort_keys=True, separators=(",", ":"))
            if not adopt and (not row["best"] or score > float(row["best"])
                              or (score == float(row["best"]) and item["result"] == "MATCH")):
                exact = item["result"] == "MATCH"
                row.update(best=f"{score:g}", result="partial" if exact else item["result"], model=model,
                           blocker=clean(item.get("blocker")) or ("matched in a banked file but not landed" if exact else ""),
                           notes=clean(item.get("note")))
            added += 1
    if added:
        with NamedTemporaryFile("w", newline="", dir=table.parent, delete=False) as f:
            try:
                writer = csv.writer(f, lineterminator="\n")
                writer.writerow(fields)
                for row in rows:
                    values = [row.get(k) or "" for k in fields]
                    writer.writerow(values)
                f.flush()
                os.replace(f.name, table)
            finally:
                Path(f.name).unlink(missing_ok=True)
    return added


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("date")
    parser.add_argument("journals", nargs="+", type=Path)
    parser.add_argument("--table", type=Path, default=Path("data/attempts.csv"))
    args = parser.parse_args()
    with open("data/functions.csv") as f:
        known = {r["address"] for r in csv.DictReader(f)}
    with open("data/progress.csv") as f:
        done = {r["address"] for r in csv.DictReader(f) if r["result"] == "MATCH"}
    lock = os.open(args.table.parent, os.O_RDONLY)
    try:
        fcntl.flock(lock, fcntl.LOCK_EX)
        count = merge(args.table, args.date, args.journals, known, done)
    finally:
        os.close(lock)
    print(f"{count} new attempt observations")


if __name__ == "__main__":
    main()
