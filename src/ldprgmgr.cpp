// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007ECF50;
extern const double DAT_007ECF58;

// FUNCTION: 0x00552850 $E2
double DAT_00ABC960 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x00552880 $E5
double DAT_00ABC9B0 = DAT_00ABC960 / DAT_007ECF50;

// FUNCTION: 0x005528A0 $E8
double DAT_00ABC998 = 1.0 / DAT_00ABC9B0;

// FUNCTION: 0x005528C0 $E11
double DAT_00ABC990 = DAT_007ECF58 * 45.0;

// FUNCTION: 0x005528E0 $E14
double DAT_00ABC968 = DAT_007ECF58 * 60.0;

// FUNCTION: 0x00552900 $E17
double DAT_00ABC940 = DAT_007ECF58 * 90.0;

// FUNCTION: 0x00552920 $E20
int DAT_00ABC9B8 = (int)(FUN_004CAD50(DAT_00ABC940 - DAT_00ABC968) * DAT_00ABC960 * 0.5);

// FUNCTION: 0x00552960 $E23
double DAT_00ABC970 = FUN_004CADE0(DAT_00ABC9B8 * 0.00390625);

// FUNCTION: 0x00552990 $E26
double DAT_00ABC988 = FUN_004CADE0((DAT_00ABC9B8 * 2) / DAT_00ABC960);

// FUNCTION: 0x005529C0 $E29
int DAT_00ABC9AC = (int)(DAT_00ABC9B8 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005529F0 $E32
ShortPair DAT_00ABC958;

// FUNCTION: 0x00552A00 $E35
IntTriple DAT_00ABC978;
