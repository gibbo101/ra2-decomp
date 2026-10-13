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
		WideWstring(wchar_t const * string);
		~WideWstring(void);

		wchar_t *		get(void) const;
		wchar_t *		text(void) const;
		unsigned int	length(void) const;
		int				toInt(void) const;
		int				getToken(int offset, wchar_t const * delim, WideWstring & out) const;

		bool			operator==(wchar_t const * other) const;
		bool			operator==(WideWstring const & other) const;
		bool			operator!=(wchar_t const * other) const;
		bool			operator!=(WideWstring const & other) const;

		WideWstring &	operator=(wchar_t const * other);
		WideWstring &	operator=(WideWstring const & other);

		wchar_t * str;
};
