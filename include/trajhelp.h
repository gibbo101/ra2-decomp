// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include "coord.h"

class BulletTypeClass;
class CellClass;
class HouseClass;
class WeaponTypeClass;

class TrajectoryHelper
{
	public:
		static int Horizontal_Range(Coord const & from, Coord const & to)
		{
			return(Point2D(from).Distance_To(Point2D(to)));
		}

		static CellClass * FindFirstObstacle(Coord const & source, Coord const & target, BulletTypeClass const * type, HouseClass const * owner);
		static CellClass * FindFirstImpenetrableObstacle(Coord const & source, Coord const & target, WeaponTypeClass const * weapon, HouseClass const * owner);
		static CellClass * GetObstacle(CellClass const * source, CellClass const * target, CellClass const * bullet, Coord current, BulletTypeClass const * type, HouseClass const * owner);
		static bool IsCliffHit(CellClass const * source, CellClass const * before, CellClass const * after);
		static bool IsWallHit(CellClass const * source, CellClass const * check, CellClass const * target, HouseClass const * owner);
};
