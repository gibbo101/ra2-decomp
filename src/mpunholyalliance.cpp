// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"
#include "geometry.h"
#include "mpunholyalliance.h"
#include "session.h"
#include "house.h"
#include "unit.h"
#include "rules.h"
#include "_rules.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>

// SYNTHETIC: GAMEMD 0x005CB190 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005CB1C0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005CB1E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005CB200 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005CB220 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005CB240 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005CB260 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005CB2A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005CB2D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005CB300 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005CB330 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005CB340 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005CB360 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005CB380 SYMBOL
// _$E41

int __fastcall FUN_00688ED0(ObjectClass * object, Cell const & cell, int flag);

static inline Cell Base_Spot(Cell const & base)
{
	return(Cell(base.X + 3, base.Y + 3));
}

// FUNCTION: GAMEMD 0x005CB3A0
MPUnholyAllianceClass::MPUnholyAllianceClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MultiplayerGameMode(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
{
}

// FUNCTION: GAMEMD 0x005CB3E0
MPUnholyAllianceClass::~MPUnholyAllianceClass(void)
{
}

// FUNCTION: GAMEMD 0x005CB3F0
bool MPUnholyAllianceClass::vt_entry_10(void)
{
	Session.Options.Bases = true;
	return(true);
}

// FUNCTION: GAMEMD 0x005CB400
bool MPUnholyAllianceClass::vt_entry_14(unsigned a)
{
	return(Session.Options.Bases && MultiplayerGameMode::vt_entry_14(a));
}

// FUNCTION: GAMEMD 0x005CB430
bool MPUnholyAllianceClass::vt_entry_18(void)
{
	Session.Options.Bases = true;
	return(true);
}

// FUNCTION: GAMEMD 0x005CB440
bool MPUnholyAllianceClass::SpawnBaseUnits(HouseClass * house, unsigned unused)
{
	bool failed = false;
	for (int i = Rule->BaseUnit.Count() - 1; i >= 0; i--) {
		UnitTypeClass const * type = Rule->BaseUnit[i];
		if (!type) {
			failed = true;
			continue;
		}
		UnitClass * unit = new UnitClass(type, house);
		if (!unit) {
			failed = true;
			continue;
		}
		Cell spot = Base_Spot(house->Get_Base_Center());
		if (i == 0) {
			if (unit->Unlimbo(Coord(spot, i), (Dir256)i)) {
				continue;
			}
		}
		if (!FUN_00688ED0(unit, house->Get_Base_Center(), 3)) {
			failed = true;
			delete unit;
		}
	}
	return(!failed);
}

// SYNTHETIC: GAMEMD 0x005CB540
// ??_GMPUnholyAllianceClass@@UAEPAXI@Z
