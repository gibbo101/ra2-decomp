// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "win.h"

struct SendQueueType {
	unsigned int IsActive	: 1;
	unsigned int IsACK		: 1;
	unsigned int IsUndeliverable : 1;
	unsigned int FirstTime;
	unsigned int LastTime;
	unsigned int SendCount;
	int BufLen;
	char *Buffer;
	int ExtraLen;
	char *ExtraBuffer;
	unsigned short Port;
};

struct ReceiveQueueType {
	unsigned int IsActive	: 1;
	unsigned int IsRead		: 1;
	unsigned int IsACK		: 1;
	unsigned int ReceiveTime;
	int BufLen;
	char *Buffer;
	int ExtraLen;
	char *ExtraBuffer;
};

class CommBufferClass
{

	public:

		CommBufferClass(int numsend, int numrecieve, int maxlen,
			int extralen = 0);
		virtual ~CommBufferClass(void);
		void Init(void);
		void Init_Send_Queue(void);

		int Queue_Send(void *buf, int buflen, void *extrabuf = NULL,
			int extralen = 0, unsigned short port = 0);
		int UnQueue_Send(void *buf, int *buflen, int index,
			void *extrabuf = NULL, int *extralen = NULL, unsigned short *port = NULL);
		int Num_Send(void) {return(SendCount);}
		int Max_Send(void) { return(MaxSend);}
		SendQueueType * Get_Send(int index);
		unsigned int Send_Total(void) {return(SendTotal);}

		int Queue_Receive(void *buf, int buflen, void *extrabuf = NULL,
			int extralen = 0);
		int UnQueue_Receive(void *buf, int *buflen, int index,
			void *extrabuf = NULL, int *extralen = NULL);
		int Num_Receive(void) {return(ReceiveCount);}
		int Max_Receive(void) { return(MaxReceive); }
		ReceiveQueueType * Get_Receive(int index);
		unsigned int Receive_Total(void) {return(ReceiveTotal);}

		void Add_Delay(unsigned int delay);
		unsigned int Avg_Response_Time(void);
		unsigned int Max_Response_Time(void);
		void Reset_Response_Time(bool zero);

		void Configure_Debug(int type_offset, int type_size, char **names,
			int namestart, int namecount);
		void Mono_Debug_Print(int refresh = 0);
		void Mono_Debug_Print2(int refresh = 0);

	private:

		int MaxSend;
		int MaxReceive;
		int MaxPacketSize;
		int MaxExtraSize;

		unsigned int DelaySum;
		unsigned int NumDelay;
		unsigned int MeanDelay;
		unsigned int MaxDelay;

		SendQueueType * SendQueue;
		int SendCount;
		unsigned int SendTotal;
		int *SendIndex;

		ReceiveQueueType * ReceiveQueue;
		int ReceiveCount;
		unsigned int ReceiveTotal;
		int *ReceiveIndex;

		int DebugOffset;
		int DebugSize;
		char **DebugNames;
		int DebugNameStart;
		int DebugNameCount;
};

