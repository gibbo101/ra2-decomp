// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "movie.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "gscreen.h"
#include "movies.h"
#include "session.h"
#include "vector.h"
#include "vqa.h"
#include "vq.hh"
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
extern const double DAT_007EE090;
extern const double DAT_007EE098;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005BEAD0 $E88
double DAT_00ABF300 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005BEB00 $E91
double DAT_00ABF350 = DAT_00ABF300 / DAT_007EE090;

// FUNCTION: 0x005BEB20 $E94
double DAT_00ABF338 = DAT_007E1718 / DAT_00ABF350;

// FUNCTION: 0x005BEB40 $E97
double DAT_00ABF330 = DAT_007E1720 * DAT_007EE098;

// FUNCTION: 0x005BEB60 $E100
double DAT_00ABF308 = DAT_007E1728 * DAT_007EE098;

// FUNCTION: 0x005BEB80 $E103
double DAT_00ABF2E0 = DAT_007E1730 * DAT_007EE098;

// FUNCTION: 0x005BEBA0 $E106
int DAT_00ABF358 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF2E0 - DAT_00ABF308) * DAT_00ABF300));

// FUNCTION: 0x005BEBE0 $E109
double DAT_00ABF310 = FUN_004CADE0(DAT_00ABF358 * DAT_007E1740);

// FUNCTION: 0x005BEC10 $E112
double DAT_00ABF328 = FUN_004CADE0((DAT_00ABF358 * 2) / DAT_00ABF300);

// FUNCTION: 0x005BEC40 $E115
int DAT_00ABF34C = (int)(DAT_00ABF358 * 4 + DAT_007E1738);

// FUNCTION: 0x005BEC70 $E118
ShortPair DAT_00ABF2F8;

// FUNCTION: 0x005BEC80 $E121
IntTriple DAT_00ABF318;

// FUNCTION: 0x005BECC0
void Rebuild_Interpolated_Palette(unsigned char * interpal)
{
	for (int y=0; y<SIZE_OF_PALETTE - 1; y++) {
		for (int x=y+1; x<SIZE_OF_PALETTE; x++) {
			*(interpal + (y*SIZE_OF_PALETTE+x)) = *(interpal + (x*SIZE_OF_PALETTE+y));
		}
	}
}

// FUNCTION: 0x005BF520
bool Has_Ingame_Movies(void)
{
	return(IngameVQ.Count() > 0);
}
