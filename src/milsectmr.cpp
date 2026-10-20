// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "milsectmr.h"

#include "dbgprint.h"
#include "getcpu.h"
#include "mpu.h"
#include "win.h"
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
extern const double DAT_007EDC60;
extern const double DAT_007EDC68;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005B1BA0 $E2
double DAT_00ABEE78 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005B1BD0 $E5
double DAT_00ABEEC8 = DAT_00ABEE78 / DAT_007EDC60;

// FUNCTION: 0x005B1BF0 $E8
double DAT_00ABEEB0 = DAT_007E1718 / DAT_00ABEEC8;

// FUNCTION: 0x005B1C10 $E11
double DAT_00ABEEA8 = DAT_007E1720 * DAT_007EDC68;

// FUNCTION: 0x005B1C30 $E14
double DAT_00ABEE80 = DAT_007E1728 * DAT_007EDC68;

// FUNCTION: 0x005B1C50 $E17
double DAT_00ABEE58 = DAT_007E1730 * DAT_007EDC68;

// FUNCTION: 0x005B1C70 $E20
int DAT_00ABEED0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABEE58 - DAT_00ABEE80) * DAT_00ABEE78));

// FUNCTION: 0x005B1CB0 $E23
double DAT_00ABEE88 = FUN_004CADE0(DAT_00ABEED0 * DAT_007E1740);

// FUNCTION: 0x005B1CE0 $E26
double DAT_00ABEEA0 = FUN_004CADE0((DAT_00ABEED0 * 2) / DAT_00ABEE78);

// FUNCTION: 0x005B1D10 $E29
int DAT_00ABEEC4 = (int)(DAT_00ABEED0 * 4 + DAT_007E1738);

// FUNCTION: 0x005B1D40 $E32
ShortPair DAT_00ABEE70;

// FUNCTION: 0x005B1D50 $E35
IntTriple DAT_00ABEE90;
