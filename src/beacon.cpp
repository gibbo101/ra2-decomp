// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

extern int DAT_0089C470;
extern double DAT_0089C398, DAT_0089C440;
extern double __cdecl FUN_004CADE0(double);

// FUNCTION: 0x004300E0
void Init_004300E0(void)
{
	DAT_0089C440 = FUN_004CADE0((DAT_0089C470 * 2) / DAT_0089C398);
}
