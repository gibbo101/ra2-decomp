// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mpu.h"
#include "win.h"
#include <math.h>

typedef union {
	LARGE_INTEGER LargeInt;
	struct QuadPart {
		unsigned int LowPart;
		unsigned int HighPart;
	} QuadPart;
} QuadValue;

// FUNCTION: 0x005CAF70
unsigned int Get_CPU_Rate(unsigned int & high)
{
	union {
		LARGE_INTEGER LargeInt;
		struct {
			unsigned int LowPart;
			unsigned int HighPart;
		} QuadPart;
	} value;

	if (QueryPerformanceFrequency(&value.LargeInt)) {
		high = value.QuadPart.HighPart;
		return(value.QuadPart.LowPart);
	}
	high = 0;
	return(0);
}
