// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#pragma once

#include <cstring>


/// Names derived from AbilityName

enum AbilityType {
	ABILITY_NONE = -1,

	ABILITY_FASTER,
	ABILITY_STRONGER,
	ABILITY_FIREPOWER,
	ABILITY_SCATTER,
	ABILITY_ROF,
	ABILITY_SIGHT,
	ABILITY_CLOAK,
	ABILITY_TIBERIUM_PROOF,
	ABILITY_VEIN_PROOF,
	ABILITY_SELF_HEAL,
	ABILITY_EXPLODES,
	ABILITY_RADAR_INVISIBLE,
	ABILITY_SENSORS,
	ABILITY_FEARLESS,
	ABILITY_C4,
	ABILITY_TIBERIUM_HEAL,
	ABILITY_GUARD_AREA,
	ABILITY_CRUSHER,

	ABILITY_COUNT,
	ABILITY_FIRST = 0
};


class NoInitClass;

struct AbilityFlagsType {
	public:
		AbilityFlagsType()
		{
			/// Init all flags to false.
			memset(AbilitiesFlag, 0, sizeof(AbilitiesFlag));
		}

		AbilityFlagsType(NoInitClass const & x) {}

		~AbilityFlagsType(void) {}

		AbilityFlagsType operator=(AbilityFlagsType const & that);

		bool const & operator[](AbilityType type) const { return(AbilitiesFlag[type]); }
		bool & operator[](AbilityType type) { return(AbilitiesFlag[type]); }

	private:
		/*
		 * These are the abilities present in this set, one flag per AbilityType. A unit type
		 * keeps one such set for the abilities its veterans earn and another for its elites.
		 */
		bool AbilitiesFlag[ABILITY_COUNT];
};
