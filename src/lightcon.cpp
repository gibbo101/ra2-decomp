// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lightcon.h"
#include "dsurface.h"
#include "globals.h"
#include "ion.h"
#include "scenario.h"
#include "sun.h"

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
extern const double DAT_007ED090;
extern const double DAT_007ED098;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00555BB0 $E32
double DAT_00ABCAB8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00555BE0 $E35
double DAT_00ABCB08 = DAT_00ABCAB8 / DAT_007ED090;

// FUNCTION: 0x00555C00 $E38
double DAT_00ABCAF0 = DAT_007E1718 / DAT_00ABCB08;

// FUNCTION: 0x00555C20 $E41
double DAT_00ABCAE8 = DAT_007E1720 * DAT_007ED098;

// FUNCTION: 0x00555C40 $E44
double DAT_00ABCAC0 = DAT_007E1728 * DAT_007ED098;

// FUNCTION: 0x00555C60 $E47
double DAT_00ABCA98 = DAT_007E1730 * DAT_007ED098;

// FUNCTION: 0x00555C80 $E50
int DAT_00ABCB10 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCA98 - DAT_00ABCAC0) * DAT_00ABCAB8));

// FUNCTION: 0x00555CC0 $E53
double DAT_00ABCAC8 = FUN_004CADE0(DAT_00ABCB10 * DAT_007E1740);

// FUNCTION: 0x00555CF0 $E56
double DAT_00ABCAE0 = FUN_004CADE0((DAT_00ABCB10 * 2) / DAT_00ABCAB8);

// FUNCTION: 0x00555D20 $E59
int DAT_00ABCB04 = (int)(DAT_00ABCB10 * 4 + DAT_007E1738);

// FUNCTION: 0x00555D50 $E62
ShortPair DAT_00ABCAB0;

// FUNCTION: 0x00555D60 $E65
IntTriple DAT_00ABCAD0;
