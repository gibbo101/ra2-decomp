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
extern const double DAT_007F1170;
extern const double DAT_007F1178;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00693960 $E2
double DAT_00B056E8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00693990 $E5
double DAT_00B05738 = DAT_00B056E8 / DAT_007F1170;

// FUNCTION: 0x006939B0 $E8
double DAT_00B05720 = DAT_007E1718 / DAT_00B05738;

// FUNCTION: 0x006939D0 $E11
double DAT_00B05718 = DAT_007E1720 * DAT_007F1178;

// FUNCTION: 0x006939F0 $E14
double DAT_00B056F0 = DAT_007E1728 * DAT_007F1178;

// FUNCTION: 0x00693A10 $E17
double DAT_00B056C8 = DAT_007E1730 * DAT_007F1178;

// FUNCTION: 0x00693A30 $E20
int DAT_00B05740 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B056C8 - DAT_00B056F0) * DAT_00B056E8));

// FUNCTION: 0x00693A70 $E23
double DAT_00B056F8 = FUN_004CADE0(DAT_00B05740 * DAT_007E1740);

// FUNCTION: 0x00693AA0 $E26
double DAT_00B05710 = FUN_004CADE0((DAT_00B05740 * 2) / DAT_00B056E8);

// FUNCTION: 0x00693AD0 $E29
int DAT_00B05734 = (int)(DAT_00B05740 * 4 + DAT_007E1738);

// FUNCTION: 0x00693B00 $E32
ShortPair DAT_00B056E0;

// FUNCTION: 0x00693B10 $E35
IntTriple DAT_00B05700;
