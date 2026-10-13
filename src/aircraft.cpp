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

extern int __fastcall FUN_0050C170(char const * name);

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
	int dist = Planar_Distance(TarCom);
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
	int dist = Planar_Distance(TarCom);
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

	if (Planar_Distance(TarCom) <= Rule->ParadropRadius) {
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

// FUNCTION: 0x00416D50
int AircraftClass::Do_MISSION_MOVE_Carryall(void)
{
	enum {
		VALIDATE_LZ,
		TAKE_OFF,
		FLY_TO_LZ,
		LAND
	};

	TechnoClass * target;

	switch (Status) {
		case VALIDATE_LZ:
			DebugString("Do_MISSION_MOVE_Carryall - VALIDATE_LZ\n");
			if (NavCom == NULL) {
				DebugString("Do_MISSION_MOVE_Carryall - VALIDATE_LZ - NavCom == NULL\n");
				Enter_Idle_Mode();
				break;
			}

			target = Dynamic_Cast<TechnoClass *>(NavCom);
			if (target != NULL &&
				!Cargo.Is_Something_Attached() &&
				House->Is_Ally(target) &&
				(!target->Is_Techno() || target->Owner_HouseClass()->Is_Ally(this))
				&& target->RTTI == RTTI_UNIT) {
				DebugString("Do_MISSION_MOVE_Carryall - VALIDATE_LZ - target != NULL\n");
				if (FUN_0065AD30(0) != target) {
					Transmit_Message(RADIO_OVER_OUT);
				}
				if (Transmit_Message(RADIO_HELLO, target) == RADIO_ROGER) {
					DebugString("Do_MISSION_MOVE_Carryall - VALIDATE_LZ - RADIO_HELLO got RADIO_ROGER\n");
					if (Transmit_Message(RADIO_WANT_RIDE) != RADIO_ROGER) {
						DebugString("Do_MISSION_MOVE_Carryall - VALIDATE_LZ - RADIO_WANT_RIDE did not get RADIO_ROGER\n");
						Transmit_Message(RADIO_OVER_OUT);
						Enter_Idle_Mode();
						break;
					} else {
						Transmit_Message(RADIO_HOLD_STILL);
					}
				} else {
					Assign_Destination(NULL);
					Enter_Idle_Mode();
					break;
				}
			}
			Assign_Destination(New_LZ(NavCom));
			if (Team != NULL) {
				Team->Assign_Mission_Target(NavCom);
			}
			Status = TAKE_OFF;
			DebugString("Do_MISSION_MOVE_Carryall - VALIDATE_LZ - Status = TAKE_OFF\n");
			break;

		case TAKE_OFF:
			if (NavCom == NULL) {
				Enter_Idle_Mode();
				return(1);
			}
			Locomotion->Move_To(NavCom->Center_Coord());
			Status = FLY_TO_LZ;
			return(1);

		case FLY_TO_LZ:
			if (NavCom != NULL && !Is_Target_Cell(NavCom) && FUN_0065AD30(0) != NavCom) {
				DebugString("Do_MISSION_MOVE_Carryall - FLY_TO_LZ - Lost contact\n");
				Status = VALIDATE_LZ;
				return(1);
			}

			if (Is_Target_Cell(NavCom) && !Is_LZ_Clear(NavCom)) {
				Status = VALIDATE_LZ;
				return(1);
			}

			if (Locomotion->Get_Status() == 1) {
				DebugString("Do_MISSION_MOVE_Carryall - FLY_TO_LZ - Begin landing\n");
				if (NavCom != NULL && NavCom->RTTI == RTTI_UNIT) {
					UnitClass *unit = ((UnitClass *)NavCom);
					if (unit->PositionCoord.As_Cell() != PositionCoord.As_Cell()) {
						Status = VALIDATE_LZ;
						DebugString("Do_MISSION_MOVE_Carryall - FLY_TO_LZ - Target moved\n");
						return(1);
					} else {
						IsCarryallNotLanding = false;
					}
				} else {
					IsCarryallNotLanding = false;
				}
			}
			if (!Locomotion->Is_Moving()) {
				DebugString("Do_MISSION_MOVE_Carryall - FLY_TO_LZ - Stopped moving\n");
				Status = LAND;
			}
			return(1);

		case LAND:
			DebugString("Do_MISSION_MOVE_Carryall - LAND\n");
			if (Cargo.Is_Something_Attached() && QueueUpToEnter != NULL) {
				IsCarryallNotLanding = true;
			} else if (Cargo.Is_Something_Attached()) {
				Mark(MARK_UP);
				Drop_Off_Cargo();
				Mark(MARK_DOWN);
				Status = VALIDATE_LZ;
			} else {
				DebugString("Do_MISSION_MOVE_Carryall - LAND - Picking up cargo\n");
				Mark(MARK_UP);
				UnitClass * unit = Map[(Coord const &)PositionCoord].Cell_Unit(Map[(Coord const &)PositionCoord].IsUnderBridge);
				if (unit != NULL && unit == FUN_0065AD30(0)) {
					DebugString("Do_MISSION_MOVE_Carryall - LAND - Got Cell_Unit\n");
					if (Transmit_Message(RADIO_NEED_TO_MOVE) == RADIO_ROGER) {
						DebugString("Do_MISSION_MOVE_Carryall - LAND - RADIO_NEED_TO_MOVE got RADIO_ROGER\n");
						unit->Limbo();
						unit->IsOnBridge = false;
						unit->IsInTransport = true;
						Cargo.Attach(unit);
					} else {
						Status = VALIDATE_LZ;
					}
				} else {
					Status = VALIDATE_LZ;
				}
				Transmit_Message(RADIO_OVER_OUT);
				Mark(MARK_DOWN);
				IsCarryallNotLanding = true;
				Enter_Idle_Mode();
			}
			return(1);
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// The cell holding the landing point an aircraft uses to reach the target.
static inline Cell LZ_Cell(AbstractClass * navcom, AircraftClass * aircraft)
{
	return Cell(navcom->Destination_Coord(aircraft));
}

// FUNCTION: 0x00417300
int AircraftClass::Do_MISSION_PATROL(void)
{
	enum {
		VALIDATE_LZ,
		TAKE_OFF,
		FLY_TO_LZ,
		IDLE,
		LAND,
	};

	unknown_bool_430 = true;

	switch (Status) {
		case VALIDATE_LZ:
			if (NavCom == NULL) {
				Enter_Idle_Mode();
			} else {
				Assign_Destination(New_LZ(NavCom));
				if (Team != NULL) {
					Team->Assign_Mission_Target(NavCom);
				}
				Status = TAKE_OFF;
			}
			break;

		case TAKE_OFF:
			if (NavCom == NULL) {
				Enter_Idle_Mode();
				return(1);
			} else {
				Locomotion->Move_To(NavCom->Destination_Coord(this));
				Status = FLY_TO_LZ;
				return(1);
			}
			break;

		case FLY_TO_LZ:
			if (Ammo == 0) {
				BuildingClass * building = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Class->Dock, 0, 0);
				if (building != NULL) {
					Override_Mission(MISSION_ENTER, NULL, building);
					Status = VALIDATE_LZ;
					return(1);
				}
			}

			if (Ammo != 0) {
				AbstractClass * found = Greatest_Threat(THREAT_AREA, PositionCoord, false);
				ObjectClass * threat = NULL;
				if (found != NULL && (found->AbstractFlags & 2)) {
					threat = (ObjectClass *)found;
				}
				if (threat != NULL) {
					Override_Mission(MISSION_ATTACK, threat, NULL);
					Status = VALIDATE_LZ;
					return(1);
				}
			}

			if (CurrentPath != PATH_NONE) {
				bool proceed_to_wp = false;
				if (NavCom != NULL) {
					Coord tar_coord = NavCom->Center_Coord();
					Coord diff = PositionCoord - (Point2D)tar_coord;
					proceed_to_wp = ((Point2D)diff).Length() < CELL_LEPTON;
				} else {
					proceed_to_wp = true;
				}
				if (proceed_to_wp) {
					WaypointClass * wp = PlayerPtr->Paths[CurrentPath]->Get_Waypoint(NextWaypoint);
					WaypointClass * nwp = PlayerPtr->Paths[CurrentPath]->Get_Next_Waypoint(wp);
					Execute_Waypoint_Path(nwp);
				}
			}

			if (!Locomotion->Is_Moving()) {
				Status = IDLE;
			} else {
				if (NavCom != NULL && !Cell_Seems_Ok(LZ_Cell(NavCom, this), true)) {
					Status = VALIDATE_LZ;
				} else {
					Status = LAND;
				}
			}
			return(1);

		case IDLE:
			Enter_Idle_Mode();
			return(1);

		case LAND:
			if (!Locomotion->Is_Moving()) {
				Status = IDLE;
			} else {
				if (NavCom != NULL) {
					if (!Cell_Seems_Ok(LZ_Cell(NavCom, this), true)) {
						Status = VALIDATE_LZ;
					}
				}
			}
			return(1);

		default:
			break;
	}

	return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
}

// FUNCTION: 0x004176F0
bool AircraftClass::Enter_Idle_Mode(bool initial, bool resume_waypoint)
{
	if (Has_Suspended_Mission()) {
		Restore_Mission();
		if (CurrentMission == MISSION_PATROL) {
			Status = 0;
			IsLockedStraight = false;
		}
		return(false);
	}

	switch (CurrentMission) {
		case MISSION_RETREAT:
		case MISSION_PARADROP_APPROACH:
		case MISSION_PARADROP_OVERFLY:
		case MISSION_SPYPLANE_APPROACH:
		case MISSION_SPYPLANE_OVERFLY:
			if (Airstrike == NULL) {
				return(false);
			}
			goto proceed;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 28:
		case 29:
			goto proceed;
	}
proceed:

	if (MissionQueue == MISSION_PARADROP_APPROACH || MissionQueue == MISSION_SPYPLANE_APPROACH) {
		Commence();
		return(false);
	}

	bool result = BASECLASS::Enter_Idle_Mode(initial, resume_waypoint);

	MissionType mission = (House->Is_Human_Player() || Team || !Is_Weapon_Equipped()) ? MISSION_GUARD : MISSION_GUARD_AREA;

	int landingalt = Landing_Altitude();
	if (In_Which_Layer() == LAYER_GROUND || Get_Height_AGL() <= landingalt || Class->MissileSpawn) {
		if (IsALoaner) {
			if (Cargo.Is_Something_Attached()) {
				if (Team != NULL) {
					mission = MISSION_GUARD;
				} else {
					mission = MISSION_UNLOAD;
				}
			} else if (Team == NULL) {
				Assign_Target(NULL);
				Assign_Destination(NULL);
				mission = MISSION_RETREAT;
			}
		} else {
			Assign_Destination(NULL);
			Assign_Target(NULL);
			if (!House->Is_Human_Player() && Team == NULL && Is_Weapon_Equipped()) {
				mission = MISSION_GUARD_AREA;
			} else {
				mission = MISSION_GUARD;
			}
		}
	} else {
		if (Cargo.Is_Something_Attached()) {
			if (IsALoaner) {
				if (Team != NULL) {
					mission = MISSION_GUARD;
				} else {
					mission = MISSION_UNLOAD;
					Assign_Destination(Good_LZ());
				}
			} else {
				Assign_Destination(Good_LZ());
				mission = MISSION_MOVE;
			}
		} else {
			if ((IsALoaner && House->Is_Human_Player()) || (!House->Is_Human_Player() && !Class->MaxAmmo)) {
				if (Team != NULL && Team->Has_Entered_Map()) {
					Team->Remove(this, -1, false);
				}
			}

			if (Get_Class_Weapon_Data(0)->Weapon != NULL) {
				if (IsALoaner) {
					if (Ammo == 0) {
						if (Team != NULL) {
							Team->Remove(this, -1, false);
						}
						Assign_Target(NULL);
						Assign_Destination(NULL);
						mission = MISSION_RETREAT;
					} else {
						if (Team == NULL) {
							mission = MISSION_HUNT;
						}
					}
				} else if ((Ammo != 0 && TarCom != NULL && Get_Mission() == MISSION_ATTACK) || MissionQueue == MISSION_ATTACK) {
					mission = MISSION_ATTACK;
				} else if (In_Air()) {
					if (NavCom == NULL || CurrentMission != MISSION_ENTER) {
						if (Class->Dock.Count() > 0 && (IsInPlayfield || Team == NULL)) {
							BuildingClass * building = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Class->Dock, 0, 0);
							Assign_Destination(NULL);
							if (building != NULL && Transmit_Message(RADIO_HELLO, building) == RADIO_ROGER) {
								Assign_Destination(building);
								mission = MISSION_ENTER;
							} else if (Class->IsAirportBound) {
								Crash(NULL);
								return(false);
							} else {
								Assign_Destination(Good_LZ());
								mission = MISSION_MOVE;
							}
						}
					}
				}
			} else {
				if (Team != NULL) {
					return(false);
				}
				Assign_Destination(Good_LZ());
				mission = MISSION_MOVE;
			}
		}
	}

	if (Ammo == 0 && Is_Weapon_Equipped() && !HasAnyLink()) {
		BuildingClass * building = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Class->Dock, 0, 0);
		if (building != NULL) {
			Assign_Destination(building);
			Assign_Target(NULL);
			mission = MISSION_ENTER;
		} else if (Class->IsAirportBound) {
			Crash(NULL);
			return(false);
		}
	}
	if (HasAnyLink()) {
		mission = MISSION_ENTER;
	}

	QueueMission(mission, false);
	if (Ready_To_Next_Mission()) {
		Commence();
	}

	return(result);
}

// FUNCTION: 0x00417BD0
bool AircraftClass::Active_Click_With(ActionType action, ObjectClass * object, bool is_waypoint)
{
	action = What_Action(object, is_waypoint);

	switch (action) {
		case ACTION_TOTE:
			return(Player_Assign_Mission(MISSION_MOVE, NULL, object));

		case ACTION_NOMOVE:
			return(false);

		case ACTION_ENTER:
			return(Player_Assign_Mission(MISSION_ENTER, NULL, object));

		case ACTION_SELF:
			return(Player_Assign_Mission(MISSION_UNLOAD, NULL, NULL));

		default:
			break;
	}

	return(BASECLASS::Active_Click_With(action, object, is_waypoint));
}

// FUNCTION: 0x00417C80
bool AircraftClass::Active_Click_With(ActionType action, Cell const & cell, Cell const & cell2, bool is_waypoint)
{
	return(BASECLASS::Active_Click_With(action, cell, cell2, is_waypoint));
}

// FUNCTION: 0x00417CA0
bool AircraftClass::Player_Assign_Mission(MissionType mission, AbstractClass * target, AbstractClass * destination, AbstractClass * nearby)
{
	return(BASECLASS::Player_Assign_Mission(mission, target, destination, nearby));
}

// FUNCTION: 0x00417CC0
ActionType AircraftClass::What_Action(ObjectClass const * target, bool disallow_force) const
{
	ActionType action = BASECLASS::What_Action(target, disallow_force);

	if (action == ACTION_TOGGLE_SELECT) {
		return(action);
	}

	if (action == ACTION_GUARD_AREA && Class->IsAirportBound) {
		action = ACTION_NONE;
	}

	if (Class->IsCarryall && House->Is_Player_Control()) {
		if (action == ACTION_SELECT || action == ACTION_NONE) {
			if (House->Is_Ally(target)) {
				if (!target->Is_Techno() || target->Owner_HouseClass()->Is_Ally(this)) {
					if (!Cargo.Is_Something_Attached() && target->RTTI == RTTI_UNIT) {
						action = ACTION_TOTE;
					}
				}
			}
		}
	}

	if (action == ACTION_NONE) {
		action = What_Action(target->Center_Coord().As_Cell(), false, disallow_force);
	}

	if (action == ACTION_SELF) {
		if (!Cargo.How_Many()) {
			action = ACTION_NONE;
		}
	} else if (action == ACTION_ATTACK && Get_Class_Weapon_Data(0)->Weapon == NULL) {
		action = ACTION_NONE;
	}

	if (House->Is_Player_Control() && action == ACTION_SELECT && target->RTTI == RTTI_BUILDING) {
		BuildingClass * building = (BuildingClass *)target;
		if (building->Class->IsCanUnitRepair) {
			if (Class->Dock.Is_In_List(building->Class) && building->HasFreeLink(this) && !building->Cargo.Is_Something_Attached() && ((AircraftClass *)this)->Transmit_Message(RADIO_CAN_LOAD, building) == RADIO_ROGER) {
				action = ACTION_ENTER;
			} else {
				action = ACTION_NO_ENTER;
			}
		}
	}

	if (Class->IsCarryall && action == ACTION_TOTE) {
		Cell cell = target->PositionCell;
		if (cell != CELL_NONE) {
			BuildingClass * building = (BuildingClass *)Map[cell].Cell_Building();
			if (building != NULL && building->Class->IsWeaponsFactory) {
				action = ACTION_NONE;
			}
		}
	} else if (action == ACTION_NOMOVE) {
		if (target->IsDisguised() && target->GetDisguiseHouse(true) == NULL && target->GetDisguise(true)->Fetch_RTTI() == RTTI_OVERLAYTYPE) {
			action = ACTION_MOVE;
		}
	} else if (action == ACTION_ATTACK) {
		if (target->Class_Of()->IsImmune) {
			action = ACTION_NOMOVE;
		}
	}

	return(action);
}

// FUNCTION: 0x00417F80
ActionType AircraftClass::What_Action(Cell const & cell, bool check_fog, bool disallow_force) const
{
	if (!House->Is_Player_Control()) {
		return(ACTION_NONE);
	}

	ActionType action = BASECLASS::What_Action(cell, check_fog, disallow_force);

	if (action == ACTION_ATTACK && Get_Class_Weapon_Data(0)->Weapon == NULL) {
		action = ACTION_NONE;
	}

	return(action);
}

// FUNCTION: 0x00417FD0
Dir256 AircraftClass::Pose_Dir(void) const
{
	return(Rule->PoseDir);
}

// FUNCTION: 0x00418E20
AbstractClass * AircraftClass::New_LZ(AbstractClass * oldlz) const
{
	if (oldlz != NULL && (Team == NULL || !Team->Is_Leaving_Map()) && (!Is_LZ_Clear(oldlz) || !Cell_Seems_Ok(oldlz->Destination_Coord(NULL).As_Cell(), false))) {
		Coord coord = oldlz->Center_Coord();

		if (ScenarioInit) {
			return(oldlz);
		}

		for (int radius = 0; radius < Rule->unknown_int_1478 / CELL_LEPTON_W; radius++) {
			FacingType modifier = Random_Pick(FACING_N, FACING_NW);
			Cell lastcell(0, 0);

			for (FacingType facing = FACING_N; facing < FACING_COUNT; facing++) {
				Cell newcell = Move_Cell(Cell(coord), DirType(Facing_Dir(FacingType(facing + modifier))).As_Int() & -(DIR_STEP_8 << 8), radius);
				if (Map.In_Local_Radar(newcell)) {
					AbstractClass * newtarget = &Map[newcell];

					if (newcell != lastcell && Is_LZ_Clear(newtarget) && Cell_Seems_Ok(newcell, false)) {
						return(newtarget);
					}
					lastcell = newcell;
				}
			}
		}
	}
	return(oldlz);
}

// FUNCTION: 0x004190B0
RadioMessageType AircraftClass::Receive_Message(RadioClass * from, RadioMessageType message, int & param)
{
	AbstractClass * target;

	switch (CurrentMission) {
		case MISSION_RETREAT:
		case MISSION_PARADROP_APPROACH:
		case MISSION_PARADROP_OVERFLY:
		case MISSION_SPYPLANE_APPROACH:
		case MISSION_SPYPLANE_OVERFLY:
			if (Airstrike == NULL) {
				return(RADIO_STATIC);
			}
			goto proceed;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 28:
		case 29:
			goto proceed;
	}
proceed:

	switch (message) {
		case RADIO_RELOAD:
			if (Ammo >= (Class->MaxAmmo / 2) && TarCom != NULL) {
				return(RADIO_ROGER);
			}
			return(BASECLASS::Receive_Message(from, message, param));

		case RADIO_PREPARED:
			if (TarCom != NULL) {
				return(RADIO_NEGATIVE);
			}
			return((Ammo != Class->MaxAmmo) ? RADIO_NEGATIVE : RADIO_ROGER);

		case RADIO_ALL_DONE:
			return((Ammo != Class->MaxAmmo) ? RADIO_NEGATIVE : RADIO_ROGER);

		case RADIO_RUN_AWAY:
			QueueMission(MISSION_MOVE, false);
			Assign_Destination(Good_LZ());
			Transmit_Message(RADIO_OVER_OUT);
			break;

		case RADIO_MOVE_HERE:
			BASECLASS::Receive_Message(from, message, param);
			target = (AbstractClass *)param;
			if (target != NULL && target->Fetch_RTTI() == RTTI_BUILDING) {
				if (Transmit_Message(RADIO_CAN_LOAD, Dynamic_Cast<TechnoClass *>(target)) != RADIO_ROGER) {
					return(RADIO_NEGATIVE);
				}
				QueueMission(MISSION_ENTER, false);
				Assign_Destination(target);
			} else {
				QueueMission(MISSION_MOVE, false);
				Assign_Destination(target);
			}
			Commence();
			return(RADIO_ROGER);

		case RADIO_NEED_TO_MOVE:
			BASECLASS::Receive_Message(from, message, param);
			if (!Locomotion->Is_Moving() || NavCom == NULL) {
				return(RADIO_ROGER);
			}
			if (Class->IsAirportBound && NavCom->Fetch_RTTI() == RTTI_CELL && NavCom == Get_Cell_Ptr()) {
				return(RADIO_ROGER);
			}
			return(RADIO_NEGATIVE);

		case RADIO_IM_IN:
			if (Cargo.How_Many() == Class->Max_Passengers()) {
				Door.Close_Door(Class->DeployTime);
			}
			if (Counts_As_Civ_Evac(from)) {
				Assign_Target(NULL);
				Assign_Destination(NULL);
				QueueMission(MISSION_RETREAT, false);
			}
			return(RADIO_ATTACH);

		case RADIO_DOCKING:
			if (Class->Max_Passengers() > 0 && Cargo.How_Many() < Class->Max_Passengers()) {
				BASECLASS::Receive_Message(from, message, param);

				if (!Locomotion->Is_Moving()) {
					Door.Open_Door(Class->DeployTime);

					if (Transmit_Message(RADIO_NEED_TO_MOVE, from) == RADIO_ROGER) {
						param = (int)this;
						if (Transmit_Message(RADIO_MOVE_HERE, param, from) != RADIO_ROGER) {
							Transmit_Message(RADIO_OVER_OUT, from);
							return(RADIO_ROGER);
						}
						FUN_0065AD30(0)->Unselect();
					}
				}
				return(RADIO_ROGER);
			}
			break;

		case RADIO_CAN_LOAD:
			if (Class->Max_Passengers() == 0 || from == NULL || !House->Is_Ally(from)) {
				return(RADIO_STATIC);
			}
			return((Cargo.How_Many() < Class->Max_Passengers()) ? RADIO_ROGER : RADIO_NEGATIVE);

		case RADIO_UNLOADED:
			if (Class->IsCarryall && Get_Mission() == MISSION_MOVE && IsTethered) {
				if ((Cargo.Is_Something_Attached() && Cargo.Attached_Object() == from) || NavCom == from) {
					return(RADIO_NEGATIVE);
				}
			}
			break;

		default:
			break;
	}

	return(BASECLASS::Receive_Message(from, message, param));
}

// FUNCTION: 0x004195A0
FacingType AircraftClass::Desired_Load_Dir(ObjectClass * object, Cell & moveto) const
{
	static FacingType _toface[FACING_COUNT] = {FACING_S, FACING_SW, FACING_SE, FACING_W, FACING_E, FACING_NW, FACING_NE, FACING_N};

	moveto = CELL_NONE;

	Cell center = Center_Coord().As_Cell();
	for (int face = FACING_N; face < FACING_COUNT; face++) {
		moveto = AdjacentCell[(int)_toface[face] & 7] + center;
		if (Map.In_Radar(moveto) && (object->Center_Coord().As_Cell() == moveto || Map[moveto].Is_Clear_To_Move(SPEED_FOOT, false, false))) {
			return(FACING_N);
		}
	}
	return(FACING_N);
}

// FUNCTION: 0x0041B110
void AircraftClass::Read_INI(CCINIClass & ini)
{
	AircraftClass * air;
	int inhouse;
	AircraftType classid;
	char buf[128];

	ini.Reset();

	int counter = ini.Entry_Count(INI_NAME);

	for (int index = 0; index < counter; index++) {
		char const * entry = ini.Get_Entry(INI_NAME, index);

		ini.Get_String(INI_NAME, entry, NULL, buf, sizeof(buf));

		inhouse = FUN_0050C170(strtok(buf, ","));
		if (inhouse != -1) {
			classid = AircraftTypeClass::From_Name(strtok(NULL, ","));

			if (classid != AIRCRAFT_NONE) {
				HouseClass * inhousep = Houses[inhouse];

				air = new AircraftClass(AircraftTypes[classid], inhousep);
				if (air) {
					int strength = atoi(strtok(NULL, ","));

					Cell cell;
					if (NewINIFormat >= 4) {
						unsigned short x = atoi(strtok(NULL, ","));
						unsigned short y = atoi(strtok(NULL, ","));
						cell = Cell(x, y);
					} else {
						int c = atoi(strtok(NULL, ","));
						cell = Cell(c);
					}

					Coord coord = cell.As_Coord();

					Dir256 dir = (Dir256)atoi(strtok(NULL, ","));

					MissionType mission = MissionClass::Mission_From_Name(strtok(NULL, ","));

					TagTypeClass * tp = TagTypeClass::From_Name(strtok(NULL, ","));
					if (tp != NULL) {
						TagClass * tt = Find_Or_Make(tp);
						if (tt != NULL) {
							air->Attach_Tag(tt);
						}
					}

					char * token = strtok(NULL, ",");
					if (token != NULL) {
						air->Veterancy.From_Integer(atoi(token));
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						air->Group = atoi(token);
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						air->IsTeamRecruitable = atoi(token) != 0;
					}

					token = strtok(NULL, ",");
					if (token != NULL) {
						air->IsAutocreateRecruitable = atoi(token) != 0;
					}

					if (air->Unlimbo(coord, dir)) {
						double fs;
						air->Strength = (fs = strength) * air->Class->MaxStrength / 256.0;
						if (air->Strength > air->Class->MaxStrength - 3) air->Strength = air->Class->MaxStrength;
						if (air->Strength == 0) air->Strength = 1;
						air->EstimatedHealth = air->Strength;
						air->QueueMission(mission, false);
						air->Commence();
					} else {
						delete air;
					}
				}
			}
		}
	}
}

// FUNCTION: 0x0041B430
HRESULT STDMETHODCALLTYPE AircraftClass::Load(IStream * stream)
{
	TargetTracker.Remove_Index(Fetch_ID());
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		ILocomotion * locomotion = Locomotion;

		new (this) AircraftClass(NoInitClass());

		Locomotion = locomotion;
		locomotion->Release();

		TargetTracker.Add_Index(Fetch_ID(), this);

		Swizzle_Pointer(&Class);
		Swizzle_Pointer(&DockNowHeadingTo);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0041B5C0
HRESULT STDMETHODCALLTYPE AircraftClass::Save(IStream * stream, int cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0041B5E0
bool AircraftClass::Ready_To_Next_Mission(void) const
{
	if (CurrentMission != MISSION_STICKY && CurrentMission != MISSION_RESCUE && (!IsLockedStraight || CurrentMission == MISSION_SPYPLANE_APPROACH)) {
		if (IsCarryallNotLanding) {
			return(true);
		}
		return(false);
	}
	return(false);
}

// FUNCTION: 0x0041B610
void AircraftClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Passenger);
	crc(unknown_bool_6D0);
	crc(unknown_bool_6D1);
	crc((int)NumParadropsLeft);
}

// FUNCTION: 0x0041B660
void AircraftClass::Detach(AbstractClass const * target, bool all)
{
	BASECLASS::Detach(target, all);
	if (DockNowHeadingTo == target) {
		DockNowHeadingTo = NULL;
	}
	if (Class == target) {
		Class = NULL;
	}
}

// FUNCTION: 0x0041B6A0
LONG STDMETHODCALLTYPE AircraftClass::Landing_Altitude(void)
{
	if (Class->IsCarryall && !Cargo.Is_Something_Attached() && HasAnyLink()) {
		TechnoClass * tptr = FUN_0065AD40(0);
		if (Get_Mission() == MISSION_ENTER) {
			if (tptr != NULL && tptr->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingClass * bptr = (BuildingClass *)tptr;
				if (bptr->Class->IsCanUnitRepair || bptr->Class->IsHelipad) {
					return(0);
				}
			}
		}
	}

	if ((HasAnyLink() && Class->IsCarryall) || (Cargo.Is_Something_Attached() && Class->IsCarryall)) {
		return(100);
	}

	return(0);
}

// FUNCTION: 0x0041B760
LONG STDMETHODCALLTYPE AircraftClass::Landing_Direction(void)
{
	if (HasAnyLink()) {
		TechnoClass * tptr = FUN_0065AD40(0);
		return(tptr->PrimaryFacing.Current().As_Dir8());
	}
	if (Cargo.Is_Something_Attached()) {
		return(SecondaryFacing.Current().As_Dir8());
	}
	return(Rule->PoseDir);
}

// FUNCTION: 0x0041B7D0
BOOL STDMETHODCALLTYPE AircraftClass::Is_Loaded(void)
{
	return(Cargo.Is_Something_Attached());
}

// FUNCTION: 0x0041B7F0
LONG STDMETHODCALLTYPE AircraftClass::Is_Strafe(void)
{
	const WeaponDataStruct * data = Get_Class_Weapon_Data(0);
	if (data == NULL) {
		return(false);
	}
	WeaponTypeClass *weapon = data->Weapon;
	if (weapon == NULL) {
		return(false);
	}
	if (weapon->Bullet->ROT > 1) {
		return(false);
	}
	if (weapon->Bullet->IsInvisible) {
		return(false);
	}
	return(true);
}

// FUNCTION: 0x0041B840
LONG STDMETHODCALLTYPE AircraftClass::Is_Fighter(void)
{
	return(Class->IsFighter != 0);
}

// FUNCTION: 0x0041B860
LONG STDMETHODCALLTYPE AircraftClass::Is_Locked(void)
{
	return(IsLockedStraight);
}

// FUNCTION: 0x0041B870
bool AircraftClass::Commence(void)
{
	if (CurrentMission != MISSION_SPYPLANE_APPROACH) {
		IsLockedStraight = false;
	}
	return(BASECLASS::Commence());
}

// FUNCTION: 0x0041B890
bool AircraftClass::Should_Delete_Off_Map(void)
{
	if ((TarCom == NULL || CurrentMission == MISSION_SPYPLANE_OVERFLY) && IsInPlayfield &&
		Get_Mission() != MISSION_PARADROP_APPROACH && Get_Mission() != MISSION_PARADROP_OVERFLY) {
		if ((Team == NULL && IsALoaner) || (Team != NULL && Team->Is_Leaving_Map())) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x0041B900
void AircraftClass::Reduce_Ammunition(void)
{
}

// FUNCTION: 0x0041B910
bool AircraftClass::IsStrange(void) const
{
	return(On_Ground());
}

// FUNCTION: 0x0041B920
bool AircraftClass::In_Air(void) const
{
	if (Class == Rule->V3Rocket.Type || Class == Rule->DMisl.Type) {
		return(Locomotion->Is_Moving_Now() != 0);
	}
	return(BASECLASS::In_Air());
}

// FUNCTION: 0x0041B980
bool AircraftClass::On_Ground(void) const
{
	if (Class == Rule->V3Rocket.Type || Class == Rule->DMisl.Type) {
		return(Locomotion->Is_Moving_Now() == 0);
	}
	return(BASECLASS::On_Ground());
}

// FUNCTION: 0x0041B9E0
bool AircraftClass::IsNotWarpingIn(void) const
{
	return(IsReturningFromAttackRun);
}

// FUNCTION: 0x0041B9F0
void AircraftClass::Set_Mission(MissionType mission)
{
	switch (CurrentMission) {
		case MISSION_RETREAT:
		case MISSION_PARADROP_APPROACH:
		case MISSION_PARADROP_OVERFLY:
		case MISSION_SPYPLANE_APPROACH:
		case MISSION_SPYPLANE_OVERFLY:
			if (Airstrike == 0) {
				switch (mission) {
					case MISSION_RETREAT:
					case MISSION_PARADROP_APPROACH:
					case MISSION_PARADROP_OVERFLY:
					case MISSION_SPYPLANE_APPROACH:
					case MISSION_SPYPLANE_OVERFLY:
						break;

					default:
						return;
				}
			}
			break;
	}
	BASECLASS::Set_Mission(mission);
}

// FUNCTION: 0x0041BA90
void AircraftClass::QueueMission(MissionType mission, bool start)
{
	switch (CurrentMission) {
		case MISSION_RETREAT:
		case MISSION_PARADROP_APPROACH:
		case MISSION_PARADROP_OVERFLY:
		case MISSION_SPYPLANE_APPROACH:
		case MISSION_SPYPLANE_OVERFLY:
			if (Airstrike == 0) {
				switch (mission) {
					case MISSION_RETREAT:
					case MISSION_PARADROP_APPROACH:
					case MISSION_PARADROP_OVERFLY:
					case MISSION_SPYPLANE_APPROACH:
					case MISSION_SPYPLANE_OVERFLY:
						break;

					default:
						return;
				}
			}
			break;
	}
	BASECLASS::QueueMission(mission, start);
}

// FUNCTION: 0x0041BB30
void AircraftClass::Override_Mission(MissionType mission, AbstractClass * tarcom, AbstractClass * navcom)
{
	switch (CurrentMission) {
		case MISSION_RETREAT:
		case MISSION_PARADROP_APPROACH:
		case MISSION_PARADROP_OVERFLY:
		case MISSION_SPYPLANE_APPROACH:
		case MISSION_SPYPLANE_OVERFLY:
			if (Airstrike == 0) {
				switch (mission) {
					case MISSION_RETREAT:
					case MISSION_PARADROP_APPROACH:
					case MISSION_PARADROP_OVERFLY:
					case MISSION_SPYPLANE_APPROACH:
					case MISSION_SPYPLANE_OVERFLY:
						break;

					default:
						return;
				}
			}
			break;
	}
	BASECLASS::Override_Mission(mission, tarcom, navcom);
}

// FUNCTION: 0x0041BBD0
BuildingClass * AircraftClass::TryNearestDockBuilding(TypeList<BuildingTypeClass *> * list, unsigned unk2, unsigned unk3) const
{
	if (Class->IsAirportBound && DockNowHeadingTo != NULL) {
		if (((AircraftClass *)this)->Transmit_Message(RADIO_CAN_LOAD, DockNowHeadingTo) != RADIO_ROGER) {
			((AircraftClass *)this)->DockNowHeadingTo = NULL;
		} else {
			return(DockNowHeadingTo);
		}
	}
	return(((AircraftClass *)this)->DockNowHeadingTo = BASECLASS::TryNearestDockBuilding(list, 0, unk3));
}

extern BulletClass * __fastcall FUN_0046B050(BulletTypeClass const * type, AbstractClass * target, TechnoClass * payback, int strength, WarheadTypeClass const * warhead, int max_speed, bool bright);

// FUNCTION: 0x0041BC30
bool AircraftClass::INoticeSink_Unknown(DWORD unknown, DWORD unknown2)
{
	if (IsAttackedByLocomotor) {
		int strength = Rule->CurrentStrengthDamage ? Strength : Class->MaxStrength;
		int damage = (int)(strength * Rule->FallingDamageMultiplier);
		if (damage < 0) damage = 0;

		TechnoClass * payback = this;
		if (BeingManipulatedBy != NULL) {
			payback = BeingManipulatedBy;
		}

		BulletClass * bullet = FUN_0046B050(Rule->DeathWeapon->Bullet, this, payback, damage, Rule->C4Warhead, 0, true);
		Coord coord = Center_Coord();
		coord.Z -= Get_Height_AGL();
		if (bullet != NULL) {
			bullet->Limbo();
			bullet->Detonate(coord);
			bullet->Release();
		}

		if (BeingManipulatedBy != NULL) {
			RegisterDestruction(BeingManipulatedBy);
		} else if (ChronoWarpedByHouse != NULL) {
			Record_The_Kill(ChronoWarpedByHouse);
		}

		coord = Center_Coord();
		bullet = FUN_0046B050(Rule->DeathWeapon->Bullet, this, payback, damage, Rule->C4Warhead, 0, true);
		if (bullet != NULL) {
			bullet->Limbo();
			bullet->Detonate(coord);
			bullet->Release();
		}

		Delete_Me();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0041BDD0 ObjectClass::Target_Coord

// FUNCTION: 0x0041BE00 ObjectClass::Render_Coord

// FUNCTION: 0x0041BE30 ObjectClass::Exit_Coord

// FUNCTION: 0x0041BE60 ObjectClass::GetDisguise

// FUNCTION: 0x0041BE70 ObjectClass::GetDisguiseHouse

// FUNCTION: 0x0041BE80 ObjectClass::Draw_Radial_Indicator

// FUNCTION: 0x0041BE90 ObjectClass::Assign_Mission

// FUNCTION: 0x0041BEA0 ObjectClass::Get_Cell

// FUNCTION: 0x0041BEE0 TechnoClass::Is_Move_Override

// FUNCTION: 0x0041BEF0 TechnoClass::Is_Ready_To_Move

// FUNCTION: 0x0041BF00 TechnoClass::Anti_Air

// FUNCTION: 0x0041BF10 TechnoClass::Anti_Armor

// FUNCTION: 0x0041BF20 TechnoClass::Anti_Infantry

// FUNCTION: 0x0041C090 FootClass::Draw_Object

// FUNCTION: 0x0041C190
HRESULT STDMETHODCALLTYPE AircraftClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AircraftClass;
	return(S_OK);
}

// FUNCTION: 0x004196B0
MoveType AircraftClass::Can_Enter_Cell(CellClass const * cell, FacingType, int cell_height, CellClass const *, bool) const
{
	if (Team && Team->Is_Leaving_Map() && !Map.In_Local_Radar(cell)) return(MOVE_OK);

	ObjectClass const * occupier = cell->Cell_Occupier();

	if (occupier == NULL ||
		!occupier->Is_Techno() ||
		((TechnoClass *)occupier)->House->Is_Ally(House) ||
		(((TechnoClass *)occupier)->Cloak != CLOAKED &&
			(ScenarioInit == 0 && (occupier->RTTI != RTTI_BUILDING || !((BuildingClass*)occupier)->Class->IsInvisible)) )
		) {

		if (!cell->Is_Clear_To_Move(SPEED_WINGED, false, false)) return(MOVE_NO);
	}

	if (Session.Type == GAME_NORMAL && IsOwnedByPlayer && !IsALoaner && Map.Is_Shrouded(cell->Center_Coord())) {
		return(MOVE_NO);
	}

	return(MOVE_OK);
}

// FUNCTION: 0x004197C0
AbstractClass * AircraftClass::Good_Fire_Location(AbstractClass * target) const
{
	if (target != NULL) {
		if (((AircraftClass *)this)->Is_Strafe()) {
			return(target);
		}

		int range = Weapon_Range(0);
		Coord tcoord = target->Center_Coord();
		Cell bestcell = CELL_NONE;
		Cell best2cell = CELL_NONE;
		int bestval = -1;
		int best2val = -1;

		Coord altcoord = COORD_NONE;
		if (target->AbstractFlags & 4) {
			AbstractClass * alttarg = ((FootClass *)target)->NavCom;
			if (alttarg != NULL) {
				altcoord = alttarg->Center_Coord();
			}
		}

		for (int r = range-CELL_LEPTON; r > CELL_LEPTON; r -= CELL_LEPTON) {
			for (int face = 0; face < DIR_MAX; face += DIR_STEP_16) {
				Coord newcoord = Move_Coord(tcoord, (Dir256)face, r);
				Cell newcell = newcoord.As_Cell();

				if (Map.In_Local_Radar(newcell) && (Session.Type != GAME_NORMAL || IsALoaner || Map[newcell].IsAltUnknown) && Cell_Seems_Ok(newcell, true)) {
					int dist;
					if (altcoord != COORD_NONE) {
						dist = Point2D(newcoord).Distance_To(Point2D(altcoord));
					} else {
						dist = Point2D(Center_Coord()).Distance_To(Point2D(newcoord));
					}
					if (bestval == -1 || dist < bestval) {
						best2val = bestval;
						best2cell = bestcell;
						bestval = dist;
						bestcell = newcell;
					}
				}
			}
			if (bestval != -1) break;
		}

		if (best2val == -1) {
			best2cell = bestcell;
		}

		if (bestval != -1) {
			if (Percent_Chance(50)) {
				return(&Map[bestcell]);
			} else {
				return(&Map[best2cell]);
			}
		}
	}
	return(NULL);
}

// FUNCTION: 0x00419B00
bool AircraftClass::Cell_Seems_Ok(Cell const & cell, bool strict) const
{
	if (!Map.In_Local_Radar(cell)) {
		return(true);
	}

	if (Class->Spawned) {
		TechnoClass * techno = Map[cell].Cell_Techno();
		if (techno != NULL) {
			if (techno->SpawnManager != NULL || techno->Techno_Type_Class()->Spawned) {
				return(true);
			}
		}
	}

	if (Class->IsAirportBound) {
		return(true);
	}

	bool is_toting = (Class->IsCarryall && NavCom != NULL && NavCom->Fetch_RTTI() == RTTI_UNIT);

	AbstractClass * astarget = &Map[cell];
	for (int index = 0; index < Feet.Count(); index++) {
		AircraftClass * air = (AircraftClass *)Feet[index];
		if (air && (!is_toting || NavCom != air) && (strict || air != this) && !air->IsInLimbo && air->IsDown) {
			if (air->PositionCell == cell || air->NavCom == astarget) {
				return(false);
			}
		}
	}
	return(true);
}

// FUNCTION: 0x0041A570
DirType AircraftClass::Fire_Direction(void) const
{
	return(SecondaryFacing.Current());
}

// FUNCTION: 0x0041A590
void AircraftClass::Scatter(Coord const & , bool, bool )
{
	if (!Current_Mission_Control().IsScatter) return;

	Enter_Idle_Mode();
}

extern bool DAT_00889ECC;
extern bool DAT_00889ECD;

// FUNCTION: 0x0041A5C0
int AircraftClass::Do_MISSION_GUARD(void)
{
	if (Get_Height_AGL() == Class->GetFlightLevel()) {
		if (Team) {
			if (NavCom != NULL) {
				IsCarryallNotLanding = true;
				QueueMission(MISSION_MOVE, false);
			}
			return(Current_Mission_Control().Normal_Delay());
		}

		if (Get_Class_Weapon_Data(0)->Weapon == NULL) {
			Assign_Destination(&Map[(Coord const &)PositionCoord]);
			QueueMission(MISSION_MOVE, false);
			return(1);
		}
		if (Team == NULL) {
			IsCarryallNotLanding = true;
			Enter_Idle_Mode();
		}
		return(1);
	}

	bool rearm;
	if (DAT_00889ECC) {
		rearm = (Ammo == 0 && Is_Weapon_Equipped());
	} else if (DAT_00889ECD) {
		rearm = (Is_Weapon_Equipped() && Ammo < Class->MaxAmmo / 2);
	} else {
		rearm = (Is_Weapon_Equipped() && Ammo < Class->MaxAmmo);
	}

	if (rearm && !HasAnyLink()) {
		BuildingClass * building = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Class->Dock, 0, 0);
		if (building != NULL) {
			QueueMission(MISSION_ENTER, false);
			Assign_Destination(building);
			Assign_Target(NULL);
			return(1);
		}
	}

	if (Ammo != -1 && Ammo < (Class->MaxAmmo / 2) && HasAnyLink()) {
		if (FUN_0065AD30(0)->Fetch_RTTI() == RTTI_BUILDING && ((BuildingClass *)FUN_0065AD30(0))->Class->IsCanUnitReload) {
			return(1);
		}
	}

	if (TarCom != NULL) {
		QueueMission(MISSION_ATTACK, false);
		return(1);
	}

	if (!Is_Weapon_Equipped()) {
		return(TICKS_PER_SECOND*3);
	}

	if (Get_Height_AGL() == 0 && !HasAnyLink()) {
		return(TICKS_PER_SECOND*3);
	}

	if (!House->Is_Human_Player()) {
		AbstractClass * target = House->Find_Juicy_Target(PositionCoord);

		if (target != NULL) {
			Assign_Target(target);
			QueueMission(MISSION_ATTACK, false);
		}
	}

	if (House->Is_Human_Player() && !In_Air()) {
		return(Current_Mission_Control().Normal_Delay() + Random_Pick(0, 2));
	}

	return(BASECLASS::Do_MISSION_GUARD());
}

// FUNCTION: 0x0041A940
int AircraftClass::Do_MISSION_GUARD_AREA(void)
{
	if (Get_Height_AGL() == Class->GetFlightLevel()) {
		if (Team == NULL) Enter_Idle_Mode();
		return(1);
	}
	if (Ammo == 0 && Is_Weapon_Equipped() && !HasAnyLink()) {
		Enter_Idle_Mode();
	}
	if (TarCom != NULL) {
		QueueMission(MISSION_ATTACK, false);
		return(1);
	}
	return(BASECLASS::Do_MISSION_GUARD_AREA());
}

// FUNCTION: 0x0041A9E0
FireErrorType AircraftClass::Can_Fire(AbstractClass * target, int which, bool ignoreRange) const
{
	FireErrorType fire = BASECLASS::Can_Fire(target, which, ignoreRange);
	if (fire != FIRE_OK) {
		return(fire);
	}

	if (Passenger && !Cargo.Is_Something_Attached()) {
		return(FIRE_AMMO);
	}

	DirType dir = SecondaryFacing.Current();
	if (!((AircraftClass *)this)->Is_Fighter()) {
		dir -= GetTargetDirection(target);
		if (DIR_STEP_32 < dir) {
			return(FIRE_FACING);
		}
	}

	return(FIRE_OK);
}

// FUNCTION: 0x0041AA80
void AircraftClass::Assign_Destination(AbstractClass * dest, bool immediate)
{
	if (dest != NULL) {
		if (dest->In_Air()) {
			dest = NULL;
		} else {
			if (dest->What_Am_I() == RTTI_BUILDING && (Get_Mission() == MISSION_ENTER || MissionQueue == MISSION_ENTER)) {
				BuildingClass * destination = (BuildingClass *)dest;

				if (!HasAnyLink()) {
					if (!destination->HasFreeLink(this)) {
						Assign_Archive_Target(dest);

						if (destination->Class->IsHelipad) {
							dest = TryNearestDockBuilding((TypeList<BuildingTypeClass *> *)&Class->Dock, 0, 0);

							Assign_Destination(NULL);

							MissionType mission = MISSION_MOVE;
							if (dest != NULL && Transmit_Message(RADIO_CAN_LOAD, (TechnoClass *)dest) == RADIO_ROGER) {
								Transmit_Message(RADIO_HELLO, (TechnoClass *)dest);
								mission = MISSION_ENTER;
							} else {
								dest = Good_LZ();
							}
							QueueMission(mission, false);
							if (Ready_To_Next_Mission()) {
								Commence();
							}
						}

						if (destination->Class->IsCanUnitRepair) {
							QueueUpToEnter = destination;
							dest = NULL;
						}
					} else {
						if (Transmit_Message(RADIO_DOCKING, (TechnoClass *)dest) != RADIO_ROGER) {
							Transmit_Message(RADIO_OVER_OUT);
							if (((BuildingClass *)dest)->Class->IsCanUnitRepair || ((BuildingClass *)dest)->Class->IsCanUnitReload) {
								Assign_Archive_Target(dest);
								dest = NULL;
							}
						}

						if (destination->Class->IsCanUnitRepair || destination->Class->IsCanUnitReload) {
							if (NavCom != NULL) {
								Assign_Archive_Target(NavCom);
							}
						} else {
							Assign_Archive_Target(dest);
						}
					}
				} else {
					TechnoClass * building = Dynamic_Cast<TechnoClass *>(dest);
					if (building != NULL) {
						if (!building->HasFreeLink(this)) {
							Assign_Archive_Target(dest);
						} else if (!building->ContainsLink(this)) {
							if (Transmit_Message(RADIO_DOCKING) == RADIO_ROGER) {
								return;
							}
							Transmit_Message(RADIO_OVER_OUT);
							if (((BuildingClass *)dest)->Class->IsCanUnitRepair || ((BuildingClass *)dest)->Class->IsCanUnitReload) {
								Assign_Archive_Target(dest);
								dest = NULL;
							}
						}
					}
				}
			}

			CellClass * cellptr = Get_Cell_Ptr();
			if (!cellptr->IsUnderBridge) {
				BuildingClass * occupier = (BuildingClass *)cellptr->Cell_Occupier();

				while (occupier != NULL) {
					if (occupier != (BuildingClass *)this && occupier->Fetch_RTTI() == RTTI_BUILDING) {
						break;
					}
					occupier = (BuildingClass *)occupier->Next;
				}

				if (occupier != NULL && (occupier->Class->IsCanUnitRepair || occupier->Class->IsCanUnitReload)) {
					if (!Locomotion->Is_Powered() && !House->IonSensitivesShouldBeOffline()) {
						Locomotion->Power_On();
					}

					if (FUN_0065AD30(0) == (TechnoClass *)occupier && dest != FUN_0065AD30(0)) {
						Transmit_Message(RADIO_OVER_OUT);
					}
				}
			}
		}
	}

	BASECLASS::Assign_Destination(dest, immediate);
}

// FUNCTION: 0x0041ADC0
LayerType AircraftClass::In_Which_Layer(void) const
{
	return(Locomotion->In_Which_Layer());
}

// FUNCTION: 0x0041ADF0
void AircraftClass::Look(bool incremental, bool dont_map)
{
	Coord coord;
	int sight_range = Class->SightRange;
	if (Get_Height_AGL() == 0) {
		sight_range = 1;
	}

	if (sight_range) {
		coord = PositionCoord;
		Map.RevealArea2(&coord, sight_range, House, incremental, dont_map, false, true, false);
		coord = PositionCoord;
		Map.RevealArea2(&coord, sight_range, House, incremental, dont_map, false, true, true);
	} else {
		if (Scen->Special.IsFogOfWar) {
			sight_range = Rule->AircraftFogReveal;
			coord = PositionCoord;
			Map.RevealArea2(&coord, sight_range, House, false, false, true, Get_Height_AGL() < (Rule->FlightLevel / 2), false);
			coord = PositionCoord;
			Map.RevealArea2(&coord, sight_range, House, false, false, true, Get_Height_AGL() < (Rule->FlightLevel / 2), true);
		}
	}
}

// FUNCTION: 0x0041AF80
void AircraftClass::Write_INI(CCINIClass & ini)
{
	ini.Clear(INI_NAME);

	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * air = Aircraft[index];
		if (air != NULL && !air->IsInLimbo && air->IsActive) {
			char uname[10];
			char buf[128];

			sprintf(uname, "%d", index);
			sprintf(buf, "%s,%s,%d,%d,%d,%d,%s,%s,%d,%d,%d,%d",
				(char const *)air->House->IniName,
				(char const *)air->Class->IniName,
				(int)(air->HealthRatio * 256),
				air->PositionCell.X,
				air->PositionCell.Y,
				air->PrimaryFacing.Current().As_Dir256(),
				MissionClass::Mission_Name(air->Get_Mission()),
				(air->Tag != NULL) ? (char const *)air->Tag->Class->IniName : "None",
				air->Veterancy.To_Integer(),
				air->Group,
				air->IsTeamRecruitable,
				air->IsAutocreateRecruitable
				);
			ini.Put_String(INI_NAME, uname, buf);
		}
	}
}
