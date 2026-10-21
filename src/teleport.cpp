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
extern const double DAT_007F5100;
extern const double DAT_007F5108;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntInit {
	int A, B, C;
	IntInit() { C = 0; A = 0x80; B = 0x80; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

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

// FUNCTION: GAMEMD 0x007192C0
void STDMETHODCALLTYPE TeleportLocomotionClass::Do_Turn(DirStruct coord)
{
	LinkedTo->PrimaryFacing.Set_Desired(coord);
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

// SYNTHETIC: GAMEMD 0x0071A1C0 SYMBOL
// _$E80
double DAT_00B0EC78 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0071A1F0 SYMBOL
// _$E83
double DAT_00B0ECC8 = DAT_00B0EC78 / DAT_007F5100;

// SYNTHETIC: GAMEMD 0x0071A210 SYMBOL
// _$E86
double DAT_00B0ECB0 = DAT_007E1718 / DAT_00B0ECC8;

// SYNTHETIC: GAMEMD 0x0071A230 SYMBOL
// _$E89
double DAT_00B0ECA8 = DAT_007E1720 * DAT_007F5108;

// SYNTHETIC: GAMEMD 0x0071A250 SYMBOL
// _$E92
double DAT_00B0EC80 = DAT_007E1728 * DAT_007F5108;

// SYNTHETIC: GAMEMD 0x0071A270 SYMBOL
// _$E95
double DAT_00B0EC40 = DAT_007E1730 * DAT_007F5108;

// SYNTHETIC: GAMEMD 0x0071A290 SYMBOL
// _$E98
int DAT_00B0ECD0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0EC40 - DAT_00B0EC80) * DAT_00B0EC78));

// SYNTHETIC: GAMEMD 0x0071A2D0 SYMBOL
// _$E101
double DAT_00B0EC88 = FUN_004CADE0(DAT_00B0ECD0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0071A300 SYMBOL
// _$E104
double DAT_00B0ECA0 = FUN_004CADE0((DAT_00B0ECD0 * 2) / DAT_00B0EC78);

// SYNTHETIC: GAMEMD 0x0071A330 SYMBOL
// _$E107
int DAT_00B0ECC4 = (int)(DAT_00B0ECD0 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0071A360 SYMBOL
// _$E110
ShortPair DAT_00B0EC58;

// SYNTHETIC: GAMEMD 0x0071A370 SYMBOL
// _$E113
IntTriple DAT_00B0EC90;

// SYNTHETIC: GAMEMD 0x0071A390 SYMBOL
// _$E116
IntInit DAT_00B0ECB8;

// SYNTHETIC: GAMEMD 0x0071A3B0 SYMBOL
// _$E119
IntQuad DAT_00B0EC48;
