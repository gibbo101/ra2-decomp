// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "session.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "addon.h"
#include "ccini.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "phone.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "special.h"
#include "wonline.h"
#include "xstraw.h"
#include <ctime> // for station ID computation
#include <dos.h> // for station ID computation
#include <math.h>

extern double DAT_007E1708, DAT_007E1710, DAT_007E1718, DAT_007E1720, DAT_007E1728;
extern double DAT_007E1730, DAT_007E1738, DAT_007E1740, DAT_007F1190, DAT_007F1198;
extern double DAT_00B057E8, DAT_00B07778, DAT_00B07760, DAT_00B07758, DAT_00B057F0;
extern double DAT_00B057C8, DAT_00B057F8, DAT_00B07750;
extern int DAT_00B07780, DAT_00B07774, DAT_00B07740, DAT_00B07744, DAT_00B07748;
extern short DAT_00B057E0, DAT_00B057E2;
extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);

// FUNCTION: 0x00696D20
void Init_00696D20(void)
{
	DAT_00B057E8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);
}

// FUNCTION: 0x00696D50
void Init_00696D50(void)
{
	DAT_00B07778 = DAT_00B057E8 / DAT_007F1190;
}

// FUNCTION: 0x00696D70
void Init_00696D70(void)
{
	DAT_00B07760 = DAT_007E1718 / DAT_00B07778;
}

// FUNCTION: 0x00696D90
void Init_00696D90(void)
{
	DAT_00B07758 = DAT_007E1720 * DAT_007F1198;
}

// FUNCTION: 0x00696DB0
void Init_00696DB0(void)
{
	DAT_00B057F0 = DAT_007E1728 * DAT_007F1198;
}

// FUNCTION: 0x00696DD0
void Init_00696DD0(void)
{
	DAT_00B057C8 = DAT_007E1730 * DAT_007F1198;
}

// FUNCTION: 0x00696DF0
void Init_00696DF0(void)
{
	DAT_00B07780 = (int)(DAT_007E1738 * (DAT_00B057E8 * FUN_004CAD50(DAT_00B057C8 - DAT_00B057F0)));
}

// FUNCTION: 0x00696E30
void Init_00696E30(void)
{
	DAT_00B057F8 = FUN_004CADE0(DAT_00B07780 * DAT_007E1740);
}

// FUNCTION: 0x00696E60
void Init_00696E60(void)
{
	DAT_00B07750 = FUN_004CADE0((DAT_00B07780 * 2) / DAT_00B057E8);
}

// FUNCTION: 0x00696E90
void Init_00696E90(void)
{
	DAT_00B07774 = (int)(DAT_00B07780 * 4 + DAT_007E1738);
}

// FUNCTION: 0x00696EC0
void Init_00696EC0(void)
{
	DAT_00B057E0 = 0;
	DAT_00B057E2 = 0;
}

// FUNCTION: 0x00696ED0
void Init_00696ED0(void)
{
	DAT_00B07740 = 0;
	DAT_00B07744 = 0;
	DAT_00B07748 = 0;
}

// FUNCTION: 0x0069A340
unsigned int SessionClass::Compute_Unique_ID(void)
{
	unsigned int id;
	char *path;
	unsigned i;

	id = timeGetTime();

	unsigned int space_available = Disk_Space_Available();
	Add_CRC(&id, space_available);

	path = getenv("PATH");
	if (path) {
		for (i = 0; i < strlen(path); i++) {
			Add_CRC(&id, (unsigned int)path[i]);
		}
	}

	return(id);
}

// FUNCTION: 0x0069B560
bool GameOptionsType::Save(IStream * stream) const
{
	if (stream != NULL) {
		unsigned int size = sizeof(*this);
		HRESULT res = stream->Write(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			res = stream->Write(this, size, NULL);
			if (SUCCEEDED(res)) {
				return(true);
			}
		}
	}

	return(false);
}
