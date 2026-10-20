// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "wsproto.h"

#include <nspapi.h>

class UDPInterfaceClass : public WinsockInterfaceClass {
		typedef WinsockInterfaceClass BASECLASS;

	public:

		UDPInterfaceClass (void);
		virtual ~UDPInterfaceClass(void) override;

		virtual void WriteTo (void *buffer, int buffer_len, void *address, int address_len, bool alternate, bool flag, unsigned short port) override;
		virtual void Broadcast (void *buffer, int buffer_len, bool alternate) override;
		virtual bool Start_Listening (void) override;
		virtual void Stop_Listening (void) override;
		virtual void Set_Broadcast_Address ( char const * address, unsigned short port ) override;
		virtual void Clear_Broadcast_Addresses(void) override;

		virtual ProtocolEnum Get_Protocol (void) override;
		virtual int Protocol_Event_Message (SOCKET socketnum) override;
		virtual bool Open_Socket ( SOCKET socketnum ) override;
		virtual int Message_Handler(HWND window, UINT message, UINT wParam, LONG lParam) override;
		virtual int Get_Num_Local_Addresses(void) override;
		virtual unsigned char *Get_Local_Address(int index) override;

		virtual bool OpenSocket (unsigned short port = 0);
		virtual unsigned short Get_Port (void);
		virtual bool Init_Ports (void);
		virtual bool Open_Port_Socket (unsigned short port);
		virtual void Close_Port_Socket (unsigned short port);
		virtual void Close_All_Port_Sockets (void);
		virtual void Select_Port_Socket (unsigned short port);
		virtual void Add_Peer (unsigned char *address, unsigned short port);

		static int UDPListenPort;

	private:

		unsigned short Port;
		SOCKET PortSockets[8];
		unsigned short PortNumbers[8];
		int PortCount;
		SOCKET SelectedSocket;

		DynamicVectorClass <unsigned char *> BroadcastAddresses;
		unsigned short BroadcastPorts[256];
		DynamicVectorClass <unsigned char *> LocalAddresses;

		int PeerCount;
		unsigned int PeerAddresses[16];
		unsigned short PeerPorts[16];
};
