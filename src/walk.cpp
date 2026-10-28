// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "walk.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "building.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "noinit.h"
#include "overtype.h"
#include "rules.h"
#include "tactical.h"
#include "tube.h"
#include "unit.h"
#include "geometry.h"
#include "rect.h"
#include "layer.hh"
#include "fastmath.h"
#include "coord.h"
#include "slavemgr.h"
#include <new.h>

// SYNTHETIC: GAMEMD 0x0075A880 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x0075A8A0 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x0075A8D0 SYMBOL
// _$E99
// SYNTHETIC: GAMEMD 0x0075A8F0 SYMBOL
// _$E102
// SYNTHETIC: GAMEMD 0x0075A910 SYMBOL
// _$E105
// SYNTHETIC: GAMEMD 0x0075A930 SYMBOL
// _$E108
// SYNTHETIC: GAMEMD 0x0075A950 SYMBOL
// _$E111
// SYNTHETIC: GAMEMD 0x0075A970 SYMBOL
// _$E114
// SYNTHETIC: GAMEMD 0x0075A9B0 SYMBOL
// _$E117
// SYNTHETIC: GAMEMD 0x0075A9E0 SYMBOL
// _$E120
// SYNTHETIC: GAMEMD 0x0075AA10 SYMBOL
// _$E123
// SYNTHETIC: GAMEMD 0x0075AA40 SYMBOL
// _$E126
// SYNTHETIC: GAMEMD 0x0075AA50 SYMBOL
// _$E129
// SYNTHETIC: GAMEMD 0x0075AA70 SYMBOL
// _$E132

// FUNCTION: GAMEMD 0x0075AA90
WalkLocomotionClass::WalkLocomotionClass(void) :
	BASECLASS(),
	DestinationCoord(COORD_NONE),
	HeadToCoord(COORD_NONE),
	IsMoving(false),
	IsProcessingMovement(false),
	IsReallyMoving(false)
{
}

// FUNCTION: GAMEMD 0x0075AB00
WalkLocomotionClass::~WalkLocomotionClass(void)
{
}

// FUNCTION: GAMEMD 0x0075AB30
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving(void)
{
	return(IsMoving);
}

// FUNCTION: GAMEMD 0x0075AB40
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Now(void)
{
	if (Is_Moving() && LinkedTo->Speed > 0 && HeadToCoord != COORD_NONE) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075ABA0
CoordStruct STDMETHODCALLTYPE WalkLocomotionClass::Destination(void)
{
	if (Is_Moving()) {
		return(DestinationCoord);
	}
	return(COORD_NONE);
}

// FUNCTION: GAMEMD 0x0075AC00
CoordStruct STDMETHODCALLTYPE WalkLocomotionClass::Head_To_Coord(void)
{
	if (HeadToCoord != COORD_NONE) {
		return(HeadToCoord);
	}
	return(LinkedTo->PositionCoord);
}

// FUNCTION: GAMEMD 0x0075AC80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Process(void)
{
	IsProcessingMovement = true;
	Movement_AI(true);
	IsProcessingMovement = false;
	return(Is_Moving());
}

// FUNCTION: GAMEMD 0x0075ACB0
void STDMETHODCALLTYPE WalkLocomotionClass::Move_To(CoordStruct to)
{
	if (!LinkedTo->IsUnderEMP() && !LinkedTo->IsBeingWarpedOut() && !LinkedTo->IsWarpingIn()) {
		DestinationCoord = to;
		if ((Coord &)to != COORD_NONE) {
			if (Map[(Coord)to].IsUnderBridge) {
				DestinationCoord.Z += Tactical::Pixel_To_Z_Lepton(60);
			}
			IsMoving = true;
		} else {
			if (HeadToCoord == COORD_NONE) {
				bool was_moving = IsMoving;
				IsMoving = false;
				if (was_moving) {
					LinkedTo->vt_entry_54C();
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0075ADA0
void STDMETHODCALLTYPE WalkLocomotionClass::Stop_Moving(void)
{
	DestinationCoord = COORD_NONE;
	if (HeadToCoord == COORD_NONE) {
		IsMoving = false;
		IsReallyMoving = false;
		LinkedTo->vt_entry_54C();
	}
}

// FUNCTION: GAMEMD 0x0075AE00
void STDMETHODCALLTYPE WalkLocomotionClass::Do_Turn(DirStruct dir)
{
	LinkedTo->PrimaryFacing.Set(dir);
}

// FUNCTION: GAMEMD 0x0075AE30
void STDMETHODCALLTYPE WalkLocomotionClass::Force_Immediate_Destination(CoordStruct coord)
{
	bool was_moving = IsMoving;
	Coord head = coord;
	Mark_Head_To(head);
	if (HeadToCoord == COORD_NONE && DestinationCoord == COORD_NONE) {
		IsMoving = false;
		if (was_moving) {
			LinkedTo->vt_entry_54C();
		}
	}
}

static inline FootClass * Cast_Infantry(FootClass * foot)
{
	return (!foot) ? NULL : ((foot->Fetch_RTTI() == RTTI_INFANTRY) ? foot : NULL);
}

// FUNCTION: GAMEMD 0x0075C240
bool WalkLocomotionClass::Mark_Head_To(Coord const & coord)
{
	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Clear_Occupy_Bit(HeadToCoord);
	} else {
		LinkedTo->Clear_Occupy_Bit(LinkedTo->PositionCoord);
	}

	Coord crd = coord;
	bool any_spot = false;

	if (coord != COORD_NONE) {
		if (LinkedTo->Get_Mission() == MISSION_CAPTURE || LinkedTo->Get_Mission() == MISSION_EATEN || LinkedTo->Get_Mission() == MISSION_ENTER || LinkedTo->Get_Mission() == MISSION_GUARD_AREA || LinkedTo->Get_Mission() == MISSION_PATROL) {
			UnitClass * unit = (UnitClass *)LinkedTo->NavCom;
			unit = (unit != NULL && ((AbstractClass *)unit)->Fetch_RTTI() == RTTI_UNIT) ? unit : NULL;
			if (unit != NULL && unit->PositionCell == coord.As_Cell()) {
				any_spot = true;
			}
			AircraftClass * aircraft = (AircraftClass *)LinkedTo->NavCom;
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

		if (LinkedTo->SlaveOwner != NULL && LinkedTo->SlaveOwner->SlaveManager != NULL) {
			if (Map[coord].Cell_Techno() == LinkedTo->SlaveOwner) {
				FootClass * infantry = Cast_Infantry(LinkedTo);
				if (LinkedTo->SlaveOwner->SlaveManager->FUN_006B0880(infantry, &Map[coord])) {
					any_spot = true;
				}
			}
		}

		bool isbridge = Map[crd].IsUnderBridge && LinkedTo->PositionCoord.Z > 3 * LEVEL_LEPTON_H + Map.Get_Height_GL(crd);
		HeadToCoord = Map[crd].Closest_Free_Spot(crd, any_spot, isbridge);

		if (!Map[HeadToCoord].Goodie_Check(LinkedTo) && !LinkedTo->IsInLimbo) {
			HeadToCoord = COORD_NONE;
			if (!LinkedTo->IsActive) {
				return(false);
			}
		}
	} else {
		HeadToCoord = coord;
	}

	if (HeadToCoord != COORD_NONE) {
		LinkedTo->Set_Occupy_Bit(HeadToCoord);
		return(true);
	}

	LinkedTo->Set_Occupy_Bit(LinkedTo->PositionCoord);
	return(false);
}

// FUNCTION: GAMEMD 0x0075C640
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_WalkLocomotion;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0075C680
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) WalkLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0075C700
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Save(IStream * stream, BOOL cleardirty)
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

// FUNCTION: GAMEMD 0x0075C7E0
LayerType STDMETHODCALLTYPE WalkLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_GROUND);
}

// FUNCTION: GAMEMD 0x0075C7F0
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
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

// FUNCTION: GAMEMD 0x0075C850
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
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

// FUNCTION: GAMEMD 0x0075C8A0
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::End_Piggyback(ILocomotion ** pointer)
{
	if (pointer == NULL) {
		return(E_POINTER);
	}
	if (Piggybacker != NULL) {
		*pointer = Piggybacker;
		Piggybacker.Detach();
		return(S_OK);
	}
	return(S_FALSE);
}

// FUNCTION: GAMEMD 0x0075C8E0
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && Piggybacker != NULL && !IsProcessingMovement && !LinkedTo->IsAttackedByLocomotor) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075C920
HRESULT STDMETHODCALLTYPE WalkLocomotionClass::Piggyback_CLSID(GUID * classid)
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

// FUNCTION: GAMEMD 0x0075CA30
void STDMETHODCALLTYPE WalkLocomotionClass::Mark_All_Occupation_Bits(int mark)
{
	if (mark == MARK_UP) {
		LinkedTo->Clear_Occupy_Bit((Coord)Head_To_Coord());
	}
}

// FUNCTION: GAMEMD 0x0075CA80
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Moving_Here(CoordStruct to)
{
	Coord headto = Head_To_Coord();
	if (headto.As_Cell() == Coord(to).As_Cell() && abs(headto.Z - to.Z) <= LEVEL_LEPTON_H) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0075CB20
boolean STDMETHODCALLTYPE WalkLocomotionClass::Is_Really_Moving_Now(void)
{
	return(IsReallyMoving);
}

// FUNCTION: GAMEMD 0x0075CB30
void STDMETHODCALLTYPE WalkLocomotionClass::Limbo(void)
{
	DestinationCoord = COORD_NONE;
	HeadToCoord = COORD_NONE;
}

// FUNCTION: GAMEMD 0x0075CB80
// WalkLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0075CB90
// WalkLocomotionClass::Release

// FUNCTION: GAMEMD 0x0075CBA0
// WalkLocomotionClass::Is_Piggybacking

// FUNCTION: GAMEMD 0x0075CBC0
// WalkLocomotionClass::Stop_Movement_Animation

// FUNCTION: GAMEMD 0x0075CBD0
// WalkLocomotionClass::Fetch_Object_Size

// SYNTHETIC: GAMEMD 0x0075CBE0
// ??_GWalkLocomotionClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x0075CC30
// WalkLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x0075CC40
// WalkLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0075CC50
// WalkLocomotionClass::Release

// FUNCTION: GAMEMD 0x0075CC60
// WalkLocomotionClass::QueryInterface

// FUNCTION: GAMEMD 0x0075CC70
// WalkLocomotionClass::AddRef

// FUNCTION: GAMEMD 0x0075CC80
// WalkLocomotionClass::Release
