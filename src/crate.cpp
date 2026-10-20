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

// FUNCTION: 0x004A1560 $E2
// FUNCTION: 0x004A1590 $E5
// FUNCTION: 0x004A15B0 $E8
// FUNCTION: 0x004A15D0 $E11
// FUNCTION: 0x004A15F0 $E14
// FUNCTION: 0x004A1610 $E17
// FUNCTION: 0x004A1630 $E20
// FUNCTION: 0x004A1670 $E23
// FUNCTION: 0x004A16A0 $E26
// FUNCTION: 0x004A16D0 $E29
// FUNCTION: 0x004A1700 $E32
// FUNCTION: 0x004A1710 $E35

// FUNCTION: 0x004A1750
bool CrateClass::Remove_It(void)
{
	if (Is_Valid()) {
		Get_Crate(Location);
		Make_Invalid();
		return(true);
	}
	return(false);
}
