// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class HashString
{
	public:
		HashString(void);
		HashString(HashString const & that);
		HashString & operator = (HashString const & that);
		bool operator == (HashString const & that) const;
		bool operator != (HashString const & that) const;

		char Str[256];
};
