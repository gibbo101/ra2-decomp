// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "emember.h"
#include "airctype.h"
#include "crc.h"
#include "globals.h"
#include "infatype.h"
#include "techtype.h"
#include "unittype.h"
#include <cstdio>
#include <cmath>

// FUNCTION: 0x004C4EF0
EnlistedMemberClass::EnlistedMemberClass(char const * entry)
{
	char name[64];
	int quantity;

	Quantity = 0;
	Class = NULL;

	sscanf(entry, "%d,%s", &quantity, name);

	Quantity = quantity;

	int type = InfantryTypeClass::From_Name(name);
	if (type != INFANTRY_NONE) {
		Class = InfantryTypes[type];
		return;
	}

	type = UnitTypeClass::From_Name(name);
	if (type != UNIT_NONE) {
		Class = UnitTypes[type];
		return;
	}

	type = AircraftTypeClass::From_Name(name);
	if (type != AIRCRAFT_NONE) {
		Class = AircraftTypes[type];
	}
}

// FUNCTION: 0x004C4F90
char const * EnlistedMemberClass::Build_INI_Entry(void) const
{
	static char _buf[32];

	sprintf(_buf, "%d,%s", Quantity, (char const *)Class->IniName);
	return(_buf);
}

// FUNCTION: 0x004C4FC0
void EnlistedMemberClass::Compute_CRC(CRCEngine & crc) const
{
	crc(Quantity);
	if (Class != NULL) {
		crc((RTTIType)Class->RTTI);
		crc(Class->Fetch_ID());
	}
}

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007E8728, DAT_007E8730;
extern double DAT_008A3820, DAT_008A3840, DAT_008A3848, DAT_008A3850, DAT_008A3868;
extern double DAT_008A3888, DAT_008A3890, DAT_008A38A8;
extern int DAT_008A38B0, DAT_008A38A4, DAT_008A38A0, DAT_008A3898, DAT_008A389C;
extern int DAT_008A3858, DAT_008A385C, DAT_008A3860;
extern int DAT_008A3828, DAT_008A382C, DAT_008A3830, DAT_008A3834;
extern short DAT_008A3838, DAT_008A383A;

// FUNCTION: 0x004C5000
void Init_004C5000(void)
{
	DAT_008A3840 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x004C5030
void Init_004C5030(void)
{
	DAT_008A38A8 = DAT_008A3840 / DAT_007E8728;
}

// FUNCTION: 0x004C5050
void Init_004C5050(void)
{
	DAT_008A3890 = DAT_007E1718 / DAT_008A38A8;
}

// FUNCTION: 0x004C5070
void Init_004C5070(void)
{
	DAT_008A3888 = DAT_007E1720 * DAT_007E8730;
}

// FUNCTION: 0x004C5090
void Init_004C5090(void)
{
	DAT_008A3848 = DAT_007E1728 * DAT_007E8730;
}

// FUNCTION: 0x004C50B0
void Init_004C50B0(void)
{
	DAT_008A3820 = DAT_007E1730 * DAT_007E8730;
}

// FUNCTION: 0x004C50D0
void Init_004C50D0(void)
{
	DAT_008A38B0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_008A3820 - DAT_008A3848) * DAT_008A3840));
}

// FUNCTION: 0x004C5110
void Init_004C5110(void)
{
	DAT_008A3850 = FUN_004CADE0(DAT_008A38B0 * DAT_007E1740);
}

// FUNCTION: 0x004C5140
void Init_004C5140(void)
{
	DAT_008A3868 = FUN_004CADE0((DAT_008A38B0 * 2) / DAT_008A3840);
}

// FUNCTION: 0x004C5170
void Init_004C5170(void)
{
	DAT_008A38A4 = (int)((DAT_008A38B0 * 4) + DAT_007E1738);
}

// FUNCTION: 0x004C51A0
void Init_004C51A0(void)
{
	DAT_008A3838 = 0;
	DAT_008A383A = 0;
}

// FUNCTION: 0x004C51B0
void Init_004C51B0(void)
{
	DAT_008A3858 = 0;
	DAT_008A385C = 0;
	DAT_008A3860 = 0;
}

// FUNCTION: 0x004C51D0
void Init_004C51D0(void)
{
	DAT_008A38A0 = 0;
	DAT_008A3898 = 0x80;
	DAT_008A389C = 0x80;
}

// FUNCTION: 0x004C51F0
void Init_004C51F0(void)
{
	DAT_008A3828 = 0;
	DAT_008A382C = 0;
	DAT_008A3830 = 0;
	DAT_008A3834 = 0;
}
