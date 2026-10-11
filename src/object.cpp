// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "object.h"
#include "_logic.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_tactica.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "assert.h"
#include "building.h"
#include "builtype.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "foot.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "logic.h"
#include "map.h"
#include "mono.h"
#include "objtype.h"
#include "rules.h"
#include "session.h"
#include "shapeset.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "tracker.h"
#define	GRAVITY	1.4

// FUNCTION: 0x005F61A0
VoxelDataStruct::~VoxelDataStruct(void)
{
	delete VoxLib;
	VoxLib = NULL;
	delete MotLib;
	MotLib = NULL;
}

// FUNCTION: 0x005F61F0
bool ObjectClass::operator < (ObjectClass const & object) const
{
	return(Sort_Y() < object.Sort_Y());
}

// FUNCTION: 0x005F6220
bool ObjectClass::operator > (ObjectClass const & object) const
{
	return((Sort_Y() > object.Sort_Y()) ? true : false);
}
