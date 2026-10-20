// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "logic.h"
#include "_bench.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "_vanim.h"
#include "aircraft.h"
#include "alphashp.h"
#include "anim.h"
#include "bench.h"
#include "blight.h"
#include "building.h"
#include "bullet.h"
#include "empulse.h"
#include "factory.h"
#include "globals.h"
#include "incdec.h"
#include "infantry.h"
#include "ion.h"
#include "ionblast.h"
#include "laser.h"
#include "light.h"
#include "mono.h"
#include "object.h"
#include "ovrlight.h"
#include "partsys.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "tactical.h"
#include "tag.h"
#include "terrain.h"
#include "tiberium.h"
#include "unit.h"
#include "vanim.h"
#include "vein.h"
#include "wave.h"
#include "bench.hh"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007ED368;
extern const double DAT_007ED370;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0055AD50 $E122
double DAT_00ABCCE0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0055AD80 $E125
double DAT_00ABCD30 = DAT_00ABCCE0 / DAT_007ED368;

// FUNCTION: 0x0055ADA0 $E128
double DAT_00ABCD18 = DAT_007E1718 / DAT_00ABCD30;

// FUNCTION: 0x0055ADC0 $E131
double DAT_00ABCD10 = DAT_007E1720 * DAT_007ED370;

// FUNCTION: 0x0055ADE0 $E134
double DAT_00ABCCE8 = DAT_007E1728 * DAT_007ED370;

// FUNCTION: 0x0055AE00 $E137
double DAT_00ABCCC0 = DAT_007E1730 * DAT_007ED370;

// FUNCTION: 0x0055AE20 $E140
int DAT_00ABCD38 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCCC0 - DAT_00ABCCE8) * DAT_00ABCCE0));

// FUNCTION: 0x0055AE60 $E143
double DAT_00ABCCF0 = FUN_004CADE0(DAT_00ABCD38 * DAT_007E1740);

// FUNCTION: 0x0055AE90 $E146
double DAT_00ABCD08 = FUN_004CADE0((DAT_00ABCD38 * 2) / DAT_00ABCCE0);

// FUNCTION: 0x0055AEC0 $E149
int DAT_00ABCD2C = (int)(DAT_00ABCD38 * 4 + DAT_007E1738);

// FUNCTION: 0x0055AEF0 $E152
ShortPair DAT_00ABCCD8;

// FUNCTION: 0x0055AF00 $E155
IntTriple DAT_00ABCCF8;

// FUNCTION: 0x0055B880
void LogicClass::Detach(AbstractClass const * target, bool all)
{
	if (Is_Target_Tag(target)) {
		for (int index = 0; index < LogicTags.Count(); index++) {
			if (target == (AbstractClass *)LogicTags[index]) {
				LogicTags.Delete_Index(index);
				index--;
			}
		}
	}
}
