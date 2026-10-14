#!/usr/bin/env python3
"""PreToolUse hook: refuse any git command from a subagent.

The working tree is shared by every agent of a batch, so one agent's stash, checkout or
reset takes the others' uncommitted work with it. Subagent calls carry an agent_id in the
hook input; the coordinator's do not, so it keeps git for committing and recovery.
"""

import json
import re
import shlex
import sys

OPERATORS = ";&|()\n"
SUBSTITUTION = re.compile(r"\$\(([^()]*)\)|`([^`]*)`")
WRAPPERS = {"sudo", "env", "nice", "nohup", "time", "command", "exec", "timeout", "doas", "xargs"}


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


def runs_git(command: str) -> bool:
    for inner in SUBSTITUTION.finditer(command):
        if runs_git(inner.group(1) or inner.group(2) or ""):
            return True
    for words in segments(command):
        i = 0
        while i < len(words) and re.fullmatch(r"\w+=.*", words[i]):
            i += 1
        names = [w.rsplit("/", 1)[-1] for w in words[i:]]
        if names[:1] == ["git"]:
            return True
        if names[:1] and names[0] in WRAPPERS and "git" in names:
            return True
    return False


payload = json.load(sys.stdin)
if payload.get("agent_id") and runs_git(payload.get("tool_input", {}).get("command", "")):
    print("Blocked: agents may not run git in this project, not even read-only. The tree is "
          "shared; report what you needed and the coordinator will handle it.", file=sys.stderr)
    sys.exit(2)
