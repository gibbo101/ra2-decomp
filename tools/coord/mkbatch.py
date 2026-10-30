"""Write batchN.js and batchN.json for a round of batches run side by side.

Usage: uv run python tools/coord/mkbatch.py TEMPLATE.js N:mod,mod,... N:mod,mod,... [...]
Each batch's briefs name the modules every other batch in the round owns.
"""

import os, json
import re
import subprocess
import sys
from pathlib import Path

COORD = Path(__file__).resolve().parent
OUT = Path("build/coord")
OUT.mkdir(parents=True, exist_ok=True)


def main() -> None:
    template = Path(sys.argv[1]).read_text()
    round_ = [(int(n), mods.split(",")) for n, mods in (a.split(":") for a in sys.argv[2:])]
    for n, mods in round_:
        others = [m for k, ms in round_ if k != n for m in ms]
        other_batches = ", ".join(str(k) for k, _ in round_ if k != n)
        s = re.sub(r"name: 'batch-\d+-small-functions'", f"name: 'batch-{n}-small-functions'", template)
        s = s.replace("REPO_PATH", os.getcwd())
        s = re.sub(r"description: '[^']*'",
                   f"description: 'Match 144 small functions in the {', '.join(mods)} modules, "
                   f"alongside batch {other_batches} into src/: Sonnet, a header merge, then Opus'", s)
        s = re.sub(r"including (?:a second batch|other batches) on the [^.]* modules\.",
                   f"including other batches on the {', '.join(others)} modules.", s)
        s = re.sub(r"(?:A second batch|Other batches) \([^)]*\) may be running at the same time, (?:its own header merge|their own header merges) included\.",
                   f"Other batches ({', '.join(others)}) may be running at the same time, their own header merges included.", s)
        s = s.replace("that batch's files, report it", "those batches' files, report it")
        assert f"batch-{n}-" in s and "other batches on the" in s and "Other batches (" in s
        (OUT / f"batch{n}.js").write_text(s)
        picked = subprocess.run([sys.executable, str(COORD / "pick.py"), ",".join(mods)],
                                capture_output=True, text=True, check=True).stdout
        (OUT / f"batch{n}.json").write_text(picked)
        counts = {k: len(v) for k, v in json.loads(picked).items()}
        print(n, sum(counts.values()), counts)


if __name__ == "__main__":
    main()
