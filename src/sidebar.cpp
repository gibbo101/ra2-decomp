// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
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

// FUNCTION: 0x006A4A00 $E2
// FUNCTION: 0x006A4A20 $E5
// FUNCTION: 0x006A4A50 $E8
// FUNCTION: 0x006A4A70 $E11
// FUNCTION: 0x006A4A90 $E14
// FUNCTION: 0x006A4AB0 $E17
// FUNCTION: 0x006A4AD0 $E20
// FUNCTION: 0x006A4AF0 $E23
// FUNCTION: 0x006A4B30 $E26
// FUNCTION: 0x006A4B60 $E29
// FUNCTION: 0x006A4B90 $E32
// FUNCTION: 0x006A4BC0 $E35
// FUNCTION: 0x006A4BD0 $E38
// FUNCTION: 0x006A4BF0 $E41

// FUNCTION: 0x006AACE0
void SidebarClass::StripClass::SelectClass::Set_Owner(StripClass & strip, int index)
{
	Strip = &strip;
	Index = index;
}
