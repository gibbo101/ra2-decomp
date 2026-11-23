// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "geometry.h"
#include "mpsiege.h"
#include "session.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x005CAC00 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005CAC20 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005CAC50 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005CAC70 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005CAC90 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005CACB0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005CACD0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005CACF0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005CAD30 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005CAD60 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005CAD90 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005CADC0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005CADD0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005CADF0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x005CAE10
MultiplayerSiegeDefenderTeam::MultiplayerSiegeDefenderTeam(void) :
	MultiplayerTeam(StringTable::LoadString("MP:DefenderTeam", NULL, "D:\\ra2mdpost\\MPSiegeTeam.cpp", 6), 1)
{
}

// FUNCTION: GAMEMD 0x005CAE60
MultiplayerSiegeDefenderTeam::~MultiplayerSiegeDefenderTeam(void)
{
}

// FUNCTION: GAMEMD 0x005CAE70
bool __stdcall Siege_Only_Spectators_Remain(int self)
{
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (self != i && Session.Players[i]->Slot.SpectatorFlag == 1) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005CAEB0
MultiplayerSiegeAttackerTeam::MultiplayerSiegeAttackerTeam(void) :
	MultiplayerTeam(StringTable::LoadString("MP:AttackerTeam", NULL, "D:\\ra2mdpost\\MPSiegeTeam.cpp", 22), 1)
{
}

// FUNCTION: GAMEMD 0x005CAF00
MultiplayerSiegeAttackerTeam::~MultiplayerSiegeAttackerTeam(void)
{
}

// SYNTHETIC: GAMEMD 0x005CAF10
// ??_GMultiplayerSiegeDefenderTeam@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x005CAF40
// ??_GMultiplayerSiegeAttackerTeam@@UAEPAXI@Z
