// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "keyboard.h"
#include "_xmouse.h"
#include "msgloop.h"
#include <cmath>
#define	ARRAY_SIZE(x)		int(sizeof(x)/sizeof(x[0]))

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007ECEB8;
extern const double DAT_007ECEC0;

// FUNCTION: 0x0054EC60



double DAT_00ABC6B8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0054EC90



double DAT_00ABC708 = DAT_00ABC6B8 / DAT_007ECEB8;

// FUNCTION: 0x0054ECB0



double DAT_00ABC6F0 = DAT_007E1718 / DAT_00ABC708;

// FUNCTION: 0x0054ECD0



double DAT_00ABC6E8 = DAT_007E1720 * DAT_007ECEC0;

// FUNCTION: 0x0054ECF0



double DAT_00ABC6C0 = DAT_007E1728 * DAT_007ECEC0;

// FUNCTION: 0x0054ED10



double DAT_00ABC698 = DAT_007E1730 * DAT_007ECEC0;

// FUNCTION: 0x0054ED30



int DAT_00ABC710 = (int)(DAT_007E1738 * (DAT_00ABC6B8 * fastmath::tan(DAT_00ABC698 - DAT_00ABC6C0)));

// FUNCTION: 0x0054ED70



double DAT_00ABC6C8 = fastmath::atan(DAT_00ABC710 * DAT_007E1740);

// FUNCTION: 0x0054EDA0



double DAT_00ABC6E0 = fastmath::atan((DAT_00ABC710 * 2) / DAT_00ABC6B8);

// FUNCTION: 0x0054EDD0



int DAT_00ABC704 = (int)((DAT_00ABC710 * 4) + DAT_007E1738);

struct DatShorts { short X; short Y; DatShorts(short x, short y) : X(x), Y(y) {} };
struct DatInts { int X; int Y; int Z; DatInts(int x, int y, int z) : X(x), Y(y), Z(z) {} };

// FUNCTION: 0x0054EE00



DatShorts DAT_00ABC6B0(0, 0);

// FUNCTION: 0x0054EE10



DatInts DAT_00ABC6D0(0, 0, 0);

// FUNCTION: 0x0054EFC0
bool WWKeyboardClass::Is_Mouse_Key(unsigned short key)
{
	key &= 0xFF;
	return(key == VK_LBUTTON || key == VK_MBUTTON || key == VK_RBUTTON);
}
