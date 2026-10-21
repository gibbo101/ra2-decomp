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
#include "geometry.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E4E98;
extern const double DAT_007E4EA0;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x004789B0 SYMBOL
// _$E64
double DAT_0089E330 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004789E0 SYMBOL
// _$E67
double DAT_0089E380 = DAT_0089E330 / DAT_007E4E98;

// SYNTHETIC: GAMEMD 0x00478A00 SYMBOL
// _$E70
double DAT_0089E368 = DAT_007E1718 / DAT_0089E380;

// SYNTHETIC: GAMEMD 0x00478A20 SYMBOL
// _$E73
double DAT_0089E360 = DAT_007E1720 * DAT_007E4EA0;

// SYNTHETIC: GAMEMD 0x00478A40 SYMBOL
// _$E76
double DAT_0089E338 = DAT_007E1728 * DAT_007E4EA0;

// SYNTHETIC: GAMEMD 0x00478A60 SYMBOL
// _$E79
double DAT_0089E310 = DAT_007E1730 * DAT_007E4EA0;

// SYNTHETIC: GAMEMD 0x00478A80 SYMBOL
// _$E82
int DAT_0089E388 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089E310 - DAT_0089E338) * DAT_0089E330));

// SYNTHETIC: GAMEMD 0x00478AC0 SYMBOL
// _$E85
double DAT_0089E340 = FUN_004CADE0(DAT_0089E388 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00478AF0 SYMBOL
// _$E88
double DAT_0089E358 = FUN_004CADE0((DAT_0089E388 * 2) / DAT_0089E330);

// SYNTHETIC: GAMEMD 0x00478B20 SYMBOL
// _$E91
int DAT_0089E37C = (int)(DAT_0089E388 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x00478B50 SYMBOL
// _$E94
ShortPair DAT_0089E328;

// SYNTHETIC: GAMEMD 0x00478B60 SYMBOL
// _$E97
IntTriple DAT_0089E348;

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

// SYNTHETIC: GAMEMD 0x00479080 SYMBOL
// _$E2
