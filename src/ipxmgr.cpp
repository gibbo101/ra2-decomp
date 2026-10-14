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

#pragma pack(push, 1)
struct IPXPlayerNode {
	wchar_t Name[20];
	IPXAddressClass Address;
	char pad[0x6f - 0x34];
	int ID;
};
#pragma pack(pop)

struct IPXAddressRaw { int a, b, c; };

extern IPXPlayerNode ** DAT_00A8DA78;
extern int DAT_00A8DA84;
extern unsigned short DAT_00A8E2B2[];
void __cdecl FUN_004068E0(char const * format, ...);

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

// FUNCTION: 0x00540F90
int IPXManagerClass::Num_Connections(void)
{
	return NumConnections;
}

// FUNCTION: 0x00540FA0
int IPXManagerClass::Connection_ID(int index)
{
	if (index >= 0 && index < NumConnections) {
		return Connection[index]->ID;
	}
	return CONNECTION_NONE;
}

// FUNCTION: 0x00540FC0
wchar_t *IPXManagerClass::Connection_Name(int id)
{
	int i;

	for (i = 0; i < NumConnections; i++) {
		if (Connection[i]->ID == id) {
			return Connection[i]->Name;
		}
	}
	return NULL;
}

// FUNCTION: 0x00541000
IPXAddressClass * IPXManagerClass::Connection_Address(int id)
{
	int i;

	for (i = 0; i < NumConnections; i++) {
		if (Connection[i]->ID == id) {
			return &Connection[i]->Address;
		}
	}
	return NULL;
}

// FUNCTION: 0x00541040
int IPXManagerClass::Connection_Index(int id)
{
	int i;

	for (i = 0; i < NumConnections; i++) {
		if (Connection[i]->ID == id) {
			return i;
		}
	}
	return CONNECTION_NONE;
}

// FUNCTION: 0x005410C0
void IPXManagerClass::Set_Connection_Parms(int index, int id, wchar_t *name)
{
	if (index >= NumConnections)
		return;

	Connection[index]->ID = id;
	wcscpy(Connection[index]->Name, name);
}

// FUNCTION: 0x005410F0
int IPXManagerClass::Send_Global_Message(void *buf, int buflen, int ack_req,
	IPXAddressClass *address, unsigned short port, int packet_id)
{
	int rc;

	if (!IPXStatus || !Listening) return 0;

	if (ack_req != 0 && (address == NULL || address->Is_Broadcast())) {
		ack_req = 0;
	}

	rc = GlobalChannel->Send_Packet(buf, buflen, address, ack_req, port, packet_id);
	if (!rc) {
		SendOverflows++;
		FUN_004068E0("Send overflow %d\n", SendOverflows);
	}
	return rc;
}

// FUNCTION: 0x00541170
int IPXManagerClass::Send_Global_Message_To_Player(void *buf, int buflen, int ack_req, int id)
{
	int i;
	unsigned short port;
	IPXAddressRaw address;
	int count = DAT_00A8DA84;
	IPXPlayerNode **players = DAT_00A8DA78;

	if (count && players[0]->ID == id) return 0;
	for (i = 1; i < count; i++) {
		if (players[i]->ID == id) {
			address = *(IPXAddressRaw *)&players[i]->Address;
			port = DAT_00A8E2B2[i];

			return Send_Global_Message(buf, buflen, ack_req, (IPXAddressClass *)&address, port, 0);
		}
	}
	return 0;
}

// FUNCTION: 0x00541260
int IPXManagerClass::Send_Global_Message_To_Name(void *buf, int buflen, int ack_req, wchar_t *name)
{
	int i;
	unsigned short port;
	IPXAddressRaw address;

	if (DAT_00A8DA84 && wcsicmp(DAT_00A8DA78[0]->Name, name) == 0) return 0;
	for (i = 1; i < DAT_00A8DA84; i++) {
		if (wcsicmp(DAT_00A8DA78[i]->Name, name) == 0) {
			address = *(IPXAddressRaw *)&DAT_00A8DA78[i]->Address;
			port = DAT_00A8E2B2[i];

			return Send_Global_Message(buf, buflen, ack_req, (IPXAddressClass *)&address, port, 0);
		}
	}
	return 0;
}

// FUNCTION: 0x00541370
int IPXManagerClass::Get_Global_Message(void *buf, int *buflen,
	IPXAddressClass *address, unsigned short *product_id)
{
	if (!IPXStatus || !Listening) return 0;

	return GlobalChannel->Get_Packet(buf, buflen, address, product_id);
}

// FUNCTION: 0x005413B0
int IPXManagerClass::Global_Channel_Op(int a1, int a2, int a3, int a4, int a5, int a6)
{
	if (!IPXStatus || !Listening) return 0;

	return GlobalChannel->vt_entry_38(a1, a2, a3, a4, a5, a6);
}
