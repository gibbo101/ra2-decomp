// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "tab.h"

#include <cmath>

// FUNCTION: 0x006CF9B0 $E2
// FUNCTION: 0x006CF9E0 $E5
// FUNCTION: 0x006CFA00 $E8
// FUNCTION: 0x006CFA20 $E11
// FUNCTION: 0x006CFA40 $E14
// FUNCTION: 0x006CFA60 $E17
// FUNCTION: 0x006CFA80 $E20
// FUNCTION: 0x006CFAC0 $E23
// FUNCTION: 0x006CFAF0 $E26
// FUNCTION: 0x006CFB20 $E29
// FUNCTION: 0x006CFB50 $E32
// FUNCTION: 0x006CFB60 $E35

// FUNCTION: 0x006D0F00
void TabClass::Redraw_Tab(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
}
