// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "_astar.h"
#include "astar.h"
#include "session.h"
#include "map.h"
#include "vector.h"
#include "geometry.h"

AStarClass Search;

// SYNTHETIC: GAMEMD 0x0040AD90 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x0040ADC0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x0040ADE0 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x0040AE00 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x0040AE20 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x0040AE40 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x0040AE60 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x0040AEA0 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x0040AED0 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x0040AF00 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x0040AF30 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x0040AF40 SYMBOL
// _$E67
// SYNTHETIC: GAMEMD 0x0040AF60 SYMBOL
// _$E70
// SYNTHETIC: GAMEMD 0x0040AF80 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x0040AFA0 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x0040AFC0 SYMBOL
// _$E73

// FUNCTION: GAMEMD 0x0040AFD0
// ??_I@YGXPAXIHP6EX0@Z@Z
// SYNTHETIC: GAMEMD 0x0040B010
// ??_EWstring@@QAEPAXI@Z
// FUNCTION: GAMEMD 0x0040B070
// ??_ESubzoneTrackingStruct@@QAEPAXI@Z
// FUNCTION: GAMEMD 0x0040B120
// ?GetItem@?$VectorClass@USubzoneConnectionStruct@@@@UBE?AUSubzoneConnectionStruct@@H@Z
// FUNCTION: GAMEMD 0x0040B140
// ??8?$VectorClass@USubzoneConnectionStruct@@@@UBE_NABV0@@Z
// FUNCTION: GAMEMD 0x0040B180
// ?Resize@?$VectorClass@USubzoneConnectionStruct@@@@UAE_NHPBUSubzoneConnectionStruct@@@Z
// FUNCTION: GAMEMD 0x0040B270
// ?Clear@?$VectorClass@USubzoneConnectionStruct@@@@UAEXXZ
// FUNCTION: GAMEMD 0x0040B2A0
// ?ID@?$VectorClass@USubzoneConnectionStruct@@@@UAEHABUSubzoneConnectionStruct@@@Z
// TEMPLATE: GAMEMD 0x0040B2E0
// ?ID@?$VectorClass@USubzoneConnectionStruct@@@@UAEHPBUSubzoneConnectionStruct@@@Z
// SYNTHETIC: GAMEMD 0x0040B300
// ??_G?$VectorClass@USubzoneConnectionStruct@@@@UAEPAXI@Z
