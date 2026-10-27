// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "cd.h"
#include "_keyboar.h"
#include "_mixfile.h"
#include "_pk.h"
#include "_xmouse.h"
#include "ccfile.h"
#include "cdfile.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "keyboard.h"
#include "mixfile.h"
#include "msgbox.h"
#include "theme.h"
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x00479080 SYMBOL
// _$E4

bool __fastcall FUN_00623CD0(wchar_t const * text);

struct CDDriveManagerClass {
	static int __fastcall GetCDNumber(int drive, int seconds);
};

// FUNCTION: GAMEMD 0x004790A0
DiskID CD::Get_Current_Disk(int timeout)
{
	return (DiskID)CDDriveManagerClass::GetCDNumber(CDFileClass::Get_CD_Drive(), timeout);
}

// FUNCTION: GAMEMD 0x004790B0
void CD::Set_Required_Disk(DiskID disk)
{
	if (_OverrideSwap == true) {
		RequiredCD = DISK_LOCAL;
		return;
	}

	if (disk >= 0) {
		disk = (DiskID)2;
	}

	RequiredCD = disk;
}

// FUNCTION: GAMEMD 0x004790E0
bool CD::Force_Available(DiskID disk)
{
	if (disk == DISK_LOCAL) {
		return true;
	}

	if (_OverrideSwap == true) {
		return true;
	}

	ThemePlaying = THEME_NONE;

	return BASECLASS::Force_Available(disk);
}

// FUNCTION: GAMEMD 0x00479110
bool CD::Request_Disk(DiskID disk)
{
	if (ThemePlaying == THEME_NONE) {
		ThemePlaying = Theme.What_Is_Playing();
	}

	Theme.Stop(false);

	int hidden = MouseCursor->Get_Mouse_State() < 0;

	while (MouseCursor->Get_Mouse_State() < 0) {
		MouseCursor->Show_Mouse();
	}

	Keyboard->Clear();

	bool res = true;

	if (!FUN_00623CD0(StringTable::LoadString("TXT_INSERT_CD_DIALOG", NULL, "D:\\ra2mdpost\\CD.CPP", 0xa4))) {
		res = false;
	}

	Keyboard->Clear();

	while (hidden--) {
		MouseCursor->Hide_Mouse();
	}

	if (res == false && ThemePlaying != THEME_NONE) {
		Theme.Queue_Song(ThemePlaying);
		ThemePlaying = THEME_NONE;
	}

	return res;
}
