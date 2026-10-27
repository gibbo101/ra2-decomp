// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).


#include "always.h"
#include "_logic.h"
#include "logic.h"

// SYNTHETIC: GAMEMD 0x0040CB80 SYMBOL
// _$E7
// SYNTHETIC: GAMEMD 0x0040CBC0 SYMBOL
// _$E5
LogicClass Logic;

// FUNCTION: GAMEMD 0x0040CC00
// LayerClass::Init

// FUNCTION: GAMEMD 0x0040CC10
// LayerClass::One_Time

// SYNTHETIC: GAMEMD 0x0040CC20
// ??_GLogicClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0040CC70
// ?Clear@?$DynamicVectorClass@PAVObjectClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040CCA0
// ?ID@?$DynamicVectorClass@PAVObjectClass@@@@UAEHPBQAVObjectClass@@@Z
// TEMPLATE: GAMEMD 0x0040CCC0
// ?GetItem@?$VectorClass@PAVObjectClass@@@@UBEPAVObjectClass@@H@Z
template class DynamicVectorClass<ObjectClass *>;

// TEMPLATE: GAMEMD 0x0040CCD0
// ??8?$VectorClass@PAVObjectClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040CD10
// ?Resize@?$VectorClass@PAVObjectClass@@@@UAE_NHPBQAVObjectClass@@@Z
// TEMPLATE: GAMEMD 0x0040CDC0
// ?Clear@?$VectorClass@PAVObjectClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040CDF0
// ?ID@?$VectorClass@PAVObjectClass@@@@UAEHABQAVObjectClass@@@Z
template class VectorClass<ObjectClass *>;
