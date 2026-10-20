// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "buff.h"
#include "gadget.h"

#include "dialog.hh"

class StaticButtonClass : public GadgetClass
{
		typedef GadgetClass BASECLASS;

	public:
		StaticButtonClass(void);
		StaticButtonClass(unsigned id, wchar_t const * text, TextPrintType style, int x, int y, int w=-1, int h=-1);
		virtual ~StaticButtonClass(void) override;
		virtual int Draw_Me(int forced=false) override;
		virtual void Set_Text(wchar_t const * text, bool resize = false);

	protected:

		virtual void Draw_Background(void);
		virtual void Draw_Text(wchar_t const * text);

		Buffer Background;

		wchar_t * String;

		TextPrintType PrintFlags;
};
