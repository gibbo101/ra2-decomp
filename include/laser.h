// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "rgb.h"
#include "stage.h"

template<class T> class DynamicVectorClass;

class LaserDrawClass : public StageClass
{
	public:
		LaserDrawClass(Coord start, Coord end, int zadjust, bool,
			RGBClass inner_color, RGBClass outer_color, RGBClass outer_spread,
			int duration, bool blinks, bool fades, float start_intensity, float end_intensity);
		~LaserDrawClass(void);

		void Draw_It(void);
		void AI(void);

		static void Update_All(void);
		static void All_Clear(void);
		static void Draw_All(void);

	public:
		int Thickness;
		bool IsHouseColor;
		bool IsSupported;

		Coord Start;
		Coord End;

		int ZAdjust;

		bool UnusedBool1;

		RGBClass InnerColor;

		RGBClass OuterColor;

		RGBClass OuterSpread;

		int Duration;

		bool Blinks;

		bool BlinkState;

		bool Fades;

		float StartIntensity;
		float EndIntensity;

		static DynamicVectorClass<LaserDrawClass *> LaserDraws;
};
