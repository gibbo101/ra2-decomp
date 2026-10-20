// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

#include "ability.hh"

class VeterancyClass
{
	public:

		VeterancyClass(void);
		VeterancyClass(NoInitClass const & x) {};
		~VeterancyClass(void);

		void Made_A_Kill(int cost, int value);

		bool Is_Dumbass(void) const;
		bool Is_Rookie(void) const;
		bool Is_Veteran(void) const;
		bool Is_Elite(void) const;

		double Modify(double value);

		void Set_Dumbass(bool set);
		void Set_Rookie(bool set);
		void Set_Veteran(bool set);
		void Set_Elite(bool set);

		int To_Integer(void) const;
		void From_Integer(int value);

		float Experience;
};

AbilityType Ability_From_Name(const char *name);
