// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "grphmenu.h"
#include "_keyboar.h"
#include "_surface.h"
#include "ccfile.h"
#include "globals.h"
#include "grphmitm.h"
#include "ini.h"
#include "keyboard.h"
#include "msanim.h"
#include "ownrdraw.h"
#include "theme.h"

// FUNCTION: 0x004F2280
void GraphicMenu::Set_Item_Enabled(int id, bool enabled)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Get_ID() == id) {
			it->Set_Enabled(enabled);
		}
	}
}

// FUNCTION: 0x004F2640
void GraphicMenu::Set_Animation(MSAnim *anim)
{
	MSAnim * old = CurrentAnim;
	if (old != NULL) {
		Engine.Replace_Anim(old, anim);
	} else {
		Engine.Add_Animation(anim);
	}
	CurrentAnim = anim;
}

// FUNCTION: 0x004F26E0
GraphicMenuItem * GraphicMenu::Get_Item_Under_Mouse(Point2D const & mouse)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Is_Mouse_Over(mouse)) {
			break;
		}
	}
	if (it) {
		return(*it);
	}
	return(NULL);
}

// FUNCTION: 0x004F2770
GraphicMenuItem * GraphicMenu::Get_Item_For_Key(KeyNumType key)
{
	ITEM_ITER it = Items;
	for (; it; it++) {
		if (it->Is_Input_Key(key)) {
			break;
		}
	}
	if (it) {
		return(*it);
	}
	return(NULL);
}
