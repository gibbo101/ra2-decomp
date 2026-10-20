// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "preview.h"
#include "_map.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccfile.h"
#include "ccini.h"
#include "cell.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "language\language.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "overtype.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "scenario.h"
#include "surface.h"
#include "tactical.h"
#include "terrain.h"
#include "windlg.h"
#include "xpipe.h"
#include "xstraw.h"
#include "color.hh"

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
extern const double DAT_007F0030;
extern const double DAT_007F0038;

// FUNCTION: 0x006404F0 $E83
double DAT_00AC4EB0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00640520 $E86
double DAT_00AC4F00 = DAT_00AC4EB0 / DAT_007F0030;

// FUNCTION: 0x00640540 $E89
double DAT_00AC4EE8 = DAT_007E1718 / DAT_00AC4F00;

// FUNCTION: 0x00640560 $E92
double DAT_00AC4EE0 = DAT_007E1720 * DAT_007F0038;

// FUNCTION: 0x00640580 $E95
double DAT_00AC4EB8 = DAT_007E1728 * DAT_007F0038;

// FUNCTION: 0x006405A0 $E98
double DAT_00AC4E88 = DAT_007E1730 * DAT_007F0038;

// FUNCTION: 0x006405C0 $E101
int DAT_00AC4F08 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00AC4E88 - DAT_00AC4EB8) * DAT_00AC4EB0));

// FUNCTION: 0x00640600 $E104
double DAT_00AC4EC0 = FUN_004CADE0(DAT_00AC4F08 * DAT_007E1740);

// FUNCTION: 0x00640630 $E107
double DAT_00AC4ED8 = FUN_004CADE0((DAT_00AC4F08 * 2) / DAT_00AC4EB0);

// FUNCTION: 0x00640660 $E110
int DAT_00AC4EFC = (int)(DAT_00AC4F08 * 4 + DAT_007E1738);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00640690 $E113
ShortPair DAT_00AC4EA8;

// FUNCTION: 0x006406A0 $E116
IntTriple DAT_00AC4EC8;

// FUNCTION: 0x006406E0
MapPreviewClass::MapPreviewClass(void) :
	SurfacePtr(NULL)
{
}

// FUNCTION: 0x006406F0
MapPreviewClass::~MapPreviewClass(void)
{
	if (SurfacePtr != NULL) {
		delete SurfacePtr;
		SurfacePtr = NULL;
	}
}
