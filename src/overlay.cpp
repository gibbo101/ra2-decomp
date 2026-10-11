// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "overlay.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "anim.h"
#include "bsurface.h"
#include "building.h"
#include "ccini.h"
#include "cell.h"
#include "dbgprint.h"
#include "draw.h"
#include "globals.h"
#include "house.h"
#include "ini.h"
#include "inline.h"
#include "lcwpipe.h"
#include "lcwstraw.h"
#include "lightcon.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tiberium.h"
#include "tracker.h"
#include "tube.h"
#include "vector.h"
#include "vein.h"
#include "xpipe.h"
#include "xstraw.h"
#include "overlay.hh"

// FUNCTION: 0x005FD950
HRESULT STDMETHODCALLTYPE OverlayClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	return(result);
}
