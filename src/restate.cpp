// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
extern const double DAT_007F0970;
extern const double DAT_007F0978;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0065F330 $E2
double DAT_00B04CC8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0065F360 $E5
double DAT_00B04D18 = DAT_00B04CC8 / DAT_007F0970;

// FUNCTION: 0x0065F380 $E8
double DAT_00B04D00 = DAT_007E1718 / DAT_00B04D18;

// FUNCTION: 0x0065F3A0 $E11
double DAT_00B04CF8 = DAT_007E1720 * DAT_007F0978;

// FUNCTION: 0x0065F3C0 $E14
double DAT_00B04CD0 = DAT_007E1728 * DAT_007F0978;

// FUNCTION: 0x0065F3E0 $E17
double DAT_00B04CA8 = DAT_007E1730 * DAT_007F0978;

// FUNCTION: 0x0065F400 $E20
int DAT_00B04D20 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B04CA8 - DAT_00B04CD0) * DAT_00B04CC8));

// FUNCTION: 0x0065F440 $E23
double DAT_00B04CD8 = FUN_004CADE0(DAT_00B04D20 * DAT_007E1740);

// FUNCTION: 0x0065F470 $E26
double DAT_00B04CF0 = FUN_004CADE0((DAT_00B04D20 * 2) / DAT_00B04CC8);

// FUNCTION: 0x0065F4A0 $E29
int DAT_00B04D14 = (int)(DAT_00B04D20 * 4 + DAT_007E1738);

// FUNCTION: 0x0065F4D0 $E32
ShortPair DAT_00B04CC0;

// FUNCTION: 0x0065F4E0 $E35
IntTriple DAT_00B04CE0;
