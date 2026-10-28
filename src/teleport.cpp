// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "teleport.h"
#include "foot.h"
#include "techno.h"
#include "_map.h"
#include "geometry.h"
#include "_rules.h"
#include "house.h"
#include "rules.h"
#include "overtype.h"
#include "slavemgr.h"
#include "techtype.h"
#include "cell.h"
#include "map.h"
#include "globals.h"
#include "noinit.h"
#include <new.h>
#include <math.h>

// SYNTHETIC: GAMEMD 0x00717DF0 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x00717E20 SYMBOL
// _$E83
// SYNTHETIC: GAMEMD 0x00717E40 SYMBOL
// _$E86
// SYNTHETIC: GAMEMD 0x00717E60 SYMBOL
// _$E89
// SYNTHETIC: GAMEMD 0x00717E80 SYMBOL
// _$E92
// SYNTHETIC: GAMEMD 0x00717EA0 SYMBOL
// _$E95
// SYNTHETIC: GAMEMD 0x00717EC0 SYMBOL
// _$E98
// SYNTHETIC: GAMEMD 0x00717F00 SYMBOL
// _$E101
// SYNTHETIC: GAMEMD 0x00717F30 SYMBOL
// _$E104
// SYNTHETIC: GAMEMD 0x00717F60 SYMBOL
// _$E107
// SYNTHETIC: GAMEMD 0x00717F90 SYMBOL
// _$E110
// SYNTHETIC: GAMEMD 0x00717FA0 SYMBOL
// _$E113
// SYNTHETIC: GAMEMD 0x00717FC0 SYMBOL
// _$E116
// SYNTHETIC: GAMEMD 0x00717FE0 SYMBOL
// _$E13


// FUNCTION: GAMEMD 0x00718000
TeleportLocomotionClass::TeleportLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	LastCoords(COORD_NONE),
	IsMoving(false),
	Unknown35(false),
	Unknown36(false),
	State(0),
	Timer(),
	Piggybacker(NULL)
{
}

// FUNCTION: GAMEMD 0x00718080
boolean STDMETHODCALLTYPE TeleportLocomotionClass::Is_Moving(void)
{
	if (IsMoving == true) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00718090
boolean TeleportLocomotionClass::Is_Stationary(void)
{
	if (Is_Moving() == false) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007180A0
CoordStruct STDMETHODCALLTYPE TeleportLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x00718100
void STDMETHODCALLTYPE TeleportLocomotionClass::Move_To(CoordStruct to)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsParalyzed() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		Coord dest = to;
		if (LinkedTo->UnlimboingInfantry && LinkedTo->RTTI == RTTI_INFANTRY) {
			CellClass & cell = Map[dest];
			cell.Incoming(COORD_NONE, true, true);
		}
		FUN_00718B70(dest);
		if (LastCoords != COORD_NONE) {
			IsMoving = true;
			DestinationCoord = LastCoords;
		} else {
			LinkedTo->Assign_Destination(NULL, true);
		}
	} else {
		LinkedTo->NavCom = NULL;
	}
}

// FUNCTION: GAMEMD 0x00718230
void STDMETHODCALLTYPE TeleportLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	IsMoving = false;
	Unknown36 = false;
}

// FUNCTION: GAMEMD 0x007187A0
void TeleportLocomotionClass::Check_Landing(Coord to)
{
	ObjectClass * obj = Map[to].Cell_Occupier();
	while (obj != NULL) {
		if (obj->IsIronCurtained()) {
			int damage = LinkedTo->Techno_Type_Class()->MaxStrength;
			LinkedTo->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, NULL);
		}
		obj = obj->Next;
	}

	bool bridge = false;
	if (LinkedTo->Techno_Type_Class()->Naval) {
		if (Map[to].IsUnderBridge || Map[to].Land_Type() == LAND_ROAD) {
			bridge = true;
		}
	}

	bool powered = false;
	if (LinkedTo->Techno_Type_Class()->Speed == SPEED_HOVER) {
		powered = true;
		if (LinkedTo->Techno_Type_Class()->PoweredUnit) {
			if (!LinkedTo->House->GainedPoweredCenter(LinkedTo->Techno_Type_Class())) {
				powered = false;
			}
		}
	}

	if (Map[Cell(to)].Land_Type() == LAND_WATER && !powered && !LinkedTo->Techno_Type_Class()->Naval && LinkedTo->RTTI != RTTI_INFANTRY && !Map[to].IsUnderBridge && Map[to].Land_Type() != LAND_ROAD) {
		LinkedTo->IsSinking = true;
		LinkedTo->Stun();
		if (LinkedTo->SlaveManager != NULL) {
			LinkedTo->SlaveManager->Killed(LinkedTo->BeingManipulatedBy, LinkedTo->ChronoWarpedByHouse);
			delete LinkedTo->SlaveManager;
			LinkedTo->SlaveManager = NULL;
		}
		if (LinkedTo->BeingManipulatedBy != NULL) {
			LinkedTo->RegisterDestruction(LinkedTo->BeingManipulatedBy);
		} else if (LinkedTo->ChronoWarpedByHouse != NULL) {
			LinkedTo->Record_The_Kill(LinkedTo->ChronoWarpedByHouse);
		}
		return;
	}

	if (LinkedTo->Can_Enter_Cell(&Map[Cell(to)], FACING_NONE, -1, NULL, true) == MOVE_NO || bridge) {
		if (Map[Cell(to)].Is_Tile_With_Water() && LinkedTo->RTTI != RTTI_INFANTRY) {
			LinkedTo->IsSinking = true;
			LinkedTo->Stun();
			if (LinkedTo->SlaveManager != NULL) {
				LinkedTo->SlaveManager->Killed(LinkedTo->BeingManipulatedBy, LinkedTo->ChronoWarpedByHouse);
				delete LinkedTo->SlaveManager;
				LinkedTo->SlaveManager = NULL;
			}
		} else {
			int damage = LinkedTo->Techno_Type_Class()->MaxStrength;
			LinkedTo->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true, false, NULL);
		}
	}
}

// FUNCTION: GAMEMD 0x00718B70
bool TeleportLocomotionClass::FUN_00718B70(Coord const & coord)
{
	if (LastCoords != COORD_NONE) {
		LinkedTo->Clear_Occupy_Bit(LastCoords);
	} else {
		LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
	}

	Coord crd = coord;

	if (coord != COORD_NONE) {
		bool isbridge = Map[crd].IsUnderBridge && LinkedTo->PositionCoord.Z > 3 * LEVEL_LEPTON_H + Map.Get_Height_GL(crd);

		if (LinkedTo->Fetch_RTTI() == RTTI_INFANTRY) {
			bool any_spot = false;
			if (LinkedTo->Ready_To_Next_Mission()) {
				LinkedTo->Commence();
			}
			if (LinkedTo->Get_Mission() == MISSION_CAPTURE || LinkedTo->Get_Mission() == MISSION_EATEN || LinkedTo->Get_Mission() == MISSION_ENTER || LinkedTo->Get_Mission() == MISSION_PATROL) {
				ObjectClass * unit = (ObjectClass *)LinkedTo->NavCom;
				unit = (unit != NULL && ((AbstractClass *)unit)->Fetch_RTTI() == RTTI_UNIT) ? unit : NULL;
				if (unit != NULL && unit->PositionCell == coord.As_Cell()) {
					any_spot = true;
				}
				ObjectClass * aircraft = (ObjectClass *)LinkedTo->NavCom;
				aircraft = (aircraft != NULL && ((AbstractClass *)aircraft)->Fetch_RTTI() == RTTI_AIRCRAFT) ? aircraft : NULL;
				if (aircraft != NULL && aircraft->PositionCell == coord.As_Cell()) {
					any_spot = true;
				}
				BuildingClass * building = (BuildingClass *)LinkedTo->NavCom;
				building = (building != NULL && ((AbstractClass *)building)->Fetch_RTTI() == RTTI_BUILDING) ? building : NULL;
				if (building != NULL && Map[coord].Cell_Building() == building) {
					any_spot = true;
				}
			}

			Coord old = LastCoords;
			LastCoords = Map[crd].Closest_Free_Spot(crd, any_spot, isbridge);
			if (LinkedTo->Can_Enter_Cell(&Map[LastCoords.As_Cell()])) {
				LastCoords = COORD_NONE;
			}
			Map[old].Center_Coord();

			if (LastCoords == COORD_NONE) {
				ObjectClass * obj = (ObjectClass *)LinkedTo->NavCom;
				if (obj != NULL && (obj->AbstractFlags & 2)) {
					MZoneType mzone = LinkedTo->Techno_Type_Class()->MZone;
					Cell navcell = obj->Destination_Coord().As_Cell();
					CellClass * cellptr = &Map[LinkedTo->Destination_Coord()];
					bool onbridge = LinkedTo->Is_Moving_Onto_Bridge();
					Cell nearcell = Map.Nearby_Location(navcell, LinkedTo->Techno_Type_Class()->Speed, Map.Get_Cell_Zone(cellptr->CellID, mzone, onbridge), mzone, Map[navcell].IsUnderBridge, Point2D(1, 1), false, true);
					if (nearcell != CELL_NONE) {
						LastCoords = Map[nearcell].Closest_Free_Spot(Map[nearcell].Center_Coord(), any_spot, isbridge);
					}
				}
			}
		} else if (LinkedTo->IsImmobilized) {
			LinkedTo->Set_Occupy_Bit(LastCoords);
		} else {
			MZoneType mzone = LinkedTo->Techno_Type_Class()->MZone;
			Cell cell = coord.As_Cell();
			if (cell != CELL_NONE) {
				if (LinkedTo->Can_Enter_Cell(&Map[cell])) {
					int zone = Map.Get_Cell_Zone(cell, mzone, isbridge);
					cell = Map.Nearby_Location(cell, LinkedTo->Techno_Type_Class()->Speed, zone, mzone, isbridge);
				}
			}
			if (cell != CELL_NONE) {
				LinkedTo->Clear_Occupy_Bit(LastCoords);
				LastCoords = Coord(Map[cell].Center_Coord().As_Cell());
				LastCoords.Z = Map.Get_Height_GL(LastCoords);
				LinkedTo->Set_Occupy_Bit(LastCoords);
			}
		}
	} else {
		LastCoords = coord;
	}

	if (LastCoords != COORD_NONE) {
		LinkedTo->Set_Occupy_Bit(LastCoords);
		return(true);
	}

	LinkedTo->Set_Occupy_Bit(LinkedTo->PositionCoord);
	return(false);
}

// FUNCTION: GAMEMD 0x007192C0
void STDMETHODCALLTYPE TeleportLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set_Desired(coord);
}

#include "anim.h"
#include "bullet.h"
#include "parasite.h"
#include "unit.h"
#include "unittype.h"
#include "voc.h"

static inline void Chrono_Out_Sound(FootClass * object)
{
	int sound = object->Techno_Type_Class()->ChronoOutSound;
	if (sound != -1 || (sound = Rule->ChronoOutSound) != -1) {
		VocClass::PlayAt(sound, object->PositionCoord);
	}
}

static inline void Chrono_In_Sound(FootClass * object)
{
	int sound = object->Techno_Type_Class()->ChronoInSound;
	if (sound != -1 || (sound = Rule->ChronoInSound) != -1) {
		VocClass::PlayAt(sound, object->PositionCoord);
	}
}

// FUNCTION: GAMEMD 0x007192F0
boolean STDMETHODCALLTYPE TeleportLocomotionClass::Process(void)
{
	if (LinkedTo->WarpingOut && State == 0 && LinkedTo->unknown_280 == 0) {
		Process_Delay();
		return(false);
	}

	if (State == 0 && LinkedTo->unknown_280 != 0) {
		State = LinkedTo->unknown_280;
		return(false);
	}

	if (!LinkedTo->IsImmobilized) {
		if (State <= 0) {
			if (Is_Moving()) {
				if (LinkedTo->PositionCoord == LastCoords || LastCoords == COORD_NONE) {
					LinkedTo->Assign_Destination(NULL, true);
					Stop_Moving();
					return(false);
				}

				LinkedTo->BecomeUntargetable();
				for (int index = Bullets.Count() - 1; index >= 0; index--) {
					if (Bullets[index]->TarCom == LinkedTo) {
						Bullets[index]->LoseTarget();
					}
				}

				new AnimClass(Rule->WarpOut, LinkedTo->PositionCoord);

				int distance = (LinkedTo->Center_Coord() - LastCoords).Length();
				Timer = 0;
				if (Rule->ChronoTrigger) {
					Timer = distance / Rule->ChronoDistanceFactor;
				}
				Timer = (Timer > Rule->ChronoMinimumDelay) ? Timer : Rule->ChronoMinimumDelay;
				if (distance < Rule->ChronoRangeMinimum) {
					Timer = Rule->ChronoMinimumDelay;
				}

				LinkedTo->WarpingOut = true;
				if (LinkedTo->Fetch_RTTI() == RTTI_UNIT && ((UnitClass *)LinkedTo)->Class->Harvester) {
					Timer = 0;
					LinkedTo->WarpingOut = false;
				}

				if (LinkedTo->ParasiteEatingMe != NULL) {
					LinkedTo->ParasiteEatingMe->ParasiteImUsing->ExitUnit();
				}

				LinkedTo->Mark(MARK_UP);
				Chrono_Out_Sound(LinkedTo);

				LinkedTo->Set_Coord(LastCoords);
				Cell cellid = LastCoords.As_Cell();
				CellClass * cell = &Map[cellid];
				LinkedTo->Set_Coord(LastCoords);
				if (cell->IsUnderBridge) {
					LinkedTo->IsOnBridge = true;
				} else {
					LinkedTo->IsOnBridge = false;
				}
				LinkedTo->Set_Height_AGL(0);
				LinkedTo->Mark(MARK_DOWN);
				Chrono_In_Sound(LinkedTo);
				LinkedTo->Per_Cell_Process(PCP_END);
				Stop_Moving();
				cell->Goodie_Check(LinkedTo);
				LinkedTo->Assign_Destination(NULL, true);
				new AnimClass(Rule->WarpOut, LinkedTo->PositionCoord);
				LinkedTo->unknown_280 = 0;
				return(false);
			}
			return(false);
		}
	} else if (State == 0) {
		LinkedTo->BeingWarpedOut = true;
		Timer = 60;
		State++;
		return(false);
	}

	if (State == 1) {
		Process_Delay();
		return(false);
	}

	if (State == 2) {
		new AnimClass(Rule->WarpOut, LinkedTo->PositionCoord);
		LinkedTo->Mark(MARK_UP);
		Chrono_Out_Sound(LinkedTo);
		LinkedTo->WarpingOut = true;
		LinkedTo->IsImmobilized = false;
		LinkedTo->BeingWarpedOut = false;
		LinkedTo->IsOnBridge = false;
		bool done = FUN_00718260(LinkedTo->ChronoDestCoords, false);
		State++;
		if (done) {
			State++;
			return(false);
		}
		return(false);
	}

	if (State == 3) {
		if (FUN_00718260(LinkedTo->ChronoDestCoords, false)) {
			State++;
		}
		LinkedTo->ChronoLockRemaining = Rule->ChronoDelay;
		return(false);
	}

	if (State == 4) {
		FUN_00718260(LinkedTo->ChronoDestCoords, true);
		LinkedTo->Set_Coord(LastCoords);
		LinkedTo->Set_Height_AGL(0);
		LinkedTo->Mark(MARK_DOWN);
		State++;
		return(false);
	}

	if (State == 5) {
		LinkedTo->Set_Coord(LastCoords);
		LinkedTo->Set_Height_AGL(0);
		LinkedTo->Mark(MARK_DOWN);
		Chrono_In_Sound(LinkedTo);
		if (!Map.In_Local_Radar(LinkedTo->PositionCell)) {
			LinkedTo->IsInPlayfield = false;
		}
		if (LinkedTo->unknown_280 == 0) {
			Check_Landing(LastCoords);
		}
		if (LinkedTo->IsActive) {
			LinkedTo->Per_Cell_Process(PCP_END);
			Stop_Moving();
			LinkedTo->BeingManipulatedBy = NULL;
			LinkedTo->ChronoWarpedByHouse = NULL;
			LinkedTo->Assign_Archive_Target(NULL);
			LinkedTo->Assign_Destination(NULL, true);
			Timer = LinkedTo->ChronoLockRemaining;
			new AnimClass(Rule->WarpOut, LinkedTo->PositionCoord);
			State++;
			return(false);
		}
		return(false);
	}

	if (State == 6) {
		Process_Delay();
		return(false);
	}

	if (State == 7) {
		LinkedTo->WarpingOut = false;
		LinkedTo->Assign_Archive_Target(NULL);
		LinkedTo->Assign_Destination(NULL, true);
		IsMoving = false;
		LinkedTo->unknown_280 = 0;
		State = 0;
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00719BF0
void STDMETHODCALLTYPE TeleportLocomotionClass::Process_Delay(void)
{
	if (Timer == 0) {
		LinkedTo->WarpingOut = false;
		if (LinkedTo->TarCom == NULL) {
			LinkedTo->FUN_0070F770();
			if (!LinkedTo->FUN_00709480()) {
				LinkedTo->Enter_Idle_Mode(false, true);
			}
		}
		if (State > 0) {
			State++;
		}
	}
}

// FUNCTION: GAMEMD 0x00719C60
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeleportLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00719CA0
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TeleportLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00719D40
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::Save(IStream * stream, BOOL cleardirty)
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

// FUNCTION: GAMEMD 0x00719E20
LayerType STDMETHODCALLTYPE TeleportLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x00719E30
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
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

// FUNCTION: GAMEMD 0x00719E90
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
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

// FUNCTION: GAMEMD 0x00719EE0
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::End_Piggyback(ILocomotion ** pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (LinkedTo != NULL) {
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

// FUNCTION: GAMEMD 0x00719F30
boolean STDMETHODCALLTYPE TeleportLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && Piggybacker != NULL && !Unknown35 && !LinkedTo->IsImmobilized && State == 0 && !LinkedTo->IsAttackedByLocomotor) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00719F80
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::Piggyback_CLSID(GUID * classid)
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

// FUNCTION: GAMEMD 0x0071A090
void STDMETHODCALLTYPE TeleportLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		LinkedTo->Clear_Occupy_Bit((Coord)Head_To_Coord());
	}
}

// FUNCTION: GAMEMD 0x0071A0E0
// TeleportLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0071A0F0
// TeleportLocomotionClass::Release

// FUNCTION: GAMEMD 0x0071A100
// TeleportLocomotionClass::Is_Piggybacking

// FUNCTION: GAMEMD 0x0071A120
// TeleportLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x0071A130
// ??_GTeleportLocomotionClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x0071A160
// TeleportLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x0071A170
// TeleportLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0071A180
// TeleportLocomotionClass::Release

// FUNCTION: GAMEMD 0x0071A190
// TeleportLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x0071A1A0
// TeleportLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0071A1B0
// TeleportLocomotionClass::Release
