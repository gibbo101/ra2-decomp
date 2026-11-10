// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "abstract.h"

class TechnoClass;
class FootClass;
class BuildingClass;
class UnitClass;
class InfantryClass;
class AircraftClass;
class CellClass;

inline TechnoClass * As_Techno(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->AbstractFlags & 1) return((TechnoClass *)object);
	return(NULL);
}

inline TechnoClass const * As_Techno(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->AbstractFlags & 1) return((TechnoClass const *)object);
	return(NULL);
}

inline FootClass * As_Foot(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->AbstractFlags & 4) return((FootClass *)object);
	return(NULL);
}

inline FootClass const * As_Foot(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->AbstractFlags & 4) return((FootClass const *)object);
	return(NULL);
}

inline BuildingClass * As_Building(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_BUILDING) return((BuildingClass *)object);
	return(NULL);
}

inline BuildingClass const * As_Building(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_BUILDING) return((BuildingClass const *)object);
	return(NULL);
}

inline UnitClass * As_Unit(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_UNIT) return((UnitClass *)object);
	return(NULL);
}

inline UnitClass const * As_Unit(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_UNIT) return((UnitClass const *)object);
	return(NULL);
}

inline InfantryClass * As_Infantry(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_INFANTRY) return((InfantryClass *)object);
	return(NULL);
}

inline InfantryClass const * As_Infantry(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_INFANTRY) return((InfantryClass const *)object);
	return(NULL);
}

inline AircraftClass * As_Aircraft(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_AIRCRAFT) return((AircraftClass *)object);
	return(NULL);
}

inline AircraftClass const * As_Aircraft(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_AIRCRAFT) return((AircraftClass const *)object);
	return(NULL);
}

inline CellClass * As_Cell(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_CELL) return((CellClass *)object);
	return(NULL);
}

inline CellClass const * As_Cell(AbstractClass const * object)
{
	if (object == NULL) return(NULL);
	if (object->Fetch_RTTI() == RTTI_CELL) return((CellClass const *)object);
	return(NULL);
}
