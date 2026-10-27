// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include <math.h>
#include "geometry.h"
#include "rect.h"

// SYNTHETIC: GAMEMD 0x00435610 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00435640 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00435660 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00435680 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004356A0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004356C0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004356E0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00435720 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00435750 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00435780 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004357B0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004357C0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004357E0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00435800 SYMBOL
// _$E41

#include "blight.h"
#include "_logic.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "building.h"
#include "cell.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "logic.h"
#include "mouse.h"
#include "ovrlight.h"
#include "rules.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "wave.h"
#include "xsurface.h"

// FUNCTION: GAMEMD 0x00435820
BuildingLightClass::BuildingLightClass(TechnoClass * owner) :
	BASECLASS(),
	Speed(0),
	RotationPivot(COORD_NONE),
	RotationTarget(COORD_NONE),
	Acceleration(0),
	IsOppositeDirection(false),
	Behavior(0),
	Target(NULL),
	Owner(owner)
{
	BuildingLights.Add(this);
	if (owner != NULL) {
		Init_Rotation_Arc(owner);

		Unlimbo(PositionCoord);
		Set_Behavior_Type(LIGHT_BEHAVIOR_SWEEP);
		IsOppositeDirection = BuildingLights.ID(this) % 2 != 0;
	}
}

// FUNCTION: GAMEMD 0x00436900
int BuildingLightClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00436910
HRESULT STDMETHODCALLTYPE BuildingLightClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BuildingLightClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00436950
HRESULT STDMETHODCALLTYPE BuildingLightClass::Load(IStream *stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) BuildingLightClass(NoInitClass());

		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Target);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004369C0
HRESULT STDMETHODCALLTYPE BuildingLightClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x004369E0
ObjectTypeClass const * BuildingLightClass::Class_Of(void) const
{
	return(NULL);
}

// FUNCTION: GAMEMD 0x004369F0
LayerType BuildingLightClass::In_Which_Layer(void) const
{
	return(LAYER_AIR);
}

// FUNCTION: GAMEMD 0x00436A00
void BuildingLightClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	if (Target == target) {
		Target = NULL;
	}
	if (Owner == target) {
		Owner = NULL;
	}
}

// FUNCTION: GAMEMD 0x00436A40
void BuildingLightClass::Init_Rotation_Arc(TechnoClass * owner)
{
	Coord target = Move_Coord(owner->PositionCoord, owner->PrimaryFacing.Current(), Rule->SpotlightLocationRadius);
	RotationTarget = target;

	RotationPivot = Move_Coord(owner->PositionCoord, owner->PrimaryFacing.Current(), -Rule->SpotlightMovementRadius);

	PositionCoord = target;
}

// FUNCTION: GAMEMD 0x00436BE0
void BuildingLightClass::Set_Behavior_Type(LightBehaviorType type)
{
	Behavior = type;
	Speed = 0;

	if (Behavior == LIGHT_BEHAVIOR_FOLLOW) {
		Cell cell = PositionCell;

		int mindist = 9999999;
		ObjectClass * closest = NULL;

		HouseClass * house = Owner->House;

		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				ObjectClass * occupier = Map[Cell(TPoint2D<short>(x + cell.X, y + cell.Y))].Cell_Occupier();
				while (occupier != NULL) {
					if ((occupier->RTTI == RTTI_INFANTRY || occupier->RTTI == RTTI_UNIT) && !house->Is_Ally(occupier)) {
						int dist = occupier->Center_Coord().Distance_To(Get_Coord());
						if (dist < mindist) {
							closest = occupier;
							mindist = dist;
						}
					}
					occupier = occupier->Next;
				}

			}
		}

		if (closest != NULL) {
			Target = (TechnoClass *)closest;
		}
	}
}

// FUNCTION: GAMEMD 0x00436DA0
int BuildingLightClass::Detection_Radius(void) const
{
	static const double _scale = 7.466666666666666;
	int radius = Sweep_Stage() * _scale;
	radius += Rule->SpotlightRadius;
	return(radius);
}

// FUNCTION: GAMEMD 0x00436E80
int BuildingLightClass::Sweep_Stage(void) const
{
	int dist = Center_Coord().Distance_To(Owner->Center_Coord());
	int radius = Rule->SpotlightLocationRadius;
	if (dist < radius) {
		return(0);
	}
	return((dist - radius) / ((Rule->SpotlightMovementRadius - radius) / 10));
}

// FUNCTION: GAMEMD 0x00436F40
void BuildingLightClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Speed);
	crc(RotationPivot.X);
	crc(RotationPivot.Y);
	crc(RotationPivot.Z);
	crc(RotationTarget.X);
	crc(RotationTarget.Y);
	crc(RotationTarget.Z);
	crc(Acceleration);
	crc(IsOppositeDirection);
	crc(Behavior);
	if (Target != NULL) {
		crc(Target->Fetch_ID());
	}
	if (Owner != NULL) {
		crc(Owner->Fetch_ID());
	}
}

// FUNCTION: GAMEMD 0x00437030
bool BuildingLightClass::Limbo(void)
{
	if (BASECLASS::Limbo()) {
		Logic.Remove(this);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00437050
bool BuildingLightClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	if (BASECLASS::Unlimbo(coord, dir)) {
		Logic.Submit(this);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00437090
// Coord::Coord

// FUNCTION: GAMEMD 0x004370B0
// BuildingLightClass::Fetch_RTTI

// FUNCTION: GAMEMD 0x00435B50
// FUNCTION: GAMEMD 0x004370C0
BuildingLightClass::~BuildingLightClass(void)
{
	Limbo();
	BuildingLights.Delete(this);
}
