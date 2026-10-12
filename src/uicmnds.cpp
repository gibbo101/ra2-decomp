// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

extern double DAT_007E1720, DAT_007F5C10, DAT_00B0FE28;

// FUNCTION: 0x007307F0
void Init_007307F0(void)
{
	DAT_00B0FE28 = DAT_007E1720 * DAT_007F5C10;
}
