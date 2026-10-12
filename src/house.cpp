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

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007EA758, DAT_007EA760;
extern double DAT_00A8EFA0, DAT_00A8F030, DAT_00A8F018, DAT_00A8F010, DAT_00A8EFE0;
extern double DAT_00A8EF58, DAT_00A8EFF0, DAT_00A8F008;
extern int DAT_00A8F038, DAT_00A8F02C, DAT_00A8EFF8, DAT_00A8EFFC, DAT_00A8F000;
extern short DAT_00A8EF98, DAT_00A8EF9A;

// FUNCTION: 0x004F4F00
void Init_004F4F00(void)
{
	DAT_00A8EFA0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x004F4F30
void Init_004F4F30(void)
{
	DAT_00A8F030 = DAT_00A8EFA0 / DAT_007EA758;
}

// FUNCTION: 0x004F4F50
void Init_004F4F50(void)
{
	DAT_00A8F018 = DAT_007E1718 / DAT_00A8F030;
}

// FUNCTION: 0x004F4F70
void Init_004F4F70(void)
{
	DAT_00A8F010 = DAT_007E1720 * DAT_007EA760;
}

// FUNCTION: 0x004F4F90
void Init_004F4F90(void)
{
	DAT_00A8EFE0 = DAT_007E1728 * DAT_007EA760;
}

// FUNCTION: 0x004F4FB0
void Init_004F4FB0(void)
{
	DAT_00A8EF58 = DAT_007E1730 * DAT_007EA760;
}

// FUNCTION: 0x004F4FD0
void Init_004F4FD0(void)
{
	DAT_00A8F038 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00A8EF58 - DAT_00A8EFE0) * DAT_00A8EFA0));
}

// FUNCTION: 0x004F5010
void Init_004F5010(void)
{
	DAT_00A8EFF0 = FUN_004CADE0(DAT_00A8F038 * DAT_007E1740);
}

// FUNCTION: 0x004F5040
void Init_004F5040(void)
{
	DAT_00A8F008 = FUN_004CADE0((DAT_00A8F038 * 2) / DAT_00A8EFA0);
}

// FUNCTION: 0x004F5070
void Init_004F5070(void)
{
	DAT_00A8F02C = (int)((DAT_00A8F038 * 4) + DAT_007E1738);
}

// FUNCTION: 0x004F50A0
void Init_004F50A0(void)
{
	DAT_00A8EF98 = 0;
	DAT_00A8EF9A = 0;
}

// FUNCTION: 0x004F50B0
void Init_004F50B0(void)
{
	DAT_00A8EFF8 = 0;
	DAT_00A8EFFC = 0;
	DAT_00A8F000 = 0;
}

// FUNCTION: 0x004F6930
IApplication * STDMETHODCALLTYPE HouseClass::Get_Application(void)
{
	IApplication *app;
	ApplicationInstance.QueryInterface(IID_IApplication, (void**)&app);
	return(app);
}

// FUNCTION: 0x004F6E60
LONG STDMETHODCALLTYPE HouseClass::ID_Number(void)
{
	return(HeapID);
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

// FUNCTION: 0x005046F0
HRESULT STDMETHODCALLTYPE HouseClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HouseClass;
	return(S_OK);
}
