"""Find targets whose recorded non-improving rounds used their compiler-check budget."""


def exhausted_retries(attempts, sizes):
    best, stalled = {}, {}
    for row in attempts:
        address = int(row["address"], 16)
        try:
            score = float(row["best"])
            checks = int(row["checks"] or 0)
        except (TypeError, ValueError):
            continue
        if address not in best or score > best[address]:
            best[address], stalled[address] = score, 0
        else:
            stalled[address] += checks
    return {a for a, count in stalled.items() if a in sizes and count >= (60 if sizes[a] > 2000 else 30)}
