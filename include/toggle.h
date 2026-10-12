// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "control.h"

class ToggleClass : public ControlClass
{
		typedef ControlClass BASECLASS;

	public:
		ToggleClass(unsigned id, int x, int y, int w, int h);
		void Turn_On(void);
		void Turn_Off(void);

		bool IsPressed;

		bool IsOn;

		bool IsToggleType;

	protected:

		virtual int Action(unsigned flags, KeyNumType &key, int modifier) override;
};
