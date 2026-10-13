// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

enum PassabilityType {
	PASSABLE_LAND,
	PASSABLE_CRUSH,
	PASSABLE_BLOCKED,
	PASSABLE_BEACH,
	PASSABLE_WATER,
	PASSABLE_PARTIALLY_BLOCKED,
	PASSABLE_NO,
	PASSABLE_OUTSIDE,
	PASSABLE_COUNT
};

enum TraversalType {
	TRAVERSAL_PASSABLE = 1,
	TRAVERSAL_IMPASSABLE = 2,
	TRAVERSAL_ILLEGAL = 3
};
