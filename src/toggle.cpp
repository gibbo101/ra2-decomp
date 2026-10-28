// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "toggle.h"
#include "_xmouse.h"

// SYNTHETIC: GAMEMD 0x00723E40 SYMBOL
// _$E2
// FUNCTION: GAMEMD 0x00723E60
ToggleClass::ToggleClass(unsigned id, int x, int y, int w, int h) :
	BASECLASS(id, x, y, w, h, LEFTPRESS|LEFTRELEASE, true),
	IsPressed(false),
	IsOn(false),
	IsToggleType(0)
{
}

// FUNCTION: GAMEMD 0x00723EA0
void ToggleClass::Turn_On(void)
{
	IsOn = true;
	Flag_To_Redraw();
}

// FUNCTION: GAMEMD 0x00723EB0
void ToggleClass::Turn_Off(void)
{
	IsOn = false;
	Flag_To_Redraw();
}

// FUNCTION: GAMEMD 0x00723EC0
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
