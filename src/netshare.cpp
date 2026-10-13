// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "netshare.h"
#include "_rules.h"
#include "cd.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "mapgen.h"
#include "modemhst.h"
#include "msgbox.h"
#include "netdlg.h"
#include "netdlg2.h"
#include "newmenu.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "rules.h"
#include "scenario.h"
#include "sendfile.h"
#include "session.h"
#include "stimer.h"
#include "wdtnet.h"
#include "windlg.h"
#include "wolapi\wolapi.h"
#include "wonline.h"
#include "worlddom.h"
#include "wstring.h"
#include "xpipe.h"
#include "xstraw.h"
#include <ctime>


// FUNCTION: 0x005E2430 $E2
// FUNCTION: 0x005E2460 $E5
// FUNCTION: 0x005E2480 $E8
// FUNCTION: 0x005E24A0 $E11
// FUNCTION: 0x005E24C0 $E14
// FUNCTION: 0x005E24E0 $E17
// FUNCTION: 0x005E2500 $E20
// FUNCTION: 0x005E2540 $E23
// FUNCTION: 0x005E2570 $E26
// FUNCTION: 0x005E25A0 $E29
// FUNCTION: 0x005E25D0 $E32
// FUNCTION: 0x005E25E0 $E35

// FUNCTION: 0x005E2620
unsigned int Wstring_Hash(Wstring & string)
{
	unsigned int hash = 0;

	hash = string.length();

	for (unsigned int i = 0; i < string.length(); i++) {
		hash += *(string.get() + i);
		hash += i;
		hash = (hash << 8) ^ (hash >> 24);
	}
	return(hash);
}

// FUNCTION: 0x005E26C0
HWND GameoptWindow(void)
{
	HWND dialog;

	dialog = WS_Find_Dialog(IDD_WOL_GAMEOPT);
	if (dialog) {
		return(dialog);
	}
	dialog = WS_Find_Dialog(IDD_WOL_GUEST);
	if (dialog) {
		return(dialog);
	}
	dialog = WS_Find_Dialog(IDD_MPLAYER_HOST);
	if (dialog) {
		return(dialog);
	}
	dialog = WS_Find_Dialog(IDD_MPLAYER_GUEST);
	if (dialog) {
		return(dialog);
	}
	return(0);
}
