// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

#include "wspudp.h"

#include "assert.h"
#include "dbgprint.h"
#include "ipxaddr.h"
#include "misc.h"
#include "msgloop.h"
#include "vector.h"

#include <cstdio>

// SYNTHETIC: GAMEMD 0x007B2BA0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x007B2BD0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x007B2BF0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x007B2C10 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007B2C30 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007B2C50 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x007B2C70 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x007B2CB0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x007B2CE0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007B2D10 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007B2D40 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007B2D50 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x007B2D70 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x007B2D90 SYMBOL
// _$E41

namespace Game { extern HWND hWnd; }
extern int DAT_00841F38;

// FUNCTION: GAMEMD 0x007B2DB0
UDPInterfaceClass::UDPInterfaceClass (void) : BASECLASS()
{
	Port = 0;
	PortCount = 0;
	SelectedSocket = INVALID_SOCKET;
	PeerCount = 0;
}

// FUNCTION: GAMEMD 0x007B2E50
UDPInterfaceClass::~UDPInterfaceClass (void)
{
	Clear_Broadcast_Addresses();

	while ( LocalAddresses.Count() ) {
		delete LocalAddresses[0];
		LocalAddresses.Delete_Index(0);
	}

	Close();
}

// FUNCTION: GAMEMD 0x007B2F90
void UDPInterfaceClass::Clear_Broadcast_Addresses(void)
{
	while ( BroadcastAddresses.Count() ) {
		delete BroadcastAddresses[0];
		BroadcastAddresses.Delete_Index(0);
	}
}

// FUNCTION: GAMEMD 0x007B2FF0
void UDPInterfaceClass::Set_Broadcast_Address (char const *address, unsigned short port)
{
	int addr[4];
	unsigned char *baddr = new unsigned char[4];

	sscanf ( address, "%d.%d.%d.%d", (unsigned char *)&addr[0], (unsigned char *)&addr[1], (unsigned char *)&addr[2], (unsigned char *)&addr[3] );
	baddr[0] = addr[0];
	baddr[1] = addr[1];
	baddr[2] = addr[2];
	baddr[3] = addr[3];
	if (port == 0) port = UDPListenPort;
	BroadcastPorts[BroadcastAddresses.Count()] = port;
	BroadcastAddresses.Add (baddr);
}

// FUNCTION: GAMEMD 0x007B30B0
bool UDPInterfaceClass::OpenSocket (unsigned short port)
{
	LINGER ling;
	struct sockaddr_in addr;

	if (Socket != INVALID_SOCKET) {
		closesocket(Socket);
		Socket = INVALID_SOCKET;
	}

	DebugString("About to open primary UDP socket\n");

	Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (Socket == INVALID_SOCKET) {
		return(false);
	}

	addr.sin_family = AF_INET;
	if (port) {
		Port = port;
		addr.sin_port = htons(port);
	} else {
		Port = 0;
		addr.sin_port = htons((unsigned short) UDPListenPort);
	}
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	DebugString("About to bind the UDP socket to port %d\n", port);

	if (bind(Socket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		DebugString("Bind failed with error code %d\n", LAST_ERROR);
		Close_Socket();
		return(false);
	}

	ling.l_onoff = 0;
	ling.l_linger = 0;
	setsockopt(Socket, SOL_SOCKET, SO_LINGER, (LPSTR)&ling, sizeof(ling));

	BASECLASS::Set_Socket_Options();

	DebugString("Primary UDP Socket init complete\n");

	return(true);
}

// FUNCTION: GAMEMD 0x007B31E0
bool UDPInterfaceClass::Open_Port_Socket (unsigned short port)
{
	struct sockaddr_in addr;

	if (port == 0) {
		return(false);
	}

	for (int i = 0; i < PortCount; i++) {
		if (PortNumbers[i] == port) {
			return(false);
		}
	}

	DebugString("About to open spare UDP socket\n");

	SOCKET spare = socket(AF_INET, SOCK_DGRAM, 0);
	if (spare == INVALID_SOCKET) {
		return(false);
	}

	DebugString("Got spare socket %d\n", spare);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	DebugString("About to bind the UDP socket to port %d\n", port);

	if (bind(spare, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		closesocket(spare);
		DebugString("Failed to bind spare socket to port %d\n", port);
		return(false);
	}

	PortNumbers[PortCount] = port;
	PortSockets[PortCount] = spare;
	PortCount++;

	BASECLASS::Set_Socket_Options(spare);

	DebugString("Spare UDP Socket init complete\n");

	return(true);
}

// FUNCTION: GAMEMD 0x007B3310
void UDPInterfaceClass::Close_Port_Socket (unsigned short port)
{
	if (port) {
		for (int i = 0; i < PortCount; i++) {
			if (PortNumbers[i] == port) {
				DebugString("Closing spare socket %d\n", PortSockets[i]);
				closesocket(PortSockets[i]);
				if (PortSockets[i] == SelectedSocket) {
					SelectedSocket = INVALID_SOCKET;
				}
				for (int j = i; j < PortCount - 1; j++) {
					PortSockets[j] = PortSockets[j + 1];
					PortNumbers[j] = PortNumbers[j + 1];
				}
				PortCount--;
				return;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007B33D0
void UDPInterfaceClass::Select_Port_Socket (unsigned short port)
{
	if (port && port != UDPListenPort) {
		for (int i = 0; i < PortCount; i++) {
			if (PortNumbers[i] == port) {
				SelectedSocket = PortSockets[i];
				return;
			}
		}
	}
	SelectedSocket = INVALID_SOCKET;
}

// FUNCTION: GAMEMD 0x007B3430
bool UDPInterfaceClass::Init_Ports (void)
{
	if (Init()) {
		PortCount = 0;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x007B3450
bool UDPInterfaceClass::Open_Socket ( SOCKET )
{
	LINGER ling;
	struct sockaddr_in addr;

	if ( !WinsockInitialised ) {
		if ( !Init_Ports()) return( false );
	}

	OpenSocket((unsigned short) UDPListenPort);

	DebugString("About to query the host name\n");

	char hostname[128];

	if (gethostname(hostname, 128) != 0) {
		DebugString("Host name is '%s'\n", hostname);
	}

	DebugString("About to call gethostbyname\n");

	struct hostent *host_info = gethostbyname ( hostname );

	if (host_info == NULL) {
		DebugString("gethostbyname failed! Error code %d\n", LAST_ERROR);
	}

	while ( LocalAddresses.Count() ) {
		delete LocalAddresses[0];
		LocalAddresses.Delete_Index(0);
	}

	unsigned int **addresses = (unsigned int**) (host_info->h_addr_list);

	for ( ;; ) {
		if ( !*addresses ) break;

		unsigned int address = **addresses++;

		DebugString("Found local address: %d.%d.%d.%d\n", address & 0xff, (address & 0xff00) >> 8, (address & 0xff0000) >> 16, (address & 0xff000000) >> 24);

		unsigned char *a = new unsigned char [4];
		* ((unsigned int*) a) = address;
		LocalAddresses.Add (a);
	}

	DebugString("About to open alternate UDP socket\n");

	AlternateSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (AlternateSocket == INVALID_SOCKET) {
		return(false);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons ( (unsigned short) DAT_00841F38);
	addr.sin_addr.s_addr = htonl (INADDR_ANY);

	DebugString("About to bind the UDP socket\n");

	if ( bind (AlternateSocket, (LPSOCKADDR)&addr, sizeof(addr) ) == SOCKET_ERROR) {
		Close_Socket ();
		return(false);
	}

	ling.l_onoff = 0;
	ling.l_linger = 0;
	setsockopt (AlternateSocket, SOL_SOCKET, SO_LINGER, (LPSTR)&ling, sizeof(ling));

	BASECLASS::Set_Socket_Options(AlternateSocket);

	DebugString("Alternate UDP Socket init complete\n");

	return(true);
}

// FUNCTION: GAMEMD 0x007B36C0
void UDPInterfaceClass::WriteTo (void *buffer, int buffer_len, void *address, int address_len, bool alternate, bool flag, unsigned short port)
{
	if ( address && ((IPXAddressClass *)address)->Is_Broadcast() ) {
		Broadcast ( buffer, buffer_len, alternate );
		return;
	}
	BASECLASS::WriteTo ( buffer, buffer_len, address, address_len, alternate, flag, port );
}

// FUNCTION: GAMEMD 0x007B3720
void UDPInterfaceClass::Broadcast (void *buffer, int buffer_len, bool alternate)
{
	for ( int i=0 ; i<BroadcastAddresses.Count() ; i++ ) {
		WinsockBufferType *packet = (WinsockBufferType *)Get_New_Out_Buffer(alternate);

		memcpy ( packet->Buffer, buffer, buffer_len );
		packet->BufferLen = buffer_len;
		packet->IsBroadcast = true;

		memset (packet->Address, 0, sizeof (packet->Address));
		memcpy (packet->Address+4, BroadcastAddresses[i], 4);
		packet->SrcPort = BroadcastPorts[i];

		Build_Packet_CRC(packet);

		SOCKET send_socket;
		if ( alternate ) {
			AltOutBuffers.Add ( packet );
			send_socket = AlternateSocket;
		} else {
			OutBuffers.Add ( packet );
			send_socket = Socket;
		}

		SendMessage ( Game::hWnd, Protocol_Event_Message(send_socket), 0, (LONG)FD_WRITE );

		Windows_Message_Handler();
	}
}

// FUNCTION: GAMEMD 0x007B3890
bool UDPInterfaceClass::Start_Listening (void)
{
	if (BASECLASS::Start_Listening()) {
		for (int i = 0; i < PortCount; i++) {
			if (WSAAsyncSelect(PortSockets[i], Game::hWnd, Protocol_Event_Message(INVALID_SOCKET), FD_READ | FD_WRITE) == SOCKET_ERROR) {
				DebugString("Async select failed on spare socket.\n");
				return(false);
			}
		}
	}
	Windows_Message_Handler();
	return(true);
}

// FUNCTION: GAMEMD 0x007B3900
void UDPInterfaceClass::Stop_Listening (void)
{
	BASECLASS::Stop_Listening();
	Close_All_Port_Sockets();
}

// FUNCTION: GAMEMD 0x007B3920
void UDPInterfaceClass::Add_Peer (unsigned char *address, unsigned short port)
{
	if (PeerCount < 16) {
		for (int i = 0; i < PeerCount; i++) {
			if (port == PeerPorts[i] && *(unsigned int *)address == PeerAddresses[i]) {
				return;
			}
		}
		DebugString("Marking address %d.%d.%d.%d ; %d as unreachable\n", address[0], address[1], address[2], address[3], port);
		PeerAddresses[PeerCount] = *(unsigned int *)address;
		PeerPorts[PeerCount] = port;
		PeerCount++;
	}
}

// FUNCTION: GAMEMD 0x007B39D0
bool UDPInterfaceClass::Is_Reachable (unsigned char *address, unsigned short port)
{
	for (int i = 0; i < PeerCount; i++) {
		if (port == PeerPorts[i] && *(unsigned int *)address == PeerAddresses[i]) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x007B3A20
void UDPInterfaceClass::Close_All_Port_Sockets (void)
{
	for (int i = 0; i < PortCount; i++) {
		DebugString("Closing spare socket %d\n", PortSockets[i]);
		WSAAsyncSelect(PortSockets[i], Game::hWnd, 0, 0);
		closesocket(PortSockets[i]);
	}
	PortCount = 0;
	SelectedSocket = INVALID_SOCKET;
}

// FUNCTION: GAMEMD 0x007B40F0
ProtocolEnum UDPInterfaceClass::Get_Protocol (void)
{
	return(PROTOCOL_UDP);
}

// FUNCTION: GAMEMD 0x007B4100
int UDPInterfaceClass::Protocol_Event_Message (SOCKET socketnum)
{
	if (socketnum == Socket || socketnum == INVALID_SOCKET) {
		return(WM_UDPASYNCEVENT);
	}
	return(socketnum == AlternateSocket ? WM_UDPASYNCEVENT + 1 : 0);
}

// FUNCTION: GAMEMD 0x007B4140
int UDPInterfaceClass::Get_Num_Local_Addresses (void)
{
	return(LocalAddresses.Count());
}

// FUNCTION: GAMEMD 0x007B4150
unsigned char *UDPInterfaceClass::Get_Local_Address (int index)
{
	return(LocalAddresses[index]);
}

// FUNCTION: GAMEMD 0x007B4160
unsigned short UDPInterfaceClass::Get_Port (void)
{
	return(Port);
}

// SYNTHETIC: GAMEMD 0x007B4170
// ??_GUDPInterfaceClass@@UAEPAXI@Z
