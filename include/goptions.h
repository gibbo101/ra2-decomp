// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "gadget.h"
#include "options.h"

class GameOptionsClass : public OptionsClass {
	public:
		GameOptionsClass(void): OptionsClass () { };
};

int Abort_Dialog(void);
void Game_Options_Dialog(void);
