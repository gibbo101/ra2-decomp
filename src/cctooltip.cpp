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
#include "wwfont.h"
#include "color.hh"

// FUNCTION: 0x00478DB0
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

// FUNCTION: 0x00479050
const char * CCToolTip::ToolTip_Text(int id)
{
	if (Get_Mouse_State() < 0) {
		return(NULL);
	}
	return(Map.Help_Text(id));
}
