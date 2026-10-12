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
#include "fastmath.h"
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

int __fastcall FUN_0050DA80(HouseClass * house);
int __fastcall FUN_0050DAC0(HouseClass * house);
extern Coord DAT_00889E88;

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
// FUNCTION: 0x004151E0
int AircraftClass::Do_MISSION_UNLOAD(void)
{
	enum {
		SEARCH_FOR_LZ,
		FLY_TO_LZ,
		LAND_ON_LZ,
		UNLOAD_PASSENGERS,
		TAKE_OFF
	};

	switch (Status) {
		case SEARCH_FOR_LZ:
			if (HeightAGL == 0 && (double)PitchAngle == 0 && (NavCom == NULL || (PositionCoord == NavCom->Center_Coord()))) {
				Status = UNLOAD_PASSENGERS;
			} else {
				if (NavCom == NULL && Class->IsDropship && HeightAGL > 0) {
					BuildingClass * building = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Class->Dock, 0, 0);
					if (building != NULL) {
						Assign_Destination(building, true);
					} else {
						Assign_Destination(Good_LZ(), true);
					}
				} else if (NavCom == NULL) {
					Status = LAND_ON_LZ;
				} else if (Is_LZ_Clear(NavCom)) {
					FootClass * foot = Cargo.Attached_Object();
					if (foot != NULL && foot->Team && foot->Team->Class->Get_Origin() != CELL_NONE) {
						Assign_Destination(New_LZ(&Map[foot->Team->Class->Get_Origin()]), true);
					} else {
						Assign_Destination(New_LZ(&Map[Scen->Get_Waypoint_Cell((WAYPOINT)0x2bc)]), true);
						if (Team != NULL) {
							Team->Assign_Mission_Target(NavCom);
						}
					}
				} else {
					if (HeightAGL != Class->GetFlightLevel()) {
						Status = TAKE_OFF;
					} else {
						Status = FLY_TO_LZ;
					}
				}
			}
			break;

		case FLY_TO_LZ:
			if (!Locomotion->Is_Moving()) {
				Status = LAND_ON_LZ;
			}
			if (!Is_LZ_Clear(NavCom)) {
				Status = SEARCH_FOR_LZ;
			}
			break;

		case LAND_ON_LZ:
			if (!Locomotion->Is_Moving()) {
				Status = UNLOAD_PASSENGERS;
			}
			return(1);

		case UNLOAD_PASSENGERS:
			if (!IsTethered) {
				if (Cargo.Is_Something_Attached()) {
					if (Class->IsCarryall) {
						Mark(MARK_UP);
						Drop_Off_Cargo();
						Mark(MARK_DOWN);
					} else {
						FootClass * unit = (FootClass *)Cargo.Detach_Object();

						Map.Pick_Up(PositionCell, this);

						if (Exit_Object(unit, CELL_NONE)) {
							unit->IsInTransport = false;
						}

						Map.Place_Down(PositionCell, this);

						if (!unit->IsInTransport) {
							if (unit->Team != NULL) {
								Team->AddMember(unit, false);
							}
						}

						if (!Cargo.Is_Something_Attached()) {
							Enter_Idle_Mode();
						}
					}
				} else {
					Enter_Idle_Mode();
				}
			}
			break;

		case TAKE_OFF:
			Status = SEARCH_FOR_LZ;
			return(1);

		default:
			break;
	}
	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x004155F0
int AircraftClass::Do_MISSION_SPYPLANE_APPROACH(void)
{
	int dist = Distance(TarCom);
	if (TarCom == NULL) {
		Assign_Destination(NULL, true);
		QueueMission(MISSION_RETREAT, false);
	} else if (NavCom == NULL) {
		Assign_Destination(TarCom, true);
	} else if (dist <= Get_Class_Weapon_Data(0)->Weapon->Range) {
		vt_entry_48C(0, 0, 0, 0);
		UpdateSight(0, 0, 0, 0, Get_Class_Weapon_Data(0)->Weapon->Attack);
		Map.RevealArea3(PositionCoord, 0, LastSightRange + 3, false);
		VocClass::PlayAt(Rule->SpyPlaneCamera, PositionCoord);
	}

	if (dist <= 0x300) {
		QueueMission(MISSION_SPYPLANE_OVERFLY, false);
		IsLockedStraight = true;
		int edge = FUN_0050DAC0(House);
		Cell cell = Map.Calculated_Cell((SourceType)edge, CELL_NONE, CELL_NONE, SPEED_WINGED, true, MZONE_NORMAL);
		if (cell != CELL_NONE) {
			Assign_Destination(&Map[cell], true);
		}
	}
	return(Rule->SpyPlaneCameraFrames);
}

// FUNCTION: 0x004157C0
int AircraftClass::Do_MISSION_SPYPLANE_OVERFLY(void)
{
	int dist = Distance(TarCom);
	if (dist <= Get_Primary_Weapon()->Range) {
		vt_entry_48C(0, 0, 0, 0);
		UpdateSight(0, 0, 0, 0, Get_Class_Weapon_Data(0)->Weapon->Attack);
		Coord coord = PositionCoord;
		Map.RevealArea3(coord, 0, LastSightRange + 3, false);
	}

	if (NavCom == NULL) {
		int edge = FUN_0050DAC0(House);
		Cell cell = Map.Calculated_Cell((SourceType)edge, CELL_NONE, CELL_NONE, SPEED_WINGED, true, MZONE_NORMAL);
		if (cell != CELL_NONE) {
			Assign_Destination(&Map[cell], true);
		}
	}
	return(3);
}

// FUNCTION: 0x004158E0
int AircraftClass::Do_MISSION_PARADROP_APPROACH(void)
{
	if (TarCom == NULL) {
		Assign_Destination(NULL, true);
		QueueMission(MISSION_RETREAT, false);
		return(3);
	}

	if (NavCom == NULL) {
		Assign_Destination(TarCom, true);
		return(3);
	}

	if (Distance(TarCom) <= Rule->ParadropRadius) {
		QueueMission(MISSION_PARADROP_OVERFLY, false);
		NumParadropsLeft--;
	}
	return(3);
}

// FUNCTION: 0x00415A50
int AircraftClass::Do_MISSION_RETREAT(void)
{
	if (NavCom == NULL) {
		int edge = House->Control.Edge;
		if (edge < 0 || edge > 3) edge = FUN_0050DA80(House);
		Cell cell = Map.Calculated_Cell((SourceType)edge, CELL_NONE, CELL_NONE, SPEED_WINGED, true, MZONE_NORMAL);
		if (cell != CELL_NONE) {
			Assign_Destination(&Map[cell], true);
			return(3);
		}
	} else if (NavCom == Get_Cell_Ptr()) {
		Assign_Destination(NULL, true);
	}
	return(3);
}

// FUNCTION: 0x00415B10
int AircraftClass::Exit_Object(TechnoClass * unit, Cell)
{
	static FacingType _toface[FACING_COUNT] = {FACING_S, FACING_SW, FACING_SE, FACING_NW, FACING_NE, FACING_N, FACING_W, FACING_E};
	Cell cell(0, 0);

	FacingType face;
	for (face = FACING_N; face < FACING_COUNT; face++) {
		FacingType dir = _toface[face];
		cell = AdjacentCell[(int)dir & 7] + PositionCell;
		if (unit->Can_Enter_Cell(&Map[cell]) == MOVE_OK) break;
	}

	if (unit->Unlimbo(PositionCoord, DirType(Facing_Dir(_toface[face])).As_Dir256())) {
		unit->QueueMission(MISSION_MOVE, false);
		unit->Assign_Destination(&Map[cell]);
		if (Transmit_Message(RADIO_HELLO, unit) == RADIO_ROGER) {
			Transmit_Message(RADIO_UNLOAD);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00415C60
int AircraftClass::Paradrop_Cargo(void)
{
	FootClass * passenger = (FootClass *)Cargo.Detach_Object();
	if (passenger != NULL) {
		Ammo--;
		Coord drop = Center_Coord();
		DirType dir;
		if (Ammo & 1) {
			dir = PrimaryFacing.Current().Left_90();
		} else {
			dir = PrimaryFacing.Current().Right_90();
		}
		double rad = dir.As_Radian();
		drop = Coord((int)(fastmath::cos(rad) * 128.0 + drop.X), (int)((double)drop.Y - fastmath::sin(rad) * 128.0), drop.Z);
		CellClass * cell = &Map[drop];

		if (passenger->Can_Enter_Cell(cell) == MOVE_OK) {
			Coord sub = cell->Closest_Free_Spot(drop, false, false, false);
			if (sub != DAT_00889E88) {
				drop.X = sub.X;
				drop.Y = sub.Y;
				if (passenger->Paradrop(drop)) {
					VocClass::PlayAt(Rule->ChuteSound, PositionCoord);
					passenger->LastAdjacencyCell = Map[drop].Center_Coord().As_Cell();
					if (Team != NULL) {
						Team->Remove(passenger, -1, false);
					}
					Arm = 0;
					NumParadropsLeft = 5;
					return(0);
				}
			}
		}
		Cargo.Attach(passenger);
		passenger->Hidden();
		Ammo++;
	}
	return(0);
}

// FUNCTION: 0x004165C0
ResultType AircraftClass::Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source, bool forced, bool no_crew, HouseClass * source_house)
{
	ResultType res = BASECLASS::Take_Damage(damage, distance, warhead, source, forced, no_crew, source_house);

	switch (res) {
		case RESULT_DESTROYED:
			Death_Announcement(NULL);
			if (Class->Explosion.Count() > 0) {
				new AnimClass(Class->Explosion.Pick(Scen->RandomNumber), Target_Coord());
			}

			if (!Crash(source)) {
				Delete_Me();
			}
			break;

		case RESULT_ALREADY_DESTROYED:
			res = RESULT_ALREADY_DESTROYED;
			break;

		default:
			break;
	}
	return(res);
}

// FUNCTION: 0x004166C0
int AircraftClass::Do_MISSION_MOVE(void)
{
	if (Class->IsCarryall) {
		return(Do_MISSION_MOVE_Carryall());
	}
	return(Do_MISSION_MOVE_Normal());
}

// FUNCTION: 0x00416AF0
void AircraftClass::Drop_Off_Cargo(void)
{
	DebugString("Do_MISSION_MOVE_Carryall - LAND - Dropping off cargo\n");

	FootClass * unit = Cargo.Detach_Object();

	Coord coord = PositionCoord;
	coord.Z = Map.Get_Height_GL(coord);
	if (Map[coord].IsUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
		unit->IsOnBridge = true;
	} else {
		unit->IsOnBridge = false;
	}

	unit->Locomotion.Release();
	unit->Locomotion = ILocomotionPtr(unit->TClass->Locomotor);
	unit->Locomotion->Link_To_Object(unit);

	if (!unit->Unlimbo(coord)) {
		Cargo.Attach(unit);
	} else {
		unit->IsInTransport = false;
		unit->vt_entry_48C(0, 0, 0, 0);
		unit->UpdateSight(0, 0, 0, 0, 0);
		Map.RevealArea3(PositionCoord, unit->LastSightRange - 3, unit->LastSightRange + 3, false);
		Cell nearby = Nearby_Location();
		if (FUN_0065AD30(0) == unit) {
			Transmit_Message(RADIO_UNTETHER);
			Transmit_Message(RADIO_OVER_OUT);
		}
		if (nearby != CELL_NONE) {
			Assign_Destination(&Map[nearby]);
		} else {
			Assign_Destination(NULL);
		}
	}
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
