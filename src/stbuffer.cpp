// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "stbuffer.h"
#include "rle.h"
#include "surface.h"
#include <cstring>

// FUNCTION: 0x006C89B0
StaticBufferClass::StaticBufferClass(int size)
{
	Size = size;
	Buffer = new unsigned char[size];
	Cursor = Buffer;
}

// FUNCTION: 0x006C89D0
StaticBufferClass::~StaticBufferClass(void)
{
	if (Buffer != NULL) {
		delete [] Buffer;
	}
}
