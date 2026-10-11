// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "foot.h"
#include "_astar.h"
#include "_bench.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "convert.h"
#include "ddist.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipiggy.h"
#include "loco.h"
#include "mono.h"
#include "partsys.h"
#include "revent.h"
#include "rules.h"
#include "session.h"
#include "swizzle.h"
#include "tactical.h"
#include "team.h"
#include "tracker.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "voc.h"
#include "vox.h"
#include "waypoint.h"
#include "weapon.h"
#include "xsurface.h"
#include "bench.hh"
#include "color.hh"
#include "tube.hh"

// FUNCTION: 0x004D9FF0
Coord FootClass::Likely_Coord(void) const
{
	return(Target_Coord());
}

// FUNCTION: 0x004DA020
Cell FootClass::Adjust_Dest(Cell const & cell) const
{
	return(cell);
}
