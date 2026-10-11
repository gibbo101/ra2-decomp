// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vanim.h"
#include "_convert.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_vanim.h"
#include "anim.h"
#include "cell.h"
#include "combat.h"
#include "crc.h"
#include "draw.h"
#include "house.h"
#include "inline.h"
#include "lightcon.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "rect.h"
#include "rules.h"
#include "scheme.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tiberium.h"
#include "tracker.h"
#include "vanimtype.h"
#include "voc.h"
#include "voxdrsys.h"

// FUNCTION: 0x00749F00
void VoxelAnimClass::Init_Clear(void)
{
	while (VoxelAnims.Count()) {
		delete VoxelAnims[0];
	}
}

// FUNCTION: 0x0074AA10
HRESULT STDMETHODCALLTYPE VoxelAnimClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0074AAD0
HRESULT STDMETHODCALLTYPE VoxelAnimClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VoxelAnimClass;
	return(S_OK);
}
