// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#pragma warning(disable : 4201)
#include "always.h"
#include "cdcntrl.h"
#include "assert.h"
#include "dbgprint.h"
#include "winfix.h"
#include <cstdio>
#include <tchar.h>
#include <winioctl.h>

// FUNCTION: GAMEMD 0x00479710
CDControlClass::CDControlClass(void)
{
	for (int i = 0; i < MAX_CD_DRIVES; i++) {
		LockCount[i] = 0;
	}
}
