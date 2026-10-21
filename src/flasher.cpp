// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "flasher.h"
#include "mono.h"
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E89C8;
extern const double DAT_007E89D0;

struct Short2 {
	short A;
	short B;
	Short2() : A(0), B(0) {}
};

struct Int3 {
	int A;
	int B;
	int C;
	Int3() : A(0), B(0), C(0) {}
};

// FUNCTION: 0x004CC770
bool FlasherClass::Process(void)
{
	if (FlashCount) {
		FlashCount--;
		IsBlushing = false;

		if (FlashCount & 0x01) {
			IsBlushing = true;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004CC790 $E4
double DAT_008B3C60 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004CC7C0 $E7
double DAT_008B3CB0 = DAT_008B3C60 / DAT_007E89C8;

// FUNCTION: 0x004CC7E0 $E10
double DAT_008B3C98 = DAT_007E1718 / DAT_008B3CB0;

// FUNCTION: 0x004CC800 $E13
double DAT_008B3C90 = DAT_007E1720 * DAT_007E89D0;

// FUNCTION: 0x004CC820 $E16
double DAT_008B3C68 = DAT_007E1728 * DAT_007E89D0;

// FUNCTION: 0x004CC840 $E19
double DAT_008B3C40 = DAT_007E1730 * DAT_007E89D0;

// FUNCTION: 0x004CC860 $E22
int DAT_008B3CB8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_008B3C40 - DAT_008B3C68) * DAT_008B3C60));

// FUNCTION: 0x004CC8A0 $E25
double DAT_008B3C70 = FUN_004CADE0(DAT_008B3CB8 * DAT_007E1740);

// FUNCTION: 0x004CC8D0 $E28
double DAT_008B3C88 = FUN_004CADE0((DAT_008B3CB8 * 2) / DAT_008B3C60);

// FUNCTION: 0x004CC900 $E31
int DAT_008B3CAC = (int)(DAT_008B3CB8 * 4 + DAT_007E1738);

// FUNCTION: 0x004CC930 $E34
Short2 DAT_008B3C58;

// FUNCTION: 0x004CC940 $E37
Int3 DAT_008B3C78;

struct Int3B {
	int A;
	int B;
	int C;
	Int3B() : A(128), B(128), C(0) {}
};

struct Int4 {
	int A;
	int B;
	int C;
	int D;
	Int4() : A(0), B(0), C(0), D(0) {}
};

// FUNCTION: 0x004CC960 $E40
Int3B DAT_008B3CA0;

// FUNCTION: 0x004CC980 $E43
Int4 DAT_008B3C48;
