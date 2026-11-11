# Bugs in the original game

The source reproduces the game as shipped, bugs included, so these stay until the
decompilation is finished. Each entry gives the address, what the original does, and its
effect, so it can be fixed in a separate build later.

- **HouseClass::AI_Building (0x004FE3E0).** When the AI adds a power plant ahead of a
  building that needs more power, a house whose ActLike is neither 0 nor 2 fills the
  owned list for its prerequisite check with BuildingClass instances instead of their
  types. An instance never equals a type, so the check fails whenever NodAdvancedPower
  has a prerequisite, and the AI always picks NodRegularPower.

- **HouseClass::AI_Raise_Money (0x004FDD10).** The weapons-factory branch removes a
  later base node only when its type equals both the first refinery type and the
  selected weapons-factory type. The comparisons at 0x004FE25A and 0x004FE262 prevent
  that duplicate-node cleanup when the two building types differ.

- **Mission score hall of fame (0x0068CD20).** When looking for a place to insert the
  mission score, the comparison indexes the nine-entry table with the preceding wipe
  animation's frame counter. That counter is 10, so the game reads past the table
  instead of comparing with the current entry.

- **Wonline guest-game dialog (0x00791B10).** The page and reply command paths
  allocate their temporary text buffers with array new and release them with scalar
  delete, preserving the original allocation/deallocation mismatch.

- **MapClass::Build_Reachable_Subzones (0x005840C0).** The final grid scan tests each
  visited flag against the subzone of the same cell. The cell offset comes from the
  starting coordinates and never advances with the row or column.
- **Dropship_Screen (0x004B6C30).** The dropship and four green-light shape files
  are loaded into raw unsigned-byte arrays, but cleanup deletes them through
  `SHPReference *` array pointers. The original reads an array cookie at pointer
  minus four (0x004B93C8 and 0x004B9409), destroys 36-byte elements and frees the
  cookie base, although the byte-array allocations created no such cookie. This
  can read an invalid count and destroy or free unrelated memory; the recovered
  source preserves the original allocation/deallocation mismatch.

- **Wonline user leave (0x00785740).** The ping refresh marks an empty player
  slot with `-1` but still passes its null name through `FUN_007350C0` to
  `FUN_007A0EA0` (0x00785DD6–0x00785DE9). The converter returns null, and the
  ping updater's inlined `strcmp` reads that pointer at 0x007A0ECF when it reaches
  a populated slot. A leave event with both empty and populated slots can
  therefore access-violate; the matched source preserves this path.

- **UnitClass::Scatter (0x00743A50).** The height expression evaluates
  `(cell height + moving onto a bridge) ? bridge height : 0`, so a nonzero
  ground height selects the bridge height instead of adding it conditionally.
  This affects the coordinate used to choose a scatter destination and is also
  present in the archived TibSun implementation.
