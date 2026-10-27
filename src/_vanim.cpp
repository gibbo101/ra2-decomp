// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "_vanim.h"
#include "vanim.h"
#include "vector.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0040EC10 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040EC30 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040EC60 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040EC80 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040ECA0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0040ECC0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0040ECE0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0040ED00 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0040ED40 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0040ED70 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0040EDA0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0040EDD0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0040EDE0 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x0040EE00 SYMBOL
// _$E41

const Rect SecondRectNone(0, 0, 0, 0);

// SYNTHETIC: GAMEMD 0x0040EE20 SYMBOL
// _$E44

DynamicVectorClass<VoxelAnimClass *> VoxelAnims;

// SYNTHETIC: GAMEMD 0x0040EE40 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x0040EE80 SYMBOL
// _$E47

// TEMPLATE: GAMEMD 0x0040EEC0
// ?Clear@?$DynamicVectorClass@PAVVoxelAnimClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040EEF0
// ?ID@?$DynamicVectorClass@PAVVoxelAnimClass@@@@UAEHPBQAVVoxelAnimClass@@@Z
template class DynamicVectorClass<VoxelAnimClass *>;

// TEMPLATE: GAMEMD 0x0040EF10
// ?GetItem@?$VectorClass@PAVVoxelAnimClass@@@@UBEPAVVoxelAnimClass@@H@Z
// TEMPLATE: GAMEMD 0x0040EF20
// ??8?$VectorClass@PAVVoxelAnimClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040EF60
// ?Resize@?$VectorClass@PAVVoxelAnimClass@@@@UAE_NHPBQAVVoxelAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0040F010
// ?Clear@?$VectorClass@PAVVoxelAnimClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040F040
// ?ID@?$VectorClass@PAVVoxelAnimClass@@@@UAEHABQAVVoxelAnimClass@@@Z
// TEMPLATE: GAMEMD 0x0040F080
// ?ID@?$VectorClass@PAVVoxelAnimClass@@@@UAEHPBQAVVoxelAnimClass@@@Z
template class VectorClass<VoxelAnimClass *>;
