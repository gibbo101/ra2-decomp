# Commit guard

`tools/hooks/` holds git hooks that stop personal data, secrets and game binaries from
entering a commit or reaching GitHub. Anything committed stays in history even after a later
commit deletes it, so the check runs before the commit exists.

## Setup, once per clone

```sh
git config core.hooksPath tools/hooks
git config user.email <id>+<login>@users.noreply.github.com
```

Then list your own personal strings in `.git/info/private-strings`, one per line: real
name, personal email addresses, machine and host names. The file lives inside `.git/`, so it
is never tracked. Each entry matches as a whole word, ignoring case.

## What each hook checks

| Hook | Runs `guard.sh` | Checks |
|---|---|---|
| `pre-commit` | `staged` | Added lines in the index, staged file names and contents, author and committer email |
| `commit-msg` | `message <file>` | The commit message |
| `pre-push` | `push` | Every commit being pushed: author and committer email, messages, added lines |

The push check repeats the others so a commit made without the hooks is still caught
before it leaves the machine.

## What it blocks

- **Email addresses**, except GitHub no-reply addresses, `noreply@…` and `git@github.com`.
- **Author or committer email** that is not a no-reply address.
- **Home-directory paths** (`/home/<name>`, `/Users/<name>`, `C:\Users\<name>`), which leak
  user names. Write `~` or a path relative to the repo instead.
- **Secrets**: private-key blocks and GitHub, Anthropic, OpenAI, AWS, Slack and Google
  token shapes.
- **Secret-looking file names**: `.env*`, `id_rsa` and friends, `credentials*`, `*.pem`,
  `*.key`, `*.p12`, `*.pfx`, `*.keystore`.
- **Windows executables** (any staged file starting `MZ`), whatever their name: the game
  and the compiler never go in the repo.
- **Private strings** from `.git/info/private-strings`.

A blocked commit means removing the data, never `--no-verify`. A false positive gets the
pattern fixed in `guard.sh`, in its own commit.
