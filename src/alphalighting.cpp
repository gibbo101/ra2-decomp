// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "alphalighting.h"
#include "vector.h"

// SYNTHETIC: GAMEMD 0x00420080 SYMBOL
// _$E6

// SYNTHETIC: GAMEMD 0x004200C0 SYMBOL
// _$E3
DynamicVectorClass<AlphaLightingRemapClass *> AlphaLightingRemapClass::AlphaLightingRemaps;

// FUNCTION: GAMEMD 0x00420120
AlphaLightingRemapInitClass::AlphaLightingRemapInitClass(void)
{
}

// FUNCTION: GAMEMD 0x00420130
AlphaLightingRemapInitClass::~AlphaLightingRemapInitClass(void)
{
}

// FUNCTION: GAMEMD 0x00420140
AlphaLightingRemapClass *AlphaLightingRemapInitClass::Init(int count)
{
	for (int i = 0; i < AlphaLightingRemapClass::AlphaLightingRemaps.Count(); i++) {
		if (AlphaLightingRemapClass::AlphaLightingRemaps[i]->Count == count) {
			AlphaLightingRemapClass::AlphaLightingRemaps[i]->RefCount++;
			return(AlphaLightingRemapClass::AlphaLightingRemaps[i]);
		}
	}

	AlphaLightingRemapClass *ptr = new AlphaLightingRemapClass(count);
	AlphaLightingRemapClass::AlphaLightingRemaps.Add(ptr);
	ptr->RefCount++;
	return(ptr);
}

// FUNCTION: GAMEMD 0x00420270
void AlphaLightingRemapInitClass::Deinit(AlphaLightingRemapClass *ptr)
{
	if (ptr != NULL) {
		AlphaLightingRemapClass *remap = ptr;
		remap->RefCount--;
		if (remap->RefCount == 0) {
			AlphaLightingRemapClass::AlphaLightingRemaps.Delete(remap);
			delete remap;
		}
	}
}

// FUNCTION: GAMEMD 0x004202F0
AlphaLightingRemapClass::AlphaLightingRemapClass(int count)
{
	Count = count;
	RefCount = 0;

	for (int i = 0; i <= BUFFER_SIZE - 1; i++) {
		int n2 = (int)((i & 0xFF) * (i >> 8) * (count - 1)) / 32258;
		if (n2 > count - 1) {
			n2 = count - 1;
		}
		Buffer[0][i] = (unsigned char)n2 << 8;
	}
}

// FUNCTION: GAMEMD 0x00420360
AlphaLightingRemapClass::~AlphaLightingRemapClass(void)
{
}

// TEMPLATE: GAMEMD 0x00420370
// ?Clear@?$DynamicVectorClass@PAVAlphaLightingRemapClass@@@@UAEXXZ
template class DynamicVectorClass<AlphaLightingRemapClass *>;

// TEMPLATE: GAMEMD 0x004203A0
// ?ID@?$DynamicVectorClass@PAVAlphaLightingRemapClass@@@@UAEHPBQAVAlphaLightingRemapClass@@@Z

// TEMPLATE: GAMEMD 0x004203C0
// ?GetItem@?$VectorClass@PAVAlphaLightingRemapClass@@@@UBEPAVAlphaLightingRemapClass@@H@Z

// TEMPLATE: GAMEMD 0x004203D0
// ??8?$VectorClass@PAVAlphaLightingRemapClass@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x00420410
// ?Resize@?$VectorClass@PAVAlphaLightingRemapClass@@@@UAE_NHPBQAVAlphaLightingRemapClass@@@Z

// TEMPLATE: GAMEMD 0x004204C0
// ?Clear@?$VectorClass@PAVAlphaLightingRemapClass@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x004204F0
// ?ID@?$VectorClass@PAVAlphaLightingRemapClass@@@@UAEHABQAVAlphaLightingRemapClass@@@Z

// TEMPLATE: GAMEMD 0x00420530
// ?ID@?$VectorClass@PAVAlphaLightingRemapClass@@@@UAEHPBQAVAlphaLightingRemapClass@@@Z

// TEMPLATE: GAMEMD 0x00420550
// ?Resize@?$DynamicVectorClass@PAVAlphaLightingRemapClass@@@@UAE_NHPBQAVAlphaLightingRemapClass@@@Z

// TEMPLATE: GAMEMD 0x00420600
// ?ID@?$DynamicVectorClass@PAVAlphaLightingRemapClass@@@@UAEHABQAVAlphaLightingRemapClass@@@Z

// SYNTHETIC: GAMEMD 0x00420630
// ??_G?$VectorClass@PAVAlphaLightingRemapClass@@@@UAEPAXI@Z
template class VectorClass<AlphaLightingRemapClass *>;

// SYNTHETIC: GAMEMD 0x00420680
// ??_G?$DynamicVectorClass@PAVAlphaLightingRemapClass@@@@UAEPAXI@Z
