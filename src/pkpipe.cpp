// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "pkpipe.h"
#include <cstring>

// FUNCTION: 0x00632CB0
PKPipe::PKPipe(CryptControl control, RandomStraw & rnd) :
	IsGettingKey(true),
	Rand(rnd),
	BF((control == ENCRYPT) ? BlowPipe::ENCRYPT : BlowPipe::DECRYPT),
	Control(control),
	CipherKey(NULL),
	Counter(0),
	BytesLeft(0)
{
}

// FUNCTION: 0x00632D10
void PKPipe::Put_To(Pipe * pipe)
{
	if (BF.ChainTo != pipe) {
		if (pipe != NULL && pipe->ChainFrom != NULL) {
			pipe->ChainFrom->Put_To(NULL);
			pipe->ChainFrom = NULL;
		}

		if (BF.ChainTo != NULL) {
			BF.ChainTo->ChainFrom = NULL;
		}
		BF.ChainTo = pipe;
		if (pipe != NULL) {
			pipe->ChainFrom = &BF;
		}
		BF.ChainFrom = this;
		ChainTo = &BF;
	}
}
