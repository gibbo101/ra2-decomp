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
