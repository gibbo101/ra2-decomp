// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "getcpu.h"
#include <cstdio>
#include <cstring>

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
