// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class ReferenceCounted
{
	public:
		ReferenceCounted(void) : Count(0) {}
		virtual ~ReferenceCounted(void);
		virtual bool FUN_0065D690(int);
		virtual void FUN_0065D670(void);

		int Count;
};
