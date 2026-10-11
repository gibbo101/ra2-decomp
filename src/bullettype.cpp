// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "bullettype.h"
#include "_map.h"
#include "_rules.h"
#include "animtype.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "rules.h"
#include "scheme.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include "weapon.h"

// FUNCTION: 0x0046C4F0
Coord const BulletTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord c = coord;
	int height = Map.Get_Height_GL(c);
	int z = c.Z;
	if (z <= height) {
		z = Map.Get_Height_GL(c) + 1;
	}
	c.Z = z;
	return(c);
}
