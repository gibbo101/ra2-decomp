// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E94E0;
extern const double DAT_007E94E8;

// FUNCTION: 0x004E3130 $E2
double DAT_008B3FA8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004E3160 $E5
double DAT_008B40B8 = DAT_008B3FA8 / DAT_007E94E0;

// FUNCTION: 0x004E3180 $E8
double DAT_008B40A0 = DAT_007E1718 / DAT_008B40B8;

// FUNCTION: 0x004E31A0 $E11
double DAT_008B4098 = DAT_007E1720 * DAT_007E94E8;

// FUNCTION: 0x004E31C0 $E14
double DAT_008B3FB0 = DAT_007E1728 * DAT_007E94E8;

// FUNCTION: 0x004E31E0 $E17
double DAT_008B3F28 = DAT_007E1730 * DAT_007E94E8;

// FUNCTION: 0x004E3200 $E20
int DAT_008B40C0 = (int)(fastmath::tan(DAT_008B3F28 - DAT_008B3FB0) * DAT_007E1738 * DAT_008B3FA8);

// FUNCTION: 0x004E3240 $E23
double DAT_008B3FB8 = fastmath::atan(DAT_008B40C0 * DAT_007E1740);

// FUNCTION: 0x004E3270 $E26
double DAT_008B4030 = fastmath::atan((DAT_008B40C0 * 2) / DAT_008B3FA8);

// FUNCTION: 0x004E32A0 $E29
int DAT_008B40B4 = (int)(DAT_008B40C0 * 4 + DAT_007E1738);

struct GDShortPair { short a; short b; GDShortPair(void) { a = 0; b = 0; } };
struct GDIntTriple { int a; int b; int c; GDIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x004E32D0 $E32
GDShortPair DAT_008B3FA0;

// FUNCTION: 0x004E32E0 $E35
GDIntTriple DAT_008B4020;
