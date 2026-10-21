// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "isotile.h"
#include "_isotile.h"
#include "_map.h"
#include "anim.h"
#include "cell.h"
#include "globals.h"
#include "isotype.h"
#include "overtype.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"

// FUNCTION: GAMEMD 0x00543780
IsometricTileClass::IsometricTileClass(IsometricTileType type, Cell const & cell) :
	BASECLASS(),
	Class(IsometricTileTypes[type])
{
	if (cell != CELL_NONE) {
		Unlimbo(Coord(cell));
	}

	IsometricTiles.Add(this);
}

// FUNCTION: GAMEMD 0x00543880
IsometricTileClass::~IsometricTileClass(void)
{
	Detach_This_From_All(this, true);
	Limbo();
	Class = NULL;
	IsometricTiles.Delete(this);
	AbstractTypePtrTracker.Delete(this);
}

// SYNTHETIC: GAMEMD 0x00543B10
// ??_GIsometricTileClass@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00543990
HRESULT STDMETHODCALLTYPE IsometricTileClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) IsometricTileClass(NoInitClass());

		Swizzle_Pointer(&Class);
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005439F0
HRESULT STDMETHODCALLTYPE IsometricTileClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00543A10
bool IsometricTileClass::Unlimbo(Coord const & coord, Dir256 dir)
{
	IsInLimbo = false;
	PositionCoord = coord;
	return(Mark(MARK_DOWN));
}

// FUNCTION: GAMEMD 0x00543A40
bool IsometricTileClass::Limbo(void)
{
	if (!GameActive || IsInLimbo) {
		return(false);
	}
	Unselect();
	Detach_All(true);
	Mark(MARK_UP);
	IsInLimbo = true;
	IsToDisplay = false;
	return(true);
}

// FUNCTION: GAMEMD 0x00543A90
int IsometricTileClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00543AA0
RTTIType IsometricTileClass::Fetch_RTTI(void) const
{
	return(RTTI_ISOTILE);
}

// FUNCTION: GAMEMD 0x00543AB0
HRESULT STDMETHODCALLTYPE IsometricTileClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_IsometricTileClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00543AF0
ObjectTypeClass const * IsometricTileClass::Class_Of(void) const
{
	return(Class);
}

// FUNCTION: GAMEMD 0x00543B00
void IsometricTileClass::Draw_It(Point2D const & point, Rect const & cliprect) const
{
}

// SYNTHETIC: GAMEMD 0x00543C40 SYMBOL
// _$E2
