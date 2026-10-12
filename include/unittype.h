// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "face.h"
#include "point.h"
#include "stringid.h"
#include "techtype.h"

#include "land.hh"
#include "unit.hh"

class ConvertClass;
class Surface;

class UnitTypeClass : public TechnoTypeClass
{
		typedef TechnoTypeClass BASECLASS;

	public:
		UnitType HeapID;

		LandType MovementRestrictedTo;

		TPoint3D<int> HalfDamageSmokeLocation;

		bool Passive;
		bool CrateGoodie;
		bool Harvester;
		bool Weeder;
		bool unknown_E10;
		bool HasTurret;
		bool DeployToFire;
		bool IsSimpleDeployer;
		bool IsTilter;
		bool UseTurretShadow;
		bool TooBigToFitUnderBridge;
		bool CanBeach;
		bool SmallVisceroid;
		bool LargeVisceroid;
		bool CarriesCrate;
		bool NonVehicle;

		int StandingFrames;
		int DeathFrames;
		int DeathFrameRate;
		int StartStandFrame;
		int StartWalkFrame;
		int StartFiringFrame;
		int StartDeathFrame;
		int MaxDeathCounter;
		int Facings;
		int FiringSyncFrame[2];
		int BurstDelay[4];

		const void * AltImage;

		char WalkFrames;
		char FiringFrames;
		char AltImageFile[25];

		UnitTypeClass(NoInitClass const & x);
		UnitTypeClass(char const * ininame = NULL);
		virtual ~UnitTypeClass() override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(bool oldsave) const override;
		virtual int Get_Object_Size_Delta(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override;

		static UnitType From_Name(char const * name);
		static void Init(TheaterType ) {};
		static void One_Time(void);
		static UnitTypeClass * Find_Or_Make(char const * name);

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual Coord const Coord_Fixup(Coord const & coord) const override;
		virtual Point3D Pixel_Dimensions(void) const override;
		virtual Point3D Lepton_Dimensions(void) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house = NULL) const override;
		virtual ObjectClass * Create_One_Of(HouseClass * house) const override;
		virtual bool Legal_Placement(Cell const & pos, HouseClass * house) const override;
		virtual int Flight_Level(void) const override;

		TPoint2D<int> Turret_Adjust(Dir256 dir, TPoint2D<int> const & xy) const;

		static void const * SmallVisceroidShapes;
		static void const * LargeVisceroidShapes;
};

extern Surface * EightBitSurface;
extern ConvertClass * EightBitDrawer;
