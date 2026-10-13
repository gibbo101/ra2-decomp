// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "connect.h"
#include "ipxaddr.h"

class IPXConnClass : public ConnectionClass
{
		typedef ConnectionClass BASECLASS;

	public:

		enum IPXConnTag {
			CONN_NAME_MAX = 40
		};

		IPXConnClass(int numsend, int numrecieve, int maxlen,
			unsigned short magicnum, IPXAddressClass *address, int id, wchar_t const *name,
			int extralen = 0);
		virtual ~IPXConnClass () override {};

		virtual void Init (void) override;

		static void Configure(unsigned short socket,
			int conn_num,
			IPXHeaderType *listen_header, IPXHeaderType *send_header,
			char *listen_buf, char *send_buf,
			int maxpacketlen);

		static int Start_Listening (void);
		static int Stop_Listening (void);

		IPXAddressClass Address;

		NetNodeType ImmediateAddress;
		int Immed_Set;

		int ID;
		wchar_t Name[CONN_NAME_MAX];

	protected:

		virtual int Send (char *buf, int buflen, void *extrabuf, int extralen) override;

		static int Open_Socket(unsigned short socket);
		static void Close_Socket(unsigned short socket);
		virtual int Send_To(char *buf, int buflen, IPXAddressClass *address,
			NetNodeType immed, bool flag, unsigned short port);
		static int Broadcast(char *buf, int buflen);

		static unsigned short Socket;

		static int ConnectionNum;

		static int PacketLen;

		static IPXHeaderType *ListenHeader;
		static char *ListenBuf;

		static IPXHeaderType *SendHeader;
		static char *SendBuf;

		static int Configured;

		static int SocketOpen;

		static int Listening;
};

