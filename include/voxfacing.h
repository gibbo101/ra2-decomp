// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once
#include "facing.h"

static inline bool Is_Barrel_Ahead(FacingClass const & facing)
{
	return(facing.Current().As_Dir4() > 0
		&& facing.Current().As_Dir4() < 3);
}
