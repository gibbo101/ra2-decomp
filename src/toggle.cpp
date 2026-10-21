// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "toggle.h"
#include "_xmouse.h"

// FUNCTION: 0x00723E60
ToggleClass::ToggleClass(unsigned id, int x, int y, int w, int h) :
	BASECLASS(id, x, y, w, h, LEFTPRESS|LEFTRELEASE, true),
	IsPressed(false),
	IsOn(false),
	IsToggleType(0)
{
}

// FUNCTION: 0x00723EA0
void ToggleClass::Turn_On(void)
{
	IsOn = true;
	Flag_To_Redraw();
}

// FUNCTION: 0x00723EB0
void ToggleClass::Turn_Off(void)
{
	IsOn = false;
	Flag_To_Redraw();
}

// FUNCTION: 0x00723EC0
int ToggleClass::Action(unsigned flags, KeyNumType &key, int modifier)
{
	bool overbutton = ((unsigned)(Get_Mouse_X() - X) < (unsigned)Width && (unsigned)(Get_Mouse_Y() - Y) < (unsigned)Height);
	if (!flags) {
		if (overbutton) {
			if (!IsPressed) {
				IsPressed = true;
				Flag_To_Redraw();
			}
		} else {
			if (IsPressed) {
				IsPressed = false;
				Flag_To_Redraw();
			}
		}
	}

	Sticky_Process(flags);

	if (flags & (LEFTPRESS|RIGHTPRESS)) {
		IsPressed = true;
		Flag_To_Redraw();
		flags &= ~(LEFTPRESS|RIGHTPRESS);
		BASECLASS::Action(flags, key, 0);
		key = KN_NONE;
		return true;
	}

	if (flags & (LEFTRELEASE|RIGHTRELEASE)) {
		if (IsPressed) {
			if (overbutton) {
				if (IsToggleType == 1) {
					IsOn = (IsOn == false);
					IsPressed = false;
					Flag_To_Redraw();
					return BASECLASS::Action(flags, key, 0);
				}
				if (IsToggleType == 2 && !IsOn) {
					IsOn = true;
				}
			}
			IsPressed = false;
			Flag_To_Redraw();
		} else {
			flags &= ~(LEFTRELEASE|RIGHTRELEASE);
		}
	}

	return BASECLASS::Action(flags, key, 0);
}

int DAT_00B0F5A8;
int DAT_00B0F5AC;
int DAT_00B0F5B0;
int DAT_00B0F5B4;

// FUNCTION: 0x00723FE0
void FUN_00723FE0(void)
{
	DAT_00B0F5A8 = 0;
	DAT_00B0F5AC = 0;
	DAT_00B0F5B0 = 0;
	DAT_00B0F5B4 = 0;
}
