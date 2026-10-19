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
