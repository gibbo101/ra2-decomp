// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "laser.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccrand.h"
#include "coord.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "inline.h"
#include "mouse.h"
#include "rgb.h"
#include "scenario.h"
#include "tactical.h"

// FUNCTION: 0x0054FFB0
LaserDrawClass::~LaserDrawClass(void)
{
	LaserDraws.Delete(this);
}

// FUNCTION: 0x00550000
void LaserDrawClass::All_Clear(void)
{
	while (LaserDraws.Count()) {
		delete LaserDraws[0];
	}
}

// FUNCTION: 0x00550240
void LaserDrawClass::Draw_All(void)
{
	for (int i = LaserDraws.Count() - 1; i >= 0; i--) {
		LaserDraws[i]->Draw_It();
	}
}
