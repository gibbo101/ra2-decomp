// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "always.h"
#include "mpunholyalliance.h"
#include "session.h"
#include "house.h"
#include "unit.h"
#include "rules.h"
#include "_rules.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>

int __fastcall FUN_00688ED0(ObjectClass * object, Cell const & cell, int flag);
extern int DAT_00ABFB88;
extern int DAT_00ABFB8C;
extern int DAT_00ABFB90;
extern int DAT_00ABFB94;

static inline Cell Base_Spot(Cell const & base)
{
	return(Cell(base.X + 3, base.Y + 3));
}

// FUNCTION: GAMEMD 0x005CB3A0
MPUnholyAllianceClass::MPUnholyAllianceClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MPGameModeClass(title, tooltip, inifile, mapfilter, aiallowed, modeindex)
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
	return(Session.Options.Bases && MPGameModeClass::vt_entry_14(a));
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

// FUNCTION: GAMEMD 0x005CB570
void Clear_Unholy_Stats(void)
{
	DAT_00ABFB88 = 0;
	DAT_00ABFB8C = 0;
	DAT_00ABFB90 = 0;
	DAT_00ABFB94 = 0;
}
