// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "gadget.h"

#include "dialog.hh"

class TextLabelClass : public GadgetClass
{
		typedef GadgetClass BASECLASS;

	public:
		TextLabelClass(wchar_t *txt, int x, int y, int color, TextPrintType style);

		virtual int Draw_Me(int forced = false) override;

		virtual void Set_Text(wchar_t *txt) {Text = txt;};

		unsigned int UserData1;
		unsigned int UserData2;
		TextPrintType Style;
		wchar_t *Text;
		int Color;
		int PixWidth;
		unsigned int AnimDword3C;
		bool SkipDraw;
		bool Animate;
		int AnimPos;
		int AnimTiming;
};
