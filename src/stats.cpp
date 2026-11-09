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
#include "arraylist.h"
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
	~PacketEntry(void) {}
	PacketEntry & operator=(PacketEntry const & rhs);
};

// FUNCTION: GAMEMD 0x006C6AD0
BasicTimerClass<SystemTimerClass> GameTimer;

// SYNTHETIC: GAMEMD 0x006C6AF0 SYMBOL
// _$E44

// FUNCTION: GAMEMD 0x006C6B00
PacketEntry & PacketEntry::operator=(PacketEntry const & rhs)
{
	Kind = rhs.Kind;
	Size = rhs.Size;
	if (Size > 0x8000) {
		Size = 0;
		return(*this);
	}
	memcpy(Data, rhs.Data, Size);
	return(*this);
}

// FUNCTION: GAMEMD 0x006C6B40
// FUNCTION: GAMEMD 0x006C6B60
static ArrayList<PacketEntry> Queue;

// FUNCTION: GAMEMD 0x006C6B80
void __fastcall Queue_Packet(void *buf, int size)
{
	if (size > 0x8000 || buf == NULL || size < 1) {
		return;
	}
	PacketEntry entry;
	entry.Size = size;
	memcpy(entry.Data, buf, size);
	Queue.addTail(entry);
}

// FUNCTION: GAMEMD 0x006C6D00
void Queue_Pop(void)
{
	Queue.remove(0);
}

extern struct INetUtil * g_pNetUtil;
extern char DAT_00B77930[];
extern int DAT_00B76184;

// FUNCTION: GAMEMD 0x006C6DE0
void Queue_Send(void)
{
	PacketEntry entry;
	if (Queue.length() == 0) {
		return;
	}
	Queue.getHead(entry);
	while (entry.Size < 1 || entry.Kind > 10) {
		Queue.remove(0);
		if (Queue.length() == 0) {
			return;
		}
		Queue.getHead(entry);
	}
	if (g_pNetUtil == NULL) {
		return;
	}
	g_pNetUtil->RequestGameresSend(DAT_00B77930, DAT_00B76184, (unsigned char *)entry.Data, entry.Size);
	entry.Kind++;
	Queue.replace(entry, 0);
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

// TEMPLATE: GAMEMD 0x006C8870
// ?shrinkVector@?$ArrayList@UPacketEntry@@@@AAEDXZ
// TEMPLATE: GAMEMD 0x006C8900
// ?growVector@?$ArrayList@UPacketEntry@@@@AAEDXZ
