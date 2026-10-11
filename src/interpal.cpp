// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "interpal.h"
#include "ccfile.h"
#include "hsv.h"
#include "misc.h"
#include "surface.h"

extern "C"{
	unsigned char PaletteInterpolationTable[SIZE_OF_PALETTE][SIZE_OF_PALETTE];
	unsigned char * InterpolationPalette;
}

// FUNCTION: 0x00538D40
void Create_Palette_Interpolation_Table( void )
{
	#if (1)

	int 				i;
	int 				j;
	int 				p;
	unsigned char	* first_palette_ptr;
	unsigned char	* second_palette_ptr;
	unsigned char	* match_pal_ptr;
	int				first_r;
	int				first_g;
	int				first_b;
	int				second_r;
	int				second_g;
	int				second_b;
	int				diff_r;
	int				diff_g;
	int				diff_b;
	int				dest_r;
	int				dest_g;
	int				dest_b;
	int 				distance;
	int 				closest_distance;
	int 				index_of_closest_color;

	first_palette_ptr = (unsigned char *) InterpolationPalette;
	for ( i = 0; i < SIZE_OF_PALETTE; i ++ ) {
		first_r = *first_palette_ptr;
		first_palette_ptr ++;
		first_g = *first_palette_ptr;
		first_palette_ptr ++;
		first_b = *first_palette_ptr;
		first_palette_ptr ++;

		second_palette_ptr = (unsigned char *) InterpolationPalette;
		for  ( j = 0; j < SIZE_OF_PALETTE; j ++ ) {
			second_r = *second_palette_ptr;
			second_palette_ptr ++;
			second_g = *second_palette_ptr;
			second_palette_ptr ++;
			second_b = *second_palette_ptr;
			second_palette_ptr ++;

			dest_r = ( first_r + second_r ) >> 1;
			dest_g = ( first_g + second_g ) >> 1;
			dest_b = ( first_b + second_b ) >> 1;

			index_of_closest_color = 0;
			closest_distance = 500000;
			match_pal_ptr = (unsigned char *) InterpolationPalette;
			for ( p = 0; p < SIZE_OF_PALETTE; p ++ ) {
				diff_r = ( ((int) (*match_pal_ptr)) - dest_r );
				match_pal_ptr ++;
				diff_g = ( ((int) (*match_pal_ptr)) - dest_g );
				match_pal_ptr ++;
				diff_b = ( ((int) (*match_pal_ptr)) - dest_b );
				match_pal_ptr ++;

				distance = ( diff_r * diff_r ) + ( diff_g * diff_g ) + ( diff_b * diff_b );
				if ( distance < closest_distance ) {
					closest_distance = distance;
					index_of_closest_color = p;
				}
			}

			PaletteInterpolationTable[ i ][ j ] = (unsigned char) index_of_closest_color;
		}
	}

	#endif
	InterpolationPaletteChanged = FALSE;
	return;
}

// FUNCTION: 0x00538E70
void Increase_Palette_Luminance (PaletteClass & palette , double percentage)
{
	for (int i=0 ; i<SIZE_OF_PALETTE; i++) {
		HSVClass hsv = palette[i];

		int value = int(hsv.Get_Value() * percentage);
		value = MIN(value, 255);
		value = MAX(value, 0);
		hsv.Set_Value(value);
		palette[i] = hsv;
	}
}
