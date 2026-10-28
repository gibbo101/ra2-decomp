// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "session.h"

// SYNTHETIC: GAMEMD 0x0077B090 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x0077B0C0 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x0077B0E0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x0077B100 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x0077B120 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x0077B140 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x0077B160 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x0077B1A0 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x0077B1D0 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x0077B200 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x0077B230 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x0077B240 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x0077B260 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x0077B280
int __fastcall FUN_0077B280(void)
{
	int state = Session.unknown_0C;
	if (state == 0 || state == 6) {
		return 0;
	}
	return (state == 3) + 1;
}
