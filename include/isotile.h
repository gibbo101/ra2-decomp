// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "object.h"
#include "sun.h"

#include "isotype.hh"

#include <comdef.h>

class IsometricTileTypeClass;

class IsometricTileClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;
	public:
		IsometricTileClass(IsometricTileType type, Cell const &cell);
		IsometricTileClass(NoInitClass const &x) : BASECLASS(x) {}
		virtual ~IsometricTileClass() override;

		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;

		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual bool Limbo() override;
		virtual bool Unlimbo(Coord const &, Dir256 facing = DIR_N) override;
		virtual void Draw_It(Point2D const &point, Rect const &bounds) const override;
		virtual bool Mark(MarkType mark = MARK_CHANGE) override;

	public:
		IsometricTileTypeClass * Class;
};

inline IsometricTileClass * AbstractClass::As_IsometricTileClass(void) { return(dynamic_cast<IsometricTileClass *>(this)); }
inline IsometricTileClass const * AbstractClass::As_IsometricTileClass(void) const { return(dynamic_cast<IsometricTileClass const *>(this)); }
