// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "multiplayer.h"

class MPMegawealthClass : public MPGameModeClass
{
	public:
		MPMegawealthClass(NoInitClass const & x) : MPGameModeClass(x) {}
		MPMegawealthClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex);
		virtual ~MPMegawealthClass(void);
		virtual bool vt_entry_8C(void) override;
};
