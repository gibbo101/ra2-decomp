// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <math.h>
#include "coord.h"
#include "globals.h"

// SYNTHETIC: GAMEMD 0x0048E270 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x0048E290 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0048E2C0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0048E2E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0048E300 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0048E320 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0048E340 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0048E360 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0048E3A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0048E3D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0048E400 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0048E430 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0048E440 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0048E460 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x0048E480 SYMBOL
// _$E50
Coord const StoppingCoordAbs[5] = {
	Coord(CELL_LEPTON_W / 2,     CELL_LEPTON_H / 2, 0),
	Coord(CELL_LEPTON_W / 4,     CELL_LEPTON_H / 4, 0),
	Coord(3 * CELL_LEPTON_W / 4, CELL_LEPTON_H / 4, 0),
	Coord(CELL_LEPTON_W / 4,     3 * CELL_LEPTON_H / 4, 0),
	Coord(3 * CELL_LEPTON_W / 4, 3 * CELL_LEPTON_H / 4, 0)
};
