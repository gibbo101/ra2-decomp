// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "gadget.h"

class SoundControlsClass
{
	enum SoundControlsClassEnums {
		VOLUME_LEVELS=10
	};

	public:
		SoundControlsClass(void) {}
		void Dialog(void);

		static BOOL CALLBACK Sound_Option_Dialog_Func(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
};
