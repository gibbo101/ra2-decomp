// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <stddef.h>

class Surface;
class ConvertClass;
class SHPStruct;
template<class T> class TRect;
typedef TRect<int> Rect;

class MSFont
{
	public:
		MSFont(bool use_side_palette = true);
		virtual ~MSFont(void);

		int Get_Font_Width(void) {return(FontWidth);}
		int Get_Font_Height(void) {return(FontHeight);}

		unsigned char Get_Red(void) {return(Red);}
		unsigned char Get_Green(void) {return(Green);}
		unsigned char Get_Blue(void) {return(Blue);}
		int Get_Color(void) {return(Color);}

		bool Init(bool use_side_palette);

		void Get_String_Rect(wchar_t const * string, Rect & rect);

		virtual int Get_Character_Width(wchar_t character);
		virtual int Get_String_Width(wchar_t const * string);

		virtual void Draw_String(Surface * surface, wchar_t const * string, int x, int y, int frame);
		virtual void Draw_Character(Surface * surface, wchar_t character, int x, int y, int frame, bool do_sound);

	private:
		// Glyph cell size in pixels; the height is the newline advance.
		int FontWidth;
		int FontHeight;

		// The print colour's components, read from the palette that colours the glyphs.
		unsigned char Red;
		unsigned char Green;
		unsigned char Blue;

		// The print colour as a pixel in the display's format, for line and rectangle drawing.
		int Color;

		// Glyph shapes: three frames per printable character, the last one measured for width.
		SHPStruct * FontFile;

		// Remaps the glyphs from the font's palette into the visible surface's colours.
		ConvertClass * Drawer;

		// True when the shapes were loaded from disk and so are freed with the last font.
		bool AllocLoaded;
};
