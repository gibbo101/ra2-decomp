// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "multiplayer.h"

static DynamicVectorClass<MPGameModeClass *> & Game_Modes(void)
{
	static DynamicVectorClass<MPGameModeClass *> modes;
	return(modes);
}

static int Game_Mode_Count(void)
{
	return(Game_Modes().Count());
}

// FUNCTION: 0x005D5F30
MPGameModeClass * __fastcall MPGameModeClass::Get(int index)
{
	for (int i = 0; i < Game_Mode_Count(); i++) {
		MPGameModeClass * mode = Game_Modes()[i];
		if (mode->MPModeIndex == index) {
			return(mode);
		}
	}
	return(NULL);
}
