// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "scheme.h"
#include "_surface.h"
#include "fastmath.h"
#include "lightcon.h"
#include "sun.h"
#include "vector.h"
#include <cstring>

bool _indexes[256] =
{
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	1
};

ConvertClass *Build_Light_Converter(HSVClass & hsv, const PaletteClass & base, const PaletteClass & screenpalette, PaletteClass * remapped, Surface const & typicalsurface, int intensity_levels, int r, int g, int b, bool *indexes)
{
	int hue = hsv.Get_Hue();
	int saturation = hsv.Get_Saturation();
	int value = hsv.Get_Value();

	*remapped = base;

	double cos_step = DEG_TO_RAD(14.0 / 3.0);
	double sin_step = DEG_TO_RAD(8.0 / 3.0);

	for (int i = 0; i < 16; i++) {
		double cosval = DEG_TO_RAD(20) + (i * cos_step);
		double sinval = DEG_TO_RAD(50) + (i * sin_step);
		if (i == 0) {
			cosval = DEG_TO_RAD(360.0/32.0);
		}

		int sat = int(fastmath::sin(sinval) * saturation);
		int val = int(fastmath::cos(cosval) * value);

		(*remapped)[i + 16] = HSVClass(hue, sat, val);
	}

	return(new LightConvertClass(*remapped, screenpalette, typicalsurface, r, g, b, false, indexes, intensity_levels));
}

// FUNCTION: GAMEMD 0x0068C650
ColorScheme::ColorScheme(void) :
	Name(NULL),
	HSV(0,0,0),
	Converter(NULL),
	IntensityLevels(1)
{
	ColorSchemes.Add(this);
	ID = ColorSchemes.Count();
}

// FUNCTION: GAMEMD 0x0068C710
ColorScheme::ColorScheme(char const * name, HSVClass & hsv, const PaletteClass & pal1, const PaletteClass & pal2, int shade_count, bool add_to_array) :
	Name(strdup(name)),
	HSV(hsv),
	Converter(NULL),
	IntensityLevels(shade_count),
	Color(16),
	BrightColor(15),
	Shadow(25),
	Background(24),
	Corners(22),
	Highlight(16),
	Box(19),
	Bright(16),
	Underline(16),
	Bar(21)
{
	Converter = (LightConvertClass *)Build_Light_Converter(HSV, pal1, pal2, &Palette, *VisibleSurface, shade_count, 1000, 1000, 1000, _indexes);

	if (add_to_array) {
		ColorSchemes.Add(this);
		ID = ColorSchemes.Count();
	}
}

// FUNCTION: GAMEMD 0x0068C860
void ColorScheme::Build_Light_Converters(const PaletteClass & base, const PaletteClass & screenpalette, int r, int g, int b)
{
	if (Converter != NULL) {
		delete Converter;
		Converter = NULL;
	}

	Converter = (LightConvertClass *)Build_Light_Converter(HSV, base, screenpalette, &Palette, *VisibleSurface, IntensityLevels, r, g, b, _indexes);
}

// FUNCTION: GAMEMD 0x0068C8D0
ColorScheme::~ColorScheme(void)
{
	if (Name != NULL) {
		free(Name);
	}
	Name = NULL;

	if (Converter != NULL) {
		delete Converter;
	}
	Converter = NULL;

	ColorSchemes.Delete(this);
}

// FUNCTION: GAMEMD 0x0068C960
bool ColorScheme::operator==(ColorScheme const & that)
{
	return(stricmp(Name, that.Name) == 0);
}

// FUNCTION: GAMEMD 0x0068C990
bool ColorScheme::operator!=(ColorScheme const & that)
{
	return(stricmp(Name, that.Name) != 0);
}

// FUNCTION: GAMEMD 0x0068C9C0
int ColorScheme::Find_Or_Make(char const * name, HSVClass & hsv, const PaletteClass & base, const PaletteClass & screenpalette, int intensity_levels)
{
	while (true) {
		for (int i = 0; i < ColorSchemes.Count(); i++) {
			if (stricmp(name, ColorSchemes[i]->Name) == 0 && ColorSchemes[i]->IntensityLevels == intensity_levels) {
				return(i);
			}
		}
		new ColorScheme(name, hsv, base, screenpalette, intensity_levels, true);
	}
}

// FUNCTION: GAMEMD 0x0068CA50
ColorScheme * Fetch_Scheme_By_Name(char const * name, int intensity_levels)
{
	int id = Fetch_Scheme_Index_By_Name(name, intensity_levels);

	if (id >= 0) {
		return(ColorSchemes[id]);
	}

	return(NULL);
}

// FUNCTION: GAMEMD 0x0068CAB0
int Fetch_Scheme_Index_By_Name(char const * name, int intensity_levels)
{
	for (int i = 0; i < ColorSchemes.Count(); i++) {
		if (stricmp(name, ColorSchemes[i]->Name) == 0 && ColorSchemes[i]->IntensityLevels == intensity_levels) {
			return(i);
		}
	}
	return(-1);
}
