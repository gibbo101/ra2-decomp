// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "alphalighting.h"
#include "vector.h"

// FUNCTION: 0x00420140
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

// FUNCTION: 0x00420270
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

// FUNCTION: 0x004202F0
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

// FUNCTION: 0x00420360
AlphaLightingRemapInitClass::~AlphaLightingRemapInitClass(void)
{
}

// FUNCTION: 0x00420370 ?$DynamicVectorClass@PAVAlphaLightingRemapClass@@::Clear
template class DynamicVectorClass<AlphaLightingRemapClass *>;

// FUNCTION: 0x004203A0 ?$DynamicVectorClass@PAVAlphaLightingRemapClass@@::ID

// FUNCTION: 0x004203C0 ?$VectorClass@PAVAlphaLightingRemapClass@@::GetItem

// FUNCTION: 0x004203D0 ?$VectorClass@PAVAlphaLightingRemapClass@@::operator==

// FUNCTION: 0x00420410 ?$VectorClass@PAVAlphaLightingRemapClass@@::Resize

// FUNCTION: 0x004204C0 ?$VectorClass@PAVAlphaLightingRemapClass@@::Clear

// FUNCTION: 0x004204F0 ?$VectorClass@PAVAlphaLightingRemapClass@@::ID

// FUNCTION: 0x00420530 ?$VectorClass@PAVAlphaLightingRemapClass@@::ID

// FUNCTION: 0x00420550 ?$DynamicVectorClass@PAVAlphaLightingRemapClass@@::Resize

// FUNCTION: 0x00420600 ?$DynamicVectorClass@PAVAlphaLightingRemapClass@@::ID

// FUNCTION: 0x00420630 ?$VectorClass@PAVAlphaLightingRemapClass@@::~?$VectorClass@PAVAlphaLightingRemapClass@@
template class VectorClass<AlphaLightingRemapClass *>;

// FUNCTION: 0x00420680 ?$DynamicVectorClass@PAVAlphaLightingRemapClass@@::~?$DynamicVectorClass@PAVAlphaLightingRemapClass@@

struct AlphaRectBlock {
	int A, B, C, D;
};
extern AlphaRectBlock DAT_0088A0A8;

// FUNCTION: 0x004206D0
void FUN_004206D0(void)
{
	DAT_0088A0A8.A = 0;
	DAT_0088A0A8.B = 0;
	DAT_0088A0A8.C = 0;
	DAT_0088A0A8.D = 0;
}
