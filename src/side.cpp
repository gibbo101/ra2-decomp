// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "_rules.h"
#include "globals.h"
#include "house.h"
#include "rules.h"
#include "side.h"
#include "crc.h"
#include "tracker.h"

// SYNTHETIC: GAMEMD 0x006A4360 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x006A4390 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x006A43B0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x006A43D0 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x006A43F0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006A4410 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006A4430 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006A4470 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006A44A0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006A44D0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006A4500 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006A4510 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006A4530 SYMBOL
// _$E38

DynamicVectorClass<SideClass *> SideClass::Array;

// FUNCTION: GAMEMD 0x006A4550
SideClass::SideClass(char const * ininame) :
	BASECLASS(ininame),
	Houses()
{
	Create_ID();
	Array.Add(this);
}

// FUNCTION: GAMEMD 0x006A4610
// FUNCTION: GAMEMD 0x006A4930
SideClass::~SideClass(void)
{
	Detach_This_From_All(this);
	Array.Delete(this);
}

// FUNCTION: GAMEMD 0x006A46D0
SideType SideClass::From_Name(char const * name)
{
	for (int classid = 0; classid < Array.Count(); classid++) {
		if (stricmp(Array[classid]->Name(), name) == 0) {
			return((SideType)classid);
		}
	}

	return(SIDE_NONE);
}

// FUNCTION: GAMEMD 0x006A4710
void SideClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
	crc(Houses.Count());
}

// FUNCTION: GAMEMD 0x006A4740
HRESULT STDMETHODCALLTYPE SideClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SideClass;
	return(S_OK);
}

// FUNCTION: GAMEMD 0x006A4780
HRESULT STDMETHODCALLTYPE SideClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) SideClass(NoInitClass());

		Houses.Load(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006A48A0
HRESULT STDMETHODCALLTYPE SideClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		Houses.Save(stream);

		result = S_OK;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x006A4910
// SideClass::Fetch_Object_Size

// FUNCTION: GAMEMD 0x006A4920
// SideClass::Fetch_RTTI
