// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "egos.h"
#include "_keyboar.h"
#include "_palette.h"
#include "_rect.h"
#include "_surface.h"
#include "_timer.h"
#include "addon.h"
#include "ccfile.h"
#include "conquer.h"
#include "dialog.h"
#include "dsurface.h"
#include "font.h"
#include "gadget.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "language\language.h"
#include "misc.h"
#include "ownrdraw.h"
#include "scheme.h"
#include "theme.h"
#include "vector.h"
#include "windlg.h"
#include "color.hh"
#include "dialog.hh"
#include <cstring>

// FUNCTION: 0x004C3CE0
bool EgoClass::Scroll(int distance)
{
	YPos -= distance;
	if (YPos < -20) {
		return(true);
	}else{
		return(false);
	}
}
