#!/usr/bin/env python3
"""PreToolUse hook: refuse any Bash command that deletes recursively.

Covers rm -r in any spelling, git rm -r, find -delete and find -exec rm, xargs rm,
shutil.rmtree, git clean and rsync --delete. Deleting named files, globs within one
directory, rmdir and find -type d -empty -delete (empty directories only) still run.
"""

import json
import re
import shlex
import sys

OPERATORS = ";&|()\n"
SUBSTITUTION = re.compile(r"\$\(([^()]*)\)|`([^`]*)`")
REMOVERS = {"rm", "unlink", "shred"}
WRAPPERS = {"sudo", "env", "nice", "nohup", "time", "command", "exec", "timeout", "doas"}
CHECKED = {"rm", "find", "xargs", "git", "rsync"}


def segments(command: str) -> list[list[str]]:
    """Each simple command's words; quoted text, newlines included, stays inside one word."""
    lex = shlex.shlex(command, posix=True, punctuation_chars=OPERATORS)
    lex.whitespace, lex.whitespace_split = " \t\r", True
    try:
        words = list(lex)
    except ValueError:
        words = re.split(r"[ \t\r]+|([;&|()\n`])", command)
    out = [[]]
    for word in filter(None, words):
        if set(word) <= set(OPERATORS + "`"):
            out.append([])
        else:
            out[-1].append(word)
    return out


def recursive_rm(args: list[str]) -> bool:
    for arg in args:
        if arg == "--":
            return False
        if arg == "--recursive" or re.fullmatch(r"-[A-Za-z]*[rR][A-Za-z]*", arg):
            return True
    return False


def find_deletes(args: list[str]) -> bool:
    if "-delete" in args:
        only_empty_dirs = "-empty" in args and any(
            a == "-type" and b == "d" for a, b in zip(args, args[1:]))
        if not only_empty_dirs:
            return True
    for i, arg in enumerate(args):
        if arg in ("-exec", "-execdir", "-ok", "-okdir") and i + 1 < len(args):
            if args[i + 1].rsplit("/", 1)[-1] in REMOVERS:
                return True
    return False


def command_words(words: list[str]) -> list[str]:
    """The command and its arguments, past VAR=value assignments and wrappers like sudo."""
    i = 0
    while i < len(words) and re.fullmatch(r"\w+=.*", words[i]):
        i += 1
    if i < len(words) and words[i] in WRAPPERS:
        for j in range(i + 1, len(words)):
            if words[j].rsplit("/", 1)[-1] in CHECKED:
                return words[j:]
    return words[i:]


def git_subcommand(args: list[str]) -> list[str]:
    i = 0
    while i < len(args) and args[i].startswith("-"):
        i += 2 if args[i] in ("-C", "-c") else 1
    return args[i:]


def reason(command: str) -> str | None:
    if re.search(r"\brmtree\s*\(", command):
        return "shutil.rmtree"
    for inner in SUBSTITUTION.finditer(command):
        if found := reason(inner.group(1) or inner.group(2) or ""):
            return found
    for segment in segments(command):
        words = command_words(segment)
        if not words:
            continue
        name, rest = words[0].rsplit("/", 1)[-1], words[1:]
        if name == "rm" and recursive_rm(rest):
            return "rm -r"
        if name == "find" and find_deletes(rest):
            return "find -delete / -exec rm"
        if name == "xargs" and any(w.rsplit("/", 1)[-1] in REMOVERS for w in rest):
            return "xargs rm"
        if name == "git" and git_subcommand(rest)[:1] == ["clean"]:
            return "git clean"
        if name == "git" and git_subcommand(rest)[:1] == ["rm"] and recursive_rm(rest):
            return "git rm -r"
        if name == "rsync" and any(w.startswith("--delete") for w in rest):
            return "rsync --delete"
    return None


payload = json.load(sys.stdin)
found = reason(payload.get("tool_input", {}).get("command", ""))
if found:
    print(f"Blocked ({found}): no recursive deletes in this project, for any agent. Delete files "
          "by name or by a glob in one directory, then rmdir the empty directory; anything larger "
          "goes to the maintainer as a list of what would go.", file=sys.stderr)
    sys.exit(2)
