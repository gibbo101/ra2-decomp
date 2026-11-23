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
| 0x00442230 | BuildingClass::Take_Damage | src/building.cpp | unread `int remaining = contacts.Count();` | FORCING, 80.5% (2617/2573) |
| 0x00486A70 | CellClass::Fog_Cell | src/cell.cpp | bare `building->PositionCell;` | FORCING, 81.3% (360/375) |
| 0x004CD600 | FlyLocomotionClass::Movement_AI | src/fly.cpp | discarded `Map[newcoord];` | FORCING, 80.1% (3739/3754) |
| 0x00511850 | HouseTypeClass::Read_INI | src/houstype.cpp | `char & p = Prefix; p = prefix[0];` alias for one store | FORCING, 99.7% (2325/2325) |
| 0x00568E40 | MapClass::Repair_High_Bridge_Span | src/map.cpp | seven unread `n0..n6 = cells.Count()` | FORCING, 64.6% (2377/2322) |
| 0x00569760 | MapClass::Repair_Train_Bridge_Span | src/map.cpp | seven unread `n0..n6 = cells.Count()` | FORCING, 64.6% (2383/2328) |
| 0x0056A080 | MapClass::Repair_All_High_Bridges | src/map.cpp | unread `bool valid = Is_Valid(cell);` | FORCING, 73.1% (1091/1081) |
| 0x0056C210 | MapClass::Close_Object | src/map.cpp | unread `Cell unused = cell + _offsets[0];` | FORCING, 42.8% (756/744) |
| 0x00585930 | MapClass::Mark_Fill_Area | src/map.cpp | unread `legal`, `legal2 = In_Radar(...)` | FORCING, 70.2% (1062/969) |
| 0x005A3AE0 | MapGeneratorClass::Generate_Vegetation | src/mapgen.cpp | unread `CellData & data = Get_Cell_Data(cell);` | FORCING, 91.3% (1953/1951) |
| 0x006CB920 | SuperClass::Discharged | src/super.cpp | unread `int chargeLeft = Control.Value();` | FORCING, 86.2% (915/895) |
| 0x006F64A0 | TechnoClass::Draw_Health_Bar | src/techno.cpp | bare `HeightAGL;` | FORCING, 55.3% (1570/1568) |
| 0x00743270 | UnitClass::Read_INI | src/unit.cpp | dead `double fs` used as a conversion vehicle | FORCING, 99.4% as `(double)strength * ...`, 77.3% as `strength * ...` |
| 0x0077D4C0 | FUN_0077D4C0 | src/wonline.cpp | unread `int count = g_UserList.length();` | SIDE-EFFECT: 11 functions, from 13.6% to 76.1% (FUN_00781480, FUN_007A8DA0, FUN_007A8ED0, FUN_007A8FC0, FUN_007A9110, FUN_007A9200, FUN_007A9550, FUN_007AAFB0, FUN_007AB0A0, FUN_007AB170, FUN_007AB260) |
| 0x00785EA0 | Logout_WOnline | src/wonline.cpp | unread `int budget = g_UserList.length();` | FORCING, 45.7% (620/471) |
| 0x00796960 | Poke_The_Host | src/wonline.cpp | unused `IPXAddressClass address;` | FORCING, 48.8% (432/441) |
| 0x00796B90 | Select_WDT_Location_Proc | src/wonline.cpp | `do SetWindowLong(...); while (0);` | FORCING, 98.1% (580/580) |
| 0x00799080 | FUN_00799080 | src/wonline.cpp | `do SetWindowLong(...); while (0);` | FORCING, 97.7% (3142/3142) |

FORCING 18, SIDE-EFFECT 1.
