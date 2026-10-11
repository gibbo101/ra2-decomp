// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mstimer.h"
#include "win.h"

// FUNCTION: 0x005D5870
MillisecondSystemTimerClass::MillisecondSystemTimerClass(void)
{
	timeBeginPeriod(1);
}

// FUNCTION: 0x005D5880
MillisecondSystemTimerClass::~MillisecondSystemTimerClass(void)
{
	timeEndPeriod(1);
}
