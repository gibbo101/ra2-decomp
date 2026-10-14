// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "session.h"

class PhoneEntryClass
{
	public:
		enum PhoneEntryEnum {
			PHONE_MAX_NAME = 21,
			PHONE_MAX_NUM = 21
		};

		PhoneEntryClass(void) {};
		~PhoneEntryClass(void) {};

		SerialSettingsType Settings;
		wchar_t Name[ PHONE_MAX_NAME ];
		char Number[ PHONE_MAX_NUM ];
};
