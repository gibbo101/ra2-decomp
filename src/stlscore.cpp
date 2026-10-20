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
extern const double DAT_007F3F48;
extern const double DAT_007F3F50;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006C8C80 $E2
double DAT_00B0BEF0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006C8CB0 $E5
double DAT_00B0BF58 = DAT_00B0BEF0 / DAT_007F3F48;

// FUNCTION: 0x006C8CD0 $E8
double DAT_00B0BF40 = DAT_007E1718 / DAT_00B0BF58;

// FUNCTION: 0x006C8CF0 $E11
double DAT_00B0BF20 = DAT_007E1720 * DAT_007F3F50;

// FUNCTION: 0x006C8D10 $E14
double DAT_00B0BEF8 = DAT_007E1728 * DAT_007F3F50;

// FUNCTION: 0x006C8D30 $E17
double DAT_00B0BED0 = DAT_007E1730 * DAT_007F3F50;

// FUNCTION: 0x006C8D50 $E20
int DAT_00B0BF60 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0BED0 - DAT_00B0BEF8) * DAT_00B0BEF0));

// FUNCTION: 0x006C8D90 $E23
double DAT_00B0BF00 = FUN_004CADE0(DAT_00B0BF60 * DAT_007E1740);

// FUNCTION: 0x006C8DC0 $E26
double DAT_00B0BF18 = FUN_004CADE0((DAT_00B0BF60 * 2) / DAT_00B0BEF0);

// FUNCTION: 0x006C8DF0 $E29
int DAT_00B0BF54 = (int)(DAT_00B0BF60 * 4 + DAT_007E1738);

// FUNCTION: 0x006C8E20 $E32
ShortPair DAT_00B0BEE8;

// FUNCTION: 0x006C8E30 $E35
IntTriple DAT_00B0BF08;
