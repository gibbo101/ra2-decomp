// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lcwpipe.h"
#include "assert.h"
#include "lcw.h"
#include <cstring>

// FUNCTION: 0x00551FF0
LCWPipe::LCWPipe(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	int margin = BlockSize/128+1;
	SafetyMargin = margin < 128 ? 128 : margin;
	Buffer = new char[BlockSize+SafetyMargin];
	Buffer2 = new char[BlockSize+SafetyMargin];
	BlockHeader.CompCount = 0xFFFF;
}
