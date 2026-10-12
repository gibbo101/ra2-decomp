// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "house.h"
#include "_logic.h"
#include "_map.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "aircraft.h"
#include "airctype.h"
#include "app.h"
#include "assert.h"
#include "building.h"
#include "builtype.h"
#include "ccrand.h"
#include "cell.h"
#include "conquer.h"
#include "cpoint.h"
#include "data.h"
#include "dbgprint.h"
#include "ddist.h"
#include "dsurface.h"
#include "factory.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ion.h"
#include "ipowerevents.h"
#include "language\language.h"
#include "lightcon.h"
#include "logic.h"
#include "mono.h"
#include "mouse.h"
#include "overtype.h"
#include "quarry.h"
#include "revent.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "taskforc.h"
#include "team.h"
#include "techtype.h"
#include "tiberium.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "unit.h"
#include "unittype.h"
#include "vector.h"
#include "vein.h"
#include "voc.h"
#include "vox.h"
#include "waypoint.h"
#include "color.hh"
#include "strategy.hh"

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007EA758, DAT_007EA760;
extern double DAT_00A8EFA0, DAT_00A8F030, DAT_00A8F018, DAT_00A8F010, DAT_00A8EFE0;
extern double DAT_00A8EF58, DAT_00A8EFF0, DAT_00A8F008;
extern int DAT_00A8F038, DAT_00A8F02C, DAT_00A8EFF8, DAT_00A8EFFC, DAT_00A8F000;
extern short DAT_00A8EF98, DAT_00A8EF9A;
extern int DAT_00A8F020, DAT_00A8F024, DAT_00A8F028;

// FUNCTION: 0x004F4F00
void Init_004F4F00(void)
{
	DAT_00A8EFA0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x004F4F30
void Init_004F4F30(void)
{
	DAT_00A8F030 = DAT_00A8EFA0 / DAT_007EA758;
}

// FUNCTION: 0x004F4F50
void Init_004F4F50(void)
{
	DAT_00A8F018 = DAT_007E1718 / DAT_00A8F030;
}

// FUNCTION: 0x004F4F70
void Init_004F4F70(void)
{
	DAT_00A8F010 = DAT_007E1720 * DAT_007EA760;
}

// FUNCTION: 0x004F4F90
void Init_004F4F90(void)
{
	DAT_00A8EFE0 = DAT_007E1728 * DAT_007EA760;
}

// FUNCTION: 0x004F4FB0
void Init_004F4FB0(void)
{
	DAT_00A8EF58 = DAT_007E1730 * DAT_007EA760;
}

// FUNCTION: 0x004F4FD0
void Init_004F4FD0(void)
{
	DAT_00A8F038 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00A8EF58 - DAT_00A8EFE0) * DAT_00A8EFA0));
}

// FUNCTION: 0x004F5010
void Init_004F5010(void)
{
	DAT_00A8EFF0 = FUN_004CADE0(DAT_00A8F038 * DAT_007E1740);
}

// FUNCTION: 0x004F5040
void Init_004F5040(void)
{
	DAT_00A8F008 = FUN_004CADE0((DAT_00A8F038 * 2) / DAT_00A8EFA0);
}

// FUNCTION: 0x004F5070
void Init_004F5070(void)
{
	DAT_00A8F02C = (int)((DAT_00A8F038 * 4) + DAT_007E1738);
}

// FUNCTION: 0x004F50A0
void Init_004F50A0(void)
{
	DAT_00A8EF98 = 0;
	DAT_00A8EF9A = 0;
}

// FUNCTION: 0x004F50B0
void Init_004F50B0(void)
{
	DAT_00A8EFF8 = 0;
	DAT_00A8EFFC = 0;
	DAT_00A8F000 = 0;
}

// FUNCTION: 0x004F50D0
void Init_004F50D0(void)
{
	DAT_00A8F028 = 0;
	DAT_00A8F020 = 128;
	DAT_00A8F024 = 128;
}

// FUNCTION: 0x004F50F0 $E1

// FUNCTION: 0x004F5130 $E1
DynamicVectorClass<HouseClass::BuildChoiceClass *> HouseClass::BuildChoice;

// FUNCTION: 0x004F5190
HouseClass::HouseClass(NoInitClass const & x) :
	BASECLASS(x),
	Control(x),
	AlertTime(x),
	BorrowedTime(x),
	PowerBlackoutTimer(x),
	RadarBlackoutTimer(x),
	AircraftTotals(x),
	InfantryTotals(x),
	UnitTotals(x),
	BuildingTotals(x),
	DestroyedAircraft(x),
	DestroyedInfantry(x),
	DestroyedUnits(x),
	DestroyedBuildings(x),
	CapturedBuildings(x),
	TotalCrates(x),
	RemapColorRGB(x),
	LaserColor(x),
	Base(x),
	Tiberium(x),
	Weed(x),
	Attack(x),
	AITimer(x),
	PickEnemyTimer(x),
	DamageTime(x),
	TeamTime(x),
	TriggerTime(x),
	SpeakAttackDelay(x),
	SpeakPowerDelay(x),
	SpeakMoneyDelay(x),
	SpeakMaxedDelay(x),
	IniName(x)
{

}

inline UnitTypeClass const * First_Ownable(HouseClass const * house, TypeList<UnitTypeClass const *> const & list)
{
	int owners = 1 << HouseTypeClass::From_Name(house->Class->ParentCountry);
	for (int i = 0; i < list.Count(); i++) {
		if (owners & list[i]->Ownable) {
			return(list[i]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x004F6540
bool HouseClass::Can_Make_Money(void)
{
	bool hasdest = CountResourceDestinations > 0;
	bool hasgath = CountResourceGatherers > 0;
	bool haswf = CountWarfactories > 0;
	bool hascy = ConYards.Count() > 0;

	if (AUQuantity.Value(First_Ownable(this, Rule->BaseUnit)->HeapID)) {
		hascy = true;
	}

	if (hasdest && hasgath) {
		return(true);
	}

	int credits = Available_Money();

	bool canbuildref = FirstBuildableFromArray(Rule->BuildRefinery) != NULL;

	bool hasharvtype = First_Ownable(this, Rule->HarvesterUnit) != NULL;

	int harvcost;
	if (hasharvtype) {
		harvcost = First_Ownable(this, Rule->HarvesterUnit)->Cost_Of(this);
	} else {
		harvcost = 0x7FFFFFFF;
	}

	int wfcost = FirstBuildableFromArray(Rule->BuildWeapons)->Cost_Of(this);

	if (!canbuildref && hasharvtype) {
		if (hasgath) {
			return(true);
		}
		if (haswf && credits >= harvcost) {
			return(true);
		}
		if (hascy && credits >= wfcost + harvcost) {
			return(true);
		}
		return(false);
	}

	int refcost = FirstBuildableFromArray(Rule->BuildRefinery)->Cost_Of(this);

	if (canbuildref && !hasharvtype) {
		if (hasgath) {
			return(true);
		}
		if (hascy && credits >= refcost) {
			return(true);
		}
		return(false);
	}

	if (hasdest) {
		if ((haswf && credits >= harvcost) || (hascy && (credits >= wfcost + harvcost || credits >= refcost))) {
			return(true);
		}
		return(false);
	}

	if (hascy && credits > refcost) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004F6830
HRESULT STDMETHODCALLTYPE HouseClass::QueryInterface(REFIID riid, LPVOID * ppvObject)
{
	if (ppvObject == NULL) {
		return(E_POINTER);
	}

	*ppvObject = NULL;

	if (riid == IID_IUnknown) {
		*ppvObject = reinterpret_cast<IUnknown *>(this);
	}

	if (riid == IID_IHouse) {
		*ppvObject = static_cast<IHouse *>(this);
	}

	if (riid == IID_IPublicHouse) {
		*ppvObject = static_cast<IPublicHouse *>(this);
	}

	if (riid == IID_IConnectionPointContainer) {
		*ppvObject = static_cast<IConnectionPointContainer *>(this);
	}

	if (riid == IID_IPersist) {
		*ppvObject = static_cast<IPersist *>(this);
	}

	if (riid == IID_IPersistStream) {
		*ppvObject = static_cast<IPersistStream *>(this);
	}

	if (*ppvObject == 0) {
		return(E_NOINTERFACE);
	}

	this->AddRef();

	return(S_OK);
}

// FUNCTION: 0x004F6910
bool STDMETHODCALLTYPE HouseClass::Is_Powered(void)
{
	return(Power >= Drain);
}

// FUNCTION: 0x004F6930
IApplication * STDMETHODCALLTYPE HouseClass::Get_Application(void)
{
	IApplication *app;
	ApplicationInstance.QueryInterface(IID_IApplication, (void**)&app);
	return(app);
}

// FUNCTION: 0x004F6950
BSTR STDMETHODCALLTYPE HouseClass::Name(void)
{
	WCHAR psz[32];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)Class->GivenName, -1, psz, ARRAY_SIZE(psz));
	return(SysAllocString(psz));
}

// FUNCTION: 0x004F6990
LONG STDMETHODCALLTYPE HouseClass::Available_Money(void)
{
	float mult = Class->IncomeMult;
	int value = Tiberium.Get_Total_Value();
	return((LONG)(value * mult + Credits));
}

// FUNCTION: 0x004F69D0
LONG STDMETHODCALLTYPE HouseClass::Available_Storage(void)
{
	return(Capacity - (int)Tiberium.Get_Total_Amount());
}

// FUNCTION: 0x004F6A00
LONG STDMETHODCALLTYPE HouseClass::Power_Output(void)
{
	return(Power);
}

// FUNCTION: 0x004F6A10
LONG STDMETHODCALLTYPE HouseClass::Power_Drain(void)
{
	return(Drain);
}

// FUNCTION: 0x004F6E60
LONG STDMETHODCALLTYPE HouseClass::ID_Number(void)
{
	return(HeapID);
}

// FUNCTION: 0x004FA210
SUGGESTED_TEAM_LIST HouseClass::Suggested_New_Team(bool alertcheck)
{
	SUGGESTED_TEAM_LIST teams = TeamTypeClass::Suggested_New_Team(this, alertcheck);
	return(teams);
}

// FUNCTION: 0x004FD120
Coord HouseClass::Find_Build_Location(BuildingClass * building) const
{
	return(COORD_NONE);
}

// FUNCTION: 0x004FFD90
ZoneType HouseClass::Which_Zone(Cell const & cell) const
{
	return(Which_Zone((Coord)cell));
}

// FUNCTION: 0x00501AA0
Cell HouseClass::Find_Cell_In_Zone(TechnoClass const * techno, ZoneType zone) const
{
	return(Cell(0, 0));
}

// FUNCTION: 0x005046F0
HRESULT STDMETHODCALLTYPE HouseClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_HouseClass;
	return(S_OK);
}
