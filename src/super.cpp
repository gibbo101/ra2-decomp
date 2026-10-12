// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "super.h"
#include "_map.h"
#include "_rules.h"
#include "_weapon.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "ccrand.h"
#include "cell.h"
#include "crc.h"
#include "data.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "inline.h"
#include "ionblast.h"
#include "language\language.h"
#include "mouse.h"
#include "rules.h"
#include "sun.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tracker.h"
#include "unit.h"
#include "vector.h"
#include "vox.h"
#include "weapon.h"

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007F3FA8, DAT_007F3FB0;
extern double DAT_00B0C008, DAT_00B0C080, DAT_00B0C068, DAT_00B0C060, DAT_00B0C010;
extern double DAT_00B0BFE8, DAT_00B0C018, DAT_00B0C030;
extern int DAT_00B0C088, DAT_00B0C07C, DAT_00B0C020, DAT_00B0C024, DAT_00B0C028;
extern short DAT_00B0C000, DAT_00B0C002;

// FUNCTION: 0x006CAC10
void Init_006CAC10(void)
{
	DAT_00B0C008 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x006CAC40
void Init_006CAC40(void)
{
	DAT_00B0C080 = DAT_00B0C008 / DAT_007F3FA8;
}

// FUNCTION: 0x006CAC60
void Init_006CAC60(void)
{
	DAT_00B0C068 = DAT_007E1718 / DAT_00B0C080;
}

// FUNCTION: 0x006CAC80
void Init_006CAC80(void)
{
	DAT_00B0C060 = DAT_007E1720 * DAT_007F3FB0;
}

// FUNCTION: 0x006CACA0
void Init_006CACA0(void)
{
	DAT_00B0C010 = DAT_007E1728 * DAT_007F3FB0;
}

// FUNCTION: 0x006CACC0
void Init_006CACC0(void)
{
	DAT_00B0BFE8 = DAT_007E1730 * DAT_007F3FB0;
}

// FUNCTION: 0x006CACE0
void Init_006CACE0(void)
{
	DAT_00B0C088 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0BFE8 - DAT_00B0C010) * DAT_00B0C008));
}

// FUNCTION: 0x006CAD20
void Init_006CAD20(void)
{
	DAT_00B0C018 = FUN_004CADE0(DAT_00B0C088 * DAT_007E1740);
}

// FUNCTION: 0x006CAD50
void Init_006CAD50(void)
{
	DAT_00B0C030 = FUN_004CADE0((DAT_00B0C088 * 2) / DAT_00B0C008);
}

// FUNCTION: 0x006CAD80
void Init_006CAD80(void)
{
	DAT_00B0C07C = (int)((DAT_00B0C088 * 4) + DAT_007E1738);
}

// FUNCTION: 0x006CADB0
void Init_006CADB0(void)
{
	DAT_00B0C000 = 0;
	DAT_00B0C002 = 0;
}

// FUNCTION: 0x006CADC0
void Init_006CADC0(void)
{
	DAT_00B0C020 = 0;
	DAT_00B0C024 = 0;
	DAT_00B0C028 = 0;
}

// FUNCTION: 0x006CDEB0
HRESULT STDMETHODCALLTYPE SuperClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SuperWeaponClass;
	return(S_OK);
}

// FUNCTION: 0x006CDFD0
HRESULT STDMETHODCALLTYPE SuperClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
