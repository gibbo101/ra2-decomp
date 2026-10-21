// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "utracker.h"

#include <math.h>
#include <winsock.h>

extern double __cdecl FUN_004CAC40(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007F62D8;
extern const double DAT_007F62E0;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
namespace fastmath { double __cdecl tan(double x); double __cdecl atan(double x); }

// FUNCTION: GAMEMD 0x00748FD0
UnitTrackerClass::UnitTrackerClass(void)
{
	UnitCount = 512;
	Clear_Unit_Total();
	InNetworkFormat = 0;
}

// FUNCTION: GAMEMD 0x00749010
UnitTrackerClass::~UnitTrackerClass(void)
{
}

// FUNCTION: GAMEMD 0x00749020
void UnitTrackerClass::Increment_Unit_Total(int unit_type)
{
	if (unit_type < UnitCount) {
		UnitTotals[unit_type]++;
	}
}

// FUNCTION: GAMEMD 0x00749040
void UnitTrackerClass::Decrement_Unit_Total(int unit_type)
{
	if (unit_type < UnitCount) {
		UnitTotals[unit_type]--;
	}
}

// FUNCTION: GAMEMD 0x00749060
void UnitTrackerClass::Populate_Unit_Count(int count)
{
	UnitCount = 512;
	Clear_Unit_Total();
	UnitCount = count;
}

// FUNCTION: GAMEMD 0x007490A0
int UnitTrackerClass::Get_Unit_Total(void)
{
	int total = 0;
	for (int i = 0; i < UnitCount; i++) {
		total += UnitTotals[i];
	}
	return total;
}

// FUNCTION: GAMEMD 0x007490C0
int * UnitTrackerClass::Get_All_Totals(void)
{
	return UnitTotals;
}

// FUNCTION: GAMEMD 0x007490D0
void UnitTrackerClass::Clear_Unit_Total(void)
{
	for (int i = 0; i < UnitCount; i++) {
		UnitTotals[i] = 0;
	}
}

// FUNCTION: GAMEMD 0x00749100
void UnitTrackerClass::To_Network_Format(void)
{
	if (!InNetworkFormat) {
		for (int i = 0; i < UnitCount; i++) {
			UnitTotals[i] = htonl(UnitTotals[i]);
		}
	}
	InNetworkFormat = 1;
}

// FUNCTION: GAMEMD 0x00749150
void UnitTrackerClass::To_PC_Format(void)
{
	if (InNetworkFormat) {
		for (int i = 0; i < UnitCount; i++) {
			UnitTotals[i] = ntohl(UnitTotals[i]);
		}
	}
	InNetworkFormat = 0;
}

// SYNTHETIC: GAMEMD 0x007491A0 SYMBOL
// _$E2
double DAT_00B1D170 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x007491D0 SYMBOL
// _$E5
double DAT_00B1D1C0 = DAT_00B1D170 / DAT_007F62D8;

// SYNTHETIC: GAMEMD 0x007491F0 SYMBOL
// _$E8
double DAT_00B1D1A8 = DAT_007E1718 / DAT_00B1D1C0;

// SYNTHETIC: GAMEMD 0x00749210 SYMBOL
// _$E11
double DAT_00B1D1A0 = DAT_007E1720 * DAT_007F62E0;

// SYNTHETIC: GAMEMD 0x00749230 SYMBOL
// _$E14
double DAT_00B1D178 = DAT_007E1728 * DAT_007F62E0;

// SYNTHETIC: GAMEMD 0x00749250 SYMBOL
// _$E17
double DAT_00B1D150 = DAT_007E1730 * DAT_007F62E0;

// SYNTHETIC: GAMEMD 0x00749270 SYMBOL
// _$E20
int DAT_00B1D1C8 = (int)(fastmath::tan(DAT_00B1D150 - DAT_00B1D178) * DAT_007E1738 * DAT_00B1D170);

// SYNTHETIC: GAMEMD 0x007492B0 SYMBOL
// _$E23
double DAT_00B1D180 = fastmath::atan(DAT_00B1D1C8 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x007492E0 SYMBOL
// _$E26
double DAT_00B1D198 = fastmath::atan((DAT_00B1D1C8 * 2) / DAT_00B1D170);

// SYNTHETIC: GAMEMD 0x00749310 SYMBOL
// _$E29
int DAT_00B1D1BC = (int)(DAT_00B1D1C8 * 4 + DAT_007E1738);

struct UtShortPair { short A; short B; UtShortPair(short a, short b) { A = a; B = b; } };
struct UtIntTriple { int A; int B; int C; UtIntTriple(int a, int b, int c) { A = a; B = b; C = c; } };
struct UtIntQuad { int A; int B; int C; int D; UtIntQuad(int a, int b, int c, int d) { A = a; B = b; C = c; D = d; } };

// SYNTHETIC: GAMEMD 0x00749340 SYMBOL
// _$E32
UtShortPair DAT_00B1D168(0, 0);

// SYNTHETIC: GAMEMD 0x00749350 SYMBOL
// _$E35
UtIntTriple DAT_00B1D188(0, 0, 0);

// SYNTHETIC: GAMEMD 0x00749370 SYMBOL
// _$E38
UtIntTriple DAT_00B1D1B0(128, 128, 0);

// SYNTHETIC: GAMEMD 0x00749390 SYMBOL
// _$E41
UtIntQuad DAT_00B1D158(0, 0, 0, 0);
