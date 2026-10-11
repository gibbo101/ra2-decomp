// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "crcpipe.h"

// FUNCTION: 0x004A2030
int CRCPipe::Put(void const * source, int slen)
{
	CRC(source, slen);
	return(BASECLASS::Put(source, slen));
}
