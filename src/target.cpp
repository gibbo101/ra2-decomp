// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "target.h"

#include "_rtti.h"
#include "aircraft.h"
#include "anim.h"
#include "building.h"
#include "bullet.h"
#include "cell.h"
#include "house.h"
#include "index.h"
#include "infantry.h"
#include "tag.h"
#include "tagtype.h"
#include "team.h"
#include "teamtype.h"
#include "terrain.h"
#include "trigger.h"
#include "trigtype.h"
#include "unit.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F4640;
extern const double DAT_007F4648;

// FUNCTION: 0x006E6A50 $E94
// FUNCTION: 0x006E6A80 $E92
IndexClass<int, AbstractClass *> TargetTracker;

// FUNCTION: 0x006E6AB0
TargetClass::TargetClass(AbstractClass const * ptr)
{
	if (ptr != NULL) {
		if (ptr->Fetch_RTTI() == RTTI_CELL) {
			RTTI = RTTI_CELL;
			Cell cell = ((CellClass const *)ptr)->CellID;
			ID = (cell.X + (1000 * cell.Y));
		} else {
			RTTI = RTTI_ABSTRACT;
			ID = ((IRTTITypeInfo const *)ptr)->Fetch_ID();
		}
	} else {
		RTTI = RTTI_NONE;
		ID = 0;
	}
}

// FUNCTION: 0x006E6B20
TargetClass::TargetClass(Cell const & cell)
{
	if (cell == CELL_NONE) {
		RTTI = RTTI_NONE;
	} else {
		RTTI = RTTI_CELL;
		ID = (cell.X + (1000 * cell.Y));
	}
}

// FUNCTION: 0x006E6B70
TargetClass::TargetClass(Coord const & coord)
{
	RTTI = RTTI_CELL;
	ID = ((coord.X / CELL_LEPTON_W) + (1000 * (coord.Y / CELL_LEPTON_H)));
}

// FUNCTION: 0x006E6BB0
AbstractTypeClass * xTargetClass::As_TypeClass(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<AbstractTypeClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E6C80
TagClass * xTargetClass::As_Tag(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TagClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E6D50
TagTypeClass * xTargetClass::As_TagType(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TagTypeClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E6E20
AbstractClass * xTargetClass::As_Abstract(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<AbstractClass *>(TargetTracker[ID]));
	}

	if (RTTI == RTTI_CELL) {
		Cell cell(ID % 1000, ID / 1000);
		return(&Map[cell]);
	}
	return(NULL);
}

// FUNCTION: 0x006E6F20
TechnoClass * xTargetClass::As_Techno(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(Dynamic_Cast<TechnoClass *>((ObjectClass *)TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E6FF0
ObjectClass * xTargetClass::As_Object(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<ObjectClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E70C0
FootClass * xTargetClass::As_Foot(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<FootClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7190
TriggerClass * xTargetClass::As_Trigger(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TriggerClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7260
HouseClass * xTargetClass::As_House(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<HouseClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7330
TechnoTypeClass * xTargetClass::As_TechnoType(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TechnoTypeClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7400
TriggerTypeClass * xTargetClass::As_TriggerType(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TriggerTypeClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E74D0
TeamTypeClass * xTargetClass::As_TeamType(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TeamTypeClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E75A0
TerrainClass * xTargetClass::As_Terrain(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TerrainClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7670
BulletClass * xTargetClass::As_Bullet(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<BulletClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7740
AnimClass * xTargetClass::As_Anim(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<AnimClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7810
TeamClass * xTargetClass::As_Team(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<TeamClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E78E0
InfantryClass * xTargetClass::As_Infantry(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<InfantryClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E79B0
UnitClass * xTargetClass::As_Unit(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<UnitClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7A80
BuildingClass * xTargetClass::As_Building(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<BuildingClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7B50
AircraftClass * xTargetClass::As_Aircraft(void) const
{
	if (RTTI == RTTI_ABSTRACT) {
		return(dynamic_cast<AircraftClass *>(TargetTracker[ID]));
	}
	return(NULL);
}

// FUNCTION: 0x006E7C20
CellClass * xTargetClass::As_Cell(void) const
{
	if (RTTI == RTTI_CELL) {
		Cell cell(ID % 1000, ID / 1000);
		return(&Map[cell]);
	}
	return(NULL);
}

// FUNCTION: 0x006E7C70 $E97
double DAT_00B0E8D0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006E7CA0 $E100
double DAT_00B0E920 = DAT_00B0E8D0 / DAT_007F4640;

// FUNCTION: 0x006E7CC0 $E103
double DAT_00B0E908 = DAT_007E1718 / DAT_00B0E920;

// FUNCTION: 0x006E7CE0 $E106
double DAT_00B0E900 = DAT_007E1720 * DAT_007F4648;

// FUNCTION: 0x006E7D00 $E109
double DAT_00B0E8D8 = DAT_007E1728 * DAT_007F4648;

// FUNCTION: 0x006E7D20 $E112
double DAT_00B0E8B0 = DAT_007E1730 * DAT_007F4648;

// FUNCTION: 0x006E7D40 $E115
int DAT_00B0E928 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0E8B0 - DAT_00B0E8D8) * DAT_00B0E8D0));

// FUNCTION: 0x006E7D80 $E118
double DAT_00B0E8E0 = FUN_004CADE0(DAT_00B0E928 * DAT_007E1740);

// FUNCTION: 0x006E7DB0 $E121
double DAT_00B0E8F8 = FUN_004CADE0((DAT_00B0E928 * 2) / DAT_00B0E8D0);

// FUNCTION: 0x006E7DE0 $E124
int DAT_00B0E91C = (int)(DAT_00B0E928 * 4 + DAT_007E1738);
