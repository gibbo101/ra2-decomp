// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "capmgr.h"
#include "_rules.h"
#include "anim.h"
#include "animtype.h"
#include "building.h"
#include "builtype.h"
#include "mission.h"
#include "unit.h"
#include "unittype.h"
#include "house.h"
#include "partsys.h"
#include "rules.h"
#include "scenario.h"
#include "techno.h"
#include "voc.h"

// SYNTHETIC: GAMEMD 0x00471540 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00471570 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00471590 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004715B0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004715D0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004715F0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00471610 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00471650 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00471680 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004716B0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004716E0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004716F0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00471710 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00471730 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00471750 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x00471790 SYMBOL
// _$E89
DynamicVectorClass<CaptureManagerClass *> CaptureManagerArray;

// FUNCTION: GAMEMD 0x004717D0
CaptureManagerClass::CaptureManagerClass(TechnoClass * owner, int damage, bool infinite) :
	AbstractClass(),
	ControlNodes(),
	Owner(owner)
{
	MaxControlNodes = damage;
	InfiniteMindControl = infinite;
	OverloadDeathSoundPlayed = false;
	OverloadDamageDelay = 30;
	OverloadPipState = 0;
	CaptureManagerArray.Add(this);
}

// FUNCTION: GAMEMD 0x00471890
CaptureManagerClass::CaptureManagerClass(void) :
	AbstractClass(),
	ControlNodes()
{
	Owner = 0;
	MaxControlNodes = 1;
	InfiniteMindControl = false;
	OverloadDeathSoundPlayed = false;
	OverloadDamageDelay = 30;
	OverloadPipState = 0;
	CaptureManagerArray.Add(this);
}

// FUNCTION: GAMEMD 0x00471950
CaptureManagerClass::CaptureManagerClass(NoInitClass const & x) :
	AbstractClass(x),
	ControlNodes()
{
}

// FUNCTION: GAMEMD 0x004719A0
CaptureManagerClass::~CaptureManagerClass(void)
{
	CaptureManagerArray.Delete(this);
}

// FUNCTION: GAMEMD 0x00471C90
bool CaptureManagerClass::CanCapture(TechnoClass * techno)
{
	if (techno == NULL) {
		return false;
	}
	if (techno->Owner_HouseClass() == Owner->Owner_HouseClass()) {
		return false;
	}
	if (techno->Techno_Type_Class()->ImmuneToPsionics) {
		return false;
	}
	if (techno->BunkerLinkedItem != NULL && techno->RTTI == RTTI_UNIT) {
		return false;
	}
	if (techno->IsMindControlled()) {
		return false;
	}
	if (techno->MindControlledByHouse != NULL) {
		return false;
	}
	if (techno->IsIronCurtained()) {
		return false;
	}
	if (InfiniteMindControl || ControlNodes.Count() < MaxControlNodes || MaxControlNodes == 1) {
		if (techno->CurrentMission == MISSION_DECONSTRUCTION || techno->CurrentMission == MISSION_CONSTRUCTION) {
			return false;
		}
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00471D40
bool CaptureManagerClass::CaptureUnit(AbstractClass * target)
{
	if (target == NULL) {
		return false;
	}
	TechnoClass * techno = (target->AbstractFlags & 1) ? (TechnoClass *)target : NULL;
	if (!CanCapture(techno)) {
		return false;
	}

	if (MaxControlNodes == 1) {
		for (int index = ControlNodes.Count() - 1; index >= 0; index--) {
			FreeUnit(ControlNodes[index]->Unit);
		}
	}

	HouseClass * previous = techno->Owner_HouseClass();
	if (!techno->Captured(Owner->Owner_HouseClass(), true)) {
		return false;
	}

	ControlNode * node = new ControlNode;
	node->Unit = techno;
	node->OriginalOwner = previous;
	ControlNodes.Add(node);
	techno->MindControlledBy = Owner;
	node->LinkDrawTimer = Rule->MindControlAttackLineFrames;

	if (techno != NULL && techno->RTTI == RTTI_UNIT && ((techno->RTTI == RTTI_UNIT) ? (UnitClass *)techno : NULL)->Class->IsSimpleDeployer && techno->CurrentMission == MISSION_UNLOAD) {
	} else if (techno->CurrentMission != MISSION_DECONSTRUCTION && techno->CurrentMission != MISSION_CONSTRUCTION) {
		techno->Guard();
	}

	DecideUnitFate(techno);

	BuildingClass * building;
	Coord ring;
	if (target->RTTI == RTTI_BUILDING) {
		building = (BuildingClass *)target;
		ring = techno->Center_Coord() + Coord(0, 0, building->Class->ZHeight * LEVEL_LEPTON_H);
	} else {
		building = NULL;
		ring = techno->Center_Coord() + Coord(0, 0, techno->Techno_Type_Class()->MindControlRingOffset);
	}
	AnimClass * anim = new AnimClass(Rule->ControlledAnimationType, ring, 0, 1, (ShapeFlags_Type)0x600, 0, false);
	techno->MindControlRingAnim = anim;
	if (anim != NULL) {
		anim->SetOwnerObject(techno);
		if (building != NULL) {
			techno->MindControlRingAnim->ZAdjust = -1024;
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x00471F90
bool CaptureManagerClass::FUN_00471F90(AbstractClass * ptr)
{
	for (int index = ControlNodes.Count() - 1; index >= 0; index--) {
		ControlNode * node = ControlNodes[index];
		if (node->Unit == ptr) {
			delete node;
			ControlNodes.Delete_Index(index);
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x00471FF0
bool CaptureManagerClass::FreeUnit(TechnoClass * techno)
{
	if (techno == NULL) {
		return false;
	}
	for (int index = ControlNodes.Count() - 1; index >= 0; index--) {
		ControlNode * node = ControlNodes[index];
		if (node->Unit == techno) {
			if (techno->MindControlRingAnim != NULL) {
				techno->MindControlRingAnim->Delete_Me();
				techno->MindControlRingAnim = NULL;
			}
			int sound = techno->Techno_Type_Class()->MindClearedSound;
			if (sound != -1) {
				VocClass::PlayAt(techno->Techno_Type_Class()->MindClearedSound, techno->PositionCoord);
			} else if (Rule->MindClearedSound != -1) {
				VocClass::PlayAt(Rule->MindClearedSound, techno->PositionCoord);
			}
			techno->Captured(node->OriginalOwner, true);
			DecideUnitFate(techno);
			techno->MindControlledBy = NULL;
			delete ControlNodes[index];
			ControlNodes.Delete_Index(index);
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x00472140
void CaptureManagerClass::FreeAll(void)
{
	for (int index = ControlNodes.Count() - 1; index >= 0; index--) {
		FreeUnit(ControlNodes[index]->Unit);
	}
}
