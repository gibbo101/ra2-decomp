// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "droppod.h"
#include "_map.h"
#include "_rand.h"
#include "_rules.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "coord.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "map.h"
#include "noinit.h"
#include "rules.h"
#include "sun.h"
#include "weapon.h"
#include "layer.hh"
#include <new.h>
#include <math.h>

// FUNCTION: 0x004B5AB0
DropPodLocomotionClass::DropPodLocomotionClass(void) :
	BASECLASS(),
	Flipped(false),
	DestinationCoord(COORD_NONE),
	Piggybacker(NULL)
{
}

// FUNCTION: 0x004B5B00
DropPodLocomotionClass::~DropPodLocomotionClass(void)
{
}

// FUNCTION: 0x004B5B30
boolean STDMETHODCALLTYPE DropPodLocomotionClass::Is_Moving(void)
{
	return(true);
}

// FUNCTION: 0x004B5B40
CoordStruct STDMETHODCALLTYPE DropPodLocomotionClass::Destination(void)
{
	return(DestinationCoord);
}

// FUNCTION: 0x004B5B70
boolean STDMETHODCALLTYPE DropPodLocomotionClass::Process(void)
{
	Coord coord = LinkedTo->Position;
	Coord smoke_coord = coord;

	int speed = LinkedTo->HeightAGL / 10 + 2;
	if (Rule->DropPodSpeed > speed) {
		speed = Rule->DropPodSpeed;
	}

	if (Flipped) {
		coord.X += fastmath::cos(Rule->DropPodAngle) * speed;
	} else {
		coord.X -= fastmath::cos(Rule->DropPodAngle) * speed;
	}
	coord.Z -= fastmath::sin(Rule->DropPodAngle) * speed;

	int height = LinkedTo->HeightAGL;
	FootClass * linked = LinkedTo;

	if (height <= 0) {
		linked->HeightAGL = 0;
		coord = linked->Position;
		linked->Limbo();

		AddRef();
		End_Piggyback(&LinkedTo->Locomotion);

		if (!linked->Unlimbo(coord, DIR_N)) {
			Map.DamageArea(coord, 100, LinkedTo, Rule->C4Warhead, true, NULL);
			new AnimClass(Combat_Anim(100, Rule->C4Warhead, LAND_CLEAR, coord), coord, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(coord));
		} else {
			new AnimClass(Rule->DropPodPuff, coord);
			if (Flipped) {
				new AnimClass(Rule->DropPod[1], coord);
			} else {
				new AnimClass(Rule->DropPod[0], coord);
			}
			linked->Mark(MARK_DOWN);
			linked->HeightAGL = 0;
			linked->Enter_Idle_Mode();
			linked->Commence();
			linked->Scatter(COORD_NONE);
		}
		Release();
	} else {
		LinkedTo->PositionCoord = coord;
		WeaponTypeClass const * weapon = Rule->DropPodWeapon;

		if (weapon != NULL) {
			if (Frame % 6 == 0) {
				new AnimClass(AnimTypes[AnimTypeClass::From_Name("SMOKEY")], smoke_coord);
			}
			if (Frame % 3 == 0) {
				if (!LinkedTo->House->Is_Ally(Map[DestinationCoord].Cell_Techno())) {
					Coord scattered_coord = Coord_Scatter(DestinationCoord, 0x55, false);
					VocClass::PlayAt(weapon->Sound.Pick(NonCriticalRandomNumber), coord, NULL);
					Map.DamageArea(scattered_coord, 2 * weapon->Attack, LinkedTo, weapon->WarheadPtr, true, NULL);
					new AnimClass(Combat_Anim(2 * weapon->Attack, weapon->WarheadPtr, LAND_CLEAR, scattered_coord), scattered_coord, 0, 1, ShapeFlags_Type(0x2600), Get_Explosion_Z(scattered_coord));
				}
			}
		}
	}
	return(true);
}

// FUNCTION: 0x004B6040
void STDMETHODCALLTYPE DropPodLocomotionClass::Move_To(CoordStruct to)
{
	if (DestinationCoord == COORD_NONE) {

		DestinationCoord = to;
		DestinationCoord.Z = Map.Get_Height_GL(DestinationCoord);

		Coord dropcoord = DestinationCoord;
		dropcoord.Z += Rule->DropPodHeight;
		dropcoord.X += (double)Rule->DropPodHeight / fastmath::tan(Rule->DropPodAngle);

		if (!Map.In_Local_Radar(dropcoord)) {
			Flipped = true;
			dropcoord.X = DestinationCoord.X - (double)Rule->DropPodHeight / fastmath::tan(Rule->DropPodAngle);
		}

		LinkedTo->PositionCoord = dropcoord;

		if (LinkedTo->Unlimbo(dropcoord, DIR_S)) {
			LinkedTo->PositionCoord = dropcoord;
			LinkedTo->PrimaryFacing.Set(DIR_S << 8);
			new AnimClass(Rule->AtmosphereEntry, dropcoord);
		} else {
			LinkedTo->Unlimbo(dropcoord, DIR_S);
		}
	}
}

// FUNCTION: 0x004B6200
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BallisticLocomotion;
	return(S_OK);
}

// FUNCTION: 0x004B6240
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) DropPodLocomotionClass(NoInitClass());

		bool haspiggy;
		stream->Read(&haspiggy, sizeof(haspiggy), NULL);

		if (haspiggy) {
			result = OleLoadFromStream(stream, IID_ILocomotion, (void**)&Piggybacker);
		}
	}
	return(result);
}

// FUNCTION: 0x004B62C0
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::Save(IStream * stream, BOOL cleardirty)
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
			result = stream->Write(&haspiggy, sizeof(haspiggy), NULL);
		}
	}
	return(result);
}

// FUNCTION: 0x004B63A0
void STDMETHODCALLTYPE DropPodLocomotionClass::Stop_Moving(void)
{
}

// FUNCTION: 0x004B63B0
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::Begin_Piggyback(ILocomotion * pointer)
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

// FUNCTION: 0x004B6400
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::End_Piggyback(ILocomotion ** pointer)
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

// FUNCTION: 0x004B6440
boolean STDMETHODCALLTYPE DropPodLocomotionClass::Is_Ok_To_End(void)
{
	if (!Is_Moving() && Piggybacker != NULL) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004B6470
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
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

// FUNCTION: 0x004B64D0
LayerType STDMETHODCALLTYPE DropPodLocomotionClass::In_Which_Layer(void)
{
	return(LAYER_AIR);
}

// FUNCTION: 0x004B64E0
HRESULT STDMETHODCALLTYPE DropPodLocomotionClass::Piggyback_CLSID(GUID * classid)
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

// FUNCTION: 0x004B65F0
int STDMETHODCALLTYPE DropPodLocomotionClass::Drawing_Code(void)
{
	return(Flipped ? 1 : 0);
}

// FUNCTION: 0x004B6610 LocomotionClass::Is_Moving_Now

// FUNCTION: 0x004B6620 LocomotionClass::Mark_All_Occupation_Bits

// FUNCTION: 0x004B6630 LocomotionClass::Is_Moving_Here

// FUNCTION: 0x004B6640 LocomotionClass::Will_Jump_Tracks

// FUNCTION: 0x004B6650 LocomotionClass::Lock

// FUNCTION: 0x004B6660 LocomotionClass::Unlock

// FUNCTION: 0x004B6670 LocomotionClass::Get_Track_Number

// FUNCTION: 0x004B6680 LocomotionClass::Get_Track_Index

// FUNCTION: 0x004B6690 LocomotionClass::Get_Speed_Accum

// FUNCTION: 0x004B66A0 DropPodLocomotionClass::AddRef

// FUNCTION: 0x004B66B0 DropPodLocomotionClass::Release

// FUNCTION: 0x004B66C0 DropPodLocomotionClass::Is_Piggybacking

// FUNCTION: 0x004B66E0 DropPodLocomotionClass::Fetch_Object_Size

// FUNCTION: 0x004B66F0 DropPodLocomotionClass::~DropPodLocomotionClass

// FUNCTION: 0x004B6740 DropPodLocomotionClass::QueryInterface

// FUNCTION: 0x004B6750 DropPodLocomotionClass::AddRef

// FUNCTION: 0x004B6760 DropPodLocomotionClass::Release

// FUNCTION: 0x004B6770 DropPodLocomotionClass::QueryInterface

// FUNCTION: 0x004B6780 DropPodLocomotionClass::AddRef

// FUNCTION: 0x004B6790 DropPodLocomotionClass::Release

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E8370;

// FUNCTION: 0x004B67A0 $E81
double DAT_008A08A8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004B67D0 $E84
double DAT_008A0918 = DAT_008A08A8 / DAT_007E8370;

// FUNCTION: 0x004B67F0 $E87
double DAT_008A0900 = DAT_007E1718 / DAT_008A0918;
