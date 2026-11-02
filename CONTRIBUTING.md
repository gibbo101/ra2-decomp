# Contributing

This project has one goal: C++ source that Visual C++ 6.0 builds into the same bytes as
the shipped `gamemd.exe` 1.001. We accept matched functions and the header and tooling
work that supports them. We are not accepting enhancements, fixes to the game's behaviour,
new features, refactors or ports at this time.

## Before you start

1. Check the pinned "In progress" issue. It lists the modules and address ranges that
   maintainers' batches are working on now; avoid them.
2. Claim what you will work on with a comment on that issue: the module, or an address
   range such as `0x00401000-0x0040AD89`. Our batch tools skip claimed functions and every
   function in an open pull request.
3. Read `docs/AGENTS.md`: what counts as a match, and what does not (no inline asm,
   hard-coded addresses, stand-in structs or casts over real objects to force one).
4. Turn on the commit guard (`docs/COMMIT_GUARD.md`).

## Pull requests

- One module or one closely related set of functions per pull request, each function in
  the `src/` file of the module it belongs to (`data/modules.csv`), in address order.
- Every file in `src/` and `include/` opens with the licence header in the README.
- Maintainers rebuild and check every pull request on their own copy before merging. A
  pull request's own "N of N match" counts only once our checker agrees.
- Pull requests that stack on unmerged pull requests are hard to review: keep each one
  independent where you can.
