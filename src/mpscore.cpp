// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include <math.h>
#include <windows.h>
#undef LoadString

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EE6D8;
extern const double DAT_007EE6E0;

// FUNCTION: 0x005C9510 $E2
double DAT_00ABF998 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005C9540 $E5
double DAT_00ABF9E8 = DAT_00ABF998 / DAT_007EE6D8;

// FUNCTION: 0x005C9560 $E8
double DAT_00ABF9D0 = DAT_007E1718 / DAT_00ABF9E8;

// FUNCTION: 0x005C9580 $E11
double DAT_00ABF9C8 = DAT_007E1720 * DAT_007EE6E0;

// FUNCTION: 0x005C95A0 $E14
double DAT_00ABF9A0 = DAT_007E1728 * DAT_007EE6E0;

// FUNCTION: 0x005C95C0 $E17
double DAT_00ABF978 = DAT_007E1730 * DAT_007EE6E0;

// FUNCTION: 0x005C95E0 $E20
int DAT_00ABF9F0 = (int)(fastmath::tan(DAT_00ABF978 - DAT_00ABF9A0) * DAT_007E1738 * DAT_00ABF998);

// FUNCTION: 0x005C9620 $E23
double DAT_00ABF9A8 = fastmath::atan(DAT_00ABF9F0 * DAT_007E1740);

// FUNCTION: 0x005C9650 $E26
double DAT_00ABF9C0 = fastmath::atan((DAT_00ABF9F0 * 2) / DAT_00ABF998);

// FUNCTION: 0x005C9680 $E29
int DAT_00ABF9E4 = (int)(DAT_00ABF9F0 * 4 + DAT_007E1738);

struct MPShortPair { short a; short b; MPShortPair(void) { a = 0; b = 0; } };
struct MPIntTriple { int a; int b; int c; MPIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x005C96B0 $E32
MPShortPair DAT_00ABF990;

// FUNCTION: 0x005C96C0 $E35
MPIntTriple DAT_00ABF9B0;
