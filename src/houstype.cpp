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

// SYNTHETIC: GAMEMD 0x005111E0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00511200 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00511230 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00511250 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00511270 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00511290 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005112B0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005112D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00511310 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00511340 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00511370 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005113A0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005113B0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005113D0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x005113F0
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

// FUNCTION: GAMEMD 0x00511650
HouseTypeClass::HouseTypeClass(NoInitClass const & x) :
	BASECLASS(x),
	ParentCountry(x),
	VeteranInfantry(x),
	VeteranUnits(x),
	VeteranAircraft(x)
{
}

// FUNCTION: GAMEMD 0x005116A0
HouseTypeClass::~HouseTypeClass(void)
{
	Detach_This_From_All(this, true);
	Array.Delete(this);
}

// FUNCTION: GAMEMD 0x005117D0
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

// FUNCTION: GAMEMD 0x00512170
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

// FUNCTION: GAMEMD 0x00512280
HRESULT STDMETHODCALLTYPE HouseTypeClass::IsDirty(void)
{
	return(false);
}

// FUNCTION: GAMEMD 0x00512290
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

// FUNCTION: GAMEMD 0x00512480
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

// FUNCTION: GAMEMD 0x00512570
HRESULT STDMETHODCALLTYPE HouseTypeClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	if (pcbSize == NULL) {
		return(E_POINTER);
	}

	pcbSize->HighPart = 0;
	pcbSize->LowPart = sizeof(int) + sizeof(*this);

	return(S_OK);
}

// FUNCTION: GAMEMD 0x005125A0
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

// FUNCTION: GAMEMD 0x00512640
HRESULT STDMETHODCALLTYPE HouseTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HouseTypeClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x00512680
HouseTypeClass * HouseTypeClass::Find_Or_Make(char const * ininame)
{
	return(TFind_Or_Make<HouseTypeClass>(ininame, Array));
}

// FUNCTION: GAMEMD 0x00512710
RTTIType HouseTypeClass::Fetch_RTTI(void) const
{
	return(RTTI_HOUSETYPE);
}

// FUNCTION: GAMEMD 0x00512720
int HouseTypeClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x00512730
int HouseTypeClass::Fetch_Heap_ID(void) const
{
	return(HeapID);
}

// FUNCTION: GAMEMD 0x00512740
ULONG STDMETHODCALLTYPE HouseTypeClass::AddRef(void)
{
	return(1);
}

// FUNCTION: GAMEMD 0x00512750
ULONG STDMETHODCALLTYPE HouseTypeClass::Release(void)
{
	return(1);
}

// SYNTHETIC: GAMEMD 0x00512760
// ??_GHouseTypeClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x005128A0
// ?Clear@?$DynamicVectorClass@PAVInfantryTypeClass@@@@UAEXXZ
template class DynamicVectorClass<InfantryTypeClass *>;

// TEMPLATE: GAMEMD 0x005128D0
// ?ID@?$VectorClass@PAVInfantryTypeClass@@@@UAEHPBQAVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x005128F0
// ?GetItem@?$VectorClass@PAVInfantryTypeClass@@@@UBEPAVInfantryTypeClass@@H@Z

// TEMPLATE: GAMEMD 0x00512900
// ?Clear@?$DynamicVectorClass@PAVUnitTypeClass@@@@UAEXXZ
template class DynamicVectorClass<UnitTypeClass *>;

// TEMPLATE: GAMEMD 0x00512930
// ?ID@?$VectorClass@PAVUnitTypeClass@@@@UAEHPBQAVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00512950
// ?GetItem@?$VectorClass@PAVUnitTypeClass@@@@UBEPAVUnitTypeClass@@H@Z

// TEMPLATE: GAMEMD 0x00512960
// ??0?$TypeList@PAVAircraftTypeClass@@@@QAE@XZ
template class TypeList<AircraftTypeClass *>;

// TEMPLATE: GAMEMD 0x00512990
// ?Clear@?$DynamicVectorClass@PAVAircraftTypeClass@@@@UAEXXZ
template class DynamicVectorClass<AircraftTypeClass *>;

// TEMPLATE: GAMEMD 0x005129C0
// ?ID@?$VectorClass@PAVAircraftTypeClass@@@@UAEHPBQAVAircraftTypeClass@@@Z
// TEMPLATE: GAMEMD 0x005129E0
// ?GetItem@?$VectorClass@PAVAircraftTypeClass@@@@UBEPAVAircraftTypeClass@@H@Z

// SYNTHETIC: GAMEMD 0x005129F0
// ??_G?$VectorClass@PAVInfantryTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00512A40
// ??_G?$VectorClass@PAVUnitTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00512A90
// ??_G?$VectorClass@PAVAircraftTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00512AE0
// ??_G?$DynamicVectorClass@PAVInfantryTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00512B30
// ??_G?$DynamicVectorClass@PAVUnitTypeClass@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x00512B80
// ??_G?$DynamicVectorClass@PAVAircraftTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00512BD0
// ??4?$VectorClass@PAVAircraftTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x00512C40
// ??8?$VectorClass@PAVAircraftTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00512C80
// ?Resize@?$VectorClass@PAVAircraftTypeClass@@@@UAE_NHPBQAVAircraftTypeClass@@@Z
template class VectorClass<AircraftTypeClass *>;

// TEMPLATE: GAMEMD 0x00512D30
// ?Clear@?$VectorClass@PBVInfantryTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00512D60
// ?ID@?$VectorClass@PBVInfantryTypeClass@@@@UAEHABQBVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00512DA0
// ?ID@?$VectorClass@PBVInfantryTypeClass@@@@UAEHPBQBVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00512DC0
// ??0?$DynamicVectorClass@PBVInfantryTypeClass@@@@QAE@IPBQBVInfantryTypeClass@@@Z
// SYNTHETIC: GAMEMD 0x00512E20
// ??_G?$VectorClass@PBVInfantryTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00512E70
// ?Resize@?$DynamicVectorClass@PBVInfantryTypeClass@@@@UAE_NHPBQBVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00512F20
// ?Add@?$DynamicVectorClass@PBVInfantryTypeClass@@@@QAE_NABQBVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00512F70
// ?ID@?$DynamicVectorClass@PBVInfantryTypeClass@@@@UAEHABQBVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00512FA0
// ??4?$VectorClass@PBVInfantryTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x00513010
// ??8?$VectorClass@PBVInfantryTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00513050
// ?Resize@?$VectorClass@PBVInfantryTypeClass@@@@UAE_NHPBQBVInfantryTypeClass@@@Z
template class VectorClass<InfantryTypeClass const *>;
template class DynamicVectorClass<InfantryTypeClass const *>;

// TEMPLATE: GAMEMD 0x00513100
// ?Clear@?$VectorClass@PBVUnitTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00513130
// ?ID@?$VectorClass@PBVUnitTypeClass@@@@UAEHABQBVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513170
// ?ID@?$VectorClass@PBVUnitTypeClass@@@@UAEHPBQBVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513190
// ??0?$DynamicVectorClass@PBVUnitTypeClass@@@@QAE@IPBQBVUnitTypeClass@@@Z
// SYNTHETIC: GAMEMD 0x005131F0
// ??_G?$VectorClass@PBVUnitTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00513240
// ?Resize@?$DynamicVectorClass@PBVUnitTypeClass@@@@UAE_NHPBQBVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x005132F0
// ?Add@?$DynamicVectorClass@PBVUnitTypeClass@@@@QAE_NABQBVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513340
// ?ID@?$DynamicVectorClass@PBVUnitTypeClass@@@@UAEHABQBVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513370
// ??4?$VectorClass@PBVUnitTypeClass@@@@QAEAAV0@ABV0@@Z
// TEMPLATE: GAMEMD 0x005133E0
// ??8?$VectorClass@PBVUnitTypeClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00513420
// ?Resize@?$VectorClass@PBVUnitTypeClass@@@@UAE_NHPBQBVUnitTypeClass@@@Z
template class VectorClass<UnitTypeClass const *>;
template class DynamicVectorClass<UnitTypeClass const *>;

// TEMPLATE: GAMEMD 0x005134D0
// ?Clear@?$VectorClass@PBVAircraftTypeClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00513500
// ?ID@?$VectorClass@PBVAircraftTypeClass@@@@UAEHABQBVAircraftTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513540
// ?ID@?$VectorClass@PBVAircraftTypeClass@@@@UAEHPBQBVAircraftTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513560
// ??0?$DynamicVectorClass@PBVAircraftTypeClass@@@@QAE@IPBQBVAircraftTypeClass@@@Z
// SYNTHETIC: GAMEMD 0x005135C0
// ??_G?$VectorClass@PBVAircraftTypeClass@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x00513610
// ?Resize@?$DynamicVectorClass@PBVAircraftTypeClass@@@@UAE_NHPBQBVAircraftTypeClass@@@Z
// TEMPLATE: GAMEMD 0x005136C0
// ?Add@?$DynamicVectorClass@PBVAircraftTypeClass@@@@QAE_NABQBVAircraftTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513710
// ?ID@?$DynamicVectorClass@PBVAircraftTypeClass@@@@UAEHABQBVAircraftTypeClass@@@Z
template class VectorClass<AircraftTypeClass const *>;
template class DynamicVectorClass<AircraftTypeClass const *>;

// TEMPLATE: GAMEMD 0x00513740
// ??0?$TypeList@PBVUnitTypeClass@@@@QAE@ABV0@@Z
template class TypeList<UnitTypeClass const *>;

// TEMPLATE: GAMEMD 0x005137D0
// ??0?$TypeList@PBVAircraftTypeClass@@@@QAE@ABV0@@Z
template class TypeList<AircraftTypeClass const *>;

// TEMPLATE: GAMEMD 0x00513860
// ??0?$TypeList@PBVInfantryTypeClass@@@@QAE@ABV0@@Z
template class TypeList<InfantryTypeClass const *>;

// TEMPLATE: GAMEMD 0x005138F0
// ??0?$VectorClass@PBVInfantryTypeClass@@@@QAE@HPBQBVInfantryTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513940
// ??0?$VectorClass@PBVUnitTypeClass@@@@QAE@HPBQBVUnitTypeClass@@@Z
// TEMPLATE: GAMEMD 0x00513990
// ??0?$VectorClass@PBVAircraftTypeClass@@@@QAE@HPBQBVAircraftTypeClass@@@Z

// FUNCTION: GAMEMD 0x005139E0
// HouseTypeClass::QueryInterface

// FUNCTION: GAMEMD 0x005139F0
// HouseTypeClass::AddRef

// FUNCTION: GAMEMD 0x00513A00
// HouseTypeClass::Release

// FUNCTION: GAMEMD 0x005139E0
// HouseTypeClass::QueryInterface

// FUNCTION: GAMEMD 0x005139F0
// HouseTypeClass::AddRef

// FUNCTION: GAMEMD 0x00513A00
// HouseTypeClass::Release

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EACE8;
extern const double DAT_007EACF0;

// SYNTHETIC: GAMEMD 0x00513A10 SYMBOL
// _$E66
double DAT_00A8F168 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x00513A40 SYMBOL
// _$E69
double DAT_00A8F1B8 = DAT_00A8F168 / DAT_007EACE8;

// SYNTHETIC: GAMEMD 0x00513A60 SYMBOL
// _$E72
double DAT_00A8F1A0 = DAT_007E1718 / DAT_00A8F1B8;

// SYNTHETIC: GAMEMD 0x00513A80 SYMBOL
// _$E75
double DAT_00A8F198 = DAT_007E1720 * DAT_007EACF0;

// SYNTHETIC: GAMEMD 0x00513AA0 SYMBOL
// _$E78
double DAT_00A8F170 = DAT_007E1728 * DAT_007EACF0;

// SYNTHETIC: GAMEMD 0x00513AC0 SYMBOL
// _$E81
double DAT_00A8F148 = DAT_007E1730 * DAT_007EACF0;

// SYNTHETIC: GAMEMD 0x00513AE0 SYMBOL
// _$E84
int DAT_00A8F1C0 = (int)(fastmath::tan(DAT_00A8F148 - DAT_00A8F170) * DAT_007E1738 * DAT_00A8F168);

// SYNTHETIC: GAMEMD 0x00513B20 SYMBOL
// _$E87
double DAT_00A8F178 = fastmath::atan(DAT_00A8F1C0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00513B50 SYMBOL
// _$E90
double DAT_00A8F190 = fastmath::atan((DAT_00A8F1C0 * 2) / DAT_00A8F168);

// SYNTHETIC: GAMEMD 0x00513B80 SYMBOL
// _$E93
int DAT_00A8F1B4 = (int)(DAT_00A8F1C0 * 4 + DAT_007E1738);

struct DAT_Pair16 { short A, B; DAT_Pair16() : A(0), B(0) {} };
struct DAT_Triple0 { int A, B, C; DAT_Triple0() : A(0), B(0), C(0) {} };
struct DAT_Triple80 { int A, B, C; DAT_Triple80() : A(128), B(128), C(0) {} };
struct DAT_Quad0 { int A, B, C, D; DAT_Quad0() : A(0), B(0), C(0), D(0) {} };

// SYNTHETIC: GAMEMD 0x00513BB0 SYMBOL
// _$E96
DAT_Pair16 DAT_00A8F160;

// SYNTHETIC: GAMEMD 0x00513BC0 SYMBOL
// _$E99
DAT_Triple0 DAT_00A8F180;

// SYNTHETIC: GAMEMD 0x00513BE0 SYMBOL
// _$E102
DAT_Triple80 DAT_00A8F1A8;

// SYNTHETIC: GAMEMD 0x00513C00 SYMBOL
// _$E105
DAT_Quad0 DAT_00A8F150;
