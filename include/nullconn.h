// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "win.h"

#include "connect.h"

#define	PACKET_SERIAL_START				0xDABD
#define	PACKET_SERIAL_VERIFY				0xDEAF

#define	PACKET_SERIAL_OVERHEAD_SIZE	(sizeof( SerialHeaderType ) + sizeof( SerialCRCType ))

struct SerialHeaderType {
	unsigned short MagicNumber;
	unsigned short Length;
	unsigned short MagicNumber2;
};

struct SerialCRCType {
	int SerialCRC;
};

struct PORT {

};

class NullModemConnClass : public ConnectionClass
{
		typedef ConnectionClass BASECLASS;

	public:

		NullModemConnClass (int numsend, int numrecieve, int maxlen,
			unsigned short magicnum);
		virtual ~NullModemConnClass () override;

		void Init (HANDLE port_handle);
		virtual void Init (void) override;

		unsigned int Actual_Max_Packet (void) { return (MaxPacketLen +
			(sizeof(SerialHeaderType)) + sizeof(int) + sizeof (char)); }

		static int Compute_CRC(char *buf, int buflen);

		static int Packet_Overhead_Size( void );

	protected:

		virtual int Send (char *buf, int buflen, void *extrabuf, int extralen, bool flag, unsigned short port) override;

		HANDLE PortHandle;

		char *SendBuf;
};

