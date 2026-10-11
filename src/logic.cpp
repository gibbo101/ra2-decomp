// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "logic.h"
#include "_bench.h"
#include "_map.h"
#include "_rules.h"
#include "_tactica.h"
#include "_vanim.h"
#include "aircraft.h"
#include "alphashp.h"
#include "anim.h"
#include "bench.h"
#include "blight.h"
#include "building.h"
#include "bullet.h"
#include "empulse.h"
#include "factory.h"
#include "globals.h"
#include "incdec.h"
#include "infantry.h"
#include "ion.h"
#include "ionblast.h"
#include "laser.h"
#include "light.h"
#include "mono.h"
#include "object.h"
#include "ovrlight.h"
#include "partsys.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "tactical.h"
#include "tag.h"
#include "terrain.h"
#include "tiberium.h"
#include "unit.h"
#include "vanim.h"
#include "vein.h"
#include "wave.h"
#include "bench.hh"

// FUNCTION: 0x0055B880
void LogicClass::Detach(AbstractClass const * target, bool all)
{
	if (Is_Target_Tag(target)) {
		for (int index = 0; index < LogicTags.Count(); index++) {
			if (target == (AbstractClass *)LogicTags[index]) {
				LogicTags.Delete_Index(index);
				index--;
			}
		}
	}
}
