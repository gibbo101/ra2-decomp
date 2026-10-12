// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "playerprofile.h"

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
