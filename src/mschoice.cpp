// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mschoice.h"
#include "_mixfile.h"
#include "addon.h"
#include "ccfile.h"
#include "ccini.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"

// FUNCTION: 0x005CF810
MapStage * MapChoice::Find_Stage_By_ID(unsigned short index)
{
	if (Stages.Count() <= index) {
		return(NULL);
	}
	return(Stages[index]);
}

// FUNCTION: 0x005CF830
short MapChoice::Get_Stage_ID(MapStage * stage)
{
	return(Stages.ID(stage));
}
