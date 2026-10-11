// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "scenario.h"
#include "_bench.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_palette.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_timer.h"
#include "_tooltip.h"
#include "_wsproto.h"
#include "addon.h"
#include "aircraft.h"
#include "aitrig.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "campaign.h"
#include "ccrand.h"
#include "cctooltip.h"
#include "cd.h"
#include "cell.h"
#include "conquer.h"
#include "convert.h"
#include "crc.h"
#include "data.h"
#include "dbgprint.h"
#include "egos.h"
#include "empulse.h"
#include "enviro.h"
#include "getcpu.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "inline.h"
#include "ion.h"
#include "isotype.h"
#include "language\language.h"
#include "light.h"
#include "logic.h"
#include "mainopt.h"
#include "mapgen.h"
#include "mapsel.h"
#include "misc.h"
#include "mouse.h"
#include "movie.h"
#include "mpu.h"
#include "msgbox.h"
#include "netdlg.h"
#include "newmenu.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "preview.h"
#include "progress.h"
#include "psystype.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "score.h"
#include "script.h"
#include "session.h"
#include "smudge.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "teamtype.h"
#include "terrain.h"
#include "theme.h"
#include "tiberium.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "vox.h"
#include "wave.h"
#include "waypoint.h"
#include "wincomm.h"
#include "wonline.h"
#include "wsproto.h"
#include "bench.hh"

// FUNCTION: 0x0068B8A0
int Adjust_To_CPU_Timing(int time)
{
	static const double P_SIX_TWEAK = .8;
	static const double TIME_SCALE = 200;

	int cpu_type;
	bool mmx;

	int speed = 0;

	Get_CPU_Type(cpu_type, mmx, NULL, 0);

	if (cpu_type > PROC_PENTIUM_PRO) {
		time = (int)(time * P_SIX_TWEAK);
	}

	double hiscale = ((__int64)2 << 31);

	unsigned int lo;
	unsigned int hi;
	lo = Get_CPU_Rate(hi);

	if (lo != 0 || hi != 0) {
		speed = (int)(((double)lo + ((double)hi * hiscale)) / (double)1000000);
	}

	time = (int)(time * TIME_SCALE / speed);
	return(time);
}

// FUNCTION: 0x0068BCB0
int ScenarioClass::Get_Unique_ID(void)
{
	UniqueID++;

	return(UniqueID);
}
