// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "hsv.h"
#include "rgb.h"

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
