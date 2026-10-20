// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "playerprofile.h"
#include "rcptr.h"
#include "stimer.h"
#include "timer.h"
#include "wolapi\wolapi.h"
#include <string.h>
#include <new.h>

bool GameTimerInUse;
int GameEndTime;

struct PacketEntry {
	int Kind;
	int Size;
	char Data[0x8000];

	PacketEntry(void) : Kind(0), Size(0) {}
	PacketEntry(PacketEntry const * rhs);
};

struct PacketQueue {
	int Count;
	int Max;
	PacketEntry *Data;

	PacketQueue(void) : Count(0), Max(0), Data(0) {}
	~PacketQueue(void)
	{
		Count = 0;
		delete Data;
	}
	bool Shrink(void);
	bool Grow(void);
};

struct StatsCounters {
	int A;
	int B;
	int C;
	int D;

	StatsCounters(void) : A(0), B(0), C(0), D(0) {}
};

// FUNCTION: 0x006C6AD0
BasicTimerClass<SystemTimerClass> GameTimer;

// FUNCTION: 0x006C6B00
PacketEntry::PacketEntry(PacketEntry const * rhs)
{
	Kind = rhs->Kind;
	Size = rhs->Size;
	if (Size > 0x8000) {
		Size = 0;
		return;
	}
	memcpy(Data, rhs->Data, Size);
}

// FUNCTION: 0x006C6B40
// FUNCTION: 0x006C6B60
PacketQueue Queue;

// FUNCTION: 0x006C6D00
void Queue_Pop(void)
{
	int index = 0;
	if (Queue.Count == 0) {
		return;
	}
	if (Queue.Count <= 0) {
		index = Queue.Count - 1;
	}
	memmove(&Queue.Data[index], &Queue.Data[index + 1], (Queue.Count - (index + 1)) * sizeof(PacketEntry));
	Queue.Count--;
	Queue.Shrink();
}

int Get_Unit_Count(int *buf, int count);

// FUNCTION: 0x006C87D0
int Get_Unit_Count(int *buf, int count)
{
	int n = 0;
	for (int i = 0; i < count; i++) {
		if (buf[i] != 0) {
			n = i + 1;
		}
	}
	return(n);
}

// FUNCTION: 0x006C87F0
void Register_Game_Start_Time(void)
{
	GameTimer = 0;
	GameTimerInUse = true;
}

// FUNCTION: 0x006C8820
void Register_Game_End_Time(void)
{
	GameEndTime = GameTimer;
	GameTimerInUse = false;
}

// FUNCTION: 0x006C8840 ?$rc_ptr@VPlayerProfile@@::~?$rc_ptr@VPlayerProfile@@
template class rc_ptr<PlayerProfile>;

// FUNCTION: 0x006C8870
bool PacketQueue::Shrink(void)
{
	if (Count * 3 > Max) {
		return false;
	}
	int newmax = Max / 2;
	if (newmax < 10) {
		newmax = 10;
	}
	if (newmax >= Max) {
		return false;
	}
	PacketEntry *p = (PacketEntry *)operator new(newmax * sizeof(PacketEntry));
	if (Data) {
		memcpy(p, Data, Count * sizeof(PacketEntry));
	}
	operator delete(Data);
	Data = p;
	Max = newmax;
	return true;
}

// FUNCTION: 0x006C8900
bool PacketQueue::Grow(void)
{
	if (Count < Max) {
		return false;
	}
	int newmax = Count * 2;
	if (newmax < 10) {
		newmax = 10;
	}
	PacketEntry *p = (PacketEntry *)operator new(newmax * sizeof(PacketEntry));
	memset(p, 0, newmax * sizeof(PacketEntry));
	if (Data) {
		memcpy(p, Data, Count * sizeof(PacketEntry));
	}
	operator delete(Data);
	Max = newmax;
	Data = p;
	return true;
}

// FUNCTION: 0x006C8990
StatsCounters Counters;
