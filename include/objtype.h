// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "_voxel.h"
#include "abstype.h"
#include "index.h"
#include "rgb.h"

#include "armor.hh"
#include "voc.hh"

template<class T> class DynamicVectorClass;

class ObjectTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:

		RGBClass RadialColor;
		unsigned char Unused_9B;

		ArmorType Armor;

		int MaxStrength;

		void const * ImageData;
		bool ImageAllocated;

		void const * AlphaImageData;

		VoxelDataStruct Voxel;
		VoxelDataStruct AuxVoxel;
		VoxelDataStruct AuxVoxel2;

		VoxelDataStruct ChargerTurrets[18];
		VoxelDataStruct ChargerBarrels[18];

		bool NoSpawnAlt;

		int MaxSize;

		VocType CrushSound;

		int AmbientSound;

		TStringID<24> GraphicName;

		bool AlternateArcticArt;
		bool ArcticArtInUse;

		TStringID<24> AlphaGraphicName;

		bool IsTheater;
		bool IsCrushable;
		bool IsBombable;
		bool IsStealthy;
		bool IsSelectable;
		bool IsLegalTarget;
		bool IsInsignificant;
		bool IsImmune;
		bool IsSentient;
		bool IsFootprint;
		bool IsVoxel;
		bool IsNewTheater;
		bool IsHasRadialIndicator;
		bool IsIgnoresFirestorm;
		bool UseLineTrail;
		RGBClass LineTrailColor;
		int LineTrailColorDecrement;

		VoxelIndexClass VoxelIndex;
		VoxelIndexClass AuxVoxelIndex;
		VoxelIndexClass ShadowVoxelIndex;
		VoxelIndexClass AuxVoxel2Index;

		ObjectTypeClass(NoInitClass const & x);
		ObjectTypeClass(char const * ininame = NULL);
		virtual ~ObjectTypeClass() override;

		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *pcbSize) override;

		static void One_Time(void);

		char const * Graphic_Name(void) const {return(GraphicName);}
		char const * Alpha_Graphic_Name(void) const {return(AlphaGraphicName);}

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual Coord const Coord_Fixup(Coord const & coord) const {return(coord);};
		virtual int Get_Ownable(void) const {return(-1);};
		virtual int Max_Pips(void) const;
		virtual Point3D Pixel_Dimensions(void) const;
		virtual Point3D Lepton_Dimensions(void) const;
		virtual bool Create_And_Place(Cell const & , HouseClass * house = NULL) const = 0;
		virtual int Cost_Of(HouseClass * house = NULL) const;
		virtual int Time_To_Build(void) const;
		virtual ObjectClass * Create_One_Of(HouseClass *) const = 0;
		virtual Cell const * Occupy_List(bool placement=false) const;
		virtual BuildingClass * Who_Can_Build_Me(bool intheory, bool needsnopower, bool legal, HouseClass * house) const;
		virtual void const * Get_Cameo_Data(void) const;
		virtual void const * Get_Image_Data(void) const {return(ImageData);};

		static ObjectTypeClass const * From_Name(char const * name);

		void Fetch_Voxel_Image(void);
		void Fetch_Aux_Voxel_Image(void);
		void Fetch_Normal_Image(void);
		void Free_Voxels(void);
		bool Fetch_Charger_Turret(char const * graphic, int index);
		bool Fetch_Charger_Barrel(char const * graphic, int index);

		void Theater_Naming_Convention(char * name, TheaterType theater) const;

		static void Clear_Voxel_Indexes(void);

		void Clear_Voxel_Index(void)
		{
			VoxelIndex.Clear();
			AuxVoxelIndex.Clear();
			ShadowVoxelIndex.Clear();
			AuxVoxel2Index.Clear();
		}

		static void const * SelectShapes;
		static void const * PipShapes;

		static void const * Pip2Shapes;

		static void const * TalkBubbleShapes;

		static DynamicVectorClass<ObjectTypeClass *> ObjectTypes;
};
