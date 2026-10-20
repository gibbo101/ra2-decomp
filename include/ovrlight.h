// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"

template<class T> class DynamicVectorClass;

class SpotLightClass
{
	public:
		SpotLightClass(Coord pos, int size);
		~SpotLightClass(void);

		void Set_Radius(int radius) {Radius=radius;}
		void AI(void);
		void Draw_It(void);

		static void One_Time(void);
		static void Update_All(void);
		static void Clear_All(void);
		static void Draw_All(void);

		enum {
			SPOTLIGHT_RADIUS_STEP = 8,
			SPOTLIGHT_MAX_RADIUS = 80,
			SPOTLIGHT_SURFACE_COUNT = 64,
			SPOTLIGHT_EXTRA_SURFACE_COUNT = 10,
		};

	private:
		Coord Position;
		int Radius;
		int Size;
		unsigned int DisableFlags;
};

extern DynamicVectorClass<SpotLightClass *> SpotLights;
