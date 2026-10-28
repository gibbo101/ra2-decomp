// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "mpmanbattle.h"

// FUNCTION: GAMEMD 0x005C6150
MPManBattleClass::MPManBattleClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
{
}

// FUNCTION: GAMEMD 0x005C6190
MPManBattleClass::~MPManBattleClass(void)
{
}

// SYNTHETIC: GAMEMD 0x005C61A0
// ??_GMPManBattleClass@@UAEPAXI@Z
