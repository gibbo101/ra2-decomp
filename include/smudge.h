// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "globals.h"
#include "noinit.h"
#include "object.h"
#include "smudtype.h"

class SmudgeClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		SmudgeTypeClass * Class;

		SmudgeClass(SmudgeTypeClass const * type, Coord const & pos = COORD_NONE, HousesType = HOUSE_NONE);
		SmudgeClass(NoInitClass const & x) : BASECLASS(x) {};
		virtual ~SmudgeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual int Fetch_Object_Size(void) const override;
		virtual RTTIType Fetch_RTTI(void) const override;

		static void Read_INI(CCINIClass const & ini);
		static void Write_INI(CCINIClass & ini);

		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual bool Mark(MarkType) override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override {};

		void Disown(Cell const & cell);

	private:
		static HousesType ToOwn;

		static char const * const INI_NAME;
};
