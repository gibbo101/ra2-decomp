// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "noinit.h"

class StorageClass
{
	public:
		StorageClass(void);
		StorageClass(NoInitClass const & x) {}
		~StorageClass(void) {};

		int Get_Total_Value(void) const;
		float Get_Total_Amount(void) const;
		float Get_Amount(int slot) const;
		float Increase_Amount(float amount, int slot);
		float Decrease_Amount(float amount, int slot);
		int First_Used_Slot(void) const;

		StorageClass operator+(StorageClass &that) const;
		StorageClass operator+=(StorageClass &that);
		StorageClass operator-(StorageClass &that) const;
		StorageClass operator-=(StorageClass &that);

	private:

		float Values[4];
};
