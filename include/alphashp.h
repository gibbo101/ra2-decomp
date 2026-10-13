// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "rect.h"

class ShapeSet;
template<class T> class DynamicVectorClass;

class AlphaShapeClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

	public:

		AlphaShapeClass(ObjectClass * owner, int x, int y);
		AlphaShapeClass(void);
		AlphaShapeClass(NoInitClass const & x);
		~AlphaShapeClass(void);

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;

		static void Update_All(void);
		static void Draw_In_Area(Point2D const & point, Rect const & cliprect);
		static void Draw_All(Rect const & rect);
		static void Calculate_Brightness_Table(void);

	public:

		ObjectClass * Owner;

		Rect DrawRect;

		ShapeSet const * ImageData;

		bool IsToDelete;

	private:

		static unsigned char BrightnessTable[256][256];
};

extern DynamicVectorClass<AlphaShapeClass *> AlphaShapes;
