// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <windows.h>
#include "refcount.h"
#include "widewstring.h"
#include "wstring.h"


struct Ladder;

class PlayerProfile : public ReferenceCounted
{
	public:
		PlayerProfile(Wstring const & name, int a, bool b, int c, int d, int e, int f, int g, int h, int i, int j, bool k, WideWstring const & l, WideWstring const & m);
		virtual ~PlayerProfile(void);
		void SetName(Wstring const * name);
		void SetLocale(int locale, int state);
		void SetCountry(int country);
		void SetMatchByRes(bool value);
		void SetUpdateStamp(void);
		void SetField20(int value);
		void UpdateStats(Ladder const * record);
		bool StatsDiffer(Ladder const * record) const;
		void AddWins(int delta);
		void AddLosses(int delta);
		void AddDisconnects(int delta);
		void Save(Wstring const * name);
		void FUN_0077A9F0(HWND window, Wstring const * name, bool flag);

		char IsChanged;
		Wstring Name;
		int Locale;
		bool Field14;
		int Field18, Country, Field20, Rung, Points, Wins, Losses, Disconnects;
		bool MatchByRes;
		WideWstring UpdateTime;
		WideWstring UpdateDate;
};
