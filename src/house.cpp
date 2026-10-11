// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "house.h"
#include "_logic.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "app.h"
#include "assert.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "conquer.h"
#include "cpoint.h"
#include "data.h"
#include "dbgprint.h"
#include "ddist.h"
#include "dsurface.h"
#include "factory.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipowerevents.h"
#include "language\language.h"
#include "lightcon.h"
#include "logic.h"
#include "mono.h"
#include "mouse.h"
#include "overtype.h"
#include "quarry.h"
#include "revent.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "taskforc.h"
#include "team.h"
#include "techtype.h"
#include "tiberium.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "unit.h"
#include "unittype.h"
#include "vector.h"
#include "vein.h"
#include "voc.h"
#include "vox.h"
#include "waypoint.h"
#include "color.hh"
#include "strategy.hh"

// FUNCTION: 0x004F6930
IApplication * STDMETHODCALLTYPE HouseClass::Get_Application(void)
{
	IApplication *app;
	ApplicationInstance.QueryInterface(IID_IApplication, (void**)&app);
	return(app);
}

// FUNCTION: 0x004FA210
SUGGESTED_TEAM_LIST HouseClass::Suggested_New_Team(bool alertcheck)
{
	SUGGESTED_TEAM_LIST teams = TeamTypeClass::Suggested_New_Team(this, alertcheck);
	return(teams);
}

// FUNCTION: 0x004FD120
Coord HouseClass::Find_Build_Location(BuildingClass * building) const
{
	return(COORD_NONE);
}

// FUNCTION: 0x004FFD90
ZoneType HouseClass::Which_Zone(Cell const & cell) const
{
	return(Which_Zone((Coord)cell));
}

// FUNCTION: 0x00501AA0
Cell HouseClass::Find_Cell_In_Zone(TechnoClass const * techno, ZoneType zone) const
{
	return(Cell(0, 0));
}
