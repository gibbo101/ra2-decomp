// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "getcpu.h"
#include <cstdio>
#include <cstring>
#include <math.h>

extern "C" {
	bool __cdecl Detect_MMX_Availability (void);
	bool __cdecl Detect_CMOV_Availability (void);

	extern char CPUType;
	extern char VendorID;
}

// FUNCTION: 0x004E6150
void Get_CPU_Type(int & cpu_type, bool & mmx, char * vendor_id, int vendor_id_length)
{
	mmx = Detect_MMX_Availability();
	Detect_CMOV_Availability();

	cpu_type = (int)CPUType;

	if (vendor_id != NULL) {
		char * vendor_ptr = &VendorID;
		strncpy(vendor_id, vendor_ptr, vendor_id_length);
	}
}

extern char DAT_0084E864;
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
extern const double DAT_007E9B50;
extern const double DAT_007E9B58;

// FUNCTION: 0x004E6190
int FUN_004E6190(void)
{
	char vendor[64];

	Detect_MMX_Availability();
	Detect_CMOV_Availability();
	int cpu = (int)CPUType;
	if (vendor != NULL) {
		strncpy(vendor, &VendorID, 64);
	}
	if (cpu == 0 || DAT_0084E864 == 0) {
		return -1;
	}
	if (strcmpi(vendor, "AuthenticAMD") == 0) {
		return 7;
	}
	if (strcmpi(vendor, "CyrixInstead") == 0) {
		return 8;
	}
	switch (cpu) {
	case 4:
		return 0;
	case 5:
		return 1;
	case 6:
		switch (DAT_0084E864) {
		case 3:
		case 5:
			return 3;
		case 4:
			return 2;
		case 6:
			return 5;
		case 7:
		case 8:
		case 10:
			return 4;
		default:
			return 2;
		}
	}
	return 6;
}

// FUNCTION: 0x004E62B0
const char * FUN_004E62B0(void)
{
	static const char * const names[9] = {
		"486", "Pentium", "Pentium Pro", "Pentium II", "Pentium III",
		"Celeron", "Pentium 4", "AMD", "Cyrix"
	};
	int type = FUN_004E6190();
	if (type > -1 && type < 9) {
		return names[type];
	}
	return "Unknown";
}

// FUNCTION: 0x004E62D0 $E2
double DAT_00A83D40 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004E6300 $E5
double DAT_00A8EC98 = DAT_00A83D40 / DAT_007E9B50;

// FUNCTION: 0x004E6320 $E8
double DAT_00A8EB58 = DAT_007E1718 / DAT_00A8EC98;

// FUNCTION: 0x004E6340 $E11
double DAT_00A8EB20 = DAT_007E1720 * DAT_007E9B58;

// FUNCTION: 0x004E6360 $E14
double DAT_00A83DB0 = DAT_007E1728 * DAT_007E9B58;

// FUNCTION: 0x004E6380 $E17
double DAT_008B4168 = DAT_007E1730 * DAT_007E9B58;

// FUNCTION: 0x004E63A0 $E20
int DAT_00A8ED58 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_008B4168 - DAT_00A83DB0) * DAT_00A83D40));

// FUNCTION: 0x004E63E0 $E23
double DAT_00A83DE0 = FUN_004CADE0(DAT_00A8ED58 * DAT_007E1740);

// FUNCTION: 0x004E6410 $E26
double DAT_00A8E958 = FUN_004CADE0((DAT_00A8ED58 * 2) / DAT_00A83D40);

// FUNCTION: 0x004E6440 $E29
int DAT_00A8EC94 = (int)(DAT_00A8ED58 * 4 + DAT_007E1738);

struct Short2 { short a; short b; Short2() : a(0), b(0) {} };
struct Int3 { int a; int b; int c; Int3() : a(0), b(0), c(0) {} };
struct Int3B { int a; int b; int c; Int3B() : a(128), b(128), c(0) {} };
struct Int4 { int a; int b; int c; int d; Int4() : a(0), b(0), c(0), d(0) {} };

// FUNCTION: 0x004E6470 $E32
Short2 DAT_00A83CD0;

// FUNCTION: 0x004E6480 $E35
Int3 DAT_00A8E380;

// FUNCTION: 0x004E64A0 $E38
Int3B DAT_00A8EC68;

// FUNCTION: 0x004E64C0 $E41
Int4 DAT_00A83C58;
