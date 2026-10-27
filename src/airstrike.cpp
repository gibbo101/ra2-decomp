// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x0041D070 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0041D0A0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0041D0C0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0041D0E0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0041D100 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0041D120 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0041D140 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0041D180 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0041D1B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0041D1E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0041D210 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0041D220 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0041D240 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0041D260 SYMBOL
// _$E47

extern "C" const CLSID CLSID_AirstrikeClass;
namespace Game { extern bool IsActive; }
namespace Unsorted { extern int CurrentFrame; }

// SYNTHETIC: GAMEMD 0x0041D280 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x0041D2C0 SYMBOL
// _$E76
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

// FUNCTION: GAMEMD 0x0041D830
void AirstrikeClass::StartMission(ObjectClass * target)
{
	if (!IsOnMission) {
		FUN_0041D860(target);
	} else if (target != Target) {
		ResetTarget(target);
	}
}

#include "airctype.h"
#include "building.h"
#include "_map.h"
#include "cell.h"
#include "infatype.h"
#include "map.h"

extern int __fastcall FUN_0065E850(HouseClass * house, int type, int count, int mission, AbstractClass * target, AbstractClass * destination, int infantrytype, int infantrycount, AirstrikeClass * sender);

// FUNCTION: GAMEMD 0x0041D860
void AirstrikeClass::FUN_0041D860(ObjectClass * target)
{
	IsOnMission = true;
	unknown_bool_3D = true;
	PlanesReleased = 0;
	int type = AirstrikeTeamType->HeapID;
	int infantrytype = Rule->SovParaDropInf[0]->HeapID;
	int count;
	if (Owner->Veterancy.Is_Elite()) {
		count = EliteAirstrikeTeam;
		type = EliteAirstrikeTeamType->HeapID;
	} else {
		count = AirstrikeTeam;
	}
	Cell cell(target->Center_Coord());
	CellClass * cellptr = &Map[cell];
	PlanesLeft = PlaneCount = FUN_0065E850(Owner->Owner_HouseClass(), type, count, MISSION_ATTACK, cellptr, NULL, infantrytype, 0, this);
	if (PlanesLeft != 0) {
		if (FirstObject != NULL) {
			VocClass::PlayAt(Rule->AirstrikeAttackVoice, FirstObject->Get_Coord());
		}
		Target = target;
		if (target != NULL && target->Fetch_RTTI() == RTTI_BUILDING) {
			BuildingClass * building = (BuildingClass *)target;
			building->Airstrike = this;
			building->StartAirstrikeTimer(100000);
			building->ColorAdd = Rule->ColorAdd[Rule->LaserTargetColor].Get_Red();
			building->ColorAddG = Rule->ColorAdd[Rule->LaserTargetColor].Get_Green();
			building->ColorAddB = Rule->ColorAdd[Rule->LaserTargetColor].Get_Blue();
			building->unknown_int_6FC = 1;
			building->Mark(MARK_CHANGE);
		}
	}
}

// FUNCTION: GAMEMD 0x0041DA20
void AirstrikeClass::ResetTarget(ObjectClass * target)
{
	FootClass * member = FirstObject;
	if (member != NULL) {
		FootClass * next = member->Member;
		for (; next != NULL; next = next->Member) {
			if (next->Ammo != member->Ammo) {
				ClearTarget();
				return;
			}
		}
	}

	TechnoClass * oldbuilding;
	TechnoClass * newbuilding;
	ObjectClass * old = Target;
	if (old == NULL) {
		oldbuilding = NULL;
	} else {
		oldbuilding = (old->Fetch_RTTI() == RTTI_BUILDING) ? (TechnoClass *)old : NULL;
	}
	if (target == NULL) {
		newbuilding = NULL;
	} else {
		newbuilding = (target->Fetch_RTTI() == RTTI_BUILDING) ? (TechnoClass *)target : NULL;
	}
	if (oldbuilding != NULL) {
		oldbuilding->Airstrike = NULL;
		oldbuilding->StopAirstrikeTimer();
		oldbuilding->Mark(MARK_CHANGE);
	}
	Target = NULL;
	if (newbuilding != NULL) {
		newbuilding->Airstrike = this;
		newbuilding->StartAirstrikeTimer(100000);
		newbuilding->Mark(MARK_CHANGE);
	}
	FootClass * foot = FirstObject;
	Target = target;
	for (int i = 0; i < PlanesLeft; i++) {
		if (foot != NULL) {
			if (foot->Ammo > 0) {
				foot->Assign_Target(target);
				foot->QueueMission(MISSION_ATTACK, false);
				unknown_bool_3D = true;
			}
			foot = foot->Member;
		}
	}
}

// FUNCTION: GAMEMD 0x0041DB40
void AirstrikeClass::ClearTarget(void)
{
	if (Game::IsActive) {
		FootClass * foot = FirstObject;
		if (foot != NULL) {
			while (foot != NULL && foot->Ammo != 0) {
				foot = foot->Member;
			}
			if (foot != NULL) {
				for (foot = FirstObject; foot != NULL; foot = foot->Member) {
					foot->Ammo = 0;
				}
			}
		}
		foot = FirstObject;
		for (int i = 0; foot != NULL; ) {
			foot->QueueMission(MISSION_RETREAT, false);
			foot->Assign_Target(NULL);
			foot->Assign_Destination(NULL, true);
			foot = foot->Member;
			if (++i > 100) break;
		}
		ObjectClass * target = Target;
		unknown_bool_3D = false;
		if (target != NULL && target->Fetch_RTTI() == RTTI_BUILDING && ((TechnoClass *)target)->Airstrike == this) {
			AirstrikeClass * other = NULL;
			for (int i = 0; i < Airstrikes.Count(); i++) {
				AirstrikeClass * strike = Airstrikes[i];
				if (strike != this && strike->Target == target) other = strike;
			}
			((TechnoClass *)target)->Airstrike = other;
			if (other == NULL && Game::IsActive) {
				target->Mark(MARK_CHANGE);
			}
			if (((TechnoClass *)target)->Airstrike == this) {
				((TechnoClass *)target)->Airstrike = NULL;
			}
		}
		Target = NULL;
	}
}

// FUNCTION: GAMEMD 0x0041DC50
void AirstrikeClass::AI(void)
{
}

// FUNCTION: GAMEMD 0x0041DC60
void AirstrikeClass::FUN_0041DC60(void)
{
	PlanesReleased++;
	if (PlanesReleased >= PlanesLeft) {
		unknown_bool_3D = false;
	}
}

// FUNCTION: GAMEMD 0x0041DC80
void AirstrikeClass::FUN_0041DC80(FootClass * foot)
{
	FootClass * current = FirstObject;
	int i = 0;
	if (current == foot) {
		FirstObject = foot->Member;
		foot->Member = NULL;
		PlanesLeft--;
		foot->Airstrike = NULL;
	} else {
		while (current != NULL) {
			FootClass * next = current->Member;
			if (next == foot) {
				current->Member = foot->Member;
				foot->Member = NULL;
				current = NULL;
				PlanesLeft--;
				foot->Airstrike = NULL;
			} else {
				current = next;
			}
			if (++i > 100) break;
		}
	}
	if (PlanesLeft <= 0) {
		TeamDissolveFrame = Unsorted::CurrentFrame;
		IsOnMission = false;
		unknown_bool_3D = false;
	}
}

// FUNCTION: GAMEMD 0x0041DD10
void AirstrikeClass::FUN_0041DD10(void)
{
	TeamDissolveFrame = Unsorted::CurrentFrame;
	IsOnMission = false;
	unknown_bool_3D = false;
}

// FUNCTION: GAMEMD 0x0041DD30
int AirstrikeClass::Fetch_Object_Size(void) const
{
	return(sizeof(AirstrikeClass));
}

// FUNCTION: GAMEMD 0x0041DD40
RTTIType AirstrikeClass::Fetch_RTTI(void) const
{
	return(RTTI_AIRSTRIKE);
}

// SYNTHETIC: GAMEMD 0x0041DD50
// ??_GAirstrikeClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0041DDE0
// ?Clear@?$DynamicVectorClass@PAVAirstrikeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0041DE10
// ?ID@?$DynamicVectorClass@PAVAirstrikeClass@@@@UAEHPBQAVAirstrikeClass@@@Z
// TEMPLATE: GAMEMD 0x0041DE30
// ?GetItem@?$VectorClass@PAVAirstrikeClass@@@@UBEPAVAirstrikeClass@@H@Z
template class DynamicVectorClass<AirstrikeClass *>;

// TEMPLATE: GAMEMD 0x0041DE40
// ??8?$VectorClass@PAVAirstrikeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0041DE80
// ?Resize@?$VectorClass@PAVAirstrikeClass@@@@UAE_NHPBQAVAirstrikeClass@@@Z
// TEMPLATE: GAMEMD 0x0041DF30
// ?Clear@?$VectorClass@PAVAirstrikeClass@@@@UAEXXZ
template class VectorClass<AirstrikeClass *>;

// TEMPLATE: GAMEMD 0x0041DF60
// ?ID@?$VectorClass@PAVAirstrikeClass@@@@UAEHABQAVAirstrikeClass@@@Z
// TEMPLATE: GAMEMD 0x0041DFA0
// ?ID@?$VectorClass@PAVAirstrikeClass@@@@UAEHPBQAVAirstrikeClass@@@Z
// TEMPLATE: GAMEMD 0x0041DFC0
// ?Resize@?$DynamicVectorClass@PAVAirstrikeClass@@@@UAE_NHPBQAVAirstrikeClass@@@Z
// TEMPLATE: GAMEMD 0x0041E070
// ?ID@?$DynamicVectorClass@PAVAirstrikeClass@@@@UAEHABQAVAirstrikeClass@@@Z
// SYNTHETIC: GAMEMD 0x0041E0A0
// ??_G?$VectorClass@PAVAirstrikeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0041E0F0
// ??_G?$DynamicVectorClass@PAVAirstrikeClass@@@@UAEPAXI@Z
