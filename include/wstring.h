// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <cstdio>
#include <cstdlib>

class Wstring
{
public:
	Wstring(void);
	Wstring(const Wstring& other);
	Wstring(char const* string);
	~Wstring(void);

	void			clear(void);

	char			cat(char const* string);
	char			cat(unsigned int size, char const* string);
	char			cat(Wstring const& string);

	void			cellCopy(char* dest, unsigned int len) const;
	char			remove(int pos, int count);
	char			removeChar(char c);
	void			removeSpaces(void);
	void			trimLeading(Wstring const& delim);
	void			trimTrailing(Wstring const& delim);
	char*			get(void) const;
	char			get(unsigned int index);
	char*			text(void) const;
	char			operator[](unsigned int index) const;
	char			getChar(unsigned int index) const;
	unsigned int	length(void) const;
	char			insert(char c, unsigned int pos);
	char			insert(char const* instring, unsigned int pos);
	char			replace(char const* replaceThis, char const* withThis);
	char			set(char const* str);
	char			set(unsigned int size, char const* str);
	char			set(char c, unsigned int index);
	void			setSize(int bytes);
	void			toLower(void);
	Wstring			toLower(void) const;
	void			toUpper(void);
	Wstring			toUpper(void) const;
	int				toInt(void) const;
	char			truncate(unsigned int len);
	char			truncate(char c);
	int				getToken(int offset, char const* delim, Wstring& out) const;
	int				getLine(int offset, Wstring& out) const;
	bool			containsAny(char const* chars) const;
	bool			containsAny(Wstring const& chars) const;

	bool			operator==(char const* other) const;
	bool			operator==(Wstring const& other) const;
	bool			operator!=(char const* other) const;
	bool			operator!=(Wstring const& other) const;

	Wstring&		operator=(char const* other);
	Wstring&		operator=(Wstring const& other);
	Wstring&		operator+=(char const* other);
	Wstring&		operator+=(Wstring const& other);
	Wstring			operator+(char const* other) const;
	Wstring			operator+(Wstring const& other) const;

private:
	char*			str;
};

Wstring Wstring_FromInt(int value);
Wstring Wstring_FromInt(int value, int width);
Wstring operator+(char const* a, Wstring const& b);
