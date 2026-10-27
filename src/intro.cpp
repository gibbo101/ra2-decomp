// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>
#include "conquer.h"
#include "globals.h"
#include "special.h"
#include "theme.h"

// SYNTHETIC: GAMEMD 0x00539110 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00539140 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00539160 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00539180 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005391A0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005391C0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005391E0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00539220 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00539250 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00539280 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005392B0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005392C0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005392E0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00539300 SYMBOL
// _$E41


void Play_Movie(VQType vq, ThemeType theme, bool clrscrn, bool stretch, bool fullscreen);

// FUNCTION: GAMEMD 0x00539320
void Choose_Side(void)
{
	if (Special.IsFromInstall) {
		Play_Movie(VQ_From_Name("INTRO"), THEME_NONE, false, true, true);
	}
}
