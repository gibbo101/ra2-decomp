// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include <cstring>

// FUNCTION: GAMEMD 0x00727CF0
char * strtrim(char * buffer)
{
	if (buffer) {
		char * source = buffer;
		while ((*source != 0) && ((unsigned char)*source <= 32)) {
			source++;
		}

		if (source != buffer) {
			strcpy(buffer, source);
		}

		for (int index = strlen(buffer)-1; index >= 0; index--) {
			if ((*source != 0) && ((unsigned char)buffer[index] <= 32)) {
				buffer[index] = '\0';
			} else {
				break;
			}
		}
	}
	return(buffer);
}

// FUNCTION: GAMEMD 0x00727D60
wchar_t * wcstrim(wchar_t * buffer)
{
	if (buffer) {
		wchar_t * source = buffer;
		while ((*source != 0) && ((unsigned short)*source <= 32)) {
			source++;
		}

		if (source != buffer) {
			wcscpy(buffer, source);
		}

		for (int index = wcslen(buffer)-1; index >= 0; index--) {
			if ((*source != 0) && ((unsigned short)buffer[index] <= 32)) {
				buffer[index] = L'\0';
			} else {
				break;
			}
		}
	}
	return(buffer);
}

// SYNTHETIC: GAMEMD 0x00727DC0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00727DF0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00727E10 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00727E30 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00727E50 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00727E70 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00727E90 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00727ED0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00727F00 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00727F30 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00727F60 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00727F70 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00727F90 SYMBOL
// _$E38

struct TrimZero4I { int a, b, c, d; TrimZero4I() : a(0), b(0), c(0), d(0) {} };

// SYNTHETIC: GAMEMD 0x00727FB0 SYMBOL
// _$E41
TrimZero4I DAT_00B0F860;
