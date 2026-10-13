// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "ramfile.h"
#include "radsite.h"
#include "cell.h"
#include "crc.h"
#include "map.h"
#include "swizzle.h"
#include "tracker.h"
#include <cstring>

DynamicVectorClass<RadSiteClass *> RadSites;

// FUNCTION: 0x0065B160 $E49
// FUNCTION: 0x0065B1A0 $E47

// FUNCTION: 0x0065B1E0
RadSiteClass::RadSiteClass(void) :
	BASECLASS(),
	LightSource(NULL),
	RadLevelTimer(),
	RadLightTimer(),
	BaseCell(0, 0),
	Spread(0),
	SpreadInLeptons(0),
	RadLevel(0),
	LevelSteps(0),
	Intensity(0),
	IntensitySteps(0),
	IntensityDecrement(0),
	RadDuration(0),
	RadTimeLeft(0)
{
	RadSites.Add(this);
}

// FUNCTION: 0x0065B2A0
RadSiteClass::RadSiteClass(NoInitClass const & x) :
	BASECLASS(x),
	RadLevelTimer(),
	RadLightTimer()
{
}

// FUNCTION: 0x0065B2F0
RadSiteClass::~RadSiteClass(void)
{
	CellClass & cell = Map[BaseCell];
	cell.FUN_00487C70(NULL);
	Detach_This_From_All(this, true);
	if (LightSource != NULL) {
		delete LightSource;
		LightSource = NULL;
	}
	RadSites.Delete(this);
}

// FUNCTION: 0x0065B3A0
int RadSiteClass::Fetch_Object_Size(void) const
{
	return(sizeof(*this));
}

// FUNCTION: 0x0065B3B0
void RadSiteClass::Compute_CRC(CRCEngine & crc) const
{
	BASECLASS::Compute_CRC(crc);
}

// FUNCTION: 0x0065B3C0
RTTIType RadSiteClass::Fetch_RTTI(void) const
{
	return(RTTI_RADSITE);
}

// FUNCTION: 0x0065B3D0
HRESULT STDMETHODCALLTYPE RadSiteClass::Load(IStream * stream)
{
	HRESULT result = BASECLASS::Load(stream);
	if (SUCCEEDED(result)) {
		new (this) RadSiteClass(NoInitClass());
		Swizzle_Pointer(&LightSource);
	}
	return(result);
}

// FUNCTION: 0x0065B450
HRESULT STDMETHODCALLTYPE RadSiteClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0065B470
HRESULT STDMETHODCALLTYPE RadSiteClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) {
		return(E_POINTER);
	}
	*retval = CLSID_RadSiteClass;
	return(S_OK);
}

// FUNCTION: 0x0065B4B0
int RadSiteClass::GetSpread(void)
{
	return(Spread);
}

// FUNCTION: 0x0065C340
int RAMFileClass::Open(char const *, int access)
{
	return(Open(access));
}
