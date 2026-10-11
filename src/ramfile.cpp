// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ramfile.h"
#include <cstring>

// FUNCTION: 0x0065C340
int RAMFileClass::Open(char const *, int access)
{
	return(Open(access));
}
