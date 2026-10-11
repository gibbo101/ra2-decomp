// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "combuf.h"
#include "_mono.h"
#include "connect.h"
#include "dbgprint.h"
#include "mono.h"
#include <cstdio>
#include <cstring>

// FUNCTION: 0x0048BA10
void CommBufferClass::Add_Delay(unsigned int delay)
{
	int roundoff = 0;

	if (NumDelay==256) {
		DelaySum -= MeanDelay;
		DelaySum += delay;
		if ( (DelaySum & 0x00ff) > 127)
			roundoff = 1;
		MeanDelay = (DelaySum >> 8) + roundoff;
	}
	else {
		NumDelay++;
		DelaySum += delay;
		MeanDelay = DelaySum / NumDelay;
	}

	if (delay > MaxDelay) {
		MaxDelay = delay;
	}
}

// FUNCTION: 0x0048BAA0
void CommBufferClass::Reset_Response_Time(bool zero)
{
	if (zero) {
	DelaySum = 0L;
	NumDelay = 0L;
	MeanDelay = 0L;
	MaxDelay = 0L;
	} else {
		DelaySum = MeanDelay;
		NumDelay = 1L;
		MaxDelay = MeanDelay;
	}
}

// FUNCTION: 0x0048BAD0
void CommBufferClass::Configure_Debug(int type_offset, int type_size,
	char **names, int namestart, int namecount)
{
	DebugOffset = type_offset;
	DebugSize = type_size;
	DebugNames = names;
	DebugNameStart = namestart;
	DebugNameCount = namecount;
}
