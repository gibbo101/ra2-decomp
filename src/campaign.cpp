// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "campaign.h"
#include "assert.h"
#include "ccini.h"
#include "crc.h"
#include "globals.h"
#include "noinit.h"
#include "sun.h"
#include "tracker.h"
#include "vector.h"

void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);
extern wchar_t DAT_00887734[];
extern char DAT_00889F64[];

// SYNTHETIC: GAMEMD 0x0046C970 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0046C9A0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0046C9C0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0046C9E0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0046CA00 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0046CA20 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0046CA40 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0046CA80 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0046CAB0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0046CAE0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0046CB10 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0046CB20 SYMBOL
// _$E35

// FUNCTION: GAMEMD 0x0046CB60
CampaignClass::CampaignClass(char const * name) :
	BASECLASS(name),
	CDNumber(-1),
	FinalMovie(VQ_NONE)
{
	Campaigns.Add(this);

	ScenarioName[0] = '\0';
	wcscpy(Description, DAT_00887734);
}

// FUNCTION: GAMEMD 0x0046CC10
CampaignClass::~CampaignClass(void)
{
	Detach_This_From_All(this, true);
	Campaigns.Delete(this);
}

// FUNCTION: GAMEMD 0x0046CC90
CampaignType CampaignClass::From_Name(char const * name)
{
	for (int index = 0; index < Campaigns.Count(); index++) {
		CampaignClass * ptr = Campaigns[index];
		if (stricmp(ptr->IniName, name) == 0) {
			return(CampaignType(index));
		}
	}
	return(CAMPAIGN_NONE);
}

// FUNCTION: GAMEMD 0x0046CCD0
bool CampaignClass::Read_INI(CCINIClass & ini)
{
	ini.Reset();
	if (BASECLASS::Read_INI(ini)) {
		CDNumber = ini.Get_Int(IniName, "CD", CDNumber);
		FinalMovie = ini.Get_VQType(IniName, "FinalMovie", FinalMovie);
		ini.Get_String(IniName, "Scenario", ScenarioName, ScenarioName, sizeof(ScenarioName));
		strupr(ScenarioName);
		if (!ini.Get_Bool(IniName, "DebugOnly", false)) {
			ini.ReadStringtableEntry(IniName, "Description", Description, sizeof(Description) / sizeof(Description[0]));
		} else {
			char buffer[1024];
			ini.Get_String(IniName, "Description", NULL, buffer, sizeof(buffer));
			strcat(buffer, " - for debug/testing");
			FUN_00735060(Description, buffer, -1);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0046CE10
void __fastcall Read_Battle_INI(CCINIClass & ini)
{
	char name[32];

	int count = ini.Entry_Count("Battles");

	for (int index = 0; index < count; index++) {

		ini.Get_String("Battles", ini.Get_Entry("Battles", index), DAT_00889F64, name, sizeof(name));

		CampaignType ctype = CampaignClass::From_Name(name);
		CampaignClass * campaign;

		if (ctype == CAMPAIGN_NONE) {
			campaign = new CampaignClass(name);
		} else {
			campaign = Campaigns[ctype];
		}

		campaign->Read_INI(ini);
	}
}

// FUNCTION: GAMEMD 0x0046CF80
HRESULT STDMETHODCALLTYPE CampaignClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_CampaignClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x0046CFC0
void CampaignClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);

	crc(CDNumber);
	crc(ScenarioName);
	crc(FinalMovie);
}

// FUNCTION: GAMEMD 0x0046D000
HRESULT STDMETHODCALLTYPE CampaignClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) CampaignClass(NoInitClass());

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0046D050
HRESULT STDMETHODCALLTYPE CampaignClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x0046D070
int CampaignClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: GAMEMD 0x0046D080
RTTIType CampaignClass::Fetch_RTTI(void) const
{
	return(RTTI_CAMPAIGN);
}

// SYNTHETIC: GAMEMD 0x0046D090
// ??_GCampaignClass@@UAEPAXI@Z
