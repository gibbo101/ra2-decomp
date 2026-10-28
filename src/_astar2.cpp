// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include <windows.h>
#include "always.h"
#include "geometry.h"
#include "bomblist.h"

BombListClass BombListInstance;

// SYNTHETIC: GAMEMD 0x0040B350 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0040B380 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0040B3A0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0040B3C0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0040B3E0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0040B400 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0040B420 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0040B460 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0040B490 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0040B4C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0040B4F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0040B500 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0040B520 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0040B540 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x0040B5C0 SYMBOL
// _$E41

template class VectorClass<BombClass *>;
template class DynamicVectorClass<BombClass *>;
template class VectorClass<TechnoClass *>;
template class DynamicVectorClass<TechnoClass *>;

// TEMPLATE: GAMEMD 0x0040B640
// ?Clear@?$DynamicVectorClass@PAVBombClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040B670
// ?ID@?$DynamicVectorClass@PAVBombClass@@@@UAEHPBQAVBombClass@@@Z
// TEMPLATE: GAMEMD 0x0040B690
// ?GetItem@?$VectorClass@PAVBombClass@@@@UBEPAVBombClass@@H@Z
// TEMPLATE: GAMEMD 0x0040B6A0
// ?Clear@?$DynamicVectorClass@PAVTechnoClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040B6D0
// ?ID@?$DynamicVectorClass@PAVTechnoClass@@@@UAEHPBQAVTechnoClass@@@Z
// TEMPLATE: GAMEMD 0x0040B6F0
// ?GetItem@?$VectorClass@PAVTechnoClass@@@@UBEPAVTechnoClass@@H@Z
// TEMPLATE: GAMEMD 0x0040B700
// ??8?$VectorClass@PAVBombClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040B740
// ?Resize@?$VectorClass@PAVBombClass@@@@UAE_NHPBQAVBombClass@@@Z
// TEMPLATE: GAMEMD 0x0040B7F0
// ?Clear@?$VectorClass@PAVBombClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040B820
// ?ID@?$VectorClass@PAVBombClass@@@@UAEHABQAVBombClass@@@Z
// TEMPLATE: GAMEMD 0x0040B860
// ?ID@?$VectorClass@PAVBombClass@@@@UAEHPBQAVBombClass@@@Z
// TEMPLATE: GAMEMD 0x0040B880
// ?Resize@?$DynamicVectorClass@PAVBombClass@@@@UAE_NHPBQAVBombClass@@@Z
// TEMPLATE: GAMEMD 0x0040B930
// ?ID@?$DynamicVectorClass@PAVBombClass@@@@UAEHABQAVBombClass@@@Z
// TEMPLATE: GAMEMD 0x0040B960
// ??8?$VectorClass@PAVTechnoClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0040B9A0
// ?Resize@?$VectorClass@PAVTechnoClass@@@@UAE_NHPBQAVTechnoClass@@@Z
// TEMPLATE: GAMEMD 0x0040BA50
// ?Clear@?$VectorClass@PAVTechnoClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0040BA80
// ?ID@?$VectorClass@PAVTechnoClass@@@@UAEHABQAVTechnoClass@@@Z
// TEMPLATE: GAMEMD 0x0040BAC0
// ?ID@?$VectorClass@PAVTechnoClass@@@@UAEHPBQAVTechnoClass@@@Z
// TEMPLATE: GAMEMD 0x0040BAE0
// ?Resize@?$DynamicVectorClass@PAVTechnoClass@@@@UAE_NHPBQAVTechnoClass@@@Z
// TEMPLATE: GAMEMD 0x0040BB90
// ?ID@?$DynamicVectorClass@PAVTechnoClass@@@@UAEHABQAVTechnoClass@@@Z
// SYNTHETIC: GAMEMD 0x0040BBC0
// ??_G?$VectorClass@PAVBombClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040BC10
// ??_G?$DynamicVectorClass@PAVBombClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040BC60
// ??_G?$VectorClass@PAVTechnoClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0040BCB0
// ??_G?$DynamicVectorClass@PAVTechnoClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0040BD00
// ??0?$VectorClass@PAVTechnoClass@@@@QAE@HPBQAVTechnoClass@@@Z
