// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "house.h"
#include "globals.h"
#include "scenario.h"
#include <string.h>
#include <windows.h>
#undef LoadString

extern char DAT_00889F64[];

// SYNTHETIC: GAMEMD 0x00510B00 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00510B30 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00510B50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00510B70 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00510B90 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00510BB0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00510BD0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00510C10 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00510C40 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00510C70 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00510CA0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00510CB0 SYMBOL
// _$E35

extern int DAT_00A8F0A8;
extern int DAT_00A8F0AC;
extern int DAT_00A8F0B0;

// FUNCTION: GAMEMD 0x00510CD0
void FUN_00510CD0(void)
{
	DAT_00A8F0B0 = 0;
	DAT_00A8F0A8 = 0x80;
	DAT_00A8F0AC = 0x80;
}

// SYNTHETIC: GAMEMD 0x00510CF0 SYMBOL
// _$E41


// FUNCTION: GAMEMD 0x00510D10
void __fastcall FUN_00510D10(HWND hwnd)
{
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ A>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4475);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ B>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4476);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ C>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4477);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ D>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4478);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ E>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4479);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ F>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4480);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ G>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4481);
	}
	{
		int item = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)"<Player @ H>");
		SendMessage(hwnd, CB_SETITEMDATA, item, 4482);
	}
}

// FUNCTION: GAMEMD 0x00510E10
void __fastcall FUN_00510E10(int index, char * buffer, int size)
{
	if (size < 16) {
		if (size < 1) return;
		buffer[0] = '\0';
	}
	if (index == 0x117b) {
		strcpy(buffer, "<Player @ A>");
	} else if (index == 0x117c) {
		strcpy(buffer, "<Player @ B>");
	} else if (index == 0x117d) {
		strcpy(buffer, "<Player @ C>");
	} else if (index == 0x117e) {
		strcpy(buffer, "<Player @ D>");
	} else if (index == 0x117f) {
		strcpy(buffer, "<Player @ E>");
	} else if (index == 0x1180) {
		strcpy(buffer, "<Player @ F>");
	} else if (index == 0x1181) {
		strcpy(buffer, "<Player @ G>");
	} else if (index == 0x1182) {
		strcpy(buffer, "<Player @ H>");
	} else {
		strcpy(buffer, DAT_00889F64);
	}
}

// FUNCTION: GAMEMD 0x00510ED0
// FUN_00510ED0
HouseClass * __fastcall FUN_00510ED0(int index)
{
	int house;
	if (index == 0x117b) {
		house = Scen->FUN_0068C030(0);
	} else if (index == 0x117c) {
		house = Scen->FUN_0068C030(1);
	} else if (index == 0x117d) {
		house = Scen->FUN_0068C030(2);
	} else if (index == 0x117e) {
		house = Scen->FUN_0068C030(3);
	} else if (index == 0x117f) {
		house = Scen->FUN_0068C030(4);
	} else if (index == 0x1180) {
		house = Scen->FUN_0068C030(5);
	} else if (index == 0x1181) {
		house = Scen->FUN_0068C030(6);
	} else if (index == 0x1182) {
		house = Scen->FUN_0068C030(7);
	} else {
		return NULL;
	}
	if (house >= 0 && house < Houses.Count()) {
		return Houses[house];
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x00510F60
bool __fastcall FUN_00510F60(int index)
{
	if (index == 0x117b || index == 0x117c || index == 0x117d || index == 0x117e
		|| index == 0x117f || index == 0x1180 || index == 0x1181 || index == 0x1182) {
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00510FB0
int __fastcall FUN_00510FB0(char const * name)
{
	if (strcmp(name, "<Player @ A>") == 0) return 0x117b;
	if (strcmp(name, "<Player @ B>") == 0) return 0x117c;
	if (strcmp(name, "<Player @ C>") == 0) return 0x117d;
	if (strcmp(name, "<Player @ D>") == 0) return 0x117e;
	if (strcmp(name, "<Player @ E>") == 0) return 0x117f;
	if (strcmp(name, "<Player @ F>") == 0) return 0x1180;
	if (strcmp(name, "<Player @ G>") == 0) return 0x1181;
	return strcmp(name, "<Player @ H>") == 0 ? 0x1182 : -1;
}
