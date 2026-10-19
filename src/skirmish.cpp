// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "skirmish.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007F3138;
extern const double DAT_007F3140;

// FUNCTION: 0x006AC970 $E2
double DAT_00B0B540 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x006AC9A0 $E5
double DAT_00B0B590 = DAT_00B0B540 / DAT_007F3138;

// FUNCTION: 0x006AC9C0 $E8
double DAT_00B0B578 = 1.0 / DAT_00B0B590;

// FUNCTION: 0x006AC9E0 $E11
double DAT_00B0B570 = DAT_007F3140 * 45.0;

// FUNCTION: 0x006ACA00 $E14
double DAT_00B0B548 = DAT_007F3140 * 60.0;

// FUNCTION: 0x006ACA20 $E17
double DAT_00B0B520 = DAT_007F3140 * 90.0;

// FUNCTION: 0x006ACA40 $E20
int DAT_00B0B598 = (int)(FUN_004CAD50(DAT_00B0B520 - DAT_00B0B548) * DAT_00B0B540 * 0.5);

// FUNCTION: 0x006ACA80 $E23
double DAT_00B0B550 = FUN_004CADE0(DAT_00B0B598 * 0.00390625);

// FUNCTION: 0x006ACAB0 $E26
double DAT_00B0B568 = FUN_004CADE0((DAT_00B0B598 * 2) / DAT_00B0B540);

// FUNCTION: 0x006ACAE0 $E29
int DAT_00B0B58C = (int)(DAT_00B0B598 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006ACB10 $E32
ShortPair DAT_00B0B538;

// FUNCTION: 0x006ACB20 $E35
IntTriple DAT_00B0B558;
