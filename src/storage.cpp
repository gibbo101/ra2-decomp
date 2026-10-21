// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "storage.h"
#include "tiberium.h"
#include "vector.h"

// FUNCTION: 0x006C95E0
StorageClass::StorageClass(void)
{
	for (int i = 0; i < ARRAY_SIZE(Values); i++) {
		Values[i] = 0;
	}
}

// FUNCTION: 0x006C9600
int StorageClass::Get_Total_Value(void) const
{
	int value = 0;
	for (int i = 0; i < ARRAY_SIZE(Values); i++) {
		if (Values[i] > 0) {
			value = (int)(Values[i] * Tiberiums[i]->CreditValue + value);
		}
	}

	return(value);
}

// FUNCTION: 0x006C9650
float StorageClass::Get_Total_Amount(void) const
{
	int amount = 0;
	for (int i = 0; i < ARRAY_SIZE(Values); i++) {
		amount = (int)(amount + Values[i]);
	}

	return(amount);
}

// FUNCTION: 0x006C9680
float StorageClass::Get_Amount(int slot) const
{
	return(Values[slot]);
}

// FUNCTION: 0x006C9690
float StorageClass::Increase_Amount(float by, int slot)
{
	Values[slot] += by;
	return(Values[slot]);
}

// FUNCTION: 0x006C96B0
float StorageClass::Decrease_Amount(float by, int slot)
{
	float amount;
	if (Values[slot] < by) {
		amount = Values[slot];
	} else {
		amount = by;
	}
	Values[slot] -= amount;
	return(amount);
}
