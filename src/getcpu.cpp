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

// FUNCTION: GAMEMD 0x004E6150
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

// FUNCTION: GAMEMD 0x004E6190
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

// FUNCTION: GAMEMD 0x004E62B0
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
