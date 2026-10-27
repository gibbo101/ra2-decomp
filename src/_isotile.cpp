// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "_isotile.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x0040C590 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040C5C0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040C5E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040C600 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040C620 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0040C640 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0040C660 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0040C6A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0040C6D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0040C700 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0040C730 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0040C740 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x0040C760 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0040C780 SYMBOL
// _$E41

DynamicVectorClass<IsometricTileClass *> IsometricTiles;

// SYNTHETIC: GAMEMD 0x0040C7A0 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x0040C7E0 SYMBOL
// _$E44

// TEMPLATE: GAMEMD 0x0040C820
// ?Clear@?$DynamicVectorClass@PAVIsometricTileClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040C850
// ?ID@?$DynamicVectorClass@PAVIsometricTileClass@@@@UAEHPBQAVIsometricTileClass@@@Z
// TEMPLATE: GAMEMD 0x0040C870
// ?GetItem@?$VectorClass@PAVIsometricTileClass@@@@UBEPAVIsometricTileClass@@H@Z
// TEMPLATE: GAMEMD 0x0040C880
// ??8?$VectorClass@PAVIsometricTileClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040C8C0
// ?Resize@?$VectorClass@PAVIsometricTileClass@@@@UAE_NHPBQAVIsometricTileClass@@@Z
// TEMPLATE: GAMEMD 0x0040C970
// ?Clear@?$VectorClass@PAVIsometricTileClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040C9A0
// ?ID@?$VectorClass@PAVIsometricTileClass@@@@UAEHABQAVIsometricTileClass@@@Z
// TEMPLATE: GAMEMD 0x0040C9E0
// ?ID@?$VectorClass@PAVIsometricTileClass@@@@UAEHPBQAVIsometricTileClass@@@Z

template class VectorClass<IsometricTileClass *>;
template class DynamicVectorClass<IsometricTileClass *>;
