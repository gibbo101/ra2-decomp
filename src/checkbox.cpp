// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "checkbox.h"
#include "_xmouse.h"
#include "convert.h"
#include "dialog.h"
#include "dsurface.h"
#include "color.hh"

struct FileSystem {
	static ConvertClass * PALETTE_PAL;
};

// SYNTHETIC: GAMEMD 0x004880D0 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x004880F0
int CheckBoxClass::Draw_Me(int forced)
{
	if (BASECLASS::Draw_Me(forced)) {
		MouseCursor->Hide_Mouse();
		Draw_Box(Rect(X, Y, Width, Height), BOXSTYLE_DOWN, false);
		DSurface::Temp->Fill_Rect(Rect(X+1, Y+1, Width-1, Height-1), FileSystem::PALETTE_PAL->Convert_Pixel(GREY));
		if (IsOn) {
			DSurface::Temp->Fill_Rect(Rect(X+1, Y+1, Width-1, Height-1), FileSystem::PALETTE_PAL->Convert_Pixel(LTGREEN));
		}
		MouseCursor->Show_Mouse();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00488210
int CheckBoxClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	if (flags & LEFTRELEASE) {
		if (IsOn) {
			Turn_Off();
		} else {
			Turn_On();
		}
	}
	return(BASECLASS::Action(flags, key, 0));
}
