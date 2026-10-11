// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "taction.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "_weapon.h"
#include "anim.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "empulse.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "ion.h"
#include "ionblast.h"
#include "map.h"
#include "movie.h"
#include "partsys.h"
#include "reinf.h"
#include "revent.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "team.h"
#include "teamtype.h"
#include "theme.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "vanim.h"
#include "vector.h"
#include "velocity.h"
#include "voc.h"
#include "vox.h"
#include "warhead.h"
#include "waypoint.h"
#include "weapon.h"
#include "draw.hh"
#include "need.hh"
#include <cstdio>

// FUNCTION: 0x006E0480
bool TActionClass::TAction_FORCE_END(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	PlayerPtr->Flag_To_End();
	return(true);
}

// FUNCTION: 0x006E2640
bool TActionClass::TAction_ION_STORM_STOP(HouseClass * , ObjectClass * , TriggerClass * , Cell const & )
{
	if (!IonStormClass::Is_Ion_Storm_Active()) {
		return(false);
	}

	IonStormClass::Ion_Storm_End();
	return(true);
}
