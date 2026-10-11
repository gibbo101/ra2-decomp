// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxconn.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "wsproto.h"

// FUNCTION: 0x0053F4F0
void IPXConnClass::Configure (unsigned short socket, int conn_num,
	                                            IPXHeaderType *listen_header,
	IPXHeaderType *send_header, char *listen_buf, char *send_buf,
	                         int maxpacketlen)
{
	Socket = socket;
	ConnectionNum = conn_num;
	ListenHeader = listen_header;
	SendHeader = send_header;
	ListenBuf = listen_buf;
	SendBuf = send_buf;
	PacketLen = maxpacketlen;

	Configured = 1;
}

// FUNCTION: 0x0053F5B0
int IPXConnClass::Stop_Listening(void)
{
	if ( PacketTransport ) PacketTransport->Stop_Listening();
	Listening = 0;

	return(1);
}

// FUNCTION: 0x0053F5F0
int IPXConnClass::Open_Socket(unsigned short socket)
{
	int rc;

	PacketTransport->Set_NetCard(Options.NetCard);
	rc = PacketTransport->Open_Socket(socket);

	SocketOpen = rc;
	return( rc );
}

// FUNCTION: 0x0053F630
void IPXConnClass::Close_Socket(unsigned short socket)
{
	socket = socket;
	PacketTransport->Close_Socket();
	SocketOpen = 0;
}
