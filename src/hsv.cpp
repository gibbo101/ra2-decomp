// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "hsv.h"
#include "rgb.h"
#include <math.h>
#include <windows.h>
#include <string.h>
#include "wolopt.h"

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007EAEE8;

// FUNCTION: GAMEMD 0x00517360
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

// FUNCTION: GAMEMD 0x005173F0
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

// FUNCTION: GAMEMD 0x00517440
// ??BHSVClass@@QBE?AVRGBClass@@XZ
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

// FUNCTION: GAMEMD 0x00517560
bool HSVClass::operator==(HSVClass & that)
{
	return(Hue == that.Hue && Saturation == that.Saturation && Value == that.Value);
}

// FUNCTION: GAMEMD 0x00517590
bool HSVClass::operator!=(const HSVClass & that)
{
	return(!(*this == const_cast<HSVClass &>(that)));
}

// FUNCTION: GAMEMD 0x005175D0
bool operator==(HSVClass const & a, HSVClass const & b)
{
	return(a == b);
}

// FUNCTION: GAMEMD 0x005175E0
bool operator!=(HSVClass const & a, HSVClass const & b)
{
	return(!(a == b));
}

// FUNCTION: GAMEMD 0x005175F0
bool WOLOptionsClass::Load(void)
{
	DWORD size = REG_DWORD;
	IsValid = false;
	char path[128];
	strcpy(path, "SOFTWARE\\Westwood\\WOLAPI\\");
	DWORD type;
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS) {
		DWORD value = 0;
		if (RegQueryValueEx(key, "Options", 0, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
			if (type == REG_DWORD) {
				IsValid = true;
				Options = value;
			}
		}
		RegCloseKey(key);
	}
	return IsValid;
}

// FUNCTION: GAMEMD 0x005176A0
bool WOLOptionsClass::Is_Bit0_Clear(void)
{
	if (Options & 1) return false;
	return true;
}

// FUNCTION: GAMEMD 0x005176B0
bool WOLOptionsClass::Is_Bit1_Clear(void)
{
	if (Options & 2) return false;
	return true;
}

// FUNCTION: GAMEMD 0x005176C0
bool WOLOptionsClass::Is_Bit2_Clear(void)
{
	if (Options & 4) return false;
	return true;
}

// FUNCTION: GAMEMD 0x005176D0
bool WOLOptionsClass::Save(unsigned long value)
{
	bool ok = false;
	char path[128];
	strcpy(path, "SOFTWARE\\Westwood\\WOLAPI\\");
	HKEY key;
	DWORD disposition;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, path, 0, 0, 0, KEY_ALL_ACCESS, 0, &key, &disposition) != ERROR_SUCCESS) {
			return false;
		}
	}
	if (RegSetValueEx(key, "Options", 0, REG_DWORD, (CONST BYTE *)&value, sizeof(value)) == ERROR_SUCCESS) {
		ok = true;
	}
	RegCloseKey(key);

	Load();
	return ok;
}

// SYNTHETIC: GAMEMD 0x00517840 SYMBOL
// _$E3
double DAT_00A8F1E8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x00517870 SYMBOL
// _$E6
double DAT_00A8F238 = DAT_00A8F1E8 / DAT_007EAEE8;

extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EAEF0;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntTripleInit {
	int A, B, C;
	IntTripleInit() { A = 128; B = 128; C = 0; }
};

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x00517890 SYMBOL
// _$E9
double DAT_00A8F220 = DAT_007E1718 / DAT_00A8F238;

// SYNTHETIC: GAMEMD 0x005178B0 SYMBOL
// _$E12
double DAT_00A8F218 = DAT_007E1720 * DAT_007EAEF0;

// SYNTHETIC: GAMEMD 0x005178D0 SYMBOL
// _$E15
double DAT_00A8F1F0 = DAT_007E1728 * DAT_007EAEF0;

// SYNTHETIC: GAMEMD 0x005178F0 SYMBOL
// _$E18
double DAT_00A8F1C8 = DAT_007E1730 * DAT_007EAEF0;

// SYNTHETIC: GAMEMD 0x00517910 SYMBOL
// _$E21
int DAT_00A8F240 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00A8F1C8 - DAT_00A8F1F0) * DAT_00A8F1E8));

// SYNTHETIC: GAMEMD 0x00517950 SYMBOL
// _$E24
double DAT_00A8F1F8 = FUN_004CADE0(DAT_00A8F240 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00517980 SYMBOL
// _$E27
double DAT_00A8F210 = FUN_004CADE0((DAT_00A8F240 * 2) / DAT_00A8F1E8);

// SYNTHETIC: GAMEMD 0x005179B0 SYMBOL
// _$E30
int DAT_00A8F234 = (int)(DAT_00A8F240 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005179E0 SYMBOL
// _$E33
ShortPair DAT_00A8F1E0;

// SYNTHETIC: GAMEMD 0x005179F0 SYMBOL
// _$E36
IntTriple DAT_00A8F200;

// SYNTHETIC: GAMEMD 0x00517A10 SYMBOL
// _$E39
IntTripleInit DAT_00A8F228;

// SYNTHETIC: GAMEMD 0x00517A30 SYMBOL
// _$E42
IntQuad DAT_00A8F1D0;
