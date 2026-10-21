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

// SYNTHETIC: GAMEMD 0x004C60F0 SYMBOL
// _$E18
// SYNTHETIC: GAMEMD 0x004C6130 SYMBOL
// _$E16
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

const double ISO_TILE_SIZE = 60.0;
const double ONE_RAD = M_PI / 180.0;

// SYNTHETIC: GAMEMD 0x004C6360 SYMBOL
// _$E21
const double CELL_LEPTON_DIAG = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);
// SYNTHETIC: GAMEMD 0x004C6390 SYMBOL
// _$E24
const double LEPTONS_PER_PIXEL = CELL_LEPTON_DIAG / ISO_TILE_SIZE;
// SYNTHETIC: GAMEMD 0x004C63B0 SYMBOL
// _$E27
const double PIXELS_PER_LEPTON = 1.0 / LEPTONS_PER_PIXEL;
// SYNTHETIC: GAMEMD 0x004C63D0 SYMBOL
// _$E30
const double RAD_45 = ONE_RAD * 45;
// SYNTHETIC: GAMEMD 0x004C63F0 SYMBOL
// _$E33
const double RAD_60 = ONE_RAD * 60;
// SYNTHETIC: GAMEMD 0x004C6410 SYMBOL
// _$E36
const double RAD_90 = ONE_RAD * 90;
// SYNTHETIC: GAMEMD 0x004C6430 SYMBOL
// _$E39
const int LEVEL_LEPTON_H = (int)(fastmath::tan(RAD_90 - RAD_60) * CELL_LEPTON_DIAG * 0.5);

// SYNTHETIC: GAMEMD 0x004C6470 SYMBOL
// _$E42
const double CELL_SLOPE_ANGLE = fastmath::atan(LEVEL_LEPTON_H * (1 / (double)CELL_LEPTON_H));
// SYNTHETIC: GAMEMD 0x004C64A0 SYMBOL
// _$E45
const double CELL_DIAG_SLOPE_ANGLE = fastmath::atan((LEVEL_LEPTON_H * 2) / CELL_LEPTON_DIAG);
// SYNTHETIC: GAMEMD 0x004C64D0 SYMBOL
// _$E48
const int BRIDGE_LEPTON_HEIGHT = (int)((LEVEL_LEPTON_H * BRIDGE_CELL_HEIGHT) + 0.5);
// SYNTHETIC: GAMEMD 0x004C6500 SYMBOL
// _$E51
const Cell CELL_NONE(0, 0);
// SYNTHETIC: GAMEMD 0x004C6510 SYMBOL
// _$E54
const Coord COORD_NONE(0, 0, 0);
// SYNTHETIC: GAMEMD 0x004C6530 SYMBOL
// _$E57
const Coord CELL_CENTER_COORD(CELL_LEPTON_W / 2, CELL_LEPTON_H / 2, 0);

// SYNTHETIC: GAMEMD 0x004C6550 SYMBOL
// _$E60
const Rect RECT_NONE_2(0, 0, 0, 0);
