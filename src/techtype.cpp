// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "techtype.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "combat.h"
#include "findmake.h"
#include "globals.h"
#include "ilocos.h"
#include "infatype.h"
#include "mixfile.h"
#include "notify.h"
#include "psystype.h"
#include "rules.h"
#include "session.h"
#include "swizzle.h"
#include "tracker.h"
#include "unittype.h"
#include "vanimtype.h"
#include "weapon.h"

DynamicVectorClass<TechnoTypeClass *> TechnoTypeClass::Array;

// FUNCTION: 0x00711840
TechnoTypeClass::TechnoTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	VeteranAbilities(x),
	EliteAbilities(x),
	DebrisTypes(x),
	DebrisMaximums(x),
	Unused2(0),
	Dock(x),
	VoiceSelect(x),
	VoiceSelectEnslaved(x),
	VoiceSelectDeactivated(x),
	VoiceMove(x),
	VoiceAttack(x),
	VoiceSpecialAttack(x),
	VoiceDie(x),
	VoiceFeedback(x),
	MoveSound(x),
	DieSound(x),
	DebrisAnims(x),
	Prerequisite(x),
	PrerequisiteOverride(x),
	CameoFilename(NULL),
	AltCameoFilename(NULL),
	Explosion(x),
	DestroyAnim(x),
	DamageParticleSystems(),
	DestroyParticleSystems(),
	TurretAnimData(2, 1, 1, 1),
	UnknownBoolCB4(false),
	BarrelAnimData(2, 1, 1, 1),
	UnknownBoolCC8(false)
{
	bool found = false;
	for (int i = 0; i < Array.Count(); i++) {
		if (Array[i] == this) {
			found = true;
			break;
		}
	}
	if (!found) {
		Array.Add(this);
	}
}

inline void Free_Alt_Cameo(TechnoTypeClass * type)
{
	if (type->AltCameoAllocated && type->AltCameoData != NULL) {
		delete (char *)type->AltCameoData;
		type->AltCameoData = NULL;
		type->AltCameoAllocated = false;
	}
}

// FUNCTION: 0x00711AE0
TechnoTypeClass::~TechnoTypeClass(void)
{
	if (CameoAllocated && CameoData != NULL) {
		delete (char *)CameoData;
		CameoData = NULL;
		CameoAllocated = false;
	}
	Free_Alt_Cameo(this);
	PointerExpiredNotification::NotifyInvalidType.Delete(this);
	Array.Delete(this);
}

// FUNCTION: 0x00711E80
bool TechnoTypeClass::CanUseWaypoint(void) const
{
	return(true);
}

// FUNCTION: 0x00711E90
bool TechnoTypeClass::CanAttackMove(void) const
{
	if (Weapons[0].Weapon != NULL) {
		if (!PreventAttackMove) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00711EB0
int TechnoTypeClass::Raw_Cost(void) const
{
	return(Cost);
}

// FUNCTION: 0x00711EC0
int TechnoTypeClass::Get_Ownable(void) const
{
	if (IsDoubleOwned && Session.Type != GAME_NORMAL) {
		return(0x7FFFFFFF);
	}
	return(Ownable);
}

// FUNCTION: 0x00711EE0
int TechnoTypeClass::Time_To_Build(void) const
{
	return(Cost * Rule->BuildSpeedBias * (TICKS_PER_MINUTE / 1000.));
}

// FUNCTION: 0x00711F00
int TechnoTypeClass::Cost_Of(HouseClass * house) const
{
	if (house != NULL) {
		float a = house->FUN_0050BDF0(this);
		float b = house->FUN_0050BEB0(this);
		return(Raw_Cost() * b * a);
	}
	return(Raw_Cost());
}

// FUNCTION: 0x00711F60
int TechnoTypeClass::GetRefund(HouseClass * house, bool unknown) const
{
	float refund = Rule->RefundPercent;
	if (unknown) {
		refund = 1.0f;
	}
	if (house != NULL) {
		float a = house->FUN_0050BDF0(this);
		float b = house->FUN_0050BEB0(this);
		if (Soylent != 0) {
			return(Soylent * a);
		}
		int c = Raw_Cost() * b * a;
		if (house->FUN_0050B730()) {
			c = c * refund;
		}
		return(c);
	}
	return(Raw_Cost() * refund);
}

// FUNCTION: 0x00712040
void const * TechnoTypeClass::Get_Cameo_Data(void) const
{
	if (PlayerPtr != NULL && PlayerPtr->BarracksInfiltrated) {
		if (Fetch_RTTI() == RTTI_INFANTRYTYPE && AltCameoData != NULL && IsTrainable) {
			return(AltCameoData);
		}
	}
	if (PlayerPtr != NULL && PlayerPtr->WarFactoryInfiltrated) {
		if (Fetch_RTTI() == RTTI_UNITTYPE || (Fetch_RTTI() == RTTI_BUILDINGTYPE && UndeploysInto != NULL)) {
			if (AltCameoData != NULL && IsTrainable) {
				return(AltCameoData);
			}
		}
	}
	return(CameoData);
}

// FUNCTION: 0x007120D0
int TechnoTypeClass::Repair_Cost(void) const
{
	int cost = (Raw_Cost()/(MaxStrength/Rule->RepairStep)) * Rule->RepairPercent;
	return(MAX(cost, 1));
}

// FUNCTION: 0x00712120
int TechnoTypeClass::Repair_Step(void) const
{
	return(Rule->RepairStep);
}
