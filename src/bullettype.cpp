// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "bullettype.h"
#include "_map.h"
#include "_rules.h"
#include "animtype.h"
#include "findmake.h"
#include "globals.h"
#include "incdec.h"
#include "rules.h"
#include "scheme.h"
#include "sun.h"
#include "swizzle.h"
#include "tracker.h"
#include "vector.h"
#include "weapon.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E4908;
extern const double DAT_007E4910;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0046B9D0 $E57
double DAT_0089DE98 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x0046BA00 $E60
double DAT_0089DEE8 = DAT_0089DE98 / DAT_007E4908;

// FUNCTION: 0x0046BA20 $E63
double DAT_0089DED0 = 1.0 / DAT_0089DEE8;

// FUNCTION: 0x0046BA40 $E66
double DAT_0089DEC8 = DAT_007E4910 * 45.0;

// FUNCTION: 0x0046BA60 $E69
double DAT_0089DEA0 = DAT_007E4910 * 60.0;

// FUNCTION: 0x0046BA80 $E72
double DAT_0089DE78 = DAT_007E4910 * 90.0;

// FUNCTION: 0x0046BAA0 $E75
int DAT_0089DEF0 = (int)(FUN_004CAD50(DAT_0089DE78 - DAT_0089DEA0) * DAT_0089DE98 * 0.5);

// FUNCTION: 0x0046BAE0 $E78
double DAT_0089DEA8 = FUN_004CADE0(DAT_0089DEF0 * 0.00390625);

// FUNCTION: 0x0046BB10 $E81
double DAT_0089DEC0 = FUN_004CADE0((DAT_0089DEF0 * 2) / DAT_0089DE98);

// FUNCTION: 0x0046BB40 $E84
int DAT_0089DEE4 = (int)(DAT_0089DEF0 * 4 + 0.5);

// FUNCTION: 0x0046BB70 $E87
ShortPair DAT_0089DE90;

// FUNCTION: 0x0046BB80 $E90
IntTriple DAT_0089DEB0;

// FUNCTION: 0x0046C4F0
Coord const BulletTypeClass::Coord_Fixup(Coord const & coord) const
{
	Coord c = coord;
	int height = Map.Get_Height_GL(c);
	int z = c.Z;
	if (z <= height) {
		z = Map.Get_Height_GL(c) + 1;
	}
	c.Z = z;
	return(c);
}

// FUNCTION: 0x0046C730
HRESULT STDMETHODCALLTYPE BulletTypeClass::Save(IStream * stream, BOOL cleardirty)
{
	HRESULT result = BASECLASS::Save(stream, cleardirty);
	if (SUCCEEDED(result)) {
		result = S_OK;
	}
	return(result);
}

// FUNCTION: 0x0046C750
HRESULT STDMETHODCALLTYPE BulletTypeClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_BulletTypeClass;
	return(S_OK);
}
