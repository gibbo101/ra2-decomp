// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "cell.h"
#include "bullettype.h"
#include "weapon.h"
#include "warhead.h"
#include "house.h"
#include "overtype.h"
#include "globals.h"
#include "_rules.h"
#include "rules.h"
#include "trajhelp.h"
#include "_map.h"
#include "inline.h"

#include <math.h>

// SYNTHETIC: GAMEMD 0x004CBEF0 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x004CBF10 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x004CBF40 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x004CBF60 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x004CBF80 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x004CBFA0 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x004CBFC0 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x004CBFE0 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x004CC020 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x004CC050 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x004CC080 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x004CC0B0 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x004CC0C0 SYMBOL
// _$E38

// SYNTHETIC: GAMEMD 0x004CC0E0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004CC100
CellClass * TrajectoryHelper::FindFirstObstacle(Coord const & source, Coord const & target, BulletTypeClass const * type, HouseClass const * owner)
{
	if (type->SubjectToCliffs || type->SubjectToWalls) {
		Cell scell = source.As_Cell();
		Cell tcell = target.As_Cell();
		int dx = abs(scell.X - tcell.X);
		int dy = abs(scell.Y - tcell.Y);
		int steps = dx;
		if (dx <= dy) {
			steps = dy;
		}
		Coord delta = target - source;
		Coord step;
		if (steps == 0) {
			step = Coord(0, 0, 0);
		} else {
			step = Coord(delta.X / steps, delta.Y / steps, delta.Z / steps);
		}
		CellClass * start = &Map[scell];
		CellClass * end = &Map[tcell];
		CellClass * prev = start;
		for (int i = 0; i < steps; i++) {
			Coord offset = step * i;
			Coord current = source + offset;
			CellClass * obstacle = GetObstacle(start, end, prev, current, type, owner);
			if (obstacle != NULL) {
				return(obstacle);
			}
			prev = &Map[current.As_Cell()];
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x004CC310
CellClass * TrajectoryHelper::FindFirstImpenetrableObstacle(Coord const & source, Coord const & target, WeaponTypeClass const * weapon, HouseClass const * owner)
{
	BulletTypeClass const * bullet = weapon->Bullet;
	CellClass * cell = FindFirstObstacle(source, target, bullet, owner);
	if (cell != NULL && cell->Has_Wall_Or_Gate() && weapon->WarheadPtr->IsWallDestroyer) {
		return(NULL);
	}
	return(cell);
}

// FUNCTION: GAMEMD 0x004CC680
bool TrajectoryHelper::IsCliffHit(CellClass const * source, CellClass const * before, CellClass const * after)
{
	return(after->GetLevel() - before->GetLevel() >= 4 && after->GetLevel() - source->GetLevel() > 0);
}

// FUNCTION: GAMEMD 0x004CC6D0
bool TrajectoryHelper::IsWallHit(CellClass const * source, CellClass const * check, CellClass const * target, HouseClass const * owner)
{
	bool iswall = check->Overlay != OVERLAY_NONE && OverlayTypes[check->Overlay]->IsWall;
	if (check != target && iswall && source->Height <= target->Height) {
		if (Rule->AlliedWallTransparency && Houses[check->Owner] != NULL && Houses[check->Owner]->Is_Ally(owner)) {
			return(false);
		}
		return(true);
	}
	return(false);
}
