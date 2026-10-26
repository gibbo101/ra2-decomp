// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

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

// SYNTHETIC: GAMEMD 0x007252D0 SYMBOL
// _$E89
DynamicVectorClass<AbstractClass *> ObjectPtrTracker;

// SYNTHETIC: GAMEMD 0x00725310 SYMBOL
// _$E87

// SYNTHETIC: GAMEMD 0x00725350 SYMBOL
// _$E94
DynamicVectorClass<AbstractClass *> AbstractTypePtrTracker;

// SYNTHETIC: GAMEMD 0x00725390 SYMBOL
// _$E92

// SYNTHETIC: GAMEMD 0x007253D0 SYMBOL
// _$E99
DynamicVectorClass<AbstractClass *> TagPtrTracker;

// SYNTHETIC: GAMEMD 0x00725410 SYMBOL
// _$E97

// SYNTHETIC: GAMEMD 0x00725450 SYMBOL
// _$E104
DynamicVectorClass<AbstractClass *> TriggerPtrTracker;

// SYNTHETIC: GAMEMD 0x00725490 SYMBOL
// _$E102

// SYNTHETIC: GAMEMD 0x007254D0 SYMBOL
// _$E109
DynamicVectorClass<AbstractClass *> AnimPtrTracker;

// SYNTHETIC: GAMEMD 0x00725510 SYMBOL
// _$E107

// SYNTHETIC: GAMEMD 0x00725550 SYMBOL
// _$E114
DynamicVectorClass<AbstractClass *> FactoryPtrTracker;

// SYNTHETIC: GAMEMD 0x00725590 SYMBOL
// _$E112
