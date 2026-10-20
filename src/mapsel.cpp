// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mapsel.h"

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
extern const double DAT_007EDB38;
extern const double DAT_007EDB40;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005ADB20 $E2
double DAT_00ABED78 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005ADB50 $E5
double DAT_00ABEDC8 = DAT_00ABED78 / DAT_007EDB38;

// FUNCTION: 0x005ADB70 $E8
double DAT_00ABEDB0 = DAT_007E1718 / DAT_00ABEDC8;

// FUNCTION: 0x005ADB90 $E11
double DAT_00ABEDA8 = DAT_007E1720 * DAT_007EDB40;

// FUNCTION: 0x005ADBB0 $E14
double DAT_00ABED80 = DAT_007E1728 * DAT_007EDB40;

// FUNCTION: 0x005ADBD0 $E17
double DAT_00ABED58 = DAT_007E1730 * DAT_007EDB40;

// FUNCTION: 0x005ADBF0 $E20
int DAT_00ABEDD0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABED58 - DAT_00ABED80) * DAT_00ABED78));

// FUNCTION: 0x005ADC30 $E23
double DAT_00ABED88 = FUN_004CADE0(DAT_00ABEDD0 * DAT_007E1740);

// FUNCTION: 0x005ADC60 $E26
double DAT_00ABEDA0 = FUN_004CADE0((DAT_00ABEDD0 * 2) / DAT_00ABED78);

// FUNCTION: 0x005ADC90 $E29
int DAT_00ABEDC4 = (int)(DAT_00ABEDD0 * 4 + DAT_007E1738);

// FUNCTION: 0x005ADCC0 $E32
ShortPair DAT_00ABED70;

// FUNCTION: 0x005ADCD0 $E35
IntTriple DAT_00ABED90;
