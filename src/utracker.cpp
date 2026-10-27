// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "utracker.h"

#include <math.h>
#include <winsock.h>

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
