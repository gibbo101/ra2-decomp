// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
