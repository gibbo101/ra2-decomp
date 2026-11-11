#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Link the whole executable from the project's objects, at the original's addresses.

Three levels, each a superset of the last:

  (default)       address-faithful stub link: every object from build.py, a stub object
                  (stubgen.py: one 0xCC COMDAT per function no matched row provides, sized
                  from the function map, zero-filled data), the toolchain's LIBCMT, COMSUPP,
                  LIBCPMT and import libraries and a binkw32.lib written by implib.py, with an
                  /ORDER file that lays .text out in the original's order. No /FORCE: every
                  external must resolve. mapcheck.py then compares each matched function's
                  linked address with the manifest. The image does not run (stubs are int3).
  --hybrid        a runnable image: unmatched code, gaps and the unidentified end of .text take
                  the original's bytes, a matched body that differs from them gives its slot
                  back, and a data object (hybrid.py) supplies .rdata, .data (with the .bss
                  extent), the import tables and .rsrc at their original addresses. Every
                  reference in matched code is pinned to the address the original uses
                  (pindata.py) and the library tail is laid out as the original's
                  (taillink.py). compare_image.py then compares every section with the
                  original and counts the bytes that come from source.
  --source-data   (with --hybrid) data the objects define at a known address and whose
                  compiled contents and relocation targets equal the original's come from the
                  objects (datasource.py); --search-literals also places string and float
                  literals with no known address where the original holds exactly their bytes.

The original's bytes are read from build/exelink/gamemd_1001.exe (prepare.py) while linking;
everything written goes under build/exelink/, which git ignores. Never commit its outputs.

Usage:
  uv run tools/exelink/prepare.py && uv run tools/exelink/build.py
  uv run tools/exelink/link.py [--hybrid [--source-data [--search-literals]]] [--out DIR]
"""

import argparse
import json
import os
from pathlib import Path
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import paths  # noqa: E402

os.environ.setdefault("GAMEMD_BINARY", str(paths.RETAIL))

import compare_image  # noqa: E402
import datasource  # noqa: E402
import hybrid as hybrid_module  # noqa: E402
import implib  # noqa: E402
import link_diag  # noqa: E402
import mapcheck  # noqa: E402
import pindata  # noqa: E402
import stubgen  # noqa: E402
import taillink  # noqa: E402

TOOL_LIBS = link_diag.TOOL_LIBS + ["OLDNAMES.LIB", "UUID.LIB"]
IMPORT_LIBS = link_diag.IMPORT_LIBS
# The original has two import-thunk blocks: WSOCK32, DDRAW, DSOUND, VERSION and IMM32 thunks
# before LIBCMT (0x7C8962), KERNEL32 and OLEAUT32 thunks after COMSUPP (0x7DCE62), so those
# import libraries are listed around the CRT in that order.
EARLY_IMPORT_LIBS = ["WSOCK32.LIB", "DDRAW.LIB", "DSOUND.LIB", "VERSION.LIB", "IMM32.LIB"]
LATE_IMPORT_LIBS = ["KERNEL32.LIB", "OLEAUT32.LIB"]
# /OPT:REF as the original (only 89 import thunks survive, so unreferenced thunk code was
# discarded); the stub object carries an anchor table that references every ordered entry so
# the layout survives it. /OPT:NOICF: the original keeps identical bodies at separate addresses,
# and folding would merge the equal-sized 0xCC stubs.
LINK_FLAGS = ["/NOLOGO", "/SUBSYSTEM:WINDOWS", "/FIXED", "/BASE:0x400000", "/INCREMENTAL:NO",
              "/OPT:REF", "/OPT:NOICF", "/MACHINE:IX86"]
MAX_ROUNDS = 4


def library_order(bink):
    rest = [l for l in IMPORT_LIBS if l not in EARLY_IMPORT_LIBS + LATE_IMPORT_LIBS]
    return EARLY_IMPORT_LIBS + TOOL_LIBS + LATE_IMPORT_LIBS + rest + [bink]


def library_symbols(extra=()):
    names = set()
    for lib in TOOL_LIBS + IMPORT_LIBS:
        path = paths.LIBDIR / lib
        if path.is_file():
            names |= link_diag.archive_symbols(path)
    for path in extra:
        names |= link_diag.archive_symbols(path)
    return names


def link(objects, libs, out, includes=()):
    _, env = link_diag.toolenv()
    wp = link_diag.wine_path
    rsp = out / "link.rsp"
    lines = LINK_FLAGS + ["/ORDER:@" + wp(out / "stubs/order.txt"), "/OUT:" + wp(out / "image.exe"),
                          "/MAP:" + wp(out / "image.map")]
    lines += [f"/INCLUDE:{n}" for n in includes]
    lines += [f'"{wp(o)}"' for o in objects] + [f'"{wp(l)}"' if "/" in str(l) else l for l in libs]
    rsp.write_text("\r\n".join(lines) + "\r\n", encoding="latin-1")
    proc = subprocess.run(["wine", str(paths.BIN / "LINK.EXE"), "@" + wp(rsp)], env=env,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    log = proc.stdout.decode("latin-1")
    (out / "link.log").write_text(log, encoding="latin-1")
    return proc.returncode, log


def load_manifest():
    return json.loads(paths.MANIFEST.read_text())


def equal_rows(objdir, index, manifest, retail):
    """The manifest rows whose compiled bytes equal the original's here: only those say where their
    references point (a body that differs, from another compiler setup or a stale object, would
    derive nonsense addresses from its relocation sites)."""
    from verify_vtable import VerifyError, parse_coff
    tu = {o["source"]: objdir / o["object"] for o in index["objects"]}
    parsed, out = {}, []
    for row in manifest:
        path = tu.get(row["tu"])
        if path is not None and path not in parsed:
            try:
                parsed[path] = parse_coff(path)
            except VerifyError:
                parsed[path] = None
        if path is None or parsed[path] is None or hybrid_module.function_matches(
                parsed[path], row["sym"], int(row["addr"], 16), int(row["len"], 16), retail) is not False:
            out.append(row)
    return out


def run(objdir, out, hybrid=False, source_data=False, search=()):
    out.mkdir(parents=True, exist_ok=True)
    retail = None
    linked_diff = {}
    reject = set()   # (VA, size) of source-placed data that did not come out equal to the original's bytes
    pin_data = order_tail = hybrid
    if hybrid:
        retail = hybrid_module.Retail(pindata.retail_path())
        data_obj, _ = hybrid_module.data_object(retail)
        (out / "hybrid_data.obj").write_bytes(data_obj)
    bink = out / "binkw32.lib"
    implib.build_from_def(paths.BINKDEF, bink)
    library_names = library_symbols([bink])
    extra, rounds = set(), []
    pin_plan = None
    if pin_data:   # derived once from the original; stubgen.plan adds call pins to it
        index = json.loads((objdir / "objects.json").read_text())
        pe, _, pe_sections = pindata.load_retail()
        pin_plan = pindata.derive(objdir, index, equal_rows(objdir, index, load_manifest(), retail), pe, pe_sections)
    for _ in range(MAX_ROUNDS + 4):
        result = stubgen.plan(objdir, library_names, out / "stubs", extra, pin_data=pin_data, pin_plan=pin_plan,
                              order_tail=order_tail, libdir=paths.LIBDIR, retail=retail,
                              source_data=source_data, data_reject=reject, data_search=search)
        objects = result["objects"]
        if source_data:
            data_obj, _ = hybrid_module.data_object(retail, result["_data_pieces"])
            (out / "hybrid_data.obj").write_bytes(data_obj)
        includes = ()
        if hybrid:
            objects = [out / "hybrid_data.obj"] + objects
            includes = result["order_symbols"]
        code, log = link(objects, library_order(bink), out, includes)
        unresolved, duplicates = link_diag.parse_link_log(log)
        rounds.append({"exit": code, "unresolved": len(unresolved), "duplicates": len(duplicates)})
        new = set(unresolved) - extra
        if source_data and code == 0 and not new:
            # a source-placed piece the linker put somewhere else, or filled differently, gives its place back
            bad = datasource.verify_image((out / "image.exe").read_bytes(), result["_data_pieces"], retail)
            rounds[-1]["data_pieces_differing"] = len(bad)
            if bad:
                reject |= set(bad)
                linked_diff.update({f"0x{va:X}+{size}": [list(d) for d in diffs] for (va, size), diffs in bad.items()})
                continue
        if not new:
            break
        extra |= new
    if hybrid:
        placement = [[".text", va, size, origin, name] for va, size, origin, name in result["text_pieces"]]
        for p in result.get("_data_pieces", ()):
            section = ".data" if p["va"] >= retail.by_name[".data"][0] else ".rdata"
            placement.append([section, p["va"], p["size"], "project", p["names"][0]])
        (out / "placement.json").write_text(json.dumps({"pieces": placement}) + "\n")
    if linked_diff:
        (out / "data_linked_diff.json").write_text(json.dumps(linked_diff, indent=1) + "\n")
    summary = {k: v for k, v in result.items()
               if k not in ("placed", "objects", "order_symbols", "text_pieces") and not k.startswith("_")}
    summary.update({"rounds": rounds, "link_exit_code": code, "unresolved_after_stubs": len(unresolved),
                    "unresolved_symbols": sorted(unresolved), "duplicates": sorted(duplicates),
                    "warnings": sorted({l.strip() for l in log.splitlines() if "warning LNK" in l})[:200]})
    return summary


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--objdir", type=Path, default=paths.OBJDIR)
    ap.add_argument("--out", type=Path)
    ap.add_argument("--hybrid", action="store_true", help="runnable image over the original's bytes (see above)")
    ap.add_argument("--source-data", action="store_true", help="with --hybrid: data from the objects where it equals the original's")
    ap.add_argument("--search-literals", action="store_true", help="with --source-data: place unbound literals by content")
    args = ap.parse_args(argv)
    if args.source_data and not args.hybrid:
        ap.error("--source-data needs --hybrid")
    if args.search_literals and not args.source_data:
        ap.error("--search-literals needs --source-data")
    if not paths.RETAIL.is_file():
        ap.error("run tools/exelink/prepare.py first")
    out = (args.out or paths.WORK / ("hybrid" if args.hybrid else "stub")).resolve()
    if paths.WORK.resolve() not in out.parents:
        ap.error("the link writes bytes derived from the original: --out must be under build/exelink/")
    summary = run(args.objdir.resolve(), out, hybrid=args.hybrid, source_data=args.source_data,
                  search=("string", "float") if args.search_literals else ())
    (out / "link_report.json").write_text(json.dumps(summary, indent=1) + "\n")
    print(f"LINK exit {summary['link_exit_code']}; unresolved after stubs: {summary['unresolved_after_stubs']}; "
          f"duplicates: {len(summary['duplicates'])}; rounds: {summary['rounds']}")
    print(f"ordered entries {summary['ordered_entries']} (matched {summary['matched_placed']}, "
          f"stubs {summary['stubs_placed']}); demoted to original bytes "
          f"{len(summary['hybrid_rows_demoted_to_retail_bytes'])}")
    if not (out / "image.map").is_file():
        return 1
    check = mapcheck.check(out / "image.map", out / "stubs/plan.json")
    (out / "mapcheck.json").write_text(json.dumps(check, indent=1) + "\n")
    mapcheck.print_summary(check)
    if args.hybrid:
        publics, _ = mapcheck.parse_map((out / "image.map").read_text(encoding="latin-1"))
        tail = taillink.check_map(publics, taillink.load_layout())
        (out / "tailcheck.json").write_text(json.dumps(tail, indent=1) + "\n")
        print(f"library tail: {tail['at_retail_address']} of {tail['symbols']} symbols at their original addresses "
              f"({tail['displaced']} displaced, {tail['not_in_map']} not in map)")
        if summary["link_exit_code"] == 0:
            compare_image.main([str(out / "image.exe"), "--json", str(out / "compare.json")])
    return 0 if summary["link_exit_code"] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
