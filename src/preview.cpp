// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "preview.h"
#include "_map.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccfile.h"
#include "ccini.h"
#include "cell.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "language\language.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "overtype.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "scenario.h"
#include "surface.h"
#include "tactical.h"
#include "terrain.h"
#include "windlg.h"
#include "xpipe.h"
#include "xstraw.h"
#include "color.hh"

// FUNCTION: 0x006406E0
MapPreviewClass::MapPreviewClass(void) :
	SurfacePtr(NULL)
{
}

// FUNCTION: 0x006406F0
MapPreviewClass::~MapPreviewClass(void)
{
	if (SurfacePtr != NULL) {
		delete SurfacePtr;
		SurfacePtr = NULL;
	}
}
