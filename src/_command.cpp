// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "_command.h"

#include "index.h"
#include "rect.h"
#include "vector.h"

class CommandClass;

IndexClass<int, CommandClass const *> HotkeyCommands;
DynamicVectorClass<CommandClass const *> AllCommands;

// SYNTHETIC: GAMEMD 0x0040BD50 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040BD70 SYMBOL
// _$E7
// SYNTHETIC: GAMEMD 0x0040BDA0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040BDD0 SYMBOL
// _$E12
// SYNTHETIC: GAMEMD 0x0040BE10 SYMBOL
// _$E10

// TEMPLATE: GAMEMD 0x0040BE50
// ?Clear@?$DynamicVectorClass@PBVCommandClass@@@@UAEXXZ
template class DynamicVectorClass<CommandClass const *>;

// TEMPLATE: GAMEMD 0x0040BE80
// ?ID@?$DynamicVectorClass@PBVCommandClass@@@@UAEHPBQBVCommandClass@@@Z
// TEMPLATE: GAMEMD 0x0040BEA0
// ?GetItem@?$VectorClass@PBVCommandClass@@@@UBEPBVCommandClass@@H@Z
// TEMPLATE: GAMEMD 0x0040BEB0
// ??8?$VectorClass@PBVCommandClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040BEF0
// ?Resize@?$VectorClass@PBVCommandClass@@@@UAE_NHPBQBVCommandClass@@@Z
// TEMPLATE: GAMEMD 0x0040BFA0
// ?Clear@?$VectorClass@PBVCommandClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040BFD0
// ?ID@?$VectorClass@PBVCommandClass@@@@UAEHABQBVCommandClass@@@Z
