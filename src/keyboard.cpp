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

// FUNCTION: 0x0054EE30



DatInts DAT_00ABC6F8(0x80, 0x80, 0);

extern bool UserInputLocked;
struct ChronoScreenEffect { static bool Active(void); };
void __fastcall FUN_0053EC40(int key, bool release);
short __fastcall FUN_0053EC70(int vk);

// FUNCTION: 0x0054EE50
void WWKeyboardClass::Noop(void) const
{
}

// FUNCTION: 0x0054EE60
WWKeyboardClass::WWKeyboardClass(void) :
	MouseQX(0),
	MouseQY(0),
	MousePos(0,0),
	Head(0),
	Tail(0)
{
	memset(KeyState, '\0', sizeof(KeyState));
}

// FUNCTION: 0x0054EE90
unsigned short WWKeyboardClass::Buff_Get(void)
{
	while (!Check()) {}

	unsigned short temp = Fetch_Element();
	if (Is_Mouse_Key(temp)) {
		MouseQX = Fetch_Element();
		MouseQY = Fetch_Element();
		MousePos = Point2D(MouseQX, MouseQY);
	} else if (Is_Wheel_Key(temp)) {
		MouseWheel = Fetch_Element();
	}
	return(temp);
}

// FUNCTION: 0x0054EFC0
bool WWKeyboardClass::Is_Mouse_Key(unsigned short key)
{
	key &= 0xFF;
	return(key == VK_LBUTTON || key == VK_MBUTTON || key == VK_RBUTTON);
}

// FUNCTION: 0x0054EFF0
bool WWKeyboardClass::Is_Wheel_Key(unsigned short key)
{
	key &= 0xFF;
	return(key == 0xA);
}

// FUNCTION: 0x0054F000
unsigned short WWKeyboardClass::Check(void) const
{
	((WWKeyboardClass *)this)->Fill_Buffer_From_System();
	if (Is_Buffer_Empty()) return(false);
	return(Peek_Element());
}

// FUNCTION: 0x0054F050
unsigned short WWKeyboardClass::Get(void)
{
	while (!Check()) {}
	return(Buff_Get());
}

// FUNCTION: 0x0054F1C0
bool WWKeyboardClass::Put(unsigned short key)
{
	if (!Is_Buffer_Full()) {
		Put_Element(key);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0054F200
bool WWKeyboardClass::Put_Key_Message(unsigned short vk_key, bool release)
{
	if (!ChronoScreenEffect::Active()) {
		if (!UserInputLocked || vk_key == VK_ESCAPE) {
			FUN_0053EC40(vk_key, release);

			if (!Is_Mouse_Key(vk_key) && !Is_Wheel_Key(vk_key)) {
				if (FUN_0053EC70(VK_SHIFT) & 0x8000) {
					vk_key |= WWKEY_SHIFT_BIT;
				}
				if (FUN_0053EC70(VK_CONTROL) & 0x8000) {
					vk_key |= WWKEY_CTRL_BIT;
				}
				if (FUN_0053EC70(VK_MENU) & 0x8000) {
					if (vk_key == VK_TAB) {
						return(true);
					}
					vk_key |= WWKEY_ALT_BIT;
				}
			}

			if (release) {
				vk_key |= WWKEY_RLS_BIT;
			}
			return(Put(vk_key));
		}
	}
	return(true);
}

// FUNCTION: 0x0054F2F0
bool WWKeyboardClass::Put_Mouse_Message(unsigned short vk_key, int x, int y, bool release)
{
	if (UserInputLocked) {
		return(true);
	}
	if (Available_Buffer_Room() >= 3 && Is_Mouse_Key(vk_key)) {
		Put_Key_Message(vk_key, release);
		Put((unsigned short)x);
		Put((unsigned short)y);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0054F3D0
bool WWKeyboardClass::Put_Wheel_Message(int delta)
{
	if (UserInputLocked) {
		return(true);
	}
	if (Available_Buffer_Room() >= 2) {
		Put_Key_Message(0xA, false);
		Put(delta & 0xFF);
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0054F450
char WWKeyboardClass::To_ASCII(unsigned short key)
{
	if (key & WWKEY_RLS_BIT) {
		return('\0');
	}
	if ((key & 0xFF) == VK_HOME) {
		return('\b');
	}
	if ((key & 0xFF) == VK_END) {
		return('\\');
	}

	if (key & WWKEY_SHIFT_BIT) {
		KeyState[VK_SHIFT] = 0x80;
	}
	if (key & WWKEY_CTRL_BIT) {
		KeyState[VK_CONTROL] = 0x80;
	}
	if (key & WWKEY_ALT_BIT) {
		KeyState[VK_MENU] = 0x80;
	}

	char buffer[10];
	int result;
	int scancode;

	scancode = MapVirtualKey(key, 0);
	result = ToAscii((UINT)key, (UINT)scancode, (PBYTE)KeyState, (LPWORD)buffer, (UINT)0);

	if (key & WWKEY_SHIFT_BIT) {
		KeyState[VK_SHIFT] = 0;
	}
	if (key & WWKEY_CTRL_BIT) {
		KeyState[VK_CONTROL] = 0;
	}
	if (key & WWKEY_ALT_BIT) {
		KeyState[VK_MENU] = 0;
	}

	if (result != 1) {
		return('\0');
	}
	return(buffer[0]);
}
