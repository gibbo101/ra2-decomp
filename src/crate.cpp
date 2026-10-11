// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
