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
extern const double DAT_007EC0B8;
extern const double DAT_007EC0C0;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0053F240 $E21
double DAT_00AA0528 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x0053F270 $E24
double DAT_00AA0598 = DAT_00AA0528 / DAT_007EC0B8;

// FUNCTION: 0x0053F290 $E27
double DAT_00AA0580 = DAT_007E1718 / DAT_00AA0598;

// FUNCTION: 0x0053F2B0 $E30
double DAT_00AA0570 = DAT_007E1720 * DAT_007EC0C0;

// FUNCTION: 0x0053F2D0 $E33
double DAT_00AA0538 = DAT_007E1728 * DAT_007EC0C0;

// FUNCTION: 0x0053F2F0 $E36
double DAT_00AA0500 = DAT_007E1730 * DAT_007EC0C0;

// FUNCTION: 0x0053F310 $E39
int DAT_00AA05A0 = (int)(FUN_004CAD50(DAT_00AA0500 - DAT_00AA0538) * DAT_007E1738 * DAT_00AA0528);

// FUNCTION: 0x0053F350 $E42
double DAT_00AA0540 = FUN_004CADE0(DAT_00AA05A0 * DAT_007E1740);

// FUNCTION: 0x0053F380 $E45
double DAT_00AA0560 = FUN_004CADE0((DAT_00AA05A0 * 2) / DAT_00AA0528);

// FUNCTION: 0x0053F3B0 $E48
int DAT_00AA0594 = (int)(DAT_00AA05A0 * 4 + DAT_007E1738);

// FUNCTION: 0x0053F3E0 $E51
ShortPair DAT_00AA0520;

// FUNCTION: 0x0053F3F0 $E54
IntTriple DAT_00AA0550;

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
