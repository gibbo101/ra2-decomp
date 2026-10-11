// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "bench.h"

// FUNCTION: 0x004318F0
Benchmark::Benchmark(void) :
	Average(0),
	Counter(0),
	TotalCount(0)
{
}

// FUNCTION: 0x00431920
void Benchmark::Reset(void)
{
	Average = 0;
	Counter = 0;
	TotalCount = 0;
}

// FUNCTION: 0x004319D0
unsigned int Benchmark::Step(void)
{
	unsigned int value = Clock;

	Average += value;

	Counter++;
	TotalCount++;

	return(TotalCount);
}

// FUNCTION: 0x00431A10
unsigned int Benchmark::Value(void) const
{
	if (Counter) {
		return(Average / Counter);
	}
	return(0);
}
