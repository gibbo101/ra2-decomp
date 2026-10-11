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
