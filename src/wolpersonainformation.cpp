// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "playerprofile.h"

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F74E0;
extern const double DAT_007F74E8;

// FUNCTION: 0x007784F0 $E2
double DAT_00B73598 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00778520 $E5
double DAT_00B735E8 = DAT_00B73598 / DAT_007F74E0;

// FUNCTION: 0x00778540 $E8
double DAT_00B735D0 = DAT_007E1718 / DAT_00B735E8;

// FUNCTION: 0x00778560 $E11
double DAT_00B735C8 = DAT_007E1720 * DAT_007F74E8;

// FUNCTION: 0x00778580 $E14
double DAT_00B735A0 = DAT_007E1728 * DAT_007F74E8;

// FUNCTION: 0x007785A0 $E17
double DAT_00B73578 = DAT_007E1730 * DAT_007F74E8;

// FUNCTION: 0x007785C0 $E20
int DAT_00B735F0 = (int)(fastmath::tan(DAT_00B73578 - DAT_00B735A0) * DAT_007E1738 * DAT_00B73598);

// FUNCTION: 0x00778600 $E23
double DAT_00B735A8 = fastmath::atan(DAT_00B735F0 * DAT_007E1740);

// FUNCTION: 0x00778630 $E26
double DAT_00B735C0 = fastmath::atan((DAT_00B735F0 * 2) / DAT_00B73598);

// FUNCTION: 0x00778660 $E29
int DAT_00B735E4 = (int)(DAT_00B735F0 * 4 + DAT_007E1738);

struct WOLShortPair { short a; short b; WOLShortPair(void) { a = 0; b = 0; } };
struct WOLIntTriple { int a; int b; int c; WOLIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x00778690 $E32
WOLShortPair DAT_00B73594;

// FUNCTION: 0x007786A0 $E35
WOLIntTriple DAT_00B735B0;

// FUNCTION: 0x00779690
PlayerProfile::PlayerProfile(Wstring const & name, int a, char b, int c, int d, int e, int f, int g, int h, int i, int j, char k, WideWstring const & l, WideWstring const & m) :
	Name(name),
	Field10(a),
	Field14(b),
	Field18(c),
	Field1C(d),
	Field20(e),
	Field24(f),
	Field28(g),
	Field2C(h),
	Field30(i),
	Field34(j),
	Field38(k),
	Wide3C(m),
	Wide40(l)
{
}
