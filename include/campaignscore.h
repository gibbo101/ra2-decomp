// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "vector.h"
struct MPlayerScoreType;

class CampaignScoreClass
{
	public:
		CampaignScoreClass(void);
		virtual ~CampaignScoreClass(void);
		virtual void Show(DynamicVectorClass<MPlayerScoreType *> list, int a6, bool a7, int a8, wchar_t const * name);
		void FUN_0046DA70(void);

		int Field4;
};
