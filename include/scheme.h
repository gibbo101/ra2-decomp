// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "hsv.h"
#include "noinit.h"
#include "palette.h"

#include "scheme.hh"

class Surface;
template<class T> class DynamicVectorClass;
class ConvertClass;
class LightConvertClass;

class ColorScheme
{
	public:
		ColorScheme(void);
		ColorScheme(char const * name, HSVClass &hsv, const PaletteClass & base, const PaletteClass & screenpalette, int intensity_levels);
		~ColorScheme(void);

		void Build_Light_Converters(const PaletteClass & base, const PaletteClass & screenpalette, int r = 1000, int g = 1000, int b = 1000);

		bool operator==(const ColorScheme &that);
		bool operator!=(const ColorScheme &that);

		static DynamicVectorClass<ColorScheme *> * __fastcall GeneratePalette(char const * name);
		static int Find_Or_Make(char const * name, HSVClass &hsv, const PaletteClass & base, const PaletteClass & screenpalette, int intensity_levels);

	public:

		int ID;

		PaletteClass Palette;

		char * Name;

		HSVClass HSV;

		LightConvertClass * Converter;

		int IntensityLevels;

		int Color;
		int BrightColor;

		int Shadow;
		int Background;
		int Corners;
		int Highlight;
		int Box;
		int Bright;
		int Underline;
		int Bar;

};

ConvertClass *Build_Converter(HSVClass & hsv, const PaletteClass & base, const PaletteClass & screenpalette, Surface const & typicalsurface, PaletteClass *remapped);
ConvertClass *Build_Light_Converter(HSVClass & hsv, const PaletteClass & base, const PaletteClass & screenpalette, PaletteClass *remapped, Surface const & typicalsurface, int intensity_levels, int r, int g, int b, bool *indexes);

ColorScheme * Fetch_Scheme_By_Name(const char * name, int intensity_levels = 1);
int Fetch_Scheme_Index_By_Name(char const * name, int intensity_levels = 1);

extern DynamicVectorClass<ColorScheme *> ColorSchemes;
