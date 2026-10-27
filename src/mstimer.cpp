// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "mstimer.h"
#include "win.h"

// FUNCTION: GAMEMD 0x005D5870
MillisecondSystemTimerClass::MillisecondSystemTimerClass(void)
{
	timeBeginPeriod(1);
}

// FUNCTION: GAMEMD 0x005D5880
MillisecondSystemTimerClass::~MillisecondSystemTimerClass(void)
{
	timeEndPeriod(1);
}

// FUNCTION: GAMEMD 0x005D5890
// MillisecondSystemTimerClass::operator()
int MillisecondSystemTimerClass::operator () (void) const
{
	return(timeGetTime());
}

// FUNCTION: GAMEMD 0x005D58A0
// ??BMillisecondSystemTimerClass@@QBEHXZ
MillisecondSystemTimerClass::operator int (void) const
{
	return(timeGetTime());
}

#include <math.h>

// SYNTHETIC: GAMEMD 0x005D58B0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005D58E0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005D5900 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005D5920 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005D5940 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005D5960 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005D5980 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005D59C0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005D59F0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005D5A20 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005D5A50 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005D5A60 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005D5A80 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005D5AA0 SYMBOL
// _$E41

