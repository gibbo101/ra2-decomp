// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "teleport.h"
#include "foot.h"
#include "techno.h"
#include "_map.h"
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

// FUNCTION: 0x00718000
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

// FUNCTION: 0x00718080
boolean STDMETHODCALLTYPE TeleportLocomotionClass::Is_Moving(void)
{
	if (IsMoving == true) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00718090
boolean TeleportLocomotionClass::Is_Stationary(void)
{
	if (Is_Moving() == false) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x007180A0
CoordStruct STDMETHODCALLTYPE TeleportLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: 0x00718100
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

// FUNCTION: 0x00718230
void STDMETHODCALLTYPE TeleportLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	IsMoving = false;
	Unknown36 = false;
}

// FUNCTION: 0x007187A0
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

// FUNCTION: 0x007192C0
void STDMETHODCALLTYPE TeleportLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set_Desired(coord);
}

// FUNCTION: 0x00719BF0
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

// FUNCTION: 0x00719C60
HRESULT STDMETHODCALLTYPE TeleportLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TeleportLocomotion;
	return(S_OK);
}

// FUNCTION: 0x00719CA0
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

// FUNCTION: 0x00719D40
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

// FUNCTION: 0x00719E20
LayerType STDMETHODCALLTYPE TeleportLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: 0x00719E30
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
