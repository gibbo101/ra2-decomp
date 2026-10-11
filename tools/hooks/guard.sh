#!/usr/bin/env bash
# Blocks personal data, secrets and game binaries from commits and pushes.
# Usage and setup: docs/COMMIT_GUARD.md.
set -euo pipefail

private_list="$(git rev-parse --git-common-dir)/info/private-strings"

secret_patterns=(
  '-----BEGIN [A-Z ]*PRIVATE KEY-----'
  'gh[pousr]_[A-Za-z0-9]{36}'
  'github_pat_[A-Za-z0-9_]{22,}'
  'sk-ant-[A-Za-z0-9_-]{20,}'
  'sk-[A-Za-z0-9]{32,}'
  'AKIA[0-9A-Z]{16}'
  'xox[abprs]-[A-Za-z0-9-]{10,}'
  'AIza[0-9A-Za-z_-]{35}'
)
email_pattern='[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}'
email_allowed='(@users\.noreply\.github\.com|^noreply@[A-Za-z0-9.-]+|^git@github\.com)$'
home_path_pattern='(/home/|/Users/|[A-Za-z]:\\Users\\)[A-Za-z0-9._-]+'
blocked_name_pattern='(^|/)(\.env(\..*)?|id_(rsa|dsa|ecdsa|ed25519)(\.pub)?|credentials[^/]*|[^/]*\.(pem|key|p12|pfx|keystore))$'

problems=0
report() { echo "guard: $1" >&2; problems=1; }

private_entries() {
  [ -s "$private_list" ] && grep -v '^[[:space:]]*$' "$private_list" || true
}

scan_text() {  # $1: where the text came from; the text arrives on stdin
  local where=$1 text hit
  text=$(cat)
  [ -n "$text" ] || return 0
  for pattern in "${secret_patterns[@]}"; do
    if hit=$(grep -oE -e "$pattern" <<<"$text" | head -1) && [ -n "$hit" ]; then
      report "$where: looks like a secret (${hit:0:10}...)"
    fi
  done
  hit=$(grep -oE "$email_pattern" <<<"$text" | grep -vE "$email_allowed" | sort -u || true)
  [ -z "$hit" ] || report "$where: email address: $(tr '\n' ' ' <<<"$hit")"
  hit=$(grep -oE "$home_path_pattern" <<<"$text" | sort -u || true)
  [ -z "$hit" ] || report "$where: home-directory path: $(tr '\n' ' ' <<<"$hit")"
  local entries
  entries=$(private_entries)
  if [ -n "$entries" ]; then
    hit=$(grep -oiwF -f <(printf '%s\n' "$entries") <<<"$text" | sort -uf || true)
    [ -z "$hit" ] || report "$where: private string: $(tr '\n' ' ' <<<"$hit")"
  fi
}

check_email() {  # $1: whose address, $2: the address
  grep -qE "$email_allowed" <<<"$2" ||
    report "$1 email <$2> is not a no-reply address; set user.email to the GitHub no-reply one"
}

added_lines() { grep '^+' | grep -v '^+++' | cut -c2- || true; }

check_staged() {
  local ident
  ident=$(git var GIT_AUTHOR_IDENT); ident=${ident#*<}; check_email author "${ident%%>*}"
  ident=$(git var GIT_COMMITTER_IDENT); ident=${ident#*<}; check_email committer "${ident%%>*}"

  while IFS= read -r -d '' path; do
    if grep -qiE "$blocked_name_pattern" <<<"$path"; then
      report "$path: file name looks like a secret"
    fi
    if [ "$(git cat-file -p ":$path" | head -c 2)" = MZ ]; then
      report "$path: Windows executable (game files and compilers are never committed)"
    fi
  done < <(git diff --cached --name-only --diff-filter=ACMR -z)

  scan_text "staged changes" < <(git diff --cached -U0 --no-color --diff-filter=ACMR | added_lines)
}

check_message() {
  scan_text "commit message" < <(grep -v '^#' "$1" || true)
}

check_push() {
  local zero=0000000000000000000000000000000000000000 range email
  while read -r _ local_sha _ remote_sha; do
    [ "$local_sha" = "$zero" ] && continue
    if [ "$remote_sha" = "$zero" ]; then range=("$local_sha" --not --remotes)
    else range=("$remote_sha..$local_sha"); fi
    while read -r email; do check_email "a pushed commit's" "$email"; done < \
      <(git log --format='%ae%n%ce' "${range[@]}" | sort -u)
    scan_text "pushed commit messages" < <(git log --format=%B "${range[@]}")
    scan_text "pushed changes" < <(git log -p --no-color --format= "${range[@]}" | added_lines)
  done
}

case ${1:-} in
  staged) check_staged ;;
  message) check_message "$2" ;;
  push) check_push ;;
  *) echo "usage: guard.sh staged | message <file> | push" >&2; exit 2 ;;
esac

if [ "$problems" -ne 0 ]; then
  echo "guard: blocked. Remove the data above; never bypass with --no-verify." >&2
  exit 1
fi
