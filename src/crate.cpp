// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "crate.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_tactica.h"
#include "ccrand.h"
#include "cell.h"
#include "overlay.h"
#include "rules.h"
#include "tactical.h"

// SYNTHETIC: GAMEMD 0x004A1540 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x004A1560 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004A1590 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004A15B0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004A15D0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004A15F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004A1610 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004A1630 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004A1670 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004A16A0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004A16D0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004A1700 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004A1710 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004A1730 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x004A1750
bool CrateClass::Remove_It(void)
{
	if (Is_Valid()) {
		Get_Crate(Location);
		Make_Invalid();
		return(true);
	}
	return(false);
}
