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
extern const double DAT_007EE7D0;
extern const double DAT_007EE7D8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005CAC20 $E2
double DAT_00ABFAA0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005CAC50 $E5
double DAT_00ABFAF0 = DAT_00ABFAA0 / DAT_007EE7D0;

// FUNCTION: 0x005CAC70 $E8
double DAT_00ABFAD8 = DAT_007E1718 / DAT_00ABFAF0;

// FUNCTION: 0x005CAC90 $E11
double DAT_00ABFAD0 = DAT_007E1720 * DAT_007EE7D8;

// FUNCTION: 0x005CACB0 $E14
double DAT_00ABFAA8 = DAT_007E1728 * DAT_007EE7D8;

// FUNCTION: 0x005CACD0 $E17
double DAT_00ABFA80 = DAT_007E1730 * DAT_007EE7D8;

// FUNCTION: 0x005CACF0 $E20
int DAT_00ABFAF8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABFA80 - DAT_00ABFAA8) * DAT_00ABFAA0));

// FUNCTION: 0x005CAD30 $E23
double DAT_00ABFAB0 = FUN_004CADE0(DAT_00ABFAF8 * DAT_007E1740);

// FUNCTION: 0x005CAD60 $E26
double DAT_00ABFAC8 = FUN_004CADE0((DAT_00ABFAF8 * 2) / DAT_00ABFAA0);

// FUNCTION: 0x005CAD90 $E29
int DAT_00ABFAEC = (int)(DAT_00ABFAF8 * 4 + DAT_007E1738);

// FUNCTION: 0x005CADC0 $E32
ShortPair DAT_00ABFA98;

// FUNCTION: 0x005CADD0 $E35
IntTriple DAT_00ABFAB8;
