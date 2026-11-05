// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "facing.h"
#include "object.h"
#include "polygon.h"
#include "vector.h"

#include "wave.hh"

class WaveClass : public ObjectClass
{
		typedef ObjectClass BASECLASS;

	public:
		WaveClass(NoInitClass const & x) : BASECLASS(x), WaveShape(x), DrawData(x), AffectedCells() {}
		WaveClass(Coord const & source_coord, Coord const & target_coord, TechnoClass * source, WaveType type, TechnoClass * target);
		WaveClass(void);
		virtual ~WaveClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual ObjectTypeClass const * Class_Of(void) const override;
		virtual void Detach(AbstractClass const * target, bool all = true) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual LayerType In_Which_Layer(void) const override;

		virtual bool Limbo(void) override;
		virtual bool Unlimbo(Coord const & coord, Dir256 facing = DIR_N) override;

		virtual void Draw_It(Point2D const & point, Rect const & cliprect) const override;
		void Draw_Sonic(Point2D const & point, Rect const & cliprect);
		void Draw_Laser(Point2D const & point, Rect const & cliprect);
		void FUN_007602E0(Point2D const & point, Rect const & cliprect);

		virtual void AI(void) override;

		static void Init_Statics(void);

		static DynamicVectorClass<WaveClass *> Array;

	private:
		void Set_Sonic_Pixel(int x, int xoff, int y, unsigned short * buffer) const;
		void Set_Laser_Pixel(unsigned short * buffer, int mult) const;
		void FUN_00760190(int x, int xoff, int y, unsigned short * buffer) const;

		void Laser_AI(void);
		void FUN_007610A0(void);

		void Sonic_AI(void);
		void Wave_Recalc_Affected_Cells(void);
		void Build_Wave_Shape(Coord const & coord1, Coord const & coord2);
		void Draw_Magnetic(Coord const & coord1, Coord const & coord2);
		void Draw_NonMagnetic(Coord const & coord1, Coord const & coord2);
		void Update_Wave(void);
		void Wave_Shape_AI(void);
		void Sonic_Damage(Coord const & coord);
		void Sonic_Add_Cell(Cell const & cell);

		void Init_Offset_Tables(void);

	private:

		TechnoClass * Target;

		WaveType Type;

		Coord StartCoord;

		Coord EndCoord;

		Point2D WaveStartMiddle;
		Point2D WaveEndMiddle;
		Point2D WaveEndLeft;
		Point2D WaveEndRight;
		Point2D WaveStartLeft;
		Point2D WaveStartRight;

		Coord WaveEndLeftCoord;
		Coord WaveEndRightCoord;
		Coord WaveStartLeftCoord;
		Coord WaveStartRightCoord;

		bool IsWaveActive;
		bool ShouldEnd;
		unsigned char field_12E;
		unsigned char field_12F;

		int WaveEC;
		int WaveCount;

		double WaveProgress;
		double FadeProgress;

		PolygonShapeStruct WaveShape;

		struct ActiveWaveStruct {
			Point2D StartMiddle;
			Point2D EndMiddle;
			Point2D EndLeft;
			Point2D EndRight;
			Point2D StartLeft;
			Point2D StartRight;
		} ActiveWave;

		struct MagPointStruct {
			int X;
			int Y;
		} MagPoints[4];

		mutable PolygonRasterStruct DrawData;

		int DirectionStrides[FACING_COUNT];

		FacingType Direction;

		int LaserEC;

		TechnoClass * Source;

		FacingClass Facing;

		DynamicVectorClass<CellClass *> AffectedCells;

		int WaveIntensityTable[13];
};

Coord Lerp(Coord const & a, Coord const & b, float t);
Point2D Lerp(Point2D const & a, Point2D const & b, float t);
FacingType Facing_Between_Points(Point2D const & pt1, Point2D const & pt2);
