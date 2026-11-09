"""Identify the player's gamemd.exe and put back the retail 1.001 bytes its build changed.

The target is retail Yuri's Revenge 1.001. The First Decade's and Steam's exes share its
link and differ only at the sites in data/exe_patches.csv, so any of the three will do.
"""
import csv
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

BUILDS = {
    "b29c47a94d6d414fad3b53ab0f9ebcd0ce22f0a98ca2be6f0c6920c4dd7e2fe2": "retail",
    "258aa247cdf8ab0946540f015e40d5fe07d812ce870fd54e56cb93c28a68a4e6": "tfd",
    "3e81a61775d2745d1dabe397325ef663cd994ffc194da4e998e3bf5d2d308600": "steam",
}
ACCEPTED = """orig/gamemd.exe must be one of these Yuri's Revenge 1.001 builds:
  retail CD patched to 1.001   4,813,072 bytes  MD5 937b14db32f07ee47e92a8114ebdbdde
  The First Decade             4,813,072 bytes  MD5 6604fd6f4ab71a11c708ea423dc0a29e
  Steam                        5,286,208 bytes  MD5 2a1307f6f57c9c492f985b4e3afb88e6"""


def build_of(data: bytes) -> str:
    """The build an exe's bytes come from; exits naming the accepted builds if unknown."""
    build = BUILDS.get(hashlib.sha256(data).hexdigest())
    if build is None:
        raise SystemExit(ACCEPTED)
    return build


def sites(build: str) -> list[tuple[int, bytes, bytes]]:
    """(address, retail bytes, build's bytes) for every site the build changed."""
    with (ROOT / "data/exe_patches.csv").open() as f:
        return [(int(r["address"], 16), bytes.fromhex(r["original"]), bytes.fromhex(r["patched"]))
                for r in csv.DictReader(f) if r["build"] == build]


def restore(image: bytearray, start: int, build: str) -> None:
    """Rewrite the build's sites inside image, which holds the bytes from address start."""
    for at, original, patched in sites(build):
        off = at - start
        if 0 <= off < len(image):
            if image[off:off + len(patched)] != patched:
                raise SystemExit(f"exe_patches.csv: bytes at 0x{at:08X} are not the {build} build's")
            image[off:off + len(original)] = original
