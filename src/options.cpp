// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "options.h"
#include "_command.h"
#include "_map.h"
#include "_rules.h"
#include "ccfile.h"
#include "ccrand.h"
#include "command.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "init.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "mouse.h"
#include "msgbox.h"
#include "ownrdraw.h"
#include "rules.h"
#include "session.h"
#include "techno.h"
#include "theme.h"
#include "vector.h"
#include "vox.h"
#include "diff.hh"
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

struct MovieUnlockableInfo {
	char const * Filename;
	char const * Description;
	int DiskRequired;

	static MovieUnlockableInfo Common[];
	static MovieUnlockableInfo Allied[];
	static MovieUnlockableInfo Soviet[];
};

extern CCINIClass DAT_008870C0;
extern void * DAT_0087E740;
extern void * DAT_0087E744;
extern void * DAT_0087E748;
void __fastcall FUN_00407150(void * p, int value);
void __fastcall FUN_00752AB0(int volume);

// FUNCTION: 0x005FA2E0
int __fastcall Hotkey_To_Key(int hotkey)
{
	int key = hotkey & 0xFF;
	unsigned int mods = (hotkey >> 8) & 0xFF;
	if (mods & 4) {
		key |= KN_ALT_BIT;
	}
	if (mods & 2) {
		key |= KN_CTRL_BIT;
	}
	if (mods & 1) {
		key |= KN_SHIFT_BIT;
	}
	if (mods & 8) {
		key = 0;
	}
	return key;
}

// FUNCTION: 0x005FA310
int __fastcall Key_To_Hotkey(int key)
{
	int mods = 0;
	if (key & KN_ALT_BIT) {
		mods = 4;
	}
	if (key & KN_CTRL_BIT) {
		mods |= 2;
	}
	if (key & KN_SHIFT_BIT) {
		mods |= 1;
	}
	if (key & WWKEY_VK_BIT) {
		mods |= 8;
	}
	return (key & 0xFF) | (mods << 8);
}

// FUNCTION: 0x005FA350
OptionsClass::OptionsClass(void) :
	Difficulty(DIFF_NORMAL),
	GameSpeed(3),
	ScrollRate(3),
	SoundVolume(.7f),
	VoiceVolume(.7f),
	ScoreVolume(.4f),
	AutoScroll(true),
	IsScoreRepeat(false),
	IsScoreShuffle(false),
	IsSidebarOnRight(true),
	SidebarCameoText(true),
	ActionLines(true),
	ShowHidden(false),
	ToolTips(true),
	ScreenWidth(-1),
	ScreenHeight(-1),
	ShellWidth(800),
	ShellHeight(600),
	InGameMusic(true),
	LastUnlockedSovMovie(-1),
	LastUnlockedAllMovie(-1),
	AllowVRAMSidebar(false),
	ScrollMethod(0),
	DetailLevel(2),
	StretchMovies(0),
	AllowHiResModes(0),
	SoundLatency(9),
	Socket(-1),
	NetCard(0),
	KeyForceMove1(KN_LALT),
	KeyForceMove2(KN_LALT),
	KeyForceAttack1(KN_LCTRL),
	KeyForceAttack2(KN_LCTRL),
	KeySelect1(KN_LSHIFT),
	KeySelect2(KN_LSHIFT),
	KeyQueueMove1(KN_Q),
	KeyQueueMove2(KN_Q)
{
	DestNet[0] = '\0';
}

// FUNCTION: 0x005FA430
void OptionsClass::One_Time(void)
{
}

// FUNCTION: 0x005FA440
void OptionsClass::Set_Shuffle(bool on)
{
	IsScoreShuffle = on;
	Theme.Set_Shuffle(on);
	DebugString("ScoreShuffle is %s\n", IsScoreShuffle == true ? "ON" : "OFF");
}

// FUNCTION: 0x005FA470
void OptionsClass::Set_Repeat(bool on)
{
	IsScoreRepeat = on;
	Theme.Set_Repeat(on);
	DebugString("ScoreRepeat is %s\n", IsScoreRepeat == true ? "ON" : "OFF");
}

// FUNCTION: 0x005FA4A0
void OptionsClass::Set_Score_Volume(float volume, bool feedback)
{
	ScoreVolume = MIN(volume, 1.0);
	FUN_00407150(DAT_0087E744, ScoreVolume * 16384.0f);
	Theme.Set_Volume(ScoreVolume * 255.0);
	DebugString("ScoreVolume = %f\n", ScoreVolume);
}

// FUNCTION: 0x005FA510
void OptionsClass::Set_Sound_Volume(float volume, bool feedback)
{
	SoundVolume = MIN(volume, 1.0);
	FUN_00407150(DAT_0087E748, SoundVolume * 16384.0f);
	if (feedback) {
		VocClass::PlayGlobal(Rule->GenericBeep, 1.0f, 0x2000, NULL);
	}
	DebugString("SoundVolume = %f\n", SoundVolume);
}

// FUNCTION: 0x005FA590
void OptionsClass::Set_Voice_Volume(float volume, bool feedback)
{
	VoiceVolume = MIN(volume, 1.0);
	FUN_00752AB0(VoiceVolume * 255.0);
	FUN_00407150(DAT_0087E740, VoiceVolume * 16384.0f);
	if (feedback) {
		VocClass::PlayGlobal(Rule->GenericBeep, VoiceVolume, 0x2000, NULL);
	}
	DebugString("VoiceVolume = %f\n", VoiceVolume);
}

// FUNCTION: 0x005FB2E0
int OptionsClass::Normalize_Delay(int delay) const
{
	static int _adjust[][MAX_SPEED_SETTING + 1] = {
		{2,2,1,1,1,1,1,1},
		{3,3,3,2,2,2,1,1},
		{5,4,4,3,3,2,2,1},
		{7,6,5,4,4,4,3,2}
	};
	if (delay) {
		if (delay < 5) {
			delay = _adjust[delay-1][GameSpeed];
		} else {
			delay = ((delay * (MAX_SPEED_SETTING + 1)) / (GameSpeed+1));
		}
	}
	return(delay);
}

// FUNCTION: 0x005FAD10
void OptionsClass::Save_Settings(void)
{
	CCFileClass file("RA2MD.INI");

	DebugString("Saving game settings\n");

	DAT_008870C0.Put_Int("Options", "GameSpeed", GameSpeed);
	DAT_008870C0.Put_Int("Options", "Difficulty", Difficulty);
	DAT_008870C0.Put_Int("Options", "CampDifficulty", CampDifficulty);
	DAT_008870C0.Put_Int("Options", "ScrollMethod", ScrollMethod);
	DAT_008870C0.Put_Int("Options", "ScrollRate", ScrollRate);
	DAT_008870C0.Put_Bool("Options", "AutoScroll", AutoScroll);
	DAT_008870C0.Put_Int("Options", "DetailLevel", DetailLevel);
	DAT_008870C0.Put_Bool("Options", "SidebarCameoText", SidebarCameoText);
	DAT_008870C0.Put_Bool("Options", "UnitActionLines", ActionLines);
	DAT_008870C0.Put_Bool("Options", "ShowHidden", ShowHidden);
	DAT_008870C0.Put_Bool("Options", "ToolTips", ToolTips);
	DAT_008870C0.Put_Int("Video", "ScreenWidth", ScreenWidth);
	DAT_008870C0.Put_Int("Video", "ScreenHeight", ScreenHeight);
	DAT_008870C0.Put_Bool("Video", "StretchMovies", StretchMovies);
	DAT_008870C0.Put_Float("Audio", "SoundVolume", SoundVolume);
	DAT_008870C0.Put_Float("Audio", "VoiceVolume", VoiceVolume);
	DAT_008870C0.Put_Float("Audio", "ScoreVolume", ScoreVolume);
	DAT_008870C0.Put_Bool("Audio", "IsScoreRepeat", IsScoreRepeat);
	DAT_008870C0.Put_Bool("Audio", "IsScoreShuffle", IsScoreShuffle);
	DAT_008870C0.Put_Int("Audio", "SoundLatency", SoundLatency);
	DAT_008870C0.Put_Bool("Audio", "InGameMusic", InGameMusic);
	DAT_008870C0.Put_Int("Network", "Socket", Socket);
	DAT_008870C0.Put_Int("Network", "NetCard", NetCard);
	DAT_008870C0.Put_String("Network", "DestNet", DestNet);

	wchar_t netid[200];
	swprintf(netid, L"%d %d", LastUnlockedSovMovie + 1, LastUnlockedAllMovie + 1);
	for (wchar_t * c = netid; *c; c++) {
		*c = ~*c;
		if (*c == 0) {
			*c = 0xFFFF;
		}
	}
	DAT_008870C0.Put_UnicodeString("Network", "NetID", netid);

	DAT_008870C0.Save(file, false);

	Setup_Dest_Net();
}

// FUNCTION: 0x005FB050
void OptionsClass::Setup_Dest_Net(void)
{
	if (Socket == 0xFFFF) {
		Ipx.Set_Socket(VIRGIN_SOCKET);
	} else if (Socket < 0x4000) {
		Ipx.Set_Socket(Socket + 0x4000);
	}

	int len = strlen(DestNet);
	if (len != 0) {
		char *string = new char[len + 1];
		strcpy(string, DestNet);

		NetNumType net;
		NetNodeType node;

		int i = 0;
		char * p = strtok(string, ".");
		if (p != NULL) {
			while (p) {
				int x;

				sscanf(p, "%x", &x);
				if (i < 4) {
					net[i] = (char)x;
				} else {
					node[i-4] = (char)x;
				}
				i++;
				p = strtok(NULL, ".");
			}

			if (i >= 4) {
				Session.IsBridge = true;
				memset(node, 0xff, sizeof(node));
				Session.BridgeNet = IPXAddressClass(net, node, 0);
			}
		}
	}
}

// FUNCTION: 0x005FB160
void OptionsClass::Set(void)
{
	Set_Sound_Volume(SoundVolume, false);
	Set_Voice_Volume(VoiceVolume, false);
	Set_Score_Volume(ScoreVolume, false);
	Set_Repeat(IsScoreRepeat);
	Set_Shuffle(IsScoreShuffle);
	Map.Toggle_Cameo_Text(SidebarCameoText);
	TechnoClass::Set_Action_Lines(ActionLines);
}


// FUNCTION: 0x005FB310
int OptionsClass::Normalize_Volume(int volume) const
{
	return(volume * SoundVolume);
}

BOOL CALLBACK FUN_005FB320(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x005FBEF0
bool OptionsClass::Hotkey_Dialog(void)
{
	int res = -1;

	HWND handle = OwnerDraw::Begin_Dialog(163, FUN_005FB320, 0);
	if (handle != NULL) {
		SetWindowLong(handle, DWL_USER, (LONG)&res);
		OwnerDraw::Display_Dialog(handle);
		while (res < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				res = 2;
			}
			if (!GameActive) {
				Title_Screen_Restore();
			}
		}
		OwnerDraw::End_Dialog(handle);
	}
	return(true);
}

// FUNCTION: 0x005FBF70
int OptionsClass::Get_Scroll_Method(void) const
{
	return(ScrollMethod);
}

// FUNCTION: 0x005FBF80
void OptionsClass::FUN_005FBF80(char const * name)
{
	int index = 0;
	for (MovieUnlockableInfo * info = MovieUnlockableInfo::Soviet; info->Filename; info++, index++) {
		if (stricmp(name, info->Filename) == 0) {
			goto sov_done;
		}
	}
	index = -1;
sov_done:
	if (index > LastUnlockedSovMovie) {
		LastUnlockedSovMovie = index;
	}

	index = 0;
	for (MovieUnlockableInfo * ainfo = MovieUnlockableInfo::Allied; ainfo->Filename; ainfo++, index++) {
		if (stricmp(name, ainfo->Filename) == 0) {
			goto all_done;
		}
	}
	index = -1;
all_done:
	if (index > LastUnlockedAllMovie) {
		LastUnlockedAllMovie = index;
	}
}

// FUNCTION: 0x005FC000
void OptionsClass::FUN_005FC000(HWND list)
{
	if (MovieUnlockableInfo::Common[0].Filename && MovieUnlockableInfo::Common[0].Description) {
		int item = SendMessage(list, 0x4cd, 0, (LPARAM)StringTable::LoadString(MovieUnlockableInfo::Common[0].Description, NULL, "D:\\ra2mdpost\\Options.CPP", 0x4f1));
		if (item != -1) {
			SendMessage(list, 0x19a, item, (LPARAM)MovieUnlockableInfo::Common);
		}
	}

	if (LastUnlockedSovMovie >= 0) {
		int count = LastUnlockedSovMovie + 1;
		for (MovieUnlockableInfo * info = MovieUnlockableInfo::Soviet; info->Filename && count > 0; info++, count--) {
			if (info->Filename && info->Description) {
				int item = SendMessage(list, 0x4cd, 0, (LPARAM)StringTable::LoadString(info->Description, NULL, "D:\\ra2mdpost\\Options.CPP", 0x4f1));
				if (item != -1) {
					SendMessage(list, 0x19a, item, (LPARAM)info);
				}
			}
		}
	}

	if (LastUnlockedAllMovie >= 0) {
		int count = LastUnlockedAllMovie + 1;
		for (MovieUnlockableInfo * info = MovieUnlockableInfo::Allied; info->Filename && count > 0; info++, count--) {
			if (info->Filename && info->Description) {
				int item = SendMessage(list, 0x4cd, 0, (LPARAM)StringTable::LoadString(info->Description, NULL, "D:\\ra2mdpost\\Options.CPP", 0x4f1));
				if (item != -1) {
					SendMessage(list, 0x19a, item, (LPARAM)info);
				}
			}
		}
	}
}

// FUNCTION: 0x005FC120 Binary_Search
template IndexClass<int, CommandClass const *>::NodeElement * Binary_Search(IndexClass<int, CommandClass const *>::NodeElement *, int, IndexClass<int, CommandClass const *>::NodeElement const &);
