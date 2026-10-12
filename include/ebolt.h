// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"

class TechnoClass;

class EBolt
{
	public:
		Coord Point1;
		Coord Point2;
		unsigned unknown_18;
		int Random;
		TechnoClass * Owner;
		int WeaponSlot;
		int Lifetime;
		bool AlternateColor;

		EBolt(void);
		~EBolt(void);
};
