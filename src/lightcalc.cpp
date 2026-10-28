// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "lightcalc.h"
#include "globals.h"
#include "goptions.h"
#include "sun.h"

// SYNTHETIC: GAMEMD 0x005558C0 SYMBOL
// _$E2

// FUNCTION: GAMEMD 0x005558E0
void Adjust_Tile_RGB(int & intensity, int & light, int & red, int & green, int & blue)
{
	double temp;

	red = max(red, 0);
	green = max(green, 0);
	blue = max(blue, 0);

	red = min(red, 2000);
	green = min(green, 2000);
	blue = min(blue, 2000);

	intensity = 65536;

	if (red != NORMAL_LIGHT || green != red || blue != red) {
		if (green <= red && red >= blue) {
			temp = ((double)red * 65536) * 0.001;
			intensity = temp;

			if (intensity < 66) {
				intensity = 65536;
				red = NORMAL_LIGHT;
				green = NORMAL_LIGHT;
				blue = NORMAL_LIGHT;
				light = 0;
			} else {
				red = NORMAL_LIGHT;
				green = (((double)green * 65536) / (double)temp);
				blue = (((double)blue * 65536) / (double)temp);
				light *= intensity;
				light >>= 16;
			}
		} else if (green >= red && green >= blue) {
			temp = ((double)green * 65536) * 0.001;
			intensity = temp;

			if (intensity < 66) {
				intensity = 65536;
				red = NORMAL_LIGHT;
				green = NORMAL_LIGHT;
				blue = NORMAL_LIGHT;
				light = 0;
			} else {
				green = NORMAL_LIGHT;
				red = (((double)red * 65536) / (double)intensity);
				blue = (((double)blue * 65536) / (double)intensity);
				light *= intensity;
				light >>= 16;
			}
		} else {
			temp = ((double)blue * 65536) * 0.001;
			intensity = temp;

			if (intensity < 66) {
				intensity = 65536;
				red = NORMAL_LIGHT;
				green = NORMAL_LIGHT;
				blue = NORMAL_LIGHT;
				light = 0;
			} else {
				blue = NORMAL_LIGHT;
				red = (((double)red * 65536) / (double)intensity);
				green = (((double)green * 65536) / (double)intensity);
				light *= intensity;
				light >>= 16;
			}
		}
	}

	light = light >= 2000 ? 2000 : light;
}

// FUNCTION: GAMEMD 0x00555AC0
void Clamp_Tile_RGB(int & red, int & green, int & blue)
{
	red = min(red, NORMAL_LIGHT);
	green = min(green, NORMAL_LIGHT);
	blue = min(blue, NORMAL_LIGHT);

	red = max(red, 0);
	green = max(green, 0);
	blue = max(blue, 0);

	switch (Options.DetailLevel) {
		case 0:
			red &= ~((1 << 7) - 1);
			green &= ~((1 << 7) - 1);
			blue &= ~((1 << 7) - 1);
			break;
		case 1:
			red &= ~((1 << 6) - 1);
			green &= ~((1 << 6) - 1);
			blue &= ~((1 << 6) - 1);
			break;
		case 2:
			red &= ~((1 << 5) - 1);
			green &= ~((1 << 5) - 1);
			blue &= ~((1 << 5) - 1);
			break;
	}
}
