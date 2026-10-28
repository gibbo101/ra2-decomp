// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"

#include "tracker.h"

#include "_logic.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "abstract.h"
#include "aircraft.h"
#include "building.h"
#include "infantry.h"
#include "object.h"
#include "partsys.h"
#include "rules.h"
#include "tactical.h"
#include "unit.h"
#include "vector.h"

#include <typeinfo>

// SYNTHETIC: GAMEMD 0x007250C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x007250F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00725110 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00725130 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00725150 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00725170 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00725190 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x007251D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00725200 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00725230 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00725260 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00725270 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00725290 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x007252B0 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x007252D0 SYMBOL
// _$E134
DynamicVectorClass<AbstractClass *> ObjectPtrTracker;

// SYNTHETIC: GAMEMD 0x00725310 SYMBOL
// _$E132

// SYNTHETIC: GAMEMD 0x00725350 SYMBOL
// _$E139
DynamicVectorClass<AbstractClass *> AbstractTypePtrTracker;

// SYNTHETIC: GAMEMD 0x00725390 SYMBOL
// _$E137

// SYNTHETIC: GAMEMD 0x007253D0 SYMBOL
// _$E144
DynamicVectorClass<AbstractClass *> TagPtrTracker;

// SYNTHETIC: GAMEMD 0x00725410 SYMBOL
// _$E142

// SYNTHETIC: GAMEMD 0x00725450 SYMBOL
// _$E149
DynamicVectorClass<AbstractClass *> TriggerPtrTracker;

// SYNTHETIC: GAMEMD 0x00725490 SYMBOL
// _$E147

// SYNTHETIC: GAMEMD 0x007254D0 SYMBOL
// _$E154
DynamicVectorClass<AbstractClass *> AnimPtrTracker;

// SYNTHETIC: GAMEMD 0x00725510 SYMBOL
// _$E152

// SYNTHETIC: GAMEMD 0x00725550 SYMBOL
// _$E159
DynamicVectorClass<AbstractClass *> FactoryPtrTracker;

// SYNTHETIC: GAMEMD 0x00725590 SYMBOL
// _$E157

// SYNTHETIC: GAMEMD 0x007255D0 SYMBOL
// _$E164
DynamicVectorClass<AbstractClass *> EventActionPtrTracker;

// SYNTHETIC: GAMEMD 0x00725610 SYMBOL
// _$E162

// SYNTHETIC: GAMEMD 0x00725650 SYMBOL
// _$E169
DynamicVectorClass<AbstractClass *> WaypointPathPtrTracker;

// SYNTHETIC: GAMEMD 0x00725690 SYMBOL
// _$E167

// SYNTHETIC: GAMEMD 0x007256D0 SYMBOL
// _$E174
DynamicVectorClass<AbstractClass *> TeamPtrTracker;

// SYNTHETIC: GAMEMD 0x00725710 SYMBOL
// _$E172

// SYNTHETIC: GAMEMD 0x00725750 SYMBOL
// _$E179
DynamicVectorClass<AbstractClass *> HousePtrTracker;

// SYNTHETIC: GAMEMD 0x00725790 SYMBOL
// _$E177

// SYNTHETIC: GAMEMD 0x007257D0 SYMBOL
// _$E184
DynamicVectorClass<AbstractClass *> NeuronPtrTracker;

// SYNTHETIC: GAMEMD 0x00725810 SYMBOL
// _$E182

// SYNTHETIC: GAMEMD 0x00725850 SYMBOL
// _$E189
DynamicVectorClass<AbstractClass *> ObjectsToDelete;

// SYNTHETIC: GAMEMD 0x00725890 SYMBOL
// _$E187

#include "bomblist.h"
#include "disklaser.h"
#include "foot.h"
#include "kamikaze.h"

void __fastcall FUN_00733160(AbstractClass * target);

static inline bool Is_Object(AbstractClass const * a)
{
	if (a == NULL) return(false);
	unsigned char flag = (a->AbstractFlags >> 1) & 1;
	return(flag != 0);
}

// FUNCTION: GAMEMD 0x007258D0
void Detach_This_From_All(AbstractClass const * target, bool all)
{
	int index = 0;
	RTTIType rtti = target->RTTI;

	if (Map.PendingObjectPtr == target) {
		Map.PendingObjectPtr = NULL;
	}

	if (Map.PendingObject == target) {
		Map.PendingObject = NULL;
		Map.SetActiveFoundation(NULL);
	}

	if (rtti == RTTI_PARTICLESYSTEM && target == GasSystem) {
		GasSystem = NULL;
	}

	if (rtti == RTTI_HOUSE) {
		for (index = 0; index < HousePtrTracker.Count(); index++) {
			HousePtrTracker[index]->Detach(target, all);
		}
		Logic.Detach(target, all);
		return;
	}

	if (rtti == RTTI_ANIM) {
		for (index = 0; index < AnimPtrTracker.Count(); index++) {
			AnimPtrTracker[index]->Detach(target, all);
		}
		return;
	}

	if (Is_Object(target)) {
		for (index = 0; index < ObjectPtrTracker.Count(); index++) {
			ObjectPtrTracker[index]->Detach(target, all);
		}
		BombListClass::Instance.PointerGotInvalid((AbstractClass *)target);
		KamikazeInstance.Remove((AbstractClass *)target);
		for (index = DiskLasers.Count() - 1; index >= 0; index--) {
			DiskLasers[index]->PointerGotInvalid((AbstractClass *)target);
		}
		if (rtti == RTTI_INFANTRY || rtti == RTTI_UNIT || rtti == RTTI_AIRCRAFT) {
			AircraftTracker.FUN_00413490((AbstractClass *)target);
		}
		FUN_00733160((AbstractClass *)target);
		if (TacticalMap != NULL) {
			TacticalMap->Detach(target, true);
		}
		Logic.Detach(target, all);
		return;
	}

	if (target->As_AbstractTypeClass() != NULL) {
		for (index = 0; index < AbstractTypePtrTracker.Count(); index++) {
			AbstractTypePtrTracker[index]->Detach(target, all);
		}
		Rule->Detach(target, all);
		return;
	}

	switch (rtti)
	{
		case RTTI_TAG:
			for (index = 0; index < TagPtrTracker.Count(); index++) {
				TagPtrTracker[index]->Detach(target, all);
			}
			Map.Detach(target, true);
			Logic.Detach(target, all);
			break;

		case RTTI_TRIGGER:
			for (index = 0; index < TriggerPtrTracker.Count(); index++) {
				TriggerPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_FACTORY:
			for (index = 0; index < FactoryPtrTracker.Count(); index++) {
				FactoryPtrTracker[index]->Detach(target, all);
			}
			Map.Detach(target, true);
			break;

		case RTTI_EVENT:
		case RTTI_ACTION:
			for (index = 0; index < EventActionPtrTracker.Count(); index++) {
				EventActionPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_WAYPOINT:
			for (index = 0; index < WaypointPathPtrTracker.Count(); index++) {
				WaypointPathPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_TEAM:
			for (index = 0; index < TeamPtrTracker.Count(); index++) {
				TeamPtrTracker[index]->Detach(target, all);
			}
			break;

		case RTTI_NEURON:
			for (index = 0; index < NeuronPtrTracker.Count(); index++) {
				NeuronPtrTracker[index]->Detach(target, all);
			}
			break;
	}
}

// FUNCTION: GAMEMD 0x00725C70
void Process_Deferred_Deletion(void)
{
	int index = 0;
	while (index < ObjectsToDelete.Count()) {
		AbstractClass *obj = ObjectsToDelete[index];
		if (obj->Is_Inactive()) {
			while (true) {
				if (!ObjectsToDelete.Delete(obj)) {
					break;
				}
			}
			if (obj->Release()) {
				if (typeid(BuildingClass) == typeid(*obj)
				 || typeid(UnitClass) == typeid(*obj)
				 || typeid(InfantryClass) == typeid(*obj)
				 || typeid(AircraftClass) == typeid(*obj)) {
					((ObjectClass *)obj)->IsActive = true;
				}
				delete obj;
			}
		} else {
			++index;
		}
	}
}
