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
extern const double DAT_007F7280;
extern const double DAT_007F7288;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0076BCA0 $E2
double DAT_00B729B0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0076BCD0 $E5
double DAT_00B72A00 = DAT_00B729B0 / DAT_007F7280;

// FUNCTION: 0x0076BCF0 $E8
double DAT_00B729E8 = DAT_007E1718 / DAT_00B72A00;

// FUNCTION: 0x0076BD10 $E11
double DAT_00B729E0 = DAT_007E1720 * DAT_007F7288;

// FUNCTION: 0x0076BD30 $E14
double DAT_00B729B8 = DAT_007E1728 * DAT_007F7288;

// FUNCTION: 0x0076BD50 $E17
double DAT_00B72990 = DAT_007E1730 * DAT_007F7288;

// FUNCTION: 0x0076BD70 $E20
int DAT_00B72A08 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72990 - DAT_00B729B8) * DAT_00B729B0));

// FUNCTION: 0x0076BDB0 $E23
double DAT_00B729C0 = FUN_004CADE0(DAT_00B72A08 * DAT_007E1740);

// FUNCTION: 0x0076BDE0 $E26
double DAT_00B729D8 = FUN_004CADE0((DAT_00B72A08 * 2) / DAT_00B729B0);

// FUNCTION: 0x0076BE10 $E29
int DAT_00B729FC = (int)(DAT_00B72A08 * 4 + DAT_007E1738);

// FUNCTION: 0x0076BE40 $E32
ShortPair DAT_00B729A8;

// FUNCTION: 0x0076BE50 $E35
IntTriple DAT_00B729C8;
