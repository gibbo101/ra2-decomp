// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mech.h"
#include "_map.h"
#include "_rules.h"
#include "assert.h"
#include "cell.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "inline.h"
#include "noinit.h"
#include "overtype.h"
#include "rules.h"
#include "tactical.h"
#include "tube.h"
#include "layer.hh"
#include <new.h>

// FUNCTION: 0x005B0060
boolean STDMETHODCALLTYPE MechLocomotionClass::Process(void)
{
	Movement_AI(true);
	return(Is_Moving());
}
