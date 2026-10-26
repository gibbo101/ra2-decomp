// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "airstrike.h"
#include "_rules.h"
#include "aircraft.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "rules.h"
#include "swizzle.h"
#include "techno.h"
#include "techtype.h"
#include "voc.h"

extern "C" const CLSID CLSID_AirstrikeClass;
namespace Game { extern bool IsActive; }
namespace Unsorted { extern int CurrentFrame; }

// SYNTHETIC: GAMEMD 0x0041D280 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x0041D2C0 SYMBOL
// _$E31
DynamicVectorClass<AirstrikeClass *> Airstrikes;

static inline TechnoClass * Cast_Techno(ObjectClass * object)
{
	if (object == NULL) return NULL;
	return (object->AbstractFlags & 1) ? (TechnoClass *)object : NULL;
}

// FUNCTION: GAMEMD 0x0041D300
AirstrikeClass::AirstrikeClass(void)
{
	Airstrikes.Add(this);
}

// FUNCTION: GAMEMD 0x0041D380
AirstrikeClass::AirstrikeClass(TechnoClass * owner)
{
	Airstrikes.Add(this);
	IsOnMission = false;
	Owner = owner;
	unknown_bool_3D = false;
	AirstrikeTeam = Owner->Techno_Type_Class()->AirstrikeTeam;
	EliteAirstrikeTeam = Owner->Techno_Type_Class()->EliteAirstrikeTeam;
	AirstrikeRechargeTime = Owner->Techno_Type_Class()->AirstrikeRechargeTime;
	EliteAirstrikeRechargeTime = Owner->Techno_Type_Class()->EliteAirstrikeRechargeTime;
	AirstrikeTeamType = Owner->Techno_Type_Class()->AirstrikeTeamType;
	EliteAirstrikeTeamType = Owner->Techno_Type_Class()->EliteAirstrikeTeamType;
	FirstObject = NULL;
	Target = NULL;
}

// FUNCTION: GAMEMD 0x0041D490
AirstrikeClass::AirstrikeClass(NoInitClass const & x) :
	AbstractClass(x)
{
}

// FUNCTION: GAMEMD 0x0041D4C0
AirstrikeClass::~AirstrikeClass(void)
{
	Airstrikes.Delete(this);
}

// FUNCTION: GAMEMD 0x0041D540
void AirstrikeClass::InvalidatePointer(AbstractClass * ptr)
{
	if (Game::IsActive) {
		FootClass * foot;
		TechnoClass * object;
		if (ptr == NULL) {
			foot = NULL;
			object = NULL;
		} else {
			foot = (ptr->AbstractFlags & 4) ? (FootClass *)ptr : NULL;
			object = (ptr->Fetch_RTTI() == RTTI_BUILDING) ? (TechnoClass *)ptr : NULL;
		}
		if (ptr == Owner) {
			if (unknown_bool_3D && Owner != NULL && FirstObject != NULL && Owner->House->Is_Player_Control()) {
				VocClass::PlayAt(Rule->AirstrikeAbortSound, FirstObject->Get_Coord());
			}
			ClearTarget();
			while (FirstObject != NULL) {
				FUN_0041DC80(FirstObject);
			}
			if (Target != NULL && (Target->AbstractFlags & 1)) {
				Cast_Techno(Target)->Airstrike = NULL;
			}
			Target = NULL;
			return;
		}
		if (foot != NULL && foot->Airstrike == this) {
			FUN_0041DC80(foot);
			return;
		}
		if (object != NULL && object->Airstrike == this) {
			if (object->Strength <= 0 || !object->IsActive) {
				ClearTarget();
			}
		}
	} else {
		if (ptr == Owner) {
			for (FootClass * foot = FirstObject; foot != NULL; foot = foot->Member) {
				foot->Airstrike = NULL;
			}
		} else if (ptr != NULL && (ptr->AbstractFlags & 4) && ((FootClass *)ptr)->Airstrike == this) {
			FUN_0041DC80((FootClass *)ptr);
		}
	}
}

// FUNCTION: GAMEMD 0x0041D6E0
void AirstrikeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: GAMEMD 0x0041D6F0
HRESULT STDMETHODCALLTYPE AirstrikeClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) AirstrikeClass(NoInitClass());

		Swizzle_Pointer(&Owner);
		Swizzle_Pointer(&Target);
		Swizzle_Pointer(&AirstrikeTeamType);
		Swizzle_Pointer(&EliteAirstrikeTeamType);
		Swizzle_Pointer(&FirstObject);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0041D780
HRESULT STDMETHODCALLTYPE AirstrikeClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: GAMEMD 0x0041D7A0
HRESULT STDMETHODCALLTYPE AirstrikeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AirstrikeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0041D7E0
bool AirstrikeClass::FUN_0041D7E0(ObjectClass * target)
{
	int recharge = Owner->Veterancy.Is_Elite() ? EliteAirstrikeRechargeTime : AirstrikeRechargeTime;
	if (!IsOnMission && (int)TeamDissolveFrame + recharge <= Unsorted::CurrentFrame) {
		return true;
	}
	if (target != Target) {
		return true;
	}
	return false;
}
