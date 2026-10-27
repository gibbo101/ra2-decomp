// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "ftimer.h"
#include "timer.h"
#include "vector.h"

class AircraftClass;
class AbstractClass;
class CellClass;
struct IStream;

struct KamikazeControl {
	AircraftClass * Item;
	CellClass * Cell;
};

class Kamikaze
{
	public:
		Kamikaze(void);
		~Kamikaze(void);

		void Add(AircraftClass * aircraft, AbstractClass * target);
		void Update(void);
		void Remove(AbstractClass * object);
		void Clear(void);
		long Save(IStream * stream);
		long Load(IStream * stream);

		CDTimerClass<FrameTimerClass> UpdateTimer;
		DynamicVectorClass<KamikazeControl *> Nodes;
};

extern Kamikaze KamikazeInstance;
