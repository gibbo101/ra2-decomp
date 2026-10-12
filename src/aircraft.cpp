// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "aircraft.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "airctype.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "ccrand.h"
#include "cell.h"
#include "dbgprint.h"
#include "findmake.h"
#include "house.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "map.h"
#include "mono.h"
#include "partsys.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "team.h"
#include "tracker.h"
#include "unit.h"
#include "waypoint.h"
#include "weapon.h"

// FUNCTION: 0x00413CD0
bool Counts_As_Civ_Evac(ObjectClass const * candidate)
{
	if (candidate == NULL) return(false);

	InfantryClass const * inf = (candidate->RTTI == RTTI_INFANTRY) ? (InfantryClass const *)candidate : NULL;

	if (inf == NULL) return(false);

	if (!inf->Class->IsCivilian) return(false);

	if (inf->IsTechnician) return(false);

	return(true);
}

// FUNCTION: 0x00413D20
AircraftClass::AircraftClass(AircraftTypeClass const * type, HouseClass * house) :
	BASECLASS(house),
	Class((AircraftTypeClass *)type),
	IsToSpendAmmo(false),
	Passenger(false),
	IsKamikaze(false),
	DockNowHeadingTo(NULL),
	unknown_bool_6D0(false),
	unknown_bool_6D1(false),
	IsLockedStraight(false),
	NumParadropsLeft(5),
	IsCarryallNotLanding(true),
	IsReturningFromAttackRun(true)
{
	Create_ID();

	if (Class != NULL) {
		Locomotion.CreateInstance(Class->Locomotor, NULL, CLSCTX_ALL);
		Locomotion->Link_To_Object(this);
	}

	Init();

	Aircraft.Add(this);

	TargetTracker.Add_Index(Fetch_ID(), this);
}

// FUNCTION: 0x00413F80
void AircraftClass::Init(void)
{
	BASECLASS::Init();
	if (Class != NULL) {
		if (House != NULL) {
			TypeList<AircraftTypeClass *> & list = House->Class->VeteranAircraft;
			for (int index = 0; index < list.Count(); index++) {
				if (list[index] == Class) {
					Veterancy.Set_Veteran(true);
					break;
				}
			}
		}
		PrimaryFacing.Set_ROT(Class->ROT);
		SecondaryFacing.Set_ROT(Class->ROT);
		SecondaryFacing.Set(PrimaryFacing.Current());
		Set_Height_AGL(Class->GetFlightLevel());
		Ammo = (Class->InitialAmmo == -1) ? Class->MaxAmmo : Class->InitialAmmo;
		EstimatedHealth = Strength = Class->MaxStrength;
	}

	if (House != NULL) {
		House->Tracking_Add(this);
	}
}

// FUNCTION: 0x00414080
AircraftClass::~AircraftClass(void)
{
	if (GameActive && Class) {
		if (House->Can_Build(Class, false, false) == -1) {
			House->IsRecalcNeeded = true;
		}

		Kill_Cargo(NULL);

		if (Team) {
			Team->Remove(this);
			Team = NULL;
		}

		House->Tracking_Remove(this);

		AircraftClass::Limbo();
		Class = NULL;
	}

	Detach_This_From_All(this);
	Aircraft.Delete(this);
	TargetTracker.Remove_Index(Fetch_ID());
	IsActive = false;
}

// FUNCTION: 0x00414290
HRESULT STDMETHODCALLTYPE AircraftClass::QueryInterface(struct _GUID const &guid, void **ppv)
{
	HRESULT res = BASECLASS::QueryInterface(guid, ppv);
	if (FAILED(res)) {
		if (guid == IID_IFlyControl) {
			*ppv = (IFlyControl *)(this);
		}
		res = S_OK;
		AddRef();
	}
	return(res);
}

// FUNCTION: 0x004142F0
ULONG STDMETHODCALLTYPE AircraftClass::AddRef(void)
{
	return(BASECLASS::AddRef());
}

// FUNCTION: 0x00414300
ULONG STDMETHODCALLTYPE AircraftClass::Release(void)
{
	return(BASECLASS::Release());
}

// FUNCTION: 0x00414310
bool AircraftClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	Coord ucoord = coord;
	if (!Class->MissileSpawn) {
		if (!IsALoaner && Map.In_Local_Radar(coord)) {
			ucoord.Z = Map.Get_Height_GL(coord);
		} else {
			ucoord.Z = Map.Get_Height_GL(coord) + Class->GetFlightLevel();
		}
	}

	if (BASECLASS::Unlimbo(ucoord, dir)) {

		if (!Class->IsSelectable || !Class->IsLandable ||
			(Get_Class_Weapon_Data(0)->Weapon != NULL && Get_Class_Weapon_Data(0)->Weapon->IsCamera)) {
			IsALoaner = true;
		}

		if (Cargo.Is_Something_Attached()) {
			Passenger = true;
		}

		SecondaryFacing.Set(dir);

		Animation.Set_Rate(1);
		Animation.Set_Stage(0);

		if (HeightAGL == Class->GetFlightLevel()) {
			Set_Speed(1);
		} else {
			Set_Speed(0);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004144B0
void AircraftClass::Draw_It(Point2D const & xpoint, Rect const & cliprect) const
{
	if (!Debug_Map && MainWindow && Scen->Special.IsFogOfWar) {
		Coord headto = (Coord)Locomotion->Head_To_Coord();
		headto.Z = Get_Coord().Z;
		if (Map.Is_Fogged(headto) && Map.Is_Fogged(Get_Coord()) && !House->Is_Player_Control()) {
			return;
		}
	}

	ObjectTypeClass const * type = Class;
	if (!IsClearlyVisibleTo(PlayerPtr)) {
		type = GetDisguise(true);
	}
	int frame = TotalFramesWalked % type->Voxel.MotLib->Get_Frame_Count();

	Point2D point = xpoint;
	point += Locomotion->Draw_Point();

	if (Cargo.Is_Something_Attached() && Class->IsCarryall) {
		Cargo.Attached_Object()->Draw_It(point, cliprect);
	}

	Cell cell = Get_Cell();

	if (Class->IsVoxel && Class->Voxel.VoxLib != NULL) {
		Coord coord = Position;
		coord.Z = Map.Get_Height_GL(coord);

		Point2D shadow;
		TacticalMap->Coord_To_Pixel(coord, shadow);
		shadow += Locomotion->Shadow_Point();

		int key = 0;
		int height = HeightAGL;
		if (key != -1) {
			key = (key << 5) | (frame & 0x1f);
		}

		bool occupies_cell = Occupies_Cells();
		((AircraftClass &)*this).IsOccupyingCell = false;

		CellClass * cellptr = &Map[Get_Coord()];

		bool draw_on_ground = false;
		if ((IsOnBridge || height < BRIDGE_LEPTON_HEIGHT) && (!IsOnBridge || height < 0)) {
			draw_on_ground = true;
		} else {
			if (cellptr->IsUnderBridge &&
				(cellptr->IsBridgeEastWest && cellptr->Adjacent_Cell(FACING_N).IsUnderBridge ||
				!cellptr->IsBridgeEastWest && cellptr->Adjacent_Cell(FACING_W).IsUnderBridge)) {

				((AircraftClass &)*this).HeightAGL = BRIDGE_LEPTON_HEIGHT;
				shadow.Y -= TacticalMap->Z_Lepton_To_Pixel(BRIDGE_LEPTON_HEIGHT);
			} else {
				draw_on_ground = true;
			}
		}

		if (draw_on_ground) {
			((AircraftClass &)*this).HeightAGL = 0;
		}

		Matrix3D matrix;
		matrix = Locomotion->Shadow_Matrix(&key);
		if (!IsSinking) {
			Draw_Voxel_Shadow(Class->Voxel, Class->ShadowIndex, key, &Class->ShadowVoxelIndex, cliprect, shadow, Get_Isometric_View_Matrix() * matrix, true, NULL, 0, 0);
		}

		((AircraftClass &)*this).HeightAGL = height;
		((AircraftClass &)*this).IsOccupyingCell = occupies_cell;

		TacticalMap->Add_To_Selectables((AircraftClass *)this, point);

		int brightness;
		if (IonStormClass::Is_Ion_Storm_Active()) {
			brightness = Scen->IonLighting.LevelLight;
		} else {
			brightness = Scen->NormalLighting.LevelLight;
		}

		brightness *= (HeightAGL / (2 * LEVEL_LEPTON_H));
		int newbrightness = brightness + Map[coord].Brightness + Rule->ExtraAircraftLight;

		key = frame & 0x1f;
		matrix = Locomotion->Draw_Matrix(&key);
		Draw_A_VXL(Class->Voxel, frame, key, &Class->VoxelIndex, cliprect, point, Get_Isometric_View_Matrix() * matrix, newbrightness, SHAPE_NORMAL, 0);
	}

	BASECLASS::Draw_It(xpoint, cliprect);
}

// FUNCTION: 0x00414A10
void AircraftClass::Draw_Rotors(Point2D const & xy, Rect const & cliprect) const
{
}

// FUNCTION: 0x00414A20
bool AircraftClass::FUN_00414A20(MissionType mission) const
{
	switch (mission) {
		case MISSION_RETREAT:
		case MISSION_PARADROP_APPROACH:
		case MISSION_PARADROP_OVERFLY:
		case MISSION_SPYPLANE_APPROACH:
		case MISSION_SPYPLANE_OVERFLY:
			return(Airstrike != 0) ? true : false;

		default:
			return(true);
	}
}

// FUNCTION: 0x00414A80
int AircraftClass::Do_MISSION_HUNT(void)
{
	if (!Ammo) {
		if (Team) Team->Remove(this);
		Enter_Idle_Mode();
	} else {
		if (TarCom == NULL) {
			if (Session.Type != GAME_NORMAL) {
				Assign_Target(Greatest_Threat(THREAT_TIBERIUM, Get_Coord(), false));
			}
			if (TarCom == NULL) {
				Assign_Target(Greatest_Threat(THREAT_NORMAL, Get_Coord(), false));
			}
			if (TarCom == NULL) {
				Enter_Idle_Mode();
				return(1);
			}
		}

		QueueMission(MISSION_ATTACK, false);
		return(1);
	}
	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}
// FUNCTION: 0x0041B5C0
HRESULT STDMETHODCALLTYPE AircraftClass::Save(IStream * stream, int cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0041C090 FootClass::Draw_Object

// FUNCTION: 0x0041C190
HRESULT STDMETHODCALLTYPE AircraftClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AircraftClass;
	return(S_OK);
}
