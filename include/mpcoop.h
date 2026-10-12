// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coopcamp.h"
#include "multiplayer.h"

class MPCooperative : public MPGameModeClass
{
	public:
		virtual bool vt_entry_10(void) override;

		char Unknown[0x324 - sizeof(MPGameModeClass)];
		DynamicVectorClass<CoopCamp_0049B610 *> Campaigns;
};
