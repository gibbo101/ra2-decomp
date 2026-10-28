# ra2-decomp: Claude context

A matching decompilation of Red Alert 2: Yuri's Revenge (`gamemd.exe` 1.001), modelled on
Byte Tactics (Total Annihilation). Active: about 85% of the game's functions match, landed
in rounds of agent batches.

**Read `docs/PLAN.md` first** for the method, the binary's measured facts, the prior art
(OpenTS, YRpp, the recomp), the legal footing and a record of every round. Then
`docs/AGENTS.md`: how a function is matched and every lesson learned. The README covers
setup and the checker.

## Rules

- Never commit game files, the original exe, or compiler binaries. `orig/`, `toolchain/` and
  `build/` are gitignored; the player brings the game and a setup script fetches the
  compiler.
- Third-party references are read-only: the TibSun archive (`reference/tibsun`), YRpp
  (`reference/YRpp`), OpenTS and EA's source releases. Copy from them; never edit them.
- The source is the game as shipped: no fixes, features or enhancements, even obvious
  ones. The only goal is a byte-matching rebuild.
- MATCH means the checker printed MATCH. No inline asm, `#pragma optimize`, hard-coded
  addresses or `volatile` tricks to force one. The exceptions are the original's own
  constructs:
  - inline asm, `#pragma optimize` and whole `.asm` files that Westwood's own source had,
    copied unchanged from the TibSun archive (`reference/tibsun/code`);
  - `volatile` on data another thread writes (a field or global, never a local), where
    the exe re-reads it in the way only `volatile` makes VC6 do;
  - Westwood's `div_zero` crash idiom as a local (`volatile int div_zero = 0;
    div_zero = 1 / div_zero;`), where the exe has that exact sequence.
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
- A third party's licence notice on code adapted from them stays intact, author name
  included; the rule above covers the maintainer's own data.
- No Co-Authored-By or other AI attribution lines in commit messages.
