// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "crcstraw.h"

// FUNCTION: 0x004A20B0
int CRCStraw::Get(void * source, int slen)
{
	if (source == NULL || slen < 1) {
		return(0);
	}

	int counter = BASECLASS::Get(source, slen);
	CRC(source, counter);
	return(counter);
}

// FUNCTION: 0x004A20F0
int CRCStraw::Result(void) const
{
	return(CRC());
}
