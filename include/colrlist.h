// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "list.h"
#include "vector.h"

#include "scheme.hh"

class ShapeSet;

class ColorListClass : public ListClass
{
		typedef ListClass BASECLASS;

	public:

		enum SelectStyleType {
			SELECT_NORMAL,
			SELECT_HIGHLIGHT,
			SELECT_BOX,
			SELECT_BAR,
			SELECT_INVERT
		};

		ColorListClass(int id, int x, int y, int w, int h, TextPrintType flags, ShapeSet const * up, ShapeSet const * down);
		virtual ~ColorListClass(void) override;

		virtual int  Add_Item(wchar_t const * text, int color = 0);
		virtual void Remove_Item(wchar_t const * text) override;
		virtual void Remove_Item(int) override;

		virtual void Set_Selected_Style(SelectStyleType style, int color = 0);

		virtual int Add_Item(wchar_t const * text) override { return(Add_Item(text, 0)); }

		DynamicVectorClass<int> Colors;

	protected:
		virtual void Draw_Entry(int index, int x, int y, int width, int selected) override;

		SelectStyleType Style;
		int SelectColor;

};
