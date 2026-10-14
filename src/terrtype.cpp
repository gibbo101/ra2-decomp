// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "terrtype.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rules.h"
#include "_theater.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "mixfile.h"
#include "rules.h"
#include "shapeset.h"
#include "sun.h"
#include "terrain.h"
#include "terrtype.h"
#include "tracker.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007F5418;
extern const double DAT_007F5420;

// FUNCTION: 0x0071D390 $E59
double DAT_00B0ED80 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x0071D3C0 $E62
double DAT_00B0F140 = DAT_00B0ED80 / DAT_007F5418;

// FUNCTION: 0x0071D3E0 $E65
double DAT_00B0EDB8 = 1.0 / DAT_00B0F140;

// FUNCTION: 0x0071D400 $E68
double DAT_00B0EDB0 = DAT_007F5420 * 45.0;

// FUNCTION: 0x0071D420 $E71
double DAT_00B0ED88 = DAT_007F5420 * 60.0;

// FUNCTION: 0x0071D440 $E74
double DAT_00B0ED58 = DAT_007F5420 * 90.0;

// FUNCTION: 0x0071D460 $E77
int DAT_00B0F148 = (int)(FUN_004CAD50(DAT_00B0ED58 - DAT_00B0ED88) * DAT_00B0ED80 * 0.5);

// FUNCTION: 0x0071D4A0 $E80
double DAT_00B0ED90 = FUN_004CADE0(DAT_00B0F148 * 0.00390625);

// FUNCTION: 0x0071D4D0 $E83
double DAT_00B0EDA8 = FUN_004CADE0((DAT_00B0F148 * 2) / DAT_00B0ED80);

// FUNCTION: 0x0071D500 $E86
int DAT_00B0F13C = (int)(DAT_00B0F148 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0071D530 $E89
ShortPair DAT_00B0ED7C;

// FUNCTION: 0x0071D540 $E92
IntTriple DAT_00B0ED98;

// FUNCTION: 0x0071E0D0
Coord const TerrainTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord tmp = coord;
	if (tmp.Z < Map.Get_Height_GL(tmp)) {
		tmp.Z = Map.Get_Height_GL(tmp);
	}
	return(tmp);
}

// FUNCTION: 0x0071E240
HRESULT STDMETHODCALLTYPE TerrainTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0071E260
HRESULT STDMETHODCALLTYPE TerrainTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_TerrainTypeClass;
	return(S_OK);
}
