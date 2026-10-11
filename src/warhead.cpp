// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "warhead.h"
#include "_warhead.h"
#include "animtype.h"
#include "ccini.h"
#include "findmake.h"
#include "globals.h"
#include "partsys.h"
#include "psystype.h"
#include "session.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x0075CE80
float _Parse_Percentage(const char * string)
{
	if (strchr(string, '%')) {
		return((double)atoi(string) / 100);
	}

	return(atof(string));
}
