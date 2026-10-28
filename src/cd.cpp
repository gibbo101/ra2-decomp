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

void __cdecl FUN_004068E0(char const * format, ...);
bool __cdecl FUN_00751C30(void);

extern MixFileClass * DAT_00884E2C;
extern MixFileClass * DAT_0087E738;
extern MixFileClass * DAT_00884DDC;
extern MixFileClass * DAT_00884DD8;
extern MixFileClass * DAT_00884E64;
extern MixFileClass * DAT_00884E60;
extern MixFileClass * DAT_00884E34;

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

// FUNCTION: GAMEMD 0x004791F0
bool CD::Swap(DiskID disk)
{
	char mixname[64];

	FUN_004068E0("***** Swapping to disk %d *****\n", disk);

	if (ThemePlaying == THEME_NONE) {
		ThemePlaying = Theme.What_Is_Playing();
	}

	Theme.Stop(false);

	if (DAT_00884E2C) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E2C->Filename);
		delete DAT_00884E2C;
		DAT_00884E2C = NULL;
	}

	if (DAT_0087E738) {
		FUN_004068E0("     Releasing %s\n", DAT_0087E738->Filename);
		delete DAT_0087E738;
		DAT_0087E738 = NULL;
	}

	if (DAT_00884DDC) {
		FUN_004068E0("     Releasing %s\n", DAT_00884DDC->Filename);
		delete DAT_00884DDC;
		DAT_00884DDC = NULL;
	}

	if (DAT_00884DD8) {
		FUN_004068E0("     Releasing %s\n", DAT_00884DD8->Filename);
		delete DAT_00884DD8;
		DAT_00884DD8 = NULL;
	}

	if (DAT_00884E64) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E64->Filename);
		delete DAT_00884E64;
		DAT_00884E64 = NULL;
	}

	if (DAT_00884E60) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E60->Filename);
		delete DAT_00884E60;
		DAT_00884E60 = NULL;
	}

	if (DAT_00884E34) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E34->Filename);
		delete DAT_00884E34;
		DAT_00884E34 = NULL;
	}

	if (!FUN_00751C30()) {
		return(true);
	}

	DAT_00884E34 = new MixFileClass("MAIN.MIX", &FastKey);
	FUN_004068E0("     Initialized %s\n", DAT_00884E34->Filename);

	disk = DiskID(disk + 1);

	sprintf(mixname, "MAPSMD%02d.MIX", disk);
	FUN_004068E0("     Initializing %s\n", mixname);
	DAT_00884E60 = new MixFileClass(mixname, &FastKey);

	sprintf(mixname, "MAPS%02d.MIX", disk);
	FUN_004068E0("     Initializing %s\n", mixname);
	DAT_00884E64 = new MixFileClass(mixname, &FastKey);
	if (!DAT_00884E64) {
		FUN_004068E0("     FAILED!\n");
		return(false);
	}

	FUN_004068E0("     Initializing New Multi.Mix\n");
	DAT_00884DD8 = new MixFileClass("MULTIMD.MIX", &FastKey);

	FUN_004068E0("     Initializing Multi.Mix\n");
	DAT_00884DDC = new MixFileClass("MULTI.MIX", &FastKey);
	if (!DAT_00884DDC) {
		FUN_004068E0("     FAILED!\n");
		return(false);
	}

	bool newthemes = CCFileClass("THEMEMD.MIX").Is_Available(0);
	if (newthemes) {
		FUN_004068E0("     Initializing Themes.MIX\n");
		DAT_0087E738 = new MixFileClass("THEMEMD.MIX", &FastKey);
	} else {
		FUN_004068E0("     Initializing Themes.MIX\n");
		DAT_0087E738 = new MixFileClass("THEME.MIX", &FastKey);
	}

	Theme.Scan();

	sprintf(mixname, "MOVMD%02d.MIX", disk);
	if (CCFileClass(mixname).Is_Available(0)) {
		FUN_004068E0("     Initializing %s\n", mixname);
		DAT_00884E2C = new MixFileClass(mixname, &FastKey);
		if (!DAT_00884E2C) {
			FUN_004068E0("     FAILED!\n");
			return(false);
		}
	} else {
		sprintf(mixname, "MOVIES%02d.MIX", disk);
		FUN_004068E0("     Initializing %s\n", mixname);
		DAT_00884E2C = new MixFileClass(mixname, &FastKey);
		if (!DAT_00884E2C) {
			FUN_004068E0("     FAILED!\n");
			return(false);
		}
	}

	return(FUN_00751C30());
}

struct CDDriveTable {
	int Drive[26];

	CDDriveTable(void) { memset(Drive, 0, sizeof(Drive)); }
	~CDDriveTable(void) {}
};

// SYNTHETIC: GAMEMD 0x004796E0 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x00479700 SYMBOL
// _$E12
CDDriveTable DAT_0089E3A8;
