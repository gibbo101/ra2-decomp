// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "palette.h"
#include "pk.h"
#include "random.h"

// SYNTHETIC: GAMEMD 0x0040D8A0 SYMBOL
// _$E8
PaletteClass CCPalette;
// SYNTHETIC: GAMEMD 0x0040D8C0 SYMBOL
// _$E11
PaletteClass GamePalette;
// SYNTHETIC: GAMEMD 0x0040D8E0 SYMBOL
// _$E14
PaletteClass SchemePalette;
// SYNTHETIC: GAMEMD 0x0040D900 SYMBOL
// _$E17
PaletteClass BlackPalette(RGBClass(0, 0, 0));
// SYNTHETIC: GAMEMD 0x0040D920 SYMBOL
// _$E20
PaletteClass WhitePalette(RGBClass(RGBClass::MAX_VALUE, RGBClass::MAX_VALUE, RGBClass::MAX_VALUE));
// SYNTHETIC: GAMEMD 0x0040D940 SYMBOL
// _$E23
PaletteClass OriginalPalette;
// SYNTHETIC: GAMEMD 0x0040D960 SYMBOL
// _$E26
PaletteClass ScorePalette;
// SYNTHETIC: GAMEMD 0x0040D980 SYMBOL
// _$E29
PaletteClass VoxelPalette;
// SYNTHETIC: GAMEMD 0x0040D9A0 SYMBOL
// _$E32
PaletteClass WaypointPalette;


// SYNTHETIC: GAMEMD 0x0040D9C0 SYMBOL
// _$E35
PKey FastKey;

// SYNTHETIC: GAMEMD 0x0040D9F0 SYMBOL
// _$E38
namespace Randomizer { Random2Class Global; }
