// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "zbuffer.h"
#include "assert.h"
#include "bsurface.h"
#include <cstdlib>
#define ZBUFFER_COLOR  0xFFFF
#define ZBUFFER_BPP    2

// FUNCTION: 0x007BCAE0
void ZBuffer::Release_Surface(void)
{
	delete SurfacePtr;
}
