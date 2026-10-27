// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "multiplayer.h"

class MPFreeForAllClass : public MultiplayerGameMode
{
	public:
		MPFreeForAllClass(NoInitClass const & x) : MultiplayerGameMode(x) {}
		MPFreeForAllClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPFreeForAllClass(void);
		virtual bool vt_entry_10(void) override;
		virtual bool vt_entry_14(unsigned a) override;
		virtual bool vt_entry_18(void) override;
		virtual bool vt_entry_40(void) override;
		virtual void PopulateTeamDropdownForPlayer(HWND hwnd, int index) override;
};
