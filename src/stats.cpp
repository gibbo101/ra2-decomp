// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "playerprofile.h"
#include "rcptr.h"
#include "stimer.h"
#include "timer.h"
#include "wolapi\wolapi.h"
#include <string.h>
#include <new.h>

// SYNTHETIC: GAMEMD 0x006C68C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006C68F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006C6910 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006C6930 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006C6950 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006C6970 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006C6990 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006C69D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006C6A00 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006C6A30 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006C6A60 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006C6A70 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006C6A90 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006C6AB0 SYMBOL
// _$E41

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

// FUNCTION: GAMEMD 0x006C6AD0
BasicTimerClass<SystemTimerClass> GameTimer;

// FUNCTION: GAMEMD 0x006C6B00
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

// FUNCTION: GAMEMD 0x006C6B40
// FUNCTION: GAMEMD 0x006C6B60
PacketQueue Queue;

// FUNCTION: GAMEMD 0x006C6D00
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

// FUNCTION: GAMEMD 0x006C87D0
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

// FUNCTION: GAMEMD 0x006C87F0
void Register_Game_Start_Time(void)
{
	GameTimer = 0;
	GameTimerInUse = true;
}

// FUNCTION: GAMEMD 0x006C8820
void Register_Game_End_Time(void)
{
	GameEndTime = GameTimer;
	GameTimerInUse = false;
}

// SYNTHETIC: GAMEMD 0x006C8840
// ??_G?$rc_ptr@VPlayerProfile@@@@UAEPAXI@Z
template class rc_ptr<PlayerProfile>;

// FUNCTION: GAMEMD 0x006C8870
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

// FUNCTION: GAMEMD 0x006C8900
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
