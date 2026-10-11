// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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
