// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ccini.h"

class CoopCamp_0049B610
{
	public:
		CoopCamp_0049B610 * FUN_0049B610(void);
		CoopCamp_0049B610 * FUN_0049B660(wchar_t const * name, wchar_t const * map);
		void FUN_0049B720(void);
		void FUN_0049B760(wchar_t const * name, wchar_t const * map);
		void FUN_0049CAF0(int index);
		bool FUN_0049D120(void);

		char Name[0x14];
		int Field14;
		int Field18;
		char Map[0x14];
		int Field30;
		int Field34;
		int Field38;
		void ** Items;
		int ItemCount;
		int Field44;
		int Field48;
		int Field4C;
		int Field50;
		int Field54;
		int Field58;
		int Field5C;
		int Field60;
		int Field64;
		int Field68;
		bool Field6C;
};

struct CoopPlayerStats
{
	wchar_t Name[0x14];
	int Scheme;
	int Alive;
	int Lost;
	int Pad34[3];
	int Kills;
	int Pad44[3];
	int Built;
	int Pad54[3];
	int Score;
	int Pad64[3];
};

class CoopCampSource_0049E4F0 : public CCINIClass
{
	public:
		CoopCampSource_0049E4F0(void) { Loaded = false; }
		~CoopCampSource_0049E4F0(void);
		bool FUN_0049E4F0(int * count);

		void * Entries;
		int EntryCount;
		bool Loaded;
};

extern CoopCampSource_0049E4F0 DAT_0089F518;
