// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "aitrig.h"
#include "_rules.h"
#include "airctype.h"
#include "builtype.h"
#include "crc.h"
#include "findmake.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "infatype.h"
#include "rules.h"
#include "scenario.h"
#include "session.h"
#include "sun.h"
#include "swizzle.h"
#include "taskforc.h"
#include "techtype.h"
#include "trim.h"
#include "unittype.h"
#include "vector.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E2A08;
extern const double DAT_007E2A10;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0041E160 $E51
double DAT_0088A020 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0041E190 $E54
double DAT_0088A070 = DAT_0088A020 / DAT_007E2A08;

// FUNCTION: 0x0041E1B0 $E57
double DAT_0088A058 = DAT_007E1718 / DAT_0088A070;

// FUNCTION: 0x0041E1D0 $E60
double DAT_0088A050 = DAT_007E1720 * DAT_007E2A10;

// FUNCTION: 0x0041E1F0 $E63
double DAT_0088A028 = DAT_007E1728 * DAT_007E2A10;

// FUNCTION: 0x0041E210 $E66
double DAT_0088A000 = DAT_007E1730 * DAT_007E2A10;

// FUNCTION: 0x0041E230 $E69
int DAT_0088A078 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0088A000 - DAT_0088A028) * DAT_0088A020));

// FUNCTION: 0x0041E270 $E72
double DAT_0088A030 = FUN_004CADE0(DAT_0088A078 * DAT_007E1740);

// FUNCTION: 0x0041E2A0 $E75
double DAT_0088A048 = FUN_004CADE0((DAT_0088A078 * 2) / DAT_0088A020);

// FUNCTION: 0x0041E2D0 $E78
int DAT_0088A06C = (int)(DAT_0088A078 * 4 + DAT_007E1738);

// FUNCTION: 0x0041E300 $E81
ShortPair DAT_0088A018;

// FUNCTION: 0x0041E310 $E84
IntTriple DAT_0088A038;

// FUNCTION: 0x0041E500
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AITriggerTypeClass;
	return(S_OK);
}

// FUNCTION: 0x0041E5C0
HRESULT STDMETHODCALLTYPE AITriggerTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}
