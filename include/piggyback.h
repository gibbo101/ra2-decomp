// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ipiggy.h"

// Save the old locomotor in the replacement before installing it on the object.
inline void Begin_Piggyback_Locomotion(
	ILocomotionPtr &locomotion,
	IPiggybackPtr &piggy,
	ILocomotionPtr &replacement)
{
	piggy->Begin_Piggyback(locomotion);
	locomotion = replacement;
}
