// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxgconn.h"
#include "coord.h"
#include "dbgprint.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include "_timer.h"
#include "globals.h"
#include "session.h"
#include "wsproto.h"

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EC0F8;
extern const double DAT_007EC100;
extern const double DAT_007EC158;
extern const double DAT_007EC160;
void __cdecl FUN_004068E0(char const * format, ...);
extern int DAT_00841F3C;
extern WinsockInterfaceClass * DAT_00887628;
struct Unknown_00B052D0 {
	unsigned char FUN_00663D60(int);
};
extern Unknown_00B052D0 DAT_00B052D0;

// FUNCTION: 0x0053F8A0 $E40
double DAT_00AA05D0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0053F8D0 $E43
double DAT_00AA0620 = DAT_00AA05D0 / DAT_007EC0F8;

// FUNCTION: 0x0053F8F0 $E46
double DAT_00AA0608 = DAT_007E1718 / DAT_00AA0620;

// FUNCTION: 0x0053F910 $E49
double DAT_00AA0600 = DAT_007E1720 * DAT_007EC100;

// FUNCTION: 0x0053F930 $E52
double DAT_00AA05D8 = DAT_007E1728 * DAT_007EC100;

// FUNCTION: 0x0053F950 $E55
double DAT_00AA05B0 = DAT_007E1730 * DAT_007EC100;

// FUNCTION: 0x0053F970 $E58
int DAT_00AA0628 = (int)(DAT_007E1738 * (fastmath::tan(DAT_00AA05B0 - DAT_00AA05D8) * DAT_00AA05D0));

// FUNCTION: 0x0053F9B0 $E61
double DAT_00AA05E0 = fastmath::atan(DAT_00AA0628 * DAT_007E1740);

// FUNCTION: 0x0053F9E0 $E64
double DAT_00AA05F8 = fastmath::atan((DAT_00AA0628 * 2) / DAT_00AA05D0);

// FUNCTION: 0x0053FA10 $E67
int DAT_00AA061C = (int)((DAT_00AA0628 * 4) + DAT_007E1738);

// FUNCTION: 0x0053FA40 $E70
Cell DAT_00AA05C8(0, 0);

// FUNCTION: 0x0053FA50 $E73
Coord DAT_00AA05E8(0, 0, 0);

// FUNCTION: 0x0053FA70 $E76
Coord DAT_00AA0610(128, 128, 0);

// FUNCTION: 0x0053FA90
IPXGlobalConnClass::IPXGlobalConnClass (int numsend, int numreceive, int maxlen,
	unsigned short product_id, unsigned short magicnum) :
	BASECLASS (numsend, numreceive, maxlen + 2, magicnum, NULL, 0, L"", sizeof (IPXAddressClass))
{
	ProductID = product_id;
	IsBridge = 0;
	FieldC8 = true;
	MaxRXIndex = numreceive;
	LastPacketID = new unsigned int[MaxRXIndex];
	LastAddress = new IPXAddressClass[MaxRXIndex];

	for (int i = 0; i < MaxRXIndex; i++) {
		LastPacketID[i] = 0xffffffff;
	}

	LastRXIndex = 0;
}

// FUNCTION: 0x0053FB70
// FUNCTION: 0x00540670
IPXGlobalConnClass::~IPXGlobalConnClass(void)
{
	if (LastPacketID) {
		delete [] LastPacketID;
		LastPacketID = NULL;
	}
	if (LastAddress) {
		delete [] LastAddress;
		LastAddress = NULL;
	}
}

// FUNCTION: 0x0053FBD0
int IPXGlobalConnClass::Send_Packet (void * buf, int buflen,
	IPXAddressClass *address, int ack_req, unsigned short port, int packet_id)
{
	IPXAddressClass dest_addr;

	((GlobalHeaderType *)PacketBuf)->Field8 = 0;
	((GlobalHeaderType *)PacketBuf)->FieldC = 0;
	((GlobalHeaderType *)PacketBuf)->MagicNumber = MagicNum;

	if (ack_req && address != NULL) {
		((GlobalHeaderType *)PacketBuf)->Code = PACKET_DATA_ACK;
	}
	else {
		((GlobalHeaderType *)PacketBuf)->Code = PACKET_DATA_NOACK;
	}

	((GlobalHeaderType *)PacketBuf)->PacketID = Queue->Send_Total();
	if (packet_id != 0) {
		((GlobalHeaderType *)PacketBuf)->PacketID = packet_id;
	}

	((GlobalHeaderType *)PacketBuf)->ProductID = ProductID;
	((GlobalHeaderType *)PacketBuf)->Field3 = 0;

	if (address != NULL) {
		dest_addr = (*address);
	}

	memcpy(PacketBuf + sizeof(GlobalHeaderType), buf, buflen);

	return(Queue->Queue_Send(PacketBuf, buflen + sizeof(GlobalHeaderType),
		&dest_addr, sizeof (IPXAddressClass), port));
}

// FUNCTION: 0x0053FF10
int IPXGlobalConnClass::Get_Packet (void * buf, int *buflen,
	IPXAddressClass *address, unsigned short *product_id)
{
	ReceiveQueueType *rec_entry;
	GlobalHeaderType *packet;
	int packetlen;

	if (Queue->Num_Receive() == 0) {
		return(0);
	}

	rec_entry = Queue->Get_Receive(0);

	if (rec_entry != NULL && rec_entry->IsRead == 0) {
		rec_entry->IsRead = 1;

		packet = (GlobalHeaderType *)(rec_entry->Buffer);
		packetlen = rec_entry->BufLen - sizeof(GlobalHeaderType);
		if (packetlen > 0) {
			memcpy(buf, rec_entry->Buffer + sizeof(GlobalHeaderType), packetlen);
		}
		(*buflen) = packetlen;
		(*product_id) = packet->ProductID;
		(*address) = (*((IPXAddressClass *)(rec_entry->ExtraBuffer)));

		return(1);
	}

	return(0);
}

// FUNCTION: 0x0053FFA0
int IPXGlobalConnClass::vt_entry_38(int index, int buf, int buflen, int address, int product_id, int packet_id)
{
	ReceiveQueueType *rec_entry;
	GlobalHeaderType *packet;
	int packetlen;

	if (Queue->Num_Receive() >= index) {
		rec_entry = Queue->Get_Receive(index);

		if (rec_entry != NULL && rec_entry->IsRead == 0) {
			packet = (GlobalHeaderType *)(rec_entry->Buffer);
			packetlen = rec_entry->BufLen - sizeof(GlobalHeaderType);
			if (packetlen > 0) {
				memcpy((void *)buf, rec_entry->Buffer + sizeof(GlobalHeaderType), packetlen);
			}
			*(int *)buflen = packetlen;
			*(unsigned short *)product_id = packet->ProductID;
			*(IPXAddressClass *)address = *((IPXAddressClass *)(rec_entry->ExtraBuffer));
			if (packet_id != 0) {
				*(unsigned int *)packet_id = packet->PacketID;
			}
			return(1);
		}
	}

	return(0);
}

// FUNCTION: 0x00540030
void IPXGlobalConnClass::vt_entry_3C(int index)
{
	if (index < Queue->Num_Receive()) {
		Queue->Get_Receive(index)->IsRead = 1;
	}
}

// FUNCTION: 0x00540050
int IPXGlobalConnClass::Send(char *buf, int buflen, void *extrabuf, int extralen, bool flag, unsigned short port)
{
	IPXAddressClass *addr = (IPXAddressClass *)extrabuf;

	if (addr->Is_Broadcast()) {
		return(Broadcast (buf, buflen));
	}

	if (Session.Type == GAME_IPX && IsBridge && !memcmp (addr, BridgeNet, 4)) {
		return(Send_To (buf, buflen, addr, BridgeNode, flag, 0));
	}
	return(Send_To (buf, buflen, addr, NULL, flag, port));
}

// FUNCTION: 0x005400D0
int IPXGlobalConnClass::Service_Receive_Queue (void)
{
	int i;
	ReceiveQueueType *rec_entry;

	for (i = 0; i < Queue->Num_Receive(); i++) {
		rec_entry = Queue->Get_Receive(i);

		if (rec_entry->IsRead) {
			Queue->UnQueue_Receive(NULL,NULL,i,NULL,NULL);
			i--;
		}
	}

	return(1);
}

// FUNCTION: 0x00540110
void IPXGlobalConnClass::vt_entry_44(unsigned age)
{
	FUN_004068E0("Stripping queue entries older than %d seconds. Num receive = %d\n", age / 60, Queue->Num_Receive());

	for (int i = 0; i < Queue->Num_Receive(); i++) {
		ReceiveQueueType *entry = Queue->Get_Receive(i);

		FUN_004068E0("i = %d\n", i);
		if (entry != NULL) {
			FUN_004068E0("Checking packet %d\n", i);

			if ((int)(entry->ReceiveTime + age) < (int)TickCount) {
				FUN_004068E0("Stripping global packet due to age of %d seconds\n",
					((int)TickCount - (int)entry->ReceiveTime) / 60);

				unsigned int id = ((GlobalHeaderType *)entry->Buffer)->PacketID;
				for (int j = 0; j < MaxRXIndex; j++) {
					if (LastPacketID[j] == id) {
						LastPacketID[j] = 0xffffffff;
						break;
					}
				}
				Queue->UnQueue_Receive(NULL, NULL, i, NULL, NULL);
				i--;
			}
			else {
				FUN_004068E0("Leaving global packet due to age of %d seconds\n",
					((int)TickCount - (int)entry->ReceiveTime) / 60);
			}
		}
	}

	FUN_004068E0("Done stripping. Num receive = %d\n", Queue->Num_Receive());
}

// FUNCTION: 0x005402B0
void IPXGlobalConnClass::Set_Bridge(NetNumType bridge)
{
	if (Configured) {
		IsBridge = 0;
	}
}

// FUNCTION: 0x005402D0
int IPXGlobalConnClass::Discard_Undeliverable_Packets(void)
{
	int num = 0;

	for (int i = 0; i < Queue->Num_Send(); i++) {
		SendQueueType *send_entry = Queue->Get_Send(i);

		if (send_entry->IsUndeliverable && !send_entry->IsACK) {
			IPXAddressClass address;
			char buf[1024];
			int buflen;
			int address_size = sizeof(address);

			Queue->UnQueue_Send(buf, &buflen, i, &address, &address_size, NULL);
			i--;
			num++;
		}
	}

	return(num);
}

// FUNCTION: 0x00540340
int IPXGlobalConnClass::vt_entry_40(IPXAddressClass *address, unsigned short port)
{
	int num = 0;

	for (int i = 0; i < Queue->Num_Send(); i++) {
		SendQueueType *send_entry = Queue->Get_Send(i);

		if (!send_entry->IsACK && send_entry->ExtraLen &&
			((IPXAddressClass *)send_entry->ExtraBuffer)->FUN_0053EF30(*address) &&
			send_entry->Port == port) {
			IPXAddressClass dest;
			char buf[1024];
			int buflen;
			int address_size = sizeof(dest);

			Queue->UnQueue_Send(buf, &buflen, i, &dest, &address_size, NULL);
			i--;
			num++;
		}
	}

	return(num);
}

// FUNCTION: 0x005403F0
int IPXGlobalConnClass::Send_To(char *buf, int buflen, IPXAddressClass *address,
	NetNodeType immed, bool flag, unsigned short port)
{
	NetNumType net;
	NetNodeType node;
	IPXAddressClass dest;
	unsigned short dest_port;

	dest = *address;
	if (immed) {
		address->Get_Address(net, node);
		memcpy(net, immed, 4);
		dest.Set_Address(net, node, 0);
	}

	if (Session.Type == GAME_INTERNET && !flag && Session.ViaPacketRouter && FieldC8) {
		if (!((GlobalHeaderType *)buf)->Field3) {
			IPXAddressClass target = dest;

			target.Get_Address(net, node);
			target.Set_Address(net, node, port);
			int id = -1;
			for (int i = 0; i < Session.Players.Count(); i++) {
				if (Session.Players[i]->Address == target) {
					id = Session.Players[i]->Player.ID;
					break;
				}
			}
			unsigned char result = DAT_00B052D0.FUN_00663D60(id);
			if (!result) {
				result = 1;
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
		dest_port = port;
	}

	if (DAT_00887628) {
		DAT_00887628->WriteTo(buf, buflen, &dest, sizeof(dest), flag, FieldC8 == 0, dest_port);
	}
	return(1);
}

// FUNCTION: 0x00540610
int IPXGlobalConnClass::Send_Packet (void * buf, int buflen, int ack_req, int extra)
{
	return BASECLASS::Send_Packet(buf, buflen, ack_req, extra);
}

// FUNCTION: 0x00540630
int IPXGlobalConnClass::Receive_Packet (void * buf, int buflen)
{
	return BASECLASS::Receive_Packet(buf, buflen);
}

// FUNCTION: 0x00540650
int IPXGlobalConnClass::Get_Packet (void * buf, int * buflen)
{
	return BASECLASS::Get_Packet(buf, buflen);
}

// FUNCTION: 0x005406E0 $E80
double DAT_00AA0650 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00540710 $E83
double DAT_00AA06A0 = DAT_00AA0650 / DAT_007EC158;

// FUNCTION: 0x00540730 $E86
double DAT_00AA0688 = DAT_007E1718 / DAT_00AA06A0;

// FUNCTION: 0x00540750 $E89
double DAT_00AA0680 = DAT_007E1720 * DAT_007EC160;

// FUNCTION: 0x00540770 $E92
double DAT_00AA0658 = DAT_007E1728 * DAT_007EC160;

// FUNCTION: 0x00540790 $E95
double DAT_00AA0630 = DAT_007E1730 * DAT_007EC160;
