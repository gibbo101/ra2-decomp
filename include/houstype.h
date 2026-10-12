// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "abstype.h"
#include "typelist.h"

#include "house.hh"
#include "side.hh"

class AircraftTypeClass;
class InfantryTypeClass;
class UnitTypeClass;

class HouseTypeClass : public AbstractTypeClass
{
		typedef AbstractTypeClass BASECLASS;

	public:
		struct CountryName
		{
			char Buffer[25];
			CountryName(void) { Buffer[0] = '\0'; }
			CountryName(NoInitClass const &) {}
			operator char const *(void) const { return(Buffer); }
			operator char *(void) { return(Buffer); }
		};

		CountryName ParentCountry;
		HousesType HeapID;
		HousesType House;
		SideType Side;
		int Scheme;
		double FirepowerBias;
		double GroundspeedBias;
		double AirspeedBias;
		double ArmorBias;
		double ROFBias;
		double CostBias;
		double BuildSpeedBias;
		float ArmorInfantryMult;
		float ArmorUnitsMult;
		float ArmorAircraftMult;
		float ArmorBuildingsMult;
		float ArmorDefensesMult;
		float CostInfantryMult;
		float CostUnitsMult;
		float CostAircraftMult;
		float CostBuildingsMult;
		float CostDefensesMult;
		float SpeedInfantryMult;
		float SpeedUnitsMult;
		float SpeedAircraftMult;
		float BuildtimeInfantryMult;
		float BuildtimeUnitsMult;
		float BuildtimeAircraftMult;
		float BuildtimeBuildingsMult;
		float BuildtimeDefensesMult;
		float IncomeMult;
		TypeList<InfantryTypeClass *> VeteranInfantry;
		TypeList<UnitTypeClass *> VeteranUnits;
		TypeList<AircraftTypeClass *> VeteranAircraft;
		char Suffix[4];
		char Prefix;
		bool IsMultiplay;
		bool IsMultiplayPassive;
		bool IsWallOwner;
		bool IsSmartAI;

		HouseTypeClass(NoInitClass const & x);
		HouseTypeClass(char const * ininame = NULL);
		virtual ~HouseTypeClass() override;

		virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID * retval) override;
		virtual HRESULT STDMETHODCALLTYPE IsDirty(void) override;
		virtual HRESULT STDMETHODCALLTYPE Load(IStream * stream) override;
		virtual HRESULT STDMETHODCALLTYPE Save(IStream * stream, BOOL cleardirty) override;
		virtual HRESULT STDMETHODCALLTYPE GetSizeMax(ULARGE_INTEGER *pcbSize) override;
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID * ppvObject) override;
		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
		virtual ULONG STDMETHODCALLTYPE Release(void) override;

		virtual RTTIType Fetch_RTTI(void) const override;
		virtual int Fetch_Object_Size(void) const override;
		virtual void Compute_CRC(CRCEngine &) const override;
		virtual int Fetch_Heap_ID(void) const override;

		static DynamicVectorClass<HouseTypeClass *> Array;

		static HousesType From_Name(char const * name);
		static void One_Time(void);
		static HouseTypeClass * Find_Or_Make(char const * ininame);

		virtual bool Read_INI(CCINIClass & ini) override;
};
