// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "phoneed.h"

#include "data.h"
#include "dbgprint.h"
#include "init.h"
#include "language\language.h"
#include "ownrdraw.h"
#include "serialed.h"
#include "trim.h"

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
extern const double DAT_007EFD90;
extern const double DAT_007EFD98;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006309A0 $E30
double DAT_00AC4AD0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006309D0 $E33
double DAT_00AC4B20 = DAT_00AC4AD0 / DAT_007EFD90;

// FUNCTION: 0x006309F0 $E36
double DAT_00AC4B08 = DAT_007E1718 / DAT_00AC4B20;

// FUNCTION: 0x00630A10 $E39
double DAT_00AC4B00 = DAT_007E1720 * DAT_007EFD98;

// FUNCTION: 0x00630A30 $E42
double DAT_00AC4AD8 = DAT_007E1728 * DAT_007EFD98;

// FUNCTION: 0x00630A50 $E45
double DAT_00AC4AB0 = DAT_007E1730 * DAT_007EFD98;

// FUNCTION: 0x00630A70 $E48
int DAT_00AC4B28 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00AC4AB0 - DAT_00AC4AD8) * DAT_00AC4AD0));

// FUNCTION: 0x00630AB0 $E51
double DAT_00AC4AE0 = FUN_004CADE0(DAT_00AC4B28 * DAT_007E1740);

// FUNCTION: 0x00630AE0 $E54
double DAT_00AC4AF8 = FUN_004CADE0((DAT_00AC4B28 * 2) / DAT_00AC4AD0);

// FUNCTION: 0x00630B10 $E57
int DAT_00AC4B1C = (int)(DAT_00AC4B28 * 4 + DAT_007E1738);

// FUNCTION: 0x00630B40 $E60
ShortPair DAT_00AC4AC8;

// FUNCTION: 0x00630B50 $E63
IntTriple DAT_00AC4AE8;
