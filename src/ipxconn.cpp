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

extern int DAT_00AA05B8, DAT_00AA05BC, DAT_00AA05C0, DAT_00AA05C4;
extern int DAT_00AA05B8, DAT_00AA05BC, DAT_00AA05C0, DAT_00AA05C4;
extern int DAT_00841F3C;
#include "ownertalk.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0053F240 SYMBOL
// _$E21
// SYNTHETIC: GAMEMD 0x0053F270 SYMBOL
// _$E24
// SYNTHETIC: GAMEMD 0x0053F290 SYMBOL
// _$E27
// SYNTHETIC: GAMEMD 0x0053F2B0 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x0053F2D0 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x0053F2F0 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x0053F310 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x0053F350 SYMBOL
// _$E42
// SYNTHETIC: GAMEMD 0x0053F380 SYMBOL
// _$E45
// SYNTHETIC: GAMEMD 0x0053F3B0 SYMBOL
// _$E48
// SYNTHETIC: GAMEMD 0x0053F3E0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x0053F3F0 SYMBOL
// _$E54

unsigned short IPXConnClass::Socket;
int IPXConnClass::ConnectionNum;
IPXHeaderType * IPXConnClass::ListenHeader;
char * IPXConnClass::ListenBuf;
IPXHeaderType * IPXConnClass::SendHeader;
char * IPXConnClass::SendBuf;
int IPXConnClass::Configured = 0;
int IPXConnClass::SocketOpen = 0;
int IPXConnClass::Listening = 0;
int IPXConnClass::PacketLen;

// SYNTHETIC: GAMEMD 0x0053F410 SYMBOL
// _$E57

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

// FUNCTION: GAMEMD 0x0053F540
int IPXConnClass::Start_Listening(void)
{
	SocketOpen = Open_Socket(Socket);
	if (!SocketOpen)
		return(false);

	if ( PacketTransport->Start_Listening () ) {
		Listening =1;
		return(true);
	} else {
		Close_Socket(Socket);
		SocketOpen = false;
		return(false);
	}
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

		dest = Session.PacketRouterAddress;
		dest_port = DAT_00841F3C;
		if (Session.PacketRouterAddressTranslate == Session.Players[0]->Address && dest.FUN_0053EF30(Session.Players[0]->Address)) {
			((GlobalHeaderType *)buf)->ForwardPort = 0;
			memset(&((GlobalHeaderType *)buf)->ForwardAddress, 0, 4);
		}
		else {
			dest_port = Session.PacketRouterPort;
			((GlobalHeaderType *)buf)->ForwardPort = DAT_00841F3C;
			memset(&((GlobalHeaderType *)buf)->ForwardAddress, 0, 4);
		}
	}
	else {
		((GlobalHeaderType *)buf)->ForwardPort = 0;
		memset(&((GlobalHeaderType *)buf)->ForwardAddress, 0, 4);
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
				if (Session.PacketRouterPort) {
					dest_port = Session.PacketRouterPort;
				}
			}
			else if (index >= 0 && index < Ipx.Num_Connections()) {
				dest_port = Session.FUN_0069BC20(Ipx.Connection_ID(index));
			}
			break;
		case -4:
			if (Session.PacketRouterPort) {
				dest_port = Session.PacketRouterPort;
			}
			break;
		}
		none:;
	}

	if (PacketTransport) {
		PacketTransport->WriteTo(buf, buflen, &dest, sizeof(dest), flag, false, dest_port);
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
