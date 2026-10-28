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

// SYNTHETIC: GAMEMD 0x0053F220 SYMBOL
// _$E13

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
extern int DAT_00AA0590, DAT_00AA0588, DAT_00AA058C;
extern int DAT_00AA05B8, DAT_00AA05BC, DAT_00AA05C0, DAT_00AA05C4;
extern const double DAT_007EC0B8;
extern int DAT_00AA0590, DAT_00AA0588, DAT_00AA058C;
extern int DAT_00AA05B8, DAT_00AA05BC, DAT_00AA05C0, DAT_00AA05C4;
extern const double DAT_007EC0B8;
extern int DAT_00841F3C;
extern WinsockInterfaceClass * DAT_00887628;
#include "ownertalk.h"

extern const double DAT_007EC0C0;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0053F240 SYMBOL
// _$E21
double DAT_00AA0528 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// SYNTHETIC: GAMEMD 0x0053F270 SYMBOL
// _$E24
double DAT_00AA0598 = DAT_00AA0528 / DAT_007EC0B8;

// SYNTHETIC: GAMEMD 0x0053F290 SYMBOL
// _$E27
double DAT_00AA0580 = DAT_007E1718 / DAT_00AA0598;

// SYNTHETIC: GAMEMD 0x0053F2B0 SYMBOL
// _$E30
double DAT_00AA0570 = DAT_007E1720 * DAT_007EC0C0;

// SYNTHETIC: GAMEMD 0x0053F2D0 SYMBOL
// _$E33
double DAT_00AA0538 = DAT_007E1728 * DAT_007EC0C0;

// SYNTHETIC: GAMEMD 0x0053F2F0 SYMBOL
// _$E36
double DAT_00AA0500 = DAT_007E1730 * DAT_007EC0C0;

// SYNTHETIC: GAMEMD 0x0053F310 SYMBOL
// _$E39
int DAT_00AA05A0 = (int)(FUN_004CAD50(DAT_00AA0500 - DAT_00AA0538) * DAT_007E1738 * DAT_00AA0528);

// SYNTHETIC: GAMEMD 0x0053F350 SYMBOL
// _$E42
double DAT_00AA0540 = FUN_004CADE0(DAT_00AA05A0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0053F380 SYMBOL
// _$E45
double DAT_00AA0560 = FUN_004CADE0((DAT_00AA05A0 * 2) / DAT_00AA0528);

// SYNTHETIC: GAMEMD 0x0053F3B0 SYMBOL
// _$E48
int DAT_00AA0594 = (int)(DAT_00AA05A0 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0053F3E0 SYMBOL
// _$E51
ShortPair DAT_00AA0520;

// SYNTHETIC: GAMEMD 0x0053F3F0 SYMBOL
// _$E54
IntTriple DAT_00AA0550;

// FUNCTION: GAMEMD 0x0053F410
void FUN_0053F410(void)
{
	DAT_00AA0590 = 0;
	DAT_00AA0588 = 128;
	DAT_00AA058C = 128;
}

// FUNCTION: GAMEMD 0x0053F430
IPXConnClass::IPXConnClass (int numsend, int numreceive, int maxlen,
	unsigned short magicnum, IPXAddressClass *address, int id, wchar_t const *name,
	int extralen) :
	BASECLASS (numsend, numreceive, maxlen, magicnum, 2, -1, 60, extralen)
{
	NetNumType net;
	NetNodeType node;

	if (address)
		Address = (*address);
	ID = id;
	wcscpy (Name, name);

	Address.Get_Address(net,node);
	memcpy(ImmediateAddress,node,6);
	Immed_Set = 0;
}

// FUNCTION: GAMEMD 0x0053F4E0
void IPXConnClass::Init (void)
{
	BASECLASS::Init();
}

// FUNCTION: GAMEMD 0x0053F4F0
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

// FUNCTION: GAMEMD 0x0053F5B0
int IPXConnClass::Stop_Listening(void)
{
	if ( PacketTransport ) PacketTransport->Stop_Listening();
	Listening = 0;

	return(1);
}

// FUNCTION: GAMEMD 0x0053F5D0
int IPXConnClass::Send(char *buf, int buflen, void *, int, bool flag, unsigned short)
{
	return(Send_To (buf, buflen, &Address, NULL, flag, 0));
}

// FUNCTION: GAMEMD 0x0053F5F0
int IPXConnClass::Open_Socket(unsigned short socket)
{
	int rc;

	PacketTransport->Set_NetCard(Options.NetCard);
	rc = PacketTransport->Open_Socket(socket);

	SocketOpen = rc;
	return( rc );
}

// FUNCTION: GAMEMD 0x0053F630
void IPXConnClass::Close_Socket(unsigned short socket)
{
	socket = socket;
	PacketTransport->Close_Socket();
	SocketOpen = 0;
}

#include "session.h"
#include "ipxgconn.h"
#include "ipxmgr.h"

// FUNCTION: GAMEMD 0x0053F650
int IPXConnClass::Send_To(char *buf, int buflen, IPXAddressClass *address,
	NetNodeType immed, bool flag, unsigned short port)
{
	IPXAddressClass dest;
	unsigned short dest_port;

	dest = *address;
	if (immed) {
		NetNodeType node;
		NetNumType net;
		address->Get_Address(net, node);
		memcpy(net, immed, 4);
		dest.Set_Address(net, node, 0);
	}

	dest_port = 0;
	if (!flag && Session.ViaPacketRouter) {
		if (!((GlobalHeaderType *)buf)->Field3) {
			unsigned char result = OwnerTalk.Mask_From_ID(ID);
			if (!result) {
				return 0;
			}
			((GlobalHeaderType *)buf)->Field3 = result;
		}

		dest = Session.unknown_296C;
		dest_port = DAT_00841F3C;
		if (Session.unknown_2978 == Session.Players[0]->Address && dest.FUN_0053EF30(Session.Players[0]->Address)) {
			((GlobalHeaderType *)buf)->FieldC = 0;
			memset(&((GlobalHeaderType *)buf)->Field8, 0, 4);
		}
		else {
			dest_port = *(unsigned short *)Session.unknown_2984;
			((GlobalHeaderType *)buf)->FieldC = DAT_00841F3C;
			memset(&((GlobalHeaderType *)buf)->Field8, 0, 4);
		}
	}
	else {
		((GlobalHeaderType *)buf)->FieldC = 0;
		memset(&((GlobalHeaderType *)buf)->Field8, 0, 4);
	}

	if (Session.Type == GAME_INTERNET && !Session.ViaPacketRouter) {
		int index = Ipx.FUN_00541070(this);
		switch (index) {
		case -3:
		case -2:
		case -1:
			goto none;
		default:
			if (index == 0 && Session.ViaPacketRouter) {
				if (*(unsigned short *)Session.unknown_2984) {
					dest_port = *(unsigned short *)Session.unknown_2984;
				}
			}
			else if (index >= 0 && index < Ipx.Num_Connections()) {
				dest_port = Session.FUN_0069BC20(Ipx.Connection_ID(index));
			}
			break;
		case -4:
			if (*(unsigned short *)Session.unknown_2984) {
				dest_port = *(unsigned short *)Session.unknown_2984;
			}
			break;
		}
		none:;
	}

	if (DAT_00887628) {
		DAT_00887628->WriteTo(buf, buflen, &dest, sizeof(dest), flag, false, dest_port);
	}
	return(1);
}


// FUNCTION: GAMEMD 0x0053F830
int IPXConnClass::Broadcast(char *buf, int buflen)
{
	PacketTransport->Broadcast (buf, buflen, false);
	return(true);
}

// SYNTHETIC: GAMEMD 0x0053F850
// ??_GIPXConnClass@@UAEPAXI@Z
