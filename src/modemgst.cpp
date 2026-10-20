// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "modemgst.h"
#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "lzostraw.h"
#include "mapgen.h"
#include "mplayer.h"
#include "msgbox.h"
#include "msgloop.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "rules.h"
#include "scenario.h"
#include "sendfile.h"
#include "stimer.h"
#include "timer.h"
#include "trim.h"
#include "winstub.h"
#include "xstraw.h"

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
extern const double DAT_007EDF58;
extern const double DAT_007EDF60;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005B47C0 $E43
double DAT_00ABF030 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005B47F0 $E46
double DAT_00ABF080 = DAT_00ABF030 / DAT_007EDF58;

// FUNCTION: 0x005B4810 $E49
double DAT_00ABF068 = DAT_007E1718 / DAT_00ABF080;

// FUNCTION: 0x005B4830 $E52
double DAT_00ABF060 = DAT_007E1720 * DAT_007EDF60;

// FUNCTION: 0x005B4850 $E55
double DAT_00ABF038 = DAT_007E1728 * DAT_007EDF60;

// FUNCTION: 0x005B4870 $E58
double DAT_00ABF010 = DAT_007E1730 * DAT_007EDF60;

// FUNCTION: 0x005B4890 $E61
int DAT_00ABF088 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF010 - DAT_00ABF038) * DAT_00ABF030));

// FUNCTION: 0x005B48D0 $E64
double DAT_00ABF040 = FUN_004CADE0(DAT_00ABF088 * DAT_007E1740);

// FUNCTION: 0x005B4900 $E67
double DAT_00ABF058 = FUN_004CADE0((DAT_00ABF088 * 2) / DAT_00ABF030);

// FUNCTION: 0x005B4930 $E70
int DAT_00ABF07C = (int)(DAT_00ABF088 * 4 + DAT_007E1738);

// FUNCTION: 0x005B4960 $E73
ShortPair DAT_00ABF028;

// FUNCTION: 0x005B4970 $E76
IntTriple DAT_00ABF048;

// FUNCTION: 0x005B5960
void ModemGuest::SetHouse(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			Session.House = ComboBox_GetItemData(handle,i);
			Transmit = true;
			Changed = true;
		}
	}
}
