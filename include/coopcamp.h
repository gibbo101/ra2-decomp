// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class CoopCamp_0049B610
{
	public:
		CoopCamp_0049B610 * FUN_0049B610(void);
		void FUN_0049B720(void);
		void FUN_0049CAF0(int index);
		bool FUN_0049D120(void);

		char Name[0x1c];
		char Map[0x40];
		char Tail[0x14];
};

class CoopCampSource_0049E4F0
{
	public:
		bool FUN_0049E4F0(int * count);
};

extern CoopCampSource_0049E4F0 DAT_0089F518;
