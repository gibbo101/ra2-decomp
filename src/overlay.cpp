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

// FUNCTION: 0x005FC380
OverlayClass::OverlayClass(OverlayTypeClass const * ttype, Cell const & pos, HousesType house) :
	Class((OverlayTypeClass *)ttype)
{
	Create_ID();

	Array.Add(this);

	if (pos != CELL_NONE) {
		ToOwn = house;
		Coord coord(pos);
		CellClass & cellref = Map[coord.As_Cell()];
		if (cellref.Cell_Terrain(false) == NULL) {
			BASECLASS::Unlimbo(coord, DIR_N);
		}
		ToOwn = HOUSE_NONE;
	}
}

// FUNCTION: 0x005FC4D0
// FUNCTION: 0x005FDF70
OverlayClass::~OverlayClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);

	if (GameActive) {
		OverlayClass::Limbo();
	}
	Class = NULL;
}

// FUNCTION: 0x005FD270
bool OverlayClass::Unlimbo(Coord const & coord, Dir256 facing)
{
	CellClass & cellref = Map[coord.As_Cell()];
	if (cellref.Cell_Terrain(false) != NULL) {
		return(false);
	}
	return(BASECLASS::Unlimbo(coord, facing));
}

// FUNCTION: 0x005FD8F0
HRESULT STDMETHODCALLTYPE OverlayClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) OverlayClass(NoInitClass());

		Swizzle_Pointer(&Class);
	}
	return(result);
}

// FUNCTION: 0x005FD950
HRESULT STDMETHODCALLTYPE OverlayClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	return(result);
}

// FUNCTION: 0x005FDF10
HRESULT STDMETHODCALLTYPE OverlayClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_OverlayClass;
	return(S_OK);
}

extern int DAT_00AC15F8, DAT_00AC15FC, DAT_00AC1600, DAT_00AC1604;

// FUNCTION: 0x005FE040
void Init_005FE040(void)
{
	DAT_00AC15F8 = 0;
	DAT_00AC15FC = 0;
	DAT_00AC1600 = 0;
	DAT_00AC1604 = 0;
}
