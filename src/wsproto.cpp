// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "wsproto.h"
#include "_keyboar.h"
#include "assert.h"
#include "dbgprint.h"
#include "globals.h"
#include "keyboard.h"
#include "vector.h"
#include <cstdio>
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007F79A8;
extern const double DAT_007F79B0;

// FUNCTION: 0x007B17D0 $E5
double DAT_00B78040 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x007B1800 $E8
double DAT_00B78090 = DAT_00B78040 / DAT_007F79A8;

// FUNCTION: 0x007B1820 $E11
double DAT_00B78078 = 1.0 / DAT_00B78090;

// FUNCTION: 0x007B1840 $E14
double DAT_00B78070 = DAT_007F79B0 * 45.0;

// FUNCTION: 0x007B1860 $E17
double DAT_00B78048 = DAT_007F79B0 * 60.0;

// FUNCTION: 0x007B1880 $E20
double DAT_00B78020 = DAT_007F79B0 * 90.0;

// FUNCTION: 0x007B18A0 $E23
int DAT_00B78098 = (int)(FUN_004CAD50(DAT_00B78020 - DAT_00B78048) * DAT_00B78040 * 0.5);

// FUNCTION: 0x007B18E0 $E26
double DAT_00B78050 = FUN_004CADE0(DAT_00B78098 * 0.00390625);

// FUNCTION: 0x007B1910 $E29
double DAT_00B78068 = FUN_004CADE0((DAT_00B78098 * 2) / DAT_00B78040);

// FUNCTION: 0x007B1940 $E32
int DAT_00B7808C = (int)(DAT_00B78098 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x007B1970 $E35
ShortPair DAT_00B78038;

// FUNCTION: 0x007B1980 $E38
IntTriple DAT_00B78058;

// FUNCTION: 0x007B25E0
void WinsockInterfaceClass::Clear_Socket_Error(SOCKET socket)
{
	unsigned int error_code;
	int length = 4;

	{
		getsockopt (socket, SOL_SOCKET, SO_ERROR, (char*)&error_code, &length);
		error_code = 0;
		setsockopt (socket, SOL_SOCKET, SO_ERROR, (char*)&error_code, length);
	}
}

// FUNCTION: 0x007B26D0
bool WinsockInterfaceClass::Get_Host_Name(char *name, int len)
{
	if (gethostname(name, len) == -1) {
		DebugString("Error - WinsockInterface Unable to get host name. Error code %d\n", LAST_ERROR );
		return(false);
	}

	DebugString("WinsockInterface Host name is %s\n", name);
	return(true);
}
