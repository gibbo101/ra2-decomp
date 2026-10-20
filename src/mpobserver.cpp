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
extern const double DAT_007EE5E0;
extern const double DAT_007EE5E8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005C91F0 $E2
double DAT_00ABF8F8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005C9220 $E5
double DAT_00ABF948 = DAT_00ABF8F8 / DAT_007EE5E0;

// FUNCTION: 0x005C9240 $E8
double DAT_00ABF930 = DAT_007E1718 / DAT_00ABF948;

// FUNCTION: 0x005C9260 $E11
double DAT_00ABF928 = DAT_007E1720 * DAT_007EE5E8;

// FUNCTION: 0x005C9280 $E14
double DAT_00ABF900 = DAT_007E1728 * DAT_007EE5E8;

// FUNCTION: 0x005C92A0 $E17
double DAT_00ABF8D8 = DAT_007E1730 * DAT_007EE5E8;

// FUNCTION: 0x005C92C0 $E20
int DAT_00ABF950 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF8D8 - DAT_00ABF900) * DAT_00ABF8F8));

// FUNCTION: 0x005C9300 $E23
double DAT_00ABF908 = FUN_004CADE0(DAT_00ABF950 * DAT_007E1740);

// FUNCTION: 0x005C9330 $E26
double DAT_00ABF920 = FUN_004CADE0((DAT_00ABF950 * 2) / DAT_00ABF8F8);

// FUNCTION: 0x005C9360 $E29
int DAT_00ABF944 = (int)(DAT_00ABF950 * 4 + DAT_007E1738);

// FUNCTION: 0x005C9390 $E32
ShortPair DAT_00ABF8F0;

// FUNCTION: 0x005C93A0 $E35
IntTriple DAT_00ABF910;
