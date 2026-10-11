// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "unittype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_rules.h"
#include "bsurface.h"
#include "convert.h"
#include "face.h"
#include "findmake.h"
#include "globals.h"
#include "inline.h"
#include "mixfile.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "tracker.h"
#include "unit.h"
#include "color.hh"

// FUNCTION: 0x00747600
TPoint2D<int> UnitTypeClass::Turret_Adjust(Dir256 dir, TPoint2D<int> const & xy) const
{
	return(xy);
}
