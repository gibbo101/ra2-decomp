// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class AbstractClass;
template<class T> class DynamicVectorClass;

void Detach_This_From_All(AbstractClass const * target, bool all = true);
void Process_Deferred_Deletion(void);

extern DynamicVectorClass<AbstractClass *> ObjectPtrTracker;
extern DynamicVectorClass<AbstractClass *> AbstractTypePtrTracker;
extern DynamicVectorClass<AbstractClass *> TagPtrTracker;
extern DynamicVectorClass<AbstractClass *> TriggerPtrTracker;
extern DynamicVectorClass<AbstractClass *> AnimPtrTracker;
extern DynamicVectorClass<AbstractClass *> FactoryPtrTracker;
extern DynamicVectorClass<AbstractClass *> EventActionPtrTracker;
extern DynamicVectorClass<AbstractClass *> WaypointPathPtrTracker;
extern DynamicVectorClass<AbstractClass *> TeamPtrTracker;
extern DynamicVectorClass<AbstractClass *> HousePtrTracker;
extern DynamicVectorClass<AbstractClass *> NeuronPtrTracker;

extern DynamicVectorClass<AbstractClass *> ObjectsToDelete;
