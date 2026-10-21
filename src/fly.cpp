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

// FUNCTION: GAMEMD 0x004CC9A0
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

// FUNCTION: GAMEMD 0x004CCA20
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

// FUNCTION: GAMEMD 0x004CCA70
FlyLocomotionClass::~FlyLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x004CCA90
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Moving(void)
{
	return(IsMoving || LinkedTo->PitchAngle > 0);
}

// FUNCTION: GAMEMD 0x004CCAC0
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Moving_Now(void)
{
	if (CurrentSpeed == 0) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x004CCAE0
CoordStruct STDMETHODCALLTYPE FlyLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x004CCB40
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

// FUNCTION: GAMEMD 0x004CCC80
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

// FUNCTION: GAMEMD 0x004CCFD0
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

// FUNCTION: GAMEMD 0x004CD2A0
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

// FUNCTION: GAMEMD 0x004CD510
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

// FUNCTION: GAMEMD 0x004CE4B0
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

// FUNCTION: GAMEMD 0x004CE5A0
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

// FUNCTION: GAMEMD 0x004CE680
bool FlyLocomotionClass::Process_Take_Off(void)
{
	int height = LinkedTo->HeightAGL;
	if (!LinkedTo->IsOnBridge && Map[(Coord const &)LinkedTo->PositionCoord].IsUnderBridge && height >= BRIDGE_LEPTON_HEIGHT) {
		height -= BRIDGE_LEPTON_HEIGHT;
	}

	IFlyControlPtr flyctrl(LinkedTo);
	int landing_altitude = 0;
	if (flyctrl) {
		landing_altitude = flyctrl->Landing_Altitude();
	}

	IsTakingOff = false;
	IsLanding = false;

	int relative_height = height - landing_altitude;
	int relative_flight_level = FlightLevel - landing_altitude;

	if (relative_height > relative_flight_level - (relative_flight_level / 3)) {
		LinkedTo->SecondaryFacing.Set_Desired(LinkedTo->PrimaryFacing.Desired());
	} else if (relative_height > relative_flight_level / 2) {
		DirType dir = Direction(LinkedTo->Center_Coord(), DestinationCoord);
		LinkedTo->PrimaryFacing.Set_Desired(dir);
		TargetSpeed = 1;
	}

	return(true);
}

// FUNCTION: GAMEMD 0x004CF610
Matrix3DStruct STDMETHODCALLTYPE FlyLocomotionClass::Draw_Matrix(int * key)
{
	Matrix3D mtx;
	mtx.Make_Identity();

	if (key) {
		*key |= LinkedTo->SecondaryFacing.Current().As_Dir32() << 3;
	}

	mtx.Rotate_Z(LinkedTo->SecondaryFacing.Current().As_Radian32());

	if (LinkedTo->HeightAGL > 0) {
		if (LinkedTo->IsCrashing) {
			mtx.Rotate_X(LinkedTo->AngleRotatedSideways);

			if (CurrentSpeed > LinkedTo->TClass->PitchSpeed) {
				mtx.Rotate_Y(float(LinkedTo->AngleRotatedForwards + LinkedTo->TClass->PitchAngle));
			} else {
				mtx.Rotate_Y(LinkedTo->AngleRotatedForwards);
			}
			if (key) {
				*key = -1;
			}
			return((Matrix3DStruct &)mtx);
		}

		if (CurrentSpeed > LinkedTo->TClass->PitchSpeed) {
			if (LinkedTo->TClass->PitchAngle != 0.0) {
				if (key) {
					*key |= 4;
				}
				mtx.Rotate_Y(float(LinkedTo->TClass->PitchAngle));
			}

			if (LinkedTo->TClass->RollAngle != 0.0) {
				if (LinkedTo->SecondaryFacing.Is_Rotating_CW()) {
					if (key) {
						*key |= 1;
					}
					mtx.Rotate_X(float(LinkedTo->TClass->RollAngle));
				} else if (LinkedTo->SecondaryFacing.Is_Rotating_CCW()) {
					if (key) {
						*key |= 2;
					}
					mtx.Rotate_X(float(-LinkedTo->TClass->RollAngle));
				}
			}
		}
	}

	return((Matrix3DStruct &)mtx);
}

// FUNCTION: GAMEMD 0x004CF830
Point2DStruct STDMETHODCALLTYPE FlyLocomotionClass::Draw_Point(void)
{
	int y = 0;
	IFlyControlPtr flyctrl(LinkedTo);

	int landing_altitude = 0;
	if (flyctrl) {
		landing_altitude = flyctrl->Landing_Altitude();
	}

	if (!LinkedTo->TClass->IsDropship && !IsLanding && !IsTakingOff && LinkedTo->HeightAGL > landing_altitude) {
		y = (fastmath::sin((Frame % 20) * M_PI / 10)) * 1.5 + 0.5;
	}

	return(Point2D(0, y));
}

// FUNCTION: GAMEMD 0x004CF940
Point2DStruct STDMETHODCALLTYPE FlyLocomotionClass::Shadow_Point(void)
{
	return(Point2D(0, 0));
}

// FUNCTION: GAMEMD 0x004CF950
void FlyLocomotionClass::Take_Off(void)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsParalyzed() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		IsLanding = false;
		IsTakingOff = true;

		if (LinkedTo->GetLastFlightMapCoords() == CELL_NONE) {
			AircraftTracker.Add(LinkedTo);
		}

		FlightLevel = LinkedTo->TClass->GetFlightLevel();

		if (LinkedTo->HeightAGL == 0) {
			LinkedTo->PrimaryFacing.Set(LinkedTo->SecondaryFacing.Desired());
		}

		VocClass::PlayAt(LinkedTo->TClass->AuxSound1, LinkedTo->PositionCoord, NULL);
	}
}

// FUNCTION: GAMEMD 0x004CFA70
void FlyLocomotionClass::Land(void)
{
	if (LinkedTo->Get_Mission() != MISSION_ENTER && LinkedTo->FUN_00705D60()) {
		return;
	}

	if (LinkedTo->Fetch_RTTI() == RTTI_AIRCRAFT) {
		FootClass * foot = LinkedTo;
		if (((AircraftClass *)foot)->Class->IsAirportBound) {
			if (!LinkedTo->ContainsLink(foot->Get_Cell_Ptr()->Cell_Building())) {
				foot->Enter_Idle_Mode(false, true);
				return;
			}
		}
	}

	IsTakingOff = false;
	IsLanding = true;
	CommencedLanding = false;
	FlightLevel = 0;
}

// FUNCTION: GAMEMD 0x004CFB00
Matrix3DStruct STDMETHODCALLTYPE FlyLocomotionClass::Shadow_Matrix(int * key)
{
	int ramp = Map[(Coord const &)LinkedTo->PositionCoord].Ramp;
	if (LinkedTo->TClass->IsDropship) {
		ramp = 0;
	}

	Matrix3D matrix = Get_Slope_Matrix(ramp);
	matrix.Rotate_Z(LinkedTo->SecondaryFacing.Current().As_Radian32());

	if (key != NULL && *key != -1) {
		*key = 32 * (ramp + (*key << 6));
		*key |= LinkedTo->SecondaryFacing.Current().As_Dir32();
	}
	return((Matrix3DStruct &)matrix);
}

// FUNCTION: GAMEMD 0x004CFC10
void STDMETHODCALLTYPE FlyLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->SecondaryFacing.Set(coord);
}

// FUNCTION: GAMEMD 0x004CFC40
bool FlyLocomotionClass::Is_In_Flight(void)
{
	if (!IsLanding && (!IsTakingOff || LinkedTo->HeightAGL >= FlightLevel / 2)) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004CFC80
HRESULT STDMETHODCALLTYPE FlyLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_FlyerLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x004CFCC0
HRESULT STDMETHODCALLTYPE FlyLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) FlyLocomotionClass(NoInitClass());
	}
	return(result);
}

// FUNCTION: GAMEMD 0x004CFCF0
LayerType STDMETHODCALLTYPE FlyLocomotionClass::In_Which_Layer(void)
{
	return(LinkedTo->HeightAGL <= 0 ? LAYER_GROUND : LAYER_TOP);
}

// FUNCTION: GAMEMD 0x004CFD20
boolean STDMETHODCALLTYPE FlyLocomotionClass::Power_Off(void)
{
	if (Is_Moving()) {
		Tumble();
		Stop_Moving();
		Riser = 0;
	}
	return(BASECLASS::Power_Off());
}

// FUNCTION: GAMEMD 0x004CFD90
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Powered(void)
{
	return(BASECLASS::Is_Powered());
}

// FUNCTION: GAMEMD 0x004CFDA0
boolean STDMETHODCALLTYPE FlyLocomotionClass::Is_Ion_Sensitive(void)
{
	return(!LinkedTo->TClass->IsHunterSeeker);
}

// FUNCTION: GAMEMD 0x004CFDD0
void FlyLocomotionClass::Tumble(void)
{
	IsTumbling = true;
	CurrentROT = Random_Pick(20, 30);
	if (Percent_Chance(50)) {
		CurrentROT = -CurrentROT;
	}
}

// FUNCTION: GAMEMD 0x004CFE20
int STDMETHODCALLTYPE FlyLocomotionClass::Apparent_Speed(void)
{
	return(LinkedTo->TClass->MaxSpeed * CurrentSpeed);
}

// FUNCTION: GAMEMD 0x004CFE50
int STDMETHODCALLTYPE FlyLocomotionClass::Get_Status(void)
{
	if (IsLanding) {
		return(1);
	}
	if (IsTakingOff) {
		return(0);
	}
	if (Is_Moving()) {
		return(2);
	}
	return(3);
}

// FUNCTION: GAMEMD 0x004CFE80
void STDMETHODCALLTYPE FlyLocomotionClass::Acquire_Hunter_Seeker_Target(void)
{
	if (LinkedTo->TarCom == NULL) {

		DynamicVectorClass<ObjectClass *> aitargets;
		DynamicVectorClass<ObjectClass *> humantargets;

		bool atenemy = Session.Type != GAME_NORMAL && LinkedTo->House->Enemy != HOUSE_NONE && !LinkedTo->House->Is_Human_Player();

		for (int i = Technos.Count() - 1; i >= 0; i--) {
			TechnoClass * techno = Technos[i];

			bool enemy = true;
			if (!atenemy) {
				if (LinkedTo->House->Is_Ally(techno->House)) {
					enemy = false;
				}
			} else {
				if (techno->House != Houses[LinkedTo->House->Enemy]) {
					enemy = false;
				}
			}

			if (enemy && techno->Strength > 0 && !techno->IsInLimbo && techno->IsActive &&
				!techno->TClass->IsInvisible && techno->TClass->IsLegalTarget &&
				!(Scen->Special.IsHarvesterImmune && techno->Fetch_RTTI() == RTTI_UNIT && ((UnitClass*)techno)->Class->Harvester)) {

				if (Session.Type != GAME_NORMAL && LinkedTo->House->Is_Human_Player() && !techno->House->Class->IsMultiplayPassive &&
					(techno->Fetch_RTTI() == RTTI_UNIT || techno->Fetch_RTTI() == RTTI_BUILDING || techno->Fetch_RTTI() == RTTI_INFANTRY || techno->Fetch_RTTI() == RTTI_AIRCRAFT)) {
					humantargets.Add(techno);
				} else {
					aitargets.Add(techno);
				}
			}
		}

		if (humantargets.Count() > 0) {
			LinkedTo->Assign_Target(humantargets[Scen->RandomNumber(0, humantargets.Count() - 1)]);
		} else if (aitargets.Count() > 0) {
			LinkedTo->Assign_Target(aitargets[Scen->RandomNumber(0, aitargets.Count() - 1)]);
		}
	}
}

// FUNCTION: GAMEMD 0x004D02B0
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

// FUNCTION: GAMEMD 0x004D0390
// FlyLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x004D03A0
// ??_GFlyLocomotionClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004D0400
// ??8?$_com_ptr_t@V?$_com_IIID@UIFlyControl@@$1?_GUID_820f501c_4f39_11d2_9b70_00104b972fe8@@3U__s_GUID@@A@@@@QAE_NH@Z

// TEMPLATE: GAMEMD 0x004D0430
// ?_Release@?$_com_ptr_t@V?$_com_IIID@UIFlyControl@@$1?_GUID_820f501c_4f39_11d2_9b70_00104b972fe8@@3U__s_GUID@@A@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x004D0440
// ??0?$VectorClass@PAVObjectClass@@@@QAE@HPBQAVObjectClass@@@Z

// TEMPLATE: GAMEMD 0x004D0490
// ?_QueryInterface@?$_com_ptr_t@V?$_com_IIID@UIFlyControl@@$1?_GUID_820f501c_4f39_11d2_9b70_00104b972fe8@@3U__s_GUID@@A@@@@AAEJABQAUIDispatch@@@Z

template class _com_ptr_t<_com_IIID<IFlyControl, &__uuidof(IFlyControl)> >;

// FUNCTION: GAMEMD 0x004D0510
// LocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x004D0520
// LocomotionClass::AddRef

// FUNCTION: GAMEMD 0x004D0530
// LocomotionClass::Release

// SYNTHETIC: GAMEMD 0x004D0540 SYMBOL
// _$E13
