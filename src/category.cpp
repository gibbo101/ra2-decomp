// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "category.h"
#include "incdec.h"
#include <cstring>

static struct {
	char const * Description;
	char const * Name;
} _categories[CATEGORY_COUNT] = {
	{ "Soldier", "Soldier" },
	{ "Civilian", "Civilian" },
	{ "VIP/Agent", "VIP" },
	{ "Recon Vehicle", "Recon" },
	{ "Armored Fighting Vehicle", "AFV" },
	{ "Infantry Fighting Vehicle", "IFV" },
	{ "Indirect Fire Support", "LRFS" },
	{ "Misc. Support Vehicle", "Support" },
	{ "Transport Vehicle", "Transport" },
	{ "Air Combat Support", "AirPower" },
	{ "Air Transport", "AirLift" }
};

// FUNCTION: GAMEMD 0x00473980
CategoryType Category_From_Name(char const * name)
{
	if (name != NULL) {
		for (CategoryType index = CATEGORY_FIRST; index < CATEGORY_COUNT; index++) {
			if (strcmpi(_categories[index].Name, name) == 0 || strcmpi(_categories[index].Description, name) == 0) {
				return(index);
			}
		}
	}
	return(CATEGORY_NONE);
}
