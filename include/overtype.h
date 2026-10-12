// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "objtype.h"

#include "rgb.h"

#include "land.hh"
#include "overlay.hh"

class AnimTypeClass;

class OverlayTypeClass: public ObjectTypeClass
{
		typedef ObjectTypeClass BASECLASS;

	public:

		OverlayType HeapID;

		LandType Land;

		AnimTypeClass *CellAnim;

		int DamageLevels;

		int DamagePoints;

		bool IsWall;

		bool IsTiberium;

		bool IsCrate;

		bool IsCrateTrigger;

		bool IsNoUseTileLandType;

		bool IsVeinholeMonster;

		bool IsVeins;

		bool DemandLoad;

		bool IsExplosive;

		bool IsChainReaction;

		bool IsOverrides;

		bool IsDrawFlat;

		bool IsRubble;

		bool IsARock;

		RGBClass RadarColor;

		OverlayTypeClass(NoInitClass const & x);
		OverlayTypeClass(char const * ininame = NULL);
		~OverlayTypeClass(void);

		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;

		static OverlayTypeClass * Find_Or_Make(char const * name);
		static OverlayType From_Name(char const * name);
		static void Init(TheaterType);
		static void Prep_For_Add(void);

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_OVERLAYTYPE);}
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Object_Size(bool oldsave) const override {return(sizeof(*this));}
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);};

		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house) const override;
		virtual ObjectClass * Create_One_Of(HouseClass *) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect, int data) const;

		virtual bool Read_INI(CCINIClass & ini) override;

		virtual void const * Get_Image_Data(void) const override;

		RGBClass Get_Radar_Color(int shape) const;
};
