// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "coord.h"
#include <cmath>
#include "enviro.h"
#include "globals.h"
#include "house.h"
#include "scenario.h"

// SYNTHETIC: GAMEMD 0x004C5EE0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004C5F10 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004C5F30 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004C5F50 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004C5F70 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004C5F90 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004C5FB0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004C5FF0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004C6020 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004C6050 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004C6080 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004C6090 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004C60B0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004C60D0 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x004C60F0 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x004C6130 SYMBOL
// _$E41
EnvironmentClass Environment;

// FUNCTION: GAMEMD 0x004C6140
EnvironmentClass::EnvironmentClass(void):
	CarryOverMoney(0),
	MissionTimer(0),
	Difficulty(DIFF_NORMAL),
	Stage(0)
{
	for (int i = 0; i < 50; i++) {
		Globals[i] = false;
	}
}

// FUNCTION: GAMEMD 0x004C6170
EnvironmentClass::~EnvironmentClass(void)
{
}

// FUNCTION: GAMEMD 0x004C6180
void EnvironmentClass::Store(void)
{
	for (int i = 0; i < 50; i++) {
		Globals[i] = Scen->GlobalFlags[i].Value;
	}

	CarryOverMoney = PlayerPtr->Available_Money();
	MissionTimer = Scen->MissionTimer;
	Difficulty = PlayerPtr->Difficulty;
	Stage = Scen->Stage;
}

// FUNCTION: GAMEMD 0x004C6210
void EnvironmentClass::Restore(void)
{
	for (int i = 0; i < 50; i++) {
		Scen->Set_Global_To(i, Globals[i]);
	}

	int cap = Scen->CarryOverCap;
	double money = (double)CarryOverMoney * Scen->CarryOverPercent;

	if (cap != -1) {
		money = MIN(money, cap);
	}

	PlayerPtr->Refund_Money((int)money);
	PlayerPtr->Control.InitialCredits += (int)money;
	PlayerPtr->Assign_Handicap(Difficulty);

	if (Scen->IsInheritTimer) {
		if (MissionTimer > 0) {
			Scen->MissionTimer = MissionTimer;
			Scen->MissionTimer.Start();
		}
	}

	Scen->Stage = Stage;
}

// FUNCTION: GAMEMD 0x004C6320
HRESULT EnvironmentClass::Load(IStream * stream)
{
	return(stream->Read(this, sizeof(*this), NULL));
}

// FUNCTION: GAMEMD 0x004C6340
HRESULT EnvironmentClass::Save(IStream * stream)
{
	return(stream->Write(this, sizeof(*this), NULL));
}
