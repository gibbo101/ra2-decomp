// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "multiplayer.h"

class UnholyAlliance : public MultiplayerGameMode
{
	public:
		UnholyAlliance(NoInitClass const & x) : MultiplayerGameMode(x) {}
		UnholyAlliance(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~UnholyAlliance(void);
		virtual bool vt_entry_10(void) override;
		virtual bool vt_entry_14(WideWstring * message) override;
		virtual bool vt_entry_18(void) override;
		virtual bool SpawnBaseUnits(HouseClass * house, unsigned unused) override;
};
