// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "mpfreeforall.h"
#include "session.h"
#include <windows.h>
#undef LoadString
#include <math.h>

// SYNTHETIC: GAMEMD 0x005C5AD0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005C5AF0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005C5B20 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005C5B40 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005C5B60 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005C5B80 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005C5BA0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005C5BC0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005C5C00 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005C5C30 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005C5C60 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005C5C90 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005C5CA0 SYMBOL
// _$E35

int __fastcall FUN_00623840(HWND list);

// SYNTHETIC: GAMEMD 0x005C5CC0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x005C5CE0
MPFreeForAllClass::MPFreeForAllClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
{
}

// FUNCTION: GAMEMD 0x005C5D20
MPFreeForAllClass::~MPFreeForAllClass(void)
{
}

// FUNCTION: GAMEMD 0x005C5D30
bool MPFreeForAllClass::vt_entry_10(void)
{
	return(MultiplayerGameMode::vt_entry_10());
}

// FUNCTION: GAMEMD 0x005C5D40
bool MPFreeForAllClass::vt_entry_14(WideWstring * message)
{
	if (!MultiplayerGameMode::vt_entry_14(message)) {
		return(false);
	}
	NodeNameType * node;
	for (int i = 0; i < Session.Players.Count(); i++) {
		node = Session.Players[i];
		if (node->Slot.SpectatorFlag != -1) {
			Session.Players[i]->Slot.SpectatorFlag = i;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C5D90
bool MPFreeForAllClass::vt_entry_18(void)
{
	if (!MultiplayerGameMode::vt_entry_18()) {
		return(false);
	}
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (Session.Players[i]->Slot.SpectatorFlag != -1) {
			Session.Players[i]->Slot.SpectatorFlag = i;
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005C5DD0
void MPFreeForAllClass::PopulateTeamDropdownForPlayer(HWND hwnd, int index)
{
	FUN_00623840(hwnd);
	MultiplayerTeam * team = FUN_005D6620(index);
	if (team) {
		team->AddToList(hwnd);
	}
	team = FUN_005D6620(-1);
	if (team) {
		team->AddToList(hwnd);
	}
	SendMessage(hwnd, 0x14e, 0, 0);
}

// FUNCTION: GAMEMD 0x005C5E30
bool MPFreeForAllClass::vt_entry_40(void)
{
	return(true);
}

// SYNTHETIC: GAMEMD 0x005C5E40
// ??_GMPFreeForAllClass@@UAEPAXI@Z
