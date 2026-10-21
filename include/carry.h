// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "link.h"

#include "house.hh"
#include "infantry.hh"
#include "rtti.hh"
#include "struct.hh"
#include "unit.hh"

class TechnoClass;

class CarryoverClass : public LinkClass {
		typedef LinkClass BASECLASS;

	public:
		CarryoverClass(TechnoClass * techno = 0);
		CarryoverClass(NoInitClass const & x) : BASECLASS(x) {}

		bool Create(void) const;

	protected:

		RTTIType RTTI;

		union {
			StructType Building;
			UnitType Unit;
			InfantryType Infantry;
		} Type;

		Cell CellID;

		int Strength;

		HousesType House;
};
