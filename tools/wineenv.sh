# Sourced by tools/wcl: the Wine environment the toolchain runs in. Needs ROOT; sets
# TC, WINEPREFIX, WINEDEBUG, INCLUDE and LIB, and defines winpath.

TC="$ROOT/toolchain/vc6sp3"

# The compiler's output depends on the paths it is given, so it sees the repo as drive
# Y: wherever the checkout lives.
winpath() {
    case "$1" in
        "$ROOT") echo 'Y:\' ;;
        "$ROOT"/*) local rel="${1#"$ROOT"/}"; echo "Y:\\${rel//\//\\}" ;;
        *) echo "Z:${1//\//\\}" ;;
    esac
}

export WINEPREFIX="$ROOT/toolchain/wineprefix"
export WINEDEBUG=-all
[ "$(readlink "$WINEPREFIX/dosdevices/y:" 2>/dev/null)" = "$ROOT" ] || ln -sfn "$ROOT" "$WINEPREFIX/dosdevices/y:"
export INCLUDE="$(winpath "$TC/INCLUDE");$(winpath "$TC/MFC/INCLUDE");$(winpath "$TC/ATL/INCLUDE")"
export LIB="$(winpath "$TC/LIB")"
