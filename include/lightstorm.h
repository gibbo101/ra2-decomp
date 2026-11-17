// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "vector.h"

class AnimClass;
class HouseClass;

class LightningStorm
{
	public:
		static bool HasDeferment(void);
		static void PrintMessage(void);

		static Cell Coords;
		static DynamicVectorClass<AnimClass const *> CloudsManifesting;
		static DynamicVectorClass<AnimClass const *> CloudsPresent;
		static DynamicVectorClass<AnimClass const *> BoltsPresent;
		static bool Active;
		static int StartTime;
		static int Duration;
		static int Deferment;
		static bool TimeToEnd;
		static HouseClass * Owner;
};

class PsyDom
{
	public:
		static bool Active(void);
		static void PrintMessage(void);

		static Cell Coords;
		static int Status;
		static AnimClass * Anim;
		static HouseClass * Owner;
};

class NukeFlash
{
	public:
		static bool IsFadingIn(void);

		static int StartTime;
		static int Duration;
		static int Status;
};
