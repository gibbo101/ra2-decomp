// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "win.h"

#include "diff.hh"

#include <comdef.h>

class EnvironmentClass
{
	public:
		EnvironmentClass(void);
		~EnvironmentClass(void);

		void Store(void);
		void Restore(void);

		HRESULT Load(IStream * stream);
		HRESULT Save(IStream * stream);

	public:

		bool Globals[50];

		int CarryOverMoney;

		int MissionTimer;

		DiffType Difficulty;

		unsigned short Stage;
};

extern EnvironmentClass Environment;
