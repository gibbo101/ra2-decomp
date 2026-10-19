// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "techtype.h"

#include "aircraft.hh"

class AnimTypeClass;

class AircraftTypeClass : public TechnoTypeClass
{
		typedef TechnoTypeClass BASECLASS;

	public:
		AircraftType HeapID;

		bool IsCarryall;
		AnimTypeClass * Trailer;
		int SpawnDelay;
		bool IsRotorEquipped;
		bool IsRotorCustom;
		bool IsLandable;
		bool IsFlyBy;
		bool IsFlyBack;
		bool IsAirportBound;
		bool IsFighter;

		AircraftTypeClass(NoInitClass const & x);
		AircraftTypeClass(char const * ininame = NULL);
		virtual ~AircraftTypeClass(void) override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;

		virtual RTTIType Fetch_RTTI(void) const override {return(RTTI_AIRCRAFTTYPE);}
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine & crc) const override;
		virtual int Fetch_Heap_ID(void) const override {return(HeapID);}

		static AircraftTypeClass * Find_Or_Make(char const * name);
		static AircraftType From_Name(char const * name);
		static void Init(TheaterType);
		static void One_Time(void);

		virtual bool Read_INI(CCINIClass & ini) override;
		virtual Point3D Lepton_Dimensions(void) const override;
		virtual bool Create_And_Place(Cell const & cell, HouseClass * house = NULL) const override;
		virtual ObjectClass * Create_One_Of(HouseClass * house) const override;
		virtual Cell const * Occupy_List(bool placement=false) const override;
		virtual bool CanUseWaypoint(void) const override;
		virtual bool CanAttackMove(void) const override;

		static void const * LRotorData;
		static void const * RRotorData;
};
