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
#include "swizzle.h"
#include "infantry.h"
#include "infatype.h"
#include "techno.h"

// SYNTHETIC: GAMEMD 0x006AEF10 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006AEF40 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006AEF60 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006AEF80 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006AEFA0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006AEFC0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006AEFE0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006AF020 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006AF050 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006AF080 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006AF0B0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006AF0C0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006AF0E0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006AF100 SYMBOL
// _$E55

extern "C" const CLSID CLSID_SlaveManagerClass;
namespace Unsorted { extern int CurrentFrame; }
namespace Game { extern bool IsActive; }
extern SideType FUN_006A46D0(char const * name);

DynamicVectorClass<SlaveManagerClass *> SlaveManagers;

// SYNTHETIC: GAMEMD 0x006AF120 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x006AF160 SYMBOL
// _$E78

#include "house.h"
#include "houstype.h"
#include "voc.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "_map.h"
#include "map.h"
#include "unit.h"

static inline BuildingClass * Cast_Building(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	return((object->Fetch_RTTI() == RTTI_BUILDING) ? (BuildingClass *)object : NULL);
}

static inline UnitClass * Cast_Unit(AbstractClass * object)
{
	if (object == NULL) return(NULL);
	return((object->Fetch_RTTI() == RTTI_UNIT) ? (UnitClass *)object : NULL);
}

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

// FUNCTION: GAMEMD 0x006B0300
Cell SlaveManagerClass::FUN_006B0300(Cell cell)
{
	int height;
	int width;
	if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_BUILDING) {
		width = Cast_Building(Owner)->Class->Width();
		height = Cast_Building(Owner)->Class->Height(false);
	} else if (Owner->Techno_Type_Class()->DeploysInto != NULL) {
		width = Owner->Techno_Type_Class()->DeploysInto->Width();
		height = Owner->Techno_Type_Class()->DeploysInto->Height(false);
	} else {
		width = 1;
		height = width;
	}
	Cell found = Map.Nearby_Location(cell, SPEED_TRACK, Map.Get_Cell_Zone(Owner->Get_Cell(), MZONE_NORMAL, false), MZONE_NORMAL, false, Point2D(width, height), true, false, false, false, cell, false, true);
	if (found == CELL_NONE) {
		return CELL_NONE;
	}
	if (width > 2) {
		found = Cell(found.X + 1, found.Y);
	}
	if (height > 2) {
		found = Cell(found.X, found.Y + 1);
	}
	return found;
}

// FUNCTION: GAMEMD 0x006B0490
void SlaveManagerClass::FUN_006B0490(void)
{
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		SlaveControl * node = SlaveNodes[index];
		int state = node->State;
		InfantryClass * slave = node->Slave;
		if (state != 6 && slave != NULL) {
			slave->Guard();
		}
	}
}

// FUNCTION: GAMEMD 0x006B04C0
void SlaveManagerClass::FUN_006B04C0(void)
{
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		SlaveControl * node = SlaveNodes[index];
		InfantryClass * slave = node->Slave;
		if (node->State == 0) {
			Cell cell;
			if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_BUILDING) {
				BuildingClass * building = Cast_Building(Owner);
				int width = building->Class->Width();
				int height = building->Class->Height(false);
				cell = Owner->Get_Cell() + Cell(width - 1, height / 2);
			} else {
				cell = Owner->Get_Cell();
			}
			Cell target = cell;
			if (target != CELL_NONE) {
				Coord coord = Map[target].Center_Coord();
				coord = Map.Closest_Free_Spot(coord, false);
				if (coord != COORD_NONE && coord != CELL_CENTER_COORD && slave->Unlimbo(coord, DIR_N)) {
					slave->Scatter(Owner->Center_Coord(), true, true);
					node->State = 1;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006B0690
Cell SlaveManagerClass::FUN_006B0690(void)
{
	Cell cell;
	if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_BUILDING) {
		BuildingClass * building = Cast_Building(Owner);
		int width = building->Class->Width();
		int height = building->Class->Height(false);
		cell = Owner->Get_Cell() + Cell(width - 1, height / 2);
		return cell;
	}
	return Owner->Get_Cell();
}

// FUNCTION: GAMEMD 0x006B0A20
void SlaveManagerClass::LostSlave(InfantryClass * slave)
{
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		if (SlaveNodes[index]->Slave == slave) {
			SlaveControl * node = SlaveNodes[index];
			if (node->Slave != NULL) {
				node->Slave->SlaveOwner = NULL;
			}
			node->Slave = NULL;
			node->State = 6;
			node->RespawnTimer = CDTimerClass<FrameTimerClass>(RegenRate);
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x006B0A90
void SlaveManagerClass::FUN_006B0A90(int index)
{
	SlaveControl * node = SlaveNodes[index];
	if (node->Slave != NULL) {
		node->Slave->SlaveOwner = NULL;
	}
	node->Slave = NULL;
	node->State = 6;
	node->RespawnTimer = CDTimerClass<FrameTimerClass>(RegenRate);
}

// FUNCTION: GAMEMD 0x006B0AE0
void SlaveManagerClass::Killed(TechnoClass * killer, HouseClass * owner)
{
	if (Owner == NULL) {
		return;
	}
	HouseClass * civilian = NULL;
	int count = Houses.Count();
	SideType side = FUN_006A46D0("Civilian");
	for (int i = 0; i < count; i++) {
		if (Houses[i]->Class->Side == side) {
			civilian = Houses[i];
			break;
		}
	}
	TechnoClass * first = NULL;
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		InfantryClass * slave = SlaveNodes[index]->Slave;
		if (slave != NULL && Game::IsActive) {
			bool limbo = slave->IsInLimbo;
			slave->SlaveOwner = NULL;
			if (limbo) {
				slave->RegisterDestruction(killer);
				slave->Delete_Me();
			} else {
				HouseClass * house;
				if (killer != NULL) {
					house = killer->House;
				} else if (owner != NULL) {
					house = owner;
				} else if (civilian != NULL) {
					house = civilian;
				} else {
					int damage = slave->Strength;
					slave->Take_Damage(damage, 0, Rule->C4Warhead, NULL, false, false, NULL);
					continue;
				}
				slave->Captured(house, true);
				slave->Guard();
				slave->Cheer(true);
				if (first == NULL) {
					first = slave;
				}
			}
		}
	}
	if (Game::IsActive && first != NULL && !ScenarioInit && Rule->SlavesFreeSound != -1) {
		VocClass::PlayAt(Rule->SlavesFreeSound, Coord(first->Position));
	}
	Owner = NULL;
}

// FUNCTION: GAMEMD 0x006B0C80
void SlaveManagerClass::AllGuard(void)
{
	State = 0;
	LastScanFrame = Unsorted::CurrentFrame;
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		SlaveControl * node = SlaveNodes[index];
		int state = node->State;
		InfantryClass * slave = node->Slave;
		if (state != 6 && slave != NULL) {
			slave->Guard();
		}
	}
}

// FUNCTION: GAMEMD 0x006B0CC0
void SlaveManagerClass::FUN_006B0CC0(void)
{
	if (State == 0) {
		State = 1;
		LastScanFrame = 0x7FFFFFFF;
		for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
			SlaveControl * node = SlaveNodes[index];
			int state = node->State;
			InfantryClass * slave = node->Slave;
			if (state != 6 && slave != NULL) {
				slave->Guard();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006B0D10
void SlaveManagerClass::FUN_006B0D10(void)
{
	if (State == 0) {
		State = 4;
		LastScanFrame = 0x7FFFFFFF;
		for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
			SlaveControl * node = SlaveNodes[index];
			int state = node->State;
			InfantryClass * slave = node->Slave;
			if (state != 6 && slave != NULL) {
				slave->Guard();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006B0D60
void SlaveManagerClass::Deploy2(void)
{
	if (State == 0) {
		State = 4;
		LastScanFrame = 0x7FFFFFFF;
		for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
			SlaveControl * node = SlaveNodes[index];
			int state = node->State;
			InfantryClass * slave = node->Slave;
			if (state != 6 && slave != NULL) {
				slave->Guard();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x006B0DB0
void SlaveManagerClass::FUN_006B0DB0(void)
{
	if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_UNIT && Cast_Unit(Owner)->NavCom != NULL) {
		Cell target = Cast_Unit(Owner)->NavCom->Center_Coord();
		Cell cell;
		cell = FUN_006B0300(target);
		if (cell == CELL_NONE) {
			FUN_006B10D0(0);
			Owner->QueueMission(MISSION_GUARD, false);
			return;
		}
		Owner->Assign_Destination(&Map[cell]);
		Owner->QueueMission(MISSION_MOVE, false);
		FUN_006B10D0(2);
		FUN_006B0490();
	} else if (Owner != NULL && Owner->Fetch_RTTI() == RTTI_BUILDING && Owner->ArchiveTarget != NULL) {
		Cell target = Owner->ArchiveTarget->Center_Coord();
		Cell cell;
		cell = FUN_006B0300(target);
		if (cell != CELL_NONE) {
			Owner->Assign_Archive_Target(&Map[cell]);
			FUN_006B10D0(6);
			FUN_006B0490();
		}
	} else {
		Owner->QueueMission(MISSION_GUARD, false);
		FUN_006B10D0(0);
	}
}

// FUNCTION: GAMEMD 0x006B1020
bool SlaveManagerClass::ShouldWakeUpNow(void)
{
	if (Owner != NULL && State == 0) {
		if (!Owner->House->IsHuman) {
			return true;
		}
		if (Owner->Get_Cell_Ptr()->Land_Type() == LAND_TIBERIUM) {
			return true;
		}
		if (Rule->SlaveMinerKickFrameDelay + LastScanFrame < Unsorted::CurrentFrame) {
			Cell cell = Owner->ScanForTiberium(Rule->SlaveMinerShortScan / 256, false);
			if (cell != CELL_NONE) {
				return true;
			}
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x006B10D0
void SlaveManagerClass::FUN_006B10D0(int state)
{
	State = state;
	if (state == 0) {
		LastScanFrame = Unsorted::CurrentFrame;
	} else {
		LastScanFrame = 0x7FFFFFFF;
	}
}

// FUNCTION: GAMEMD 0x006B10F0
void SlaveManagerClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(State);
	for (int index = SlaveNodes.Count() - 1; index >= 0; index--) {
		crc(SlaveNodes[index]->State);
	}
}

// FUNCTION: GAMEMD 0x006B1130
HRESULT STDMETHODCALLTYPE SlaveManagerClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SlaveManagerClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006B1170
HRESULT STDMETHODCALLTYPE SlaveManagerClass::Load(IStream * stream)
{
	int count;
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SlaveManagerClass(NoInitClass());
		result = stream->Read(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			new (&SlaveNodes) DynamicVectorClass<SlaveControl *>();
			int index;
			for (index = 0; index < count; index++) {
				SlaveControl * node = new SlaveControl;
				result = stream->Read(node, sizeof(SlaveControl), NULL);
				if (FAILED(result)) {
					return result;
				}
				SlaveNodes.Add(node);
			}
			for (index = 0; index < count; index++) {
				Swizzle_Pointer(&SlaveNodes[index]->Slave);
			}
			Swizzle_Pointer(&Owner);
			Swizzle_Pointer(&SlaveType);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x006B1300
HRESULT STDMETHODCALLTYPE SlaveManagerClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int count = SlaveNodes.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			for (int index = 0; index < count; index++) {
				result = stream->Write(SlaveNodes[index], sizeof(SlaveControl), NULL);
				if (FAILED(result)) {
					break;
				}
			}
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x006B1370
int SlaveManagerClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x006B1380
RTTIType SlaveManagerClass::Fetch_RTTI(void) const
{
	return(RTTI_SLAVEMANAGER);
}

// SYNTHETIC: GAMEMD 0x006B1390
// ??_GSlaveManagerClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x006B13B0
// ?Clear@?$DynamicVectorClass@PAUSlaveControl@SlaveManagerClass@@@@UAEXXZ
