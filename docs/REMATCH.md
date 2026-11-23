# Functions to re-match

These functions match only through a construct the rules do not allow: a dead local, a
statement with no effect, a constant test or an empty block that exists to steer the
compiler. Each one needs a natural form that matches before the construct can go, and each
stays matched with the construct until then.

Verdicts:
- **FORCING**: without the construct the function itself no longer matches. The score is
  the checker's result after removal (our bytes / original bytes).
- **SIDE-EFFECT**: without the construct the function still matches, but the functions
  listed stop matching.

| address | function | file | construct | verdict |
|---|---|---|---|---|
| 0x0041B110 | AircraftClass::Read_INI | src/aircraft.cpp | dead `double fs` used as a conversion vehicle | FORCING, 99.2% as `(double)strength * ...`, 57.4% as `strength * ...` |
| 0x00511850 | HouseTypeClass::Read_INI | src/houstype.cpp | `char & p = Prefix; p = prefix[0];` alias for one store | FORCING, 99.7% (2325/2325) |
| 0x0056A080 | MapClass::Repair_All_High_Bridges | src/map.cpp | unread `bool valid = Is_Valid(cell);` | FORCING, 73.1% (1091/1081) |
| 0x00585930 | MapClass::Mark_Fill_Area | src/map.cpp | unread `legal`, `legal2 = In_Radar(...)` | FORCING, 70.2% (1062/969) |
| 0x005A3AE0 | MapGeneratorClass::Generate_Vegetation | src/mapgen.cpp | unread `CellData & data = Get_Cell_Data(cell);` | FORCING, 91.3% (1953/1951) |
| 0x00626DA0 | FUN_00626DA0 | src/palette.cpp | unread `SchemeLibrary * library = Library();` (the RA2 1.001 reconstruction has the same local) | FORCING, 91.6% (590/592) as `Library();` |
| 0x00743270 | UnitClass::Read_INI | src/unit.cpp | dead `double fs` used as a conversion vehicle | FORCING, 99.4% as `(double)strength * ...`, 77.3% as `strength * ...` |

FORCING 7, SIDE-EFFECT 0.
