// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

class NoInitClass;

class UnitTrackerClass
{
	public:
		UnitTrackerClass(void);
		UnitTrackerClass(NoInitClass const &) {}
		~UnitTrackerClass(void);

		void Increment_Unit_Total(int unit_type);
		void Decrement_Unit_Total(int unit_type);
		void Clear_Unit_Total(void);
		void Populate_Unit_Count(int count);

		int Get_Unit_Total(void);
		int * Get_All_Totals(void);
		int Get_Unit_Count(void) { return(UnitCount); };

		void To_Network_Format(void);
		void To_PC_Format(void);

	private:
		int UnitTotals[512];
		int UnitCount;
		int InNetworkFormat;
};
