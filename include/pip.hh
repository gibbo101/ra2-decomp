// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

enum PipEnum {
	PIP_NONE = -1,

	PIP_EMPTY,
	PIP_GREEN,
	PIP_YELLOW,
	PIP_WHITE,
	PIP_RED,
	PIP_BLUE,
	PIP_PERSON_EMPTY,
	PIP_PERSON_GREEN,
	PIP_PERSON_YELLOW,
	PIP_PERSON_WHITE,
	PIP_PERSON_RED,
	PIP_PERSON_BLUE,
	PIP_PERSON_PURPLE,

	PIP_COUNT,
};


enum Pip2Enum  {
	PIP2_NONE = -1,

	PIP2_EMPTY,
	PIP2_GREEN,
	PIP2_YELLOW,
	PIP2_WHITE,
	PIP2_RED,
	PIP2_BLUE,
	PIP2_AMMO,

	PIP2_COUNT,
};


enum PipScaleType {
	PIPSCALE_NONE,

	PIPSCALE_AMMO,
	PIPSCALE_TIBERIUM,
	PIPSCALE_PASSENGERS,
	PIPSCALE_POWER,
	PIPSCALE_CHARGE,

	PIPSCALE_COUNT,
};
