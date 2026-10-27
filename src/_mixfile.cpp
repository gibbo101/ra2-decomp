// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "_mixfile.h"
#include "vector.h"

DynamicVectorClass<MFCD *> ExpandMix;
DynamicVectorClass<MFCD *> ExpandSpeechMix;
DynamicVectorClass<MFCD *> ExpandSideMix;
DynamicVectorClass<MFCD *> MoviesMixLocal;

// SYNTHETIC: GAMEMD 0x0040D340 SYMBOL
// _$E4

// SYNTHETIC: GAMEMD 0x0040D380 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x0040D3C0 SYMBOL
// _$E9

// SYNTHETIC: GAMEMD 0x0040D400 SYMBOL
// _$E7

// SYNTHETIC: GAMEMD 0x0040D440 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x0040D480 SYMBOL
// _$E12

// SYNTHETIC: GAMEMD 0x0040D4C0 SYMBOL
// _$E19

// SYNTHETIC: GAMEMD 0x0040D500 SYMBOL
// _$E17

// TEMPLATE: GAMEMD 0x0040D540
// ?Clear@?$DynamicVectorClass@PAVMixFileClass@@@@UAEXXZ
template class DynamicVectorClass<MFCD *>;

// TEMPLATE: GAMEMD 0x0040D570
// ?ID@?$DynamicVectorClass@PAVMixFileClass@@@@UAEHPBQAVMixFileClass@@@Z

// TEMPLATE: GAMEMD 0x0040D590
// ?GetItem@?$VectorClass@PAVMixFileClass@@@@UBEPAVMixFileClass@@H@Z

// TEMPLATE: GAMEMD 0x0040D5A0
// ??8?$VectorClass@PAVMixFileClass@@@@UBE_NABV0@@Z
