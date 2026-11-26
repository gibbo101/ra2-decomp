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
| 0x00743270 | UnitClass::Read_INI | src/unit.cpp | dead `double fs` used as a conversion vehicle | FORCING, 99.4% as `(double)strength * ...`, 77.3% as `strength * ...` |

FORCING 3, SIDE-EFFECT 0.
