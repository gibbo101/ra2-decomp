// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "multiplayer.h"

class MPUnholyAllianceClass : public MPGameModeClass
{
	public:
		MPUnholyAllianceClass(NoInitClass const & x) : MPGameModeClass(x) {}
		MPUnholyAllianceClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPUnholyAllianceClass(void);
		virtual bool vt_entry_10(void) override;
		virtual bool vt_entry_14(unsigned a) override;
		virtual bool vt_entry_18(void) override;
		virtual bool SpawnBaseUnits(HouseClass * house, unsigned unused) override;
};
