// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "credits.h"
#include "_bench.h"
#include "_map.h"
#include "_rules.h"
#include "_surface.h"
#include "bench.h"
#include "bound.h"
#include "dialog.h"
#include "dsurface.h"
#include "globals.h"
#include "house.h"
#include "geometry.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "tab.h"
#include "voc.h"
#include "vox.h"
#include "bench.hh"
#include "color.hh"
#include <math.h>


// SYNTHETIC: GAMEMD 0x004A2140 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004A2160 SYMBOL
// _$E73
// SYNTHETIC: GAMEMD 0x004A2190 SYMBOL
// _$E76
// SYNTHETIC: GAMEMD 0x004A21B0 SYMBOL
// _$E79
// SYNTHETIC: GAMEMD 0x004A21D0 SYMBOL
// _$E82
// SYNTHETIC: GAMEMD 0x004A21F0 SYMBOL
// _$E85
// SYNTHETIC: GAMEMD 0x004A2210 SYMBOL
// _$E88
// SYNTHETIC: GAMEMD 0x004A2230 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x004A2270 SYMBOL
// _$E94
// SYNTHETIC: GAMEMD 0x004A22A0 SYMBOL
// _$E97
// SYNTHETIC: GAMEMD 0x004A22D0 SYMBOL
// _$E100
// SYNTHETIC: GAMEMD 0x004A2300 SYMBOL
// _$E103
// SYNTHETIC: GAMEMD 0x004A2310 SYMBOL
// _$E106
// SYNTHETIC: GAMEMD 0x004A2330 SYMBOL
// _$E109

// FUNCTION: GAMEMD 0x004A2350
CreditClass::CreditClass(void) :
	Credits(0),
	Current(0),
	IsToRedraw(false),
	IsUp(false),
	IsAudible(false),
	Countdown(0)
{
}

extern HouseClass * DAT_00AC1198;

// FUNCTION: GAMEMD 0x004A2600
void CreditClass::AI(bool forced)
{
	static int _last = 0;

	_last = Frame;

	if (PlayerPtr == DAT_00AC1198) {
		int c = Scen->ElapsedTimer() / 60;
		Credits = c;
		if (c == Current && !forced) return;
		Current = c;
		IsToRedraw = true;
		Map.Flag_To_Redraw();
		Map.IsToRedrawCredits = true;
		return;
	}
	Credits = PlayerPtr->Available_Money();
	Credits = MAX(Credits, 0L);

	if (Scen->MissionTimer.Is_Active() || Scen->MissionTimer) {
		IsToRedraw = true;
		Map.Flag_To_Redraw();
	}

	if (Current == Credits) return;

	if (forced) {
		IsAudible = false;
		Current = Credits;
	} else {
		if (Countdown) Countdown--;

		int adder = Credits - Current;

		if (adder > 0) {
			Countdown = 1;
		} else {
			Countdown = 3;
		}

		adder = abs(adder);
		adder >>= 3;
		adder = Bound(adder, 1, 71+72);
		if (Current > Credits) adder = -adder;
		Current += adder;
		if (Current-adder != Current) {
			IsAudible = true;
			IsUp = (adder > 0);
		}
	}
	IsToRedraw = true;
	Map.Flag_To_Redraw();
	Map.IsToRedrawCredits = true;
}
