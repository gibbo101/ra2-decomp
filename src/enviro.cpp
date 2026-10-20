// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "coord.h"
#include <cmath>
#include "enviro.h"
#include "globals.h"
#include "house.h"
#include "scenario.h"

// FUNCTION: 0x004C60F0 $E18
// FUNCTION: 0x004C6130 $E16
EnvironmentClass Environment;

// FUNCTION: 0x004C6140
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

// FUNCTION: 0x004C6170
EnvironmentClass::~EnvironmentClass(void)
{
}

// FUNCTION: 0x004C6180
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

// FUNCTION: 0x004C6210
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

// FUNCTION: 0x004C6320
HRESULT EnvironmentClass::Load(IStream * stream)
{
	return(stream->Read(this, sizeof(*this), NULL));
}

// FUNCTION: 0x004C6340
HRESULT EnvironmentClass::Save(IStream * stream)
{
	return(stream->Write(this, sizeof(*this), NULL));
}

const double ISO_TILE_SIZE = 60.0;
const double ONE_RAD = M_PI / 180.0;

// FUNCTION: 0x004C6360 $E21
const double CELL_LEPTON_DIAG = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);
// FUNCTION: 0x004C6390 $E24
const double LEPTONS_PER_PIXEL = CELL_LEPTON_DIAG / ISO_TILE_SIZE;
// FUNCTION: 0x004C63B0 $E27
const double PIXELS_PER_LEPTON = 1.0 / LEPTONS_PER_PIXEL;
// FUNCTION: 0x004C63D0 $E30
const double RAD_45 = ONE_RAD * 45;
// FUNCTION: 0x004C63F0 $E33
const double RAD_60 = ONE_RAD * 60;
// FUNCTION: 0x004C6410 $E36
const double RAD_90 = ONE_RAD * 90;
// FUNCTION: 0x004C6430 $E39
const int LEVEL_LEPTON_H = (int)(fastmath::tan(RAD_90 - RAD_60) * CELL_LEPTON_DIAG * 0.5);
