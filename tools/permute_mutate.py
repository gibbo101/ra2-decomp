# Adapted from Byte Tactics (tools/permute_mutate.py), whose tools/ are under this licence:
#
# MIT License
#
# Copyright (c) 2026 Hector Bailey
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""Meaning-preserving source mutations for tools/permute.py.

The C++ is parsed with tree-sitter. Each mutation rewrites a small part of the
target function (or of an inline helper it calls) in a way that cannot change
what the program computes: it only changes how the compiler lays out, orders and
allocates the code. Every mutation checks the conditions it needs (no side
effects in reordered operands, no dependency between swapped statements, no
`continue` in a loop being rewritten, and so on) and gives up when it cannot
prove them; the compiler then rejects anything that does not compile.

Text is handled as latin-1, so one character is one byte and tree-sitter's
byte offsets index the string directly.
"""

from __future__ import annotations

import random
import re
from dataclasses import dataclass, field

import tree_sitter_cpp
from tree_sitter import Language, Node, Parser

LANG = Language(tree_sitter_cpp.language())
_PARSER = Parser(LANG)

# tree-sitter-cpp cannot parse a few MSVC spellings (a calling convention in a
# function declaration, `operator delete(p)` as a call, `__inline`). They are
# replaced by text of the same length before parsing, so every offset still
# points into the real source.
_SANITISE = re.compile(
    r"\b(?:__stdcall|__cdecl|__fastcall|__thiscall|_stdcall|_cdecl|_fastcall)\b"
    r"|\b(?:__inline|__forceinline)\b"
    r"|\boperator\s*(?:new|delete)\b(?:\s*\[\s*\])?")


def sanitise(text: str) -> str:
    def sub(m: re.Match) -> str:
        s = m.group(0)
        if s.startswith("operator"):
            return re.sub(r"[^A-Za-z0-9_]", "_", s)
        if s in ("__inline", "__forceinline"):
            return "inline" + " " * (len(s) - 6)
        return " " * len(s)
    return _SANITISE.sub(sub, text)


def parse(text: str):
    return _PARSER.parse(sanitise(text).encode("latin-1"))


ANNOTATION = re.compile(r"^\s*//\s*FUNCTION:\s*(0x[0-9a-fA-F]+)(?:\s+(\S+))?")

INT_TYPES = {
    "char", "signed char", "unsigned char", "short", "unsigned short", "int", "unsigned int",
    "long", "unsigned long", "bool", "BOOL", "BYTE", "WORD", "DWORD", "UINT", "ULONG", "LONG",
    "USHORT", "UCHAR", "size_t", "__int8", "__int16", "__int32", "unsigned __int8",
    "unsigned __int16", "unsigned __int32", "wchar_t", "HRESULT",
}
FLOAT_TYPES = {"float", "double", "long double"}
TYPE_ALIASES = {
    "unsigned": "unsigned int", "signed": "int", "signed int": "int", "long int": "long",
    "unsigned long int": "unsigned long", "short int": "short", "signed short": "short",
    "unsigned short int": "unsigned short", "signed long": "long", "signed char": "signed char",
}
# Calls that neither write memory nor depend on it: they can move freely.
PURE_CALLS = {"abs", "labs", "fabs", "sqrt", "sin", "cos", "tan", "atan", "atan2", "asin", "acos",
              "floor", "ceil", "exp", "log", "log10", "pow", "fmod", "toupper", "tolower",
              "isdigit", "isalpha", "isspace", "isupper", "islower", "isalnum", "isprint",
              "isxdigit", "ispunct", "_toupper", "_tolower"}
# Calls that read memory but write nothing.
READ_CALLS = {"strlen", "strcmp", "stricmp", "_stricmp", "strncmp", "strnicmp", "_strnicmp",
              "memcmp", "strchr", "strrchr", "strstr", "_strcmpi", "strcmpi", "atoi", "atol", "atof"}
STD_HEADERS = ["windows.h", "stdio.h", "stdlib.h", "string.h", "math.h", "memory.h"]

STATEMENT_TYPES = {
    "expression_statement", "declaration", "if_statement", "for_statement", "while_statement",
    "do_statement", "return_statement", "compound_statement", "switch_statement",
    "break_statement", "continue_statement", "goto_statement", "labeled_statement",
    "case_statement", "type_definition", "for_range_loop", "throw_statement", "try_statement",
}
LOOPS = {"for_statement", "while_statement", "do_statement", "for_range_loop"}
UNKNOWN = ("?", ())
FOCUS = 0.75  # how often a mutation picks among the sites on hot lines


def canon_type(s: str) -> str:
    s = re.sub(r"\b(const|volatile|register|static|struct|class|union|enum)\b", " ", s)
    s = re.sub(r"\s+", " ", s).strip()
    s = re.sub(r"\s*\*\s*", "*", s)
    base = s.rstrip("*[]&")
    return TYPE_ALIASES.get(base, base) + s[len(base):]


def is_int(t: str | None) -> bool:
    return t is not None and t in INT_TYPES


def is_float(t: str | None) -> bool:
    return t is not None and t in FLOAT_TYPES


def is_ptr(t: str | None) -> bool:
    return t is not None and (t.endswith("*") or t.endswith("[]"))


def is_scalar(t: str | None) -> bool:
    return is_int(t) or is_float(t) or is_ptr(t)


UNSIGNED32 = {"unsigned int", "unsigned long", "DWORD", "UINT", "ULONG", "size_t", "unsigned __int32"}


def promote(t: str | None) -> str | None:
    """The type of t after the integer promotions."""
    if t is None or is_float(t) or is_ptr(t):
        return t
    if t in UNSIGNED32:
        return "unsigned int"
    if is_int(t):
        return "int"
    return None


def arith(a: str | None, b: str | None) -> str | None:
    """The type of a binary arithmetic expression (usual conversions)."""
    if a is None or b is None:
        return None
    if "long double" in (a, b) or "double" in (a, b):
        return "double"
    if "float" in (a, b):
        return "float"
    pa, pb = promote(a), promote(b)
    if pa is None or pb is None or is_ptr(pa) or is_ptr(pb):
        return None
    if "unsigned int" in (pa, pb):
        return "unsigned int"
    return "int"


def int_literal(s: str) -> int | None:
    s = s.strip().rstrip("uUlL")
    try:
        if re.fullmatch(r"0[xX][0-9a-fA-F]+", s):
            return int(s, 16)
        if re.fullmatch(r"0[0-7]*", s):
            return int(s, 8)
        if re.fullmatch(r"[1-9][0-9]*", s):
            return int(s)
    except ValueError:
        return None
    return None


# --- file-level knowledge ---------------------------------------------------


@dataclass
class StructInfo:
    name: str
    fields: dict[str, str] = field(default_factory=dict)
    methods: dict[str, str] = field(default_factory=dict)        # name -> return type
    method_params: dict[str, list[list[str]]] = field(default_factory=dict)
    bases: list[str] = field(default_factory=list)
    has_ctor: bool = False
    is_union: bool = False
    nontrivial: bool = False      # default construction or destruction runs code


@dataclass
class FileInfo:
    structs: dict[str, StructInfo] = field(default_factory=dict)
    globals: dict[str, str] = field(default_factory=dict)
    funcs: dict[str, str] = field(default_factory=dict)            # name -> return type
    func_params: dict[str, list[list[str]]] = field(default_factory=dict)
    consts: set[str] = field(default_factory=set)
    macros: set[str] = field(default_factory=set)                   # function-like macros
    union_fields: set[str] = field(default_factory=set)
    types: set[str] = field(default_factory=set)

    def struct(self, name: str | None) -> StructInfo | None:
        if not name:
            return None
        name = re.sub(r"<.*", "", name).split("::")[-1].strip()
        return self.structs.get(name)

    def field_type(self, cls: str | None, name: str, depth: int = 0) -> str | None:
        st = self.struct(cls)
        if st is None or depth > 6:
            return None
        if name in st.fields:
            return st.fields[name]
        for b in st.bases:
            t = self.field_type(b, name, depth + 1)
            if t is not None:
                return t
        return None

    def has_field(self, cls: str | None, name: str, depth: int = 0) -> bool:
        st = self.struct(cls)
        if st is None or depth > 6:
            return False
        return name in st.fields or any(self.has_field(b, name, depth + 1) for b in st.bases)

    def nontrivial(self, name: str | None, depth: int = 0) -> bool:
        """Does declaring a `name` local without an initialiser run code
        (a default constructor or destructor with a body, a vtable)?"""
        if name is None:
            return True
        st = self.struct(name)
        if st is None or depth > 6:
            return True
        if st.nontrivial:
            return True
        for b in st.bases:
            if self.nontrivial(b, depth + 1):
                return True
        for t in st.fields.values():
            base = t.rstrip("[]")
            if base.endswith("*") or base in INT_TYPES or base in FLOAT_TYPES:
                continue
            if self.nontrivial(base, depth + 1):
                return True
        return False

    def method_type(self, cls: str | None, name: str, depth: int = 0) -> str | None:
        st = self.struct(cls)
        if st is None or depth > 6:
            return None
        if name in st.methods:
            return st.methods[name]
        for b in st.bases:
            t = self.method_type(b, name, depth + 1)
            if t is not None:
                return t
        return None


def unwrap_declarator(d: Node | None, text: str) -> tuple[str | None, str, Node | None]:
    """(name, suffix of * [] & (), init value) of a declarator."""
    init = None
    suffix = ""
    while d is not None:
        t = d.type
        if t == "init_declarator":
            init = d.child_by_field_name("value")
            d = d.child_by_field_name("declarator")
        elif t == "pointer_declarator":
            suffix += "*"
            d = d.child_by_field_name("declarator")
        elif t == "reference_declarator":
            suffix += "&"
            d = d.named_children[-1] if d.named_children else None
        elif t == "array_declarator":
            suffix += "[]"
            d = d.child_by_field_name("declarator")
        elif t == "parenthesized_declarator":
            suffix += "()"
            d = d.named_children[0] if d.named_children else None
        elif t == "function_declarator":
            suffix += "(fn)"
            d = d.child_by_field_name("declarator")
        elif t in ("identifier", "field_identifier"):
            return text[d.start_byte:d.end_byte], suffix, init
        elif t == "bitfield_clause":
            return None, suffix, init
        else:
            return text[d.start_byte:d.end_byte] if t in ("qualified_identifier", "operator_name",
                                                           "destructor_name") else None, suffix, init
    return None, suffix, init


def make_type(base: str, suffix: str) -> str:
    if "(fn)" in suffix or "()" in suffix:
        return "fn"
    t = canon_type(base)
    if "&" in suffix:
        suffix = suffix.replace("&", "")
    stars = suffix.count("*")
    arr = suffix.count("[]")
    out = t + "*" * stars
    if arr:
        out += "[]"
    return out


def param_types(fdecl: Node, text: str) -> list[str]:
    params = fdecl.child_by_field_name("parameters")
    out = []
    if params is None:
        return out
    for p in params.named_children:
        if p.type not in ("parameter_declaration", "optional_parameter_declaration"):
            continue
        tnode = p.child_by_field_name("type")
        base = text[tnode.start_byte:tnode.end_byte] if tnode else ""
        _, suffix, _ = unwrap_declarator(p.child_by_field_name("declarator"), text)
        out.append(("&" if "&" in suffix else "") + make_type(base, suffix))
    return out


def find_function_declarator(d: Node | None) -> Node | None:
    while d is not None and d.type != "function_declarator":
        if d.type in ("pointer_declarator", "reference_declarator", "parenthesized_declarator",
                      "init_declarator", "attributed_declarator"):
            d = d.child_by_field_name("declarator") or (d.named_children[-1] if d.named_children else None)
        else:
            return None
    return d


def build_file_info(root: Node, text: str) -> FileInfo:
    fi = FileInfo()

    def T(n: Node | None) -> str:
        return text[n.start_byte:n.end_byte] if n is not None else ""

    def struct_body(node: Node, st: StructInfo):
        body = node.child_by_field_name("body")
        if body is None:
            return
        for c in body.named_children:
            if c.type in ("field_declaration", "declaration"):
                tnode = c.child_by_field_name("type")
                if tnode is not None and tnode.type in ("struct_specifier", "class_specifier", "union_specifier"):
                    visit_struct(tnode)
                base = T(tnode)
                for d in c.children_by_field_name("declarator"):
                    name, suffix, _ = unwrap_declarator(d, text)
                    fd = find_function_declarator(d)
                    if fd is not None:
                        if name:
                            short = name.split("::")[-1]
                            st.methods[short] = make_type(base, suffix.replace("(fn)", ""))
                            st.method_params.setdefault(short, []).append(param_types(fd, text))
                            if short == st.name or short == "~" + st.name:
                                st.has_ctor = True
                                if short != st.name or not param_types(fd, text):
                                    st.nontrivial = True  # body unknown
                            if any(k.type == "virtual" for k in c.children):
                                st.nontrivial = True
                    elif name:
                        st.fields[name] = make_type(base, suffix)
                        if st.is_union:
                            fi.union_fields.add(name)
            elif c.type == "function_definition":
                fd = find_function_declarator(c.child_by_field_name("declarator"))
                name = T(fd.child_by_field_name("declarator")) if fd is not None else ""
                short = name.split("::")[-1]
                if short:
                    tnode = c.child_by_field_name("type")
                    _, suffix, _ = unwrap_declarator(c.child_by_field_name("declarator"), text)
                    st.methods[short] = make_type(T(tnode), suffix.replace("(fn)", "")) if tnode else "void"
                    st.method_params.setdefault(short, []).append(param_types(fd, text))
                    if short == st.name or short == "~" + st.name:
                        st.has_ctor = True
                        body = c.child_by_field_name("body")
                        runs = any(k.type == "field_initializer_list" for k in c.children) or (
                            body is not None and any(k.type != "comment" for k in body.named_children))
                        if runs and (short != st.name or not param_types(fd, text)):
                            st.nontrivial = True
                    if any(k.type == "virtual" for k in c.children):
                        st.nontrivial = True
            elif c.type in ("struct_specifier", "class_specifier", "union_specifier"):
                visit_struct(c)
            elif c.type == "virtual_function_specifier":
                st.has_ctor = True
                st.nontrivial = True
        if st.is_union:
            # every name declared anywhere inside a union may overlap another
            stack = [body]
            while stack:
                n = stack.pop()
                if n.type == "field_identifier":
                    fi.union_fields.add(T(n))
                stack.extend(n.named_children)

    def visit_struct(node: Node):
        name_node = node.child_by_field_name("name")
        name = re.sub(r"<.*", "", T(name_node)).split("::")[-1].strip() if name_node else ""
        st = StructInfo(name or f"<anon{node.start_byte}>", is_union=node.type == "union_specifier")
        for c in node.named_children:
            if c.type == "base_class_clause":
                for b in c.named_children:
                    if b.type in ("type_identifier", "qualified_type_identifier", "template_type"):
                        st.bases.append(re.sub(r"<.*", "", T(b)).split("::")[-1])
        struct_body(node, st)
        if name:
            fi.types.add(name)
            old = fi.structs.get(name)
            if old is None or (not old.fields and st.fields) or len(st.fields) + len(st.methods) > len(old.fields) + len(old.methods):
                fi.structs[name] = st
        if node.type == "union_specifier":
            for f in st.fields:
                fi.union_fields.add(f)

    def visit(node: Node, top: bool):
        for c in node.named_children:
            t = c.type
            if t in ("struct_specifier", "class_specifier", "union_specifier"):
                visit_struct(c)
            elif t == "enum_specifier":
                for e in c.named_children:
                    if e.type == "enumerator_list":
                        for en in e.named_children:
                            if en.type == "enumerator":
                                fi.consts.add(T(en.child_by_field_name("name")))
                if c.child_by_field_name("name") is not None:
                    fi.types.add(T(c.child_by_field_name("name")))
            elif t in ("declaration", "field_declaration"):
                tnode = c.child_by_field_name("type")
                if tnode is not None and tnode.type in ("struct_specifier", "class_specifier", "union_specifier"):
                    visit_struct(tnode)
                if tnode is not None and tnode.type == "enum_specifier":
                    for e in tnode.named_children:
                        if e.type == "enumerator_list":
                            for en in e.named_children:
                                if en.type == "enumerator":
                                    fi.consts.add(T(en.child_by_field_name("name")))
                base = T(tnode)
                for d in c.children_by_field_name("declarator"):
                    name, suffix, _ = unwrap_declarator(d, text)
                    if not name:
                        continue
                    fd = find_function_declarator(d)
                    short = name.split("::")[-1]
                    if fd is not None:
                        fi.funcs[short] = make_type(base, suffix.replace("(fn)", ""))
                        fi.func_params.setdefault(short, []).append(param_types(fd, text))
                    else:
                        fi.globals[short] = make_type(base, suffix)
            elif t == "function_definition":
                fd = find_function_declarator(c.child_by_field_name("declarator"))
                if fd is not None:
                    name = T(fd.child_by_field_name("declarator"))
                    short = name.split("::")[-1]
                    tnode = c.child_by_field_name("type")
                    _, suffix, _ = unwrap_declarator(c.child_by_field_name("declarator"), text)
                    if "::" in name:
                        cls = name.split("::")[-2]
                        st = fi.structs.get(cls)
                        if st is not None and short not in st.methods:
                            st.methods[short] = make_type(T(tnode), suffix.replace("(fn)", "")) if tnode else "void"
                    else:
                        fi.funcs[short] = make_type(T(tnode), suffix.replace("(fn)", "")) if tnode else "void"
                        fi.func_params.setdefault(short, []).append(param_types(fd, text))
            elif t == "type_definition":
                for d in c.children_by_field_name("declarator"):
                    name, _, _ = unwrap_declarator(d, text)
                    if name:
                        fi.types.add(name)
                tnode = c.child_by_field_name("type")
                if tnode is not None and tnode.type in ("struct_specifier", "class_specifier", "union_specifier"):
                    visit_struct(tnode)
            elif t == "preproc_def":
                fi.consts.add(T(c.child_by_field_name("name")))
            elif t == "preproc_function_def":
                fi.macros.add(T(c.child_by_field_name("name")))
            elif t in ("namespace_definition", "declaration_list", "linkage_specification",
                       "template_declaration", "preproc_if", "preproc_ifdef", "preproc_else",
                       "preproc_elif"):
                visit(c, top)

    visit(root, True)
    return fi


# --- functions ---------------------------------------------------------------


CONTAINERS = {"namespace_definition", "declaration_list", "linkage_specification",
              "template_declaration", "preproc_if", "preproc_ifdef", "preproc_else", "preproc_elif",
              "field_declaration_list"}


def function_definitions(root: Node, text: str) -> list[tuple[Node, str | None]]:
    """Every function definition outside function bodies, with its class name
    when it is defined inside a class body."""
    out = []
    stack: list[tuple[Node, str | None]] = [(root, None)]
    while stack:
        node, cls = stack.pop()
        for c in node.named_children:
            t = c.type
            if t == "function_definition":
                out.append((c, cls))
            elif t in ("struct_specifier", "class_specifier", "union_specifier"):
                n = c.child_by_field_name("name")
                stack.append((c, re.sub(r"<.*", "", text[n.start_byte:n.end_byte]).split("::")[-1] if n else cls))
            elif t in ("declaration", "field_declaration", "type_definition"):
                tn = c.child_by_field_name("type")
                if tn is not None and tn.type in ("struct_specifier", "class_specifier", "union_specifier"):
                    stack.append((c, cls))
            elif t in CONTAINERS:
                stack.append((c, cls))
    out.sort(key=lambda x: x[0].start_byte)
    return out


def function_name(defn: Node, text: str) -> str:
    fd = find_function_declarator(defn.child_by_field_name("declarator"))
    if fd is None:
        return ""
    n = fd.child_by_field_name("declarator")
    return re.sub(r"\s+", "", text[n.start_byte:n.end_byte]) if n is not None else ""


def symbol_method_name(sym: str) -> tuple[str | None, str | None]:
    """(name, class) a mangled symbol most likely defines."""
    if sym.startswith("??0") or sym.startswith("??1"):
        cls = sym[3:].split("@", 1)[0]
        return ("~" + cls if sym[2] == "1" else cls), cls
    if sym.startswith("?") and not sym.startswith("??"):
        parts = sym[1:].split("@@", 1)[0].split("@")
        cls = parts[1] if len(parts) > 1 else None
        if cls and cls.startswith("?$"):
            cls = cls[2:]
        return parts[0], cls
    if sym.startswith("_") and not sym.startswith("_$"):
        return sym[1:].split("@", 1)[0], None
    return None, None


@dataclass
class TargetSpec:
    """How to find the functions to mutate in any version of the file."""
    address: int
    symbol: str | None = None              # explicit symbol from the annotation
    helpers: list[str] = field(default_factory=list)   # names of inline helpers
    stack_names: set[str] = field(default_factory=set)  # locals stackcmp.py flags as wrong


def find_targets(root: Node, text: str, spec: TargetSpec) -> tuple[list[tuple[Node, str | None]], str]:
    """The function definition(s) for the annotated address plus the named helpers."""
    defs = function_definitions(root, text)
    primary: list[tuple[Node, str | None]] = []
    ann_line = None
    for i, line in enumerate(text.split("\n")):
        m = ANNOTATION.match(line)
        if m and int(m.group(1), 16) == spec.address:
            ann_line = i
            break
    if ann_line is None:
        return [], f"no // FUNCTION: {spec.address:#x} annotation"
    if spec.symbol:
        name, cls = symbol_method_name(spec.symbol)
        if not name:
            return [], f"cannot tell which definition {spec.symbol} is"
        named = [(d, c) for d, c in defs if function_name(d, text).split("::")[-1] == name]
        in_class = [(d, c) for d, c in named
                    if cls and (c == cls or function_name(d, text).endswith(cls + "::" + name))]
        primary = in_class or named
    else:
        after = [(d, c) for d, c in defs if d.start_point[0] > ann_line]
        if after:
            d, c = after[0]
            between = text.split("\n")[ann_line + 1:d.start_point[0]]
            if d.start_point[0] - ann_line <= 15 or all(
                    not l.strip() or l.strip().startswith("//") for l in between):
                primary.append((d, c))
    if not primary:
        return [], "the annotated definition was not found"
    out = list(primary)
    if spec.helpers:
        for d, c in defs:
            if any(d is p for p, _ in primary):
                continue
            if function_name(d, text).split("::")[-1] in spec.helpers and not any(d.id == p.id for p, _ in out):
                out.append((d, c))
    return out, ""


def called_names(node: Node, text: str) -> set[str]:
    out = set()
    stack = [node]
    while stack:
        n = stack.pop()
        if n.type == "call_expression":
            f = n.child_by_field_name("function")
            if f is not None:
                if f.type == "identifier":
                    out.add(text[f.start_byte:f.end_byte])
                elif f.type == "field_expression":
                    fld = f.child_by_field_name("field")
                    if fld is not None:
                        out.add(text[fld.start_byte:fld.end_byte])
                elif f.type in ("qualified_identifier", "template_function"):
                    out.add(re.sub(r"<.*", "", text[f.start_byte:f.end_byte]).split("::")[-1])
        stack.extend(n.named_children)
    return out


def helper_names(text: str, spec: TargetSpec, max_lines: int = 60) -> list[str]:
    """Inline helpers defined in the file (not annotated) that the target calls,
    directly or through other helpers."""
    tree = parse(text)
    root = tree.root_node
    targets, err = find_targets(root, text, TargetSpec(spec.address, spec.symbol))
    if err:
        return []
    defs = function_definitions(root, text)
    annotated = set()
    for i, line in enumerate(text.split("\n")):
        if ANNOTATION.match(line):
            # the first definition after an annotation is a scored function itself
            after = [d for d, _ in defs if d.start_point[0] > i]
            if after and after[0].start_point[0] - i <= 15:
                annotated.add(after[0].id)
    candidates = {}
    for d, c in defs:
        if d.id in annotated:
            continue
        if d.end_point[0] - d.start_point[0] > max_lines:
            continue
        body = d.child_by_field_name("body")
        if body is None:
            continue
        name = function_name(d, text).split("::")[-1]
        if name:
            candidates.setdefault(name, []).append(d)
    seen: set[str] = set()
    todo = []
    for d, _ in targets:
        todo.extend(called_names(d, text))
    while todo:
        n = todo.pop()
        if n in seen or n not in candidates:
            continue
        seen.add(n)
        for d in candidates[n]:
            todo.extend(called_names(d, text))
    primary_names = {function_name(d, text).split("::")[-1] for d, _ in targets}
    return sorted(seen - primary_names)


# --- per-function analysis ----------------------------------------------------


@dataclass
class Eff:
    reads: set = field(default_factory=set)
    writes: set = field(default_factory=set)
    mreads: list = field(default_factory=list)
    mwrites: list = field(default_factory=list)
    calls: bool = False
    control: bool = False


class Func:
    """One target function: its locals, parameters and an effect analysis."""

    def __init__(self, ctx: "Ctx", node: Node, cls: str | None):
        self.ctx = ctx
        self.node = node
        self.text = ctx.text
        self.fi = ctx.fi
        name = function_name(node, self.text)
        if cls is None and "::" in name:
            cls = name.split("::")[-2]
        self.cls = cls
        self.name = name
        self.body = node.child_by_field_name("body")
        self.params: dict[str, str] = {}
        self.locals: dict[str, str] = {}
        self.decl_count: dict[str, int] = {}
        self.escaped: set[str] = set()
        self.refs: set[str] = set()
        fd = find_function_declarator(node.child_by_field_name("declarator"))
        if fd is not None:
            params = fd.child_by_field_name("parameters")
            for p in params.named_children if params is not None else []:
                if p.type not in ("parameter_declaration", "optional_parameter_declaration"):
                    continue
                tnode = p.child_by_field_name("type")
                pname, suffix, _ = unwrap_declarator(p.child_by_field_name("declarator"), self.text)
                if pname:
                    self.params[pname] = make_type(self.T(tnode), suffix)
                    if "&" in suffix or "[]" in suffix:
                        self.escaped.add(pname)
                        self.refs.add(pname)
        self._scan()

    def T(self, n: Node | None) -> str:
        return self.text[n.start_byte:n.end_byte] if n is not None else ""

    def is_local(self, name: str) -> bool:
        return name in self.locals or name in self.params

    def var_type(self, name: str) -> str | None:
        if name in self.locals:
            return self.locals[name]
        return self.params.get(name)

    def _scan(self):
        if self.body is None:
            return
        stack = [self.body]
        while stack:
            n = stack.pop()
            t = n.type
            if t == "declaration":
                tnode = n.child_by_field_name("type")
                base = self.T(tnode)
                for d in n.children_by_field_name("declarator"):
                    name, suffix, init = unwrap_declarator(d, self.text)
                    if not name or find_function_declarator(d) is not None:
                        continue
                    self.locals[name] = make_type(base, suffix)
                    self.decl_count[name] = self.decl_count.get(name, 0) + 1
                    if "&" in suffix:
                        self.escaped.add(name)
                        self.refs.add(name)
                        if init is not None and init.type == "identifier":
                            self.escaped.add(self.T(init))
                    if "[]" in suffix:
                        self.escaped.add(name)
                    if not is_scalar(self.locals[name]) and tnode is not None and tnode.type != "primitive_type":
                        # a struct local: its members are written through `this` in method calls
                        st = self.fi.struct(canon_type(base))
                        if st is None or st.methods or st.has_ctor:
                            self.escaped.add(name)
            elif t == "pointer_expression":
                op = n.child_by_field_name("operator")
                if op is not None and self.T(op) == "&":
                    arg = n.child_by_field_name("argument")
                    root = self._root_ident(arg)
                    if root:
                        self.escaped.add(root)
            elif t == "call_expression":
                f = n.child_by_field_name("function")
                args = n.child_by_field_name("arguments")
                if f is not None and f.type == "field_expression":
                    obj = f.child_by_field_name("argument")
                    op = f.child_by_field_name("operator")
                    if op is not None and self.T(op) == "." and obj is not None:
                        root = self._root_ident(obj)
                        if root:
                            self.escaped.add(root)
                if args is not None:
                    by_ref = self._ref_params(f)
                    for i, a in enumerate(args.named_children):
                        if a.type == "identifier" and (by_ref is None or i in by_ref):
                            self.escaped.add(self.T(a))
            stack.extend(n.named_children)

    def _root_ident(self, n: Node | None) -> str | None:
        while n is not None:
            if n.type == "identifier":
                return self.T(n)
            if n.type == "parenthesized_expression":
                n = n.named_children[0] if n.named_children else None
            elif n.type == "field_expression":
                op = n.child_by_field_name("operator")
                if op is not None and self.T(op) == "->":
                    return None
                n = n.child_by_field_name("argument")
            elif n.type == "subscript_expression":
                base = n.child_by_field_name("argument")
                bt = self.type_of(base)
                if bt is not None and bt.endswith("[]"):
                    n = base
                else:
                    return None
            else:
                return None
        return None

    def _ref_params(self, f: Node | None) -> set[int] | None:
        """Indexes of reference parameters of the callee, or None when unknown."""
        if f is None:
            return None
        if f.type == "identifier":
            name = self.T(f)
            if name in self.fi.macros:
                return None
            lists = self.fi.func_params.get(name)
            if lists is None:
                st_lists = self.fi.struct(self.cls).method_params.get(name) if self.fi.struct(self.cls) else None
                if st_lists is None:
                    return set()  # a C library function: no references
                lists = st_lists
        elif f.type == "field_expression":
            fld = self.T(f.child_by_field_name("field"))
            lists = [pl for st in self.fi.structs.values() for pl in st.method_params.get(fld, [])]
            if not lists:
                return None
        else:
            return None
        out = set()
        for pl in lists:
            for i, p in enumerate(pl):
                if p.startswith("&"):
                    out.add(i)
        return out

    # --- types -----------------------------------------------------------------

    def type_of(self, n: Node | None, depth: int = 0) -> str | None:
        if n is None or depth > 20:
            return None
        t = n.type
        T = self.T
        if t == "parenthesized_expression":
            return self.type_of(n.named_children[0] if n.named_children else None, depth + 1)
        if t == "number_literal":
            s = T(n)
            if re.fullmatch(r"0[xX][0-9a-fA-F]+[uUlL]*", s) or re.fullmatch(r"[0-9]+[uUlL]*", s):
                v = int_literal(s)
                if "u" in s.lower() or (v is not None and v > 0x7fffffff):
                    return "unsigned int"
                return "int"
            return "float" if s.lower().endswith("f") else "double"
        if t == "char_literal":
            return "char"
        if t in ("string_literal", "concatenated_string"):
            return "char*"
        if t in ("true", "false"):
            return "bool"
        if t == "this":
            return (self.cls + "*") if self.cls else None
        if t == "identifier":
            name = T(n)
            if self.is_local(name):
                return self.var_type(name)
            ft = self.fi.field_type(self.cls, name) if self.cls else None
            if ft is not None:
                return ft
            return self.fi.globals.get(name)
        if t == "field_expression":
            base = n.child_by_field_name("argument")
            op = T(n.child_by_field_name("operator"))
            bt = self.type_of(base, depth + 1)
            if bt is None:
                return None
            if op == "->":
                if not bt.endswith("*"):
                    return None
                bt = bt[:-1]
            return self.fi.field_type(bt, T(n.child_by_field_name("field")))
        if t == "subscript_expression":
            bt = self.type_of(n.child_by_field_name("argument"), depth + 1)
            if bt is None:
                return None
            if bt.endswith("[]"):
                return bt[:-2]
            if bt.endswith("*"):
                return bt[:-1]
            return None
        if t == "pointer_expression":
            op = T(n.child_by_field_name("operator"))
            at = self.type_of(n.child_by_field_name("argument"), depth + 1)
            if at is None:
                return None
            if op == "&":
                return at + "*" if not at.endswith("[]") else at[:-2] + "*"
            if at.endswith("*"):
                return at[:-1]
            if at.endswith("[]"):
                return at[:-2]
            return None
        if t == "cast_expression":
            tn = n.child_by_field_name("type")
            if tn is None:
                return None
            ty = T(tn)
            return canon_type(ty)
        if t == "call_expression":
            f = n.child_by_field_name("function")
            if f is None:
                return None
            if f.type == "identifier":
                return self.fi.funcs.get(T(f)) or (self.fi.method_type(self.cls, T(f)) if self.cls else None)
            if f.type == "field_expression":
                bt = self.type_of(f.child_by_field_name("argument"), depth + 1)
                op = T(f.child_by_field_name("operator"))
                if bt is None:
                    return None
                if op == "->":
                    bt = bt[:-1] if bt.endswith("*") else None
                return self.fi.method_type(bt, T(f.child_by_field_name("field"))) if bt else None
            return None
        if t == "binary_expression":
            op = T(n.child_by_field_name("operator"))
            if op in ("<", ">", "<=", ">=", "==", "!=", "&&", "||"):
                return "bool"
            lt = self.type_of(n.child_by_field_name("left"), depth + 1)
            rt = self.type_of(n.child_by_field_name("right"), depth + 1)
            if op in ("<<", ">>"):
                return promote(lt) if lt else None
            if lt is None or rt is None:
                return None
            if is_ptr(lt) and is_int(rt) and op in ("+", "-"):
                return lt if lt.endswith("*") else lt[:-2] + "*"
            if is_ptr(rt) and is_int(lt) and op == "+":
                return rt if rt.endswith("*") else rt[:-2] + "*"
            if is_ptr(lt) and is_ptr(rt) and op == "-":
                return "int"
            return arith(lt, rt)
        if t == "unary_expression":
            op = T(n.child_by_field_name("operator"))
            if op == "!":
                return "bool"
            at = self.type_of(n.child_by_field_name("argument"), depth + 1)
            return promote(at) if at else None
        if t == "update_expression":
            return self.type_of(n.child_by_field_name("argument"), depth + 1)
        if t == "assignment_expression":
            return self.type_of(n.child_by_field_name("left"), depth + 1)
        if t == "conditional_expression":
            a = self.type_of(n.child_by_field_name("consequence"), depth + 1)
            b = self.type_of(n.child_by_field_name("alternative"), depth + 1)
            if a is not None and a == b:
                return a
            if a and b and is_scalar(a) and is_scalar(b) and not is_ptr(a) and not is_ptr(b):
                return arith(a, b)
            return None
        if t == "sizeof_expression":
            return "unsigned int"
        return None

    # --- effects -------------------------------------------------------------

    def effects(self, n: Node, skip: Node | None = None) -> Eff:
        e = Eff()
        self._visit(n, e, "r", 0, 0, skip)
        return e

    def _mem(self, e: Eff, path, mode: str):
        if "r" in mode:
            e.mreads.append(path)
        if "w" in mode:
            e.mwrites.append(path)

    def _visit(self, n: Node, e: Eff, mode: str, loops: int, switches: int, skip: Node | None):
        if skip is not None and n.id == skip.id:
            return
        t = n.type
        T = self.T
        if t in ("comment", "number_literal", "string_literal", "char_literal", "true", "false",
                 "null", "nullptr", "concatenated_string", "this", "type_descriptor", "primitive_type",
                 "type_identifier", "sized_type_specifier", "sizeof_expression", "escape_sequence",
                 "string_content", "raw_string_literal", "alignof_expression"):
            return
        if t == "identifier":
            self._ident(n, e, mode)
            return
        if t in ("field_expression", "subscript_expression"):
            self._access(n, e, mode, loops, switches, skip)
            return
        if t == "pointer_expression":
            op = T(n.child_by_field_name("operator"))
            arg = n.child_by_field_name("argument")
            if op == "&":
                self._address(arg, e, loops, switches, skip)
            else:
                self._access(n, e, mode, loops, switches, skip)
            return
        if t == "parenthesized_expression":
            for c in n.named_children:
                self._visit(c, e, mode, loops, switches, skip)
            return
        if t == "assignment_expression":
            op = T(n.child_by_field_name("operator"))
            self._visit(n.child_by_field_name("left"), e, "w" if op == "=" else "rw", loops, switches, skip)
            self._visit(n.child_by_field_name("right"), e, "r", loops, switches, skip)
            return
        if t == "update_expression":
            self._visit(n.child_by_field_name("argument"), e, "rw", loops, switches, skip)
            return
        if t == "call_expression":
            f = n.child_by_field_name("function")
            args = n.child_by_field_name("arguments")
            fname = T(f) if f is not None and f.type == "identifier" else None
            if fname in PURE_CALLS and not self.is_local(fname):
                pass
            elif fname in READ_CALLS and not self.is_local(fname):
                e.mreads.append(UNKNOWN)
            else:
                e.calls = True
            if f is not None:
                if f.type == "field_expression":
                    obj = f.child_by_field_name("argument")
                    op = T(f.child_by_field_name("operator"))
                    if op == "->":
                        self._visit(obj, e, "r", loops, switches, skip)
                    else:
                        self._address(obj, e, loops, switches, skip)
                elif f.type != "identifier" and f.type not in ("qualified_identifier", "template_function"):
                    self._visit(f, e, "r", loops, switches, skip)
                elif f.type == "identifier" and self.is_local(fname or ""):
                    self._visit(f, e, "r", loops, switches, skip)
            if args is not None:
                for a in args.named_children:
                    self._visit(a, e, "r", loops, switches, skip)
            return
        if t in ("new_expression", "delete_expression", "lambda_expression", "throw_statement",
                 "co_await_expression"):
            e.calls = True
            if t == "throw_statement":
                e.control = True
            for c in n.named_children:
                self._visit(c, e, "r", loops, switches, skip)
            return
        if t == "return_statement" or t == "goto_statement" or t == "labeled_statement" \
                or t == "case_statement" or t == "try_statement":
            e.control = True
            for c in n.named_children:
                if c.type != "statement_identifier":
                    self._visit(c, e, "r", loops, switches, skip)
            return
        if t == "break_statement":
            if loops == 0 and switches == 0:
                e.control = True
            return
        if t == "continue_statement":
            if loops == 0:
                e.control = True
            return
        if t in LOOPS:
            for c in n.named_children:
                self._visit(c, e, "r", loops + 1, switches, skip)
            return
        if t == "switch_statement":
            for c in n.named_children:
                self._visit(c, e, "r", loops, switches + 1, skip)
            return
        if t == "declaration":
            tnode = n.child_by_field_name("type")
            base = T(tnode)
            st = self.fi.struct(canon_type(base))
            for d in n.children_by_field_name("declarator"):
                name, suffix, init = unwrap_declarator(d, self.text)
                if name:
                    e.writes.add(name)
                    e.reads.add(name)  # a declaration is ordered against every use of its name
                ty = make_type(base, suffix)
                if not is_scalar(ty) and tnode is not None and tnode.type != "primitive_type":
                    if "&" in suffix or self.fi.nontrivial(canon_type(base)) or (
                            st is not None and st.has_ctor and d.type == "init_declarator"):
                        e.calls = True
                if init is not None:
                    self._visit(init, e, "r", loops, switches, skip)
                if d.type == "init_declarator" and init is None:
                    for c in d.named_children:
                        if c.type in ("argument_list", "initializer_list"):
                            e.calls = True
                            self._visit(c, e, "r", loops, switches, skip)
            return
        if t in ("gnu_asm_expression", "ms_asm"):
            e.control = True
            return
        if t == "cast_expression":
            v = n.child_by_field_name("value")
            if v is not None:
                self._visit(v, e, mode, loops, switches, skip)
            return
        if t == "comma_expression":
            kids = n.named_children
            for i, c in enumerate(kids):
                self._visit(c, e, mode if i == len(kids) - 1 else "r", loops, switches, skip)
            return
        if t in ("qualified_identifier", "template_function"):
            e.mreads.append(UNKNOWN)
            if "w" in mode:
                e.mwrites.append(UNKNOWN)
            return
        for c in n.named_children:
            self._visit(c, e, "r", loops, switches, skip)

    def _ident(self, n: Node, e: Eff, mode: str):
        name = self.T(n)
        if self.is_local(name):
            if "r" in mode:
                e.reads.add(name)
            if "w" in mode:
                e.writes.add(name)
            if name in self.escaped:
                self._mem(e, ("esc:" + name, ()), mode)
            if name in self.refs:
                self._mem(e, UNKNOWN, mode)
            return
        if self.cls and self.fi.has_field(self.cls, name):
            self._mem(e, ("this", (self._fkey(name),)), mode)
            return
        if name in self.fi.globals:
            self._mem(e, ("g:" + name, ()), mode)
            return
        if name in self.fi.consts or name in self.fi.funcs or name in self.fi.types:
            return
        self._mem(e, UNKNOWN, mode)

    def _fkey(self, name: str) -> str:
        return "*" if name in self.fi.union_fields else name

    def _address(self, n: Node | None, e: Eff, loops: int, switches: int, skip):
        """Visit the parts of an lvalue that are read to compute its address."""
        if n is None:
            return
        t = n.type
        if t == "identifier":
            return
        if t == "parenthesized_expression":
            for c in n.named_children:
                self._address(c, e, loops, switches, skip)
            return
        if t == "field_expression":
            op = self.T(n.child_by_field_name("operator"))
            base = n.child_by_field_name("argument")
            if op == "->":
                self._visit(base, e, "r", loops, switches, skip)
            else:
                self._address(base, e, loops, switches, skip)
            return
        if t == "subscript_expression":
            base = n.child_by_field_name("argument")
            idx = n.child_by_field_name("index")
            if idx is not None:
                self._visit(idx, e, "r", loops, switches, skip)
            bt = self.type_of(base)
            if bt is not None and bt.endswith("[]"):
                self._address(base, e, loops, switches, skip)
            else:
                self._visit(base, e, "r", loops, switches, skip)
            return
        if t == "pointer_expression" and self.T(n.child_by_field_name("operator")) == "*":
            self._visit(n.child_by_field_name("argument"), e, "r", loops, switches, skip)
            return
        self._visit(n, e, "r", loops, switches, skip)

    def _access(self, n: Node, e: Eff, mode: str, loops: int, switches: int, skip):
        self._address(n, e, loops, switches, skip)
        p = self.path(n)
        if p is None:
            self._mem(e, UNKNOWN, mode)
            return
        root, _ = p
        if root.startswith("L:"):
            name = root[2:]
            if "r" in mode:
                e.reads.add(name)
            if "w" in mode:
                e.writes.add(name)
                e.reads.add(name)  # a partial write keeps the rest of the local
            return
        self._mem(e, p, mode)

    def path(self, n: Node | None):
        """(root, field path) naming the storage an lvalue refers to, or None."""
        if n is None:
            return None
        t = n.type
        T = self.T
        if t == "parenthesized_expression":
            return self.path(n.named_children[0]) if n.named_children else None
        if t == "identifier":
            name = T(n)
            if self.is_local(name):
                if name in self.refs:
                    return None
                return ("esc:" + name, ()) if name in self.escaped else ("L:" + name, ())
            if self.cls and self.fi.has_field(self.cls, name):
                return ("this", (self._fkey(name),))
            if name in self.fi.globals:
                return ("g:" + name, ())
            return None
        if t == "field_expression":
            base = n.child_by_field_name("argument")
            op = T(n.child_by_field_name("operator"))
            key = self._fkey(T(n.child_by_field_name("field")))
            if op == "->":
                root = self.deref_root(base)
                return (root, (key,)) if root else None
            bp = self.path(base)
            return (bp[0], bp[1] + (key,)) if bp else None
        if t == "subscript_expression":
            base = n.child_by_field_name("argument")
            idx = n.child_by_field_name("index")
            v = int_literal(T(idx)) if idx is not None and idx.type == "number_literal" else None
            key = ("[%d]" % v) if v is not None else "*"
            bt = self.type_of(base)
            if bt is not None and bt.endswith("[]"):
                bp = self.path(base)
                return (bp[0], bp[1] + (key,)) if bp else None
            root = self.deref_root(base)
            return (root, (key,)) if root else None
        if t == "pointer_expression" and T(n.child_by_field_name("operator")) == "*":
            root = self.deref_root(n.child_by_field_name("argument"))
            return (root, ()) if root else None
        return None

    def deref_root(self, p: Node | None) -> str | None:
        if p is None:
            return None
        while p.type == "parenthesized_expression" and p.named_children:
            p = p.named_children[0]
        if p.type == "this":
            return "this"
        if not self._simple_chain(p):
            return None
        ty = self.type_of(p)
        txt = re.sub(r"\s+", "", self.T(p))
        return "P:" + txt + "|" + (ty or "")

    def _simple_chain(self, p: Node) -> bool:
        t = p.type
        if t in ("identifier", "this"):
            return True
        if t == "field_expression":
            return self._simple_chain(p.child_by_field_name("argument"))
        if t == "subscript_expression":
            idx = p.child_by_field_name("index")
            return idx is not None and idx.type in ("number_literal", "identifier") and \
                self._simple_chain(p.child_by_field_name("argument"))
        if t == "parenthesized_expression" and p.named_children:
            return self._simple_chain(p.named_children[0])
        return False

    def pointee(self, root: str) -> str | None:
        if root == "this":
            return self.cls
        if root.startswith("P:"):
            ty = root.split("|", 1)[1]
            if ty.endswith("*"):
                return ty[:-1]
            if ty.endswith("[]"):
                return ty[:-2]
        return None


def paths_overlap(a: tuple, b: tuple) -> bool:
    for x, y in zip(a, b):
        if x != y and x != "*" and y != "*":
            return False
    return True


def may_alias(f: Func, a, b) -> bool:
    if a == UNKNOWN or b == UNKNOWN:
        return True
    ra, pa = a
    rb, pb = b
    if ra == rb:
        return paths_overlap(pa, pb)
    ka, kb = ra.split(":", 1)[0], rb.split(":", 1)[0]
    named = ("g", "esc")
    if ka in named and kb in named:
        return False  # two different named objects
    if ka in ("P", "this") and kb in ("P", "this"):
        ta, tb = f.pointee(ra), f.pointee(rb)
        if ta and tb and ta == tb and pa and pb and pa[0] != pb[0] and "*" not in (pa[0], pb[0]) \
                and not pa[0].startswith("[") and not pb[0].startswith("["):
            return False  # different members of the same struct type
        return True
    return True


def independent(f: Func, a: Eff, b: Eff) -> bool:
    if a.control or b.control:
        return False
    if a.calls and b.calls:
        return False
    if a.calls and (b.mreads or b.mwrites):
        return False
    if b.calls and (a.mreads or a.mwrites):
        return False
    if a.writes & (b.reads | b.writes) or b.writes & a.reads:
        return False
    for w in a.mwrites:
        for x in b.mreads + b.mwrites:
            if may_alias(f, w, x):
                return False
    for w in b.mwrites:
        for x in a.mreads:
            if may_alias(f, w, x):
                return False
    return True


def pure(e: Eff) -> bool:
    return not (e.calls or e.control or e.writes or e.mwrites)


# --- mutation context ---------------------------------------------------------------


class Ctx:
    def __init__(self, text: str, spec: TargetSpec, fi: FileInfo | None, rng: random.Random):
        self.text = text
        self.spec = spec
        self.rng = rng
        self.tree = parse(text)
        self.root = self.tree.root_node
        self.fi = fi if fi is not None else build_file_info(self.root, text)
        targets, self.error = find_targets(self.root, text, spec)
        self.funcs = [Func(self, n, c) for n, c in targets]
        self._nodes = None
        self.simplify = False   # prefer the undoing direction of two-way mutations
        self.hot: set[int] = set()  # 0-based rows to aim at (lines behind differing code)

    def set_hot(self, lines) -> None:
        """Aim at these 1-based source lines (and their neighbours)."""
        self.hot = {r for line in lines for r in (line - 2, line - 1, line)}

    def _is_hot(self, item) -> bool:
        n = item if isinstance(item, Node) else next((x for x in item if isinstance(x, Node)), None) \
            if isinstance(item, tuple) else None
        if n is None:
            return False
        return any(r in self.hot for r in range(n.start_point[0], n.end_point[0] + 1))

    def pick(self, items: list):
        """A random item, usually one on a hot line when there are hot lines."""
        if self.hot and self.rng.random() < FOCUS:
            hot = [it for it in items if self._is_hot(it)]
            if hot:
                return self.rng.choice(hot)
        return self.rng.choice(items)

    def pick_index(self, nodes: list) -> int:
        if self.hot and self.rng.random() < FOCUS:
            hot = [i for i, n in enumerate(nodes) if self._is_hot(n)]
            if hot:
                return self.rng.choice(hot)
        return self.rng.randrange(len(nodes))

    def T(self, n: Node | None) -> str:
        return self.text[n.start_byte:n.end_byte] if n is not None else ""

    def nodes(self) -> list[tuple[Func, Node]]:
        """Every node inside the target function bodies, with its function."""
        if self._nodes is None:
            out = []
            for f in self.funcs:
                if f.body is None:
                    continue
                stack = [f.body]
                while stack:
                    n = stack.pop()
                    out.append((f, n))
                    stack.extend(reversed(n.named_children))
            self._nodes = out
        return self._nodes

    def of_type(self, *types: str) -> list[tuple[Func, Node]]:
        return [(f, n) for f, n in self.nodes() if n.type in types]

    def blocks(self) -> list[tuple[Func, Node, list[Node]]]:
        """Statement lists: compound statements and the bodies of case labels."""
        out = []
        for f, n in self.of_type("compound_statement", "case_statement"):
            if n.type == "compound_statement":
                stmts = [c for c in n.named_children if c.type in STATEMENT_TYPES]
            else:
                kids = n.named_children
                start = 1 if n.child_by_field_name("value") is not None else 0
                stmts = [c for c in kids[start:] if c.type in STATEMENT_TYPES]
            if stmts:
                out.append((f, n, stmts))
        return out

    def indent_of(self, n: Node) -> str:
        line_start = self.text.rfind("\n", 0, n.start_byte) + 1
        m = re.match(r"[ \t]*", self.text[line_start:n.start_byte])
        return m.group(0) if m else ""

    def fresh_name(self, stem: str = "tmp") -> str:
        i = 0
        while True:
            name = f"{stem}{i}"
            if not re.search(r"\b%s\b" % name, self.text):
                return name
            i += 1


def apply_edits(text: str, edits: list[tuple[int, int, str]]) -> str:
    # from the end; an insertion at the start of a replaced range goes in front of it
    edits = sorted(edits, key=lambda e: (e[0], e[1]), reverse=True)
    last = len(text) + 1
    for s, e, r in edits:
        if e > last:
            raise ValueError("overlapping edits")
        text = text[:s] + r + text[e:]
        last = s
    return text


def in_block(n: Node) -> bool:
    p = n.parent
    return p is not None and p.type in ("compound_statement", "case_statement")


def wrap_stmts(ctx: Ctx, stmt: Node, texts: list[str]) -> str:
    """Text replacing `stmt` by several statements, braced if it is not in a block."""
    ind = ctx.indent_of(stmt)
    if in_block(stmt):
        return ("\n" + ind).join(texts)
    return "{ " + " ".join(texts) + " }"


def is_primary(n: Node) -> bool:
    return n.type in ("identifier", "number_literal", "field_expression", "subscript_expression",
                      "call_expression", "parenthesized_expression", "string_literal", "char_literal",
                      "this", "true", "false")


def paren(ctx: Ctx, n: Node) -> str:
    s = ctx.T(n)
    return s if is_primary(n) else "(" + s + ")"


def cond_value(n: Node) -> Node | None:
    """The expression inside an if/while/switch condition_clause."""
    if n is None:
        return None
    if n.type == "condition_clause":
        v = n.child_by_field_name("value")
        if v is None or v.type == "declaration" or n.child_by_field_name("initializer") is not None:
            return None
        return v
    if n.type == "parenthesized_expression":
        return n.named_children[0] if n.named_children else None
    return n


def has_type(n: Node, types: set[str], stop: set[str] | None = None) -> bool:
    stack = [n]
    while stack:
        x = stack.pop()
        if x.type in types:
            return True
        for c in x.named_children:
            if stop and c.type in stop:
                continue
            stack.append(c)
    return False


def own_continues(body: Node) -> bool:
    """Does `body` contain a continue that belongs to the enclosing loop?"""
    stack = [body]
    while stack:
        x = stack.pop()
        if x.type == "continue_statement":
            return True
        for c in x.named_children:
            if c.type in LOOPS:
                continue
            stack.append(c)
    return False


def own_breaks(body: Node) -> bool:
    stack = [body]
    while stack:
        x = stack.pop()
        if x.type == "break_statement":
            return True
        for c in x.named_children:
            if c.type in LOOPS or c.type == "switch_statement":
                continue
            stack.append(c)
    return False


def has_error(n: Node) -> bool:
    return n.has_error


def negate(ctx: Ctx, f: Func, c: Node) -> str:
    """Text of the logical negation of condition expression c."""
    while c.type == "parenthesized_expression" and len(c.named_children) == 1:
        c = c.named_children[0]
    if c.type == "unary_expression" and ctx.T(c.child_by_field_name("operator")) == "!":
        a = c.child_by_field_name("argument")
        inner = a
        while inner.type == "parenthesized_expression" and len(inner.named_children) == 1:
            inner = inner.named_children[0]
        return ctx.T(inner) if inner.type != "comma_expression" else ctx.T(a)
    if c.type == "binary_expression":
        op = ctx.T(c.child_by_field_name("operator"))
        left, right = c.child_by_field_name("left"), c.child_by_field_name("right")
        inv = {"==": "!=", "!=": "=="}
        if op in inv:
            return f"{ctx.T(left)} {inv[op]} {ctx.T(right)}"
        rel = {"<": ">=", ">=": "<", ">": "<=", "<=": ">"}
        if op in rel:
            lt, rt = f.type_of(left), f.type_of(right)
            if (is_int(lt) or is_ptr(lt)) and (is_int(rt) or is_ptr(rt)):
                return f"{ctx.T(left)} {rel[op]} {ctx.T(right)}"
    return "!" + paren(ctx, c)


# --- the mutations -------------------------------------------------------------------
#
# Each takes a Ctx and returns a list of edits, or None if it found no site.


def m_move_stmt(ctx: Ctx):
    blocks = [(f, b, s) for f, b, s in ctx.blocks() if len(s) >= 2]
    if not blocks:
        return None
    f, b, stmts = ctx.pick(blocks)
    i = ctx.pick_index(stmts)
    d = ctx.rng.choice([-1, 1])
    k = 1
    while ctx.rng.random() < 0.35:
        k += 1
    j = i + d * k
    if not 0 <= j < len(stmts):
        return None
    me = f.effects(stmts[i])
    if me.control:
        return None
    lo, hi = min(i, j), max(i, j)
    for x in range(lo, hi + 1):
        if x == i:
            continue
        if not independent(f, me, f.effects(stmts[x])):
            return None
    order = list(range(lo, hi + 1))
    order.remove(i)
    if d > 0:
        order.append(i)
    else:
        order.insert(0, i)
    return [region_edit(ctx, stmts, lo, hi, order)]


def region_edit(ctx: Ctx, stmts: list[Node], lo: int, hi: int, order: list[int]):
    """Replace statements lo..hi by the same statements in `order`, keeping the
    text between them (indentation, comments) in place."""
    seps = [ctx.text[stmts[x].end_byte:stmts[x + 1].start_byte] for x in range(lo, hi)]
    out = ctx.T(stmts[order[0]])
    for sep, x in zip(seps, order[1:]):
        out += sep + ctx.T(stmts[x])
    return (stmts[lo].start_byte, stmts[hi].end_byte, out)


def _declarator_name(ctx: Ctx, n: Node) -> str | None:
    if n.type in ("identifier", "field_identifier"):
        return ctx.T(n)
    child = n.child_by_field_name("declarator")
    if child is not None:
        return _declarator_name(ctx, child)
    return next((ctx.T(c) for c in n.named_children if c.type == "identifier"), None)


def declared_names(ctx: Ctx, node: Node) -> set[str]:
    return {n for d in node.children_by_field_name("declarator") if (n := _declarator_name(ctx, d))}


def m_move_decl(ctx: Ctx):
    """Move a declaration up or down past statements it does not depend on
    (the compiler hands out stack slots in declaration order)."""
    blocks = [(f, b, s) for f, b, s in ctx.blocks() if len(s) >= 2 and any(x.type == "declaration" for x in s)]
    wanted = ctx.spec.stack_names
    named = [blk for blk in blocks if any(x.type == "declaration" and declared_names(ctx, x) & wanted
                                          for x in blk[2])] if wanted else []
    blocks = named or blocks
    if not blocks:
        return None
    f, b, stmts = ctx.pick(blocks)
    decls = [i for i, s in enumerate(stmts) if s.type == "declaration"]
    idx = [i for i in decls if declared_names(ctx, stmts[i]) & wanted] if named else decls
    i = idx[ctx.pick_index([stmts[k] for k in idx])]
    # prefer moving among the other declarations
    targets = [j for j in decls if j != i] or [j for j in range(len(stmts)) if j != i]
    if not targets:
        return None
    j = ctx.rng.choice(targets)
    me = f.effects(stmts[i])
    lo, hi = min(i, j), max(i, j)
    for x in range(lo, hi + 1):
        if x != i and not independent(f, me, f.effects(stmts[x])):
            return None
    order = list(range(lo, hi + 1))
    order.remove(i)
    if j > i:
        order.append(i)
    else:
        order.insert(0, i)
    return [region_edit(ctx, stmts, lo, hi, order)]


def decl_parts(ctx: Ctx, d: Node):
    """(prefix text, [declarator nodes], suffix) of a declaration."""
    decls = d.children_by_field_name("declarator")
    if not decls:
        return None
    prefix = ctx.text[d.start_byte:decls[0].start_byte]
    return prefix, decls


def m_split_multi_decl(ctx: Ctx):
    """`int a, b;` -> `int a; int b;`, or reorder the declarators."""
    sites = []
    for f, n in ctx.of_type("declaration"):
        if not in_block(n):
            continue
        parts = decl_parts(ctx, n)
        if parts and len(parts[1]) >= 2:
            sites.append((f, n, parts))
    if not sites:
        return None
    f, n, (prefix, decls) = ctx.pick(sites)
    ind = ctx.indent_of(n)
    if ctx.rng.random() < 0.5:
        texts = [prefix + ctx.T(d) + ";" for d in decls]
        return [(n.start_byte, n.end_byte, ("\n" + ind).join(texts))]
    # reorder: only declarators without initialisers that use other names
    i = ctx.rng.randrange(len(decls) - 1)
    a, b = decls[i], decls[i + 1]
    if a.type == "init_declarator" or b.type == "init_declarator":
        return None
    return [(a.start_byte, a.end_byte, ctx.T(b)), (b.start_byte, b.end_byte, ctx.T(a))]


def m_merge_decls(ctx: Ctx):
    """`int a; int b;` -> `int a, b;` for adjacent declarations of one type."""
    sites = []
    for f, b, stmts in ctx.blocks():
        for i in range(len(stmts) - 1):
            x, y = stmts[i], stmts[i + 1]
            if x.type == "declaration" and y.type == "declaration":
                px, py = decl_parts(ctx, x), decl_parts(ctx, y)
                if px and py and px[0].strip() == py[0].strip():
                    between = ctx.text[x.end_byte:y.start_byte]
                    if between.strip() == "":
                        sites.append((x, y, px, py))
    if not sites:
        return None
    x, y, px, py = ctx.pick(sites)
    text = px[0] + ", ".join(ctx.T(d) for d in px[1] + py[1]) + ";"
    return [(x.start_byte, y.end_byte, text)]


SCALAR_BASES = ("primitive_type", "sized_type_specifier")


def scalar_decl(ctx: Ctx, f: Func, n: Node) -> bool:
    """A declaration whose split into declaration plus assignment is exact."""
    if n.type != "declaration":
        return False
    if re.search(r"\b(static|const|extern)\b", ctx.text[n.start_byte:(n.child_by_field_name("type") or n).end_byte]):
        return False
    for c in n.children:
        if c.type in ("storage_class_specifier", "type_qualifier"):
            return False
    tnode = n.child_by_field_name("type")
    for d in n.children_by_field_name("declarator"):
        name, suffix, init = unwrap_declarator(d, ctx.text)
        if name is None or "&" in suffix or "[]" in suffix or "(" in suffix:
            return False
        ty = make_type(ctx.T(tnode), suffix)
        if not is_scalar(ty) and not (tnode is not None and tnode.type in SCALAR_BASES):
            if "*" not in suffix:
                return False
    return True


def m_split_init(ctx: Ctx):
    """`T x = e;` -> `T x; x = e;`"""
    sites = []
    for f, n in ctx.of_type("declaration"):
        if not in_block(n) or not scalar_decl(ctx, f, n):
            continue
        decls = n.children_by_field_name("declarator")
        if len(decls) != 1 or decls[0].type != "init_declarator":
            continue
        init = decls[0].child_by_field_name("value")
        if init is None or init.type == "initializer_list":
            continue
        sites.append((f, n, decls[0], init))
    if not sites:
        return None
    f, n, d, init = ctx.pick(sites)
    name, _, _ = unwrap_declarator(d, ctx.text)
    inner = d.child_by_field_name("declarator")
    prefix = ctx.text[n.start_byte:d.start_byte]
    ind = ctx.indent_of(n)
    return [(n.start_byte, n.end_byte, f"{prefix}{ctx.T(inner)};\n{ind}{name} = {ctx.T(init)};")]


def m_merge_init(ctx: Ctx):
    """`T x; x = e;` -> `T x = e;` (adjacent statements, e not using x)."""
    sites = []
    for f, b, stmts in ctx.blocks():
        for i in range(len(stmts) - 1):
            x, y = stmts[i], stmts[i + 1]
            if not scalar_decl(ctx, f, x):
                continue
            decls = x.children_by_field_name("declarator")
            if len(decls) != 1 or decls[0].type == "init_declarator":
                continue
            name, _, _ = unwrap_declarator(decls[0], ctx.text)
            if y.type != "expression_statement" or not y.named_children:
                continue
            a = y.named_children[0]
            if a.type != "assignment_expression" or ctx.T(a.child_by_field_name("operator")) != "=":
                continue
            left = a.child_by_field_name("left")
            if left.type != "identifier" or ctx.T(left) != name:
                continue
            right = a.child_by_field_name("right")
            if re.search(r"\b%s\b" % re.escape(name), ctx.T(right)):
                continue
            sites.append((x, y, decls[0], right))
    if not sites:
        return None
    x, y, d, right = ctx.pick(sites)
    prefix = ctx.text[x.start_byte:d.start_byte]
    return [(x.start_byte, y.end_byte, f"{prefix}{ctx.T(d)} = {ctx.T(right)};")]


def uses_of(ctx: Ctx, f: Func, name: str) -> list[Node]:
    return [n for g, n in ctx.nodes() if g is f and n.type == "identifier" and ctx.T(n) == name]


def m_decl_scope(ctx: Ctx):
    """Move an uninitialised declaration into the innermost block that holds
    every use, or out of an inner block to the top of the function."""
    sites = []
    for f, n in ctx.of_type("declaration"):
        if not in_block(n) or not scalar_decl(ctx, f, n):
            continue
        decls = n.children_by_field_name("declarator")
        if len(decls) != 1:
            continue
        name, _, init = unwrap_declarator(decls[0], ctx.text)
        if not name or f.decl_count.get(name, 0) != 1 or name in f.params:
            continue
        sites.append((f, n, decls[0], name, init))
    if not sites:
        return None
    f, n, d, name, init = ctx.pick(sites)
    block = n.parent
    uses = [u for u in uses_of(ctx, f, name) if not (n.start_byte <= u.start_byte < n.end_byte)]
    if ctx.rng.random() < 0.5:
        # outward: to the top of the function body
        if block.id == f.body.id or block.type != "compound_statement":
            return None
        top = f.body
        # every use must be inside the old block (else the name meant something else)
        if any(not (block.start_byte <= u.start_byte < block.end_byte) for u in uses):
            return None
        if re.search(r"\b%s\b" % re.escape(name), ctx.text[top.start_byte:block.start_byte] +
                     ctx.text[block.end_byte:top.end_byte]):
            return None
        first = [c for c in top.named_children if c.type in STATEMENT_TYPES]
        anchor = first[0]
        ind = ctx.indent_of(anchor)
        prefix = ctx.text[n.start_byte:d.start_byte]
        inner = d.child_by_field_name("declarator") if d.type == "init_declarator" else d
        edits = [(anchor.start_byte, anchor.start_byte, f"{prefix}{ctx.T(inner)};\n{ind}")]
        if init is not None:
            if init.type == "initializer_list":
                return None
            edits.append((n.start_byte, n.end_byte, f"{name} = {ctx.T(init)};"))
        else:
            edits.append(delete_stmt(ctx, n))
        return edits
    # inward: only uninitialised declarations, not into loops
    if init is not None or not uses:
        return None
    inner_block = None
    cand = uses[0].parent
    while cand is not None and cand.id != block.id:
        if cand.type == "compound_statement" and all(cand.start_byte <= u.start_byte < cand.end_byte for u in uses):
            # no loop between the old block and the new one
            x, ok = cand.parent, True
            while x is not None and x.id != block.id:
                if x.type in LOOPS:
                    ok = False
                    break
                x = x.parent
            if ok:
                inner_block = cand
            break
        cand = cand.parent
    if inner_block is None:
        return None
    first = [c for c in inner_block.named_children if c.type in STATEMENT_TYPES]
    if not first:
        return None
    anchor = first[0]
    ind = ctx.indent_of(anchor)
    return [delete_stmt(ctx, n), (anchor.start_byte, anchor.start_byte, ctx.T(n) + "\n" + ind)]


def delete_stmt(ctx: Ctx, n: Node):
    """Edit removing a statement and, if it sat alone on its line, the line."""
    line_start = ctx.text.rfind("\n", 0, n.start_byte) + 1
    line_end = ctx.text.find("\n", n.end_byte)
    if line_end < 0:
        line_end = len(ctx.text)
    if ctx.text[line_start:n.start_byte].strip() == "" and ctx.text[n.end_byte:line_end].strip() == "":
        return (line_start, line_end + 1, "")
    return (n.start_byte, n.end_byte, "")


COMMUTATIVE = {"+", "*", "&", "|", "^", "==", "!="}


def m_swap_commutative(ctx: Ctx):
    sites = []
    for f, n in ctx.of_type("binary_expression"):
        op = ctx.T(n.child_by_field_name("operator"))
        if op not in COMMUTATIVE:
            continue
        a, b = n.child_by_field_name("left"), n.child_by_field_name("right")
        if a is None or b is None:
            continue
        sites.append((f, n, a, b, op))
    if not sites:
        return None
    f, n, a, b, op = ctx.pick(sites)
    if not swappable(f, a, b):
        return None
    ta, tb = f.type_of(a), f.type_of(b)
    if (ta and "string" in ta) or (tb and "string" in tb):
        return None
    if op == "+" and (a.type == "string_literal" or b.type == "string_literal"):
        return None
    return [(a.start_byte, a.end_byte, paren_for(ctx, b, n)), (b.start_byte, b.end_byte, paren_for(ctx, a, n))]


PREC = {"||": 1, "&&": 2, "|": 3, "^": 4, "&": 5, "==": 6, "!=": 6, "<": 7, ">": 7, "<=": 7, ">=": 7,
        "<<": 8, ">>": 8, "+": 9, "-": 9, "*": 10, "/": 10, "%": 10}


def paren_for(ctx: Ctx, operand: Node, parent: Node) -> str:
    """Text of operand, parenthesised if needed to sit on either side of parent."""
    s = ctx.T(operand)
    if operand.type in ("binary_expression",):
        op = ctx.T(operand.child_by_field_name("operator"))
        pop = ctx.T(parent.child_by_field_name("operator"))
        if PREC.get(op, 0) <= PREC.get(pop, 0):
            return "(" + s + ")"
        return s
    if operand.type in ("conditional_expression", "assignment_expression", "comma_expression"):
        return "(" + s + ")"
    return s


def swappable(f: Func, a: Node, b: Node) -> bool:
    """Can a and b be evaluated in the other order?"""
    ea, eb = f.effects(a), f.effects(b)
    if ea.control or eb.control:
        return False
    if ea.writes or eb.writes or ea.mwrites or eb.mwrites:
        return False
    return independent(f, ea, eb)


def m_flip_compare(ctx: Ctx):
    flip = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}
    sites = []
    for f, n in ctx.of_type("binary_expression"):
        opn = n.child_by_field_name("operator")
        op = ctx.T(opn)
        if op in flip:
            sites.append((f, n, opn, op))
    if not sites:
        return None
    f, n, opn, op = ctx.pick(sites)
    a, b = n.child_by_field_name("left"), n.child_by_field_name("right")
    if not swappable(f, a, b):
        return None
    # `a < b > c` style template confusion: only plain operands
    return [(a.start_byte, a.end_byte, paren_for(ctx, b, n)), (opn.start_byte, opn.end_byte, flip[op]),
            (b.start_byte, b.end_byte, paren_for(ctx, a, n))]


def body_text(ctx: Ctx, s: Node) -> str:
    """A statement as a braced block."""
    if s.type == "compound_statement":
        return ctx.T(s)
    return "{ " + ctx.T(s) + " }"


def m_negate_if(ctx: Ctx):
    """`if (c) A else B` -> `if (!c) B else A` (and `if (c) A` -> `if (!c) {} else A`)."""
    sites = []
    for f, n in ctx.of_type("if_statement"):
        cond = cond_value(n.child_by_field_name("condition"))
        if cond is None:
            continue
        if any(c.type == "constexpr" for c in n.children):
            continue
        sites.append((f, n, cond))
    if not sites:
        return None
    f, n, cond = ctx.pick(sites)
    cons = n.child_by_field_name("consequence")
    alt = n.child_by_field_name("alternative")
    if alt is not None and alt.type == "else_clause":
        alt = alt.named_children[0] if alt.named_children else None
    neg = negate(ctx, f, cond)
    if alt is None:
        if ctx.simplify or ctx.rng.random() < 0.7:
            return None
        return [(n.start_byte, n.end_byte, f"if ({neg}) {{\n{ctx.indent_of(n)}}} else {body_text(ctx, cons)}")]
    if alt.type == "if_statement" and ctx.rng.random() < 0.5:
        return None
    a_txt = body_text(ctx, alt)
    c_txt = body_text(ctx, cons)
    return [(n.start_byte, n.end_byte, f"if ({neg}) {a_txt} else {c_txt}")]


def m_empty_then(ctx: Ctx):
    """`if (!c) {} else A` -> `if (c) A`: undo the empty-arm form."""
    sites = []
    for f, n in ctx.of_type("if_statement"):
        cons = n.child_by_field_name("consequence")
        alt = n.child_by_field_name("alternative")
        cond = cond_value(n.child_by_field_name("condition"))
        if cond is None or alt is None or cons is None:
            continue
        if cons.type == "compound_statement" and not [c for c in cons.named_children if c.type != "comment"]:
            sites.append((f, n, cond, alt))
    if not sites:
        return None
    f, n, cond, alt = ctx.pick(sites)
    a = alt.named_children[0] if alt.type == "else_clause" and alt.named_children else alt
    return [(n.start_byte, n.end_byte, f"if ({negate(ctx, f, cond)}) {ctx.T(a)}")]


def m_loop_form(ctx: Ctx):
    sites = ctx.of_type("for_statement", "while_statement", "do_statement")
    if not sites:
        return None
    f, n = ctx.pick(sites)
    ind = ctx.indent_of(n)
    body = n.child_by_field_name("body")
    if body is None:
        return None
    T = ctx.T
    if n.type == "for_statement":
        init = n.child_by_field_name("initializer")
        cond = n.child_by_field_name("condition")
        upd = n.child_by_field_name("update")
        if n.child_by_field_name("value") is not None:  # range for
            return None
        init_txt = ""
        if init is not None:
            init_txt = T(init) if init.type == "declaration" else T(init) + ";"
        cond_txt = T(cond) if cond is not None else ""
        upd_txt = T(upd) if upd is not None else ""
        choice = ctx.rng.random()
        if choice < 0.45:
            # for -> while, increments at the end of the body
            if own_continues(body):
                return None
            if upd_txt:
                if body.type == "compound_statement":
                    close = body.end_byte - 1
                    inner = ctx.text[body.start_byte:close].rstrip()
                    new_body = inner + "\n" + ind + "    " + upd_txt + ";\n" + ind + "}"
                else:
                    new_body = "{ " + T(body) + " " + upd_txt + "; }"
            else:
                new_body = T(body)
            loop = f"while ({cond_txt or '1'}) {new_body}"
        elif choice < 0.75:
            # for -> guarded do/while, increments in the loop test (continue-safe)
            if not cond_txt:
                return None
            # the tests and increments run in the same order as in the for loop,
            # and a continue still reaches the increments
            test = f"({upd_txt}), ({cond_txt})" if upd_txt else cond_txt
            loop = f"if ({cond_txt}) do {body_text(ctx, body)} while ({test});"
        else:
            # move the initialiser out of the header
            if init is None or init.type == "declaration":
                return None
            loop = f"for (; {cond_txt}; {upd_txt}) {T(body)}"
        parts = ([init_txt] if init_txt else []) + [loop]
        return [(n.start_byte, n.end_byte, wrap_stmts(ctx, n, parts))]
    if n.type == "while_statement":
        cond = cond_value(n.child_by_field_name("condition"))
        if cond is None:
            return None
        choice = ctx.rng.random()
        if choice < 0.4:
            # while -> for, trailing increments into the header
            if body.type == "compound_statement" and not own_continues(body):
                stmts = [c for c in body.named_children if c.type in STATEMENT_TYPES]
                tail = []
                for s in reversed(stmts):
                    if is_increment(ctx, f, s) and len(tail) < 3:
                        tail.insert(0, s)
                    else:
                        break
                if tail and len(tail) < len(stmts) + 1:
                    upd = ", ".join(T(s.named_children[0]) for s in tail)
                    edits = [delete_stmt(ctx, s) for s in tail]
                    edits.append((n.start_byte, body.start_byte, f"for (; {T(cond)}; {upd}) "))
                    return edits
            return [(n.start_byte, body.start_byte, f"for (; {T(cond)}; ) ")]
        # while (c) S -> if (c) do S while (c): the same tests in the same order
        return [(n.start_byte, n.end_byte, f"if ({T(cond)}) do {body_text(ctx, body)} while ({T(cond)});")]
    if n.type == "do_statement":
        cond = cond_value(n.child_by_field_name("condition"))
        if cond is None or own_continues(body) or constant(cond):
            return None
        neg = negate(ctx, f, cond)
        if body.type == "compound_statement":
            close = body.end_byte - 1
            inner = ctx.text[body.start_byte:close].rstrip()
            new_body = inner + "\n" + ind + "    if (" + neg + ")\n" + ind + "        break;\n" + ind + "}"
        else:
            new_body = "{ " + T(body) + " if (" + neg + ") break; }"
        return [(n.start_byte, n.end_byte, f"for (;;) {new_body}")]
    return None


def constant(n: Node) -> bool:
    """No variable, field or call in n: a constant expression."""
    return not has_type(n, {"identifier", "field_expression", "call_expression", "subscript_expression",
                            "pointer_expression", "this", "string_literal"})


def is_increment(ctx: Ctx, f: Func, s: Node) -> bool:
    if s.type != "expression_statement" or not s.named_children:
        return False
    x = s.named_children[0]
    if x.type == "update_expression":
        arg = x.child_by_field_name("argument")
        return arg.type == "identifier" and f.is_local(ctx.T(arg))
    if x.type == "assignment_expression" and ctx.T(x.child_by_field_name("operator")) in ("+=", "-="):
        left = x.child_by_field_name("left")
        if left.type != "identifier" or not f.is_local(ctx.T(left)):
            return False
        return pure(f.effects(x.child_by_field_name("right")))
    return False


def enclosing_stmt(n: Node) -> Node | None:
    """The statement directly in a block that contains n."""
    x = n
    while x is not None:
        if x.parent is not None and x.parent.type in ("compound_statement", "case_statement") \
                and x.type in STATEMENT_TYPES:
            return x
        x = x.parent
    return None


def hoist_ok(n: Node, stmt: Node) -> bool:
    """n is evaluated exactly once, unconditionally, when stmt runs, before any
    nested statement of it."""
    x = n
    while x.id != stmt.id:
        p = x.parent
        if p is None:
            return False
        t = p.type
        if t in LOOPS or t in ("compound_statement", "case_statement", "switch_statement", "lambda_expression",
                               "sizeof_expression", "else_clause", "initializer_list"):
            return False
        if t == "if_statement":
            if p.child_by_field_name("condition") is None or x.id != p.child_by_field_name("condition").id:
                return False
        if t == "binary_expression" and ctx_op(p) in ("&&", "||"):
            if x.id == p.child_by_field_name("right").id:
                return False
        if t == "conditional_expression" and x.id != p.child_by_field_name("condition").id:
            return False
        if t == "pointer_expression" and ctx_op(p) == "&":
            return False
        if t == "assignment_expression" and x.id == p.child_by_field_name("left").id:
            return False
        if t == "update_expression":
            return False
        if t == "field_expression" and x.id == p.child_by_field_name("argument").id:
            op = p.child_by_field_name("operator")
            if op is not None and op.type == ".":
                return False
        if t == "call_expression" and x.id == p.child_by_field_name("function").id:
            return False
        x = p
    return True


def ctx_op(n: Node) -> str:
    op = n.child_by_field_name("operator")
    return op.type if op is not None else ""


TEMP_KINDS = {"field_expression", "subscript_expression", "pointer_expression", "binary_expression",
              "cast_expression", "identifier", "call_expression", "unary_expression"}


def m_temp_intro(ctx: Ctx):
    """Name a subexpression: `T tmp = e;` before its statement."""
    sites = []
    for f, n in ctx.nodes():
        if n.type not in TEMP_KINDS:
            continue
        if n.type == "identifier":
            nm = ctx.T(n)
            if f.is_local(nm) or not (nm in ctx.fi.globals or (f.cls and ctx.fi.has_field(f.cls, nm))):
                continue
        if n.type == "pointer_expression" and ctx.T(n.child_by_field_name("operator")) == "&":
            continue
        if n.type == "binary_expression" and ctx_op(n) in ("&&", "||"):
            continue
        sites.append((f, n))
    if not sites:
        return None
    f, n = ctx.pick(sites)
    if constant(n):
        return None
    stmt = enclosing_stmt(n)
    if stmt is None or stmt.id == n.id:
        return None
    if stmt.type not in ("expression_statement", "return_statement", "declaration", "if_statement",
                         "switch_statement"):
        return None
    if not hoist_ok(n, stmt):
        return None
    ty = f.type_of(n)
    if not is_scalar(ty) or ty.endswith("[]"):
        return None
    if ty == "bool" and ctx.rng.random() < 0.5:
        ty = "int"  # `sete dl; test dl, dl` comes from a bool local; an int one differs
    en = f.effects(n)
    if en.control or en.writes or en.mwrites:
        return None
    # n moves ahead of everything else the statement evaluates; a call in n
    # may then only pass operands that nothing else in the statement touches
    if not independent(f, en, stmt_rest_effects(f, stmt, n)):
        return None
    name = ctx.fresh_name("tmp")
    ty_txt = type_text(ty)
    decl = f"{ty_txt} {name} = {ctx.T(n)};"
    ind = ctx.indent_of(stmt)
    if in_block(stmt):
        return [(stmt.start_byte, stmt.start_byte, decl + "\n" + ind), (n.start_byte, n.end_byte, name)]
    return None


def stmt_rest_effects(f: Func, stmt: Node, n: Node) -> Eff:
    """Effects of a statement other than n, ignoring the final store of a top-
    level assignment (it happens after its right-hand side is evaluated)."""
    target = stmt
    if stmt.type == "expression_statement" and stmt.named_children:
        x = stmt.named_children[0]
        if x.type == "assignment_expression":
            left = x.child_by_field_name("left")
            right = x.child_by_field_name("right")
            if right.start_byte <= n.start_byte < right.end_byte:
                e = f.effects(right, skip=n)
                f._address(left, e, 0, 0, None)
                if ctx_op_text(f, x) != "=":
                    lp = f.path(left)
                    if lp is not None and not lp[0].startswith("L:"):
                        e.mreads.append(lp)
                    elif left.type == "identifier":
                        e.reads.add(f.T(left))
                return e
    if stmt.type == "declaration":
        e = Eff()
        for d in stmt.children_by_field_name("declarator"):
            _, _, init = unwrap_declarator(d, f.text)
            if init is not None:
                sub = f.effects(init, skip=n)
                e.reads |= sub.reads
                e.writes |= sub.writes
                e.mreads += sub.mreads
                e.mwrites += sub.mwrites
                e.calls |= sub.calls
        return e
    if stmt.type in ("if_statement", "switch_statement"):
        cond = stmt.child_by_field_name("condition")
        return f.effects(cond, skip=n)
    return f.effects(target, skip=n)


def ctx_op_text(f: Func, n: Node) -> str:
    return f.T(n.child_by_field_name("operator"))


def type_text(ty: str) -> str:
    return ty if not ty.endswith("[]") else ty[:-2] + "*"


def m_temp_inline(ctx: Ctx):
    """`T v = e; ... use(v)` -> `use(e)` for a temporary read once."""
    sites = []
    for f, b, stmts in ctx.blocks():
        for i, s in enumerate(stmts):
            if s.type != "declaration" or not scalar_decl(ctx, f, s):
                continue
            decls = s.children_by_field_name("declarator")
            if len(decls) != 1 or decls[0].type != "init_declarator":
                continue
            name, suffix, init = unwrap_declarator(decls[0], ctx.text)
            if not name or init is None or init.type == "initializer_list" or f.decl_count.get(name) != 1:
                continue
            if name in f.escaped:
                continue
            sites.append((f, stmts, i, name, init))
    if not sites:
        return None
    f, stmts, i, name, init = ctx.pick(sites)
    uses = [u for u in uses_of(ctx, f, name) if not (stmts[i].start_byte <= u.start_byte < stmts[i].end_byte)]
    reexpress = len(uses) > 1
    if not uses:
        return None
    # the variable must never be written again
    for u in uses:
        p = u.parent
        if p.type == "assignment_expression" and p.child_by_field_name("left").id == u.id:
            return None
        if p.type == "update_expression":
            return None
        if p.type == "pointer_expression" and ctx.T(p.child_by_field_name("operator")) == "&":
            return None
    use = ctx.pick(uses) if reexpress else uses[0]
    stmt = enclosing_stmt(use)
    if stmt is None:
        return None
    # the statement holding the use must be in the same block, later
    j = next((k for k in range(i + 1, len(stmts)) if stmts[k].id == stmt.id), None)
    if j is None:
        return None
    ei = f.effects(init)
    if ei.calls or ei.control or ei.writes or ei.mwrites:
        return None
    for k in range(i + 1, j):
        if not independent(f, ei, f.effects(stmts[k])):
            return None
    if not hoist_ok(use, stmt):
        return None
    if not independent(f, ei, stmt_rest_effects(f, stmt, use)):
        return None
    ty = f.var_type(name)
    ity = f.type_of(init)
    if ity is not None and ity == ty:
        repl = ctx.T(init) if is_primary(init) else "(" + ctx.T(init) + ")"
    else:
        repl = f"(({type_text(ty)}){paren(ctx, init)})"
    edits = [(use.start_byte, use.end_byte, repl)]
    if not reexpress:
        edits.append(delete_stmt(ctx, stmts[i]))
    return edits


def m_compound_assign(ctx: Ctx):
    """`x += y` <-> `x = x + y`."""
    sites = []
    for f, n in ctx.of_type("assignment_expression"):
        sites.append((f, n))
    if not sites:
        return None
    f, n = ctx.pick(sites)
    op = ctx.T(n.child_by_field_name("operator"))
    left, right = n.child_by_field_name("left"), n.child_by_field_name("right")
    # the left side is evaluated once in `x op= y` and twice in `x = x op y`
    if has_type(left, {"update_expression", "assignment_expression", "call_expression",
                       "new_expression", "delete_expression"}):
        return None
    ops = {"+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="}
    if op in ops:
        bop = op[:-1]
        rt = ctx.T(right)
        if not is_primary(right) and right.type != "unary_expression":
            rt = "(" + rt + ")"
        return [(n.start_byte, n.end_byte, f"{ctx.T(left)} = {ctx.T(left)} {bop} {rt}")]
    if op == "=" and right.type == "binary_expression":
        bop = ctx.T(right.child_by_field_name("operator"))
        if bop + "=" not in ops:
            return None
        a, b = right.child_by_field_name("left"), right.child_by_field_name("right")
        norm = lambda x: re.sub(r"\s+", "", ctx.T(x))
        if norm(a) == norm(left):
            return [(n.start_byte, n.end_byte, f"{ctx.T(left)} {bop}= {ctx.T(b)}")]
        if bop in ("+", "*", "&", "|", "^") and norm(b) == norm(left) and swappable(f, a, b):
            return [(n.start_byte, n.end_byte, f"{ctx.T(left)} {bop}= {ctx.T(a)}")]
    return None


def m_incdec(ctx: Ctx):
    """`i++` / `++i` / `i += 1` / `i = i + 1` in statement position."""
    sites = []
    for f, n in ctx.nodes():
        p = n.parent
        if p is None:
            continue
        stmt_pos = p.type == "expression_statement" or (
            p.type == "for_statement" and p.child_by_field_name("update") is not None
            and p.child_by_field_name("update").id == n.id) or (
            p.type == "comma_expression" and p.parent is not None and p.parent.type == "for_statement")
        if not stmt_pos:
            continue
        if n.type == "update_expression":
            sites.append((f, n))
        elif n.type == "assignment_expression" and ctx.T(n.child_by_field_name("operator")) in ("+=", "-="):
            r = n.child_by_field_name("right")
            if r.type == "number_literal" and int_literal(ctx.T(r)) == 1:
                sites.append((f, n))
    if not sites:
        return None
    f, n = ctx.pick(sites)
    if n.type == "update_expression":
        arg = n.child_by_field_name("argument")
        op = ctx.T(n.child_by_field_name("operator"))
        a = ctx.T(arg)
        if has_type(arg, {"call_expression", "assignment_expression", "update_expression"}):
            return None
        sign = "+" if op == "++" else "-"
        prefix = n.children[0].type in ("++", "--")
        forms = [f"{a}{op}" if prefix else f"{op}{a}", f"{a} {sign}= 1"]
        if arg.type == "identifier":
            forms.append(f"{a} = {a} {sign} 1")
        return [(n.start_byte, n.end_byte, ctx.rng.choice(forms))]
    left = n.child_by_field_name("left")
    if has_type(left, {"call_expression", "assignment_expression", "update_expression"}):
        return None
    op = ctx.T(n.child_by_field_name("operator"))
    sym = "++" if op == "+=" else "--"
    a = ctx.T(left)
    return [(n.start_byte, n.end_byte, ctx.rng.choice([f"{a}{sym}", f"{sym}{a}"]))]


SAFE_TERM_TYPES = {"identifier", "number_literal", "char_literal", "binary_expression", "unary_expression",
                   "parenthesized_expression", "cast_expression", "true", "false", "null", "nullptr",
                   "field_expression"}


def fault_free(f: Func, n: Node) -> bool:
    """Evaluating n cannot fault or have effects: locals, constants, arithmetic
    without division, members of `this` or of local structs."""
    stack = [n]
    while stack:
        x = stack.pop()
        if x.type not in SAFE_TERM_TYPES and x.type not in ("type_descriptor", "primitive_type",
                                                             "sized_type_specifier", "type_identifier",
                                                             "field_identifier", "abstract_pointer_declarator"):
            return False
        if x.type == "binary_expression" and f.T(x.child_by_field_name("operator")) in ("/", "%"):
            return False
        if x.type == "field_expression":
            if f.T(x.child_by_field_name("operator")) == "->":
                base = x.child_by_field_name("argument")
                if base.type != "this":
                    return False
        if x.type == "identifier":
            nm = f.T(x)
            if not (f.is_local(nm) or nm in f.fi.consts or nm in f.fi.globals
                    or (f.cls and f.fi.has_field(f.cls, nm))):
                return False
        if x.type == "cast_expression":
            stack.append(x.child_by_field_name("value"))
            continue
        stack.extend(c for c in x.named_children if c.type not in ("field_identifier",))
    return True


def m_andor_swap(ctx: Ctx):
    sites = []
    for f, n in ctx.of_type("binary_expression"):
        if ctx_op(n) in ("&&", "||"):
            sites.append((f, n))
    if not sites:
        return None
    f, n = ctx.pick(sites)
    a, b = n.child_by_field_name("left"), n.child_by_field_name("right")
    if not (fault_free(f, a) and fault_free(f, b)):
        return None
    if not swappable(f, a, b):
        return None
    return [(a.start_byte, a.end_byte, paren_for(ctx, b, n)), (b.start_byte, b.end_byte, paren_for(ctx, a, n))]


def m_do_while0(ctx: Ctx):
    """Wrap statements in `do { ... } while (0);`, or unwrap such a block."""
    if ctx.simplify or ctx.rng.random() < 0.3:
        sites = []
        for f, n in ctx.of_type("do_statement"):
            cond = cond_value(n.child_by_field_name("condition"))
            body = n.child_by_field_name("body")
            if cond is None or ctx.T(cond).strip() != "0" or body is None or body.type != "compound_statement":
                continue
            if own_breaks(body) or own_continues(body) or not in_block(n):
                continue
            sites.append((f, n, body))
        if not sites:
            return None
        f, n, body = ctx.pick(sites)
        inner = ctx.text[body.start_byte + 1:body.end_byte - 1].strip()
        if any(c.type == "declaration" for c in body.named_children):
            return [(n.start_byte, n.end_byte, ctx.T(body))]
        return [(n.start_byte, n.end_byte, inner)]
    blocks = ctx.blocks()
    if not blocks:
        return None
    f, b, stmts = ctx.pick(blocks)
    i = ctx.pick_index(stmts)
    j = min(len(stmts) - 1, i + ctx.rng.randrange(4))
    sel = stmts[i:j + 1]
    for s in sel:
        if s.type in ("case_statement", "labeled_statement"):
            return None
        if own_breaks(s) or own_continues(s):
            return None
        if s.type == "break_statement" or s.type == "continue_statement":
            return None
    # declarations inside must not be used after the range
    after = ctx.text[sel[-1].end_byte:b.end_byte]
    for s in sel:
        if s.type == "declaration":
            for d in s.children_by_field_name("declarator"):
                name, _, _ = unwrap_declarator(d, ctx.text)
                if name and re.search(r"\b%s\b" % re.escape(name), after):
                    return None
    ind = ctx.indent_of(sel[0])
    inner = ctx.text[sel[0].start_byte:sel[-1].end_byte].replace("\n", "\n    ")
    return [(sel[0].start_byte, sel[-1].end_byte, f"do {{\n{ind}    {inner}\n{ind}}} while (0);")]


def m_include(ctx: Ctx):
    """Add or remove one standard C header (compiler state can move registers)."""
    text = ctx.text
    present = {h: m for h in STD_HEADERS
               for m in [re.search(r"^[ \t]*#[ \t]*include[ \t]*<%s>[ \t]*\n" % re.escape(h), text, re.M)] if m}
    if present and ctx.rng.random() < 0.4:
        h = ctx.rng.choice(sorted(present))
        m = present[h]
        return [(m.start(), m.end(), "")]
    missing = [h for h in STD_HEADERS if h not in present]
    if not missing:
        return None
    h = ctx.rng.choice(missing)
    first = re.search(r"^[ \t]*#[ \t]*include\b", text, re.M)
    if first is not None:
        pos = first.start()
    else:
        pos = 0
        for m in re.finditer(r"^//[^\n]*\n", text, re.M):
            if m.start() == pos:
                pos = m.end()
            else:
                break
    return [(pos, pos, f"#include <{h}>\n")]


def m_ternary(ctx: Ctx):
    """`x = c ? a : b;` <-> `if (c) x = a; else x = b;`, and the same for return."""
    if ctx.rng.random() < 0.5:
        sites = []
        for f, n in ctx.of_type("expression_statement", "return_statement"):
            x = n.named_children[0] if n.named_children else None
            if x is None:
                continue
            if n.type == "expression_statement":
                if x.type != "assignment_expression" or ctx.T(x.child_by_field_name("operator")) != "=":
                    continue
                if x.child_by_field_name("right").type != "conditional_expression":
                    continue
                sites.append((f, n, x.child_by_field_name("left"), x.child_by_field_name("right")))
            elif x.type == "conditional_expression":
                sites.append((f, n, None, x))
        if not sites:
            return None
        f, n, left, c = ctx.pick(sites)
        cond, a, b = (c.child_by_field_name(k) for k in ("condition", "consequence", "alternative"))
        if a is None or b is None or cond is None:
            return None
        ta, tb = f.type_of(a), f.type_of(b)
        if not ((is_int(ta) and is_int(tb)) or (ta and ta == tb and is_scalar(ta))):
            return None
        if left is not None:
            if has_type(left, {"call_expression", "update_expression", "assignment_expression"}):
                return None
            if not lhs_order_free(f, left, [cond, a, b]):
                return None
            l = ctx.T(left)
            new = f"if ({ctx.T(cond)}) {l} = {ctx.T(a)}; else {l} = {ctx.T(b)};"
        else:
            new = f"if ({ctx.T(cond)}) return {ctx.T(a)}; else return {ctx.T(b)};"
        return [(n.start_byte, n.end_byte, new if in_block(n) else "{ " + new + " }")]
    sites = []
    for f, n in ctx.of_type("if_statement"):
        cons = n.child_by_field_name("consequence")
        alt = n.child_by_field_name("alternative")
        cond = cond_value(n.child_by_field_name("condition"))
        if cons is None or alt is None or cond is None:
            continue
        a = alt.named_children[0] if alt.type == "else_clause" and alt.named_children else alt
        sa, sb = single_stmt(cons), single_stmt(a)
        if sa is None or sb is None:
            continue
        sites.append((f, n, cond, sa, sb))
    if not sites:
        return None
    f, n, cond, sa, sb = ctx.pick(sites)
    if sa.type == "return_statement" and sb.type == "return_statement":
        va = sa.named_children[0] if sa.named_children else None
        vb = sb.named_children[0] if sb.named_children else None
        if va is None or vb is None:
            return None
        ta, tb = f.type_of(va), f.type_of(vb)
        if not ((is_int(ta) and is_int(tb)) or (ta and ta == tb and is_scalar(ta))):
            return None
        return [(n.start_byte, n.end_byte, f"return {ctx.T(cond)} ? {paren(ctx, va)} : {paren(ctx, vb)};")]
    if sa.type == "expression_statement" and sb.type == "expression_statement":
        xa, xb = sa.named_children[0], sb.named_children[0]
        if xa.type != "assignment_expression" or xb.type != "assignment_expression":
            return None
        if ctx.T(xa.child_by_field_name("operator")) != "=" or ctx.T(xb.child_by_field_name("operator")) != "=":
            return None
        la, lb = xa.child_by_field_name("left"), xb.child_by_field_name("left")
        if re.sub(r"\s+", "", ctx.T(la)) != re.sub(r"\s+", "", ctx.T(lb)):
            return None
        if has_type(la, {"call_expression", "update_expression", "assignment_expression"}):
            return None
        ra, rb = xa.child_by_field_name("right"), xb.child_by_field_name("right")
        if not lhs_order_free(f, la, [cond, ra, rb]):
            return None
        ta, tb = f.type_of(ra), f.type_of(rb)
        if not ((is_int(ta) and is_int(tb)) or (ta and ta == tb and is_scalar(ta))):
            return None
        return [(n.start_byte, n.end_byte,
                 f"{ctx.T(la)} = {ctx.T(cond)} ? {paren(ctx, ra)} : {paren(ctx, rb)};")]
    return None


def lhs_order_free(f: Func, left: Node, others: list[Node]) -> bool:
    """Can the address of `left` be computed before or after `others`?"""
    e = Eff()
    f._address(left, e, 0, 0, None)
    for o in others:
        if not independent(f, e, f.effects(o)):
            return False
    return True


def single_stmt(s: Node) -> Node | None:
    if s.type == "compound_statement":
        kids = [c for c in s.named_children if c.type != "comment"]
        return kids[0] if len(kids) == 1 and kids[0].type in ("expression_statement", "return_statement") else None
    return s if s.type in ("expression_statement", "return_statement") else None


def m_nested_if(ctx: Ctx):
    """`if (a && b) S` <-> `if (a) { if (b) S }` (no else)."""
    sites = []
    for f, n in ctx.of_type("if_statement"):
        if n.child_by_field_name("alternative") is not None:
            continue
        cond = cond_value(n.child_by_field_name("condition"))
        if cond is None:
            continue
        while cond.type == "parenthesized_expression" and len(cond.named_children) == 1:
            cond = cond.named_children[0]
        if cond.type == "binary_expression" and ctx_op(cond) == "&&":
            sites.append(("split", f, n, cond))
        cons = n.child_by_field_name("consequence")
        inner = cons
        if inner.type == "compound_statement":
            kids = [c for c in inner.named_children if c.type != "comment"]
            inner = kids[0] if len(kids) == 1 else None
        if inner is not None and inner.type == "if_statement" and inner.child_by_field_name("alternative") is None \
                and cond_value(inner.child_by_field_name("condition")) is not None:
            sites.append(("merge", f, n, inner))
    if not sites:
        return None
    kind, f, n, x = ctx.pick(sites)
    cons = n.child_by_field_name("consequence")
    if kind == "split":
        a, b = x.child_by_field_name("left"), x.child_by_field_name("right")
        ind = ctx.indent_of(n)
        inner = f"if ({ctx.T(b)}) {ctx.T(cons)}".replace("\n", "\n    ")
        return [(n.start_byte, n.end_byte, f"if ({ctx.T(a)}) {{\n{ind}    {inner}\n{ind}}}")]
    c1 = cond_value(n.child_by_field_name("condition"))
    c2 = cond_value(x.child_by_field_name("condition"))
    p = lambda c: ctx.T(c) if not (c.type == "binary_expression" and ctx_op(c) == "||") and c.type not in (
        "conditional_expression", "assignment_expression", "comma_expression") else "(" + ctx.T(c) + ")"
    return [(n.start_byte, n.end_byte, f"if ({p(c1)} && {p(c2)}) {ctx.T(x.child_by_field_name('consequence'))}")]


def m_zero_compare(ctx: Ctx):
    """`!e` <-> `e == 0`, and `if (e)` <-> `if (e != 0)`."""
    sites = []
    for f, n in ctx.nodes():
        if n.type == "unary_expression" and ctx.T(n.child_by_field_name("operator")) == "!":
            sites.append(("not", f, n))
        elif n.type == "binary_expression" and ctx_op(n) in ("==", "!="):
            r = n.child_by_field_name("right")
            if r.type in ("number_literal", "null", "nullptr") and int_literal(ctx.T(r)) == 0 or ctx.T(r) in ("NULL",):
                sites.append(("cmp", f, n))
        elif is_condition(n):
            sites.append(("cond", f, n))
    if not sites:
        return None
    kind, f, n = ctx.pick(sites)
    if constant(n):
        return None
    if kind == "not":
        a = n.child_by_field_name("argument")
        return [(n.start_byte, n.end_byte, wrap_cmp(n, f"{paren_for_cmp(ctx, a)} == 0"))]
    if kind == "cmp":
        a = n.child_by_field_name("left")
        if ctx_op(n) == "==":
            return [(n.start_byte, n.end_byte, f"!{paren(ctx, a)}")]
        if is_condition(n):
            return [(n.start_byte, n.end_byte, ctx.T(a) if is_primary(a) else "(" + ctx.T(a) + ")")]
        return None
    if n.type in ("binary_expression",) and ctx_op(n) in ("==", "!=", "<", ">", "<=", ">=", "&&", "||"):
        return None
    if n.type == "unary_expression":
        return None
    return [(n.start_byte, n.end_byte, wrap_cmp(n, f"{paren_for_cmp(ctx, n)} != 0"))]


LOOSE_PARENTS = {"condition_clause", "parenthesized_expression", "argument_list", "expression_statement",
                 "return_statement", "init_declarator", "comma_expression", "for_statement",
                 "conditional_expression", "assignment_expression"}


def wrap_cmp(n: Node, s: str) -> str:
    """Parenthesise a new `x == 0` unless its context binds more loosely than ==."""
    p = n.parent
    if p is None:
        return "(" + s + ")"
    if p.type in LOOSE_PARENTS or (p.type == "binary_expression" and ctx_op(p) in ("&&", "||")):
        return s
    return "(" + s + ")"


def paren_for_cmp(ctx: Ctx, n: Node) -> str:
    s = ctx.T(n)
    if n.type in ("binary_expression", "conditional_expression", "assignment_expression", "comma_expression"):
        op = ctx_op(n) if n.type == "binary_expression" else ""
        if PREC.get(op, 0) > PREC["=="]:
            return s
        return "(" + s + ")"
    return s


def is_condition(n: Node) -> bool:
    """Is n's value only tested for zero / non-zero?"""
    p = n.parent
    if p is None:
        return False
    if p.type == "condition_clause" and p.child_by_field_name("value") is not None \
            and p.child_by_field_name("value").id == n.id:
        gp = p.parent
        return gp is not None and gp.type in ("if_statement", "while_statement")
    if p.type == "parenthesized_expression" and p.parent is not None and p.parent.type == "do_statement":
        return True
    if p.type == "for_statement" and p.child_by_field_name("condition") is not None \
            and p.child_by_field_name("condition").id == n.id:
        return True
    if p.type == "binary_expression" and ctx_op(p) in ("&&", "||"):
        return True
    if p.type == "unary_expression" and p.child_by_field_name("operator").type == "!":
        return True
    if p.type == "conditional_expression" and p.child_by_field_name("condition").id == n.id:
        return True
    return False


def m_cast(ctx: Ctx):
    """Add a cast of a local to its own type, or remove such a cast."""
    sites = []
    for f, n in ctx.nodes():
        if n.type == "cast_expression":
            v = n.child_by_field_name("value")
            tn = n.child_by_field_name("type")
            if v is not None and v.type == "identifier" and f.is_local(ctx.T(v)) and tn is not None:
                if canon_type(ctx.T(tn)) == f.var_type(ctx.T(v)):
                    sites.append(("remove", f, n))
        elif n.type == "identifier" and f.is_local(ctx.T(n)):
            p = n.parent
            if p is None or p.type in ("init_declarator", "pointer_declarator", "array_declarator",
                                       "reference_declarator", "update_expression", "parameter_declaration",
                                       "declaration", "function_declarator", "cast_expression"):
                continue
            if p.type == "assignment_expression" and p.child_by_field_name("left").id == n.id:
                continue
            if p.type == "pointer_expression" and ctx.T(p.child_by_field_name("operator")) == "&":
                continue
            if p.type == "field_expression" or p.type == "call_expression" and p.child_by_field_name("function").id == n.id:
                continue
            if p.type == "subscript_expression" and p.child_by_field_name("argument").id == n.id:
                continue
            ty = f.var_type(ctx.T(n))
            if not is_int(ty) and not is_float(ty) and not (is_ptr(ty) and ty.endswith("*")):
                continue
            sites.append(("add", f, n))
    if not sites:
        return None
    kind, f, n = ctx.pick(sites)
    if kind == "remove":
        return [(n.start_byte, n.end_byte, ctx.T(n.child_by_field_name("value")))]
    ty = f.var_type(ctx.T(n))
    decl_txt = original_type_text(ctx, f, ctx.T(n)) or ty
    return [(n.start_byte, n.end_byte, f"(({decl_txt}){ctx.T(n)})")]


def original_type_text(ctx: Ctx, f: Func, name: str) -> str | None:
    """The type of local `name` as spelled in its declaration."""
    stack = [f.node]
    while stack:
        n = stack.pop()
        if n.type in ("declaration", "parameter_declaration"):
            tnode = n.child_by_field_name("type")
            ds = n.children_by_field_name("declarator")
            for d in ds:
                nm, suffix, _ = unwrap_declarator(d, ctx.text)
                if nm == name and tnode is not None and "[]" not in suffix and "&" not in suffix and "(" not in suffix:
                    quals = " ".join(ctx.T(c) for c in n.children if c.type == "type_qualifier")
                    base = (quals + " " if quals else "") + ctx.T(tnode)
                    return base + suffix
        stack.extend(n.named_children)
    return None


def star_type(prefix: str) -> str:
    """`int` -> `int* `, `unsigned short` -> `unsigned short* `, keeping any
    spacing, so the star lands on the type and not on the name."""
    head = prefix.rstrip()
    gap = prefix[len(head):] or " "
    return head + "*" + gap


def local_type_text(ctx: Ctx, f: Func, name: str) -> str | None:
    """The type of local `name` as spelled in its own declaration, or None when
    it cannot be recovered (a macro, an elaborated type)."""
    ty = original_type_text(ctx, f, name)
    return ty if ty is not None and "(" not in ty and "[]" not in ty else None


def within(n: Node, *types: str) -> bool:
    """Is n inside a node of one of these types?"""
    p = n.parent
    while p is not None:
        if p.type in types:
            return True
        p = p.parent
    return False


def subscript_index(n: Node) -> Node | None:
    """The index expression of a subscript. This grammar leaves the `index`
    field empty and hangs the index off as a `subscript_argument_list`."""
    idx = n.child_by_field_name("index")
    if idx is not None:
        return idx
    arg = n.child_by_field_name("argument")
    kids = [c for c in n.named_children if arg is None or c.id != arg.id]
    if len(kids) != 1:
        return None
    idx = kids[0]
    if idx.type == "subscript_argument_list":
        kids = idx.named_children
        idx = kids[0] if len(kids) == 1 else None
    return idx


def addressable_chain(n: Node) -> bool:
    """An lvalue built only from identifiers, field accesses and subscripts by
    a constant or a plain name: taking its address has no side effect, so it
    can be done once where a copy used to be made."""
    t = n.type
    if t in ("identifier", "this"):
        return True
    if t == "field_expression":
        return addressable_chain(n.child_by_field_name("argument"))
    if t == "subscript_expression":
        idx = subscript_index(n)
        return idx is not None and idx.type in ("number_literal", "identifier") and \
            addressable_chain(n.child_by_field_name("argument"))
    if t == "pointer_expression" and ctx_op(n) == "*":
        return addressable_chain(n.child_by_field_name("argument"))
    return False


def copy_local(ctx: Ctx, f: Func, n: Node):
    """`T x = e;` in a block: (declarator, name, type, e) for a local that is
    initialised once, from an expression, and never declared twice."""
    if n.type != "declaration" or not in_block(n):
        return None
    tnode = n.child_by_field_name("type")
    head = ctx.text[n.start_byte:(tnode or n).end_byte]
    if re.search(r"\b(?:static|extern|volatile|register|typedef)\b", head):
        return None
    if re.search(r"\bvolatile\b", ctx.text[f.node.start_byte:f.node.end_byte]):
        return None  # a volatile read repeated is not the same as a read once
    decls = n.children_by_field_name("declarator")
    if len(decls) != 1 or decls[0].type != "init_declarator":
        return None
    name, suffix, init = unwrap_declarator(decls[0], ctx.text)
    if name is None or init is None or init.type == "initializer_list" or has_error(init):
        return None
    if "&" in suffix or "[]" in suffix or "(" in suffix:
        return None
    if f.decl_count.get(name) != 1 or name in f.escaped or name in f.refs:
        return None
    ty = make_type(ctx.T(tnode), suffix)
    if not is_scalar(ty):
        return None
    # `&arr` of an array field is a pointer to the array, not to its first
    # element, so a pointer local cannot take its address this way
    if is_ptr(ty) and (f.type_of(init) or "").endswith("[]"):
        return None
    return decls[0], name, ty, init


def ptr_local(ctx: Ctx, f: Func, n: Node):
    """`T* x = &e;` in a block: the same, for a local initialised with an address."""
    if n.type != "declaration" or not in_block(n):
        return None
    tnode = n.child_by_field_name("type")
    head = ctx.text[n.start_byte:(tnode or n).end_byte]
    if re.search(r"\b(?:static|extern|volatile|register|typedef)\b", head):
        return None
    if re.search(r"\bvolatile\b", ctx.text[f.node.start_byte:f.node.end_byte]):
        return None  # a volatile read repeated is not the same as a read once
    decls = n.children_by_field_name("declarator")
    if len(decls) != 1 or decls[0].type != "init_declarator":
        return None
    name, suffix, init = unwrap_declarator(decls[0], ctx.text)
    if name is None or init is None or has_error(init):
        return None
    if suffix.count("*") != 1 or "&" in suffix or "[]" in suffix or "(" in suffix:
        return None
    if f.decl_count.get(name) != 1 or name in f.escaped or name in f.refs:
        return None
    while init.type == "parenthesized_expression" and init.named_children:
        init = init.named_children[0]
    if init.type != "pointer_expression" or ctx_op(init) != "&":
        return None
    arg = init.child_by_field_name("argument")
    if arg is None or has_error(arg) or not is_scalar(make_type(ctx.T(tnode), suffix)[:-1]):
        return None
    return decls[0], name, make_type(ctx.T(tnode), suffix)[:-1], arg


def same_type(f: Func, ty: str, n: Node) -> bool:
    """Is `n` of the type the local was declared with? The array and pointer
    spellings of one type count as one (`int[]` and `int*`)."""
    got = f.type_of(n)
    return got is not None and canon_type(type_text(got)) == canon_type(type_text(ty))


def read_only_uses(ctx: Ctx, f: Func, name: str, decl: Node) -> list[Node] | None:
    """Every use of `name` outside its declaration, or None when one of them is
    not a plain read (written, incremented, addressed or called through)."""
    out = []
    for u in uses_of(ctx, f, name):
        if decl.start_byte <= u.start_byte < decl.end_byte:
            continue
        p = u.parent
        if p is None:
            return None
        if p.type == "assignment_expression" and p.child_by_field_name("left").id == u.id:
            return None
        if p.type == "update_expression":
            return None
        if p.type == "pointer_expression" and ctx_op(p) == "&":
            return None
        if p.type == "call_expression" and p.child_by_field_name("function").id == u.id:
            return None
        if within(u, "sizeof_expression", "type_identifier", "template_function",
                  "qualified_identifier"):
            return None
        out.append(u)
    return out or None


def stable_storage(f: Func, path, decl: Node, last: Node) -> bool:
    """Nothing the function does between the declaration and the last use may
    write what `path` names: a copy reads it once, a pointer reads it every time.

    Only the statements in that byte range matter. When the path names something
    outside this function, a write to a local of it is skipped: `f.path` gives an
    unescaped local the root `L:`, and the address of such a local is never taken,
    so no pointer in the path can reach it.
    """
    lo, hi = decl.end_byte, last.end_byte
    local_path = str(path[0]).startswith("L:")
    local_name = str(path[0])[2:] if local_path else None
    stack = [f.body]
    while stack:
        n = stack.pop()
        if n.type in STATEMENT_TYPES and lo <= n.start_byte <= hi:
            e = f.effects(n)
            if e.calls:
                return False  # an unknown callee may write anything
            if local_name is not None and local_name in e.writes:
                return False  # the local the pointer would point at is written
            for w in e.mwrites:
                if not local_path and str(w[0]).startswith("L:"):
                    continue  # a stack slot no pointer in the path can reach
                if may_alias(f, w, path):
                    return False
        stack.extend(n.named_children)
    return True


def m_copy_to_ptr(ctx: Ctx):
    """`T x = e; ... x` -> `T* x = &e; ... *x`: keep the copy an lvalue.

    A local initialised from a global or a field and read more than once has to
    live in a register, and which register it wins decides the allocation of the
    rest of the block (0x47de60: the width's copy took ecx, the register the
    original spends on spotX, which cost the folded imul and the shared features
    lookup at once). As a pointer the value stays an lvalue to the multiply, so
    the front end folds it instead of allocating a register for it.
    """
    sites = []
    for f, n in ctx.of_type("declaration"):
        got = copy_local(ctx, f, n)
        if got is None:
            continue
        d, name, ty, init = got
        if not addressable_chain(init) or has_error(init):
            continue
        p = f.path(init)
        if p is None or not same_type(f, ty, init):
            continue
        uses = read_only_uses(ctx, f, name, d)
        if uses is None:
            continue
        last = max(uses, key=lambda u: u.start_byte)
        if not stable_storage(f, p, d, last):
            continue
        sites.append((f, n, d, name, init, uses))
    if not sites:
        return None
    f, n, d, name, init, uses = ctx.pick(sites)
    return [(n.start_byte, n.end_byte,
             f"{star_type(local_type_text(ctx, f, name))}{name} = &{ctx.T(init)};")] + \
        [(u.start_byte, u.end_byte, f"(*{name})") for u in uses]


def m_ptr_to_copy(ctx: Ctx):
    """`T* x = &e; ... *x` -> `T x = e; ... x`: the other direction."""
    sites = []
    for f, n in ctx.of_type("declaration"):
        got = ptr_local(ctx, f, n)
        if got is None:
            continue
        d, name, ty, arg = got
        if not addressable_chain(arg) or has_error(arg):
            continue
        p = f.path(arg)
        if p is None or not same_type(f, ty, arg):
            continue
        uses = read_only_uses(ctx, f, name, d)
        if uses is None:
            continue
        last = max(uses, key=lambda u: u.start_byte)
        if not stable_storage(f, p, d, last):
            continue
        sites.append((f, n, d, name, arg, uses))
    if not sites:
        return None
    f, n, d, name, arg, uses = ctx.pick(sites)
    base = (local_type_text(ctx, f, name) or "").rstrip()
    edits = [(n.start_byte, n.end_byte,
              f"{base[:-1].rstrip() if base.endswith('*') else base} {name} = {ctx.T(arg)};")]
    # a use the forward direction wrote as `(*x)` goes back to `x`
    for u in uses:
        p = u.parent
        if p is not None and p.type == "pointer_expression" and ctx_op(p) == "*" \
                and p.child_by_field_name("argument").id == u.id and len(p.named_children) == 1:
            edits.append((p.start_byte, p.end_byte, name))
    return edits

def m_sign(ctx: Ctx):
    """int <-> unsigned int for a local whose every use is sign-blind."""
    sites = []
    for f, n in ctx.of_type("declaration"):
        tnode = n.child_by_field_name("type")
        if tnode is None:
            continue
        base = canon_type(ctx.T(tnode))
        if base not in ("int", "unsigned int", "long", "unsigned long"):
            continue
        decls = n.children_by_field_name("declarator")
        if len(decls) != 1:
            continue
        name, suffix, init = unwrap_declarator(decls[0], ctx.text)
        if not name or suffix or f.decl_count.get(name) != 1 or name in f.escaped:
            continue
        sites.append((f, n, tnode, name, base, init))
    if not sites:
        return None
    f, n, tnode, name, base, init = ctx.pick(sites)
    if init is not None and not sign_blind_value(ctx, f, init):
        return None
    for u in uses_of(ctx, f, name):
        if n.start_byte <= u.start_byte < n.end_byte:
            continue
        if not sign_blind_use(ctx, f, u):
            return None
    new = {"int": "unsigned int", "unsigned int": "int", "long": "unsigned long", "unsigned long": "long"}[base]
    return [(tnode.start_byte, tnode.end_byte, new)]


def sign_blind_value(ctx: Ctx, f: Func, v: Node) -> bool:
    t = f.type_of(v)
    return is_int(t) or (v.type == "number_literal" and int_literal(ctx.T(v)) is not None)


def sign_blind_use(ctx: Ctx, f: Func, u: Node) -> bool:
    """The use reads or writes all 32 bits and nothing depends on the sign."""
    p = u.parent
    if p.type == "assignment_expression":
        op = ctx.T(p.child_by_field_name("operator"))
        if p.child_by_field_name("left").id == u.id:
            if op in ("=", "+=", "-=", "*=", "&=", "|=", "^=", "<<="):
                return sign_blind_value(ctx, f, p.child_by_field_name("right")) and stmt_level(p)
            return False
        # used as a value assigned to something else of a 32-bit integer type
        lt = f.type_of(p.child_by_field_name("left"))
        return op == "=" and lt in ("int", "unsigned int", "long", "unsigned long", "DWORD", "UINT") \
            and stmt_level(p)
    if p.type == "update_expression":
        return stmt_level(p)
    if p.type == "binary_expression":
        op = ctx_op(p)
        other = p.child_by_field_name("right") if p.child_by_field_name("left").id == u.id else p.child_by_field_name("left")
        if op in ("==", "!="):
            return sign_blind_value(ctx, f, other)
        return False
    if p.type == "subscript_expression" and p.child_by_field_name("index").id == u.id:
        return True
    if p.type == "condition_clause" or is_condition(u):
        return True
    return False


def stmt_level(n: Node) -> bool:
    p = n.parent
    return p is not None and (p.type == "expression_statement" or p.type == "for_statement"
                              or p.type == "comma_expression" and p.parent is not None
                              and p.parent.type == "for_statement")


def m_goto_polarity(ctx: Ctx):
    """`if (c) { A }` -> `if (!c) goto skip; A; skip:;` (sets which arm falls through)."""
    sites = []
    for f, n in ctx.of_type("if_statement"):
        if n.child_by_field_name("alternative") is not None or not in_block(n):
            continue
        cond = cond_value(n.child_by_field_name("condition"))
        cons = n.child_by_field_name("consequence")
        if cond is None or cons is None or cons.type != "compound_statement":
            continue
        if any(c.type == "declaration" for c in cons.named_children):
            continue
        sites.append((f, n, cond, cons))
    if not sites:
        return None
    f, n, cond, cons = ctx.pick(sites)
    label = ctx.fresh_name("skip")
    ind = ctx.indent_of(n)
    inner = ctx.text[cons.start_byte + 1:cons.end_byte - 1].strip("\n").rstrip()
    inner = re.sub(r"^    ", "", inner, flags=re.M) if inner.startswith(ind + "    ") else inner.strip()
    return [(n.start_byte, n.end_byte,
             f"if ({negate(ctx, f, cond)}) goto {label};\n{ind}{inner.strip()}\n{label}:;")]


def m_return_var(ctx: Ctx):
    """`return e;` <-> `T r = e; return r;` (a named result can change registers)."""
    sites = []
    for f, n in ctx.of_type("return_statement"):
        if not n.named_children or not in_block(n):
            continue
        sites.append((f, n, n.named_children[0]))
    if not sites:
        return None
    f, n, v = ctx.pick(sites)
    if v.type in ("number_literal", "identifier"):
        return None
    ty = f.type_of(v)
    if not is_scalar(ty) or ty.endswith("[]"):
        return None
    name = ctx.fresh_name("ret")
    ind = ctx.indent_of(n)
    return [(n.start_byte, n.end_byte, f"{type_text(ty)} {name} = {ctx.T(v)};\n{ind}return {name};")]


def m_dead_decl(ctx: Ctx):
    """Remove a local that is never used and whose initialiser has no effect."""
    sites = []
    for f, n in ctx.of_type("declaration"):
        if not in_block(n):
            continue
        decls = n.children_by_field_name("declarator")
        if len(decls) != 1:
            continue
        name, suffix, init = unwrap_declarator(decls[0], ctx.text)
        if not name or "&" in suffix or f.decl_count.get(name) != 1:
            continue
        tnode = n.child_by_field_name("type")
        if not is_scalar(make_type(ctx.T(tnode), suffix)):
            continue
        if init is not None:
            e = f.effects(init)
            if e.calls or e.writes or e.mwrites or e.control:
                continue
        if [u for u in uses_of(ctx, f, name) if not (n.start_byte <= u.start_byte < n.end_byte)]:
            continue
        sites.append(n)
    if not sites:
        return None
    return [delete_stmt(ctx, ctx.pick(sites))]


def m_strip_parens(ctx: Ctx):
    """`(x)` -> `x` where x is a primary expression, and `((e))` -> `(e)`."""
    sites = []
    for f, n in ctx.of_type("parenthesized_expression"):
        p = n.parent
        if p is None or p.type in ("condition_clause", "sizeof_expression", "do_statement",
                                   "while_statement", "if_statement", "switch_statement"):
            continue
        kids = n.named_children
        if len(kids) != 1:
            continue
        inner = kids[0]
        if inner.type == "comma_expression":
            continue
        loose = p.type in ("argument_list", "init_declarator", "return_statement", "expression_statement") \
            or (p.type == "assignment_expression" and p.child_by_field_name("right") is not None
                and p.child_by_field_name("right").id == n.id)
        if loose or inner.type in ("identifier", "number_literal", "field_expression", "subscript_expression",
                                   "call_expression", "parenthesized_expression", "this", "string_literal",
                                   "char_literal"):
            if p.type == "call_expression" and p.child_by_field_name("function") is not None \
                    and p.child_by_field_name("function").id == n.id:
                continue
            sites.append((n, inner))
    if not sites:
        return None
    n, inner = ctx.pick(sites)
    gap = " " if re.match(r"\w", ctx.text[n.start_byte - 1:n.start_byte]) else ""
    return [(n.start_byte, n.end_byte, gap + ctx.T(inner))]


HELPER_KINDS = {"binary_expression", "field_expression", "subscript_expression", "unary_expression",
                "pointer_expression", "cast_expression"}


def annotation_line_start(ctx: Ctx, defn: Node) -> int:
    """Offset of the `// FUNCTION:` line above a definition (or the definition)."""
    pos = defn.start_byte
    lines_above = ctx.text[:pos].split("\n")
    for back in range(1, 16):
        if back > len(lines_above):
            break
        line = lines_above[-back]
        if ANNOTATION.match(line):
            return len("\n".join(lines_above[:-back])) + (1 if len(lines_above) > back else 0)
    return ctx.text.rfind("\n", 0, pos) + 1


def m_extract_helper(ctx: Ctx):
    """Move an expression into a `static inline` helper: `inl0(a, b)` with
    `static inline T inl0(A a, B b) { return <expression>; }` (an inlined
    function boundary changes MSVC's evaluation order and registers)."""
    primary = ctx.funcs[0]
    if primary.body is None or primary.node.parent is None or primary.node.parent.type not in (
            "translation_unit", "namespace_definition", "declaration_list", "linkage_specification"):
        return None
    sites = [n for f, n in ctx.nodes() if f is primary and n.type in HELPER_KINDS]
    if not sites:
        return None
    n = ctx.pick(sites)
    if constant(n) or not hoist_ok_expr(n):
        return None
    f = primary
    e = f.effects(n)
    if e.calls or e.control or e.writes or e.mwrites:
        return None
    ty = f.type_of(n)
    if not is_scalar(ty) or ty.endswith("[]"):
        if ty == "bool":
            ty = "int"
        else:
            return None
    params: list[str] = []
    decls: list[str] = []
    rename: list[tuple[int, int, str]] = []
    stack = [n]
    idents = []
    while stack:
        x = stack.pop()
        if x.type == "this":
            if not f.cls:
                return None
            rename.append((x.start_byte, x.end_byte, "self"))
            if "self" not in params:
                params.append("self")
                decls.append(f"{f.cls}* self")
            continue
        if x.type == "identifier":
            idents.append(x)
            continue
        if x.type == "pointer_expression" and ctx.T(x).lstrip().startswith("&"):
            # Locals become by-value parameters, so `&local` in the helper
            # would take the address of the copy (#5010).
            inner = [x]
            while inner:
                y = inner.pop()
                if y.type == "identifier" and f.is_local(ctx.T(y)):
                    return None
                inner.extend(y.named_children)
        if x.type == "field_expression":
            stack.append(x.child_by_field_name("argument"))
            continue
        stack.extend(x.named_children)
    for x in sorted(idents, key=lambda i: i.start_byte):
        name = ctx.T(x)
        if f.is_local(name):
            if name in f.refs or name in f.escaped and "[]" in (f.var_type(name) or ""):
                return None
            t = original_type_text(ctx, f, name)
            if t is None:
                return None
            if name not in params:
                params.append(name)
                decls.append(f"{t} {name}" if not t.endswith("*") else f"{t}{name}")
        elif f.cls and ctx.fi.has_field(f.cls, name):
            rename.append((x.start_byte, x.end_byte, "self->" + name))
            if "self" not in params:
                params.append("self")
                decls.append(f"{f.cls}* self")
        elif name in ctx.fi.globals or name in ctx.fi.consts or name in ctx.fi.funcs:
            continue
        else:
            return None
    body = ctx.T(n)
    for s, e2, r in sorted(rename, reverse=True):
        body = body[:s - n.start_byte] + r + body[e2 - n.start_byte:]
    name = ctx.fresh_name("inl")
    args = ", ".join("this" if p == "self" else p for p in params)
    helper = f"static inline {type_text(ty)} {name}({', '.join(decls)}) {{ return {body}; }}\n\n"
    pos = annotation_line_start(ctx, primary.node)
    return [(pos, pos, helper), (n.start_byte, n.end_byte, f"{name}({args})")]


def hoist_ok_expr(n: Node) -> bool:
    """n is a value (not written, not address-taken, not a call target)."""
    p = n.parent
    if p is None:
        return False
    if p.type == "assignment_expression" and p.child_by_field_name("left").id == n.id:
        return False
    if p.type == "update_expression":
        return False
    if p.type == "pointer_expression" and ctx_op(p) == "&":
        return False
    if p.type == "call_expression" and p.child_by_field_name("function").id == n.id:
        return False
    if p.type == "field_expression" and p.child_by_field_name("argument").id == n.id:
        op = p.child_by_field_name("operator")
        if op is not None and op.type == ".":
            return False
    if p.type == "sizeof_expression":
        return False
    return True


def helper_params(ctx: Ctx, f: Func, nodes: list[Node]) -> tuple[list[str], list[str], list[tuple]] | None:
    """Parameters for a helper holding `nodes`: the locals they read (by value),
    `self` for members of `this`; None if something cannot be passed."""
    params: list[str] = []
    decls: list[str] = []
    rename: list[tuple[int, int, str]] = []
    idents = []
    stack = list(nodes)
    while stack:
        x = stack.pop()
        if x.type == "this":
            if not f.cls:
                return None
            rename.append((x.start_byte, x.end_byte, "self"))
            if "self" not in params:
                params.append("self")
                decls.append(f"{f.cls}* self")
            continue
        if x.type == "identifier":
            idents.append(x)
            continue
        if x.type == "field_expression":
            stack.append(x.child_by_field_name("argument"))
            continue
        if x.type in ("declaration", "labeled_statement", "lambda_expression"):
            return None
        stack.extend(x.named_children)
    for x in sorted(idents, key=lambda i: i.start_byte):
        name = ctx.T(x)
        p = x.parent
        if p is not None and p.type == "call_expression" and p.child_by_field_name("function").id == x.id \
                and not f.is_local(name):
            continue  # a function called by name
        if f.is_local(name):
            if name in f.refs or name in f.escaped or "[]" in (f.var_type(name) or ""):
                return None  # a copy would not see (or make) the changes through its address
            t = original_type_text(ctx, f, name)
            if t is None:
                return None
            if name not in params:
                params.append(name)
                decls.append(f"{t} {name}" if not t.endswith("*") else f"{t}{name}")
        elif f.cls and ctx.fi.has_field(f.cls, name):
            rename.append((x.start_byte, x.end_byte, "self->" + name))
            if "self" not in params:
                params.append("self")
                decls.append(f"{f.cls}* self")
        elif f.cls and ctx.fi.method_type(f.cls, name) is not None:
            return None  # an implicit method call on this
        elif name in ctx.fi.globals or name in ctx.fi.consts or name in ctx.fi.funcs:
            continue
        else:
            return None
    return params, decls, rename


def m_extract_stmts(ctx: Ctx):
    """Move one to three adjacent statements into `static inline void inlN(...)`
    (two stores in an inline helper can be what orders them, agent guide 0x4644d0)."""
    primary = ctx.funcs[0]
    if primary.body is None or primary.node.parent is None or primary.node.parent.type not in (
            "translation_unit", "namespace_definition", "declaration_list", "linkage_specification"):
        return None
    blocks = [(f, b, s) for f, b, s in ctx.blocks() if f is primary]
    if not blocks:
        return None
    f, b, stmts = ctx.pick(blocks)
    i = ctx.pick_index(stmts)
    j = min(len(stmts) - 1, i + ctx.rng.randrange(3))
    sel = stmts[i:j + 1]
    for s in sel:
        if s.type not in ("expression_statement", "if_statement", "compound_statement"):
            return None
        e = f.effects(s)
        if e.control or e.writes:
            return None  # a helper cannot assign the caller's locals
        if has_type(s, {"declaration", "return_statement", "break_statement", "continue_statement",
                        "goto_statement", "labeled_statement", "case_statement"}):
            return None
    got = helper_params(ctx, f, sel)
    if got is None:
        return None
    params, decls, rename = got
    lo = sel[0].start_byte
    body = ctx.text[lo:sel[-1].end_byte]
    for s, e2, r in sorted(rename, reverse=True):
        body = body[:s - lo] + r + body[e2 - lo:]
    body = " ".join(line.strip() for line in body.split("\n"))
    name = ctx.fresh_name("inl")
    args = ", ".join("this" if p == "self" else p for p in params)
    helper = f"static inline void {name}({', '.join(decls)}) {{ {body} }}\n\n"
    pos = annotation_line_start(ctx, primary.node)
    return [(pos, pos, helper), (lo, sel[-1].end_byte, f"{name}({args});")]


def m_self_store(ctx: Ctx):
    """`T t = v; v = t;` after a store to local v: compiles to nothing, yet can
    change live ranges (agent guide, 0x461b10)."""
    sites = []
    for f, b, stmts in ctx.blocks():
        for s in stmts:
            if s.type == "expression_statement" and s.named_children:
                x = s.named_children[0]
                if x.type == "assignment_expression" and x.child_by_field_name("left").type == "identifier":
                    name = ctx.T(x.child_by_field_name("left"))
                    if f.is_local(name) and name not in f.escaped and is_scalar(f.var_type(name)) \
                            and not (f.var_type(name) or "").endswith("[]"):
                        sites.append((f, s, name))
            elif s.type == "declaration" and scalar_decl(ctx, f, s):
                for d in s.children_by_field_name("declarator"):
                    name, suffix, init = unwrap_declarator(d, ctx.text)
                    if name and init is not None and name not in f.escaped:
                        sites.append((f, s, name))
    if not sites:
        return None
    f, s, name = ctx.pick(sites)
    t = original_type_text(ctx, f, name)
    if t is None or not in_block(s):
        return None
    tmp = ctx.fresh_name("same")
    ind = ctx.indent_of(s)
    return [(s.end_byte, s.end_byte, f"\n{ind}{t} {tmp} = {name};\n{ind}{name} = {tmp};")]


SELF_STORE = re.compile(r"\n[ \t]*[^\n;{}]*\b(same\d+) = (\w+);\n[ \t]*\2 = \1;")


def m_drop_self_store(ctx: Ctx):
    """Undo m_self_store."""
    found = list(SELF_STORE.finditer(ctx.text))
    if not found:
        return None
    m = ctx.rng.choice(found)
    return [(m.start(), m.end(), "")]


HELPER_DEF = re.compile(r"static inline [^\n]*? (inl\d+)\(([^)]*)\) \{ return (.*); \}\n\n")
VOID_HELPER_DEF = re.compile(r"static inline void (inl\d+)\(([^)]*)\) \{ (.*) \}\n\n")


def m_inline_helper(ctx: Ctx):
    """Undo m_extract_helper and m_extract_stmts: replace a call of an `inlN`
    helper by its body."""
    defs = {m.group(1): m for m in HELPER_DEF.finditer(ctx.text)}
    voids = {m.group(1): m for m in VOID_HELPER_DEF.finditer(ctx.text)}
    if voids and (not defs or ctx.rng.random() < 0.5):
        return inline_void_helper(ctx, voids)
    if not defs:
        return None
    calls = []
    for f, n in ctx.nodes():
        if n.type == "call_expression":
            fn = n.child_by_field_name("function")
            if fn is not None and fn.type == "identifier" and ctx.T(fn) in defs:
                calls.append((f, n, ctx.T(fn)))
    if not calls:
        return None
    f, n, name = ctx.pick(calls)
    m = defs[name]
    params = [re.search(r"(\w+)\s*$", p).group(1) for p in m.group(2).split(",") if p.strip()]
    args = [ctx.T(a) for a in n.child_by_field_name("arguments").named_children]
    if len(args) != len(params) or any(a != p and not (p == "self" and a == "this") for a, p in zip(args, params)):
        return None
    body = m.group(3).replace("self->", "this->") if "self" in params else m.group(3)
    p = n.parent
    loose = p is not None and (p.type in ("argument_list", "init_declarator", "return_statement",
                                          "expression_statement", "parenthesized_expression")
                               or (p.type == "assignment_expression" and p.child_by_field_name("right").id == n.id))
    simple = re.fullmatch(r"\w+(?:(?:\.|->)\w+|\[\w+\])*", body) is not None
    edits = [(n.start_byte, n.end_byte, body if (loose or simple) and "," not in body else "(" + body + ")")]
    others = [c for c in calls if c[2] == name and c[1].id != n.id]
    if not others and len(re.findall(r"\b%s\b" % name, ctx.text)) == 2:
        edits.append((m.start(), m.end(), ""))
    return edits


def inline_void_helper(ctx: Ctx, voids: dict):
    calls = []
    for f, n in ctx.nodes():
        if n.type == "call_expression":
            fn = n.child_by_field_name("function")
            if fn is not None and fn.type == "identifier" and ctx.T(fn) in voids \
                    and n.parent is not None and n.parent.type == "expression_statement":
                calls.append((f, n, ctx.T(fn)))
    if not calls:
        return None
    f, n, name = ctx.pick(calls)
    m = voids[name]
    params = [re.search(r"(\w+)\s*$", p).group(1) for p in m.group(2).split(",") if p.strip()]
    args = [ctx.T(a) for a in n.child_by_field_name("arguments").named_children]
    if len(args) != len(params) or any(a != p and not (p == "self" and a == "this") for a, p in zip(args, params)):
        return None
    body = re.sub(r"\bself->", "this->", m.group(3)) if "self" in params else m.group(3)
    body = re.sub(r"\bself\b", "this", body) if "self" in params else body
    stmt = n.parent
    edits = [(stmt.start_byte, stmt.end_byte, body if in_block(stmt) else "{ " + body + " }")]
    if len(re.findall(r"\b%s\b" % name, ctx.text)) == 2:
        edits.append((m.start(), m.end(), ""))
    return edits


def m_loop_back(ctx: Ctx):
    """Undo loop_form's one-way shapes:
    `if (c) do S while ((u), (c));` -> `for (; c; u) S` (or `while (c) S`),
    `for (;;) { S; if (x) break; }` -> `do { S } while (!x);`, and
    `e; for (; c; u) S` -> `for (e; c; u) S`."""
    T = ctx.T
    sites = []
    for f, n in ctx.of_type("if_statement", "for_statement"):
        if n.type == "if_statement":
            cons = n.child_by_field_name("consequence")
            cond = cond_value(n.child_by_field_name("condition"))
            if n.child_by_field_name("alternative") is None and cons is not None and cons.type == "do_statement" \
                    and cond is not None:
                sites.append(("guarded", f, n, cons, cond))
        else:
            init = n.child_by_field_name("initializer")
            cond = n.child_by_field_name("condition")
            upd = n.child_by_field_name("update")
            body = n.child_by_field_name("body")
            if init is None and cond is None and upd is None and body is not None \
                    and body.type == "compound_statement":
                sites.append(("forever", f, n, body, None))
            elif init is None and in_block(n):
                prev = n.prev_named_sibling
                while prev is not None and prev.type == "comment":
                    prev = prev.prev_named_sibling
                if prev is not None and prev.type == "expression_statement" and prev.named_children \
                        and prev.named_children[0].type in ("assignment_expression", "comma_expression"):
                    sites.append(("init", f, n, prev, None))
    if not sites:
        return None
    kind, f, n, x, cond = ctx.pick(sites)
    if kind == "guarded":
        dcond = cond_value(x.child_by_field_name("condition"))
        body = x.child_by_field_name("body")
        if dcond is None or body is None:
            return None
        norm = lambda s: re.sub(r"[\s()]", "", s)
        if norm(T(dcond)) == norm(T(cond)):
            return [(n.start_byte, n.end_byte, f"while ({T(cond)}) {T(body)}")]
        if dcond.type == "comma_expression":
            parts = dcond.named_children
            if len(parts) == 2 and norm(T(parts[1])) == norm(T(cond)):
                upd = T(parts[0])
                while upd.startswith("(") and upd.endswith(")") and upd.count("(") == 1:
                    upd = upd[1:-1]
                return [(n.start_byte, n.end_byte, f"for (; {T(cond)}; {upd}) {T(body)}")]
        return None
    if kind == "forever":
        stmts = [c for c in x.named_children if c.type in STATEMENT_TYPES]
        if not stmts or own_continues(x):
            return None
        last = stmts[-1]
        if last.type != "if_statement" or last.child_by_field_name("alternative") is not None:
            return None
        cons = last.child_by_field_name("consequence")
        inner = single_stmt(cons) if cons.type == "compound_statement" else cons
        if cons.type == "compound_statement":
            kids = [c for c in cons.named_children if c.type != "comment"]
            inner = kids[0] if len(kids) == 1 else None
        if inner is None or inner.type != "break_statement":
            return None
        c = cond_value(last.child_by_field_name("condition"))
        if c is None:
            return None
        rest = ctx.text[x.start_byte:last.start_byte].rstrip() + "\n" + ctx.indent_of(n) + "}"
        return [(n.start_byte, n.end_byte, f"do {rest} while ({negate(ctx, f, c)});")]
    # init: move the statement before the loop into its empty initialiser
    expr = T(x.named_children[0])
    body_start = n.child_by_field_name("condition") or n.child_by_field_name("update") \
        or n.child_by_field_name("body")
    head = ctx.text[n.start_byte:body_start.start_byte]
    m = re.match(r"for\s*\(\s*;", head)
    if m is None:
        return None
    return [delete_stmt(ctx, x), (n.start_byte, n.start_byte + m.end(), f"for ({expr};")]


def m_goto_back(ctx: Ctx):
    """Undo goto_polarity: `if (c) goto skipN; A skipN:;` -> `if (!c) { A }`."""
    for f, b, stmts in ctx.blocks():
        for i, s in enumerate(stmts):
            if s.type != "if_statement" or s.child_by_field_name("alternative") is not None:
                continue
            g = s.child_by_field_name("consequence")
            if g is None or g.type != "goto_statement":
                continue
            label = ctx.T(g.child_by_field_name("label"))
            if not re.fullmatch(r"skip\d+", label) or len(re.findall(r"\b%s\b" % label, ctx.text)) != 2:
                continue
            for j in range(i + 1, len(stmts)):
                t = stmts[j]
                if t.type == "labeled_statement" and ctx.T(t.child_by_field_name("label")) == label:
                    inner = [x for x in stmts[i + 1:j]]
                    after = [c for c in t.named_children if c.type in STATEMENT_TYPES]
                    if after and ctx.T(after[0]).strip() != ";":
                        break
                    cond = cond_value(s.child_by_field_name("condition"))
                    if cond is None:
                        break
                    ind = ctx.indent_of(s)
                    body = ctx.text[inner[0].start_byte:inner[-1].end_byte] if inner else ""
                    body = body.replace("\n", "\n    ")
                    return [(s.start_byte, t.end_byte,
                             f"if ({negate(ctx, f, cond)}) {{\n{ind}    {body}\n{ind}}}")]
    return None


def m_strip_braces(ctx: Ctx):
    """`if (c) { S }` -> `if (c) S` for one plain statement (no if, so no
    dangling else, and no declaration)."""
    sites = []
    for f, n in ctx.of_type("compound_statement"):
        p = n.parent
        if p is None or p.type not in ("if_statement", "else_clause", "for_statement", "while_statement",
                                       "do_statement"):
            continue
        kids = [c for c in n.named_children]
        if len(kids) != 1 or kids[0].type not in ("expression_statement", "return_statement", "break_statement",
                                                    "continue_statement", "goto_statement"):
            continue
        sites.append((n, kids[0]))
    if not sites:
        return None
    n, inner = ctx.pick(sites)
    return [(n.start_byte, n.end_byte, ctx.T(inner))]


def m_dead_helper(ctx: Ctx):
    """Remove an `inlN` helper that nothing calls any more."""
    found = [m for m in list(HELPER_DEF.finditer(ctx.text)) + list(VOID_HELPER_DEF.finditer(ctx.text))
             if len(re.findall(r"\b%s\b" % m.group(1), ctx.text)) == 1]
    if not found:
        return None
    m = ctx.rng.choice(found)
    return [(m.start(), m.end(), "")]


def m_convention(ctx: Ctx):
    """Toggle the calling convention of the target when it has no parameters:
    the ABI is the same, the compiler's code can differ (agent guide, 0x46c920).
    Callees are left alone: their convention is fixed by their own files."""
    if ctx.funcs[0].cls is not None:
        return None
    sites = [ctx.funcs[0].node]
    choices = []
    for s in sites:
        decl = s.child_by_field_name("declarator")
        fd = find_function_declarator(decl)
        if fd is None:
            continue
        name = fd.child_by_field_name("declarator")
        if name is None or "::" in ctx.T(name):
            continue
        params = fd.child_by_field_name("parameters")
        ptxt = ctx.T(params).replace(" ", "")
        if ptxt not in ("()", "(void)"):
            continue
        choices.append((s, name))
    if not choices:
        return None
    s, name = ctx.pick(choices)
    head = ctx.text[s.start_byte:name.start_byte]
    m = re.search(r"\b(__stdcall|__cdecl|__fastcall|_stdcall|_cdecl)\b\s*", head)
    options = ["", "__cdecl ", "__fastcall ", "__stdcall "]
    if m:
        current = m.group(1).lstrip("_")
        new = ctx.rng.choice([o for o in options if o.strip().lstrip("_") != current])
        return [(s.start_byte + m.start(), s.start_byte + m.end(), new)]
    new = ctx.rng.choice(options[1:3])
    return [(name.start_byte, name.start_byte, new)]


def m_subscript_form(ctx: Ctx):
    """`a[i]` and `(*(a + i))`, for a pointer or array a and an integer i, both ways."""
    sites = []
    for f, n in ctx.of_type("subscript_expression"):
        base, idx = n.child_by_field_name("argument"), subscript_index(n)
        if base is None or idx is None or not is_ptr(f.type_of(base)) or not is_int(f.type_of(idx)):
            continue
        sites.append((n, f"(*({paren(ctx, base)} + {paren(ctx, idx)}))"))
    for f, n in ctx.of_type("pointer_expression"):
        if ctx.T(n.child_by_field_name("operator")) != "*":
            continue
        arg = n.child_by_field_name("argument")
        while arg is not None and arg.type == "parenthesized_expression" and len(arg.named_children) == 1:
            arg = arg.named_children[0]
        if arg is None or arg.type != "binary_expression" or ctx.T(arg.child_by_field_name("operator")) != "+":
            continue
        a, b = arg.child_by_field_name("left"), arg.child_by_field_name("right")
        if is_ptr(f.type_of(b)) and is_int(f.type_of(a)):
            a, b = b, a
        if is_ptr(f.type_of(a)) and is_int(f.type_of(b)):
            sites.append((n, f"{paren(ctx, a)}[{ctx.T(b)}]"))
    if not sites:
        return None
    n, new = ctx.pick(sites)
    return [(n.start_byte, n.end_byte, new)]


MUTATIONS = {
    "move_stmt": (m_move_stmt, 30),
    "move_decl": (m_move_decl, 12),
    "split_multi_decl": (m_split_multi_decl, 3),
    "merge_decls": (m_merge_decls, 2),
    "split_init": (m_split_init, 5),
    "merge_init": (m_merge_init, 4),
    "decl_scope": (m_decl_scope, 5),
    "swap_commutative": (m_swap_commutative, 10),
    "flip_compare": (m_flip_compare, 5),
    "negate_if": (m_negate_if, 8),
    "empty_then": (m_empty_then, 2),
    "loop_form": (m_loop_form, 5),
    "temp_intro": (m_temp_intro, 12),
    "temp_inline": (m_temp_inline, 10),
    "compound_assign": (m_compound_assign, 4),
    "incdec": (m_incdec, 4),
    "andor_swap": (m_andor_swap, 3),
    "do_while0": (m_do_while0, 2),
    "include": (m_include, 3),
    "ternary": (m_ternary, 3),
    "nested_if": (m_nested_if, 3),
    "zero_compare": (m_zero_compare, 4),
    "cast": (m_cast, 3),
    "sign": (m_sign, 3),
    "copy_to_ptr": (m_copy_to_ptr, 4),
    "ptr_to_copy": (m_ptr_to_copy, 2),
    "goto_polarity": (m_goto_polarity, 1),
    "return_var": (m_return_var, 2),
    "dead_decl": (m_dead_decl, 1),
    "strip_parens": (m_strip_parens, 1),
    "extract_helper": (m_extract_helper, 5),
    "extract_stmts": (m_extract_stmts, 3),
    "inline_helper": (m_inline_helper, 2),
    "convention": (m_convention, 1),
    "self_store": (m_self_store, 1),
    "drop_self_store": (m_drop_self_store, 1),
    "dead_helper": (m_dead_helper, 1),
    "loop_back": (m_loop_back, 1),
    "goto_back": (m_goto_back, 1),
    "strip_braces": (m_strip_braces, 1),
    "subscript_form": (m_subscript_form, 3),
}

# The kinds that can undo another kind's change, for cleaning up a result
# (tools/permute.py's simplify phase), with their weights there.
SIMPLIFY = {
    "move_stmt": 10, "move_decl": 6, "merge_decls": 3, "merge_init": 6, "decl_scope": 3,
    "swap_commutative": 6, "flip_compare": 4, "negate_if": 3, "empty_then": 4, "loop_form": 3,
    "temp_inline": 10, "compound_assign": 3, "incdec": 3, "andor_swap": 2, "do_while0": 3,
    "include": 2, "ternary": 2, "nested_if": 3, "zero_compare": 4, "cast": 4, "sign": 1,
    "copy_to_ptr": 1, "ptr_to_copy": 6,
    "dead_decl": 8, "strip_parens": 8, "split_multi_decl": 1, "inline_helper": 8, "convention": 1,
    "drop_self_store": 8, "dead_helper": 8, "loop_back": 6, "goto_back": 6, "strip_braces": 6,
    "subscript_form": 2,
}


class Mutator:
    """Applies random mutations to the targets of one file."""

    def __init__(self, base_text: str, spec: TargetSpec, weights: dict[str, float] | None = None):
        self.spec = spec
        tree = parse(base_text)
        self.fi = build_file_info(tree.root_node, base_text)
        self.weights = {k: (weights or {}).get(k, w) for k, (_, w) in MUTATIONS.items()}
        self.simplify = False

    def errors_in_targets(self, text: str) -> int | None:
        ctx = Ctx(text, self.spec, self.fi, random.Random(0))
        if ctx.error or not ctx.funcs:
            return None
        return sum(1 for f in ctx.funcs if f.node.has_error)

    def mutate(self, text: str, rng: random.Random, n: int = 1, tries: int = 60,
               hot=()) -> tuple[str, list[str]]:
        """Apply n mutations; returns the new text and the mutation names.
        `hot` lists 1-based source lines that the mutations should aim at."""
        names = []
        base_errors = self.errors_in_targets(text)
        if base_errors is None:
            return text, []
        keys = [k for k in self.weights if self.weights[k] > 0]
        w = [self.weights[k] for k in keys]
        for _ in range(n):
            ctx = Ctx(text, self.spec, self.fi, rng)
            ctx.simplify = self.simplify
            if hot and not names:
                ctx.set_hot(hot)  # the lines belong to the parent text; later mutations go blind
            if not ctx.funcs:
                return text, names
            for _attempt in range(tries):
                kind = rng.choices(keys, w)[0]
                try:
                    edits = MUTATIONS[kind][0](ctx)
                except (ValueError, AttributeError, IndexError, KeyError, TypeError):
                    edits = None
                if not edits:
                    continue
                try:
                    new = apply_edits(text, edits)
                except ValueError:
                    continue
                if new == text:
                    continue
                errs = self.errors_in_targets(new)
                if errs is None or errs > base_errors:
                    continue
                text = new
                names.append(kind)
                break
        return text, names
