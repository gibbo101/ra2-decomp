// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "sidebar.h"
#include "_bench.h"
#include "_convert.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tooltip.h"
#include "bench.h"
#include "building.h"
#include "cctooltip.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "draw.h"
#include "event.h"
#include "factory.h"
#include "font.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "language\language.h"
#include "map.h"
#include "mixfile.h"
#include "movie.h"
#include "queue.h"
#include "rect.h"
#include "rules.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "voc.h"
#include "vox.h"
#include "bench.hh"
#include "color.hh"
#include <cmath>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007F2F30, DAT_007F2F38;
extern double DAT_00B07DE0, DAT_00B0B398, DAT_00B0B320, DAT_00B0B318, DAT_00B07DE8;
extern double DAT_00B07BB8, DAT_00B07DF0, DAT_00B0B310;
extern int DAT_00B0B400, DAT_00B0B394, DAT_00B0B300, DAT_00B0B304, DAT_00B0B308;
extern short DAT_00B07DD8, DAT_00B07DDA;

// FUNCTION: 0x006A4A20
void Init_006A4A20(void)
{
	DAT_00B07DE0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x006A4A50
void Init_006A4A50(void)
{
	DAT_00B0B398 = DAT_00B07DE0 / DAT_007F2F30;
}

// FUNCTION: 0x006A4A70
void Init_006A4A70(void)
{
	DAT_00B0B320 = DAT_007E1718 / DAT_00B0B398;
}

// FUNCTION: 0x006A4A90
void Init_006A4A90(void)
{
	DAT_00B0B318 = DAT_007E1720 * DAT_007F2F38;
}

// FUNCTION: 0x006A4AB0
void Init_006A4AB0(void)
{
	DAT_00B07DE8 = DAT_007E1728 * DAT_007F2F38;
}

// FUNCTION: 0x006A4AD0
void Init_006A4AD0(void)
{
	DAT_00B07BB8 = DAT_007E1730 * DAT_007F2F38;
}

// FUNCTION: 0x006A4AF0
void Init_006A4AF0(void)
{
	DAT_00B0B400 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B07BB8 - DAT_00B07DE8) * DAT_00B07DE0));
}

// FUNCTION: 0x006A4B30
void Init_006A4B30(void)
{
	DAT_00B07DF0 = FUN_004CADE0(DAT_00B0B400 * DAT_007E1740);
}

// FUNCTION: 0x006A4B60
void Init_006A4B60(void)
{
	DAT_00B0B310 = FUN_004CADE0((DAT_00B0B400 * 2) / DAT_00B07DE0);
}

// FUNCTION: 0x006A4B90
void Init_006A4B90(void)
{
	DAT_00B0B394 = (int)((DAT_00B0B400 * 4) + DAT_007E1738);
}

// FUNCTION: 0x006A4BC0
void Init_006A4BC0(void)
{
	DAT_00B07DD8 = 0;
	DAT_00B07DDA = 0;
}

// FUNCTION: 0x006A4BD0
void Init_006A4BD0(void)
{
	DAT_00B0B300 = 0;
	DAT_00B0B304 = 0;
	DAT_00B0B308 = 0;
}

// FUNCTION: 0x006AACE0
void SidebarClass::StripClass::SelectClass::Set_Owner(StripClass & strip, int index)
{
	Strip = &strip;
	Index = index;
}
