// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "ipxconn.h"

#pragma pack(push,1)

struct GlobalHeaderType {
	unsigned short MagicNumber;
	unsigned char Code;
	unsigned char Field3;
	unsigned int PacketID;
	unsigned int Field8;
	unsigned short FieldC;
	unsigned short ProductID;
};
#pragma pack(pop)

class IPXGlobalConnClass : public IPXConnClass
{
		typedef IPXConnClass BASECLASS;

	public:

		enum GlobalConnectionEnum {

			GLOBAL_MAGICNUM = 0x1235,

			COMMAND_AND_CONQUER4 = 0xaa04,
			COMMAND_AND_CONQUER3 = 0xaa03,
			COMMAND_AND_CONQUER2 = 0xaa02,
			COMMAND_AND_CONQUER1 = 0xaa01,
			COMMAND_AND_CONQUER0 = 0xaa00,
		};

		IPXGlobalConnClass (int numsend, int numrecieve, int maxlen,
			unsigned short product_id, unsigned short magicnum);
		virtual ~IPXGlobalConnClass () override;

		virtual int Send_Packet (void * buf, int buflen, int ack_req, int extra) override;
		virtual int Receive_Packet (void * buf, int buflen) override;
		virtual int Get_Packet (void * buf, int * buflen) override;

		virtual int Send_Packet (void * buf, int buflen,
			IPXAddressClass *address, int ack_req, unsigned short port, int packet_id);
		virtual int Receive_Packet (void * buf, int buflen,
			IPXAddressClass *address, unsigned short port);
		virtual int Get_Packet (void * buf, int *buflen,
			IPXAddressClass *address, unsigned short *product_id);
		virtual int vt_entry_38(int a1, int a2, int a3, int a4, int a5, int a6);
		virtual void vt_entry_3C(int a1);
		virtual int vt_entry_40(int a1, int a2);
		virtual void vt_entry_44(unsigned a1);

		virtual int Discard_Undeliverable_Packets(void) override;

		void Set_Bridge (NetNumType bridge);

		unsigned short ProductID;

		NetNumType BridgeNet;
		NetNodeType BridgeNode;
		int IsBridge;
		bool FieldC8;

	protected:

		virtual int Send (char *buf, int buflen, void *extrabuf, int extralen, bool flag, unsigned short port) override;
		virtual int Send_To(char *buf, int buflen, IPXAddressClass *address,
			NetNodeType immed, bool flag, unsigned short port) override;

		virtual int Service_Receive_Queue (void) override;

	private:

		IPXAddressClass *LastAddress;
		unsigned int *LastPacketID;
		int MaxRXIndex;
		int LastRXIndex;
};

