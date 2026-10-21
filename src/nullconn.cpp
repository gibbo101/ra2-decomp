// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "nullconn.h"
#include "wincomm.h"

// FUNCTION: GAMEMD 0x005EF100
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

// FUNCTION: GAMEMD 0x005EF150
// FUNCTION: GAMEMD 0x005EF290
NullModemConnClass::~NullModemConnClass (void)
{
	delete [] SendBuf;
}

// FUNCTION: GAMEMD 0x005EF170
void NullModemConnClass::Init (HANDLE port_handle)
{
	BASECLASS::Init();
	PortHandle = port_handle;
}

// FUNCTION: GAMEMD 0x005EF190
int NullModemConnClass::Send (char *buf, int buflen, void *, int, bool, unsigned short)
{
	int *ibuf;
	SerialHeaderType *header;
	unsigned int sendlen;

	if ( PortHandle == INVALID_HANDLE_VALUE ) return(false);

	header = (SerialHeaderType *) SendBuf;
	header->MagicNumber	= PACKET_SERIAL_START;
	header->Length			= (short) buflen;
	header->MagicNumber2	= PACKET_SERIAL_VERIFY;

	sendlen = sizeof( SerialHeaderType );
	memcpy (SendBuf + sendlen, buf, buflen);
	sendlen += buflen;
	ibuf = (int *)(SendBuf + sendlen);
	*ibuf = Compute_CRC( buf, buflen );
	sendlen += sizeof( int );

	*(SendBuf + sendlen) = '\r';
	sendlen += 1;

	SerialPort->Write_To_Serial_Port((unsigned char *)SendBuf, (int)sendlen );
	return(true);
}

// FUNCTION: GAMEMD 0x005EF240
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

// FUNCTION: GAMEMD 0x005EF270
int NullModemConnClass::Packet_Overhead_Size ( void )
{
	return( (PACKET_SERIAL_OVERHEAD_SIZE + sizeof(CommHeaderType)) );
}

// FUNCTION: GAMEMD 0x005EF280
void NullModemConnClass::Init (void)
{
	BASECLASS::Init();
}

NullModemConnClass *NullConnDeleteProbe (NullModemConnClass *p)
{
	delete p;
	return p;
}

struct NullConnProbe {
	int A, B, C, D;
	NullConnProbe () : A(0), B(0), C(0), D(0) {}
};

// SYNTHETIC: GAMEMD 0x005EF2D0 SYMBOL
// _$E3
NullConnProbe NullConnProbeObj;
