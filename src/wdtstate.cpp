// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "wdtnet.h"
#include "geometry.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x0076F260 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0076F280 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0076F2B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0076F2D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0076F2F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0076F310 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0076F330 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0076F350 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0076F390 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0076F3C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0076F3F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0076F420 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0076F430 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x0076F450 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x0076F470
rc_ptr<WorldDominationTour::State> __fastcall WorldDominationTour::FUN_0076F470(rc_ptr<WDTState> const & state, unsigned int tick)
{
	return rc_ptr<State>(new State(state, tick));
}

// FUNCTION: GAMEMD 0x0076F500
rc_ptr<WorldDominationTour::State> __fastcall WorldDominationTour::FUN_0076F500(void)
{
	return rc_ptr<State>(new State());
}

// FUNCTION: GAMEMD 0x0076F560
WorldDominationTour::State::State(void) :
	Owners(NULL)
{
}

// FUNCTION: GAMEMD 0x0076F5A0
WorldDominationTour::State::State(rc_ptr<WDTState> const & state, unsigned int tick) :
	TourState(state),
	Owners(NULL)
{
	if (TourState.Get() != NULL && tick < TourState->NumTicks) {
		Owners = TourState->OwnerHistory[tick];
	}
}

// FUNCTION: GAMEMD 0x0076F650
WorldDominationTour::State::~State(void)
{
}

// SYNTHETIC: GAMEMD 0x0076F6C0
// ??_GState@WorldDominationTour@@UAEPAXI@Z
