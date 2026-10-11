#!/usr/bin/env bash
# Assemble Visual C++ 6.0 with Service Pack 3 (C2.DLL 12.00.8447) into toolchain/vc6sp3
# under Wine, from hash-pinned downloads. Needs: curl, 7z, cabextract, wine, python3.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TC="$ROOT/toolchain"
DL="$TC/downloads"
VC="$TC/vc6sp3"
mkdir -p "$DL"

for tool in curl 7z cabextract wine wineboot python3 sha256sum; do
    command -v "$tool" >/dev/null || { echo "missing: $tool" >&2; exit 1; }
done

# Resumes a stalled transfer until the file is complete, then checks it.
fetch() {  # url dest sha256
    local url="$1" dest="$2" sum="$3" tries=0
    if [ ! -f "$dest" ]; then
        until curl -fSL --speed-limit 10240 --speed-time 30 -C - -o "$dest.part" "$url"; do
            tries=$((tries + 1))
            [ "$tries" -lt 50 ] || { echo "download failed: $url" >&2; exit 1; }
            sleep 5
        done
        mv "$dest.part" "$dest"
    fi
    echo "$sum  $dest" | sha256sum -c --quiet
}

BASE_ISO="$DL/en_vs6_ent_cd1.iso"
SP3_ISO="$DL/TNSB9908.iso"
fetch https://archive.org/download/en_vs6_ent/en_vs6_ent_cd1.iso "$BASE_ISO" \
    d38250b86c3bef0d8123cade40abd1728a776e22e9633683c3e311a913ae0347
fetch https://archive.org/download/ms-technet-tnsb9908/TNSB9908.iso "$SP3_ISO" \
    f3eefbd0b7d7958a44ef63c2bb28bcf75ef8d5adcd2e65082f54fb399004a928

# Copies every file in $1 into $2 under its upper-cased name, replacing any case variant.
overlay() {  # src_dir dest_dir
    local f b
    for f in "$1"/*; do
        [ -f "$f" ] || continue
        b="$(basename "$f")"
        find "$2" -maxdepth 1 -iname "$b" -delete
        cp "$f" "$2/${b^^}"
    done
}

if [ ! -d "$VC" ]; then
    tmp="$(mktemp -d -p "$TC")"
    trap 'rm -rf "$tmp"' EXIT

    7z x -y -bso0 -bsp0 "$BASE_ISO" -o"$tmp/base" \
        'VC98/BIN/*' 'VC98/INCLUDE/*' 'VC98/LIB/*' \
        'COMMON/MSDEV98/BIN/MSPDB60.DLL' 'COMMON/MSDEV98/BIN/MSDIS110.DLL' \
        'COMMON/MSDEV98/BIN/MSOBJ10.DLL' 'COMMON/MSDEV98/BIN/RC.EXE' \
        'COMMON/MSDEV98/BIN/RCDLL.DLL'
    mkdir -p "$tmp/vc"
    mv "$tmp/base/VC98/"{BIN,INCLUDE,LIB} "$tmp/vc/"
    cp "$tmp/base/COMMON/MSDEV98/BIN/"* "$tmp/vc/BIN/"

    7z e -y -bso0 -bsp0 "$SP3_ISO" -o"$tmp/sp3" 'VSTUDIO/SP3/VS6SP3_1.EXE' 'VSTUDIO/SP3/VS6SP3_5.EXE'
    # The service pack's own setup picks C2.DLL by edition; msvcep.dll is the
    # Professional and Enterprise one, the only 12.00.8447 build.
    7z e -y -bso0 -bsp0 "$tmp/sp3/VS6SP3_1.EXE" -o"$tmp/sp3/1" msvcep.dll
    cp "$tmp/sp3/1/msvcep.dll" "$tmp/vc/BIN/C2.DLL"
    # VS6sp3_4.cab spans its neighbours, but every file wanted here lies wholly inside it.
    7z e -y -bso0 -bsp0 "$tmp/sp3/VS6SP3_5.EXE" -o"$tmp/sp3/5" VS6sp3_4.cab
    cabextract -q -F 'vc98/bin/*' -F 'vc98/include/*' -F 'vc98/lib/*' \
        -d "$tmp/sp3/5/x" "$tmp/sp3/5/VS6sp3_4.cab" 2>/dev/null || true
    overlay "$tmp/sp3/5/x/vc98/bin" "$tmp/vc/BIN"
    overlay "$tmp/sp3/5/x/vc98/include" "$tmp/vc/INCLUDE"
    overlay "$tmp/sp3/5/x/vc98/lib" "$tmp/vc/LIB"

    mv "$tmp/vc" "$VC"
    rm -rf "$tmp"
    trap - EXIT
fi

# The files that decide what the build emits, as the base install plus SP3 leaves them.
(cd "$VC" && sha256sum -c --quiet) <<'EOF'
d9220fdfaf355c81f26e7b03c5dc628ec72ff81c99dc38ed98b1ad4bdbbb309c  BIN/CL.EXE
d1b25d84da1d3d0d460996ea298488cb183b857e1b90628147983114ac4ab55f  BIN/C1.DLL
28c355499c2f8090910624734faff31fe719a69a8aac7dd44b2ce64addf96ee2  BIN/C1XX.DLL
a0cc45f83fd0ed009aa4f43df5598105a1049ad9675de0b1089be7cce7d3c153  BIN/C2.DLL
9672e578fdfaa43bdb8e9c16071682988665cb90bba2904bf02f6a5576d8ffbc  BIN/LINK.EXE
35aa049482a40f3ce04028200151cd6f1d417dc50de6bc7951e310654f167df9  BIN/LIB.EXE
83b602ed8e69e979fc9557f482a4a4c6c9a97b4ad67b879aedeacd2b09e5b20b  BIN/CVTRES.EXE
7c90b80d42914562332e28c253415e72a80c6651003d08a9ffdc4ba6e27fc37c  BIN/MSPDB60.DLL
965a9c55b8c0ebfa18bead612e63924f55c3ba992047dc7f256e6ca9ec0af1b9  BIN/MSDIS110.DLL
aba45ffeec3e1c8b4ec5ce9df5cb8c9def6cbda8066be78123f438b6f768c389  BIN/MSOBJ10.DLL
582d0e68739b1128199d0ffc12eb62f48a17a3216a791fd6be948ff9e2eb2ffa  BIN/RC.EXE
5932342fc326b056988cebc710b414592b81c9ba18b9ae247b838a6021f2e434  BIN/RCDLL.DLL
EOF

if [ ! -f "$TC/wineprefix/system.reg" ]; then
    WINEPREFIX="$TC/wineprefix" WINEARCH="${WINEARCH:-win64}" WINEDEBUG=-all wineboot -i >/dev/null 2>&1
    WINEPREFIX="$TC/wineprefix" wineserver -w
fi

# Gate: a compiled object must carry the stamp the Rich header records for the game's
# C++ modules (product 11, build 8447).
probe="$ROOT/build/probe"
mkdir -p "$probe"
printf 'struct S { virtual int f(); int x; };\nint S::f() { return x * 3; }\n' > "$probe/probe.cpp"
rm -f "$probe/probe.obj"
. "$ROOT/tools/wineenv.sh"
"$ROOT/tools/wcl" /c /O2 "/Fo$(winpath "$probe/probe.obj")" "$(winpath "$probe/probe.cpp")" ||
    { echo "the compiler failed under Wine" >&2; exit 1; }
stamp="$(python3 -I "$ROOT/tools/compid.py" "$probe/probe.obj")"
[ "$stamp" = "11 8447" ] || { echo "compiler stamp is '$stamp', want '11 8447'" >&2; exit 1; }

echo "Toolchain ready: VC6 SP3, objects stamped 12.00.8447."
