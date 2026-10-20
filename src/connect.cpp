// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "connect.h"
#include "_timer.h"
#include "dbgprint.h"
#include <cstdio>
#include <cstring>
#include <sys\timeb.h>
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
extern const double DAT_007E51A0;
extern const double DAT_007E51A8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0048BC70 $E3
double DAT_0089E8B8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0048BCA0 $E6
double DAT_0089E908 = DAT_0089E8B8 / DAT_007E51A0;

// FUNCTION: 0x0048BCC0 $E9
double DAT_0089E8F0 = DAT_007E1718 / DAT_0089E908;

// FUNCTION: 0x0048BCE0 $E12
double DAT_0089E8E8 = DAT_007E1720 * DAT_007E51A8;

// FUNCTION: 0x0048BD00 $E15
double DAT_0089E8C0 = DAT_007E1728 * DAT_007E51A8;

// FUNCTION: 0x0048BD20 $E18
double DAT_0089E898 = DAT_007E1730 * DAT_007E51A8;

// FUNCTION: 0x0048BD40 $E21
int DAT_0089E910 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_0089E898 - DAT_0089E8C0) * DAT_0089E8B8));

// FUNCTION: 0x0048BD80 $E24
double DAT_0089E8C8 = FUN_004CADE0(DAT_0089E910 * DAT_007E1740);

// FUNCTION: 0x0048BDB0 $E27
double DAT_0089E8E0 = FUN_004CADE0((DAT_0089E910 * 2) / DAT_0089E8B8);

// FUNCTION: 0x0048BDE0 $E30
int DAT_0089E904 = (int)(DAT_0089E910 * 4 + DAT_007E1738);

// FUNCTION: 0x0048BE10 $E33
ShortPair DAT_0089E8B0;

// FUNCTION: 0x0048BE20 $E36
IntTriple DAT_0089E8D0;

// FUNCTION: 0x0048BEE0
ConnectionClass::~ConnectionClass (void)
{
	delete [] PacketBuf;
	delete Queue;
}

// FUNCTION: 0x0048BF10
void ConnectionClass::Init (void)
{
	NumRecNoAck = 0;
	NumRecAck = 0;
	NumSendNoAck = 0;
	NumSendAck = 0;

	NumResends = 0;
	NumLost = 0;
	PercentLost = 0;
	MissedOverall = 0;
	MissedMagic = 0;

	LastSeqID = 0xffffffff;
	LastReadID = 0xffffffff;

	Queue->Init();
}

// FUNCTION: 0x0048C3B0
int ConnectionClass::Service (void)
{
	if ( Service_Send_Queue() && Service_Receive_Queue() ) {
		return(1);
	} else {
		return(0);
	}
}

// FUNCTION: 0x0048C600
unsigned int ConnectionClass::Time (void)
{
	return(TickCount);
}

// FUNCTION: 0x0048C630
const char *ConnectionClass::Command_Name(int command)
{
	if (command >= 0 && command < PACKET_COUNT) {
		return(Commands[command]);
	} else {
		return(NULL);
	}
}
