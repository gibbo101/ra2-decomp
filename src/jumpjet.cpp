// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "jumpjet.h"
#include "_map.h"
#include "_rules.h"
#include "building.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "ion.h"
#include "mouse.h"
#include "rules.h"
#include "infantry.h"
#include "unit.h"
#include "unittype.h"
#include "layer.hh"
#include <new.h>

// FUNCTION: 0x0054AC40
JumpjetLocomotionClass::JumpjetLocomotionClass(void) :
	BASECLASS(),
	HeadToCoord(COORD_NONE),
	IsMoving(false),
	CurrentState(GROUNDED),
	Facing(Rule->JumpjetTurnRate),
	CurrentSpeed(0.0),
	TargetSpeed(0.0),
	FlightLevel(0),
	CurrentWobble(0.0),
	IsLanding(false),
	IsPiggyReady(false),
	Piggybacker(NULL)
{
	Facing.Set_Desired(DIR_E);
	Facing.Set(DIR_E);
}

// FUNCTION: 0x0054AD00
JumpjetLocomotionClass::~JumpjetLocomotionClass(void)
{
}

// FUNCTION: 0x0054AD30
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::Link_To_Object(void * object)
{
	FootClass * foot = (FootClass *)object;
	TurnRate = foot->TClass->JumpjetTurnRate;
	Speed = foot->TClass->JumpjetSpeed;
	Climb = foot->TClass->JumpjetClimb;
	Crash = foot->TClass->JumpjetCrash;
	int height = foot->TClass->JumpjetHeight;
	Height = MAX(height, LEVEL_LEPTON_H * 2);
	Accel = foot->TClass->JumpjetAccel;
	Wobbles = foot->TClass->JumpjetWobbles;
	Deviation = foot->TClass->JumpjetDeviation;
	NoWobbles = foot->TClass->JumpjetNoWobbles;
	FacingClass turning(TurnRate);
	Facing = turning;
	Facing.Set_Desired(DIR_E);
	Facing.Set(DIR_E);
	return(BASECLASS::Link_To_Object(object));
}

// FUNCTION: 0x0054AE50
boolean STDMETHODCALLTYPE JumpjetLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}

// FUNCTION: 0x0054AE60
CoordStruct STDMETHODCALLTYPE JumpjetLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(HeadToCoord);
	} else {
		return(COORD_NONE);
	}
}

// FUNCTION: 0x0054B1C0
void STDMETHODCALLTYPE JumpjetLocomotionClass::Move_To(CoordStruct to)
{
	if (HeadToCoord != COORD_NONE && CurrentState != GROUNDED && IsLanding) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
		IsLanding = false;
	}

	HeadToCoord = to;

	if ((Coord)to != COORD_NONE) {
		Cell cell;
		if (LinkedTo->RTTI == RTTI_UNIT && !((UnitClass *)LinkedTo)->Class->BalloonHover) {
			cell = Map.Nearby_Location(((Coord&)to).As_Cell(), SPEED_HOVER, -1, MZONE_FLYER, false, Point2D(1, 1), false, false, false, false);
		} else {
			cell = Map.Nearby_Location(((Coord&)to).As_Cell(), SPEED_HOVER, -1, MZONE_FLYER, Map[to].IsUnderBridge);
		}
		Coord free = Closest_Free_Spot(Coord(cell, 0));
		if (free != COORD_NONE) {
			if (LinkedTo->RTTI != RTTI_UNIT || (!LinkedTo->TClass->BalloonHover && LinkedTo->MissionQueue != MISSION_ENTER && LinkedTo->Get_Mission() != MISSION_ENTER)) {
				HeadToCoord = free;
			}
			if (LinkedTo->MissionQueue != MISSION_ENTER && LinkedTo->Get_Mission() != MISSION_ENTER) {
				LinkedTo->NavCom = &Map[HeadToCoord];
			}
			IsMoving = true;
			if (CurrentState == DESCENDING) {
				CurrentState = ASCENDING;
				FlightLevel = Height;
				LinkedTo->InAir = false;
				if (LinkedTo->Get_Mission() == MISSION_UNLOAD) {
					LinkedTo->QueueMission(MISSION_GUARD, true);
					LinkedTo->IsMissionUnloadStandby = true;
					if (LinkedTo->Ready_To_Next_Mission()) {
						LinkedTo->Commence();
					}
				}
			}
		}
	} else {
		IsMoving = false;
	}
}

// FUNCTION: 0x0054B4D0
void STDMETHODCALLTYPE JumpjetLocomotionClass::Stop_Moving(void)
{
	if (!ScenarioInit && IsMoving) {
		if (HeadToCoord != COORD_NONE && CurrentState != GROUNDED && IsLanding) {
			LinkedTo->Clear_Occupy_Bit(HeadToCoord);
			IsLanding = false;
		}
		Coord position = LinkedTo->Position;
		Cell cell = position.As_Cell();
		Cell nearby;
		nearby = Map.Nearby_Location(Cell(cell), LinkedTo->TClass->Speed, -1, LinkedTo->TClass->MZone, false, Point2D(1, 1), false, false, false, true);
		if (nearby != CELL_NONE) {
			Coord nearby_coord = nearby.As_Coord();
			nearby_coord.Z = Map.Get_Height_GL(nearby_coord);
			if (Map[nearby].IsUnderBridge) {
				nearby_coord.Z += BRIDGE_LEPTON_HEIGHT;
			}
			Move_To(nearby_coord);
		} else {
			if (LinkedTo->Strength > 0) {
				LinkedTo->Take_Damage(LinkedTo->Strength, 0, Rule->C4Warhead, NULL, true, true);
				HeadToCoord = COORD_NONE;
			}
		}
	}
}

// FUNCTION: 0x0054B6E0
void STDMETHODCALLTYPE JumpjetLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set(coord);
}

// FUNCTION: 0x0054B710
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_JumpjetLocomotion;
	return(S_OK);
}

// FUNCTION: 0x0054B750
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) JumpjetLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: 0x0054B7E0
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		bool haspiggy;
		if (Piggybacker != NULL) {
			haspiggy = true;
			stream->Write(&haspiggy, sizeof(haspiggy), NULL);
			IPersistStreamPtr ptr(Piggybacker);
			result = OleSaveToStream(ptr, stream);
		} else {
			haspiggy = false;
			stream->Write(&haspiggy, sizeof(haspiggy), NULL);
		}
	}
	return(result);
}

// FUNCTION: 0x0054B8D0
LayerType STDMETHODCALLTYPE JumpjetLocomotionClass::In_Which_Layer(void)
{
	int height = LinkedTo->HeightAGL;
	if (!LinkedTo->IsOnBridge) {
		if (Map[LinkedTo->Get_Coord()].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT && !LinkedTo->IsFalling) {
			height -= BRIDGE_LEPTON_HEIGHT;
		}
	}

	if (!LinkedTo->In_Air()) {
		return(LAYER_GROUND);
	}
	if (height == 0) {
		return(LAYER_GROUND);
	}
	int layer = LAYER_AIR;
	layer += (height >= Height);
	return((LayerType)layer);
}
