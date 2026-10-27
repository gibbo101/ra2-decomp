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
