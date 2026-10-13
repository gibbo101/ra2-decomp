// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <stddef.h>

class WideWstring
{
	public:
		WideWstring(void);
		WideWstring(WideWstring const & other);
		~WideWstring(void);

		wchar_t * str;
};
