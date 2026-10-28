// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

class CreditClass {
	public:
		int Credits;

		CreditClass(void);
		CreditClass(NoInitClass const & ) {};

		void Update(bool forced=false, bool redraw=false);

		void Graphic_Logic(bool forced=false);
		void AI(bool forced=false);

		int Current;

		bool IsToRedraw;
		bool IsUp;
		bool IsAudible;

	private:
		int	Countdown;
};
