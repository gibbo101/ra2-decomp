// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "_rules.h"
#include "rules.h"
#include "coord.h"
#include "slavemgr.h"
#include "infantry.h"
#include "infatype.h"
#include "techno.h"

namespace Unsorted { extern int CurrentFrame; }

DynamicVectorClass<SlaveManagerClass *> SlaveManagers;

// SYNTHETIC: GAMEMD 0x006AF120 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x006AF160 SYMBOL
// _$E78

// FUNCTION: GAMEMD 0x006AF1A0
SlaveManagerClass::SlaveManagerClass(TechnoClass * owner, InfantryTypeClass * type, int number, int regen, int reload) :
	AbstractClass()
{
	Owner = owner;
	SlaveType = type;
	SlaveCount = number;
	RegenRate = regen;
	ReloadRate = reload;
	for (int index = 0; index < SlaveCount; index++) {
		SlaveControl * node = new SlaveControl;
		node->Slave = (InfantryClass *)SlaveType->Create_One_Of(Owner->Owner_HouseClass());
		if (node->Slave != NULL) {
			node->Slave->Limbo();
			node->Slave->SlaveOwner = Owner;
			node->State = 0;
			node->RespawnTimer = CDTimerClass<FrameTimerClass>(0);
		}
		SlaveNodes.Add(node);
	}
	RespawnTimer = CDTimerClass<FrameTimerClass>(10);
	State = 0;
	LastScanFrame = 0;
	SlaveManagers.Add(this);
}

// FUNCTION: GAMEMD 0x006AF360
SlaveManagerClass::SlaveManagerClass(void) :
	AbstractClass()
{
	Owner = NULL;
	SlaveType = NULL;
	SlaveCount = 0;
	RegenRate = 0;
	ReloadRate = 0;
	RespawnTimer = CDTimerClass<FrameTimerClass>(0);
	State = 0;
	LastScanFrame = 0;
	SlaveManagers.Add(this);
}

// FUNCTION: GAMEMD 0x006AF440
SlaveManagerClass::SlaveManagerClass(NoInitClass const & x) :
	AbstractClass(x)
{
}

// FUNCTION: GAMEMD 0x006AF4A0
SlaveManagerClass::~SlaveManagerClass(void)
{
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		delete SlaveNodes[index];
	}
	SlaveNodes.Delete_All();
	SlaveManagers.Delete(this);
}

// FUNCTION: GAMEMD 0x006AF580
void SlaveManagerClass::SetOwner(TechnoClass * owner)
{
	Owner->SlaveManager = NULL;
	Owner = owner;
	if (owner->SlaveManager != NULL) {
		owner->SlaveManager->Killed(NULL, NULL);
		delete owner->SlaveManager;
	}
	owner->SlaveManager = this;
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		if (SlaveNodes[index]->Slave != NULL) {
			SlaveNodes[index]->Slave->SlaveOwner = Owner;
		}
	}
}

// FUNCTION: GAMEMD 0x006AF5F0
void SlaveManagerClass::AI(void)
{
	if (RespawnTimer == 0) {
		RespawnTimer = CDTimerClass<FrameTimerClass>(10);
		if (Owner != NULL) {
			FUN_006AF6C0();
			FUN_006AFD60();
		}
	}
}

// FUNCTION: GAMEMD 0x006AF650
void SlaveManagerClass::CreateSlave(SlaveControl * node)
{
	node->Slave = (InfantryClass *)SlaveType->Create_One_Of(Owner->Owner_HouseClass());
	if (node->Slave != NULL) {
		node->Slave->Limbo();
		node->Slave->SlaveOwner = Owner;
		node->State = 0;
		node->RespawnTimer = CDTimerClass<FrameTimerClass>(0);
	}
}

// FUNCTION: GAMEMD 0x006B0260
bool SlaveManagerClass::FUN_006B0260(void)
{
	Cell cell = Owner->ScanForTiberium(Rule->SlaveMinerShortScan / 256, false);
	return cell != CELL_NONE;
}

// FUNCTION: GAMEMD 0x006B02C0
Cell SlaveManagerClass::FUN_006B02C0(void)
{
	return Owner->ScanForTiberium(Rule->SlaveMinerLongScan / 256, true);
}
