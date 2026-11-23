# Bugs in the original game

The source reproduces the game as shipped, bugs included, so these stay until the
decompilation is finished. Each entry gives the address, what the original does, and its
effect, so it can be fixed in a separate build later.

- **BulletClass::Unlimbo (0x00468670).** An invisible projectile with no
  Firestorm intercept checks for a trajectory obstacle. If it finds one, it
  sets its position to the obstacle’s center at 0x00468966. Both paths then
  set the position to the intended target at 0x00468986, overwriting the
  obstacle position before the fuse is armed.

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

- **TechnoClass::Draw_Pips (0x00709A90).** The passenger-pip color buffer is
  allocated with array new and released with scalar delete on both cleanup
  paths. The recovered source preserves this allocation/deallocation mismatch.

- **AnimClass::Draw_It (0x00422CA0).** The airstrike/laser-target and force-shield
  tint switches fall through from color mode 2 into mode 1 and then the default
  encoding. Mode 1 also falls through into the default. The resulting tint ORs
  multiple packed-color encodings together; the recovered source preserves this
  cumulative packing.

- **BitFont::LoadInternalData (0x00433990).** A short read of the range table
  in the extended font format jumps to failure cleanup without freeing the
  allocated range buffer. The recovered source preserves this leak.

- **MapGeneratorClass::FUN_0059BBC0 (0x0059BBC0).** The region-growth routine
  allocates its cell-node array and priority queue before searching for a random
  starting cell. Exhausting the 200-attempt search returns without releasing
  either allocation. The recovered source preserves this leak.

- **Do_Reinforcements (0x0065D8E0).** After `_Create_Group`, the routine writes
  through `object->Team` before checking whether `object` is null. If group
  creation fails, execution can access-violate before reaching the intended
  failure return. The recovered source preserves this ordering.

- **AircraftClass::Fire_At (0x00415EE0).** The shroud checks around the aircraft
  test `(2 * CELL_LEPTON_W, -2 * CELL_LEPTON_H)` twice and omit the opposite
  `(-2 * CELL_LEPTON_W, 2 * CELL_LEPTON_H)` offset. The recovered source
  preserves the repeated check.

- **ScoreFontClass glyph selection (0x006907E0, 0x00690850, 0x00690910).** The lower
  frame bound uses an unsigned comparison after subtracting the first glyph. Characters
  whose OEM byte gives a negative signed offset can therefore select a negative shape frame.

- **Wait_For_Players (0x00648710).** Command-stall statistics use the frame-stall index instead of the command-stall index.

- **WinMain (0x006BB9A0).** The IPX destination-network parser writes the node
  address without checking its length. An invalid configuration can corrupt
  stack memory during startup.

- **Wonline game-options encoding (0x0077E430).** With no players in the session,
  replacing the final player separator writes one byte before the static options buffer:
  the zeroed buffer has length zero, but the index is still `strlen(buffer) - 1`.

- **WorldDominationTour::Selection::Play_SFX (0x0076EBE0).** The lookup loop stops at the
  first sound entry whose name differs from the one asked for, and plays that entry, so a
  named campaign-map sound plays only when it is not the first entry.

- **Read_PCX_File (0x00630310).** Header validation rejects a file only when
  its identifier, version and pixel size are all invalid; failure returns after
  opening the file also omit `Close`. In the padded one-plane path, a literal
  padding byte does not advance the scanline index, so decoding can consume
  subsequent data or stall. In the three-plane path, a zero-length RLE packet
  wraps the byte counter and writes 256 bytes. These original paths are preserved.

- **DisplayClass::Mouse_Left_Up (0x004AAE90).** In the shadow branch,
  `ACTION_ENTER_WAYPOINT` selects the disallowed cursor and then falls through
  to the beacon case, which replaces it with the beacon cursor. The recovered
  source preserves both calls and the fallthrough.

- **BounceClass::AI (0x00439B00).** When speed exceeds a positive MaxVelocity,
  the cap divides speed by itself at 0x00439B87 before scaling Velocity.
  For finite speed the factor is 1, so this cap does not reduce velocity
  to the configured maximum.

- **World Domination Tour selection (0x0076D3A0).** Select_Territory_At tests the hovered
  territory id with `>= 0` before looking it up, but the id is unsigned, so the test is
  always true and the "none" id is looked up too. The lookup finds nothing, so there is no
  visible effect.
