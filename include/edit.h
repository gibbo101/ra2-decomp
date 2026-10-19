// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "control.h"

#include "dialog.hh"

class EditClass : public ControlClass
{
		typedef ControlClass BASECLASS;

	public:
		enum EditStyle {
			ALPHA			=0x0001,
			NUMERIC		=0x0002,
			MISC			=0x0004,
			UPPERCASE	=0x0008,
			ALPHANUMERIC=(int)ALPHA|(int)NUMERIC|(int)MISC
		};

		EditClass (int id, wchar_t * text, int max_len, TextPrintType flags, int x, int y, int w=-1, int h=-1, EditStyle style=ALPHANUMERIC);
		virtual ~EditClass(void) override;

		virtual void Set_Focus(void) override;
		virtual int  Draw_Me(int forced) override;
		virtual void Set_Text(wchar_t * text, int max_len);
		virtual wchar_t * Get_Text(void) {return(String);};
		void Set_Color (int color) { Color = color; }

		void Set_Read_Only(int rdonly) {IsReadOnly = rdonly;}

	protected:
		TextPrintType TextFlags;

		EditStyle EditFlags;

		wchar_t *String;
		int MaxLength;

		int Length;

		int Color;

		virtual int Action (unsigned flags, KeyNumType &key, int modifier) override;
		virtual void Draw_Background(void);
		virtual void Draw_Text(wchar_t const * text);
		virtual bool Handle_Key(KeyASCIIType ascii);

	private:
		int IsReadOnly;
};
