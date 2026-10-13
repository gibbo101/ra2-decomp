// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "object.h"
#include "stage.h"
#include "terrtype.h"

class TerrainClass : public ObjectClass, public StageClass
{
		typedef ObjectClass BASECLASS;

	public:

		TerrainTypeClass * Class;

		TerrainClass(void);
		TerrainClass(TerrainTypeClass const * type, Cell const & cell);
		TerrainClass(NoInitClass const & x) : BASECLASS(x), StageClass(x) {}
		virtual ~TerrainClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		void Start_To_Crumble(void);
		bool Is_Animating(void) const {return(Class->IsAnimated || IsCrumbling);}

		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;
		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;

		virtual bool Unlimbo(Coord const & coord, Dir256 dir=DIR_N) override;
		virtual bool Limbo(void) override;
		virtual MoveType Can_Enter_Cell(CellClass const * cell, FacingType dir = FACING_NONE, int cell_height = -1, CellClass const * = 0, bool = true) const override;

		virtual bool Render(Rect &, bool forced, bool extras_only) const override;
		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual void Editor_Draw_It(Point2D const & point, Rect const & cliprect) const override;
		virtual bool Mark(MarkType mark=MARK_CHANGE) override;
		virtual Rect Get_Render_Rect(void) override;

		virtual void Fire_Out(void) override;
		virtual bool Catch_Fire(void) override;
		virtual ResultType Take_Damage(int & damage, int distance, WarheadTypeClass const * warhead, TechnoClass * source=0, bool forced=false, bool=false, HouseClass * = 0) override;
		virtual void Set_Occupy_Bit(Coord const & coord) override;
		virtual void Clear_Occupy_Bit(Coord const & coord) override;

		virtual void AI(void) override;

#ifdef _DEBUG
		virtual void Debug_Dump(MonoClass *mono) const override;
#endif

		static void Read_INI(CCINIClass const & ini);
		static void Write_INI(CCINIClass & ini);

	private:

		bool IsOnFire;

		bool IsCrumbling;

		int Unused1;
		int Unused2;

		Point2D RenderPixelPos;

		static char const * const INI_NAME;
};
