"""Token use and wall time of each agent in a workflow run, from its transcripts.

Usage: uv run tools/agentcost.py <workflow transcript dir> [--csv OUT]

One row per agent: its label, model, minutes from first to last message, and its tokens
(fresh input, cache writes, cache reads, output). Cost units weight them by the API's
relative prices: cache reads at a tenth of input, cache writes at 1.25 times, output at
five times, all scaled by the model's input price (Haiku 1, Sonnet 2, Opus 4, Fable 10).
"""

import csv
import json
import sys
from datetime import datetime
from pathlib import Path

PRICE = {"haiku": 1, "sonnet": 2, "opus": 4, "fable": 10}


def agent_rows(directory: Path):
    for meta_path in sorted(directory.glob("agent-*.meta.json")):
        meta = json.loads(meta_path.read_text())
        transcript = meta_path.with_name(meta_path.name.replace(".meta.json", ".jsonl"))
        tokens = {"input": 0, "cache_write": 0, "cache_read": 0, "output": 0}
        times, last_usage = [], {}
        model = meta.get("model", "")
        for line in transcript.read_text().splitlines():
            entry = json.loads(line)
            if "timestamp" in entry:
                times.append(datetime.fromisoformat(entry["timestamp"].replace("Z", "+00:00")))
            message = entry.get("message") or {}
            usage = message.get("usage")
            if usage:
                # A streamed message is logged more than once; its last record is complete.
                last_usage[message.get("id")] = usage
                model = message.get("model", model)
        for usage in last_usage.values():
            tokens["input"] += usage.get("input_tokens", 0)
            tokens["cache_write"] += usage.get("cache_creation_input_tokens", 0)
            tokens["cache_read"] += usage.get("cache_read_input_tokens", 0)
            tokens["output"] += usage.get("output_tokens", 0)
        family = next((k for k in PRICE if k in model), None)
        units = None
        if family:
            units = PRICE[family] * (tokens["input"] + 1.25 * tokens["cache_write"]
                                     + 0.1 * tokens["cache_read"] + 5 * tokens["output"]) / 1e6
        minutes = (max(times) - min(times)).total_seconds() / 60 if times else 0
        yield {"label": meta.get("description", ""), "model": model,
               "minutes": round(minutes, 1), **tokens,
               "cost_units": round(units, 3) if units is not None else ""}


def main() -> None:
    directory = Path(sys.argv[1])
    rows = list(agent_rows(directory))
    out = sys.argv[sys.argv.index("--csv") + 1] if "--csv" in sys.argv else None
    if out:
        with open(out, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=list(rows[0]), lineterminator="\n")
            w.writeheader()
            w.writerows(rows)
    for r in rows:
        print(f"{r['label']:16} {r['model']:20} {r['minutes']:6} min  out {r['output']:7}  "
              f"read {r['cache_read']:9}  units {r['cost_units']}")


if __name__ == "__main__":
    main()
