// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
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

// SYNTHETIC: GAMEMD 0x0065D6C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0065D6F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0065D710 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0065D730 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0065D750 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0065D770 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0065D790 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0065D7D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0065D800 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0065D830 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0065D860 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0065D870 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0065D890 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x0065D8B0 SYMBOL
// _$E41

