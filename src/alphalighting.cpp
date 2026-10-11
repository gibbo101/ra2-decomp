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
