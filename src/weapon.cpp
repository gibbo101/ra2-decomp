// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "weapon.h"
#include "_rules.h"
#include "_weapon.h"
#include "animtype.h"
#include "bullettype.h"
#include "combat.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "psystype.h"
#include "rules.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "warhead.h"
#include <cstring>

// FUNCTION: 0x00772A50
ArmorType Armor_From_Name(char const * name)
{
	if (!name) return(ARMOR_NONE);

	for (ArmorType index = ARMOR_FIRST; index < ARMOR_COUNT; index++) {
		if (stricmp(ArmorName[index], name) == 0) {
			return(index);
		}
	}

	return(ARMOR_NONE);
}
