// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

#include "milsectmr.h"

#include "dbgprint.h"
#include "getcpu.h"
#include "mpu.h"
#include "win.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x005B1B80 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005B1BA0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005B1BD0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005B1BF0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005B1C10 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005B1C30 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005B1C50 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005B1C70 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005B1CB0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005B1CE0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005B1D10 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005B1D40 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005B1D50 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005B1D70 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x005B1D90
MillisecondTimerClass::MillisecondTimerClass(void)
{
	unsigned int high = 0;
	Frequency = 1.0;
	unsigned int low = Get_CPU_Rate(high);

	if (low == 0 && high == 0) {
		timeBeginPeriod(1);

	} else {
		double dl = low;
		double dh = high;

		DebugString("MillisecondTimerClass low = %d, high = %d\n");

		Frequency = ((double)((double)dh * 4.294967296E9 + (double)dl)) / 1000;
	}
}

// FUNCTION: GAMEMD 0x005B1E20
MillisecondTimerClass::~MillisecondTimerClass(void)
{
	if (Frequency != 1.0) {
		timeEndPeriod(1);
	}
}

// FUNCTION: GAMEMD 0x005B1E40
MillisecondTimerClass::operator double () const
{
	static int cpu_type = -1;
	static bool has_mmx = false;

	if (cpu_type == -1) {
		Get_CPU_Type(cpu_type, has_mmx, NULL, 0);
	}
	if (Frequency != 1.0 && cpu_type > 4) {
		unsigned int high;
		unsigned int low;

		low = Get_CPU_Clock(high);
		double dl = low;
		double dh = high;

		return(((double)((double)dh * 4.294967296E9 + (double)dl)) / Frequency);
	}

	return(timeGetTime());
}
