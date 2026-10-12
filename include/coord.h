// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "comtypes.h"
#include "fastmath.h"
#include "point.h"
#include "sun.h"

class Coord;
class Cell;

Coord Coord_Scatter(Coord const & coord, int distance, bool lock=false);
Coord Adjacent_Coord_With_Height(Coord const & coord, FacingType dir);

typedef int LEPTON;

class Cell : public TPoint2D<short>
{
		typedef TPoint2D<short> BASECLASS;

	public:
		Cell(void) : BASECLASS() {}
		Cell(Coord const & cell);
		Cell(CellStruct const & coord) { X = coord.X; Y = coord.Y; }
		Cell(int x, int y) : BASECLASS(x, y) {}
		explicit Cell(TPoint2D<int> const & pt) : BASECLASS(pt.X, pt.Y) {}
		Cell(BASECLASS const & pt) : BASECLASS(pt) {};
		explicit Cell(int cellnum) : BASECLASS(cellnum % 128, cellnum / 128) {}

		bool operator == (Cell const & rvalue) const {return(X==rvalue.X && Y==rvalue.Y);}

		Coord As_Coord(int z = 0) const;

		operator CellStruct(void) const
		{
			CellStruct cs;
			cs.X = X;
			cs.Y = Y;
			return(cs);
		}

		int As_Int(void) const { return((Y - (MAP_CELL_W * (X + Y)) - X) << 6); }
};

class Coord : public Point3D
{
		typedef TPoint3D<int> BASECLASS;

	public:
		Coord(void) : BASECLASS() {}
		Coord(Point2D const & pt, LEPTON z) {X=pt.X; Y=pt.Y; Z=z;}
		Coord(Cell const & cell, LEPTON z = 0);
		Coord(CoordStruct const & coord) { X = coord.X; Y = coord.Y; Z = coord.Z; }
		Coord(int x, int y, int z = 0) : BASECLASS(x, y, z) {}
		Coord(BASECLASS const & pt) : BASECLASS(pt) {};

		Cell As_Cell(void) const;

		operator CoordStruct(void) const
		{
			CoordStruct cs;
			cs.X = X;
			cs.Y = Y;
			cs.Z = Z;
			return(cs);
		}

		int As_Int(void) { return((X / 10) + ((Y / 10) << 16)); }
};

inline Cell::Cell(Coord const & coord)
{
	*this = coord.As_Cell();
}

inline Coord Cell::As_Coord(int z) const
{
	Coord coord(X * CELL_LEPTON_W + CELL_LEPTON_W / 2, Y * CELL_LEPTON_H + CELL_LEPTON_H / 2, z);
	return(coord);
}

inline Coord::Coord(Cell const & cell, LEPTON z) :
	Point3D((cell.X * CELL_LEPTON_W) + (CELL_LEPTON_W / 2), (cell.Y * CELL_LEPTON_H) + (CELL_LEPTON_H / 2), z)
{

}

inline Cell Coord::As_Cell(void) const
{
	return(Cell(X / CELL_LEPTON_W, Y / CELL_LEPTON_H));
}

struct xCell
{

	short X;
	short Y;

	xCell & operator = (Cell const & that)
	{
		X = that.X;
		Y = that.Y;
		return(*this);
	}
};

struct xCoord
{

	LEPTON X;
	LEPTON Y;

	xCoord & operator = (Coord const & that)
	{
		X = that.X;
		Y = that.Y;
		return(*this);
	}
};
