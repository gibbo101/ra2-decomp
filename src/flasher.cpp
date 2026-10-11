// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "flasher.h"
#include "mono.h"

// FUNCTION: 0x004CC770
bool FlasherClass::Process(void)
{
	if (FlashCount) {
		FlashCount--;
		IsBlushing = false;

		if (FlashCount & 0x01) {
			IsBlushing = true;
		}
		return(true);
	}
	return(false);
}
