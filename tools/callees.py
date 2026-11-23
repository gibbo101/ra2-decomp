"""The functions an unmatched function calls out of line, and what our source already has
for each: name, whether it matches, and where it is defined.

Usage: uv run tools/callees.py 0x00410A60

A callee the exe calls by name is an API the original used: call it, never write its body
out at the call site. A callee we have not named or matched is listed so it can be found.
"""

import sys

from exeindex import ExeIndex


def report(index: ExeIndex, address: int) -> list[str]:
    calls = index.calls(address)
    out = [f"0x{address:08X} {index.name(address)}: {len(calls)} direct calls to "
           f"{len(set(calls))} functions"]
    seen = set()
    for target in calls:
        if target in seen:
            continue
        seen.add(target)
        times = calls.count(target)
        where = index.source_line(target)
        state = "MATCH" if index.matched(target) else "unmatched"
        line = f"  0x{target:08X} x{times} {index.name(target)} [{state}]"
        if where:
            line += f"\n      {where[0]}: {where[1]}"
        out.append(line)
    callers = sorted(index.callers.get(address, ()))
    matched = [c for c in callers if index.matched(c)]
    out.append(f"\ncalled from {len(callers)} functions, {len(matched)} matched:")
    for c in callers[:12]:
        where = index.source_line(c)
        out.append(f"  0x{c:08X} {index.name(c)} [{'MATCH' if index.matched(c) else 'unmatched'}]"
                   + (f" {where[0]}" if where else ""))
    if len(callers) > 12:
        out.append(f"  ... {len(callers) - 12} more")
    return out


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    print("\n".join(report(ExeIndex(), int(sys.argv[1], 16))))


if __name__ == "__main__":
    main()
