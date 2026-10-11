// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "abuffer.h"
#include "assert.h"
#include "bsurface.h"
#include <cstdlib>
#define ABUFFER_COLOR  0x007F
#define ABUFFER_MAX    0x8000
#define ABUFFER_BPP    2

// FUNCTION: 0x00410E50
void ABuffer::Release_Surface(void)
{
	delete SurfacePtr;
	SurfacePtr = NULL;
}
