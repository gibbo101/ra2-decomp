// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"
#include "rgb.h"

#include "bsize.hh"
#include "terrain.hh"
#include "theater.hh"

class TerrainTypeClass : public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:

		TerrainType HeapID;

		BSizeType Foundation;

		RGBClass RadarColor;

		int AnimationRate;
		float AnimationProbability;

		int TemperateOccupationBits;
		int SnowOccupationBits;

		bool IsWaterBased;

		bool IsTiberiumSpawn;

		bool IsFlammable;

		bool IsAnimated;

		bool IsVeinhole;

		TerrainTypeClass(NoInitClass const & x);
		TerrainTypeClass(char const * ininame = NULL);
		virtual ~TerrainTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual int Fetch_Heap_ID(void) const override;

		static TerrainType From_Name(char const * name);
		static TerrainTypeClass * Find_Or_Make(const char * name);
		static void Init(TheaterType theater = THEATER_TEMPERATE);
		static void One_Time(void){}

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house) const override;
		virtual ObjectClass * Create_One_Of(HouseClass *) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;

	private:
		Cell const * Occupy;
};
