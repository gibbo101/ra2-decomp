// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

char * __fastcall FUN_007350C0(wchar_t const * text);

// The narrow copy of `text`, valid until eight later conversions have reused its buffer.
inline char * Narrow_Text(wchar_t const * text)
{
	return FUN_007350C0(text);
}
