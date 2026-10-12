// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "isun.h"
#include "object.h"
#include "vector.h"

#include "overlay.hh"
#include "tiberium.hh"

class CCINIClass;
class OverlayTypeClass;

class OverlayClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:

		OverlayTypeClass * Class;

		static DynamicVectorClass<OverlayClass *> Array;

		OverlayClass(OverlayTypeClass const * ttype, Cell const & pos = CELL_NONE, HousesType = HOUSE_NONE);
		OverlayClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~OverlayClass(void) override;

		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		static void Read_INI(CCINIClass & ini);
		static void Write_INI(CCINIClass & ini);
		static void Post_Read_Vein_Fixups(void);

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_OVERLAY);}
		virtual bool Unlimbo(Coord const & coord, Dir256 facing = DIR_N) override;
		virtual bool Mark(MarkType) override;
		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual void Draw_It(Point2D const &, Rect const &) const override {}
		virtual void Editor_Draw_It(Point2D const & point, Rect const & cliprect) const override;

	private:

		static HousesType ToOwn;

		static char const * const INI_NAME;
};

Point2D Overlay_Draw_Offset(OverlayType overlay);
TiberiumType Which_Tiberium_Type(OverlayType overlay);
