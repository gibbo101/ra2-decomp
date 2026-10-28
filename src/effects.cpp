// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "effects.h"
#include "palette.h"
#include "rect.h"
#include "win.h"

// SYNTHETIC: GAMEMD 0x004C35A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004C35D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004C35F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004C3610 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004C3630 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004C3650 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004C3670 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004C36B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004C36E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004C3710 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004C3740 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004C3750 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004C3770 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004C3790 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x004C37B0 SYMBOL
// _$E44
const Rect RECT_NONE_2(0, 0, 0, 0);

void * Build_Fading_Table(PaletteClass const & palette, void * dest, int color, int frac);

// FUNCTION: GAMEMD 0x004C37D0
void * Conquer_Build_Fading_Table(PaletteClass const & palette, void * dest, int color, int frac)
{
	if (dest) {
		unsigned char * ptr = (unsigned char *)dest;

		for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {
			if (index > PaletteClass::COLOR_COUNT-16 || index == 0) {
				*ptr++ = index;
			} else {
				RGBClass trycolor = palette[index];
				trycolor.Adjust(frac, palette[color]);

				int best = -1;
				int bvalue = 0;
				for (int id = PaletteClass::COLOR_COUNT-16; id < PaletteClass::COLOR_COUNT-1; id++) {
					int diff = palette[id].Difference(trycolor);
					if (best == -1 || diff < bvalue) {
						best = id;
						bvalue = diff;
					}
				}
				*ptr++ = best;
			}
		}
	}
	return(dest);
}

// FUNCTION: GAMEMD 0x004C38D0
int Translucent_Table_Size(int count)
{
	return(256L + (256L * count));
}

// FUNCTION: GAMEMD 0x004C38E0
void * Build_Translucent_Table(PaletteClass const & palette, TLucentType const * control, int count, void * buffer)
{
	unsigned char	const *table;
	int			index;

	if (count && control) {
		if (!buffer) {
			buffer = new char [Translucent_Table_Size(count)];
		}

		if (buffer) {
			memset(buffer, -1, 256);
			table = (unsigned char*)buffer + 256;

			for (index = 0; index < count; index++) {
				((unsigned char*)buffer)[control[index].SourceColor] = index;
				Build_Fading_Table(palette, (void*)table, control[index].DestColor, control[index].Fading);
				table = (unsigned char*)table + 256;
			}
		}
	}
	return(buffer);
}

// FUNCTION: GAMEMD 0x004C3A20
void * Conquer_Build_Translucent_Table(PaletteClass const & palette, TLucentType const * control, int count, void * buffer)
{
	unsigned char	const *table;

	if (count && control) {
		if (!buffer) {
			buffer = new char [Translucent_Table_Size(count)];
		}

		if (buffer) {
			memset(buffer, -1, 256);
			table = (unsigned char*)buffer + 256;

			for (int index = 0; index < count; index++) {
				((unsigned char*)buffer)[control[index].SourceColor] = index;
				Conquer_Build_Fading_Table(palette, (void*)table, control[index].DestColor, control[index].Fading);
				table = (unsigned char*)table + 256;
			}
		}
	}
	return(buffer);
}

// FUNCTION: GAMEMD 0x004C3AD0
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

// FUNCTION: GAMEMD 0x004C3B60
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
