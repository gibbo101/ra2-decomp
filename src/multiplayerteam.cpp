// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "multiplayer.h"
#include "session.h"
#include <windows.h>
#undef LoadString
#include <math.h>

// SYNTHETIC: GAMEMD 0x005D8A40 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005D8A60 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005D8A90 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005D8AB0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005D8AD0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005D8AF0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005D8B10 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005D8B30 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005D8B70 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005D8BA0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005D8BD0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005D8C00 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005D8C10 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x005D8C30 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x005D8C50
MultiplayerTeam::MultiplayerTeam(WideWstring const & title, int index) :
	Title(title),
	Index(index)
{
}

// FUNCTION: GAMEMD 0x005D8C80
MultiplayerTeam::~MultiplayerTeam(void)
{
}

// FUNCTION: GAMEMD 0x005D8C90
bool MultiplayerTeam::IsTeamIncluded(int index)
{
	if (index >= 0 && index < Session.Players.Count()) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x005D8CB0
bool MultiplayerTeam::SetPlayerTeam(int player)
{
	if (!IsTeamIncluded(player)) {
		return false;
	}
	Session.Players[player]->Slot.SpectatorFlag = Index;
	return true;
}

// FUNCTION: GAMEMD 0x005D8CE0
int MultiplayerTeam::CountMembers(void)
{
	int count = 0;
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (Session.Players[i]->Slot.SpectatorFlag == Index) {
			count++;
		}
	}
	return count;
}

// FUNCTION: GAMEMD 0x005D8D10
int MultiplayerTeam::AddToList(HWND list)
{
	int item = SendMessage(list, CB_ADDSTRING, 0, (LPARAM)Title.text());
	SendMessage(list, CB_SETITEMDATA, item, Index);
	return item;
}

// SYNTHETIC: GAMEMD 0x005D8D50
// ??_GMultiplayerTeam@@UAEPAXI@Z
