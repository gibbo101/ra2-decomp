// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "connect.h"
#include "_timer.h"
#include "dbgprint.h"
#include <cstdio>
#include <cstring>
#include <sys\timeb.h>
#include <math.h>


extern ConnectionClass *DAT_00A8EA0C;


// SYNTHETIC: GAMEMD 0x0048BC70 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0048BCA0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x0048BCC0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x0048BCE0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x0048BD00 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x0048BD20 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x0048BD40 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x0048BD80 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x0048BDB0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x0048BDE0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x0048BE10 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x0048BE20 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x0048BE40 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x0048BE60
ConnectionClass::ConnectionClass (int numsend, int numreceive,
	int maxlen, unsigned short magicnum, unsigned int retry_delta,
	unsigned int max_retries, unsigned int timeout, int extralen)
{
	MaxPacketLen = maxlen + sizeof(CommHeaderType);
	MagicNum = magicnum;
	RetryDelta = retry_delta;
	MaxRetries = max_retries;
	Timeout = timeout;
	PacketBuf = new char[ MaxPacketLen ];
	Queue = new CommBufferClass (numsend, numreceive, MaxPacketLen, extralen);
}

// FUNCTION: GAMEMD 0x0048BEE0
ConnectionClass::~ConnectionClass (void)
{
	delete [] PacketBuf;
	delete Queue;
}

// FUNCTION: GAMEMD 0x0048BF10
void ConnectionClass::Init (void)
{
	NumRecNoAck = 0;
	NumRecAck = 0;
	NumSendNoAck = 0;
	NumSendAck = 0;

	NumResends = 0;
	NumLost = 0;
	PercentLost = 0;
	MissedOverall = 0;
	MissedMagic = 0;

	LastSeqID = 0xffffffff;
	LastReadID = 0xffffffff;

	Queue->Init();
}

#include "session.h"

// FUNCTION: GAMEMD 0x0048BF40
int ConnectionClass::Send_Packet (void * buf, int buflen, int ack_req, int extra)
{
	((CommHeaderType *)PacketBuf)->MagicNumber = MagicNum;

	if (ack_req) {
		((CommHeaderType *)PacketBuf)->Code = PACKET_DATA_ACK;
		((CommHeaderType *)PacketBuf)->PacketID = NumSendAck;
		DebugString("NumSendAck = %d\n", NumSendAck);
	} else {
		((CommHeaderType *)PacketBuf)->Code = PACKET_DATA_NOACK;
		((CommHeaderType *)PacketBuf)->PacketID = NumSendNoAck;
	}

	if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter) {
		((CommHeaderType *)PacketBuf)->Field3 = extra;
	}

	if (MaxPacketLen < buflen) {
		DebugString("MaxPacketLen = %d\n", MaxPacketLen);
		DebugString("buflen = %d\n", buflen);
		return(0);
	}

	memcpy(PacketBuf + sizeof(CommHeaderType), buf, buflen);

	if (Queue->Queue_Send(PacketBuf, buflen + sizeof(CommHeaderType), NULL, 0)) {
		if (ack_req) {
			NumSendAck++;
		} else {
			NumSendNoAck++;
		}
		return(1);
	} else {
		return(0);
	}
}

// FUNCTION: GAMEMD 0x0048C040
int ConnectionClass::Receive_Packet (void * buf, int buflen)
{
	CommHeaderType *packet;
	SendQueueType *send_entry;
	ReceiveQueueType *rec_entry;
	CommHeaderType *entry_data;
	CommHeaderType ackpacket;
	int i;
	int save_packet = 1;
	int found;

	packet = (CommHeaderType *)buf;
	if (packet->MagicNumber != MagicNum) {
		MissedMagic++;
		return(0);
	}

	if (packet->Code == PACKET_ACK) {
		DebugString("Received ACK for packet %d\n", packet->PacketID);

		for (i = 0; i < Queue->Num_Send(); i++) {
			send_entry = Queue->Get_Send(i);

			if (send_entry != NULL) {
				entry_data = (CommHeaderType *)send_entry->Buffer;

				if (packet->PacketID==entry_data->PacketID &&
					entry_data->Code == PACKET_DATA_ACK) {
					send_entry->IsACK = 1;
					break;
				}
			}
		}

		return(1);
	}

	else if (packet->Code == PACKET_DATA_NOACK) {
		if (Queue->Max_Receive() - Queue->Num_Receive() <= 1) {
			MissedOverall++;
			DebugString("Error - ConnectClass::Receive_Packet Queue full on PACKET_DATA_NOACK\n");
			return(0);
		}

		if (!Queue->Queue_Receive (buf, buflen, NULL, 0)) {
#line 332 "D:\\ra2mdpost\\Connect.CPP"
			DebugString("Error - unable to queue incoming packet. %s %s\n", __FILE__, __LINE__);
			MissedOverall++;
			return(0);
		}

		NumRecNoAck++;

		NumLost = packet->PacketID - NumRecNoAck + 1;
		if (packet->PacketID) {
			PercentLost = 100 - (100 * NumRecNoAck) / (unsigned int)(packet->PacketID + 1);
		}

		return(1);
	}

	else if (packet->Code == PACKET_DATA_ACK) {
		if (packet->PacketID <= LastSeqID && LastSeqID != 0xffffffff) {
			save_packet = 0;
		}

		else {
			save_packet = 1;
			for (i = 0; i < Queue->Num_Receive(); i++) {
				rec_entry = Queue->Get_Receive(i);

				if (rec_entry) {

					entry_data = (CommHeaderType *)rec_entry->Buffer;

					if (entry_data->Code == PACKET_DATA_ACK &&
						entry_data->PacketID == packet->PacketID) {
						save_packet = 0;
						break;
					}
				}
			}
		}

		if (save_packet) {
			if (Queue->Max_Receive() - Queue->Num_Receive() <= 1) {
				if (packet->PacketID != (LastSeqID + 1) ) {
					DebugString("Error - ConnectClass::Receive_Packet Queue full on PACKET_DATA_ACK\n");
					return(0);
				}
			}

			if (!Queue->Queue_Receive (buf, buflen, NULL, 0)) {
				DebugString("Error - ConnectClass::Receive_Packet Queue->Queue_Receive error\n");
				return(0);
			}

			NumRecAck++;

			if (packet->PacketID == (LastSeqID + 1)) {
				LastSeqID = packet->PacketID;
				do {
					found = 0;
					for (i = 0; i < Queue->Num_Receive(); i++) {

						rec_entry = Queue->Get_Receive(i);

						if (rec_entry) {
							entry_data = (CommHeaderType *)rec_entry->Buffer;

							if (entry_data->Code == PACKET_DATA_ACK &&
								entry_data->PacketID == (LastSeqID + 1)) {

								LastSeqID = entry_data->PacketID;
								found = 1;
								break;
							}
						}
					}
				} while (found);
			}
		}

		ackpacket.MagicNumber = Magic_Num();
		ackpacket.Code = PACKET_ACK;
		ackpacket.PacketID = packet->PacketID;
		if (Session.ViaPacketRouter) {
			ackpacket.Field3 = packet->Field3;
		} else {
			ackpacket.Field3 = 0;
		}
		DebugString("Sending ACK for packet %d to be forwarded to %02x\n", packet->PacketID, ackpacket.Field3);
		Send ((char *)&ackpacket, sizeof(CommHeaderType), NULL, 0, this == DAT_00A8EA0C, 0);

		return(1);
	}

	DebugString("Error - ConnectClass::Receive_Packet did not understand packet->Code\n");
	return(0);
}

// FUNCTION: GAMEMD 0x0048C320
int ConnectionClass::Get_Packet (void * buf, int *buflen)
{
	ReceiveQueueType *rec_entry;
	int packetlen;
	CommHeaderType *entry_data;
	int i;

	for (i = 0; i < Queue->Num_Receive(); i++) {

		rec_entry = Queue->Get_Receive(i);

		if (rec_entry && rec_entry->IsRead==0) {

			entry_data = (CommHeaderType *)rec_entry->Buffer;

			if ( (entry_data->Code == PACKET_DATA_ACK) &&
				(entry_data->PacketID == (LastReadID + 1))) {

				LastReadID = entry_data->PacketID;
				rec_entry->IsRead = 1;

				packetlen = rec_entry->BufLen - sizeof(CommHeaderType);
				if (packetlen > 0) {
					memcpy(buf, rec_entry->Buffer + sizeof(CommHeaderType),
						packetlen);
				}
				(*buflen) = packetlen;
				return(1);
			}
			else if (entry_data->Code == PACKET_DATA_NOACK) {
				rec_entry->IsRead = 1;

				packetlen = rec_entry->BufLen - sizeof(CommHeaderType);
				if (packetlen > 0) {
					memcpy(buf, rec_entry->Buffer + sizeof(CommHeaderType),
						packetlen);
				}
				(*buflen) = packetlen;
				return(1);
			}
		}
	}

	return(0);
}

// FUNCTION: GAMEMD 0x0048C3B0
int ConnectionClass::Service (void)
{
	if ( Service_Send_Queue() && Service_Receive_Queue() ) {
		return(1);
	} else {
		return(0);
	}
}

// FUNCTION: GAMEMD 0x0048C3E0
int ConnectionClass::Service_Send_Queue (void)
{
	int i;
	int num_entries;
	SendQueueType *send_entry;
	CommHeaderType *packet_hdr;
	unsigned int curtime;
	int bad_conn = 0;

	for (i = 0; i < Queue->Num_Send(); i++) {
		send_entry = Queue->Get_Send(i);

		if (send_entry->IsACK) {
			packet_hdr = (CommHeaderType *)send_entry->Buffer;
			if (packet_hdr->Code == PACKET_DATA_ACK) {
				Queue->Add_Delay(Time() - send_entry->FirstTime);
			}

			Queue->UnQueue_Send(NULL,NULL,i,NULL,NULL);
			i--;
		}
	}

	num_entries = Queue->Num_Send();

	for (i = 0; i < num_entries; i++) {
		send_entry = Queue->Get_Send(i);

		if (send_entry->IsACK) {
			continue;
		}

		curtime = Time();
		if (curtime - send_entry->LastTime > RetryDelta) {

			Send (send_entry->Buffer, send_entry->BufLen, send_entry->ExtraBuffer,
				send_entry->ExtraLen, this == DAT_00A8EA0C, send_entry->Port);

			send_entry->LastTime = curtime;
			if (send_entry->SendCount==0) {
				send_entry->FirstTime = curtime;

				packet_hdr = (CommHeaderType *)send_entry->Buffer;
				if (packet_hdr->Code == PACKET_DATA_NOACK) {
					send_entry->IsACK = 1;
				}
			} else {
				NumResends++;
			}

			send_entry->SendCount++;

			if (MaxRetries != -1 && send_entry->SendCount > MaxRetries) {
				bad_conn = 1;
				send_entry->IsUndeliverable = true;
			}

			if (Timeout != -1 &&
				(send_entry->LastTime - send_entry->FirstTime) > Timeout) {
				bad_conn = 1;
				send_entry->IsUndeliverable = true;
			}
		}
	}

	if (bad_conn) {
		DebugString("ConnectionClass::Service_Send_Queue bad connection\n");
		return(0);
	} else {
		return(1);
	}
}

// FUNCTION: GAMEMD 0x0048C590
int ConnectionClass::Discard_Undeliverable_Packets(void)
{
	return(0);
}

// FUNCTION: GAMEMD 0x0048C5A0
int ConnectionClass::Service_Receive_Queue (void)
{
	ReceiveQueueType *rec_entry;
	CommHeaderType *packet_hdr;
	int i;

	for (i = 0; i < Queue->Num_Receive(); i++) {
		rec_entry = Queue->Get_Receive(i);

		if (rec_entry->IsRead) {
			packet_hdr = (CommHeaderType *)(rec_entry->Buffer);

			if (packet_hdr->Code == PACKET_DATA_NOACK) {
				Queue->UnQueue_Receive(NULL,NULL,i,NULL,NULL);
				i--;

			} else if (packet_hdr->PacketID < LastSeqID) {
				Queue->UnQueue_Receive(NULL,NULL,i,NULL,NULL);
				i--;
			}
		}
	}

	return(1);
}

// FUNCTION: GAMEMD 0x0048C600
unsigned int ConnectionClass::Time (void)
{
	return(TickCount);
}

// FUNCTION: GAMEMD 0x0048C630
const char *ConnectionClass::Command_Name(int command)
{
	if (command >= 0 && command < PACKET_COUNT) {
		return(Commands[command]);
	} else {
		return(NULL);
	}
}


// SYNTHETIC: GAMEMD 0x0048C650
// ??_GConnectionClass@@UAEPAXI@Z
