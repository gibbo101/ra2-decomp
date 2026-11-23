// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "geometry.h"
#include "mpobserver.h"
#include "mpsiege.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x005C91D0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005C91F0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005C9220 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005C9240 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005C9260 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005C9280 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005C92A0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005C92C0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005C9300 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005C9330 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005C9360 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005C9390 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005C93A0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005C93C0 SYMBOL
// _$E38

class HouseClass;

class MPRevealMap
{
	public:
		void FUN_00577F30(HouseClass * house);
};

extern MPRevealMap DAT_0087F7E8;
bool __stdcall FUN_005D8C90(int index);

// FUNCTION: GAMEMD 0x005C93E0
Megawealth::Megawealth(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
{
}

// FUNCTION: GAMEMD 0x005C9420
Megawealth::~Megawealth(void)
{
}

// FUNCTION: GAMEMD 0x005C9430
bool Megawealth::vt_entry_8C(void)
{
	DAT_0087F7E8.FUN_00577F30(NULL);
	return(true);
}

// SYNTHETIC: GAMEMD 0x005C9440
// ??_GMegawealth@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x005C9470
MultiplayerObserverTeam::MultiplayerObserverTeam(void) :
	MultiplayerTeam(StringTable::LoadString("MP:ObserverTeam", NULL, "D:\\ra2mdpost\\MPObserver.cpp", 5), -1)
{
}

// FUNCTION: GAMEMD 0x005C94C0
bool MultiplayerObserverTeam::IsTeamIncluded(int index)
{
	return(FUN_005D8C90(index));
}

// SYNTHETIC: GAMEMD 0x005C94D0
// ??_GMultiplayerObserverTeam@@UAEPAXI@Z
