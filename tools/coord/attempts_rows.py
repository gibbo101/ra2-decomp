"""Import distinct journal observations into the daily attempts table."""
import argparse
import csv
import fcntl
import hashlib
import json
import os
from pathlib import Path
import re
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


def merge(table, date, paths, known, done):
    with table.open(newline="") as f:
        reader = csv.DictReader(f)
        fields = list(reader.fieldnames)
        rows = list(reader)
    for col in ("attempt_ids", "replays"):
        if col not in fields:
            fields.append(col)
    today = {r["address"]: r for r in rows if r["date"] == date}
    added = 0
    for addr, token, model, item in observations(paths, date):
        if addr in done or addr not in known:
            continue
        row = today.get(addr)
        if row is None:
            row = dict.fromkeys(fields, "")
            row.update(address=addr, date=date)
            rows.append(row)
            today[addr] = row
        ids = (row.get("attempt_ids") or "").split()
        if token in ids:
            continue
        score = float(item["best_percent"])
        checks = int(item.get("checks", 0))
        # Legacy rows have no observation IDs. An exact re-import of their
        # winning observation establishes its ID without counting it twice.
        legacy_repeat = (not ids and row["best"] and float(row["best"]) == score
                         and row["blocker"] == clean(item.get("blocker"))
                         and checks <= int(row["checks"] or 0))
        row["attempt_ids"] = " ".join([*ids, token])
        row["checks"] = int(row["checks"] or 0) + (0 if legacy_repeat else checks)
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
        if not row["best"] or score > float(row["best"]) or (score == float(row["best"]) and item["result"] == "MATCH"):
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
                    writer.writerow(values if row.get("attempt_ids") else values[:-2])
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
