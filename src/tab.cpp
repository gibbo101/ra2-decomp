// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "tab.h"

// FUNCTION: 0x006D0F00
void TabClass::Redraw_Tab(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
}
