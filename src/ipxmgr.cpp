// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "ipxmgr.h"

#include "_surface.h"
#include "dbgprint.h"
#include "dialog.h"
#include "globals.h"
#include "house.h"
#include "queue.h"
#include "scheme.h"
#include "session.h"
#include "stimer.h"
#include "stringtable.h"
#include "surface.h"
#include "vector.h"
#include "wspipx.h"
#include "wsproto.h"

// FUNCTION: 0x005408F0
IPXManagerClass::IPXManagerClass (int glb_maxlen, int pvt_maxlen,
	int glb_num_packets, int pvt_num_packets, unsigned short socket,
	unsigned short product_id)
{
	int i;

	if ( PacketTransport ) {
		delete  PacketTransport;
		PacketTransport = NULL;
	}
	PacketTransport = new IPXInterfaceClass;

	if ( PacketTransport->Init() ){
		IPXStatus = 1;
	}else{
		IPXStatus = 0;
	}
	delete PacketTransport;
	PacketTransport = NULL;

	Listening = 0;

	Glb_MaxPacketLen = glb_maxlen;
	Glb_NumPackets = glb_num_packets;
	Pvt_MaxPacketLen = pvt_maxlen;
	Pvt_NumPackets = pvt_num_packets;
	Lobby_MaxPacketLen = glb_maxlen;
	Lobby_NumPackets = pvt_num_packets;

	ProductID = product_id;

	Socket = (unsigned short)( (((unsigned int)socket & 0x00ff) << 8) |
		(((unsigned int)socket & 0xff00) >> 8));

	ConnectionNum = 0;

	NumConnections = 0;
	CurConnection = 0;
	for (i = 0; i < CONNECT_MAX; i++) {
		Connection[i] = 0;
	}
	GlobalChannel = 0;
	LobbyChannel = 0;
	MulticastChannel = 0;

	SendOverflows = 0;
	ReceiveOverflows = 0;
	BadConnection = CONNECTION_NONE;

	RetryDelta = 2;
	MaxRetries = -1;
	Timeout = 60;
}

// FUNCTION: 0x005409F0
IPXManagerClass::~IPXManagerClass(void)
{
	int i;

	if (Listening) {
		IPXConnClass::Stop_Listening();
		Listening = 0;
	}

	if (GlobalChannel) {
		delete GlobalChannel;
		GlobalChannel = 0;
	}
	if (LobbyChannel) {
		delete LobbyChannel;
		LobbyChannel = 0;
	}
	if (MulticastChannel) {
		delete MulticastChannel;
		MulticastChannel = 0;
	}
	for (i = 0; i < NumConnections; i++) {
		delete Connection[i];
		Connection[i] = 0;
	}
	NumConnections = 0;
}

// FUNCTION: 0x00540A80
int IPXManagerClass::Init(void)
{
	int i;

	DebugString("**** IPXManagerClass::Init ***\n");
	if (PacketTransport) {
		PacketTransport->Discard_In_Buffers();
		PacketTransport->Discard_Out_Buffers();
	}

	if (Session.Type != GAME_INTERNET) {
		if (!IPXStatus) {
			return(0);
		}
		if (Listening) {
			IPXConnClass::Stop_Listening();
			Listening = 0;
		}
	} else {
		IPXStatus = 1;
	}

	if (GlobalChannel) {
		delete GlobalChannel;
		GlobalChannel = 0;
	}
	if (LobbyChannel) {
		delete LobbyChannel;
		LobbyChannel = 0;
	}
	if (MulticastChannel) {
		delete MulticastChannel;
		MulticastChannel = 0;
	}
	for (i = 0; i < NumConnections; i++) {
		delete Connection[i];
		Connection[i] = 0;
	}
	NumConnections = 0;

	GlobalChannel = new IPXGlobalConnClass (Glb_NumPackets, Glb_NumPackets * 2,
		Glb_MaxPacketLen, ProductID, 0x1236);
	if (!GlobalChannel) {
		return(0);
	}
	GlobalChannel->Init();
	GlobalChannel->Set_Retry_Delta (RetryDelta);
	GlobalChannel->Set_Max_Retries (MaxRetries);
	GlobalChannel->Set_TimeOut (Timeout);

	LobbyChannel = new IPXGlobalConnClass (Lobby_NumPackets, Lobby_NumPackets * 2,
		Lobby_MaxPacketLen, ProductID, 0xCDEF);
	if (!LobbyChannel) {
		return(0);
	}
	LobbyChannel->Init();
	LobbyChannel->Set_Retry_Delta (RetryDelta);
	LobbyChannel->Set_Max_Retries (MaxRetries);
	LobbyChannel->Set_TimeOut (Timeout);

	IPXConnClass::Configure(Socket, ConnectionNum,
		FirstHeaderBuf, SendHeader, FirstDataBuf, SendBuf, PacketLen);

	if (PacketTransport == NULL) {
		return(0);
	}

	if (Session.Type != GAME_INTERNET) {
		if (!IPXConnClass::Start_Listening()) return(0);
	}
	Listening = 1;

	return(1);
}

// FUNCTION: 0x00540C50
int IPXManagerClass::Is_IPX(void)
{
	return(IPXStatus);
}

// FUNCTION: 0x00540C60
void IPXManagerClass::Set_Timing (unsigned int retrydelta,
	unsigned int maxretries, unsigned int timeout, bool set_external)
{
	int i;

	RetryDelta = retrydelta;
	MaxRetries = maxretries;
	Timeout = timeout;

	DebugString("IPX Manager: RetryDelta = %d\n", retrydelta);
	DebugString("MaxAhead is %d\n", Session.MaxAhead);

	if (set_external) {
		Set_External_Timing(RetryDelta, MaxRetries, Timeout);
	}

	for (i = 0; i < NumConnections; i++) {
		Connection[i]->Set_Retry_Delta (RetryDelta);
		Connection[i]->Set_Max_Retries (MaxRetries);
		Connection[i]->Set_TimeOut (Timeout);
	}

	if (MulticastChannel) {
		MulticastChannel->Set_Retry_Delta (RetryDelta);
		MulticastChannel->Set_Max_Retries (MaxRetries);
		MulticastChannel->Set_TimeOut (Timeout);
	}
}

// FUNCTION: 0x00540D10
void IPXManagerClass::Set_External_Timing (unsigned int retrydelta,
	unsigned int maxretries, unsigned int timeout)
{
	if (GlobalChannel) {
		GlobalChannel->Set_Retry_Delta (retrydelta);
		GlobalChannel->Set_Max_Retries (maxretries);
		GlobalChannel->Set_TimeOut (timeout);
	}
}

// FUNCTION: 0x00540D40
void IPXManagerClass::Save_Timing(void)
{
	if (GlobalChannel) {
		SavedRetryDelta = GlobalChannel->Retry_Delta();
		SavedMaxRetries = GlobalChannel->Max_Retries();
		SavedTimeout = GlobalChannel->Time_Out();
	}
}

// FUNCTION: 0x00540D60
void IPXManagerClass::Restore_Timing(void)
{
	if (GlobalChannel) {
		GlobalChannel->Set_Retry_Delta (SavedRetryDelta);
		GlobalChannel->Set_Max_Retries (SavedMaxRetries);
		GlobalChannel->Set_TimeOut (SavedTimeout);
	}
}

// FUNCTION: 0x00540D80
void IPXManagerClass::Set_Lobby_Timing(unsigned int retrydelta,
	unsigned int maxretries, unsigned int timeout)
{
	if (LobbyChannel) {
		LobbyChannel->Set_Retry_Delta (retrydelta);
		LobbyChannel->Set_Max_Retries (maxretries);
		LobbyChannel->Set_TimeOut (timeout);
	}
	if (MulticastChannel) {
		MulticastChannel->Set_Retry_Delta (retrydelta);
		MulticastChannel->Set_Max_Retries (maxretries);
		MulticastChannel->Set_TimeOut (timeout);
	}
}

// FUNCTION: 0x00540DC0
int IPXManagerClass::Create_Connection(int id, wchar_t *name,
	IPXAddressClass *address)
{
	if (!IPXStatus) {
		return(0);
	}

	if (NumConnections==CONNECT_MAX) {
		return(0);
	}

	Connection[NumConnections] = new IPXConnClass(Pvt_NumPackets,
		Pvt_NumPackets, Pvt_MaxPacketLen, ProductID, address, id, name, 0);
	if (!Connection[NumConnections]) {
		return(0);
	}

	Connection[NumConnections]->Init ();
	Connection[NumConnections]->Set_Retry_Delta (RetryDelta);
	Connection[NumConnections]->Set_Max_Retries (MaxRetries);
	Connection[NumConnections]->Set_TimeOut (Timeout);

	NumConnections++;

	return(1);
}

// FUNCTION: 0x00540E70
int IPXManagerClass::Create_Multicast_Connection(void)
{
	if (!IPXStatus) {
		return(0);
	}

	MulticastChannel = new IPXConnClass(Pvt_NumPackets,
		Pvt_NumPackets, Pvt_MaxPacketLen, ProductID,
		&Session.Players[0]->Address, -1, FUN_00735120("Multicast"));
	if (!MulticastChannel) {
		return(0);
	}

	MulticastChannel->Init ();
	MulticastChannel->Set_Retry_Delta (RetryDelta);
	MulticastChannel->Set_Max_Retries (MaxRetries);
	MulticastChannel->Set_TimeOut (Timeout);
	return(1);
}

// FUNCTION: 0x00540F00
int IPXManagerClass::Delete_Connection(int id)
{
	int i,j;

	if (!IPXStatus) {
		return(0);
	}

	if (NumConnections==0) {
		return(0);
	}

	for (i = 0; i < NumConnections; i++) {
		if (Connection[i]->ID==id) {
			delete Connection[i];

			for (j = i; j < NumConnections - 1; j++) {
				Connection[j] = Connection[j+1];
			}

			NumConnections--;
			if (CurConnection >= NumConnections)
				CurConnection = 0;
			return(1);
		}
	}

	return(0);
}
