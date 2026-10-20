// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "wsproto.h"

#include <wsipx.h>
#include <wsnwlink.h>

class IPXInterfaceClass : public WinsockInterfaceClass {
		typedef WinsockInterfaceClass BASECLASS;

	public:

		IPXInterfaceClass (void);
		int Get_Network_Card_Address (int card_number, SOCKADDR_IPX *addr);
		virtual int Message_Handler(HWND window, UINT message, UINT wParam, LONG lParam) override;
		virtual bool Open_Socket ( SOCKET socketnum ) override;

		virtual ProtocolEnum Get_Protocol (void) override {
			return(PROTOCOL_IPX);
		};

		virtual int Protocol_Event_Message (SOCKET) override {
			return(WM_IPXASYNCEVENT);
		};

		int Get_Network_Card_Address_As_String(int card_number, char *buffer, int length);

	private:
		// Broadcast network and node; the network is the local one, since ff,ff,ff,ff fails on NT 4.0.
		unsigned char	BroadcastNet[4];
		unsigned char	BroadcastNode[6];

		unsigned char	MyNode[6];
		SOCKET			IPXSocketNumber;
};
