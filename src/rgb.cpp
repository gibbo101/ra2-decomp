// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rgb.h"
#include "hsv.h"

// FUNCTION: 0x00661020
RGBClass & RGBClass::Lerp(RGBClass const & a, RGBClass const & b, float t)
{
	Red = (unsigned char)MAX(0.0f, MIN(255.0f, a.Red * (1.0f - t) + b.Red * t));
	Green = (unsigned char)MAX(0.0f, MIN(255.0f, a.Green * (1.0f - t) + b.Green * t));
	Blue = (unsigned char)MAX(0.0f, MIN(255.0f, a.Blue * (1.0f - t) + b.Blue * t));

	return(*this);
}

// FUNCTION: 0x00661190
RGBClass & RGBClass::Set(RGBClass const & rgb, float value)
{
	Red = (unsigned char)MAX(0.0f, MIN(255.0f, rgb.Red * value));
	Green = (unsigned char)MAX(0.0f, MIN(255.0f, rgb.Green * value));
	Blue = (unsigned char)MAX(0.0f, MIN(255.0f, rgb.Blue * value));

	return(*this);
}

// FUNCTION: 0x006612C0
void RGBClass::Adjust(int ratio, RGBClass const & rgb)
{
	ratio &= 0x00FF;

	int value = (int)rgb.Red - (int)Red;
	Red = (unsigned char)((int)Red + (value * ratio) / 256);

	value = (int)rgb.Green - (int)Green;
	Green = (unsigned char)((int)Green + (value * ratio) / 256);

	value = (int)rgb.Blue - (int)Blue;
	Blue = (unsigned char)((int)Blue + (value * ratio) / 256);
}

// FUNCTION: 0x00661350
int RGBClass::Difference(RGBClass const & rgb) const
{
	int r = (int)Red - (int)rgb.Red;
	if (r < 0) r = -r;

	int g = (int)Green - (int)rgb.Green;
	if (g < 0) g = -g;

	int b = (int)Blue - (int)rgb.Blue;
	if (b < 0) b = -b;

	return(4*g + 3*b + 2*r);
}
