// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "anim.h"
#include "_bench.h"
#include "_convert.h"
#include "_map.h"
#include "_palette.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "animtype.h"
#include "bench.h"
#include "ccrand.h"
#include "cell.h"
#include "combat.h"
#include "conquer.h"
#include "crc.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "incdec.h"
#include "inline.h"
#include "lightcon.h"
#include "overlay.h"
#include "overtype.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "setpal.h"
#include "shapeset.h"
#include "smudtype.h"
#include "sun.h"
#include "swizzle.h"
#include "tactical.h"
#include "techno.h"
#include "tiberium.h"
#include "tracker.h"
#include "bench.hh"
#include "fastmath.h"
#include "coord.h"
#include <math.h>

extern const double DAT_007E3308;
extern const double DAT_007E3310;
extern double DAT_0089A158;
extern double DAT_0089A1B8;
extern double DAT_0089A1A0;
extern double DAT_0089A198;
extern double DAT_0089A168;
extern double DAT_0089A138;
extern int DAT_0089A1C0;
extern double DAT_0089A170;
extern double DAT_0089A188;
extern int DAT_0089A1B4;

// FUNCTION: 0x00421CB0



double DAT_0089A158 = fastmath::sqrt(pow((double)CELL_LEPTON_H, 2) * 2);

// FUNCTION: 0x00421CE0



double DAT_0089A1B8 = DAT_0089A158 / DAT_007E3308;

// FUNCTION: 0x00421D00



double DAT_0089A1A0 = 1.0 / DAT_0089A1B8;

// FUNCTION: 0x00421D20



double DAT_0089A198 = DAT_007E3310 * 45;

// FUNCTION: 0x00421D40



double DAT_0089A168 = DAT_007E3310 * 60;

// FUNCTION: 0x00421D60



double DAT_0089A138 = DAT_007E3310 * 90;

// FUNCTION: 0x00421D80



int DAT_0089A1C0 = (int)(fastmath::tan(DAT_0089A138 - DAT_0089A168) * DAT_0089A158 * 0.5);

// FUNCTION: 0x00421DC0



double DAT_0089A170 = fastmath::atan(DAT_0089A1C0 * (1 / (double)CELL_LEPTON_H));

// FUNCTION: 0x00421DF0



double DAT_0089A188 = fastmath::atan((DAT_0089A1C0 * 2) / DAT_0089A158);

// FUNCTION: 0x00421E20



int DAT_0089A1B4 = (int)((DAT_0089A1C0 * 4) + 0.5);

// FUNCTION: 0x00421E50



Cell const DAT_0089A150(0, 0);

// FUNCTION: 0x00421E60



Coord const DAT_0089A178(0, 0, 0);


// FUNCTION: 0x00422BE0
Coord AnimClass::Center_Coord(void) const
{
	if (OwnerObject != NULL) {
		return(OwnerObject->Center_Coord() + BASECLASS::Center_Coord());
	}
	return(BASECLASS::Center_Coord());
}

// FUNCTION: 0x004238B0
bool AnimClass::Mark(MarkType mark)
{
	if (BASECLASS::Mark(mark)) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004238D0
Cell const * AnimClass::Occupy_List(bool) const
{
	static Cell _simple[] = {REFRESH_EOL};

	return(_simple);
}

// FUNCTION: 0x00426540
HRESULT STDMETHODCALLTYPE AnimClass::GetClassID(CLSID * retval)
{
	if (retval == NULL) return(E_POINTER);
	*retval = CLSID_AnimClass;
	return(S_OK);
}
