// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "shastraw.h"

#include <math.h>

// FUNCTION: GAMEMD 0x0069E9F0
int SHAStraw::Get(void * source, int slen)
{
	if (source == NULL || slen < 1) {
		return(0);
	}

	int counter = BASECLASS::Get(source, slen);
	if (!IsDisabled) {
		SHA.Hash(source, counter);
	}
	return(counter);
}

// FUNCTION: GAMEMD 0x0069EA30
int SHAStraw::Result(void * result) const
{
	return(SHA.Result(result));
}
