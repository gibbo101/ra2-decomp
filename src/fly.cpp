// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "fly.h"
#include "_map.h"
#include "_rand.h"
#include "_rtti.h"
#include "_rules.h"
#include "aircraft.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "coord.h"
#include "face.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "iflyctrl.h"
#include "incdec.h"
#include "inline.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "team.h"
#include "unit.h"
#include "unittype.h"
#include "weapon.h"
#include "layer.hh"
#include <new.h>

// FUNCTION: 0x004CC9A0
FlyLocomotionClass::FlyLocomotionClass(void) :
	BASECLASS(),
	AirportBound(false),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsMoving(false),
	FlightLevel(0),
	TargetSpeed(0.0),
	CurrentSpeed(0.0),
	IsTakingOff(false),
	IsLanding(false),
	CommencedLanding(false),
	IsTumbling(false),
	CurrentROT(0),
	Riser(0),
	IsElevating(false)
{
}

// FUNCTION: 0x004CCA20
HRESULT STDMETHODCALLTYPE FlyLocomotionClass::Link_To_Object(void * object)
{
	HRESULT result = BASECLASS::Link_To_Object(object);
	if (result == S_OK) {
		if (object != NULL && ((FootClass *)object)->Fetch_RTTI() == RTTI_AIRCRAFT && ((AircraftClass *)object)->Class->IsAirportBound) {
			AirportBound = true;
		} else {
			AirportBound = false;
		}
	}
	return(result);
}

// FUNCTION: 0x004CCA70
FlyLocomotionClass::~FlyLocomotionClass(void)
{
}

// FUNCTION: 0x004CCA90
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Moving(void)
{
	return(IsMoving || LinkedTo->PitchAngle > 0);
}

// FUNCTION: 0x004CCAC0
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Moving_Now(void)
{
	if (CurrentSpeed == 0) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x004CCAE0
CoordStruct STDMETHODCALLTYPE FlyLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: 0x004CCB40
boolean STDMETHODCALLTYPE FlyLocomotionClass::Process(void)
{
	if (!IsLanding && !IsTakingOff && TargetSpeed >= 1.0 && FlightLevel == 0) {
		FlightLevel = LinkedTo->TClass->GetFlightLevel();
	}

	if (LinkedTo->Strength > 0 && !IsLanding && !IsTakingOff && LinkedTo->Ready_To_Next_Mission()) {
		LinkedTo->Commence();
	}

	Rotation_AI();
	Movement_AI();

	if (!Is_Powered()) return(false);

	if (!LinkedTo->IsActive) return(false);

	if (LinkedTo->Strength > 0 &&
		DestinationCoord != COORD_NONE &&
		!IsLanding && !IsTakingOff && LinkedTo->HeightAGL > 0) {
		Nearing_Target(true, DestinationCoord);
	}

	if (LinkedTo->Strength > 0) {
		Landing_Takeoff_AI();
	}

	Edge_Of_World_AI();

	return(Is_Moving());
}

bool FlyLocomotionClass::Is_Locked_To_Straight_Flight(void)
{
	if (DestinationCoord.Z > Map.Get_Height_GL(DestinationCoord) + 120) {
		return(true);
	}
	if (LinkedTo->TarCom != NULL && LinkedTo->Ammo) {
		return(true);
	}

	IFlyControlPtr flyctrl(LinkedTo);
	if (flyctrl != NULL) {
		if (flyctrl->Is_Locked()) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x004CCC80
void STDMETHODCALLTYPE FlyLocomotionClass::Move_To(CoordStruct to)
{
	if (((Coord)to).As_Cell() != DestinationCoord.As_Cell() || !IsLanding) {

		if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsParalyzed() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn() && Is_Powered()) {

			if ((Coord)to == COORD_NONE) {
				int landing_altitude = 0;
				IFlyControlPtr flyctrl(LinkedTo);
				if (flyctrl != NULL) {
					landing_altitude = flyctrl->Landing_Altitude();
				}

				if ((LinkedTo->HeightAGL > landing_altitude || IsTakingOff) && !IsLanding) {
					DestinationCoord = LinkedTo->PositionCoord;
					Land();
				} else {
					DestinationCoord = COORD_NONE;
				}
				IsElevating = false;
			} else {
				DestinationCoord = to;

				if (LinkedTo->TarCom && LinkedTo->Ammo) {
					DestinationCoord.Z = LinkedTo->TClass->GetFlightLevel() + Map.Get_Height_GL(to);
				}

				IFlyControlPtr flyctrl(LinkedTo);
				int landing_altitude = 0;
				if (flyctrl != NULL) {
					landing_altitude = flyctrl->Landing_Altitude();
				}


				IsMoving = true;

				if (LinkedTo->Strength > 0 && !IsTakingOff && (IsLanding || LinkedTo->HeightAGL <= landing_altitude)) {
					Take_Off();
				}

				bool locked = Is_Locked_To_Straight_Flight();
				IsElevating = locked;
				if (LinkedTo->Fetch_RTTI() == RTTI_AIRCRAFT && !((AircraftClass *)LinkedTo)->Class->IsLandable) {
					IsElevating = true;
				}
			}
		}
	}
}

// FUNCTION: 0x004CCFD0
void STDMETHODCALLTYPE FlyLocomotionClass::Stop_Moving(void)
{
	if (Is_Moving()) {

		Cell cell = LinkedTo->PositionCoord.As_Cell();
		if (!LinkedTo->IsALoaner && !Map.In_Local_Radar(cell, true) && !LinkedTo->Should_Delete_Off_Map()) {
			cell = Map.Closest_Edge_Cell(cell, true);
		}

		FootClass * foot = LinkedTo;
		if (foot != NULL && foot->Fetch_RTTI() == RTTI_AIRCRAFT) {
			if (cell == CELL_NONE) {
				LinkedTo->Take_Damage(LinkedTo->Strength, 0, Rule->C4Warhead, NULL, true, true, false);
				DestinationCoord = COORD_NONE;
			} else {
				AbstractClass * target;
				if (LinkedTo->Get_Mission() == MISSION_ATTACK) {
					target = ((AircraftClass *)foot)->FUN_0041A160();
				} else {
					target = ((AircraftClass *)foot)->New_LZ(&Map[cell]);
				}
				LinkedTo->Assign_Destination(target, true);
			}
		} else {
			Cell nearby_cell = Map.Nearby_Location(cell, SPEED_TRACK, -1, MZONE_FLYER);
			if (nearby_cell != CELL_NONE) {

				Coord dest = nearby_cell.As_Coord();
				dest.Z = Map.Get_Height_GL(dest);
				if (Map[nearby_cell].IsUnderBridge) {
					dest.Z += BRIDGE_LEPTON_HEIGHT;
				}

				LinkedTo->Assign_Destination(&Map[dest], true);
				if (!IsLanding) {
					LinkedTo->Assign_Destination(&Map[dest], true);
				}
			} else {
				if (LinkedTo->Strength > 0) {
					LinkedTo->Take_Damage(LinkedTo->Strength, 0, Rule->C4Warhead, NULL, true, true, false);
					DestinationCoord = COORD_NONE;
				}
			}
		}
	}
}

// FUNCTION: 0x004CD2A0
bool FlyLocomotionClass::Landing_Takeoff_AI(void)
{
	FootClass * foot = LinkedTo;
	if (foot != NULL && foot->Fetch_RTTI() == RTTI_AIRCRAFT && !((AircraftClass *)foot)->Class->IsLandable) {
		IsElevating = true;
		IsLanding = false;
		IsTakingOff = false;
		FlightLevel = LinkedTo->TClass->GetFlightLevel();
		return(false);
	}

	if (LinkedTo->Strength > 0) {
		if (IsLanding || IsTakingOff) {
			LayerType layer = LinkedTo->In_Which_Layer();

			LinkedTo->Mark(MARK_UP);
			Map.Remove(LinkedTo);

			if (IsLanding) {
				Process_Landing();
			}
			if (IsTakingOff) {
				Process_Take_Off();
			}

			if (layer != LinkedTo->In_Which_Layer()) {

				bool ok = true;
				if (LinkedTo->In_Which_Layer() == LAYER_GROUND && !IsTakingOff) {
					if (!LinkedTo->Is_LZ_Clear(&Map[(Coord const &)LinkedTo->PositionCoord])) {
						IsTakingOff = true;
						LinkedTo->IsOnBridge = false;
						LinkedTo->Mark(MARK_UP);
						LinkedTo->HeightAGL += 10;
						LinkedTo->Mark(MARK_DOWN);
						ok = false;
					}
				}

				if (ok) {
					if (LinkedTo->In_Which_Layer() == LAYER_GROUND) {
						LinkedTo->Assign_Destination(NULL, true);
						LinkedTo->Transmit_Message(RADIO_TETHER);
						LinkedTo->vt_entry_48C(false, false, false, NULL);
						LinkedTo->UpdateSight(false, false, false, NULL, 0);
						Map.RevealArea3(LinkedTo->PositionCoord, LinkedTo->LastSightRange - 3, LinkedTo->LastSightRange + 3, false);
					} else  {
						LinkedTo->Transmit_Message(RADIO_UNTETHER);

						if (LinkedTo->HasAnyLink() && LinkedTo->NavCom && LinkedTo->NavCom != LinkedTo->FUN_0065AD30(0)) {
							LinkedTo->Transmit_Message(RADIO_OVER_OUT);
						}
					}
				}
			}

			Map.Submit(LinkedTo);
			LinkedTo->Mark(MARK_DOWN);
		}
	}
	return(false);
}

// FUNCTION: 0x004CD510
bool FlyLocomotionClass::Edge_Of_World_AI(void)
{
	if (!Map.In_Local_Radar((Coord const &)LinkedTo->PositionCoord)) {
		if (LinkedTo->Mission == MISSION_RETREAT) {
			while (LinkedTo->Cargo.Is_Something_Attached()) {
				FootClass * obj = LinkedTo->Cargo.Detach_Object();

				if (Counts_As_Civ_Evac(obj)) {
					obj->House->IsCivEvacuated = true;
				}

				if (obj->Team != NULL) obj->Team->IsLeaveMap = true;

				delete obj;
			}
			if (LinkedTo->Team != NULL) {
				LinkedTo->Team->IsLeaveMap = true;
			}
			LinkedTo->Stun();
			LinkedTo->Delete_Me();
			return(true);
		}
	} else {
		LinkedTo->IsInPlayfield = true;
	}
	return(false);
}

// FUNCTION: 0x004CE4B0
ImpactType FlyLocomotionClass::Physics(Coord & coord, DirType facing)
{
	int actual = Apparent_Speed();

	if (actual > 0) {
		Coord		newcoord;
		newcoord = Move_Coord(coord, facing, actual);

		coord = newcoord;

		if (!Map.In_Radar(coord.As_Cell())) {
			return(IMPACT_EDGE);
		}

		return(IMPACT_NORMAL);
	}
	return(IMPACT_NONE);
}

// FUNCTION: 0x004CE5A0
void FlyLocomotionClass::Rotation_AI(void)
{
	if (IsTumbling && LinkedTo->Strength > 0) {
		if (!Is_Powered()) {
			DirType newdir = DirType(LinkedTo->PrimaryFacing.Current()) + DirType((Dir256)CurrentROT);
			LinkedTo->PrimaryFacing.Set(newdir);
			int diff = MIN(abs(CurrentROT), 1);
			if (CurrentROT < 0) {
				CurrentROT += diff;
			} else {
				CurrentROT -= diff;
			}
		} else {
			IsTumbling = false;
			Map[(Coord const &)LinkedTo->PositionCoord].Trigger_Veins();
		}
	}
}

// FUNCTION: 0x004CF940
Point2DStruct STDMETHODCALLTYPE FlyLocomotionClass::Shadow_Point(void)
{
	return(Point2D(0, 0));
}

// FUNCTION: 0x004CFC80
HRESULT STDMETHODCALLTYPE FlyLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FlyerLocomotion;
	return(S_OK);
}

// FUNCTION: 0x004CFCC0
HRESULT STDMETHODCALLTYPE FlyLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) FlyLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: 0x004CFD90
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Powered(void)
{
	return(BASECLASS::Is_Powered());
}

// FUNCTION: 0x004D0390 FlyLocomotionClass::Fetch_Object_Size
