// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"

#include "smudge.hh"
#include "theater.hh"

class SmudgeTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:

		SmudgeType HeapID;

		int Width;
		int Height;

		bool IsCrater;

		bool IsScorch;

		SmudgeTypeClass(NoInitClass const & x);
		SmudgeTypeClass(char const * ininame = NULL);
		virtual ~SmudgeTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_SMUDGETYPE);}
		virtual int Fetch_Object_Size(void) const override {return(sizeof(*this));}
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);}

		static SmudgeType From_Name(char const * name);
		static void Init(TheaterType);
		static void One_Time(void) {BASECLASS::One_Time();}
		static void Prep_For_Add(void);

		static SmudgeTypeClass * Find_Or_Make(const char *name);

		virtual bool Read_INI(CCINIClass & ini) override;

		virtual bool Create_And_Place(Cell const & cell, HouseClass * house) const override;
		virtual ObjectClass * Create_One_Of(HouseClass *) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect, int size, int z, Cell const & cell) const;

		bool Can_Place_Here(Cell const & origin, bool underbuildings) const;
		void Place(Cell const & origin) const;

		static bool Scorch_The_Ground(Coord const & coord, int width = 100, int height = 100, bool large = false);
		static bool Crater_The_Ground(Coord const & coord, int width = 100, int height = 100, bool large = false);
};
