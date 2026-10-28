// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "control.h"
#include "rect.h"

// SYNTHETIC: GAMEMD 0x0048E500 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x0048E520
ControlClass::ControlClass(unsigned id, int x, int y, int w, int h, unsigned flags, bool sticky) :
	BASECLASS(x, y, w, h, flags, sticky),
	ID(id),
	Peer(0)
{
}

// FUNCTION: GAMEMD 0x0048E570
ControlClass::ControlClass(ControlClass const & control) :
	BASECLASS(control),
	ID(control.ID),
	Peer(control.Peer)
{
}

// FUNCTION: GAMEMD 0x0048E5A0
int ControlClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	if (flags) {
		if (ID) {
			key = (KeyNumType)(ID | KN_BUTTON);
			if (flags & RIGHTRELEASE && Flags & RIGHTPRESS) {
				key = (KeyNumType)(key | WWKEY_UNK_BIT);
			}
		} else {
			key = KN_NONE;
		}
	}

	if (Peer) {
		Peer->Peer_To_Peer(flags, key, *this);
	}

	return(BASECLASS::Action(flags, key, 0));
}


// FUNCTION: GAMEMD 0x0048E600
void ControlClass::Make_Peer(BASECLASS & gadget)
{
	Peer = &gadget;
}

// FUNCTION: GAMEMD 0x0048E610
unsigned ControlClass::Get_ID(void) const
{
	return(ID);
}

// FUNCTION: GAMEMD 0x0048E620
int ControlClass::Draw_Me(int forced)
{
	if (Peer) {
		Peer->Draw_Me();
	}
	return(BASECLASS::Draw_Me(forced));
}
// FUNCTION: GAMEMD 0x0048E650
// GadgetClass::Peer_To_Peer

// SYNTHETIC: GAMEMD 0x0048E660
// ??_GControlClass@@UAEPAXI@Z
