// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mstimer.h"
#include "win.h"

// FUNCTION: 0x005D5870
MillisecondSystemTimerClass::MillisecondSystemTimerClass(void)
{
	timeBeginPeriod(1);
}

// FUNCTION: 0x005D5880
MillisecondSystemTimerClass::~MillisecondSystemTimerClass(void)
{
	timeEndPeriod(1);
}

// FUNCTION: 0x005D5890 MillisecondSystemTimerClass::operator()
int MillisecondSystemTimerClass::operator () (void) const
{
	return(timeGetTime());
}

// FUNCTION: 0x005D58A0 ?BMillisecondSystemTimerClass
MillisecondSystemTimerClass::operator int (void) const
{
	return(timeGetTime());
}

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
extern const double DAT_007EED08;
extern const double DAT_007EED10;

// FUNCTION: 0x005D58B0 $E2
double DAT_00ABFD58 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005D58E0 $E5
double DAT_00ABFDC8 = DAT_00ABFD58 / DAT_007EED08;

// FUNCTION: 0x005D5900 $E8
double DAT_00ABFD98 = DAT_007E1718 / DAT_00ABFDC8;

// FUNCTION: 0x005D5920 $E11
double DAT_00ABFD90 = DAT_007E1720 * DAT_007EED10;

// FUNCTION: 0x005D5940 $E14
double DAT_00ABFD60 = DAT_007E1728 * DAT_007EED10;

// FUNCTION: 0x005D5960 $E17
double DAT_00ABFD38 = DAT_007E1730 * DAT_007EED10;

// FUNCTION: 0x005D5980 $E20
int DAT_00ABFDD0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABFD38 - DAT_00ABFD60) * DAT_00ABFD58));

// FUNCTION: 0x005D59C0 $E23
double DAT_00ABFD68 = FUN_004CADE0(DAT_00ABFDD0 * DAT_007E1740);

// FUNCTION: 0x005D59F0 $E26
double DAT_00ABFD88 = FUN_004CADE0((DAT_00ABFDD0 * 2) / DAT_00ABFD58);

// FUNCTION: 0x005D5A20 $E29
int DAT_00ABFDC4 = (int)(DAT_00ABFDD0 * 4 + DAT_007E1738);

struct MsInit2 { short a; short b; MsInit2() : a(0), b(0) {} };
struct MsInit3 { int a; int b; int c; MsInit3() : a(0), b(0), c(0) {} };
struct MsInit4 { int a; int b; int c; int d; MsInit4() : a(0), b(0), c(0), d(0) {} };
struct MsInit128 { int a; int b; int c; MsInit128() : c(0), a(0x80), b(0x80) {} };

// FUNCTION: 0x005D5A50 $E32
MsInit2 DAT_00ABFD50;

// FUNCTION: 0x005D5A60 $E35
MsInit3 DAT_00ABFD78;

// FUNCTION: 0x005D5A80 $E38
MsInit128 DAT_00ABFDB8;

// FUNCTION: 0x005D5AA0 $E41
MsInit4 DAT_00ABFD40;
