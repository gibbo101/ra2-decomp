// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "diskswap.h"

#include "theme.hh"

class CD : public DiskSwap
{
		typedef DiskSwap BASECLASS;

	public:
		virtual bool Force_Available(DiskID disk) override;
		virtual bool Request_Disk(DiskID disk) override;
		virtual bool Swap(DiskID disk) override;

		static DiskID Get_Current_Disk(int timeout);

		static bool Force_Available(void) { return(CD().Force_Available(RequiredCD)); }

		static void Set_Required_Disk(DiskID disk);
		static DiskID Get_Required_Disk(void) { return(RequiredCD); }

		static void Override_Swap(bool state) { _OverrideSwap = state; }
		static bool Is_Override_Swap(void) { return(_OverrideSwap); }

	private:

		ThemeType ThemePlaying;

		static DiskID RequiredCD;

		static bool _OverrideSwap;
};
