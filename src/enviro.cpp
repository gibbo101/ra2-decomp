// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "enviro.h"
#include "globals.h"
#include "house.h"
#include "scenario.h"

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
