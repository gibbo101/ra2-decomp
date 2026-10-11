// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rawfile.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <direct.h>
#include <share.h>

// FUNCTION: 0x0065CB30
int RawFileClass::Open(char const * filename, int rights)
{
	Set_Name(filename);
	return(Open(rights));
}
