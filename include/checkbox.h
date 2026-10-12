// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "toggle.h"

class CheckBoxClass : public ToggleClass
{
		typedef ToggleClass BASECLASS;

	public:
		CheckBoxClass(unsigned id, int x, int y) :
			BASECLASS(id, x, y, 7, 7)
		{};

		virtual int Draw_Me(int forced=false) override;
		virtual int Action(unsigned flags, KeyNumType & key, int modifier) override;

	protected:
};
