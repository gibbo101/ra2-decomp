// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "houstype.h"
#include "ccini.h"
#include "crc.h"
#include "findmake.h"
#include "globals.h"
#include "side.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include <cmath>

DynamicVectorClass<HouseTypeClass *> HouseTypeClass::Array;

// FUNCTION: 0x005111E0 $E2
// FUNCTION: 0x00511200 $E5
// FUNCTION: 0x00511230 $E8
// FUNCTION: 0x00511250 $E11
// FUNCTION: 0x00511270 $E14
// FUNCTION: 0x00511290 $E17
// FUNCTION: 0x005112B0 $E20
// FUNCTION: 0x005112D0 $E23
// FUNCTION: 0x00511310 $E26
// FUNCTION: 0x00511340 $E29
// FUNCTION: 0x00511370 $E32
// FUNCTION: 0x005113A0 $E35
// FUNCTION: 0x005113B0 $E38
// FUNCTION: 0x005113D0 $E41

// FUNCTION: 0x005113F0
HouseTypeClass::HouseTypeClass(char const * ininame) :
	BASECLASS(ininame),
	HeapID(HOUSE_NONE),
	House(HOUSE_NONE),
	Side(SIDE_NONE),
	Scheme(0),
	FirepowerBias(1.0),
	GroundspeedBias(1.0),
	AirspeedBias(1.0),
	ArmorBias(1.0),
	ROFBias(1.0),
	CostBias(1.0),
	BuildSpeedBias(1.0),
	ArmorInfantryMult(1.0f),
	ArmorUnitsMult(1.0f),
	ArmorAircraftMult(1.0f),
	ArmorBuildingsMult(1.0f),
	ArmorDefensesMult(1.0f),
	CostInfantryMult(1.0f),
	CostUnitsMult(1.0f),
	CostAircraftMult(1.0f),
	CostBuildingsMult(1.0f),
	CostDefensesMult(1.0f),
	SpeedInfantryMult(1.0f),
	SpeedUnitsMult(1.0f),
	SpeedAircraftMult(1.0f),
	BuildtimeInfantryMult(1.0f),
	BuildtimeUnitsMult(1.0f),
	BuildtimeAircraftMult(1.0f),
	BuildtimeBuildingsMult(1.0f),
	BuildtimeDefensesMult(1.0f),
	IncomeMult(1.0f),
	Prefix('A'),
	IsMultiplay(false),
	IsMultiplayPassive(false),
	IsWallOwner(true),
	IsSmartAI(false)
{
	Create_ID();
	Suffix[0] = '\0';
	Array.Add(this);
	House = (HousesType)Array.ID(this);
	HeapID = (HousesType)Array.ID(this);
}

// FUNCTION: 0x00511650
HouseTypeClass::HouseTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	ParentCountry(x),
	VeteranInfantry(x),
	VeteranUnits(x),
	VeteranAircraft(x)
{
}

// FUNCTION: 0x005116A0
HouseTypeClass::~HouseTypeClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);
}

// FUNCTION: 0x005117D0
HousesType HouseTypeClass::From_Name(char const * name)
{
	if (stricmp(name, "<random>") == 0) {
		return((HousesType)-2);
	}
	if (name != NULL) {
		for (int house = HOUSE_FIRST; house < Array.Count(); house++) {
			HouseTypeClass *ptr = Array[house];
			if (stricmp(ptr->Full_Name(), name) == 0 || stricmp(ptr->Name(), name) == 0) {
				return(ptr->House);
			}
		}
	}
	return(HOUSE_NONE);
}

// FUNCTION: 0x00512170
void HouseTypeClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(House);
	crc(Side);
	crc(Scheme);
	crc(FirepowerBias);
	crc(GroundspeedBias);
	crc(AirspeedBias);
	crc(ArmorBias);
	crc(ROFBias);
	crc(CostBias);
	crc(BuildSpeedBias);
	crc(Suffix, strlen(Suffix));
	crc(Prefix);
	crc(IsMultiplay);
}

// FUNCTION: 0x00512280
HRESULT STDMETHODCALLTYPE HouseTypeClass::IsDirty(void)
{
	return(false);
}

// FUNCTION: 0x00512290
HRESULT STDMETHODCALLTYPE HouseTypeClass::Load(IStream * stream)
{
	VeteranInfantry.Clear();
	VeteranUnits.Clear();
	VeteranAircraft.Clear();
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) HouseTypeClass(NoInitClass());

		VeteranInfantry.Load(stream);
		VeteranUnits.Load(stream);
		VeteranAircraft.Load(stream);

		for (int i = 0; i < VeteranInfantry.Count(); i++) {
			Swizzle_Pointer(&VeteranInfantry[i]);
		}
		for (int j = 0; j < VeteranUnits.Count(); j++) {
			Swizzle_Pointer(&VeteranUnits[j]);
		}
		for (int k = 0; k < VeteranAircraft.Count(); k++) {
			Swizzle_Pointer(&VeteranAircraft[k]);
		}

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00512480
HRESULT STDMETHODCALLTYPE HouseTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		VeteranInfantry.Save(stream);
		VeteranUnits.Save(stream);
		VeteranAircraft.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00512570
HRESULT STDMETHODCALLTYPE HouseTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	if (pcbSize == NULL) {
		return(E_POINTER);
	}

	pcbSize->HighPart = 0;
	pcbSize->LowPart = sizeof(int) + sizeof(*this);

	return(S_OK);
}

// FUNCTION: 0x005125A0
HRESULT STDMETHODCALLTYPE HouseTypeClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = (IUnknown *)(IPersistStream *)this;
	}
	if (riid == IID_IPersist) {
		*ppvObject = (IPersistStream *)this;
	}
	if (riid == IID_IPersistStream) {
		*ppvObject = (IPersist *)this;
	}
	if (riid == IID_IRTTITypeInfo) {
		*ppvObject = (IRTTITypeInfo *)this;
	}
	if (*ppvObject == NULL) {
		return(E_NOINTERFACE);
	}

	AddRef();
	return(S_OK);
}

// FUNCTION: 0x00512640
HRESULT STDMETHODCALLTYPE HouseTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HouseTypeClass;
	return(S_OK);
}

// FUNCTION: 0x00512680
HouseTypeClass * HouseTypeClass::Find_Or_Make(char const * ininame)
{
	return(TFind_Or_Make<HouseTypeClass>(ininame, Array));
}

// FUNCTION: 0x00512710
RTTIType HouseTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_HOUSETYPE);
}

// FUNCTION: 0x00512720
int HouseTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x00512730
int HouseTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}

// FUNCTION: 0x00512740
ULONG STDMETHODCALLTYPE HouseTypeClass::AddRef(void)
{
	return(1);
}

// FUNCTION: 0x00512750
ULONG STDMETHODCALLTYPE HouseTypeClass::Release(void)
{
	return(1);
}

// FUNCTION: 0x00512760 HouseTypeClass::~HouseTypeClass

// FUNCTION: 0x005128A0 ?$DynamicVectorClass@PAVInfantryTypeClass@@::Clear
template class DynamicVectorClass<InfantryTypeClass *>;

// FUNCTION: 0x005128D0 ?$VectorClass@PAVInfantryTypeClass@@::ID
// FUNCTION: 0x005128F0 ?$VectorClass@PAVInfantryTypeClass@@::GetItem

// FUNCTION: 0x00512900 ?$DynamicVectorClass@PAVUnitTypeClass@@::Clear
template class DynamicVectorClass<UnitTypeClass *>;

// FUNCTION: 0x00512930 ?$VectorClass@PAVUnitTypeClass@@::ID
// FUNCTION: 0x00512950 ?$VectorClass@PAVUnitTypeClass@@::GetItem

// FUNCTION: 0x00512960 ?$TypeList@PAVAircraftTypeClass@@::?$TypeList@PAVAircraftTypeClass@@
template class TypeList<AircraftTypeClass *>;
