// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "nullconn.h"
#include "wincomm.h"

// FUNCTION: 0x005EF100
NullModemConnClass::NullModemConnClass (int numsend, int numreceive,
	int maxlen, unsigned short magicnum) :
	BASECLASS (numsend, numreceive, maxlen, magicnum,
		60,
		-1,
		1200)
{
	PortHandle = INVALID_HANDLE_VALUE;
	SendBuf = new char [ Actual_Max_Packet() ];
}

// FUNCTION: 0x005EF150
NullModemConnClass::~NullModemConnClass (void)
{
	delete [] SendBuf;
}

// FUNCTION: 0x005EF170
void NullModemConnClass::Init (HANDLE port_handle)
{
	BASECLASS::Init();
	PortHandle = port_handle;
}

// FUNCTION: 0x005EF240
int NullModemConnClass::Compute_CRC (char *buf, int buflen)
{
	unsigned int sum, hibit;

	sum = 0;
	for (int i = 0; i < buflen; i++) {
		if ( sum & 0x80000000 ) {
			hibit = 1;
		}
		else {
			hibit = 0;
		}

		sum <<= 1;
		sum += (hibit + (unsigned char)buf[i]);
	}

	return((int)sum);
}
