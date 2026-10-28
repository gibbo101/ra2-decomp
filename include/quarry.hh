// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

enum QuarryType {
	QUARRY_NONE,

	QUARRY_ANYTHING,
	QUARRY_BUILDINGS,
	QUARRY_HARVESTERS,
	QUARRY_INFANTRY,
	QUARRY_VEHICLES,
	QUARRY_FACTORIES,
	QUARRY_DEFENSE,
	QUARRY_THREAT,
	QUARRY_POWER,
	QUARRY_OCCUPIABLE_BUILDINGS,
	QUARRY_TECH_BUILDINGS,

	QUARRY_COUNT,
	QUARRY_FIRST=0
};
