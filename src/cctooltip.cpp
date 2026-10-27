// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cctooltip.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_xmouse.h"
#include "dialog.h"
#include "dsurface.h"
#include "goptions.h"
#include "scheme.h"
#include "geometry.h"
#include "wwfont.h"
#include "color.hh"
#include <math.h>

// SYNTHETIC: GAMEMD 0x00478990 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004789B0 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x004789E0 SYMBOL
// _$E67
// SYNTHETIC: GAMEMD 0x00478A00 SYMBOL
// _$E70
// SYNTHETIC: GAMEMD 0x00478A20 SYMBOL
// _$E73
// SYNTHETIC: GAMEMD 0x00478A40 SYMBOL
// _$E76
// SYNTHETIC: GAMEMD 0x00478A60 SYMBOL
// _$E79
// SYNTHETIC: GAMEMD 0x00478A80 SYMBOL
// _$E82
// SYNTHETIC: GAMEMD 0x00478AC0 SYMBOL
// _$E85
// SYNTHETIC: GAMEMD 0x00478AF0 SYMBOL
// _$E88
// SYNTHETIC: GAMEMD 0x00478B20 SYMBOL
// _$E91
// SYNTHETIC: GAMEMD 0x00478B50 SYMBOL
// _$E94
// SYNTHETIC: GAMEMD 0x00478B60 SYMBOL
// _$E97
// SYNTHETIC: GAMEMD 0x00478B80 SYMBOL
// _$E100

// FUNCTION: GAMEMD 0x00478DB0
void CCToolTip::Reset(const ToolTipText * text)
{
	bool redraw = false;
	if (Options.IsSidebarOnRight == true) {
		if (text->Pos.x >= TacticalRect.X + TacticalRect.Width) {
			redraw = true;
		}
	} else {
		if (text->Pos.x <= SidebarRect.X + SidebarRect.Width) {
			redraw = true;
		}
	}

	if (redraw) {
		Map.SidebarClass::IsToRedraw = true;
		Map.SidebarClass::IsForceCompleteRedraw = true;
	}
	Map.Flag_To_Redraw();
	BASECLASS::Reset(text);
}

// FUNCTION: GAMEMD 0x00478E10
void CCToolTip::Draw_Current(bool sidebar)
{
	UseSidebarSurface = sidebar;
	BASECLASS::Draw_Current(sidebar == 0);
}

// FUNCTION: GAMEMD 0x00479050
const char * CCToolTip::ToolTip_Text(int id)
{
	if (Get_Mouse_State() < 0) {
		return(NULL);
	}
	return(Map.Help_Text(id));
}
