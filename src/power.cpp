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
extern const double DAT_007EFF40;
extern const double DAT_007EFF48;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0063F4C0 $E2
double DAT_00AC4D18 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0063F4F0 $E5
double DAT_00AC4E68 = DAT_00AC4D18 / DAT_007EFF40;

// FUNCTION: 0x0063F510 $E8
double DAT_00AC4E50 = DAT_007E1718 / DAT_00AC4E68;

// FUNCTION: 0x0063F530 $E11
double DAT_00AC4E48 = DAT_007E1720 * DAT_007EFF48;

// FUNCTION: 0x0063F550 $E14
double DAT_00AC4D20 = DAT_007E1728 * DAT_007EFF48;

// FUNCTION: 0x0063F570 $E17
double DAT_00AC4CF8 = DAT_007E1730 * DAT_007EFF48;

// FUNCTION: 0x0063F590 $E20
int DAT_00AC4E70 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00AC4CF8 - DAT_00AC4D20) * DAT_00AC4D18));

// FUNCTION: 0x0063F5D0 $E23
double DAT_00AC4D28 = FUN_004CADE0(DAT_00AC4E70 * DAT_007E1740);

// FUNCTION: 0x0063F600 $E26
double DAT_00AC4E40 = FUN_004CADE0((DAT_00AC4E70 * 2) / DAT_00AC4D18);

// FUNCTION: 0x0063F630 $E29
int DAT_00AC4E64 = (int)(DAT_00AC4E70 * 4 + DAT_007E1738);

// FUNCTION: 0x0063F660 $E32
ShortPair DAT_00AC4D10;

// FUNCTION: 0x0063F670 $E35
IntTriple DAT_00AC4E30;
