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
extern const double DAT_007EEEF8;
extern const double DAT_007EEF00;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005DA4D0 $E80
double DAT_00ABFF10 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005DA500 $E83
double DAT_00ABFF60 = DAT_00ABFF10 / DAT_007EEEF8;

// FUNCTION: 0x005DA520 $E86
double DAT_00ABFF48 = DAT_007E1718 / DAT_00ABFF60;

// FUNCTION: 0x005DA540 $E89
double DAT_00ABFF40 = DAT_007E1720 * DAT_007EEF00;

// FUNCTION: 0x005DA560 $E92
double DAT_00ABFF18 = DAT_007E1728 * DAT_007EEF00;

// FUNCTION: 0x005DA580 $E95
double DAT_00ABFEF0 = DAT_007E1730 * DAT_007EEF00;

// FUNCTION: 0x005DA5A0 $E98
int DAT_00ABFF68 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABFEF0 - DAT_00ABFF18) * DAT_00ABFF10));

// FUNCTION: 0x005DA5E0 $E101
double DAT_00ABFF20 = FUN_004CADE0(DAT_00ABFF68 * DAT_007E1740);

// FUNCTION: 0x005DA610 $E104
double DAT_00ABFF38 = FUN_004CADE0((DAT_00ABFF68 * 2) / DAT_00ABFF10);

// FUNCTION: 0x005DA640 $E107
int DAT_00ABFF5C = (int)(DAT_00ABFF68 * 4 + DAT_007E1738);

// FUNCTION: 0x005DA670 $E110
ShortPair DAT_00ABFF08;

// FUNCTION: 0x005DA680 $E113
IntTriple DAT_00ABFF28;


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
