// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "toggle.h"
#include "_xmouse.h"

// FUNCTION: 0x00723EA0
void ToggleClass::Turn_On(void)
{
	IsOn = true;
	Flag_To_Redraw();
}

// FUNCTION: 0x00723EB0
void ToggleClass::Turn_Off(void)
{
	IsOn = false;
	Flag_To_Redraw();
}
