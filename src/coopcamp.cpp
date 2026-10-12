// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ccfile.h"
#include "ccini.h"
#include "coopcamp.h"
#include <cstring>
#include <cctype>

extern char DAT_0089ED18[];
char __fastcall FUN_0049B430(int nibble);

static inline char HexDigit(int nibble)
{
	if (nibble < 0 || nibble > 0xf) {
		return 0;
	}
	if (nibble < 10) {
		return (char)('0' + nibble);
	}
	return (char)('7' + nibble);
}

// FUNCTION: 0x0049D120
bool CoopCamp_0049B610::FUN_0049D120(void)
{
	if (!Name[0] || !Map[0]) {
		return false;
	}
	char section[100];
	struct Loaded {
		CCFileClass file;
		CCINIClass ini;
		Loaded(void) : file("coopsave.ini") {}
	} l;
	CCFileClass & file = l.file;
	CCINIClass & ini = l.ini;
	if (!ini.ReadCCFile(file, false, false)) {
		l.file.Close();
		return false;
	}
	l.file.Close();

	int i = 0;
	char const * p;
	for (p = Name; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcpy(section, DAT_0089ED18);
	i = 0;
	for (p = Map; *p && i < 0x3fd; i++, p++) {
		if (!isalnum(*p)) {
			DAT_0089ED18[i++] = '=';
			int hi = *p >> 4;
			DAT_0089ED18[i++] = HexDigit(hi);
			DAT_0089ED18[i] = FUN_0049B430(*p & 0xf);
		} else {
			DAT_0089ED18[i] = *p;
		}
	}
	DAT_0089ED18[i] = 0;
	strcat(section, DAT_0089ED18);
	return ini.GetSection(section) != NULL;
}
