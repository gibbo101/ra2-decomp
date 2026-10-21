// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "gadget.h"
#include "goptions.h"

extern char const * GameSpeedNames[OptionsClass::MAX_SPEED_SETTING];
extern char const * GameScrollSpeedNames[OptionsClass::MAX_SCROLL_SETTING];
extern char const * GameDetailLevelNames[OptionsClass::MAX_DETAIL_SETTING];
extern char const * GameDifficultyNames[OptionsClass::MAX_DIFFICULTY_SETTING];

class GameControlsClass
{
	public:
		GameControlsClass(void) {};
		void Dialog(void);

		static char const * GetDifficultyLabel(int difficulty)
		{
			return(GameDifficultyNames[difficulty]);
		}

	private:
		void Set(void);

	private:

		HWND _Dialog;
};
