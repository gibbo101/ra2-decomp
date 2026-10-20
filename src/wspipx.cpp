// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "wspipx.h"

#include "_winfix.h"
#include "assert.h"
#include "dbgprint.h"
#include "ipxaddr.h"
#include "random.h"
#include "vector.h"

#include <cstdio>

namespace Randomizer { extern Random2Class Global; }
extern int __cdecl FUN_007B2730(void);
extern bool DAT_00B7801C;

// FUNCTION: 0x007B0D90
IPXInterfaceClass::IPXInterfaceClass (void) : BASECLASS()
{
	memset ( BroadcastNet, 0xff, sizeof (BroadcastNet) );
	memset ( BroadcastNode, 0xff, sizeof (BroadcastNode) );
	memset ( MyNode, 0xff, sizeof (MyNode) );
}

// FUNCTION: 0x007B0DD0
int IPXInterfaceClass::Get_Network_Card_Address (int card_number, SOCKADDR_IPX *addr)
{
	int            	cbOpt;
	int					cbAddr = 14;
	SOCKET         	s;
	SOCKADDR_IPX   	Addr;
	IPX_ADDRESS_DATA  IpxData;

	s = socket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX );
	if ( s == SOCKET_ERROR ) {
		return(0);
	}

	memset( &Addr, 0, sizeof( Addr ));
	Addr.sa_family = AF_IPX;
	int err = bind( s, (SOCKADDR*) &Addr, cbAddr);
	if ( err == SOCKET_ERROR ) {
		closesocket (s);
		return(0);
	}

	int adapter_count = 1;
	int adapter_count_len = sizeof(adapter_count);

	if (!WinVersion.Is_Win95()) {
		if (getsockopt(s, NSPROTO_IPX, IPX_MAX_ADAPTER_NUM, (char*) &adapter_count, &adapter_count_len) == 0) {
			if ( card_number >= adapter_count) {
				return(0);
			}
		} else {
			DebugString("getsockopt with IPX_MAX_ADAPTER_NUM failed with error code %d\n", LAST_ERROR);
		}
	} else {
		DebugString("Skipping IPX_MAX_ADAPTER_NUM call due to bug in Win95\n");
	}

	memset( &IpxData, 0, sizeof(IpxData));
	IpxData.adapternum = card_number;
	cbOpt = sizeof( IpxData );

	err = getsockopt( s, NSPROTO_IPX, IPX_ADDRESS, (char*) &IpxData, &cbOpt );
	if ( err == SOCKET_ERROR ) {
		DebugString("Failed to get address of net card %d - error code %d.\n", card_number, LAST_ERROR);
		closesocket (s);
		return(0);
	}

	memcpy (addr->sa_netnum, IpxData.netnum, sizeof (addr->sa_netnum));
	memcpy (BroadcastNet, IpxData.netnum, sizeof (addr->sa_netnum));
	memcpy (addr->sa_nodenum, IpxData.nodenum, sizeof (addr->sa_nodenum));

	closesocket (s);
	return(1);
}

// FUNCTION: 0x007B0F50
int IPXInterfaceClass::Get_Network_Card_Address_As_String(int card_number, char *buffer, int length)
{
	SOCKADDR_IPX address;
	char buf[256];

	if (!WinsockInitialised) {
		return(0);
	}
	{
		int result = Get_Network_Card_Address(card_number, &address);
		if (result) {
			if (result == 2) {
				sprintf(
					buf,
					"?? ?? ?? ?? : %02x %02x %02x %02x %02x %02x",
					address.sa_nodenum[0] & 0xff,
					address.sa_nodenum[1] & 0xff,
					address.sa_nodenum[2] & 0xff,
					address.sa_nodenum[3] & 0xff,
					address.sa_nodenum[4] & 0xff,
					address.sa_nodenum[5] & 0xff
					);
			} else {
				sprintf(
					buf,
					"%02x %02x %02x %02x : %02x %02x %02x %02x %02x %02x",
					address.sa_netnum[0] & 0xff,
					address.sa_netnum[1] & 0xff,
					address.sa_netnum[2] & 0xff,
					address.sa_netnum[3] & 0xff,
					address.sa_nodenum[0] & 0xff,
					address.sa_nodenum[1] & 0xff,
					address.sa_nodenum[2] & 0xff,
					address.sa_nodenum[3] & 0xff,
					address.sa_nodenum[4] & 0xff,
					address.sa_nodenum[5] & 0xff
					);
			}
			if ((int)strlen(buf) >= length) {
				return(0);
			}
			strcpy(buffer, buf);
		}
		return(result);
	}
}

// FUNCTION: 0x007B10C0
bool IPXInterfaceClass::Open_Socket( SOCKET socketnum )
{
	SOCKADDR_IPX 	addr;
	int				err;

	if ( !WinsockInitialised ) {
		if ( !Init()) return( false );
	} else if (Socket != INVALID_SOCKET) {
		return( false );
	}

	IPXSocketNumber = socketnum;

	DebugString("IPX Socket number is %04x\n", socketnum);

	addr.sa_family = AF_IPX;
	memset (addr.sa_netnum, 0, sizeof (addr.sa_netnum));
	memset (addr.sa_nodenum, -1, sizeof (addr.sa_nodenum));
	addr.sa_socket = htons ( socketnum );

	DebugString("About to create an IPX socket\n");
	Socket = socket (AF_NS, SOCK_DGRAM, NSPROTO_IPX);
	if (Socket == INVALID_SOCKET) {
		DebugString("Failed to create IPX socket - error code %d.\n", LAST_ERROR );
		closesocket(Socket);
		return( false );
	}

	DebugString("IPX Socket created OK\n");

	if ( !Get_Network_Card_Address (NetCard, &addr)){
		DebugString("IPX Error - failed to get the net card address\n");
		closesocket ( Socket );
		return( false );
	}

	DebugString(
		"IPX net card address is %02x %02x %02x %02x : %02x %02x %02x %02x %02x %02x\n",
		(unsigned char)addr.sa_netnum[0],
		(unsigned char)addr.sa_netnum[1],
		(unsigned char)addr.sa_netnum[2],
		(unsigned char)addr.sa_netnum[3],
		(unsigned char)addr.sa_nodenum[0],
		(unsigned char)addr.sa_nodenum[1],
		(unsigned char)addr.sa_nodenum[2],
		(unsigned char)addr.sa_nodenum[3],
		(unsigned char)addr.sa_nodenum[4],
		(unsigned char)addr.sa_nodenum[5]);

	memcpy(MyNode, addr.sa_nodenum, sizeof(MyNode));

	DebugString("About to bind IPX Socket\n");
	if (bind ( Socket, (const struct sockaddr *) &addr, 16) == SOCKET_ERROR ){
		DebugString ("IPX socket bind failed with error code %d.\n", LAST_ERROR );
		closesocket(Socket);
		return( false );
	}

	DebugString("IPX Socket bound OK\n");

	unsigned int 	optval = true;
	int 	packet_type = 4;

	DebugString("About to set IPX socket option SO_BROADCAST\n");
	if ( setsockopt ( Socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval,	sizeof(optval) ) == SOCKET_ERROR ) {
		DebugString ("Failed to set IPX socket option SO_BROADCAST - error code %d.\n", LAST_ERROR );
	} else {
		DebugString("IPX socket option SO_BROADCAST set OK\n");
	}

	DebugString("About to set IPX socket option IPX_PTYPE\n");
	err = setsockopt ( Socket, NSPROTO_IPX, IPX_PTYPE, (char*)&packet_type,	sizeof(packet_type));
	if ( err == INVALID_SOCKET ) {
		DebugString ("Failed to set IPX protocol option IPX_PTYPE - error code %d.\n", LAST_ERROR );
	} else {
		DebugString("IPX socket option IPX_PTYPE set OK\n");
	}

	DebugString("About to set IPX socket option IPX_FILTERTYPE\n");
	err = setsockopt ( Socket, NSPROTO_IPX, IPX_FILTERPTYPE, (char*)&packet_type,	sizeof(packet_type));
	if ( err == INVALID_SOCKET ) {
		DebugString ("Failed to set IPX protocol option IPX_FILTERTYPE - error code %d.\n", LAST_ERROR );
	} else {
		DebugString("IPX socket option IPX_FILTERTYPE set OK\n");
	}

	BASECLASS::Set_Socket_Options();

	DebugString("IPX Socket init complete\n");
	return( true );
}

// FUNCTION: 0x007B13D0
int IPXInterfaceClass::Message_Handler(HWND , UINT message, UINT , LONG lParam)
{
	int					addr_len;
	int	 				rc;
	SOCKADDR_IPX 		addr;
	WinsockBufferType *packet;
	NetNumType			netnum;
	NetNodeType			nodenum;

	if ( message != WM_IPXASYNCEVENT ) return( 1 );

	switch ( WSAGETSELECTEVENT(lParam) ) {

		case FD_READ:
			rc = WSAGETSELECTERROR(lParam);
			if (rc != 0) {
				Clear_Socket_Error (Socket);
				return(0);
			}

			addr_len = sizeof(addr);
			rc = recvfrom ( Socket, (char*) ReceiveBuffer, sizeof (ReceiveBuffer), 0, (LPSOCKADDR)&addr, &addr_len );
			if (rc == SOCKET_ERROR) {
				if (LAST_ERROR != WSAEWOULDBLOCK) {
					Clear_Socket_Error (Socket);
				}
				return(0);
			}

			if ( rc ) {
				memcpy ( netnum, addr.sa_netnum, sizeof (netnum) );
				memcpy ( nodenum, addr.sa_nodenum, sizeof (nodenum) );

				if ( !memcmp (netnum, BroadcastNet, sizeof (BroadcastNet)) && !memcmp(nodenum, MyNode, sizeof (MyNode)) ) {
					return(0);
				}

				int loss = FUN_007B2730();
				if (loss > 0) {
					if (Randomizer::Global(0, 99) <= loss) {
						return(0);
					}
				}

				packet = (WinsockBufferType *)Get_New_In_Buffer();
				packet->BufferLen = rc - sizeof(packet->CRC);
				packet->CRC = *((unsigned int*) (&ReceiveBuffer[0]));
				memcpy ( packet->Buffer, ReceiveBuffer + sizeof(packet->CRC), packet->BufferLen );

				if (!Passes_CRC_Check(packet)) {
					DebugString("Throwing away malformed packet\n");
					if (!DAT_00B7801C) {
						DAT_00B7801C = true;
					}
					if (packet->IsAllocated) {
						delete packet;
					} else {
						packet->InUse = false;
						OutBuffersUsed--;
					}
				} else {
					IPXAddressClass *paddress = (IPXAddressClass*) (&packet->Address[0]);
					paddress->Set_Address ( netnum, nodenum, 0 );
					InBuffers.Add ( packet );
				}
			}
			return(0);

		case FD_WRITE:
			rc = WSAGETSELECTERROR(lParam);
			if (rc != 0) {
				Clear_Socket_Error ( Socket );
				return(0);
			}

			while ( OutBuffers.Count() != 0 ) {
				int packetnum = 0;

				packet = OutBuffers [ packetnum ];

				addr.sa_family = AF_IPX;
				addr.sa_socket = htons ( IPXSocketNumber );

				if ( packet->IsBroadcast ) {
					memcpy ( addr.sa_netnum, BroadcastNet, sizeof (BroadcastNet) );
					memcpy ( addr.sa_nodenum, BroadcastNode, sizeof (BroadcastNode) );
				}else{
					IPXAddressClass *paddress = (IPXAddressClass*) (&packet->Address[0]);
					paddress->Get_Address ( netnum, nodenum );
					memcpy ( addr.sa_netnum, netnum, sizeof (netnum) );
					memcpy ( addr.sa_nodenum, nodenum, sizeof (nodenum) );
				}

				rc = sendto ( Socket, ((const char*) packet->Buffer) - sizeof(packet->CRC), packet->BufferLen + sizeof(packet->CRC), 0, (LPSOCKADDR)&addr, sizeof (addr) );

				if (rc == SOCKET_ERROR){
					if (LAST_ERROR != WSAEWOULDBLOCK) {
						Clear_Socket_Error (Socket);
						break;
					}
				}

				OutBuffers.Delete_Index(0);
				if (packet->IsAllocated) {
					delete packet;
				} else {
					packet->InUse = 0;
					OutBuffersUsed--;
				}
			}

			return(0);
	}

	return(0);
}

// FUNCTION: 0x007B1720 WinsockInterfaceClass::Set_Broadcast_Address

// FUNCTION: 0x007B1730 WinsockInterfaceClass::Clear_Broadcast_Addresses

// FUNCTION: 0x007B1740 WinsockInterfaceClass::Get_Num_Local_Addresses

// FUNCTION: 0x007B1750 WinsockInterfaceClass::Get_Local_Address
