// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mssfx.h"
#include <string.h>
#include <stdlib.h>

extern int FUN_00407000(void);

// FUNCTION: GAMEMD 0x005D5820
MSSfx::MSSfx(char const * name, char const * file_name) :
	Name(NULL),
	Sample(NULL),
	Loaded(false)
{
	if (FUN_00407000()) {
		Name = strdup(name);
	}
}

// FUNCTION: GAMEMD 0x005D5850
MSSfx::~MSSfx(void)
{
	if (Name != NULL) {
		free(Name);
	}
}

// FUNCTION: GAMEMD 0x005D5860
void MSSfx::Play(int volume)
{
}
