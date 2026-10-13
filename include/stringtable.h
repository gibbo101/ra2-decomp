// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

// The game's localised text, keyed by label; the source file and line of each lookup
// are passed for its error report.
class StringTable
{
	public:
		static wchar_t const * LoadString(char const * label, char * extra, char const * file, int line);
};

char * __fastcall FUN_007350C0(wchar_t const * text);
wchar_t * __fastcall FUN_00735120(char const * text);
