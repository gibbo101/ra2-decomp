"""Conservative VC6 include dependencies for verified object reuse.

Consider every conditional branch and every file with the requested basename,
including SDK headers. Overestimating readers is safe; ambiguous syntax uses the
whole-tree key. A new shadowing file changes the key even on a case-sensitive host.
"""
from collections import defaultdict
import hashlib
import re
from pathlib import PurePosixPath

# Preserve strings and line breaks when removing comments (including across directives).
TOKENS = re.compile(rb'''//[^\n]*|/\*[\s\S]*?(?:\*/|\Z)|"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*' ''', re.X)
DIRECTIVE = re.compile(rb"(?m)^[^\S\n]*#[^\S\n]*(include(?:_next)?|import)\b(.*)", re.I)
LITERAL = re.compile(rb'\s*(?:"([^"]+)"|<([^>]+)>)\s*$')


def includes(data):
    data = data.replace(b"\\\r\n", b"").replace(b"\\\n", b"")
    # Alternative directive spellings and generated #line filenames need a
    # conservative fallback rather than approximating their preprocessor semantics.
    if any(s in data for s in (b"%:", b"??=", b"??/", b"\x00")):
        return None
    data = TOKENS.sub(lambda m: (b" " + b"\n" * m[0].count(b"\n")
                                 if m[0].startswith(b"/") else m[0]), data)
    edges = set()
    for directive, operand in DIRECTIVE.findall(data):
        m = LITERAL.fullmatch(operand)
        if directive.lower() != b"include" or not m:
            return None
        path = (m[1] or m[2]).decode("latin-1").replace("\\", "/")
        if path.startswith("/") or ":" in path or ".." in path.split("/"):
            return None
        edges.add(path.rsplit("/", 1)[-1].casefold())
    return edges


class HeaderInputs:
    def __init__(self):
        self.files = {}
        self.by_name = defaultdict(list)
        self.global_key = hashlib.sha256()

    def add(self, path, data, entry):
        sdk = path.startswith("toolchain/")
        if sdk or path.startswith("tools/"):
            self.global_key.update(entry)
        if path.startswith("tools/"):
            return
        p = PurePosixPath(path)
        # prepare_headers copies only these suffixes from include/ and the archive.
        if path.startswith(("reference/", "include/")) and p.suffix.lower() not in (".h", ".hh", ".hpp", ".inl"):
            return
        if sdk and "/include/" not in path.casefold():
            return
        self.by_name[p.name.casefold()].append(path)
        self.files[path] = (entry, includes(data))

    def keys(self, units, tree):
        result = {}
        for unit in units:
            src = unit[:-4]
            todo, seen = [src], set()
            while todo:
                path = todo.pop()
                if path in seen:
                    continue
                seen.add(path)
                _, names = self.files[path]
                if names is None or not src.endswith(".cpp"):
                    result[unit] = tree
                    break
                for name in names:
                    todo.extend(self.by_name.get(name, ()))
            else:
                key = self.global_key.copy()
                for path in sorted(seen):
                    key.update(self.files[path][0])
                result[unit] = key.hexdigest()
        return result
