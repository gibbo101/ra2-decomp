// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "keyboard.h"
#include "_xmouse.h"
#include "msgloop.h"
#include <cmath>
#define	ARRAY_SIZE(x)		int(sizeof(x)/sizeof(x[0]))

// FUNCTION: 0x0054EFC0
bool WWKeyboardClass::Is_Mouse_Key(unsigned short key)
{
	key &= 0xFF;
	return(key == VK_LBUTTON || key == VK_MBUTTON || key == VK_RBUTTON);
}
