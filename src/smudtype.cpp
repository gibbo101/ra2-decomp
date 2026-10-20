// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "smudtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "ccini.h"
#include "ccrand.h"
#include "cell.h"
#include "draw.h"
#include "findmake.h"
#include "globals.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mouse.h"
#include "scenario.h"
#include "smudge.h"
#include "sun.h"
#include "tactical.h"
#include "tracker.h"

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
extern const double DAT_007F34E8;
extern const double DAT_007F34F0;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006B5070 $E133
double DAT_00B0B790 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x006B50A0 $E136
double DAT_00B0B7E0 = DAT_00B0B790 / DAT_007F34E8;

// FUNCTION: 0x006B50C0 $E139
double DAT_00B0B7C8 = DAT_007E1718 / DAT_00B0B7E0;

// FUNCTION: 0x006B50E0 $E142
double DAT_00B0B7C0 = DAT_007E1720 * DAT_007F34F0;

// FUNCTION: 0x006B5100 $E145
double DAT_00B0B798 = DAT_007E1728 * DAT_007F34F0;

// FUNCTION: 0x006B5120 $E148
double DAT_00B0B770 = DAT_007E1730 * DAT_007F34F0;

// FUNCTION: 0x006B5140 $E151
int DAT_00B0B7E8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0B770 - DAT_00B0B798) * DAT_00B0B790));

// FUNCTION: 0x006B5180 $E154
double DAT_00B0B7A0 = FUN_004CADE0(DAT_00B0B7E8 * DAT_007E1740);

// FUNCTION: 0x006B51B0 $E157
double DAT_00B0B7B8 = FUN_004CADE0((DAT_00B0B7E8 * 2) / DAT_00B0B790);

// FUNCTION: 0x006B51E0 $E160
int DAT_00B0B7DC = (int)(DAT_00B0B7E8 * 4 + DAT_007E1738);

// FUNCTION: 0x006B5210 $E163
ShortPair DAT_00B0B788;

// FUNCTION: 0x006B5220 $E166
IntTriple DAT_00B0B7A8;

// FUNCTION: 0x006B58B0
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x006B58D0
HRESULT STDMETHODCALLTYPE SmudgeTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_SmudgeTypeClass;
	return(S_OK);
}
