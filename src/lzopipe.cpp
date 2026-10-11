// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "lzopipe.h"
#include "assert.h"
#include "lzo.h"
#include <cstring>

// FUNCTION: 0x0055C2B0
LZOPipe::LZOPipe(CompControl control, int blocksize) :
		Control(control),
		Counter(0),
		Buffer(NULL),
		Buffer2(NULL),
		BlockSize(blocksize)
{
	SafetyMargin = BlockSize;
	Buffer = new char[BlockSize+SafetyMargin];
	Buffer2 = new char[BlockSize+SafetyMargin];
	BlockHeader.CompCount = 0xFFFF;
}

// FUNCTION: 0x0055C310
LZOPipe::~LZOPipe(void)
{
	delete [] Buffer;
	Buffer = NULL;

	delete [] Buffer2;
	Buffer2 = NULL;
}
