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
		wchar_t			get(unsigned int index);
		wchar_t			operator[](unsigned int index) const;
		wchar_t			getChar(unsigned int index) const;
		char			set(wchar_t c, unsigned int index);
		unsigned int	length(void) const;
		int				toInt(void) const;
		int				getToken(int offset, wchar_t const * delim, WideWstring & out) const;
		bool			containsAny(wchar_t const * chars) const;
		bool			containsAny(WideWstring const & chars) const;
		int				getLine(int offset, WideWstring & out) const;
		char			cat(wchar_t const * s);
		char			cat(unsigned int size, wchar_t const * s);
		char			cat(WideWstring const & other);
		WideWstring &	operator+=(wchar_t const * string);
		WideWstring &	operator+=(WideWstring const & other);
		WideWstring		operator+(wchar_t const * string) const;
		WideWstring		operator+(WideWstring const & s) const;
		wchar_t			remove(int pos, int count);
		char			removeChar(wchar_t c);
		void			removeSpaces(void);
		void			trimLeading(WideWstring const & delim);
		void			trimTrailing(WideWstring const & delim);
		char			truncate(unsigned int len);
		char			truncate(wchar_t c);
		wchar_t			set(unsigned int size, wchar_t const * string);
		char			replace(wchar_t const * replaceThis, wchar_t const * withThis);
		wchar_t			set(wchar_t const * s);
		void			clear(void);
		void			setSize(int size);
		void			cellCopy(wchar_t * dest, unsigned int len) const;
		char			insert(wchar_t const * instring, unsigned int pos);
		char			insert(wchar_t k, unsigned int pos);
		void			toLower(void);
		WideWstring		toLower(void) const;
		void			toUpper(void);
		WideWstring		toUpper(void) const;

		bool			operator==(wchar_t const * other) const;
		bool			operator==(WideWstring const & other) const;
		bool			operator!=(wchar_t const * other) const;
		bool			operator!=(WideWstring const & other) const;

		WideWstring &	operator=(wchar_t const * other);
		WideWstring &	operator=(WideWstring const & other);

		wchar_t * str;
};
