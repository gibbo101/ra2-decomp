// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
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

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007EAB18, DAT_007EAB20;
extern double DAT_00A8F0E8, DAT_00A8F138, DAT_00A8F120, DAT_00A8F118, DAT_00A8F0F0;
extern double DAT_00A8F0C8, DAT_00A8F0F8, DAT_00A8F110;
extern int DAT_00A8F140, DAT_00A8F134, DAT_00A8F100, DAT_00A8F104, DAT_00A8F108;
extern short DAT_00A8F0E0, DAT_00A8F0E2;

// FUNCTION: 0x00511200
void Init_00511200(void)
{
	DAT_00A8F0E8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x00511230
void Init_00511230(void)
{
	DAT_00A8F138 = DAT_00A8F0E8 / DAT_007EAB18;
}

// FUNCTION: 0x00511250
void Init_00511250(void)
{
	DAT_00A8F120 = DAT_007E1718 / DAT_00A8F138;
}

// FUNCTION: 0x00511270
void Init_00511270(void)
{
	DAT_00A8F118 = DAT_007E1720 * DAT_007EAB20;
}

// FUNCTION: 0x00511290
void Init_00511290(void)
{
	DAT_00A8F0F0 = DAT_007E1728 * DAT_007EAB20;
}

// FUNCTION: 0x005112B0
void Init_005112B0(void)
{
	DAT_00A8F0C8 = DAT_007E1730 * DAT_007EAB20;
}

// FUNCTION: 0x005112D0
void Init_005112D0(void)
{
	DAT_00A8F140 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00A8F0C8 - DAT_00A8F0F0) * DAT_00A8F0E8));
}

// FUNCTION: 0x00511310
void Init_00511310(void)
{
	DAT_00A8F0F8 = FUN_004CADE0(DAT_00A8F140 * DAT_007E1740);
}

// FUNCTION: 0x00511340
void Init_00511340(void)
{
	DAT_00A8F110 = FUN_004CADE0((DAT_00A8F140 * 2) / DAT_00A8F0E8);
}

// FUNCTION: 0x00511370
void Init_00511370(void)
{
	DAT_00A8F134 = (int)((DAT_00A8F140 * 4) + DAT_007E1738);
}

// FUNCTION: 0x005113A0
void Init_005113A0(void)
{
	DAT_00A8F0E0 = 0;
	DAT_00A8F0E2 = 0;
}

// FUNCTION: 0x005113B0
void Init_005113B0(void)
{
	DAT_00A8F100 = 0;
	DAT_00A8F104 = 0;
	DAT_00A8F108 = 0;
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
