#!/bin/bash
# Landing rule scans over changed and new files, against HEAD or the ref given (main, for a
# pull request merged in a worktree).
B=${1:-HEAD}
F=$( (git diff "$B" --name-only -- src include; git ls-files --others --exclude-standard -- src include) | sort -u)
D=$(git diff "$B" -U0 -- src include; for f in $(git ls-files --others --exclude-standard -- src include); do sed 's/^/+/' "$f"; done)
echo "== pragma"; echo "$D" | grep -E '^\+.*#pragma' | grep -vE 'pragma (once|pack|warning)'
echo "== asm/volatile"; echo "$D" | grep -E '^\+.*(__asm|\bvolatile\b)'
echo "== home/host"; echo "$D" | grep -E "^\+.*(/home/|$(id -un)|$(hostname)|Users\\\\)"
echo "== SPDX"; for f in $F; do [ -f "$f" ] && head -1 "$f" | grep -q SPDX || echo "$f"; done
echo "== Init_ stand-ins"; echo "$D" | grep -E '^\+.*\bInit_[A-Za-z0-9_]*\s*\(' | grep -v '//' | head
echo "== casts over objects"; echo "$D" | grep -E '^\+.*\(\([A-Za-z_]+ *\*\) *&'
echo "== local header-run structs"; echo "$D" | grep -E '^\+\s*struct (ShortPair|IntTriple|IntQuad|IntInit|ZeroQuad|ColorTriple)'
echo "== Session/Options aliases"; echo "$D" | grep -E '^\+.*DAT_00A8(B2[3-9A-F]|B[3-9A-F]|C|D|E[0-2]|E3[01])[0-9A-F]*\b|DAT_00A8EB[6-9A-F][0-9A-F]|DAT_00A8EC0|DAT_00A8EC1[0-7]'
echo "== block comments"; echo "$D" | grep -E '^\+.*/\*' | head
echo "== 3+ comment lines"; for f in $F; do [ -f "$f" ] && awk -v f="$f" '/^[ \t]*\/\/ (FUNCTION|TEMPLATE|SYNTHETIC|LIBRARY|GLOBAL|VTABLE|STRING|SYMBOL)/{n=0;next} /^[ \t]*\/\/ [?_]/{n=0;next} /^[ \t]*\/\//{n++; if(n==3) print f":"NR; next} {n=0}' "$f"; done | head -30
echo "== game data (any path)"; for f in $( (git diff "$B" --name-only --diff-filter=AM; git ls-files --others --exclude-standard) | sort -u); do [ -f "$f" ] || continue; case "${f,,}" in *.mix|*.shp|*.vxl|*.hva|*.pal|*.aud|*.wav|*.bik|*.vqa|*.csf|*.map|*.mpr|*.yrm|*.mmx|*.pcx|*.fnt|*.exe|*.dll|*.gpr|*.idb|*.i64) echo "$f: game data type";; esac; [ -s "$f" ] && ! grep -qI . "$f" && echo "$f: binary content"; done
echo "== new local classes/structs (review)"; echo "$D" | grep -E '^\+(struct|class) [A-Za-z_0-9]+' | grep -v ';$' | head -30

# Findings are advisory. An empty final search is also a successful scan.
exit 0
