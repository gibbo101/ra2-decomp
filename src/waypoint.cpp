// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "waypoint.h"
#include "crc.h"
#include "noinit.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: 0x00763610
const char *Waypoint_To_Name(WAYPOINT wp)
{
	static char _string[4];

	if (wp == -1) {
		_string[0] = '\0';
		return(_string);
	}

	const int num_chars = ('Z' - 'A') + 1;

	if (wp < num_chars) {
		wsprintf(_string, "%c", wp + 'A');
		return(_string);
	}

	wsprintf(_string, "%c%c", (wp / num_chars) + ('A' - 1), (wp % num_chars) + 'A');
	return(_string);
}

// FUNCTION: 0x00763700
WaypointClass::WaypointClass(Coord const & coord) :
	Location(coord)
{
}
