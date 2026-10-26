// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include <math.h>
#include "conquer.h"
#include "globals.h"
#include "special.h"
#include "theme.h"


void Play_Movie(VQType vq, ThemeType theme, bool clrscrn, bool stretch, bool fullscreen);

// FUNCTION: GAMEMD 0x00539320
void Choose_Side(void)
{
	if (Special.IsFromInstall) {
		Play_Movie(VQ_From_Name("INTRO"), THEME_NONE, false, true, true);
	}
}

// SYNTHETIC: GAMEMD 0x00539350 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x00539380 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x005393A0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x005393C0 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x005393E0 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x00539400 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x00539420 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x00539460 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x00539490 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x005394C0 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x005394F0 SYMBOL
// _$E32
