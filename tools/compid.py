"""Print the "<product> <build>" pair in a COFF object's @comp.id symbol.

The pair is what the linker copies into the exe's Rich header, so it identifies the
compiler that emitted the object. Prints "none" when the object has no stamp.
"""

import struct
import sys


def comp_id(data: bytes) -> tuple[int, int] | None:
    symtab, count = struct.unpack_from("<II", data, 8)
    i = 0
    while i < count:
        entry = data[symtab + i * 18 : symtab + i * 18 + 18]
        if entry[:8] == b"@comp.id":
            value = struct.unpack_from("<I", entry, 8)[0]
            return value >> 16, value & 0xFFFF
        i += 1 + entry[17]
    return None


def main() -> None:
    with open(sys.argv[1], "rb") as f:
        stamp = comp_id(f.read())
    print("none" if stamp is None else f"{stamp[0]} {stamp[1]}")


if __name__ == "__main__":
    main()
