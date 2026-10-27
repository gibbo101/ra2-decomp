// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "flasher.h"
#include "mono.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>


// FUNCTION: GAMEMD 0x004CC770
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
// SYNTHETIC: GAMEMD 0x004CC790 SYMBOL
// _$E4
// SYNTHETIC: GAMEMD 0x004CC7C0 SYMBOL
// _$E7
// SYNTHETIC: GAMEMD 0x004CC7E0 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x004CC800 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x004CC820 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x004CC840 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x004CC860 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x004CC8A0 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x004CC8D0 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x004CC900 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x004CC930 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x004CC940 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x004CC960 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x004CC980 SYMBOL
// _$E43

