// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <windows.h>

#include "coord.h"

template<class T> class DynamicVectorClass;

class IonBlastClass
{
	public:
		IonBlastClass(Coord coord);
		~IonBlastClass(void);

		void AI(void);
		void Draw_It(void);

		static void One_Time(void);
		static void Update_All(void);
		static void Clear_All(void);
		static void Draw_All(void);

	private:
		// The centre the shockwave expands from, where the damage and explosions land.
		Coord Position;

		// Age in game frames; indexes the shockwave surface, and the blast ends at the last.
		int Lifetime;

	public:
		// Set for a blast that shows only the impact, with no beam from the sky.
		BOOL DisableIonBeam;

		// Every blast in progress; each adds itself on creation and removes itself on death.
		static DynamicVectorClass<IonBlastClass *> IonBlasts;
};
