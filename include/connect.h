// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "combuf.h"

#define CONN_DEBUG	0
#pragma pack(push,1)

struct CommHeaderType {
	unsigned short MagicNumber;
	unsigned char Code;
	unsigned int PacketID;
};
#pragma pack(pop)

class ConnectionClass
{

	public:

		enum ConnectionEnum {
			PACKET_DATA_ACK,
			PACKET_DATA_NOACK,
			PACKET_ACK,
			PACKET_COUNT
		};

		ConnectionClass (int numsend, int numrecieve, int maxlen,
			unsigned short magicnum, unsigned int retry_delta,
			unsigned int max_retries, unsigned int timeout, int extralen = 0);
		virtual ~ConnectionClass (void);

		virtual void Init (void);

		virtual int Send_Packet (void * buf, int buflen, int ack_req, int extra);
		virtual int Receive_Packet (void * buf, int buflen);
		virtual int Get_Packet (void * buf, int * buflen);

		virtual int Service (void);

		virtual int Discard_Undeliverable_Packets(void);

		static unsigned int Time (void);

		unsigned short Magic_Num (void) { return(MagicNum); }
		unsigned int Retry_Delta (void) { return(RetryDelta); }
		void Set_Retry_Delta (unsigned int delta) { RetryDelta = delta;}
		unsigned int Max_Retries (void) { return(MaxRetries); }
		void Set_Max_Retries (unsigned int retries) { MaxRetries = retries;}
		unsigned int Time_Out (void) { return(Timeout); }
		void Set_TimeOut (unsigned int t) { Timeout = t;}
		unsigned int Max_Packet_Len (void) { return(MaxPacketLen); }
		static const char * Command_Name(int command);

		int Num_Resends(void) const { return(NumResends); }
		int Num_Lost(void) const { return(NumLost); }
		int Percent_Lost(void) const { return(PercentLost); }
		int Missed_Overall(void) const { return(MissedOverall); }
		int Missed_Magic(void) const { return(MissedMagic); }

		CommBufferClass *Queue;

	protected:

		virtual int Service_Send_Queue(void);
		virtual int Service_Receive_Queue(void);

		virtual int Send(char *buf, int buflen, void *extrabuf,
			int extralen, bool flag, unsigned short port) = 0;

		int NumResends;

		int NumLost;
		int PercentLost;

		int MissedOverall;

		int MissedMagic;

		int MaxPacketLen;

		char *PacketBuf;

		unsigned short MagicNum;

		unsigned int RetryDelta;

		unsigned int MaxRetries;

		unsigned int Timeout;

		unsigned int NumRecNoAck;
		unsigned int NumRecAck;
		unsigned int NumSendNoAck;
		unsigned int NumSendAck;

		unsigned int LastSeqID;

		unsigned int LastReadID;

		static char const * Commands[PACKET_COUNT];
};

