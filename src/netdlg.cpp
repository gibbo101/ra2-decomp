// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "netdlg.h"
#include "_map.h"
#include "_rules.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "houstype.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "list.h"
#include "nulldlg.h"
#include "queue.h"
#include "rules.h"
#include "wonline.h"
#include "wsproto.h"
#include <ctime>

// FUNCTION: 0x005DAB80
unsigned int Compute_Name_CRC(char *name)
{
	char buf[80];
	unsigned int crc = 0L;
	int i;

	strcpy (buf, name);
	_strupr (buf);

	for (i = 0; (unsigned)i < strlen(buf); i++) {
		Add_CRC (&crc, (unsigned int)buf[i]);
	}

	return(crc);
}
