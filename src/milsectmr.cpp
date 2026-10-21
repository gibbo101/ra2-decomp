// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "milsectmr.h"

#include "dbgprint.h"
#include "getcpu.h"
#include "mpu.h"
#include "win.h"
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
extern const double DAT_007EDC60;
extern const double DAT_007EDC68;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x005B1BA0 SYMBOL
// _$E2
double DAT_00ABEE78 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005B1BD0 SYMBOL
// _$E5
double DAT_00ABEEC8 = DAT_00ABEE78 / DAT_007EDC60;

// SYNTHETIC: GAMEMD 0x005B1BF0 SYMBOL
// _$E8
double DAT_00ABEEB0 = DAT_007E1718 / DAT_00ABEEC8;

// SYNTHETIC: GAMEMD 0x005B1C10 SYMBOL
// _$E11
double DAT_00ABEEA8 = DAT_007E1720 * DAT_007EDC68;

// SYNTHETIC: GAMEMD 0x005B1C30 SYMBOL
// _$E14
double DAT_00ABEE80 = DAT_007E1728 * DAT_007EDC68;

// SYNTHETIC: GAMEMD 0x005B1C50 SYMBOL
// _$E17
double DAT_00ABEE58 = DAT_007E1730 * DAT_007EDC68;

// SYNTHETIC: GAMEMD 0x005B1C70 SYMBOL
// _$E20
int DAT_00ABEED0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABEE58 - DAT_00ABEE80) * DAT_00ABEE78));

// SYNTHETIC: GAMEMD 0x005B1CB0 SYMBOL
// _$E23
double DAT_00ABEE88 = FUN_004CADE0(DAT_00ABEED0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005B1CE0 SYMBOL
// _$E26
double DAT_00ABEEA0 = FUN_004CADE0((DAT_00ABEED0 * 2) / DAT_00ABEE78);

// SYNTHETIC: GAMEMD 0x005B1D10 SYMBOL
// _$E29
int DAT_00ABEEC4 = (int)(DAT_00ABEED0 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005B1D40 SYMBOL
// _$E32
ShortPair DAT_00ABEE70;

// SYNTHETIC: GAMEMD 0x005B1D50 SYMBOL
// _$E35
IntTriple DAT_00ABEE90;

struct IntTripleB {
	int A, B, C;
	IntTripleB() { C = 0; A = 128; B = 128; }
};

// SYNTHETIC: GAMEMD 0x005B1D70 SYMBOL
// _$E38
IntTripleB DAT_00ABEEB8;


// FUNCTION: GAMEMD 0x005B1D90
MillisecondTimerClass::MillisecondTimerClass(void)
{
	unsigned int high = 0;
	Frequency = 1.0;
	unsigned int low = Get_CPU_Rate(high);

	if (low == 0 && high == 0) {
		timeBeginPeriod(1);

	} else {
		double dl = low;
		double dh = high;

		DebugString("MillisecondTimerClass low = %d, high = %d\n");

		Frequency = ((double)((double)dh * 4.294967296E9 + (double)dl)) / 1000;
	}
}

// FUNCTION: GAMEMD 0x005B1E20
MillisecondTimerClass::~MillisecondTimerClass(void)
{
	if (Frequency != 1.0) {
		timeEndPeriod(1);
	}
}

// FUNCTION: GAMEMD 0x005B1E40
MillisecondTimerClass::operator double () const
{
	static int cpu_type = -1;
	static bool has_mmx = false;

	if (cpu_type == -1) {
		Get_CPU_Type(cpu_type, has_mmx, NULL, 0);
	}
	if (Frequency != 1.0 && cpu_type > 4) {
		unsigned int high;
		unsigned int low;

		low = Get_CPU_Clock(high);
		double dl = low;
		double dh = high;

		return(((double)((double)dh * 4.294967296E9 + (double)dl)) / Frequency);
	}

	return(timeGetTime());
}

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// SYNTHETIC: GAMEMD 0x005B1EE0 SYMBOL
// _$E41
IntQuad DAT_00ABEEE0;
