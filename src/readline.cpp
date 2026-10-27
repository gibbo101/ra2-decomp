// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "readline.h"
#include "wwfile.h"
#include "xstraw.h"
#include <cstring>
#include <math.h>
#include "refcount.h"


// FUNCTION: GAMEMD 0x0065D510
int Read_Line(FileClass & file, char * buffer, int len, bool & eof)
{
	FileStraw fs(file);
	return(Read_Line(fs, buffer, len, eof));
}

// FUNCTION: GAMEMD 0x0065D5C0
int Read_Line(Straw & file, char * buffer, int len, bool & eof)
{
	if (len == 0 || buffer == NULL) return(0);

	int count = 0;
	for (;;) {
		char c;
		if (file.Get(&c, sizeof(c)) != sizeof(c)) {
			eof = true;
			buffer[count] = '\0';
			break;
		}

		if (c == '\x0A') break;
		if (c != '\x0D' && count+1 < len) {
			buffer[count++] = c;
		}
	}
	buffer[count] = '\0';

	return(strlen(buffer));
}

// FUNCTION: GAMEMD 0x0065D660
ReferenceCounted::~ReferenceCounted(void)
{
}

// FUNCTION: GAMEMD 0x0065D670
void ReferenceCounted::FUN_0065D670(void)
{
	if (--Count == 0) delete this;
}

// FUNCTION: GAMEMD 0x0065D690
bool ReferenceCounted::FUN_0065D690(int)
{
	return false;
}

// SYNTHETIC: GAMEMD 0x0065D6A0
// ??_GReferenceCounted@@UAEPAXI@Z
