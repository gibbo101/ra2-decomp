// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "gadget.h"

class ControlClass : public GadgetClass
{
		typedef GadgetClass BASECLASS;

	public:
		ControlClass(NoInitClass const & x) : BASECLASS(x) {};
		ControlClass(unsigned id, int x, int y, int w, int h, unsigned flags=LEFTPRESS|RIGHTPRESS, bool sticky=false);
		ControlClass(ControlClass const & control);

		virtual void Make_Peer(GadgetClass & gadget);

		virtual int Draw_Me(int forced=false) override;

		unsigned ID;

	protected:
		virtual unsigned Get_ID(void) const override;
		virtual int Action(unsigned flags, KeyNumType & key, int modifier) override;

		GadgetClass * Peer;
};
