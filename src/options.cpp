// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "geometry.h"
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

// SYNTHETIC: GAMEMD 0x005FA0D0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005FA100 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005FA120 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005FA140 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005FA160 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005FA180 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005FA1A0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005FA1E0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005FA210 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005FA240 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005FA270 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005FA280 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005FA2A0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005FA2C0 SYMBOL
// _$E41

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

// FUNCTION: GAMEMD 0x005FA2E0
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

// FUNCTION: GAMEMD 0x005FA310
int __fastcall Key_To_Hotkey(unsigned short key)
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

// FUNCTION: GAMEMD 0x005FA350
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

// FUNCTION: GAMEMD 0x005FA430
void OptionsClass::One_Time(void)
{
}

// FUNCTION: GAMEMD 0x005FA440
void OptionsClass::Set_Shuffle(bool on)
{
	IsScoreShuffle = on;
	Theme.Set_Shuffle(on);
	DebugString("ScoreShuffle is %s\n", IsScoreShuffle == true ? "ON" : "OFF");
}

// FUNCTION: GAMEMD 0x005FA470
void OptionsClass::Set_Repeat(bool on)
{
	IsScoreRepeat = on;
	Theme.Set_Repeat(on);
	DebugString("ScoreRepeat is %s\n", IsScoreRepeat == true ? "ON" : "OFF");
}

// FUNCTION: GAMEMD 0x005FA4A0
void OptionsClass::Set_Score_Volume(float volume, bool feedback)
{
	ScoreVolume = MIN(volume, 1.0);
	FUN_00407150(DAT_0087E744, ScoreVolume * 16384.0f);
	Theme.Set_Volume(ScoreVolume * 255.0);
	DebugString("ScoreVolume = %f\n", ScoreVolume);
}

// FUNCTION: GAMEMD 0x005FA510
void OptionsClass::Set_Sound_Volume(float volume, bool feedback)
{
	SoundVolume = MIN(volume, 1.0);
	FUN_00407150(DAT_0087E748, SoundVolume * 16384.0f);
	if (feedback) {
		VocClass::PlayGlobal(Rule->GenericBeep, 1.0f, 0x2000, NULL);
	}
	DebugString("SoundVolume = %f\n", SoundVolume);
}

// FUNCTION: GAMEMD 0x005FA590
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

extern wchar_t const DAT_00887734[];
extern char DAT_00889F64[];

// FUNCTION: GAMEMD 0x005FA620
void OptionsClass::Load_Settings(void)
{
	DebugString("--------- Loading SUN.INI settings ---------------\n");

	GameSpeed = DAT_008870C0.Get_Int("Options", "GameSpeed", GameSpeed);
	DebugString("GameSpeed = %d\n", GameSpeed);

	Difficulty = DAT_008870C0.Get_Int("Options", "Difficulty", Difficulty);
	Difficulty = MIN(Difficulty, 4);
	Difficulty = MAX(Difficulty, 0);
	DebugString("Difficulty = %d\n", Difficulty);

	CampDifficulty = DAT_008870C0.Get_Int("Options", "CampDifficulty", CampDifficulty);
	CampDifficulty = MIN(CampDifficulty, 2);
	CampDifficulty = MAX(CampDifficulty, 0);
	DebugString("CampDifficulty = %d\n", CampDifficulty);

	ScrollMethod = DAT_008870C0.Get_Int("Options", "ScrollMethod", ScrollMethod);
	DebugString("ScrollMethod = %d\n", ScrollMethod);

	ScrollRate = DAT_008870C0.Get_Int("Options", "ScrollRate", ScrollRate);
	DebugString("ScrollRate = %d\n", ScrollRate);

	AutoScroll = DAT_008870C0.Get_Bool("Options", "AutoScroll", AutoScroll);
	DebugString("AutoScroll is %s\n", AutoScroll == true ? "ON" : "OFF");

	DetailLevel = DAT_008870C0.Get_Int("Options", "DetailLevel", DetailLevel);
	DetailLevel = MIN(DetailLevel, 2);
	DetailLevel = MAX(DetailLevel, 0);
	DebugString("DetailLevel = %d\n", DetailLevel);

	IsSidebarOnRight = true;
	DebugString("SideBar on %s\n", IsSidebarOnRight == true ? "RIGHT" : "LEFT");

	SidebarCameoText = DAT_008870C0.Get_Bool("Options", "SidebarCameoText", SidebarCameoText);
	DebugString("Sidebar Text is %s\n", SidebarCameoText == true ? "ON" : "OFF");

	ActionLines = DAT_008870C0.Get_Bool("Options", "UnitActionLines", ActionLines);
	DebugString("ActionLines are %s\n", ActionLines == true ? "ON" : "OFF");

	ShowHidden = DAT_008870C0.Get_Bool("Options", "ShowHidden", ShowHidden);
	DebugString("ShowHidden is %s\n", ShowHidden == true ? "ON" : "OFF");

	ToolTips = DAT_008870C0.Get_Bool("Options", "ToolTips", ToolTips);
	DebugString("ToolTips are %s\n", ToolTips == true ? "ON" : "OFF");

	ScreenWidth = DAT_008870C0.Get_Int("Video", "ScreenWidth", ScreenWidth);
	ScreenHeight = DAT_008870C0.Get_Int("Video", "ScreenHeight", ScreenHeight);
	DebugString("Resolution = %d X %d\n", ScreenWidth, ScreenHeight);

	StretchMovies = DAT_008870C0.Get_Bool("Video", "StretchMovies", StretchMovies);
	StretchMovies = StretchMovies == true && DSurface::AllowStretchBlits == true;
	DebugString("StretchMovies is %s\n", StretchMovies == true ? "ON" : "OFF");

	AllowHiResModes = DAT_008870C0.Get_Bool("Video", "AllowHiResModes", AllowHiResModes);
	DebugString("AllowHiRes is %s\n", AllowHiResModes == true ? "ON" : "OFF");

	Debug_IngameModeChange = DAT_008870C0.Get_Bool("Video", "AllowModeToggle", Debug_IngameModeChange);

	AllowVRAMSidebar = DAT_008870C0.Get_Bool("Video", "AllowVRAMSidebar", AllowVRAMSidebar);

	Set_Sound_Volume(DAT_008870C0.Get_Float("Audio", "SoundVolume", SoundVolume), false);
	Set_Voice_Volume(DAT_008870C0.Get_Float("Audio", "VoiceVolume", VoiceVolume), false);
	Set_Score_Volume(DAT_008870C0.Get_Float("Audio", "ScoreVolume", ScoreVolume), false);

	Set_Repeat(DAT_008870C0.Get_Bool("Audio", "IsScoreRepeat", IsScoreRepeat));
	Set_Shuffle(DAT_008870C0.Get_Bool("Audio", "IsScoreShuffle", IsScoreShuffle));

	InGameMusic = DAT_008870C0.Get_Bool("Audio", "InGameMusic", InGameMusic);

	LastUnlockedSovMovie = -1;
	LastUnlockedAllMovie = -1;
	wchar_t netid[200];
	DAT_008870C0.Get_UnicodeString("Network", "NetID", DAT_00887734, netid, 200);
	if (netid[0] != 0) {
		for (wchar_t * c = netid; *c; c++) {
			*c = ~*c;
			if (*c == 0) {
				*c = 0xFFFF;
			}
		}
		wchar_t * token = wcstok(netid, L" ");
		if (token != NULL) {
			LastUnlockedSovMovie = wcstol(token, NULL, 10) - 1;
			token = wcstok(NULL, L" ");
			if (token != NULL) {
				LastUnlockedAllMovie = wcstol(token, NULL, 10) - 1;
			}
		}
	}
	if (LastUnlockedSovMovie < -1 || LastUnlockedSovMovie >= 8) {
		LastUnlockedSovMovie = -1;
	}
	if (LastUnlockedAllMovie < -1 || LastUnlockedAllMovie >= 8) {
		LastUnlockedAllMovie = -1;
	}

	SoundLatency = DAT_008870C0.Get_Int("Audio", "SoundLatency", SoundLatency);
	DebugString("Emulated sound card latency default = %d\n", SoundLatency);

	Socket = DAT_008870C0.Get_Int("Network", "Socket", Socket);
	NetCard = DAT_008870C0.Get_Int("Network", "NetCard", NetCard);
	DAT_008870C0.Get_String("Network", "DestNet", DAT_00889F64, DestNet, sizeof(DestNet));

	DebugString("--------- Complete -------------------------------\n");

	Map.Toggle_Cameo_Text(SidebarCameoText);
	TechnoClass::Set_Action_Lines(ActionLines);
	Setup_Dest_Net();
}

// FUNCTION: GAMEMD 0x005FB2E0
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

// FUNCTION: GAMEMD 0x005FAD10
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

// FUNCTION: GAMEMD 0x005FB050
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

// FUNCTION: GAMEMD 0x005FB160
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


// FUNCTION: GAMEMD 0x005FB310
int OptionsClass::Normalize_Volume(int volume) const
{
	return(volume * SoundVolume);
}

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	void __fastcall GetKeyboardKeyString(unsigned short key, wchar_t * buffer);
}

bool __fastcall FUN_0048BB40(CommandClass const * command, int key);
bool __fastcall FUN_0048BB60(int key);

// FUNCTION: GAMEMD 0x005FB320
BOOL CALLBACK Hotkey_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	wchar_t buffer[64];
	int * retval;
	static int current_selection = -1;

	int result = UI::StandardWndProc(window, message, wparam, lparam);
	if (result) {
		return(result);
	}

	retval = (int *)GetWindowLong(window, DWL_USER);

	switch (message) {
		case 0x466: {
			HWND list_commands = GetDlgItem(window, 0x4C8);
			int selection = SendMessage(list_commands, LB_GETCURSEL, 0, 0);
			if (selection != LB_ERR) {
				CommandClass const * cmd = (CommandClass const *)SendMessage(list_commands, LB_GETITEMDATA, selection, 0);
				HWND hotkey = GetDlgItem(window, 0x4CD);
				int key = Hotkey_To_Key(SendMessage(hotkey, 0x402, 0, 0));

				if (!FUN_0048BB40(cmd, key)) {
					HWND error = GetDlgItem(window, 0x63D);
					SendMessage(error, 0x4B2, 0, (LPARAM)StringTable::LoadString("Error:CannotMap", NULL, "D:\\ra2mdpost\\Options.CPP", 0x3EC));
				} else if (!FUN_0048BB60(key)) {
					HWND error = GetDlgItem(window, 0x63D);
					SendMessage(error, 0x4B2, 0, (LPARAM)StringTable::LoadString("Error:CannotRemap", NULL, "D:\\ra2mdpost\\Options.CPP", 0x3F4));
					return(TRUE);
				} else {
					for (int i = 0; i < HotkeyCommands.Count(); i++) {
						if (HotkeyCommands.Fetch_By_Position(i) == cmd) {
							HotkeyCommands.Remove_Index(HotkeyCommands.Fetch_ID_By_Position(i));
							break;
						}
					}
					if (key != 0) {
						HotkeyCommands.Remove_Index(key);
						HotkeyCommands.Add_Index(key, cmd);
						return(TRUE);
					}
				}
			}
			return(TRUE);
		}

		case 0x467: {
			HWND cmb_category = GetDlgItem(window, 0x4C7);
			SendMessage(cmb_category, CB_RESETCONTENT, 0, 0);
			for (int i = 0; i < AllCommands.Count(); i++) {
				CommandClass const * cmd = AllCommands[i];
				if (SendMessage(cmb_category, 0x4BE, 0, (LPARAM)cmd->Get_Category()) == -1) {
					SendMessage(cmb_category, 0x4C2, 0, (LPARAM)cmd->Get_Category());
				}
			}
			SendMessage(cmb_category, CB_SETCURSEL, 0, 0);
			current_selection = -1;
			SendMessage(window, 0x464, 0, 0);
			return(TRUE);
		}

		case 0x465: {
			HWND list_commands = GetDlgItem(window, 0x4C8);
			int selection = SendMessage(list_commands, LB_GETCURSEL, 0, 0);
			if (selection != LB_ERR) {
				CommandClass const * cmd = (CommandClass const *)SendMessage(list_commands, LB_GETITEMDATA, selection, 0);
				HWND description = GetDlgItem(window, 0x4CB);
				SendMessage(description, 0x4B2, 0, (LPARAM)cmd->Get_Description());

				int key = 0;
				for (int i = 0; i < HotkeyCommands.Count(); i++) {
					if (HotkeyCommands.Fetch_By_Position(i) == cmd) {
						key = HotkeyCommands.Fetch_ID_By_Position(i);
						break;
					}
				}

				HWND key_label = GetDlgItem(window, 0x63C);
				UI::GetKeyboardKeyString(Key_To_Hotkey(key), buffer);
				SendMessage(key_label, 0x4B2, 0, (LPARAM)buffer);

				HWND hotkey = GetDlgItem(window, 0x4CD);
				SendMessage(hotkey, 0x401, 0, 0);

				HWND hotkey_name = GetDlgItem(window, 0x63B);
				SendMessage(hotkey_name, 0x4B2, 0, (LPARAM)DAT_00887734);
				return(TRUE);
			}
			return(TRUE);
		}

		case 0x464: {
			HWND cmb_category = GetDlgItem(window, 0x4C7);
			if (SendMessage(cmb_category, CB_GETCURSEL, 0, 0) != current_selection) {
				current_selection = SendMessage(cmb_category, CB_GETCURSEL, 0, 0);
				SendMessage(cmb_category, 0x4B3, ARRAY_SIZE(buffer), (LPARAM)buffer);
				HWND list_commands = GetDlgItem(window, 0x4C8);
				SendMessage(list_commands, LB_RESETCONTENT, 0, 0);
				for (int i = 0; i < AllCommands.Count(); i++) {
					CommandClass const * cmd = AllCommands[i];
					if (wcsicmp(cmd->Get_Category(), buffer) == 0) {
						int index = SendMessage(list_commands, 0x4CD, 0, (LPARAM)cmd->Get_Display_Name());
						if (index != LB_ERR) {
							SendMessage(list_commands, LB_SETITEMDATA, index, (LPARAM)cmd);
						}
					}
				}
				HWND description = GetDlgItem(window, 0x4CB);
				SendMessage(description, 0x4B2, 0, (LPARAM)DAT_00887734);
				HWND key_label = GetDlgItem(window, 0x63C);
				SendMessage(key_label, 0x4B2, 0, (LPARAM)DAT_00887734);
				HWND hotkey_name = GetDlgItem(window, 0x63B);
				SendMessage(hotkey_name, 0x4B2, 0, (LPARAM)DAT_00887734);
				SendMessage(hotkey_name, LB_SETCURSEL, 0, 0);
				SendMessage(window, 0x465, 0, 0);
				return(TRUE);
			}
			return(TRUE);
		}

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x686:
					if (HIWORD(wparam) == BN_CLICKED) {
						CCINIClass ini;
						ini.Clear();

						for (int i = 0; i < HotkeyCommands.Count(); i++) {
							CommandClass const * cmd = HotkeyCommands.Fetch_By_Position(i);
							int key = HotkeyCommands.Fetch_ID_By_Position(i);
							ini.Put_Int("Hotkey", cmd->Get_Unique_Name(), key);
						}

						RawFileClass file("KeyboardMD.ini");
						ini.Save(file, false);
						*retval = 1;
						return(TRUE);
					}
					break;

				case IDCANCEL:
					if (HIWORD(wparam) == BN_CLICKED) {
						Init_Hotkeys();
						*retval = 2;
						return(TRUE);
					}
					break;

				case 0x4C8:
					if (HIWORD(wparam) == LBN_SELCHANGE) {
						SendMessage(window, 0x465, 0, 0);
						HWND hotkey = GetDlgItem(window, 0x4CD);
						if (hotkey != NULL) {
							SetFocus(hotkey);
							return(TRUE);
						}
					}
					break;

				case 0x4CF:
					SendMessage(window, 0x466, 0, 0);
					SendMessage(window, 0x465, 0, 0);
					return(TRUE);

				case 0x4D0:
					if (HIWORD(wparam) == BN_CLICKED) {
						DebugString("Deleting users KEYBOARDMD.INI\n");
						CCFileClass file("KEYBOARDMD.INI");
						file.Delete();
						Init_Hotkeys();
						SendMessage(window, 0x467, 0, 0);
						return(TRUE);
					}
					break;

				case 0x4C7:
					if (HIWORD(wparam) == CBN_SELCHANGE) {
						SendMessage(window, 0x464, 0, 0);
						return(TRUE);
					}
					break;

				case 0x4CD:
					if (HIWORD(wparam) == EN_CHANGE) {
						HWND error = GetDlgItem(window, 0x63D);
						SendMessage(error, 0x4B2, 0, (LPARAM)DAT_00887734);
						int key = Hotkey_To_Key(SendMessage((HWND)lparam, 0x402, 0, 0));
						wchar_t const * key_name;
						if (HotkeyCommands.Is_Present(key)) {
							key_name = HotkeyCommands[key]->Get_Display_Name();
							if (key_name == NULL) {
								key_name = DAT_00887734;
							}
						} else {
							key_name = DAT_00887734;
						}
						HWND hotkey_name = GetDlgItem(window, 0x63B);
						SendMessage(hotkey_name, 0x4B2, 0, (LPARAM)key_name);
						return(TRUE);
					}
					break;
			}
			return(TRUE);

		case 0x497:
			SendMessage(window, 0x467, 0, 0);
			return(FALSE);
	}

	return(FALSE);
}

// FUNCTION: GAMEMD 0x005FBEF0
bool OptionsClass::Hotkey_Dialog(void)
{
	int res = -1;

	HWND handle = OwnerDraw::Begin_Dialog(163, Hotkey_Dialog_Proc, 0);
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

// FUNCTION: GAMEMD 0x005FBF70
int OptionsClass::Get_Scroll_Method(void) const
{
	return(ScrollMethod);
}

// FUNCTION: GAMEMD 0x005FBF80
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

// FUNCTION: GAMEMD 0x005FC000
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

// TEMPLATE: GAMEMD 0x005FC120
// ?Binary_Search@@YIPAUNodeElement@?$IndexClass@HPBVCommandClass@@@@PAU12@HABU12@@Z
template IndexClass<int, CommandClass const *>::NodeElement * Binary_Search(IndexClass<int, CommandClass const *>::NodeElement *, int, IndexClass<int, CommandClass const *>::NodeElement const &);
