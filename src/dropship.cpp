// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dropship.h"
#include "_alpha.h"
#include "_convert.h"
#include "_font.h"
#include "_keyboar.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "_zbuffer.h"
#include "bsurface.h"
#include "ccfile.h"
#include "ccrand.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "draw.h"
#include "dsurface.h"
#include "fastmath.h"
#include "font.h"
#include "globals.h"
#include "house.h"
#include "infatype.h"
#include "keyboard.h"
#include "language\language.h"
#include "misc.h"
#include "mixfile.h"
#include "mouse.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "shapebtn.h"
#include "shapeset.h"
#include "stimer.h"
#include "swizzle.h"
#include "techtype.h"
#include "unittype.h"
#include "weapon.h"
#include "zbuffer.h"
#include <climits>
#include <cstring>

// FUNCTION: 0x004B69B0
DropshipLoadoutClass::DropshipLoadoutClass(void) :
	CreationFrame(Frame),
	Unused2(INT_MAX),
	UnusedBool1(false),
	EntryCount(0),
	TotalCost(0)
{
}

// FUNCTION: 0x004B6B80
void DropshipLoadoutClass::Clear(void)
{
	UnusedBool1 = false;
	int count = EntryCount;
	while (count > 0) {
		Remove(0);
		count--;
	}
	EntryCount = 0;
	TotalCost = 0;
}

// FUNCTION: 0x004B6BB0
void DropshipLoadoutClass::Swizzle(void)
{
	for (int i = 0; i < EntryCount; i++) {
		Swizzle_Pointer(&Entries[i]);
	}
}

// FUNCTION: 0x004B6BE0
TechnoTypeClass * DropshipLoadoutClass::Fetch(int index)
{
	return(Entries[index]);
}
