// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rocket.h"
#include "fastmath.h"
#include "_map.h"
#include "_rules.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "foot.h"
#include "inline.h"
#include "globals.h"
#include "rules.h"

// SYNTHETIC: GAMEMD 0x00661CB0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00661CE0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00661D00 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00661D20 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00661D40 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00661D60 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00661D80 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00661DC0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00661DF0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00661E20 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00661E50 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00661E60 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00661E80 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00661EA0 SYMBOL
// _$E66

// FUNCTION: GAMEMD 0x00661EC0
RocketLocomotionClass::RocketLocomotionClass(void) :
	BASECLASS(),
	MovingDestination(COORD_NONE),
	MissionState(0),
	CurrentSpeed(0.0),
	Unknown50(true),
	IsElite(false),
	CurrentPitch(0.0f),
	Unknown58(0)
{
}

// FUNCTION: GAMEMD 0x00661F30
RocketLocomotionClass::~RocketLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x00661F50
boolean STDMETHODCALLTYPE RocketLocomotionClass::Is_Moving(void)
{
	return(MovingDestination != COORD_NONE);
}

// FUNCTION: GAMEMD 0x00661F90
boolean STDMETHODCALLTYPE RocketLocomotionClass::Is_Moving_Now(void)
{
	if (MissionState >= 3 && MissionState <= 5) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00661FB0
CoordStruct STDMETHODCALLTYPE RocketLocomotionClass::Destination(void)
{
	return(MovingDestination);
}

// FUNCTION: GAMEMD 0x00661FE0
Coord RocketLocomotionClass::Position_At(int distance)
{
	Coord pos = LinkedTo->PositionCoord;
	int flat = (int)(fastmath::cos(CurrentPitch) * (double)distance);
	Coord result = Move_Coord(pos, LinkedTo->PrimaryFacing.Current(), flat);
	result.Z = (int)(fastmath::sin(CurrentPitch) * (double)distance + pos.Z);
	return(result);
}

// FUNCTION: GAMEMD 0x006620F0
bool RocketLocomotionClass::Reached_Target(RocketStruct const * data)
{
	int z = Position_At(data->BodyLength).Z;
	if (z <= MovingDestination.Z) {
		Detonate();
		return(true);
	}
	CellClass * cell = LinkedTo->Get_Cell_Ptr();
	if (cell != NULL && cell->IsUnderBridge) {
		if (cell->Center_Coord().Z == MovingDestination.Z && z <= MovingDestination.Z + BRIDGE_LEPTON_HEIGHT) {
			Detonate();
			return(true);
		}
	}
	if (LinkedTo->Get_Height_AGL() <= 0) {
		Detonate();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00662240
double RocketLocomotionClass::Pitch_To_Target(void)
{
	Coord pos = LinkedTo->PositionCoord;
	Coord diff = MovingDestination - pos;
	double flat = fastmath::sqrt((double)(diff.X * diff.X + diff.Y * diff.Y));
	if (flat > 0.0) {
		return(fastmath::atan((double)diff.Z / flat));
	}
	return(-1.5707963267948966);
}

// FUNCTION: GAMEMD 0x00663030
void RocketLocomotionClass::Detonate(void)
{
	AircraftTracker.Remove(LinkedTo);
	AircraftTypeClass const * type = ((AircraftClass *)LinkedTo)->Class;
	RocketStruct const * data;
	WarheadTypeClass const * warhead;
	if (type == Rule->V3Rocket.Type) {
		data = &Rule->V3Rocket;
		warhead = IsElite ? Rule->V3EliteWarhead : Rule->V3Warhead;
	} else if (type == Rule->CMisl.Type) {
		data = &Rule->CMisl;
		warhead = IsElite ? Rule->CMislEliteWarhead : Rule->CMislWarhead;
	} else {
		data = &Rule->DMisl;
		warhead = IsElite ? Rule->DMislEliteWarhead : Rule->DMislWarhead;
	}
	Coord coord = Position_At(data->BodyLength);
	Cell cell = coord;
	int damage = IsElite ? data->EliteDamage : data->Damage;
	new AnimClass(Combat_Anim(damage, warhead, Map[cell].Land_Type(), coord), coord, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(coord), false);
	Combat_Lighting(coord, damage, warhead);
	MapClass::DamageArea(coord, damage, LinkedTo, warhead, true, NULL);
	LinkedTo->Delete_Me();
}

// FUNCTION: GAMEMD 0x006632E0
void STDMETHODCALLTYPE RocketLocomotionClass::Move_To(CoordStruct to)
{
	AircraftTypeClass const * type = ((AircraftClass *)LinkedTo)->Class;
	RocketStruct const * data;
	if (type == Rule->V3Rocket.Type) {
		data = &Rule->V3Rocket;
	} else if (type == Rule->CMisl.Type) {
		data = &Rule->CMisl;
	} else {
		data = &Rule->DMisl;
	}
	if (MovingDestination == COORD_NONE) {
		if (data->PauseFrames != 0) {
			MissionState = 1;
			MissionTimer = data->PauseFrames;
		} else {
			MissionState = 2;
			MissionTimer = data->TiltFrames;
		}
		CurrentPitch = data->PitchInitial * 1.5707963267948966;
		MovingDestination = to;
	}
}

// FUNCTION: GAMEMD 0x006633C0
void STDMETHODCALLTYPE RocketLocomotionClass::Stop_Moving(void)
{
}

// FUNCTION: GAMEMD 0x006633D0
HRESULT STDMETHODCALLTYPE RocketLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSIDs::Rocket;
	return(S_OK);
}
