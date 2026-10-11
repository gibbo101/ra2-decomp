// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "movie.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "movies.h"
#include "session.h"
#include "vector.h"
#include "vqa.h"
#include "vq.hh"

// FUNCTION: 0x005BECC0
void Rebuild_Interpolated_Palette(unsigned char * interpal)
{
	for (int y=0; y<SIZE_OF_PALETTE - 1; y++) {
		for (int x=y+1; x<SIZE_OF_PALETTE; x++) {
			*(interpal + (y*SIZE_OF_PALETTE+x)) = *(interpal + (x*SIZE_OF_PALETTE+y));
		}
	}
}

// FUNCTION: 0x005BF520
bool Has_Ingame_Movies(void)
{
	return(IngameVQ.Count() > 0);
}
