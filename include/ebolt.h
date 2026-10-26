// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"
#include "vector.h"

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
		void Fire(Coord start, Coord end, int zadjust);
		void SetOwner(TechnoClass * owner, int weapon);
		~EBolt(void);
		void ClearOwner(void);
		Coord GetSourceCoords(void) const;
		void FUN_004C1F20(Coord start, Coord end, int color);
		void FUN_004C2790(void);

		static void FUN_004C2830(void);
};

extern DynamicVectorClass<EBolt *> EBoltArray;
