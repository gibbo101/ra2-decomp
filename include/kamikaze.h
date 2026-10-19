// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "ftimer.h"
#include "timer.h"
#include "vector.h"

class AircraftClass;
class CellClass;

class Kamikaze
{
	public:
		struct KamikazeControl {
			AircraftClass * Item;
			CellClass * Cell;
		};

		void Clear(void);

		static Kamikaze Instance;

		CDTimerClass<FrameTimerClass> UpdateTimer;
		DynamicVectorClass<KamikazeControl *> Nodes;
};
