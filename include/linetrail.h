// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"
#include "rgb.h"

class ObjectClass;

struct LineTrailNode
{
	Coord Position;
	int Value;
};

class LineTrail
{
	public:
		RGBClass Color;
		ObjectClass * Owner;
		int Decrement;
		int ActiveSlot;
		LineTrailNode Trails[32];

		LineTrail(void);
		~LineTrail(void);

		void Set_Decrement(int value);
		static void Delete_All(void);
};
