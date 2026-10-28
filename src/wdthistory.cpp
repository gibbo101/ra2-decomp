// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "wdtnet.h"
#include "geometry.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x00767910 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00767940 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00767960 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00767980 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007679A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007679C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x007679E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00767A20 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00767A50 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00767A80 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00767AB0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00767AC0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00767AE0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x00767B00
rc_ptr<WorldDominationTour::History> __fastcall FUN_00767B00(rc_ptr<WDTState> const & state)
{
	return rc_ptr<WorldDominationTour::History>(new WorldDominationTour::History(state));
}

// FUNCTION: GAMEMD 0x00767B70
WorldDominationTour::History::History(rc_ptr<WDTState> const & state) :
	TourState(state)
{
}

// FUNCTION: GAMEMD 0x00767BB0
rc_ptr<WorldDominationTour::State> WorldDominationTour::History::FUN_00767BB0(int tick)
{
	return FUN_0076F470(TourState, tick);
}

// FUNCTION: GAMEMD 0x00767BD0
bool WorldDominationTour::History::Is_Tick_Valid(unsigned int ticks)
{
	return(TourState.Get() != NULL && ticks < TourState->NumTicks);
}

// FUNCTION: GAMEMD 0x00767BF0
WorldDominationTour::History::~History(void)
{
}

// SYNTHETIC: GAMEMD 0x00767C10
// ??_GHistory@WorldDominationTour@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00767C50
// ??_G?$rc_ptr@VWDTState@@@@UAEPAXI@Z
template class rc_ptr<WDTState>;
