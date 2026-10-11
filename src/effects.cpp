// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "effects.h"
#include "palette.h"
#include "win.h"

void * Build_Fading_Table(PaletteClass const & palette, void * dest, int color, int frac);

// FUNCTION: 0x004C3AD0
void * Make_Fading_Table(PaletteClass const & palette, void * dest, int color, int frac)
{
	if (dest) {
		unsigned char * ptr = (unsigned char *)dest;

		for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {
			RGBClass trycolor = palette[index];
			trycolor.Adjust(frac, palette[color]);

			*ptr++ = palette.Closest_Color(trycolor);
		}
	}
	return(dest);
}

// FUNCTION: 0x004C3B60
void * Build_Fading_Table(PaletteClass const & palette, void * dest, int color, int frac)
{
	unsigned char * ptr = (unsigned char *)dest;

	RGBClass c = palette[color];
	ptr[0] = 0;
	for (int index = 1; index < PaletteClass::COLOR_COUNT; index++) {
		RGBClass rgb = palette[index];
		rgb.Adjust(frac, c);
		ptr[index] = palette.Closest_Color(rgb);
	}

	return(ptr);
}
