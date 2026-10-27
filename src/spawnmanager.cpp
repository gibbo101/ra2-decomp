// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "spawnmgr.h"
#include "aircraft.h"
#include "airctype.h"
#include "globals.h"
#include "house.h"
#include "kamikaze.h"
#include "_rules.h"
#include "rules.h"
#include "swizzle.h"
#include "techno.h"
#include "techtype.h"

// SYNTHETIC: GAMEMD 0x006B6A00 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006B6A30 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006B6A50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006B6A70 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006B6A90 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006B6AB0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006B6AD0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006B6B10 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006B6B40 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006B6B70 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006B6BA0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006B6BB0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006B6BD0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006B6BF0 SYMBOL
// _$E47

extern "C" const CLSID CLSID_SpawnManagerClass;
namespace Game { extern bool IsActive; }
namespace Unsorted { extern int CurrentFrame; }

// SYNTHETIC: GAMEMD 0x006B6C10 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x006B6C50 SYMBOL
// _$E78
DynamicVectorClass<SpawnManagerClass *> SpawnManagers;

// FUNCTION: GAMEMD 0x006B6C90
SpawnManagerClass::SpawnManagerClass(TechnoClass * owner, AircraftTypeClass * type, int number, int regen, int reload) :
	SpawnedNodes(),
	UpdateTimer(),
	SpawnTimer()
{
	Owner = owner;
	SpawnType = type;
	SpawnCount = number;
	RegenRate = regen;
	ReloadRate = reload;
	for (int index = 0; index < SpawnCount; index++) {
		SpawnControl * control = new SpawnControl;
		if (!control) {
			break;
		}
		control->Unit = (AircraftClass *)SpawnType->Create_One_Of(Owner->Owner_HouseClass());
		if (control->Unit) {
			if (SpawnType == Rule->V3Rocket.Type || SpawnType == Rule->DMisl.Type || SpawnType == Rule->CMisl.Type) {
				control->IsSpawnMissile = 1;
			} else {
				control->IsSpawnMissile = 0;
			}
			control->Unit->Limbo();
			control->Unit->SpawnOwner = Owner;
			control->Status = SPAWN_IDLE;
			control->SpawnTimer = 0;
			SpawnedNodes.Add(control);
		}
	}
	UpdateTimer = 20;
	Target = 0;
	NewTarget = 0;
	Status = 0;
	SpawnTimer = 0;
	SpawnManagers.Add(this);
}

// FUNCTION: GAMEMD 0x006B6EA0
SpawnManagerClass::SpawnManagerClass(void) :
	SpawnedNodes(),
	UpdateTimer(),
	SpawnTimer()
{
	Owner = 0;
	SpawnType = 0;
	SpawnCount = 0;
	UpdateTimer = 0;
	Target = 0;
	NewTarget = 0;
	Status = 0;
	SpawnTimer = 0;
	SpawnManagers.Add(this);
}

// FUNCTION: GAMEMD 0x006B6FA0
SpawnManagerClass::SpawnManagerClass(NoInitClass const & x) :
	AbstractClass(x),
	SpawnedNodes(),
	UpdateTimer(),
	SpawnTimer()
{
}

// FUNCTION: GAMEMD 0x006B7010
SpawnManagerClass::~SpawnManagerClass(void)
{
	if (Game::IsActive) {
		KillNodes();
	}
	for (int index = SpawnedNodes.Count() - 1; index >= 0; index--) {
		delete SpawnedNodes[index];
	}
	SpawnedNodes.Delete_All();
	SpawnManagers.Delete(this);
}

// FUNCTION: GAMEMD 0x006B7100
void SpawnManagerClass::KillNodes(void)
{
	int delay = (Owner->Strength > 0 && Owner->IsActive) ? 0 : RegenRate;
	for (int index = SpawnedNodes.Count() - 1; index >= 0; index--) {
		if (SpawnedNodes[index]->Status == SPAWN_DEAD) {
			continue;
		}
		if (SpawnedNodes[index]->Status == SPAWN_IDLE || SpawnedNodes[index]->Status == SPAWN_RELOADING) {
			SpawnedNodes[index]->Status = SPAWN_DEAD;
			SpawnedNodes[index]->Unit->Delete_Me();
			SpawnedNodes[index]->Unit = 0;
			SpawnedNodes[index]->SpawnTimer = delay;
		} else if (SpawnedNodes[index]->Status == SPAWN_TAKEOFF) {
			KamikazeInstance.Remove(SpawnedNodes[index]->Unit);
			SpawnedNodes[index]->Status = SPAWN_DEAD;
			SpawnedNodes[index]->Unit->Delete_Me();
			SpawnedNodes[index]->Unit = 0;
			SpawnedNodes[index]->SpawnTimer = delay;
		} else {
			SpawnedNodes[index]->Status = SPAWN_DEAD;
			KamikazeInstance.Add(SpawnedNodes[index]->Unit, Target);
			SpawnedNodes[index]->Unit = 0;
			SpawnedNodes[index]->SpawnTimer = delay;
		}
	}
}

// FUNCTION: GAMEMD 0x006B7B90
void SpawnManagerClass::SetTarget(AbstractClass * target)
{
	if (target != Target) {
		NewTarget = target;
	}
}

// FUNCTION: GAMEMD 0x006B7BB0
void SpawnManagerClass::ResetTarget(void)
{
	for (int index = 0; index < SpawnedNodes.Count(); index++) {
		SpawnControl * node = SpawnedNodes[index];
		if (node->Status == SPAWN_PREPARING && node->Unit->Class->MissileSpawn) {
			KamikazeInstance.Add(node->Unit, Target);
			KamikazeInstance.UpdateTimer = 2;
			UnlinkPointer(SpawnedNodes[index]->Unit);
		}
	}
	Status = 0;
	NewTarget = 0;
	Target = 0;
}

// FUNCTION: GAMEMD 0x006B7C40
bool SpawnManagerClass::UpdateTarget(void)
{
	if (NewTarget) {
		Target = NewTarget;
		NewTarget = 0;
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x006B7C60
void SpawnManagerClass::UnlinkPointer(AbstractClass * ptr)
{
	if (ptr == Target) {
		Target = 0;
		if (!NewTarget) {
			ResetTarget();
		}
		return;
	}
	if (ptr == NewTarget) {
		NewTarget = 0;
		return;
	}
	for (int index = SpawnedNodes.Count() - 1; index >= 0; index--) {
		if (ptr == SpawnedNodes[index]->Unit) {
			SpawnControl * node = SpawnedNodes[index];
			if (node->Unit->Strength > 0 && !node->Unit->IsKamikaze && node->IsSpawnMissile != 1) {
				return;
			}
			node->Unit = 0;
			SpawnedNodes[index]->Status = SPAWN_DEAD;
			SpawnedNodes[index]->SpawnTimer = RegenRate;
			return;
		}
	}
	if (ptr == Owner) {
		KillNodes();
		ResetTarget();
	}
}

// FUNCTION: GAMEMD 0x006B7D30
int SpawnManagerClass::CountAliveSpawns(void)
{
	int count = 0;
	for (int index = 0; index < SpawnedNodes.Count(); index++) {
		if (SpawnedNodes[index]->Status != SPAWN_DEAD) {
			count++;
		}
	}
	return count;
}

// FUNCTION: GAMEMD 0x006B7D50
int SpawnManagerClass::CountDockedSpawns(void)
{
	int count = 0;
	for (int index = 0; index < SpawnedNodes.Count(); index++) {
		int status = SpawnedNodes[index]->Status;
		if (status == SPAWN_RELOADING || status == SPAWN_IDLE) {
			count++;
		}
	}
	return count;
}

// FUNCTION: GAMEMD 0x006B7D80
int SpawnManagerClass::CountLaunchingSpawns(void)
{
	int count = 0;
	for (int index = 0; index < SpawnedNodes.Count(); index++) {
		SpawnControl * node = SpawnedNodes[index];
		if (node->Status == SPAWN_TAKEOFF) {
			count++;
		}
		if (node->Status == SPAWN_PREPARING && node->Unit && !node->Unit->IsInLimbo && node->Unit->Techno_Type_Class()->MissileSpawn) {
			count++;
		}
	}
	return count;
}

// FUNCTION: GAMEMD 0x006B7DE0
void SpawnManagerClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Status);
	if (NewTarget) {
		crc(NewTarget->Fetch_ID());
	}
	if (Target) {
		crc(Target->Fetch_ID());
	}
	crc((int)SpawnTimer);
	crc((int)UpdateTimer);
	crc(SpawnedNodes.Count());
	crc(SpawnCount);
	if (SpawnType) {
		crc(SpawnType->Fetch_ID());
	}
	if (Owner) {
		crc(Owner->Fetch_ID());
	}
}

// FUNCTION: GAMEMD 0x006B7ED0
HRESULT STDMETHODCALLTYPE SpawnManagerClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SpawnManagerClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006B7F10
HRESULT STDMETHODCALLTYPE SpawnManagerClass::Load(IStream * stream)
{
	int count;
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SpawnManagerClass(NoInitClass());
		result = stream->Read(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			new (&SpawnedNodes) DynamicVectorClass<SpawnControl *>();
			int index;
			for (index = 0; index < count; index++) {
				SpawnControl * node = new SpawnControl;
				result = stream->Read(node, sizeof(SpawnControl), NULL);
				if (FAILED(result)) {
					return result;
				}
				SpawnedNodes.Add(node);
			}
			for (index = 0; index < count; index++) {
				Swizzle_Pointer(&SpawnedNodes[index]->Unit);
			}
			Swizzle_Pointer(&Owner);
			Swizzle_Pointer(&SpawnType);
			Swizzle_Pointer(&Target);
			Swizzle_Pointer(&NewTarget);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x006B80B0
HRESULT STDMETHODCALLTYPE SpawnManagerClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		int count = SpawnedNodes.Count();
		result = stream->Write(&count, sizeof(count), NULL);
		if (SUCCEEDED(result)) {
			for (int index = 0; index < count; index++) {
				result = stream->Write(SpawnedNodes[index], sizeof(SpawnControl), NULL);
				if (FAILED(result)) {
					break;
				}
			}
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x006B8120
int SpawnManagerClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x006B8130
RTTIType SpawnManagerClass::Fetch_RTTI(void) const
{
	return(RTTI_SPAWNMANAGER);
}

// SYNTHETIC: GAMEMD 0x006B8140
// ??_GSpawnManagerClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x006B8160
// ?Clear@?$DynamicVectorClass@PAUSpawnControl@SpawnManagerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006B8190
// ?ID@?$DynamicVectorClass@PAUSpawnControl@SpawnManagerClass@@@@UAEHPBQAUSpawnControl@SpawnManagerClass@@@Z
// TEMPLATE: GAMEMD 0x006B81B0
// ?GetItem@?$VectorClass@PAUSpawnControl@SpawnManagerClass@@@@UBEPAUSpawnControl@SpawnManagerClass@@H@Z
template class DynamicVectorClass<SpawnManagerClass::SpawnControl *>;

// TEMPLATE: GAMEMD 0x006B81C0
// ?Clear@?$DynamicVectorClass@PAVSpawnManagerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006B81F0
// ?ID@?$DynamicVectorClass@PAVSpawnManagerClass@@@@UAEHPBQAVSpawnManagerClass@@@Z
// TEMPLATE: GAMEMD 0x006B8210
// ?GetItem@?$VectorClass@PAVSpawnManagerClass@@@@UBEPAVSpawnManagerClass@@H@Z
template class DynamicVectorClass<SpawnManagerClass *>;

// TEMPLATE: GAMEMD 0x006B8220
// ??8?$VectorClass@PAUSpawnControl@SpawnManagerClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x006B8260
// ?Resize@?$VectorClass@PAUSpawnControl@SpawnManagerClass@@@@UAE_NHPBQAUSpawnControl@SpawnManagerClass@@@Z
// TEMPLATE: GAMEMD 0x006B8310
// ?Clear@?$VectorClass@PAUSpawnControl@SpawnManagerClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006B8340
// ?ID@?$VectorClass@PAUSpawnControl@SpawnManagerClass@@@@UAEHABQAUSpawnControl@SpawnManagerClass@@@Z
// TEMPLATE: GAMEMD 0x006B8380
// ?ID@?$VectorClass@PAUSpawnControl@SpawnManagerClass@@@@UAEHPBQAUSpawnControl@SpawnManagerClass@@@Z
template class VectorClass<SpawnManagerClass::SpawnControl *>;

// TEMPLATE: GAMEMD 0x006B83A0
// ??0?$DynamicVectorClass@PAUSpawnControl@SpawnManagerClass@@@@QAE@IPBQAUSpawnControl@SpawnManagerClass@@@Z
