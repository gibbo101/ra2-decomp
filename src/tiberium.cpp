// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "tiberium.h"
#include "_map.h"
#include "animtype.h"
#include "astar.h"
#include "ccini.h"
#include "cell.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "overtype.h"
#include "priority.h"
#include "scenario.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#define MAX_SPREAD_DELAY	50
#define MAX_GROWTH_DELAY	50
#define MAX_GROWTH_STAGE	11

DynamicVectorClass<TiberiumClass *> Tiberiums;

// FUNCTION: 0x00721640 $E58
// FUNCTION: 0x00721680 $E56

// FUNCTION: 0x007216C0
TiberiumClass::TiberiumClass(char const * ininame) :
	BASECLASS(ininame),
	HeapID(TIBERIUM_NONE),
	SpreadPercentage(0.1),
	GrowthPercentage(0.1),
	SpreadDelay(0),
	GrowthDelay(0),
	CreditValue(0),
	Power(0),
	Color(0),
	Debris(),
	Overlay(NULL),
	FrameCount(0),
	Variety(0),
	RampVariety(0),
	SpreadCount(0),
	SpreadQueue(NULL),
	SpreadState(NULL),
	SpreadNodes(NULL),
	SpreadTimer(),
	GrowthCount(0),
	GrowthQueue(NULL),
	GrowthState(NULL),
	GrowthNodes(NULL),
	GrowthTimer()
{
	HeapID = (TiberiumType)Tiberiums.Count();
	Tiberiums.Add(this);
	Debris.Clear();
	AbstractTypePtrTracker.Add(this);
}

// FUNCTION: 0x00721880
TiberiumClass::~TiberiumClass(void)
{
	Detach_This_From_All(this);
	AbstractTypePtrTracker.Delete(this);
	Tiberiums.Delete(this);

	delete [] SpreadNodes;
	SpreadNodes = NULL;

	delete [] SpreadState;
	SpreadState = NULL;

	if (SpreadQueue) {
		delete SpreadQueue;
	}
	SpreadQueue = NULL;

	if (GrowthQueue) {
		GrowthQueue->Clear();
		delete GrowthQueue;
		GrowthQueue = NULL;
	}

	if (GrowthNodes) {
		delete [] GrowthNodes;
		GrowthNodes = NULL;
	}

	if (GrowthState) {
		delete [] GrowthState;
		GrowthState = NULL;
	}

	GrowthCount = 0;
}

static TypeList<AnimTypeClass const *> Get_Debris(CCINIClass const & ini, char const * section, TypeList<AnimTypeClass const *> const & defvalue)
{
	char buffer[128];

	if (ini.Get_String(section, "Debris", "", buffer, sizeof(buffer)) != 0) {
		TypeList<AnimTypeClass const *> list;

		const char * token = strtok(buffer, ",");

		while (token != NULL && token[0] != '\0') {
			AnimTypeClass const * ptr = AnimTypeClass::Find_Or_Make(token);
			if (ptr) {
				list.Add(ptr);
			}
			token = strtok(NULL, ",");
		}
		return(list);
	}

	return(defvalue);
}

// FUNCTION: 0x00721A50
bool TiberiumClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {

		SpreadDelay = ini.Get_Int(IniName, "Spread", SpreadDelay);
		SpreadPercentage = ini.Get_Float(IniName, "SpreadPercentage", SpreadPercentage);
		GrowthDelay = ini.Get_Int(IniName, "Growth", GrowthDelay);
		GrowthPercentage = ini.Get_Float(IniName, "GrowthPercentage", GrowthPercentage);
		CreditValue = ini.Get_Int(IniName, "Value", CreditValue);
		Power = ini.Get_Int(IniName, "Power", Power);
		Color = ini.Get_Scheme_Index(IniName, "Color", Color);
		Debris = Get_Debris(ini, IniName, Debris);

		switch (ini.Get_Int(IniName, "Image", -1)) {
			case -1:
				break;

			case 2:
				Overlay = OverlayTypes[OVERLAY_LARGE_TIBERIUM01];
				Variety = FrameCount = 12;
				return(true);

			case 3:
				Overlay = OverlayTypes[OVERLAY_TIBERIUM2_01];
				goto ramp;

			case 4:
				Overlay = OverlayTypes[OVERLAY_TIBERIUM3_01];
				goto ramp;

			case 0:
			case 1:
			default:
				Overlay = OverlayTypes[OVERLAY_TIBERIUM01];
			ramp:
				FrameCount = 12;
				RampVariety = 8;
				Variety = 12;
				break;
		}

		return(true);
	}
	return(false);
}

// FUNCTION: 0x00721D10
bool TiberiumClass::Process(CCINIClass & ini)
{
	char buffer[24];
	int count = ini.Entry_Count("Tiberiums");
	for (int i = 0; i < count; i++) {
		char const * index = ini.Get_Entry("Tiberiums", i);
		if (ini.Get_String("Tiberiums", index, "", buffer, sizeof(buffer)) > 0) {
			int tibindex = atoi(index);
			TiberiumClass * tib;
			if (tibindex < Tiberiums.Count()) {
				tib = Tiberiums[tibindex];
			} else {
				tib = new TiberiumClass(buffer);
			}
			tib->Read_INI(ini);
		}
	}
	return(true);
}

// FUNCTION: 0x00721DC0
void TiberiumClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(SpreadDelay);
	crc(GrowthDelay);
	crc(CreditValue);
	crc(Power);
	crc(Color);
	crc(FrameCount);
	crc(Variety);
}

// FUNCTION: 0x00721E40
HRESULT STDMETHODCALLTYPE TiberiumClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TiberiumClass;
	return(S_OK);
}

void TiberiumClass::Clear_Spread(void)
{
	if (SpreadQueue) {
		SpreadQueue->Clear();
		delete SpreadQueue;
		SpreadQueue = NULL;
	}

	if (SpreadNodes) {
		delete SpreadNodes;
		SpreadNodes = NULL;
	}

	if (SpreadState) {
		delete SpreadState;
		SpreadState = NULL;
	}

	SpreadCount = 0;
}

void TiberiumClass::Clear_Growth(void)
{
	if (GrowthQueue) {
		GrowthQueue->Clear();
		delete GrowthQueue;
		GrowthQueue = NULL;
	}

	if (GrowthNodes) {
		delete GrowthNodes;
		GrowthNodes = NULL;
	}

	if (GrowthState) {
		delete GrowthState;
		GrowthState = NULL;
	}

	GrowthCount = 0;
}

// FUNCTION: 0x00721E80
HRESULT STDMETHODCALLTYPE TiberiumClass::Load(IStream * stream)
{
	Clear_Spread();
	Clear_Growth();
	Debris.Clear();

	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) TiberiumClass(NoInitClass());

		Swizzle_Pointer(&Overlay);

		Debris.Load(stream);
		for (int i = 0; i < Debris.Count(); i++) {
			Swizzle_Pointer(&Debris[i]);
		}

		GrowthQueue = NULL;
		GrowthState = NULL;
		GrowthNodes = NULL;
		SpreadQueue = NULL;
		SpreadState = NULL;
		SpreadNodes = NULL;

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x007220A0
HRESULT STDMETHODCALLTYPE TiberiumClass::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	HRESULT res = BASECLASS::GetSizeMax(pcbSize);
	if (SUCCEEDED(res)) {
		pcbSize->u.LowPart += sizeof(int) + (Debris.Count() * sizeof(Debris[0]));
		res = S_OK;
	}
	return(res);
}

// FUNCTION: 0x007220D0
HRESULT STDMETHODCALLTYPE TiberiumClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		Debris.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x00722140
void TiberiumClass::Detach(AbstractClass const * target, bool all)
{
	for (int i = Debris.Count() -1; i >= 0; i--) {
		if (Debris[i] == target) {
			Debris.Delete_Index(i);
		}
	}

	if (Overlay == target) {
		Overlay = NULL;
	}
}

// FUNCTION: 0x007221B0
void TiberiumClass::Tiberium_Spread(void)
{
	if (Scen->IsTibGrowth) {
		for (int i = 0; i < Tiberiums.Count(); i++) {
			if (!Tiberiums[i]->SpreadTimer) {
				Tiberiums[i]->Spread_AI();
				Tiberiums[i]->SpreadTimer = Tiberiums[i]->SpreadDelay;
			}
		}
	}
}

