// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "fly.h"
#include "_map.h"
#include "_rand.h"
#include "_rtti.h"
#include "_rules.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "coord.h"
#include "face.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "iflyctrl.h"
#include "incdec.h"
#include "inline.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "team.h"
#include "unit.h"
#include "unittype.h"
#include "weapon.h"
#include "layer.hh"
#include <new.h>

// FUNCTION: 0x004CE4B0
ImpactType FlyLocomotionClass::Physics(Coord & coord, DirType facing)
{
	int actual = Apparent_Speed();

	if (actual > 0) {
		Coord		newcoord;
		newcoord = Move_Coord(coord, facing, actual);

		coord = newcoord;

		if (!Map.In_Radar(coord.As_Cell())) {
			return(IMPACT_EDGE);
		}

		return(IMPACT_NORMAL);
	}
	return(IMPACT_NONE);
}

// FUNCTION: 0x004CFD90
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Powered(void)
{
	return(BASECLASS::Is_Powered());
}
