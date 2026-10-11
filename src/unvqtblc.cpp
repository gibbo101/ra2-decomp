// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "unvqtblc.h"
#include "_vqa.h"
#include "assert.h"
#include "dsurface.h"

// FUNCTION: 0x00748380
void Hicolor_Translate(void * buffer, int size)
{
	unsigned short * buf = (unsigned short *)buffer;
	while (size > 0) {
		*buf = HicolorTable[*buf];
		buf++;
		size--;
	}
}
