// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "theme.h"
#include "ccfile.h"
#include "ccini.h"
#include "ccrand.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "session.h"
#include "vector.h"

// FUNCTION: 0x00720940
char const * ThemeClass::Base_Name(ThemeType theme) const
{
	if ((unsigned)theme < (unsigned)Themes.Count()) {
		return(Themes[theme]->Name);
	}
	return("No theme");
}
