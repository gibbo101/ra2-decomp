// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "hsv.h"
#include "rgb.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007EAEE8;

// FUNCTION: 0x00517360
void HSVClass::Adjust(int ratio, HSVClass const & hsv)
{
	ratio &= 0x00FF;

	int value = hsv.Get_Value() - Get_Value();
	Value = (unsigned char)(Get_Value() + (value * ratio) / 256);

	int saturation = hsv.Get_Saturation() - Get_Saturation();
	Saturation = (unsigned char)(Get_Saturation() + (saturation * ratio) / 256);

	int hue = hsv.Get_Hue() - Get_Hue();
	Hue = (unsigned char)(Get_Hue() + (hue * ratio) / 256);
}

// FUNCTION: 0x005173F0
int HSVClass::Difference(HSVClass const & hsv) const
{
	int hue = (int)Hue - (int)hsv.Hue;
	if (hue < 0) hue = -hue;

	int saturation = (int)Saturation - (int)hsv.Saturation;
	if (saturation < 0) saturation = -saturation;

	int value = (int)Value - (int)hsv.Value;
	if (value < 0) value = -value;

	return(hue*hue + saturation*saturation + value*value);
}

// FUNCTION: 0x00517440 ?BHSVClass
HSVClass::operator RGBClass (void) const
{
	unsigned int i;
	unsigned int f;
	unsigned int tmp;
	unsigned int values[7];

	int hue = Get_Hue();
	int saturation = Get_Saturation();
	int value = Get_Value();
	int red, green, blue;

	hue *= 6;
	f = hue % 255;

	values[1] =
	values[2] = value;

	tmp = (saturation * f) / 255;
	values[3] = (value * (255 - tmp)) / 255;

	values[4] =
	values[5] = (value * (255 - saturation)) / 255;

	tmp = 255 - (saturation * (255 - f)) / 255;
	values[6] = (value * tmp) / 255;

	i = hue / 255;

	i += (i > 4) ? -4 : 2;
	red = values[i];

	i += (i > 4) ? -4 : 2;
	blue = values[i];

	i += (i > 4) ? -4 : 2;
	green = values[i];

	RGBClass rgb((unsigned char)red, (unsigned char)green, (unsigned char)blue);
	return(rgb);
}

// FUNCTION: 0x00517560
bool HSVClass::operator==(HSVClass & that)
{
	return(Hue == that.Hue && Saturation == that.Saturation && Value == that.Value);
}

// FUNCTION: 0x00517590
bool HSVClass::operator!=(const HSVClass & that)
{
	return(!(*this == const_cast<HSVClass &>(that)));
}

// FUNCTION: 0x005175D0
bool operator==(HSVClass const & a, HSVClass const & b)
{
	return(a == b);
}

// FUNCTION: 0x005175E0
bool operator!=(HSVClass const & a, HSVClass const & b)
{
	return(!(a == b));
}

// FUNCTION: 0x00517840 $E3
double DAT_00A8F1E8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00517870 $E6
double DAT_00A8F238 = DAT_00A8F1E8 / DAT_007EAEE8;
