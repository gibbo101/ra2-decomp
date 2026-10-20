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
extern const double DAT_007F7078;
extern const double DAT_007F7080;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00767110 $E2
double DAT_00B727B0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x00767140 $E5
double DAT_00B72800 = DAT_00B727B0 / DAT_007F7078;

// FUNCTION: 0x00767160 $E8
double DAT_00B727E8 = DAT_007E1718 / DAT_00B72800;

// FUNCTION: 0x00767180 $E11
double DAT_00B727E0 = DAT_007E1720 * DAT_007F7080;

// FUNCTION: 0x007671A0 $E14
double DAT_00B727B8 = DAT_007E1728 * DAT_007F7080;

// FUNCTION: 0x007671C0 $E17
double DAT_00B72790 = DAT_007E1730 * DAT_007F7080;

// FUNCTION: 0x007671E0 $E20
int DAT_00B72808 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B72790 - DAT_00B727B8) * DAT_00B727B0));

// FUNCTION: 0x00767220 $E23
double DAT_00B727C0 = FUN_004CADE0(DAT_00B72808 * DAT_007E1740);

// FUNCTION: 0x00767250 $E26
double DAT_00B727D8 = FUN_004CADE0((DAT_00B72808 * 2) / DAT_00B727B0);

// FUNCTION: 0x00767280 $E29
int DAT_00B727FC = (int)(DAT_00B72808 * 4 + DAT_007E1738);

// FUNCTION: 0x007672B0 $E32
ShortPair DAT_00B727A8;

// FUNCTION: 0x007672C0 $E35
IntTriple DAT_00B727C8;
