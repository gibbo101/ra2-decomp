// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tag.h"
#include "_map.h"
#include "cell.h"
#include "crc.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "noinit.h"
#include "sun.h"
#include "swizzle.h"
#include "tagtype.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "vector.h"

// FUNCTION: 0x006E5570
void TagClass::Init(void)
{
	while (Tags.Count() > 0) {
		delete Tags[0];
	}
}
