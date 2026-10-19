// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "vein.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "astar.h"
#include "ccrand.h"
#include "cell.h"
#include "draw.h"
#include "globals.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "isotype.h"
#include "lightcon.h"
#include "mixfile.h"
#include "mouse.h"
#include "overtype.h"
#include "particle.h"
#include "partsys.h"
#include "ptype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "stimer.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "terrtype.h"
#include "tracker.h"
#include "vector.h"
#include "ramp.hh"
#include <cstdio>
#include <math.h>

static bool * GlobalGrowthState = NULL;

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
extern const double DAT_007F6610;
extern const double DAT_007F6618;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0074C190 $E140
double DAT_00B1D278 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0074C1C0 $E143
double DAT_00B1D2E0 = DAT_00B1D278 / DAT_007F6610;

// FUNCTION: 0x0074C1E0 $E146
double DAT_00B1D2C8 = DAT_007E1718 / DAT_00B1D2E0;

// FUNCTION: 0x0074C200 $E149
double DAT_00B1D2C0 = DAT_007E1720 * DAT_007F6618;

// FUNCTION: 0x0074C220 $E152
double DAT_00B1D280 = DAT_007E1728 * DAT_007F6618;

// FUNCTION: 0x0074C240 $E155
double DAT_00B1D258 = DAT_007E1730 * DAT_007F6618;

// FUNCTION: 0x0074C260 $E158
int DAT_00B1D2E8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B1D258 - DAT_00B1D280) * DAT_00B1D278));

// FUNCTION: 0x0074C2A0 $E161
double DAT_00B1D288 = FUN_004CADE0(DAT_00B1D2E8 * DAT_007E1740);

// FUNCTION: 0x0074C2D0 $E164
double DAT_00B1D2B8 = FUN_004CADE0((DAT_00B1D2E8 * 2) / DAT_00B1D278);

// FUNCTION: 0x0074C300 $E167
int DAT_00B1D2DC = (int)(DAT_00B1D2E8 * 4 + DAT_007E1738);

// FUNCTION: 0x0074C330 $E170
ShortPair DAT_00B1D270;

// FUNCTION: 0x0074C340 $E173
IntTriple DAT_00B1D2A8;


// FUNCTION: 0x0074D430
void VeinholeMonsterClass::Draw_All(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		VeinholeMonsters[i]->Draw_It();
	}
}

// FUNCTION: 0x0074D760
void VeinholeMonsterClass::Reset(void)
{
	for (int i = VeinholeMonsters.Count() - 1; i >= 0; i--) {
		delete VeinholeMonsters[i];
	}
	VeinholeMonsters.Clear();
}

// FUNCTION: 0x0074E880
void VeinholeMonsterClass::Clear_Global_Data(void)
{
	if (GlobalGrowthState) {
		delete [] GlobalGrowthState;
		GlobalGrowthState = NULL;
	}
}

// FUNCTION: 0x0074EEE0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::Save(IStream * stream, BOOL cleardirty)
{
	return(BASECLASS::Save(stream, cleardirty));
}

// FUNCTION: 0x0074F2D0
HRESULT STDMETHODCALLTYPE VeinholeMonsterClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_VeinholeMonsterClass;
	return(S_OK);
}
