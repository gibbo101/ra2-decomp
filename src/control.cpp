// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "control.h"

// FUNCTION: 0x0048E520
ControlClass::ControlClass(unsigned id, int x, int y, int w, int h, unsigned flags, bool sticky) :
	BASECLASS(x, y, w, h, flags, sticky),
	ID(id),
	Peer(0)
{
}

// FUNCTION: 0x0048E570
ControlClass::ControlClass(ControlClass const & control) :
	BASECLASS(control),
	ID(control.ID),
	Peer(control.Peer)
{
}

// FUNCTION: 0x0048E600
void ControlClass::Make_Peer(BASECLASS & gadget)
{
	Peer = &gadget;
}

// FUNCTION: 0x0048E620
int ControlClass::Draw_Me(int forced)
{
	if (Peer) {
		Peer->Draw_Me();
	}
	return(BASECLASS::Draw_Me(forced));
}
