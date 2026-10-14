// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstract.h"
#include "coord.h"
#include "index.h"
#include "noinit.h"
#include "rect.h"
#include "vector.h"

#include "overlay.hh"
#include "smudge.hh"

class ObjectTypeClass;
class Cell;
class Coord;
class TerrainClass;
class BuildingTypeClass;
template<class T> class DynamicVectorClass;

class FoggedObjectClass : public AbstractClass
{
		typedef AbstractClass BASECLASS;

		friend class CellClass;
		friend void Update_Fogged_Objects(void);

	public:
		FoggedObjectClass(NoInitClass const & x);
		FoggedObjectClass(void);
		FoggedObjectClass(Coord const & coord, OverlayType type, int data);
		FoggedObjectClass(Coord const & coord, SmudgeType type, int data);
		FoggedObjectClass(BuildingClass * object, bool fade);
		FoggedObjectClass(TerrainClass * object);
		virtual ~FoggedObjectClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;

		virtual int Fetch_Object_Size(bool oldsave) const override;
		virtual void Compute_CRC(CRCEngine &) const override;

		Cell const *Get_Head_Record_Occupy_List(void);
		ObjectTypeClass *Get_Head_Record_Object_Type(void);
		virtual Cell Get_Cell(void);

		static DynamicVectorClass<FoggedObjectClass *> FoggyObjects;

		static IndexClass<int, FoggedObjectClass *> FoggedObjectIndex;

	private:

		struct DrawRecord {

			ObjectTypeClass *TypeClass;

			int FrameNumber;

			unsigned char HeightAdjust;

			int ZAdjust;

			DrawRecord(ObjectTypeClass *type = NULL, int framenum = 0, int hadjust = 0, int zadjust = 0) :
				TypeClass(type),
				FrameNumber(framenum),
				HeightAdjust(hadjust),
				ZAdjust(zadjust)
			{
			}

			DrawRecord(const DrawRecord &that) :
				TypeClass(that.TypeClass),
				FrameNumber(that.FrameNumber),
				HeightAdjust(that.HeightAdjust),
				ZAdjust(that.ZAdjust)
			{
			}

			bool operator == (DrawRecord const & that) { return(TypeClass == that.TypeClass && FrameNumber == that.FrameNumber); }
			bool operator != (DrawRecord const & that) { return(TypeClass != that.TypeClass || FrameNumber != that.FrameNumber); }

		};

	public:

		OverlayType Overlay;

		HouseClass *House;

		int OverlayData;

		RTTIType RTTI;

		Coord Position;

		Rect BoundingRect;

		int CellHeight;

		SmudgeType Smudge;

		int SmudgeData;

		DynamicVectorClass<DrawRecord> Records;

		bool CanDraw;
};

void Draw_Fogged_Objects(Rect const & rect);
void Update_Fogged_Objects(void);
