// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "counter.h"
#include <comdef.h>

// FUNCTION: 0x0049FAE0
int CounterClass::Value(int index) const
{
	if (const_cast<CounterClass *>(this)->Reserve(index)) {
		return(Vector[index]);
	}
	return(0);
}

// FUNCTION: 0x0049FC80
bool CounterClass::Reserve(int index)
{
	if (index < VectorMax) {
		return(true);
	}

	int oldmax = VectorMax;
	if (Resize(index + GROWTH_STEP)) {
		for (int i = oldmax; i < VectorMax; i++) {
			Vector[i] = int(0);
		}
		return(true);
	}
	return(false);
}
