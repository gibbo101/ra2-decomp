// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rgb.h"
#include "hsv.h"
#include <objidl.h>

// FUNCTION: GAMEMD 0x00661020
RGBClass & RGBClass::Lerp(RGBClass const & a, RGBClass const & b, float t)
{
	Red = (unsigned char)MAX(0.0f, MIN(255.0f, a.Red * (1.0f - t) + b.Red * t));
	Green = (unsigned char)MAX(0.0f, MIN(255.0f, a.Green * (1.0f - t) + b.Green * t));
	Blue = (unsigned char)MAX(0.0f, MIN(255.0f, a.Blue * (1.0f - t) + b.Blue * t));

	return(*this);
}

// FUNCTION: GAMEMD 0x00661190
RGBClass & RGBClass::Set(RGBClass const & rgb, float value)
{
	Red = (unsigned char)MAX(0.0f, MIN(255.0f, rgb.Red * value));
	Green = (unsigned char)MAX(0.0f, MIN(255.0f, rgb.Green * value));
	Blue = (unsigned char)MAX(0.0f, MIN(255.0f, rgb.Blue * value));

	return(*this);
}

// FUNCTION: GAMEMD 0x006612C0
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

// FUNCTION: GAMEMD 0x00661350
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

// FUNCTION: GAMEMD 0x006613A0
RGBClass::operator HSVClass (void) const
{
	int hue;
	int saturation;
	int value;
	int red = Get_Red();
	int green = Get_Green();
	int blue = Get_Blue();
	hue = 0;
	value = (red > green) ? red : green;
	if (blue > value) value = blue;
	int white = (red < green) ? red : green;
	if (blue < white) white = blue;
	saturation = 0;
	if (value) {
		saturation = ((value - white) * 255) / value;
	}
	if (saturation != 0) {
		unsigned int tmp = value - white;
	 	unsigned int r1 = ((value - red) * 255) / tmp;
	 	unsigned int g1 = ((value - green) * 255) / tmp;
	 	unsigned int b1 = ((value - blue) * 255) / tmp;
		if (value == red) {
		 	if (white == green) {
				tmp = 5 * 256 + b1;
			} else {
				tmp = 1 * 256 - g1;
			}
		} else {
			if (value == green) {
			 	if (white == blue) {
					tmp = 1 * 256 + r1;
				} else {
					tmp = 3 * 256 - b1;
				}
			} else {
			 	if (white == red) {
					tmp = 3 * 256 + g1;
				} else {
					tmp = 5 * 256 - r1;
				}
			}
		}
		hue = tmp / 6;
	}
	HSVClass hsv((unsigned char)hue, (unsigned char)saturation, (unsigned char)value);
	return(hsv);
}

// FUNCTION: GAMEMD 0x006614F0
void RGBClass::Save(IStream * stream) const
{
	stream->Write(this, 3, NULL);
}

// FUNCTION: GAMEMD 0x00661510
void RGBClass::Load(IStream * stream)
{
	stream->Read(this, 3, NULL);
}
