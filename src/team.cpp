// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "team.h"

#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "aircraft.h"
#include "aitrig.h"
#include "anim.h"
#include "building.h"
#include "cell.h"
#include "crc.h"
#include "foot.h"
#include "globals.h"
#include "house.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "map.h"
#include "mission.h"
#include "mono.h"
#include "movie.h"
#include "rules.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "target.h"
#include "taskforc.h"
#include "teamtype.h"
#include "theme.h"
#include "tmission.h"
#include "tracker.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "voc.h"
#include "vox.h"

#include "target.hh"
#include "tube.hh"

// FUNCTION: 0x006EDEF0
void TeamClass::TMission_PLAY_MUSIC(TeamMissionClass * mission, bool)
{
	Theme.Queue_Song(mission->Data.Theme);
	IsNextMission = true;
}
