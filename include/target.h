// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

#include "abstract.h"

#include "rtti.hh"

inline bool Is_Target_Team(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_TEAM);}
inline bool Is_Target_TeamType(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_TEAMTYPE);}
inline bool Is_Target_Trigger(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_TRIGGER);}
inline bool Is_Target_TriggerType(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_TRIGGERTYPE);}
inline bool Is_Target_Infantry(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_INFANTRY);}
inline bool Is_Target_Bullet(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_BULLET);}
inline bool Is_Target_Terrain(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_TERRAIN);}
inline bool Is_Target_Cell(const AbstractClass * target) {return(target != NULL && target->RTTI ==  RTTI_CELL);}
inline bool Is_Target_Unit(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_UNIT);}
inline bool Is_Target_Building(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_BUILDING);}
inline bool Is_Target_IsoTile(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_ISOTILE);}
inline bool Is_Target_Aircraft(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_AIRCRAFT);}
inline bool Is_Target_Animation(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_ANIM);}
inline bool Is_Target_Tag(const AbstractClass * target) {return(target != NULL && target->RTTI == RTTI_TAG);}

class UnitClass;
class BuildingClass;
class TechnoClass;
class TerrainClass;
class ObjectClass;
class InfantryClass;
class BulletClass;
class TriggerClass;
class TeamClass;
class TeamTypeClass;
class AnimClass;
class AircraftClass;
class CellClass;
class TriggerTypeClass;
class AbstractTypeClass;
class TagClass;
class TagTypeClass;
class FootClass;
class HouseClass;
class TechnoTypeClass;
class Cell;
class Coord;
class CCINIClass;

#pragma pack(push,1)

class xTargetClass
{
	friend class CCINIClass;
	protected:

		int ID;

		unsigned char RTTI;

	public:

		operator RTTIType (void) const {return(RTTIType(RTTI));}

		bool operator == (xTargetClass & tgt) {return(RTTI == tgt.RTTI && ID == tgt.ID);}

		unsigned Value(void) const {return(ID);};

		void Invalidate(void) {RTTI = RTTI_NONE; ;}
		bool Is_Valid(void) const {return(RTTI != RTTI_NONE);}

		int Encode(void)
		{
			int value;
			if (RTTI == RTTI_NONE) {
				value = -1;
			} else {
				value = ((RTTI << 24) | (ID & 0x00FFFFFF));
			}
			return(value);
		}

		xTargetClass Decode(int value)
		{
			Invalidate();

			RTTI = (value >> 24) & 0xFF;
			ID = value   & 0x00FFFFFF;

			return(*this);
		}

		AbstractTypeClass * As_TypeClass(void) const;
		AbstractClass * As_Abstract(void) const;
		TechnoClass * As_Techno(void) const;
		ObjectClass * As_Object(void) const;
		CellClass * As_Cell(void) const;

		TriggerTypeClass * As_TriggerType(void) const;
		TeamTypeClass * As_TeamType(void) const;
		TerrainClass * As_Terrain(void) const;
		BulletClass * As_Bullet(void) const;
		AnimClass * As_Anim(void) const;
		TeamClass * As_Team(void) const;
		InfantryClass * As_Infantry(void) const;
		UnitClass * As_Unit(void) const;
		BuildingClass * As_Building(void) const;
		AircraftClass * As_Aircraft(void) const;

		TagClass * As_Tag(void) const;
		TagTypeClass * As_TagType(void) const;
		FootClass * As_Foot(void) const;
		TriggerClass * As_Trigger(void) const;
		HouseClass * As_House(void) const;
		TechnoTypeClass * As_TechnoType(void) const;
};

class TargetClass : public xTargetClass
{
	public:

		TargetClass(void) {Invalidate();}
		TargetClass(NoInitClass const &) {}

		TargetClass(Cell const &cell);
		TargetClass(Coord const &coord);

		TargetClass(AbstractClass const * ptr);
};
#pragma pack(pop)

template<class INDEX, class T> class IndexClass;

extern IndexClass<int, AbstractClass *> TargetTracker;
