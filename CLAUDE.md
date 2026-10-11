# ra2-decomp: Claude context

A matching decompilation of Red Alert 2: Yuri's Revenge (`gamemd.exe` 1.001), modelled on
Byte Tactics (Total Annihilation). Parked until there's token budget for it.

**Read `docs/PLAN.md` first.** It holds everything gathered so far: the method, the binary's
measured facts, the prior art (OpenTS, YRpp, the recomp), the legal footing and the spike
that comes first.

## Rules

- Never commit game files, the original exe, or compiler binaries. `orig/`, `toolchain/` and
  `build/` are gitignored; the player brings the game and a setup script fetches the
  compiler.
- `../cnc-remastered-mods/reference/OpenTS` and the EA source clones (`../CnC_*`) are
  read-only references. Copy from them; never edit them.
- v1 is the game as shipped: no fixes, features or enhancements in this repo, even
  obvious ones. Those go to the v2 repo (see "Scope" in `docs/PLAN.md`).
- MATCH means the checker printed MATCH. No inline asm, `#pragma optimize`, hard-coded
  addresses or `volatile` tricks to force one.
- Running agents in parallel (a workflow) needs the maintainer's explicit opt-in each time.

## Comments

- A comment states expected behaviour only: what the code does or guarantees, or the
  intent of a non-obvious branch.
- Never names, dates, decisions, tickets, issue numbers, "fixes the bug" or history. That
  belongs in the commit message or the docs.
- Two lines at most. A longer comment needs the maintainer's sign-off before it is written.
- A comment never points to a doc.
- Checker markers that tie a function to its address are tooling, not comments, and are
  allowed.
- The licence header that opens every file in `src/` and `include/` (format in the README)
  is a legal notice, not a comment; it is required, and none of the rules above apply.

## Commits

- The commit guard in `tools/hooks/` must be active (`git config core.hooksPath
  tools/hooks`); see `docs/COMMIT_GUARD.md`. A blocked commit means removing the data,
  never `--no-verify`.
- No personal data in files, comments, commit messages or author fields: no real names,
  personal email, home-directory paths or host names. Author email is the GitHub no-reply
  address.
- No Co-Authored-By or other AI attribution lines in commit messages.
