// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define INCLUDE_COM
#include "always.h"
#include "saveload.h"
#include "_logic.h"
#include "_map.h"
#include "_rect.h"
#include "_rules.h"
#include "_script.h"
#include "_tactica.h"
#include "_vanim.h"
#include "_warhead.h"
#include "_weapon.h"
#include "aircraft.h"
#include "airctype.h"
#include "aitrig.h"
#include "alphashp.h"
#include "anim.h"
#include "animtype.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "dbgprint.h"
#include "empulse.h"
#include "enviro.h"
#include "factory.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "ilinkstm.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "ion.h"
#include "loaddlg.h"
#include "light.h"
#include "logic.h"
#include "overlay.h"
#include "overtype.h"
#include "ovrlight.h"
#include "particle.h"
#include "partsys.h"
#include "psystype.h"
#include "ptype.h"
#include "revent.h"
#include "rules.h"
#include "savever.h"
#include "scenario.h"
#include "script.h"
#include "session.h"
#include "side.h"
#include "sidebar.h"
#include "smudtype.h"
#include "sun.h"
#include "super.h"
#include "suprtype.h"
#include "swizzle.h"
#include "tactical.h"
#include "taction.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "team.h"
#include "teamtype.h"
#include "terrain.h"
#include "terrtype.h"
#include "tevent.h"
#include "tiberium.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vanim.h"
#include "vanimtype.h"
#include "vein.h"
#include "vox.h"
#include "warhead.h"
#include "wave.h"
#include "waypoint.h"
#include "weapon.h"
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
extern const double DAT_007F0E28;
extern const double DAT_007F0E30;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0067C4A0 $E156
double DAT_00B053E0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0067C4D0 $E159
double DAT_00B05430 = DAT_00B053E0 / DAT_007F0E28;

// FUNCTION: 0x0067C4F0 $E162
double DAT_00B05418 = DAT_007E1718 / DAT_00B05430;

// FUNCTION: 0x0067C510 $E165
double DAT_00B05410 = DAT_007E1720 * DAT_007F0E30;

// FUNCTION: 0x0067C530 $E168
double DAT_00B053E8 = DAT_007E1728 * DAT_007F0E30;

// FUNCTION: 0x0067C550 $E171
double DAT_00B053C0 = DAT_007E1730 * DAT_007F0E30;

// FUNCTION: 0x0067C570 $E174
int DAT_00B05438 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B053C0 - DAT_00B053E8) * DAT_00B053E0));

// FUNCTION: 0x0067C5B0 $E177
double DAT_00B053F0 = FUN_004CADE0(DAT_00B05438 * DAT_007E1740);

// FUNCTION: 0x0067C5E0 $E180
double DAT_00B05408 = FUN_004CADE0((DAT_00B05438 * 2) / DAT_00B053E0);

// FUNCTION: 0x0067C610 $E183
int DAT_00B0542C = (int)(DAT_00B05438 * 4 + DAT_007E1738);

// FUNCTION: 0x0067C640 $E186
ShortPair DAT_00B053D8;

// FUNCTION: 0x0067C650 $E189
IntTriple DAT_00B053F8;

#define	SAVE_BLOCK_SIZE	4096
#define	SAVEGAME_VERSION		(DESCRIP_MAX + 0x01000006)

// FUNCTION: 0x0067FD20
bool Get_Savefile_Info(char const * name, SaveVersionInfo * info)
{
	IStoragePtr storage;
	WCHAR wname[64];

	MultiByteToWideChar(0, 0, name, -1, wname, sizeof(wname) / sizeof(WCHAR));

	HRESULT result = StgOpenStorage(wname, NULL, STGM_SHARE_EXCLUSIVE|STGM_READWRITE, NULL, 0, &storage);
	if (FAILED(result)) {
		return(false);
	}

	result = info->Load(storage);
	if (FAILED(result)) {
		return(false);
	}

	return(true);
}
