// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include <cstring>

// FUNCTION: 0x00727CF0
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

// FUNCTION: 0x00727D60
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

// FUNCTION: 0x00727DC0 $E2
// FUNCTION: 0x00727DF0 $E5
// FUNCTION: 0x00727E10 $E8
// FUNCTION: 0x00727E30 $E11
// FUNCTION: 0x00727E50 $E14
// FUNCTION: 0x00727E70 $E17
// FUNCTION: 0x00727E90 $E20
// FUNCTION: 0x00727ED0 $E23
// FUNCTION: 0x00727F00 $E26
// FUNCTION: 0x00727F30 $E29
// FUNCTION: 0x00727F60 $E32
