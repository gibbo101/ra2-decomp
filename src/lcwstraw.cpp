// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lcwstraw.h"
#include "assert.h"
#include "lcw.h"
#include <cstring>

// FUNCTION: 0x005523E0
LCWStraw::LCWStraw(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	SafetyMargin = BlockSize/128+1;
	Buffer = new char[BlockSize+SafetyMargin];
	if (control == COMPRESS) {
		Buffer2 = new char[BlockSize+SafetyMargin];
	}
}

// FUNCTION: 0x00552450
LCWStraw::~LCWStraw(void)
{
	delete [] Buffer;
	Buffer = NULL;

	delete [] Buffer2;
	Buffer2 = NULL;
}
