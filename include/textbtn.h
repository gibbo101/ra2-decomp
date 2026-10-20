// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "toggle.h"

#include "dialog.hh"

class TextButtonClass : public ToggleClass
{
		typedef ToggleClass BASECLASS;

	public:
		TextButtonClass(void);
		TextButtonClass(unsigned id, wchar_t const * text, TextPrintType style, int x, int y, int w=-1, int h=-1, bool blackborder=false, bool nobackground=false);
		virtual int Draw_Me(int forced=false) override;
		virtual void Set_Text(wchar_t const * text, bool resize = false);
		virtual void Set_Style (TextPrintType style) {PrintFlags = style;}

	protected:

		virtual void Draw_Background(void);
		virtual void Draw_Text(wchar_t const * text);

		bool IsBlackBorder;

		bool IsNoBackground;

		wchar_t const * String;

		TextPrintType PrintFlags;
};
