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
#include "ccrand.h"
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
extern const double DAT_007ECE68;
extern const double DAT_007ECE70;

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

// FUNCTION: 0x0054B980
void JumpjetLocomotionClass::Process_Grounded(void)
{
	if (Is_Moving()) {
		LinkedTo->Set_Speed(1.0);
		Facing.Set(LinkedTo->PrimaryFacing.Current());
		CurrentSpeed = 0;
		TargetSpeed = 0;
		FlightLevel = Height;
		if (!LinkedTo->House->IonSensitivesShouldBeOffline()) {
			if (LinkedTo->GetLastFlightMapCoords() == CELL_NONE) {
				AircraftTracker.Add(LinkedTo);
			}
			CurrentState = ASCENDING;
		}
	}
}

static inline DirType & Direction_To(DirType & dir, Coord const & coord1, Coord const & coord2)
{
	if (coord1.X == coord2.X && coord1.Y == coord2.Y) {
		dir.Facing = 0;
	}
	dir = DirType(fastmath::atan2((double)coord1.Y - (double)coord2.Y, (double)coord2.X - (double)coord1.X));
	return dir;
}

// FUNCTION: 0x0054BA30
void JumpjetLocomotionClass::Process_Ascent(void)
{
	int height = LinkedTo->HeightAGL;
	if (!LinkedTo->IsOnBridge) {
		if (Map[LinkedTo->Get_Coord()].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
			height -= BRIDGE_LEPTON_HEIGHT;
		}
	}

	Cell cell = LinkedTo->Get_Cell();
	if (cell != LinkedTo->LastFlightMapCoords) {
		AircraftTracker.Update(LinkedTo, LinkedTo->LastFlightMapCoords, cell);
	}

	if (height >= FlightLevel) {
		if (AircraftTracker.IsJumpjet(LinkedTo)) {
			int face = Random_Pick(0, 7);
			LinkedTo->Assign_Destination(&Map[cell].Adjacent_Cell((FacingType)face), true);
			bool down = LinkedTo->IsDown;
			LinkedTo->Mark(MARK_UP);
			LinkedTo->IsDown = down;
			CurrentState = CRUISING;
			return;
		} else {
			LinkedTo->Get_Cell_Ptr()->TryAssignJumpjet(LinkedTo);
			bool down = LinkedTo->IsDown;
			LinkedTo->Mark(MARK_UP);
			LinkedTo->IsDown = down;
			CurrentState = HOVERING;
			return;
		}
	}

	int limit;
	if (LinkedTo->Fetch_RTTI() == RTTI_UNIT && LinkedTo->TClass->BalloonHover) {
		limit = FlightLevel / 2;
	} else {
		limit = FlightLevel / 4;
	}
	if (height > limit) {
		TargetSpeed = Speed;
		DirType dir;
		Facing.Set_Desired(Direction_To(dir, LinkedTo->PositionCoord, HeadToCoord));

		TechnoClass * contact = LinkedTo->FUN_0065AD30(0);
		if (contact != NULL && contact->Fetch_RTTI() == RTTI_BUILDING) {
			BuildingTypeClass * btype = ((BuildingClass *)contact)->Class;
			if (btype->IsWeaponsFactory || btype->IsGDIBarracks || btype->IsNODBarracks || btype->IsYuriBarracks) {
				LinkedTo->INoticeSink_Unknown(0x117b, 0);
			}
		}

		if (cell == HeadToCoord.As_Cell() && !LinkedTo->TClass->BalloonHover) {
			FlightLevel = LinkedTo->HeightAGL;
			bool down = LinkedTo->IsDown;
			LinkedTo->Mark(MARK_UP);
			CurrentState = CRUISING;
			LinkedTo->IsDown = down;
		}
	}
}

static inline void Release_Jumpjet(FootClass * const & foot)
{
	if (foot->Get_Cell_Ptr()->Jumpjet == foot) {
		foot->Get_Cell_Ptr()->TryAssignJumpjet(NULL);
	}
}

// FUNCTION: 0x0054BFF0
void JumpjetLocomotionClass::Process_Cruise(void)
{
	Coord position = LinkedTo->PositionCoord;

	Cell cell = LinkedTo->Get_Cell();
	if (cell != LinkedTo->LastFlightMapCoords) {
		AircraftTracker.Update(LinkedTo, LinkedTo->LastFlightMapCoords, cell);
	}

	DirType dir;
	Facing.Set_Desired(Direction_To(dir, position, HeadToCoord));
	DirType turn = Facing.Difference();
	bool balloon = LinkedTo->Techno_Type_Class()->BalloonHover;

	int distance = Point2D(position).Distance_To(Point2D(HeadToCoord));

	FootClass * foot = LinkedTo;
	UnitClass * unit;
	if (foot == NULL) {
		unit = NULL;
	} else {
		unit = (foot->Fetch_RTTI() == RTTI_UNIT) ? (UnitClass *)foot : NULL;
	}

	if (distance < 20) {
		CurrentSpeed = 0;
		TargetSpeed = 0;
		position.X = HeadToCoord.X;
		position.Y = HeadToCoord.Y;
		bool down = LinkedTo->IsDown;
		LinkedTo->IsDown = false;
		LinkedTo->Set_Coord(position);
		LinkedTo->IsDown = down;
		if (LinkedTo->IsAttackedByLocomotor) {
			FlightLevel = 0;
			if (LinkedTo->LocomotorSource != NULL) {
				LinkedTo->LocomotorSource->ReleaseLocomotor(true);
			}
			CurrentState = DESCENDING;
		} else if (LinkedTo->TarCom == NULL && !balloon) {
			if (unit != NULL && unit->Class->IsSimpleDeployer && unit->Techno_Type_Class()->DeployToLand && !LinkedTo->IsAttackedByLocomotor) {
				if (AircraftTracker.IsJumpjet(LinkedTo)) {
					int face = Random_Pick(0, 7);
					LinkedTo->Assign_Destination(&Map[cell].Adjacent_Cell((FacingType)face), true);
					LinkedTo->FUN_00705D60();
				} else {
					LinkedTo->Get_Cell_Ptr()->TryAssignJumpjet(LinkedTo);
					FlightLevel = Height;
					bool d = LinkedTo->IsDown;
					LinkedTo->Mark(MARK_UP);
					LinkedTo->IsDown = d;
					CurrentState = HOVERING;
					LinkedTo->FUN_00705D60();
				}
			} else {
				FlightLevel = 0;
				CurrentState = DESCENDING;
				LinkedTo->FUN_00705D60();
			}
		} else {
			if (AircraftTracker.IsJumpjet(LinkedTo)) {
				int face = Random_Pick(0, 7);
				LinkedTo->Assign_Destination(&Map[cell].Adjacent_Cell((FacingType)face), true);
			} else {
				LinkedTo->Get_Cell_Ptr()->TryAssignJumpjet(LinkedTo);
				bool d = LinkedTo->IsDown;
				LinkedTo->Mark(MARK_UP);
				LinkedTo->IsDown = d;
				CurrentState = HOVERING;
			}
		}
	} else if (distance < Speed) {
		TargetSpeed = Speed / 8;
		if (LinkedTo->TarCom == NULL) {
			FlightLevel = Height / 2;
		}
	} else if (distance < Speed * 2) {
		TargetSpeed = Speed / 4;
		if (LinkedTo->TarCom == NULL) {
			FlightLevel = Height / 2;
		}
		if (turn.As_Dir256() > TurnRate) {
			TargetSpeed = Speed / 10;
			if (TargetSpeed < 1.0) {
				TargetSpeed = 1.0;
			}
		}
	} else if (distance < Speed * 50 / TurnRate) {
		TargetSpeed = Speed / 2;
		if (LinkedTo->TarCom == NULL) {
			FlightLevel = Height * 0.75;
		}
		if (turn.As_Dir256() > TurnRate * 5) {
			TargetSpeed = Speed / 5;
			if (TargetSpeed < 1.0) {
				TargetSpeed = 1.0;
			}
		}
	} else {
		TargetSpeed = Speed;
		FlightLevel = Height;
	}

	CellClass * dest = &Map[HeadToCoord];
	if (balloon || dest->Land_Type() == LAND_WATER || dest->Land_Type() == LAND_BEACH || (unit != NULL && unit->Techno_Type_Class()->DeployToLand)) {
		FlightLevel = Height;
	}
}

// FUNCTION: 0x0054C550
void JumpjetLocomotionClass::Process_Descent(void)
{
	CellClass * cellptr = &Map[HeadToCoord];

	if (!LinkedTo->IsAttackedByLocomotor) {
		if ((cellptr->Land_Type() == LAND_WATER || cellptr->Land_Type() == LAND_BEACH) && LinkedTo->MissionQueue != MISSION_ENTER && LinkedTo->Get_Mission() != MISSION_ENTER) {
			if (AircraftTracker.IsJumpjet(LinkedTo)) {
				int face = Random_Pick(0, 7);
				LinkedTo->Assign_Destination(&LinkedTo->Get_Cell_Ptr()->Adjacent_Cell((FacingType)face), true);
				bool down = LinkedTo->IsDown;
				LinkedTo->Mark(MARK_UP);
				LinkedTo->IsDown = down;
				CurrentState = CRUISING;
				return;
			} else {
				LinkedTo->Get_Cell_Ptr()->TryAssignJumpjet(LinkedTo);
				bool down = LinkedTo->IsDown;
				LinkedTo->Mark(MARK_UP);
				LinkedTo->IsDown = down;
				CurrentState = HOVERING;
				FlightLevel = Height;
				return;
			}
		}

		MoveType move = LinkedTo->Can_Enter_Cell(cellptr, FACING_NONE, -1, 0, true);
		int spot = CellClass::Spot_Index(HeadToCoord);
		bool landing = cellptr->Is_Spot_Free(spot, cellptr->IsUnderBridge) || IsLanding;
		if (LinkedTo->Fetch_RTTI() == RTTI_UNIT && spot == 0) {
			landing = true;
		}

		if (LinkedTo->MissionQueue != MISSION_ENTER && LinkedTo->Get_Mission() != MISSION_ENTER) {
			if (!landing || !(Map[HeadToCoord].IsUnderBridge || move <= MOVE_MOVING_BLOCK && (move != MOVE_MOVING_BLOCK || IsLanding))) {
				Stop_Moving();
				return;
			}
			if (!IsLanding) {
				IsLanding = true;
				LinkedTo->Set_Occupy_Bit(HeadToCoord);
			}
		}
	}

	FootClass * foot = LinkedTo;
	if (foot != NULL && foot->Fetch_RTTI() == RTTI_UNIT && ((UnitClass *)foot)->Class->IsSimpleDeployer && !LinkedTo->IsAttackedByLocomotor) {
		if (Facing.Current() != DirType((Dir256)Rule->DeployDir)) {
			Facing.Set_Desired(DirType((Dir256)Rule->DeployDir));
		}
	}

	FlightLevel = 0;

	int height = LinkedTo->HeightAGL;
	if (!LinkedTo->IsOnBridge) {
		if (Map[LinkedTo->Get_Coord()].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
			height -= BRIDGE_LEPTON_HEIGHT;
		}
	}

	if (height == 0) {
		LinkedTo->Set_Speed(0.0);
		LinkedTo->Mark(MARK_UP);
		if (!LinkedTo->IsAttackedByLocomotor) {
			LinkedTo->Set_Coord(HeadToCoord);
		}

		Coord position = LinkedTo->PositionCoord;
		if (position.Z > Map.Get_Height_GL(LinkedTo->PositionCoord)) {
			if (Map[LinkedTo->Get_Coord()].IsUnderBridge) {
				LinkedTo->IsOnBridge = true;
			}
		}

		LinkedTo->Mark(MARK_DOWN);
		HeadToCoord = COORD_NONE;
		IsMoving = false;
		LinkedTo->Per_Cell_Process(PCP_END);
		LinkedTo->Assign_Destination(NULL, true);

		if (LinkedTo->IsActive && !LinkedTo->IsInLimbo && !LinkedTo->IsFalling) {
			LinkedTo->vt_entry_48C(false, false, false, NULL);
			if (LinkedTo->IsActive) {
				LinkedTo->UpdateSight(false, false, false, NULL, 0);
			}
			if (LinkedTo->IsActive) {
				Map.RevealArea3(LinkedTo->PositionCoord, LinkedTo->LastSightRange - 3, LinkedTo->LastSightRange + 3, 0);
			}

			Release_Jumpjet(LinkedTo);
			AircraftTracker.Remove(LinkedTo);
			CurrentState = GROUNDED;
			LinkedTo->Get_Cell_Ptr()->Goodie_Check(LinkedTo);
			LinkedTo->IsLetGoByLocomotor = true;
			LinkedTo->IsBeingManipulated = false;
			LinkedTo->IsCrashing = false;
			IsLanding = false;

			if (LinkedTo != NULL && LinkedTo->Fetch_RTTI() == RTTI_UNIT) {
				foot = LinkedTo;
				UnitClass * unit;
				if (foot == NULL) {
					unit = NULL;
				} else {
					unit = (foot->Fetch_RTTI() == RTTI_UNIT) ? (UnitClass *)foot : NULL;
				}
				if (unit->Techno_Type_Class()->DeployToLand) {
					LinkedTo->InAir = false;
				}
			}
		}
	}
}

// FUNCTION: 0x0054D0C0
void JumpjetLocomotionClass::Process_Unknown(void)
{
}

// FUNCTION: 0x0054D0D0
boolean STDMETHODCALLTYPE JumpjetLocomotionClass::Is_Moving_Now(void)
{
	if (CurrentState != GROUNDED && CurrentState != HOVERING) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0054D6D0
Coord JumpjetLocomotionClass::Closest_Free_Spot(Coord const & to) const
{
	Coord closest;
	if (LinkedTo != NULL && LinkedTo->Fetch_RTTI() == RTTI_UNIT) {
		closest = Map[to].Center_Coord();
		if (closest != COORD_NONE) {
			closest.Z = Map.Get_Height_GL(closest);
			if (Map[closest].IsUnderBridge) {
				closest.Z += BRIDGE_LEPTON_HEIGHT;
			}
		}
	} else {
		closest = Map.Closest_Free_Spot(to);
		if (closest != COORD_NONE) {
			closest.Z = Map.Get_Height_GL(closest);
			if (Map[closest].IsUnderBridge) {
				closest.Z += BRIDGE_LEPTON_HEIGHT;
			}
		}
	}
	return(closest);
}

// FUNCTION: 0x0054D820
int JumpjetLocomotionClass::Desired_Flight_Level(void) const
{
	Coord coord = LinkedTo->PositionCoord;

	int height = Map[coord].Occupier_Height();
	if (Map[coord].IsUnderBridge) {
		height += BRIDGE_LEPTON_HEIGHT;
	}

	if (CurrentSpeed > 0) {
		coord = Adjacent_Cell(coord, (FacingType)Facing.Current().As_Dir8());
		int adjacent_height = Map[coord].Occupier_Height();
		if (Map[coord].IsUnderBridge) {
			height += BRIDGE_LEPTON_HEIGHT;
		}
		if (adjacent_height > height) {
			return(adjacent_height);
		}
		return((height + adjacent_height) / 2);
	}
	return(height);
}

// FUNCTION: 0x0054D930
void STDMETHODCALLTYPE JumpjetLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		Coord headto = Head_To_Coord();
		if (headto != COORD_NONE && (CurrentState == GROUNDED || IsLanding)) {
			LinkedTo->Clear_Occupy_Bit(headto);
			IsLanding = false;
		}
	}
}

// FUNCTION: 0x0054D9B0
CoordStruct STDMETHODCALLTYPE JumpjetLocomotionClass::Head_To_Coord(void)
{
	if (CurrentState == GROUNDED) {
		return(LinkedTo->PositionCoord);
	} else {
		return(HeadToCoord);
	}
}

// FUNCTION: 0x0054DA00
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker == NULL) {
		Piggybacker = pointer;
		return(S_OK);
	}
	return(E_FAIL);
}

// FUNCTION: 0x0054DA50
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::End_Piggyback(ILocomotion ** pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (LinkedTo != NULL) {
		if (LinkedTo->IsAttackedByLocomotor || LinkedTo->IsLetGoByLocomotor) {
			LinkedTo->IsAttackedByLocomotor = false;
			LinkedTo->IsLetGoByLocomotor = false;
			if (!LinkedTo->House->IsHuman) {
				if (LinkedTo->Techno_Type_Class()->ResourceGatherer) {
					LinkedTo->QueueMission(MISSION_HARVEST, false);
				}
			}
		}
		LinkedTo->BeingManipulatedBy = NULL;
		LinkedTo->ChronoWarpedByHouse = NULL;
	}
	if (Piggybacker != NULL) {
		*pointer = Piggybacker;
		Piggybacker.Detach();
		return(S_OK);
	}
	return(S_FALSE);
}

// FUNCTION: 0x0054DB00
boolean STDMETHODCALLTYPE JumpjetLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && Piggybacker != NULL && !IsPiggyReady) {
		if (!LinkedTo->IsAttackedByLocomotor || LinkedTo->IsLetGoByLocomotor) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0054DB50
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::Piggyback_CLSID(GUID * classid)
{
	if (classid == NULL) {
		return(E_POINTER);
	}

	if (Piggybacker != NULL) {
		IPersistPtr ptr(Piggybacker);
		if (ptr == NULL) {
			return(E_FAIL);
		}
		return(ptr->GetClassID(classid));
	}

	IPersistPtr ptr(this);
	if (ptr == NULL) {
		return(E_FAIL);
	}
	return(ptr->GetClassID(classid));
}

// FUNCTION: 0x0054DC60
HRESULT STDMETHODCALLTYPE JumpjetLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	HRESULT result = BASECLASS::QueryInterface(riid, ppvObject);

	if (result == E_NOINTERFACE) {
		if (riid == IID_IPiggyback) {
			*ppvObject = (IPiggyback*)this;
		}
		if (*ppvObject == NULL) {
			result = E_NOINTERFACE;
		} else {
			AddRef();
			result = S_OK;
		}
	}
	return(result);
}

// FUNCTION: 0x0054DCC0
Matrix3DStruct STDMETHODCALLTYPE JumpjetLocomotionClass::Draw_Matrix(int * key)
{
	if (LinkedTo->TClass->TiltCrashJumpjet && ((float)fabs(LinkedTo->AngleRotatedSideways) >= 0.005 || (float)fabs(LinkedTo->AngleRotatedForwards) >= 0.005)) {
		if (key) {
			*key = -1;
		}

		Matrix3D m1(true), m2(true);
		Matrix3D basemtx;

		float val = LinkedTo->TClass->VoxelCenterY;
		float val2 = LinkedTo->TClass->VoxelCenterX;

		float scos, fcos, fsin;
		fcos = fastmath::cos(LinkedTo->AngleRotatedForwards);
		fsin = fastmath::sin(LinkedTo->AngleRotatedForwards);

		scos = fastmath::cos(LinkedTo->AngleRotatedSideways);
		float ssin = fastmath::sin(LinkedTo->AngleRotatedSideways);

		int num = fabs(ssin) * val + fabs(fsin) * val2;

		int x_offset = (int)(val2 - (float)(int)(fcos * val2));
		int y_offset = (int)(val - (float)(int)(scos * val));
		if (LinkedTo->AngleRotatedForwards < 0.0f) {
			x_offset = -x_offset;
		}
		if (LinkedTo->AngleRotatedSideways > 0.0f) {
			y_offset = -y_offset;
		}

		m1.Translate_Z(num);
		m2.Translate_X(x_offset);
		m2.Translate_Y(y_offset);
		m2.Rotate_X(LinkedTo->AngleRotatedSideways);
		m2.Rotate_Y(LinkedTo->AngleRotatedForwards);

		return(basemtx.Matrix3D::Matrix3D(BASECLASS::Draw_Matrix(key)), (Matrix3DStruct &)((m1 * basemtx) * m2));
	}

	return(BASECLASS::Draw_Matrix(key));
}

// FUNCTION: 0x0054DF50
ULONG STDMETHODCALLTYPE JumpjetLocomotionClass::AddRef(void)
{
	return(BASECLASS::AddRef());
}

// FUNCTION: 0x0054DF60
ULONG STDMETHODCALLTYPE JumpjetLocomotionClass::Release(void)
{
	return(BASECLASS::Release());
}

// FUNCTION: 0x0054DF70 JumpjetLocomotionClass::Is_Piggybacking

// FUNCTION: 0x0054DF90 JumpjetLocomotionClass::Fetch_Object_Size

// FUNCTION: 0x0054DFA0 JumpjetLocomotionClass::~JumpjetLocomotionClass

// FUNCTION: 0x0054DFF0 JumpjetLocomotionClass::QueryInterface

// FUNCTION: 0x0054E000 JumpjetLocomotionClass::AddRef

// FUNCTION: 0x0054E010 JumpjetLocomotionClass::Release

// FUNCTION: 0x0054E020 JumpjetLocomotionClass::QueryInterface

// FUNCTION: 0x0054E030 JumpjetLocomotionClass::AddRef

// FUNCTION: 0x0054E040 JumpjetLocomotionClass::Release

// FUNCTION: 0x0054E050 $E102
double DAT_00ABC638 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0054E080 $E105
double DAT_00ABC688 = DAT_00ABC638 / DAT_007ECE68;

// FUNCTION: 0x0054E0A0 $E108
double DAT_00ABC670 = DAT_007E1718 / DAT_00ABC688;

// FUNCTION: 0x0054E0C0 $E111
double DAT_00ABC668 = DAT_007E1720 * DAT_007ECE70;

// FUNCTION: 0x0054E0E0 $E114
double DAT_00ABC640 = DAT_007E1728 * DAT_007ECE70;

// FUNCTION: 0x0054E100 $E117
double DAT_00ABC5F0 = DAT_007E1730 * DAT_007ECE70;

// FUNCTION: 0x0054E120 $E120
int DAT_00ABC690 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABC5F0 - DAT_00ABC640) * DAT_00ABC638));

// FUNCTION: 0x0054E160 $E123
double DAT_00ABC648 = FUN_004CADE0(DAT_00ABC690 * DAT_007E1740);
