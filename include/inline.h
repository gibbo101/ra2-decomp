// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#pragma once

#include "ccrand.h"
#include "coord.h"
#include "facing.h"
#include "sun.h"

extern Cell const							AdjacentCell[FACING_COUNT];
extern Point2D const						AdjacentPoint[FACING_COUNT];

inline LEPTON Cell_To_Lepton(int cell_distance)
{
	LEPTON lepton;
	lepton = cell_distance * CELL_LEPTON_W;
	return(lepton);
}

inline int Lepton_To_Cell(LEPTON lepton_distance)
{
	return(lepton_distance / CELL_LEPTON);
}

inline Coord Coord_Snap(Coord const & coord)
{
	return(Coord(coord.X - (coord.X & (CELL_LEPTON_W-1)) + CELL_LEPTON_W/2, coord.Y - (coord.Y & (CELL_LEPTON_H-1)) + CELL_LEPTON_H/2, coord.Z));
}

inline Coord Coord_Fraction(Coord const & coord)
{
	Coord tmp;
	tmp.X = coord.X & (CELL_LEPTON_W-1);
	tmp.Y = coord.Y & (CELL_LEPTON_H-1);
	tmp.Z = coord.Z;
	return(tmp);
}

inline Coord Coord_Whole(Coord const & coord)
{
	int x = coord.X / CELL_LEPTON_W;
	int y = coord.Y / CELL_LEPTON_H;

	return(Coord(x * CELL_LEPTON_W, y * CELL_LEPTON_H, coord.Z));
}

inline Coord Coord_Mid(Coord const & coord1, Coord const & coord2)
{
	Coord coord = coord1 + coord2;
	coord.X = coord.X / 2;
	coord.Y = coord.Y / 2;
	coord.Z = coord.Z / 2;
	return(coord);
}

inline Coord Move_Coord(Coord const & start, DirType const & dir, int distance)
{
	double radians = dir.As_Radian();
	return(Coord((start.X + fastmath::cos(radians) * distance), (start.Y - fastmath::sin(radians) * distance), start.Z));
}

inline Cell Move_Cell(Cell const & start, DirType const & dir, short distance)
{
	double radians = dir.As_Radian();
	return(Cell((start.X + fastmath::cos(radians) * distance), (start.Y - fastmath::sin(radians) * distance)));
}

inline FacingType Dir_Facing(DirType facing)
{
	return(facing.As_Facing());
}

inline FacingType Dir_Facing(Dir256 facing)
{
	return(Dir_Facing(DirType(facing)));
}

inline Dir256 Facing_Dir(FacingType facing)
{
	return((Dir256)((int)facing << 5));
}

inline DirType Direction(Coord const & coord1, Coord const & coord2)
{
	int x1 = coord1.X;
	int x2 = coord2.X;
	double angle = fastmath::atan2((double)coord1.Y - (double)coord2.Y, (double)x2 - (double)x1);
	return(DirType(angle));
}

inline DirType Direction(Cell const & cell1, Cell const & cell2)
{
	return(fastmath::atan2((double)(int)cell1.Y - (double)(int)cell2.Y, (double)(int)cell2.X - (double)(int)cell1.X));
}

inline DirType Direction(Point2D const & coord1, Point2D const & coord2)
{
	int x1 = coord1.X;
	int x2 = coord2.X;

	return(fastmath::atan2((double)coord1.Y - (double)coord2.Y, (double)x2 - (double)x1));
}

inline Coord Adjacent_Cell(Coord const & coord, FacingType dir)
{
	return(coord + AdjacentPoint[(int)dir & 0x07]);
}

inline Coord Adjacent_Cell(Coord const & coord, Dir256 dir)
{
	return(Adjacent_Cell(coord, Dir_Facing(dir)));
}

inline Cell Adjacent_Cell(Cell const & cell, FacingType dir)
{
	return(cell + AdjacentCell[(int)dir & 0x07]);
}

inline Cell Adjacent_Cell(Cell const & cell, Dir256 dir)
{
	return(cell + AdjacentCell[Dir_Facing(dir)]);
}

inline int Distance(Cell const & cell1, Cell const & cell2)
{
	return((cell1 - cell2).Length());
}

inline int Distance(Coord const & coord1, Coord const & coord2)
{
	return((coord1 - coord2).Length());
}

inline DirType Random_Dir(int min, int max)
{
	union Ugh {
		unsigned int UInt;
		struct {
			short LoWord : 16;
			short HiWord : 16;
		};
		struct {
			unsigned int LoByte : 8;
			unsigned int HiByte : 8;
		};
		unsigned short Dir;
	};

	Ugh dir;

	dir.LoWord = 0;
	dir.HiByte = Scen->RandomNumber();

	DirType d;
	d.Raw = dir.UInt;
	return(d);
}
