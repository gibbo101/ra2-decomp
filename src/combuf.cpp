// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "combuf.h"
#include "_mono.h"
#include "_timer.h"
#include "connect.h"
#include "dbgprint.h"
#include "rect.h"
#include "mono.h"
#include <cstdio>
#include <cstring>

// FUNCTION: 0x0048B0E0
CommBufferClass::CommBufferClass(int numsend, int numreceive, int maxlen,
	int extralen)
{
	int i;

	MaxSend = numsend;
	MaxReceive = numreceive;
	MaxPacketSize = maxlen;
	MaxExtraSize = extralen;

	SendQueue = new SendQueueType[numsend];
	ReceiveQueue = new ReceiveQueueType[numreceive];

	SendIndex = new int[numsend];
	ReceiveIndex = new int[numreceive];

	for (i = 0; i < MaxSend; i++) {
		SendQueue[i].Buffer = new char[maxlen];
		if (MaxExtraSize > 0) {
			SendQueue[i].ExtraBuffer = new char[MaxExtraSize];
		}
		else {
			SendQueue[i].ExtraBuffer = NULL;
		}
	}

	for (i = 0; i < MaxReceive; i++) {
		ReceiveQueue[i].Buffer = new char[maxlen];
		if (MaxExtraSize > 0) {
			ReceiveQueue[i].ExtraBuffer = new char[MaxExtraSize];
		}
		else {
			ReceiveQueue[i].ExtraBuffer = NULL;
		}
	}

	Init();
}

// FUNCTION: 0x0048B1F0
CommBufferClass::~CommBufferClass(void)
{
	int i;

	for (i = 0; i < MaxSend; i++) {
		delete [] SendQueue[i].Buffer;
		if (SendQueue[i].ExtraBuffer) {
			delete [] SendQueue[i].ExtraBuffer;
		}
	}

	for (i = 0; i < MaxReceive; i++) {
		delete [] ReceiveQueue[i].Buffer;
		if (ReceiveQueue[i].ExtraBuffer) {
			delete [] ReceiveQueue[i].ExtraBuffer;
		}
	}

	delete [] SendQueue;
	delete [] ReceiveQueue;

	delete [] SendIndex;
	delete [] ReceiveIndex;
}

// FUNCTION: 0x0048B2A0
void CommBufferClass::Init(void)
{
	int i;

	SendTotal = 0L;
	ReceiveTotal = 0L;

	DelaySum = 0L;
	NumDelay = 0L;
	MeanDelay = 0L;
	MaxDelay = 0L;

	SendCount = 0;

	ReceiveCount = 0;

	for (i = 0; i < MaxSend; i++) {
		SendQueue[i].IsActive = 0;
		SendQueue[i].IsACK = 0;
		SendQueue[i].IsUndeliverable = 0;
		SendQueue[i].FirstTime = 0L;
		SendQueue[i].LastTime = 0L;
		SendQueue[i].SendCount = 0L;
		SendQueue[i].BufLen = 0;
		SendQueue[i].ExtraLen = 0;
		SendQueue[i].Port = 0;

		SendIndex[i] = 0;
	}

	for (i = 0; i < MaxReceive; i++) {
		ReceiveQueue[i].IsActive = 0;
		ReceiveQueue[i].IsRead = 0;
		ReceiveQueue[i].IsACK = 0;
		ReceiveQueue[i].ReceiveTime = 0L;
		ReceiveQueue[i].BufLen = 0;
		ReceiveQueue[i].ExtraLen = 0;

		ReceiveIndex[i] = 0;
	}

	DebugOffset = 0;
	DebugSize = 0;
	DebugNames = NULL;
	DebugNameCount = 0;
}

// FUNCTION: 0x0048B390
void CommBufferClass::Init_Send_Queue(void)
{
	int i;

	SendCount = 0;

	for (i = 0; i < MaxSend; i++) {
		SendQueue[i].IsActive = 0;
		SendQueue[i].IsACK = 0;
		SendQueue[i].IsUndeliverable = 0;
		SendQueue[i].FirstTime = 0L;
		SendQueue[i].LastTime = 0L;
		SendQueue[i].SendCount = 0L;
		SendQueue[i].BufLen = 0;
		SendQueue[i].ExtraLen = 0;
		SendQueue[i].Port = 0;

		SendIndex[i] = 0;
	}
}

// FUNCTION: 0x0048B410
int CommBufferClass::Queue_Send(void *buf, int buflen, void *extrabuf,
	int extralen, unsigned short port)
{
	int i;
	int index;

	if (SendCount==MaxSend || buflen > MaxPacketSize) {
		if (SendCount==MaxSend) {
			DebugString("Queue_Send - No room in send queue\n");
			return(0);
		}
		DebugString("buflen > MaxPacketSize by %d bytes\n", MaxPacketSize - buflen);
		return(0);
	}

	index = -1;
	for (i = 0; i < MaxSend; i++) {
		if (SendQueue[i].IsActive==0) {
			index = i;
			break;
		}
	}
	if (index == -1)
		return(0);

	SendQueue[index].IsActive = 1;
	SendQueue[index].IsACK = 0;
	SendQueue[index].IsUndeliverable = 0;
	SendQueue[index].FirstTime = 0L;
	SendQueue[index].LastTime = 0L;
	SendQueue[index].SendCount = 0L;
	SendQueue[index].BufLen = buflen;
	SendQueue[index].Port = port;

	memcpy(SendQueue[index].Buffer,buf,buflen);

	if (extrabuf!=NULL && extralen > 0 && extralen <= MaxExtraSize) {
		memcpy(SendQueue[index].ExtraBuffer,extrabuf,extralen);
		SendQueue[index].ExtraLen = extralen;
	}
	else {
		SendQueue[index].ExtraLen = 0;
	}

	SendIndex[SendCount] = index;

	SendCount++;
	SendTotal++;

	return(1);
}

// FUNCTION: 0x0048B570
int CommBufferClass::UnQueue_Send(void *buf, int *buflen, int index,
	void *extrabuf, int *extralen, unsigned short *port)
{
	int i;

	if (SendCount==0 || SendQueue[SendIndex[index]].IsActive==0) {
		return(0);
	}

	if (buf!=NULL) {
		memcpy(buf,SendQueue[SendIndex[index]].Buffer,
			SendQueue[SendIndex[index]].BufLen);
		(*buflen) = SendQueue[SendIndex[index]].BufLen;
	}

	if (extrabuf!=NULL && extralen!=NULL) {
		memcpy(extrabuf,SendQueue[SendIndex[index]].ExtraBuffer,
			SendQueue[SendIndex[index]].ExtraLen);
		(*extralen) = SendQueue[SendIndex[index]].ExtraLen;
	}

	if (port!=NULL) {
		(*port) = SendQueue[SendIndex[index]].Port;
	}

	SendQueue[SendIndex[index]].IsActive = 0;
	SendQueue[SendIndex[index]].IsACK = 0;
	SendQueue[SendIndex[index]].IsUndeliverable = 0;
	SendQueue[SendIndex[index]].FirstTime = 0L;
	SendQueue[SendIndex[index]].LastTime = 0L;
	SendQueue[SendIndex[index]].SendCount = 0L;
	SendQueue[SendIndex[index]].BufLen = 0;
	SendQueue[SendIndex[index]].ExtraLen = 0;
	SendQueue[SendIndex[index]].Port = 0;

	for (i = index; i < SendCount - 1; i++) {
		SendIndex[i] = SendIndex[i + 1];
	}
	SendIndex[SendCount - 1] = 0;
	SendCount--;

	return(1);
}

// FUNCTION: 0x0048B720
SendQueueType * CommBufferClass::Get_Send(int index)
{
	if (SendQueue[SendIndex[index]].IsActive==0) {
		return(NULL);
	}
	else {
		return(&SendQueue[SendIndex[index]]);
	}
}

// FUNCTION: 0x0048B750
int CommBufferClass::Queue_Receive(void *buf, int buflen, void *extrabuf,
	int extralen)
{
	int i;
	int index;

	if (ReceiveCount==MaxReceive || buflen > MaxPacketSize) {
		return(0);
	}

	index = -1;
	for (i = 0; i < MaxReceive; i++) {
		if (ReceiveQueue[i].IsActive==0) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		DebugString("Can't queue incoming packet - Unable to find free slot in array.\n");
		return(0);
	}

	ReceiveQueue[index].IsActive = 1;
	ReceiveQueue[index].IsRead = 0;
	ReceiveQueue[index].IsACK = 0;
	ReceiveQueue[index].ReceiveTime = TickCount;
	ReceiveQueue[index].BufLen = buflen;

	memcpy(ReceiveQueue[index].Buffer,buf,buflen);

	if (extrabuf!=NULL && extralen > 0 && extralen <= MaxExtraSize) {
		memcpy(ReceiveQueue[index].ExtraBuffer,extrabuf,extralen);
		ReceiveQueue[index].ExtraLen = extralen;
	}
	else {
		ReceiveQueue[index].ExtraLen = 0;
	}

	ReceiveIndex[ReceiveCount] = index;

	ReceiveCount++;
	ReceiveTotal++;

	return(1);
}

// FUNCTION: 0x0048B890
int CommBufferClass::UnQueue_Receive(void *buf, int *buflen, int index,
	void *extrabuf, int *extralen)
{
	int i;

	if (ReceiveCount==0 || ReceiveQueue[ReceiveIndex[index]].IsActive==0) {
		return(0);
	}

	if (buf!=NULL) {
		memcpy(buf,ReceiveQueue[ReceiveIndex[index]].Buffer,
			ReceiveQueue[ReceiveIndex[index]].BufLen);
		(*buflen) = ReceiveQueue[ReceiveIndex[index]].BufLen;
	}

	if (extrabuf!=NULL && extralen!=NULL) {
		memcpy(extrabuf,ReceiveQueue[ReceiveIndex[index]].ExtraBuffer,
			ReceiveQueue[ReceiveIndex[index]].ExtraLen);
		(*extralen) = ReceiveQueue[ReceiveIndex[index]].ExtraLen;
	}

	ReceiveQueue[ReceiveIndex[index]].IsActive = 0;
	ReceiveQueue[ReceiveIndex[index]].IsRead = 0;
	ReceiveQueue[ReceiveIndex[index]].IsACK = 0;
	ReceiveQueue[ReceiveIndex[index]].BufLen = 0;
	ReceiveQueue[ReceiveIndex[index]].ExtraLen = 0;

	for (i = index; i < ReceiveCount - 1; i++) {
		ReceiveIndex[i] = ReceiveIndex[i + 1];
	}
	ReceiveIndex[ReceiveCount - 1] = 0;
	ReceiveCount--;

	return(1);
}

// FUNCTION: 0x0048B9E0
ReceiveQueueType * CommBufferClass::Get_Receive(int index)
{
	if (ReceiveQueue[ReceiveIndex[index]].IsActive==0) {
		return(NULL);
	}
	else {
		return(&ReceiveQueue[ReceiveIndex[index]]);
	}
}

// FUNCTION: 0x0048BA10
void CommBufferClass::Add_Delay(unsigned int delay)
{
	int roundoff = 0;

	if (NumDelay==256) {
		DelaySum -= MeanDelay;
		DelaySum += delay;
		if ( (DelaySum & 0x00ff) > 127)
			roundoff = 1;
		MeanDelay = (DelaySum >> 8) + roundoff;
	}
	else {
		NumDelay++;
		DelaySum += delay;
		MeanDelay = DelaySum / NumDelay;
	}

	if (delay > MaxDelay) {
		MaxDelay = delay;
	}
}

// FUNCTION: 0x0048BA80
unsigned int CommBufferClass::Avg_Response_Time(void)
{
	return(MeanDelay);
}

// FUNCTION: 0x0048BA90
unsigned int CommBufferClass::Max_Response_Time(void)
{
	return(MaxDelay);
}

// FUNCTION: 0x0048BAA0
void CommBufferClass::Reset_Response_Time(bool zero)
{
	if (zero) {
	DelaySum = 0L;
	NumDelay = 0L;
	MeanDelay = 0L;
	MaxDelay = 0L;
	} else {
		DelaySum = MeanDelay;
		NumDelay = 1L;
		MaxDelay = MeanDelay;
	}
}

// FUNCTION: 0x0048BAD0
void CommBufferClass::Configure_Debug(int type_offset, int type_size,
	char **names, int namestart, int namecount)
{
	DebugOffset = type_offset;
	DebugSize = type_size;
	DebugNames = names;
	DebugNameStart = namestart;
	DebugNameCount = namecount;
}

// FUNCTION: 0x0048BB00 CommBufferClass::~CommBufferClass

// FUNCTION: 0x0048BB20 $E8
const Rect RECT_NONE_2(0, 0, 0, 0);
