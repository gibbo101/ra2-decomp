// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "coord.h"
#include "noinit.h"
#include "rect.h"
#include "vector.h"

#include "house.hh"
#include "struct.hh"

#include <unknwn.h>

class CCINIClass;
class CRCEngine;
class BuildingClass;
class HouseClass;

class BaseNodeClass
{
	public:
		BaseNodeClass(void) {};
		BaseNodeClass(StructType building, Cell cell) : Type(building), CellID(cell) {};
		bool operator == (BaseNodeClass const & node);
		bool operator != (BaseNodeClass const & node);
		bool operator > (BaseNodeClass const & node);

		StructType Type;

		Cell CellID;

		bool Placed;

		int Attempts;
};

class BaseClass
{
	public:

		BaseClass(void);
		BaseClass(NoInitClass const & x);
		~BaseClass(void) {Nodes.Clear();}

		void Init(void) {Nodes.Clear();}

		void Read_INI(CCINIClass const & ini, char const * hname);
		void Write_INI(CCINIClass & ini, char const * hname);
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream);
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream);
		virtual void Compute_CRC(CRCEngine &) const;

		bool Is_Built(int index) const;

		BuildingClass * Get_Building(int index) const;

		bool Is_Node(BuildingClass const * obj);

		void Mark_Node_Placed(BuildingClass const * obj);

		BaseNodeClass * Get_Node(BuildingClass const * obj);
		BaseNodeClass * Get_Node(int index) { return(&Nodes[index]); }
		BaseNodeClass * Get_Node(Cell const & cell);

		BaseNodeClass * Next_Buildable(StructType type = STRUCT_NONE);
		int Next_Buildable_Index(StructType type = STRUCT_NONE);

		DynamicVectorClass<BaseNodeClass> Nodes;

		int PercentBuilt;

		DynamicVectorClass<Cell> InnerCells;
		DynamicVectorClass<Cell> OuterCells;

		Cell PlacementCenter;

		Rect BaseAreaRect;

		Rect LastBaseAreaRect;

		HouseClass * House;

		static char const * const INI_NAME;
};

extern HouseClass *UnusedHouse;
