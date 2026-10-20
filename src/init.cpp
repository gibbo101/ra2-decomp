// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG

#include "always.h"
#include "init.h"
#include "_bench.h"
#include "_command.h"
#include "_convert.h"
#include "_font.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_mixfile.h"
#include "_mono.h"
#include "_palette.h"
#include "_pk.h"
#include "_rand.h"
#include "_rect.h"
#include "_rules.h"
#include "_script.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "_timer.h"
#include "_tooltip.h"
#include "_voxel.h"
#include "abstract.h"
#include "addon.h"
#include "aircraft.h"
#include "airctype.h"
#include "alphashp.h"
#include "anim.h"
#include "bench.h"
#include "blight.h"
#include "building.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "campaign.h"
#include "ccfile.h"
#include "cctooltip.h"
#include "cd.h"
#include "cdcntrl.h"
#include "cell.h"
#include "command.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "dialog.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "egos.h"
#include "empulse.h"
#include "enviro.h"
#include "expand.h"
#include "factory.h"
#include "fog.h"
#include "gamedlg.h"
#include "getcpu.h"
#include "geometry.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "infatype.h"
#include "ionblast.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "laser.h"
#include "light.h"
#include "lightcon.h"
#include "loaddlg.h"
#include "logic.h"
#include "mainopt.h"
#include "mixfile.h"
#include "modemgst.h"
#include "modemhst.h"
#include "mono.h"
#include "movie.h"
#include "mplayer.h"
#include "msgbox.h"
#include "netdlg.h"
#include "netdlg2.h"
#include "newmenu.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "obscure.h"
#include "overlay.h"
#include "overtype.h"
#include "ovrlight.h"
#include "ownrdraw.h"
#include "partsys.h"
#include "pcx.h"
#include "queue.h"
#include "ramfile.h"
#include "revent.h"
#include "rndstraw.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "script.h"
#include "side.h"
#include "skirmish.h"
#include "smudtype.h"
#include "stimer.h"
#include "stringtable.h"
#include "tactical.h"
#include "tag.h"
#include "team.h"
#include "techno.h"
#include "terrain.h"
#include "theme.h"
#include "timer.h"
#include "tracker.h"
#include "trigger.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "voc.h"
#include "volinfo.h"
#include "vox.h"
#include "vqoption.h"
#include "wave.h"
#include "waypoint.h"
#include "winstub.h"
#include "wonline.h"
#include "worlddom.h"
#include "wspipx.h"
#include "wsproto.h"
#include "wspudp.h"
#include "wwfont.h"
#include "bench.hh"
#include "scrnsel.hh"
#include <conio.h>
#include <ctime>
#include <dos.h>

// FUNCTION: 0x0052B7C0 $E112
// FUNCTION: 0x0052B7F0 $E115
// FUNCTION: 0x0052B810 $E118
// FUNCTION: 0x0052B830 $E121
// FUNCTION: 0x0052B850 $E124
// FUNCTION: 0x0052B870 $E127
// FUNCTION: 0x0052B890 $E130
// FUNCTION: 0x0052B8D0 $E133
// FUNCTION: 0x0052B900 $E136
// FUNCTION: 0x0052B930 $E139
// FUNCTION: 0x0052B960 $E142
// FUNCTION: 0x0052B970 $E145

extern int DAT_00A8F768;
extern int DAT_00A8F76C;
extern int DAT_00A8F770;

// FUNCTION: 0x0052B990
void FUN_0052B990(void)
{
	DAT_00A8F770 = 0;
	DAT_00A8F768 = 128;
	DAT_00A8F76C = 128;
}

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};

// FUNCTION: 0x0052B9B0
void __fastcall FUN_0052B9B0(HWND dialog)
{
	HWND control = GetDlgItem(dialog, 0x71A);
	if (control) {
		int x = (Drawing::RenderWidth <= 800) ? 0 : (Drawing::RenderWidth - 800) / 2;
		int y = (Drawing::RenderHeight <= 600) ? 0 : (Drawing::RenderHeight - 600) / 2;
		SetWindowPos(control, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW);
		SendMessage(control, 0x4E3, 1, 0);
		if (Drawing::RenderWidth == 640) {
			SendMessage(control, 0x4E4, 0, (LPARAM)"Ra2ts_s");
		} else {
			SendMessage(control, 0x4E4, 0, (LPARAM)"Ra2ts_l");
		}
	}
}

void __cdecl FUN_004068E0(char const * format, ...);
extern unsigned char DAT_00A8E960;
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void __fastcall FUN_0046CE10(CCINIClass * ini);
struct Game {
	static void __fastcall PlayMovie(const char * name, int theme, int a, int b, int c, int d);
	static bool __cdecl CallBack(void);
	static int TechLevel;
	static int Seed;
	static unsigned char RecordingFlag;
	static void InitRandom(void);
};

// FUNCTION: 0x0052CB50
bool FUN_0052CB50(void)
{
	if (DAT_00A8E960 & 4) {
		Game::PlayMovie("A00_F00E", -1, 1, 1, 1, 0);
	}
	FUN_0052FEC0(NULL, true);
	return Game::CallBack();
}

// FUNCTION: 0x0052CB80
void FUN_0052CB80(void)
{
}

// FUNCTION: 0x0052CB90
void FUN_0052CB90(void)
{
	bool found = false;
	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile("BATTLEMD*.INI", &fd);

	while (handle != INVALID_HANDLE_VALUE) {
		if ((fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_TEMPORARY)) == 0) {
			const char * name = &fd.cAlternateFileName[0];
			if (strlen(name) == 0) {
				name = &fd.cFileName[0];
			}

			CCFileClass file(name);
			CCINIClass * ini = new CCINIClass;
			ini->ReadCCFile(file, false, false);

			if (stricmp(name, "BATTLEMD.INI") == 0) {
				found = true;
			}

			FUN_0046CE10(ini);
			delete ini;
		}

		if (FindNextFile(handle, &fd) == 0) {
			break;
		}
	}

	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
	}

	if (!found) {
		CCFileClass file("BATTLEMD.INI");
		CCINIClass * ini = new CCINIClass;

		if (ini != NULL) {
			ini->ReadCCFile(file, false, false);
			FUN_0046CE10(ini);
			delete ini;
		}
	}
}

// FUNCTION: 0x0052D430
BOOL CALLBACK Rules_Choice_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	char buffer[128];

	switch (message) {
		case WM_INITDIALOG: {
			Center_Window_Within_Window(window);

			DynamicVectorClass<CCINIClass *> * rules;
			rules = (DynamicVectorClass<CCINIClass *> *)lparam;

			HWND list = GetDlgItem(window, IDC_RULES_LIST);

			for (int index = 0; index < rules->Count(); index++) {
				(*rules)[index]->Get_String("General", "Name", "", buffer, sizeof(buffer));
				ListBox_AddString(list, buffer);
			}
			ListBox_SetCurSel(list, 0);
		}
		break;

		case WM_HELP:
			On_WM_HELP(lparam);
			break;

		case WM_CONTEXTMENU:
			On_WM_CONTEXTMENU(wparam);
			break;

		case WM_MOVING:
			return(On_WM_MOVING(window, wparam, lparam));

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDCANCEL:
				case IDC_RULES_OK:
					if (HIWORD(wparam) == BN_CLICKED) {
						HWND list = GetDlgItem(window, IDC_RULES_LIST);
						EndDialog(window, ListBox_GetCurSel(list));
						DestroyWindow(window);
					}
					break;
			}
			break;
	}

	return(0);
}

// FUNCTION: 0x0052D5B0
BOOL CALLBACK Pick_Protocol_Dialog(HWND window, UINT message, WPARAM wparam, LPARAM           )
{
	switch (message) {
		case WM_COMMAND:
			switch ((int)wparam) {
				case IDCANCEL:
					EndDialog(window, 0);
					break;

				case IDC_PROTOCOL_IPX:
					EndDialog(window, 1);
					break;

				case IDC_PROTOCOL_UDP:
					EndDialog(window, 2);
					break;
			}
			break;
	}
	return(FALSE);
}

// FUNCTION: 0x0052D610
void FUN_0052D610(void)
{
}

// FUNCTION: 0x0052D620
void FUN_0052D620(void)
{
}

// FUNCTION: 0x0052D630
void FUN_0052D630(void)
{
}

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}

// FUNCTION: 0x0052D640
BOOL CALLBACK FUN_0052D640(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}

	int * result = (int *)GetWindowLong(window, 8);
	switch (message) {
		case WM_PAINT:
			{
				HWND item = GetDlgItem(window, 0x71A);
				SendMessage(item, 0x4F0, 0, 0);
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x686:
					*result = 0x12;
					return(0);

				case 0x579:
					*result = 0xB;
					return(0);

				case 0x688:
					*result = 8;
					return(0);

				case 0x689:
					*result = 9;
					return(0);

				case 0x68A:
					*result = 10;
					return(0);
			}
			break;

		case 0x497: {
			HWND item = GetDlgItem(window, 0x689);
			if (item) {
				bool present = LoadOptionsClass().Files_Present();
				EnableWindow(item, present);
			}
			return(0);
		}
	}
	return(0);
}

// FUNCTION: 0x0052D790
BOOL CALLBACK FUN_0052D790(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}

	{
		int * result = (int *)GetWindowLong(window, 8);
		switch (message) {
			case WM_PAINT:
				{
					HWND item = GetDlgItem(window, 0x71A);
					SendMessage(item, 0x4F0, 0, 0);
				}
				break;

			case WM_COMMAND:
				switch (LOWORD(wparam)) {
					case 0x68D:
						*result = 0xD;
						break;

					case 0x68E:
						*result = 0xE;
						break;

					case 0x68F:
						*result = 0xF;
						break;

					case 0x686:
						*result = 0x12;
						break;
				}
				break;
		}
	}
	return(0);
}

extern int DAT_00825C80;

// FUNCTION: 0x0052D870
BOOL CALLBACK FUN_0052D870(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	bool changed = false;
	HWND list = GetDlgItem(window, 0x744);
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}

	int * result = (int *)GetWindowLong(window, 8);
	switch (message) {
		case 0x497:
			if (list) {
				Options.FUN_005FC000(list);
				if (SendMessage(list, LB_SETCURSEL, DAT_00825C80, 0) == -1) {
					SendMessage(list, LB_SETCURSEL, 0, 0);
				}
			} else if (result) {
				*result = -1;
				return(0);
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x686:
					if (result) {
						*result = -1;
					}
					break;

				case 0x744:
					if (HIWORD(wparam) == 0) {
						changed = true;
					}
					break;

				case 0x745:
					changed = true;
					break;
			}
			break;
	}

	if (list && changed) {
		int sel = SendMessage(list, LB_GETCURSEL, 0, 0);
		DAT_00825C80 = sel;
		if (sel != -1) {
			int data = SendMessage(list, LB_GETITEMDATA, sel, 0);
			if (data && result) {
				*result = data;
			}
		}
	}
	return(0);
}

// FUNCTION: 0x0052F610
void Anim_Init(void)
{
	Initialize_Options();
	Set_Option(OPTION_PLAY_FROM_MIXFILE);
}

extern unsigned char DAT_00A8ED6B;
extern int DAT_00840A70;
extern int DAT_00A8F784;
extern unsigned char DAT_0089E3A0;
extern int DAT_00A8D5FC;
extern IPXAddressClass DAT_00A8D600;
extern unsigned char DAT_00B04880;
extern unsigned char DAT_00A8EDDC;
extern int DAT_00B77E18;
extern unsigned char DAT_00A8E2C8;
extern unsigned char DAT_00A8D60C;
extern unsigned char DAT_00A8D60D;
extern int DAT_00A8EB84;
extern int DAT_00A8EB88;
extern unsigned char DAT_008175B0;
extern unsigned char DAT_0089F978;
extern unsigned char DAT_00A8ED64;
extern unsigned int DAT_00A8D5F8;
void __fastcall FUN_00734F50(int value);
void __fastcall FUN_00734F80(int value);
void __fastcall FUN_007B2720(int value);

// FUNCTION: 0x0052F620
bool Parse_Command_Line(int argc, char * argv[])
{
	Special.Init();
	DAT_00A8ED6B = false;

	for (int index = 1; index < argc; index++) {
		char * string = strupr(argv[index]);

		if (stricmp("-jabber", string) == 0) {
			FUN_00734F50(1);
			continue;
		}
		if (stricmp("-str", string) == 0) {
			FUN_00734F80(1);
			continue;
		}
		if (stricmp("-nostr", string) == 0) {
			FUN_00734F80(0);
			continue;
		}
		if (stricmp("-noaudio", string) == 0) {
			DAT_00840A70 = 0;
			continue;
		}
		if (stricmp("-record", string) == 0) {
			DAT_00A8F784 = 1;
			continue;
		}
		if (stricmp("-play", string) == 0) {
			DAT_00A8F784 = 2;
			continue;
		}

		if (stricmp("/?", string) == 0 || stricmp("-?", string) == 0 || stricmp("-h", string) == 0 || stricmp("/h", string) == 0) {
			_putws(StringTable::LoadString("TXT_OPTION_HELP_01", NULL, "D:\\ra2mdpost\\Init.CPP", 0xD56));
			_putws(StringTable::LoadString("TXT_OPTION_HELP_02", NULL, "D:\\ra2mdpost\\Init.CPP", 0xD57));
			_putws(StringTable::LoadString("TXT_OPTION_HELP_03", NULL, "D:\\ra2mdpost\\Init.CPP", 0xD58));
			_putws(StringTable::LoadString("TXT_OPTION_HELP_04", NULL, "D:\\ra2mdpost\\Init.CPP", 0xD59));
			_putws(StringTable::LoadString("TXT_OPTION_HELP_05", NULL, "D:\\ra2mdpost\\Init.CPP", 0xD5A));
			_putws(StringTable::LoadString("TXT_OPTION_HELP_06", NULL, "D:\\ra2mdpost\\Init.CPP", 0xD5B));
			return(false);
		}

		if (Obfuscate(string) == 0xD95C68A2) {
			Special.IsFromInstall = true;
			continue;
		}

		if (strstr(string, "-CD")) {
			DAT_0089E3A0 = 1;
			CDFileClass::Set_Search_Drives(&string[3]);
			continue;
		}

		if (strstr(string, "-DESTNET")) {
			NetNumType net;
			NetNodeType node;

			int i = 0;
			char * p = strtok(string + 8, ".");
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
				DAT_00A8D5FC = 1;
				memset(node, 0xff, 6);
				DAT_00A8D600 = IPXAddressClass(net, node, 0);
			}
			continue;
		}

		if (strstr(string, "-SOCKET")) {
			unsigned short socket;
			socket = (unsigned short)(atoi(string + strlen("SOCKET")));
			socket += (unsigned short)0x4000;
			if (socket >= 0x4000 && socket < 0x8000) {
				Ipx.Set_Socket(socket);
			}
			continue;
		}

		FUN_007B2720(0);

		if (strstr(string, "-MPDEBUG")) {
			DAT_00B04880 = true;
			Session.EnableMultiplayerDebug = true;
		}

		if (strstr(string, "-DROP=")) {
			FUN_007B2720((unsigned short)atoi(string + strlen("-DROP=")));
			continue;
		}

		if (strstr(string, "-SPEEDCONTROL")) {
			DAT_00A8EDDC = true;
		}
		if (strstr(string, "-DLINK1")) {
			DAT_00B77E18 = 1;
		}
		if (strstr(string, "-NETGEAR")) {
			DAT_00B77E18 = 2;
		}
		if (strstr(string, "-NOROUTER")) {
			DAT_00A8E2C8 = false;
		}

		if (strstr(string, "-STEALTH")) {
			DAT_00A8D60C = true;
			continue;
		}
		if (strstr(string, "-MESSAGES")) {
			DAT_00A8D60D = false;
			continue;
		}
		if (strstr(string, "-ATTRACT")) {
			DAT_00A8D5F8 |= 4;
			continue;
		}

		if (isdigit(string[1])) {
			sscanf(string, "-%dX%d", &DAT_00A8EB84, &DAT_00A8EB88);
			continue;
		}
		if (strcmp(string, "-480") == 0) {
			DAT_00A8EB88 = 480;
			continue;
		}
		if (strcmp(string, "-16") == 0) {
			DAT_008175B0 = true;
			continue;
		}
		if (stricmp(string, "-WIN") == 0) {
			DAT_0089F978 = true;
			continue;
		}

		if (strnicmp(string, "-X", strlen("-O")) == 0) {
			string += strlen("-X");
			while (*string) {
				char code = *string++;
				switch (toupper(code)) {
					case 'Q':
						DAT_00A8ED64 = true;
						break;
					default:
						_putws(StringTable::LoadString("TXT_INVALID", NULL, "D:\\ra2mdpost\\Init.CPP", 0xE94));
						return(false);
				}
			}
			continue;
		}
	}
	return(true);
}

void __fastcall FUN_0072E820(BSurface * image, Rect * rect, int flag);

extern int DAT_00A8B8B8;
extern int DAT_00A8ED98;
extern RandomStraw CryptRandom;
int __cdecl FUN_0053E720(void);

// FUNCTION: 0x0052FC20
void Game::InitRandom(void)
{
	FUN_004068E0("Init random number\n");
	if (DAT_00A8B8B8 || (RecordingFlag & 2) || FUN_0053E720()) {
		Scen->RandomNumber = Seed;
		NonCriticalRandomNumber = Seed;
		FUN_004068E0("Seed is %08x\n", Seed);
		return;
	}

	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		SYSTEMTIME t;
		GetSystemTime(&t);
		CryptRandom.Seed_Byte(t.wMilliseconds);
		CryptRandom.Seed_Bit(t.wSecond);
		CryptRandom.Seed_Bit(t.wSecond>>1);
		CryptRandom.Seed_Bit(t.wSecond>>2);
		CryptRandom.Seed_Bit(t.wSecond>>3);
		CryptRandom.Seed_Bit(t.wSecond>>4);
		CryptRandom.Seed_Bit(t.wMinute);
		CryptRandom.Seed_Bit(t.wMinute>>1);
		CryptRandom.Seed_Bit(t.wMinute>>2);
		CryptRandom.Seed_Bit(t.wMinute>>3);
		CryptRandom.Seed_Bit(t.wMinute>>4);
		CryptRandom.Seed_Bit(t.wHour);
		CryptRandom.Seed_Bit(t.wDay);
		CryptRandom.Seed_Bit(t.wDayOfWeek);
		CryptRandom.Seed_Bit(t.wMonth);
		CryptRandom.Seed_Bit(t.wYear);

		if (DAT_00A8ED98 != 0) {
			Seed = DAT_00A8ED98;
		} else {
			CryptRandom.Get(&Seed, sizeof(Seed));
			Seed = GetTickCount();
		}
	}

	FUN_004068E0("Seed is %08x\n", Seed);
	Scen->RandomNumber = Seed;
	NonCriticalRandomNumber = Seed;
}

// FUNCTION: 0x0052FEC0
void __fastcall FUN_0052FEC0(Surface * surface, bool flag)
{
	if (surface == NULL) {
		surface = HiddenSurface;
	}
	surface->Fill(0);
	int width = surface->Get_Width();
	int height = surface->Get_Height();
	Rect rect(0, 0, width, height);
	BSurface * image = new BSurface(width, height, 2);
	FUN_0072E820(image, &rect, 0);
	surface->Blit_From(*image, false, true);
	delete image;
	if (flag) {
		Update_Visible_Surface(false, surface);
	}
}

// FUNCTION: 0x0052FF90
void Load_Title_Page(const char * name, bool visible)
{
	HiddenSurface->Fill(0);
	Load_Title_Screen(name, HiddenSurface, &CCPalette);
	if (visible)
		Update_Visible_Surface(true, HiddenSurface);
}

// FUNCTION: 0x0052FFD0
void Blank_Hidden_Surface(bool visible)
{
	HiddenSurface->Fill(0);
	if (visible)
		Update_Visible_Surface(true, HiddenSurface);
}

// FUNCTION: 0x00530000
bool Init_Expansion_Files(void)
{
	HANDLE handle;
	WIN32_FIND_DATA ff;
	MFCD * ptr;

	handle = FindFirstFile("ECACHE*.MIX", &ff);
	while (handle != INVALID_HANDLE_VALUE) {
		if ((ff.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY)) == 0) {
			ptr = new MFCD(ff.cFileName, &FastKey);
			if (ptr != NULL) {
				ExpandMix.Add(ptr);
				ptr->Cache();
			}
		}
		if (FindNextFile(handle, &ff) == false) {
			break;
		}
	}
	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
	}

	handle = FindFirstFile("ELOCAL*.MIX", &ff);
	while (handle != INVALID_HANDLE_VALUE) {
		if ((ff.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY)) == 0) {
			ptr = new MFCD(ff.cFileName, &FastKey);
			if (ptr != NULL) {
				ExpandMix.Add(ptr);
			}
		}
		if (FindNextFile(handle, &ff) == false) {
			break;
		}
	}
	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
	}
	return(true);
}

extern MFCD * DAT_00884DF8;
extern MFCD * DAT_00884DFC;
extern MFCD * DAT_00884E48;
extern MFCD * DAT_00884E4C;
extern MFCD * DAT_00884E50;
extern MFCD * DAT_00884E54;

// FUNCTION: 0x005301A0
bool Init_Bootstrap_Mixfiles(void)
{
	int index;
	char name[64];
	MFCD * expand;
	DiskID temp;

	temp = CD::Get_Required_Disk();
	CD::Set_Required_Disk(DISK_LOCAL);

	for (index = 99; index >= 0; index--) {
		sprintf(name, "EXPANDMD%02d.MIX", index);
		if (RawFileClass(name).Is_Available()) {
			expand = new MFCD(name, &FastKey);
			if (expand != NULL) {
				ExpandMix.Add(expand);
				DebugStringNoPrefix(" %s", name);
			}
		}
	}

	DAT_00884DF8 = new MFCD("RA2MD.MIX", &FastKey);
	if (DAT_00884DF8 == NULL) {
		return(false);
	}
	DAT_00884DFC = new MFCD("RA2.MIX", &FastKey);
	if (DAT_00884DFC == NULL) {
		return(false);
	}

	DebugStringNoPrefix(" CACHE.MIX");
	DAT_00884E48 = new MFCD("CACHEMD.MIX", &FastKey);
	if ((DAT_00884E48 == NULL) || (MFCD::Cache("CACHEMD.MIX") == false)) {
		return(false);
	}

	DebugStringNoPrefix(" CACHE.MIX");
	DAT_00884E4C = new MFCD("CACHE.MIX", &FastKey);
	if ((DAT_00884E4C == NULL) || (MFCD::Cache("CACHE.MIX") == false)) {
		return(false);
	}

	DebugStringNoPrefix(" CACHE.MIX");
	DAT_00884E50 = new MFCD("LOCALMD.MIX", &FastKey);
	if (DAT_00884E50 == NULL) {
		return(false);
	}
	DAT_00884E54 = new MFCD("LOCAL.MIX", &FastKey);
	if (DAT_00884E54 == NULL) {
		return(false);
	}

	DebugStringNoPrefix(" LOCAL.MIX");
	CD::Set_Required_Disk(temp);
	return(true);
}

extern MFCD * DAT_00884E38;
extern MFCD * DAT_00884E3C;
extern MFCD * DAT_00884E40;
extern MFCD * DAT_00884E44;
void FUN_005FF420(void);
void FUN_0053D330(void);

struct DropPodVoxelStruct {
	VoxelLibrary * VoxLib;
	MotionLibrary * MotLib;
};
extern DropPodVoxelStruct DAT_00A8ECD8;

static bool Init_One_Time_Systems(void)
{
	Game::CallBack();

	Map.One_Time();
	Options.One_Time();
	Session.One_Time();
	ObjectTypeClass::One_Time();
	BuildingTypeClass::One_Time();
	ObjectTypeClass::One_Time();
	ObjectTypeClass::One_Time();
	ObjectTypeClass::One_Time();
	UnitTypeClass::One_Time();
	ObjectTypeClass::One_Time();
	ObjectTypeClass::One_Time();
	AircraftTypeClass::One_Time();
	HouseClass::One_Time();
	FUN_005FF420();
	FUN_0053D330();

	CCFileClass dropvxl("DPOD.VXL");
	DAT_00A8ECD8.VoxLib = new VoxelLibrary(dropvxl);
	if (DAT_00A8ECD8.VoxLib == NULL) {
		FUN_004068E0("Failed to create VoxLib!\n");
		return(false);
	}

	CCFileClass dropmot("DPOD.HVA");
	DAT_00A8ECD8.MotLib = new MotionLibrary(dropmot);
	if (DAT_00A8ECD8.MotLib == NULL) {
		FUN_004068E0("Failed to create MotLib!\n");
		return(false);
	}

	return(true);
}

// FUNCTION: 0x00531680
bool Init_Bulk_Data(void)
{
	if (DAT_00884E38 == NULL || !DAT_00884E38->Cache()) {
		return(false);
	}
	if (DAT_00884E3C == NULL || !DAT_00884E3C->Cache()) {
		return(false);
	}
	if (DAT_00884E40 == NULL || !DAT_00884E40->Cache()) {
		return(false);
	}
	if (DAT_00884E44 == NULL || !DAT_00884E44->Cache()) {
		return(false);
	}

	Game::CallBack();
	return(Init_One_Time_Systems());
}

extern int DAT_00A8EC90;

// FUNCTION: 0x005318C0
bool Save_Recording_Values(CCFileClass & file)
{
	FUN_004068E0("Saving recording values for scenario : %s\n", Scen->ScenarioName);
	file.Write(&Game::TechLevel, sizeof(Game::TechLevel));
	file.Write(&Game::Seed, sizeof(Game::Seed));
	file.Write(&Scen->Scenario, sizeof(Scen->Scenario));
	file.Write(Scen->ScenarioName, sizeof(Scen->ScenarioName));
	file.Write(&DAT_00A8EC90, sizeof(DAT_00A8EC90));
	file.Write(&DAT_00A8E960, 4);
	file.Write(&Options, sizeof(OptionsClass));
	return(true);
}

// FUNCTION: 0x00531960
bool Load_Recording_Values(CCFileClass & file)
{
	file.Read(&Game::TechLevel, sizeof(Game::TechLevel));
	file.Read(&Game::Seed, sizeof(Game::Seed));
	file.Read(&Scen->Scenario, sizeof(Scen->Scenario));
	file.Read(Scen->ScenarioName, sizeof(Scen->ScenarioName));
	file.Read(&DAT_00A8EC90, sizeof(DAT_00A8EC90));
	file.Read(&DAT_00A8E960, 4);
	file.Read(&Options, sizeof(OptionsClass));
	FUN_004068E0("Loaded recording values for scenario : %s\n", Scen->ScenarioName);
	return(true);
}

extern char DAT_00A8F314[];
extern char DAT_00A8F4A0[];
extern char DAT_00A8F420[];
extern char DAT_00A8F520[];
extern char DAT_00A8F394[];

// FUNCTION: 0x00531A00
BOOL CALLBACK Version_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND handle;
	int * res;
	char buffer[256];
	char build_date[128];
	char build_number[128];
	char build_by[128];
	int cpu_type;
	bool mmx;

	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}

	res = (int *)GetWindowLong(window, 8);

	switch (message) {
		case 0x497:
			handle = GetDlgItem(window, 0x5B5);
			SendMessage(handle, 0x4CD, 0, (LPARAM)StringTable::LoadString("TXT_SHORT_TITLE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1630));

			sprintf(buffer, "Version %s", Version_Name());
			SendMessage(handle, 0x4C8, 0, (LPARAM)buffer);

			sprintf(buffer, "Internal Version %s", VerNum.Version_Name());
			SendMessage(handle, 0x4C8, 0, (LPARAM)buffer);

			build_date[0] = 0;
			Build_Date_String(build_date, sizeof(build_date));
			sprintf(buffer, "Release Build: %s by %s - %s", Build_Number_String(build_number, sizeof(build_number)), Build_By_String(build_by, sizeof(build_by)), build_date);
			SendMessage(handle, 0x4C8, 0, (LPARAM)buffer);

			cpu_type = 5;
			mmx = false;
			SendMessage(handle, 0x4C8, 0, (LPARAM)DAT_00A8F314);
			SendMessage(handle, 0x4C8, 0, (LPARAM)DAT_00A8F4A0);
			SendMessage(handle, 0x4C8, 0, (LPARAM)DAT_00A8F420);
			SendMessage(handle, 0x4C8, 0, (LPARAM)DAT_00A8F520);
			SendMessage(handle, 0x4C8, 0, (LPARAM)DAT_00A8F394);
			Get_CPU_Type(cpu_type, mmx, NULL, 0);
			sprintf(buffer, "MMX %s, Vendor: %s", mmx ? "Yes" : "No", "Not available");
			SendMessage(handle, 0x4C8, 0, (LPARAM)buffer);

			Get_Language_Version(buffer);
			SendMessage(handle, 0x4C8, 0, (LPARAM)buffer);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDCANCEL:
				case IDOK:
					*res = LOWORD(wparam);
					break;
			}
			break;
	}
	return(FALSE);
}


namespace UI {
	HWND __fastcall BeginDialog(int id, DLGPROC proc, int extra);
	void __fastcall EndDialog(HWND window);
}

// FUNCTION: 0x00531C30
void Version_Dialog(void)
{
	HWND dialog;
	int res = 0;

	dialog = UI::BeginDialog(0xD4, (DLGPROC)Version_Dialog_Proc, 0);
	if (dialog != NULL) {
		SetWindowLong(dialog, DWL_USER, (LONG)&res);
		OwnerDraw::Display_Dialog(dialog);

		while (res == 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}
		UI::EndDialog(dialog);
	}
}

struct CheatEntryStruct {
	bool * State;
	const char * CheatString;
	const char * VersionSuffix;
	bool IsAllowedInMP;
};

extern CheatEntryStruct OriginalCheats[4];
extern char DAT_00A8F7B8[32];
void __fastcall FUN_004790B0(int disk);
void __cdecl FUN_004A4AC0(const char * text);
short __fastcall FUN_0054F000(void * input);
short __fastcall FUN_0054F050(void * input);
extern void * InputManagerInstance;
void __fastcall FUN_0052B9B0(HWND dialog);

static bool Cheat_Key_Process(char chr)
{
	if (!isalnum(chr) || chr == '~') {
		memset(DAT_00A8F7B8, 0, sizeof(DAT_00A8F7B8));
		return(false);
	}

	char tmp[2];
	tmp[0] = chr;
	tmp[1] = 0;
	FUN_004A4AC0(tmp);

	int len = strlen(DAT_00A8F7B8);
	if (len >= (int)sizeof(DAT_00A8F7B8) - 1) {
		len = 0;
		DAT_00A8F7B8[0] = 0;
	}
	DAT_00A8F7B8[len] = toupper(chr);

	for (int c = 0; c < 4; c++) {
		if (strstr(DAT_00A8F7B8, OriginalCheats[c].CheatString) != NULL) {
			*OriginalCheats[c].State = !(*OriginalCheats[c].State);
			memset(DAT_00A8F7B8, 0, sizeof(DAT_00A8F7B8));
			return(true);
		}
	}
	return(false);
}

BOOL CALLBACK Main_Menu_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x00531CC0
int Main_Menu(unsigned int timeout)
{
	HWND dialog;
	int retval = 0x12;

	FUN_004790B0(-1);

	dialog = UI::BeginDialog(0xE2, (DLGPROC)Main_Menu_Dialog_Proc, 0);
	if (dialog != NULL) {
		SetWindowLong(dialog, DWL_USER, (LONG)&retval);
		Center_Window_Within_Window(dialog, MainWindow);
		OwnerDraw::Display_Dialog(dialog);
		SetFocus(MainWindow);

		FUN_0052B9B0(dialog);

		do {
			if (OwnerDraw::Dialog_Message_Handler()) {
				retval = 7;
			}
			Title_Screen_Restore();

			if (FUN_0054F000(InputManagerInstance)) {
				int input = (unsigned short)FUN_0054F050(InputManagerInstance);
				if ((input & 0x800) == 0) {
					if (Cheat_Key_Process((char)input) == true) {
						VocClass::PlayGlobal(Rule->OptionsChanged, 1.0f, 0x2000);
						Title_Screen_Restore(true);
					}
				}
			}
		} while (retval == 0x12);

		UI::EndDialog(dialog);

		SYSTEMTIME t;
		GetSystemTime(&t);
		CryptRandom.Seed_Byte(t.wMilliseconds);
	} else {
		retval = 7;
	}

	SetFocus(MainWindow);
	return(retval);
}

// FUNCTION: 0x00531F60
BOOL CALLBACK Main_Menu_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int * res;

	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return(rc);
	}

	res = (int *)GetWindowLong(window, DWL_USER);

	switch (message) {
		case 0x497: {
			HWND control = GetDlgItem(window, 0x71D);
			wchar_t text[128];
			swprintf(text, L"%s %s", StringTable::LoadString("GUI:Version", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1757), FUN_00735120(VerNum.Version_Name()));
			SendMessage(control, 0x4B2, 0, (LPARAM)text);
		}
		break;
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x683: *res = 1; break;
				case 0x684: *res = 2; break;
				case 0x578: *res = 3; break;
				case 0x686: *res = 4; break;
				case 0x55C: *res = 5; break;
				case 0x3EE: *res = 6; break;
			}
			break;
		case WM_PAINT:
			HWND item = GetDlgItem(window, 0x71A);
			SendMessage(item, 0x4F0, 0, 0);
			break;
	}
	return(FALSE);
}

// FUNCTION: 0x00532100
void Title_Screen_Restore(bool force)
{
	if (SurfacesRestored || force) {
		SurfacesRestored = false;
		FUN_0052FEC0(NULL, true);
		Update_Visible_Surface(false, HiddenSurface);
	}
}

// FUNCTION: 0x00532130
void FUN_00532130(void)
{
}

// FUNCTION: 0x00532140
void FUN_00532140(void)
{
}

extern char DAT_00A8F5B4[];
extern wchar_t DAT_00A8F654[];
extern unsigned char DAT_00A8B8B5;
extern unsigned char DAT_00A8F7D8;
void __fastcall FUN_00731060(int group);
void __fastcall FUN_007311C0(int group);
void __fastcall FUN_007313A0(int group);
void __fastcall FUN_007314C0(int group);
void FUN_00730AF0(void);
void FUN_00730D60(void);
void FUN_00730EA0(void);
void FUN_00730F30(void);
void FUN_00730FE0(void);
void FUN_007315A0(void);
void FUN_007318F0(void);
void FUN_00731A10(void);
void FUN_00731A30(void);
void __fastcall FUN_00731A50(bool flag);
void __fastcall FUN_00731A70(bool flag);
void __fastcall FUN_00732280(bool flag);
void FUN_00732CA0(void);
void FUN_00732CC0(void);
void __fastcall FUN_00733380(bool redraw);
void __fastcall FUN_007336C0(bool redraw);
void FUN_00734120(void);
BuildingClass * FUN_007342A0(void);
BuildingClass * FUN_007342B0(void);


class CreateTeamCommandClass : public CommandClass
{
	public:
		CreateTeamCommandClass(int team) : Team(team) {}

		virtual char const * Get_Unique_Name(void) const
		{
			sprintf(DAT_00A8F5B4, "TeamCreate_%d", Team);
			return DAT_00A8F5B4;
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_CREATE_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1815), Team);
			return DAT_00A8F654;
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1817);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_CREATE_TEAM_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1819), Team);
			return DAT_00A8F654;
		}
		virtual void Execute(int key) const
		{
			FUN_00731060(Team);
		}

	private:
		int Team;
};

class SetStructureTabCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "StructureTab";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_STRUCTURE_TAB", NULL, "D:\\ra2mdpost\\Init.CPP", 0x182A);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x182C);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_STRUCTURE_TAB_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x182E);
		}
		virtual void Execute(int key) const
		{
			if (Map.Column[0].FUN_006ABAC0()) {
				Map.SetTab(0);
				BuildingClass * building = FUN_007342A0();
				if (building != NULL) {
					PlayerPtr->Manual_Place(building->Who_Can_Build_Me(false, false), building);
				}
			}
		}
};

class SetDefenseTabCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "DefenseTab";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_DEFENSE_TAB", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1844);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1846);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_DEFENSE_TAB_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1848);
		}
		virtual void Execute(int key) const
		{
			if (Map.Column[1].FUN_006ABAC0()) {
				Map.SetTab(1);
				BuildingClass * building = FUN_007342B0();
				if (building != NULL) {
					PlayerPtr->Manual_Place(building->Who_Can_Build_Me(false, false), building);
				}
			}
		}
};

class SetInfantryTabCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "InfantryTab";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_INFANTRY_TAB", NULL, "D:\\ra2mdpost\\Init.CPP", 0x185E);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1860);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_INFANTRY_TAB_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1862);
		}
		virtual void Execute(int key) const
		{
			if (Map.Column[2].FUN_006ABAC0()) {
				Map.SetTab(2);
			}
		}
};

class SetUnitTabCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "UnitTab";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_UNIT_TAB", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1873);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1875);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_UNIT_TAB_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1877);
		}
		virtual void Execute(int key) const
		{
			if (Map.Column[3].FUN_006ABAC0()) {
				Map.SetTab(3);
			}
		}
};

class SelectTeamCommandClass : public CommandClass
{
	public:
		SelectTeamCommandClass(int team) : Team(team) {}

		virtual char const * Get_Unique_Name(void) const
		{
			sprintf(DAT_00A8F5B4, "TeamSelect_%d", Team);
			return DAT_00A8F5B4;
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_SELECT_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1894), Team);
			return DAT_00A8F654;
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1896);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_SELECT_TEAM_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1898), Team);
			return DAT_00A8F654;
		}
		virtual void Execute(int key) const
		{
			FUN_007311C0(Team);
		}

	private:
		int Team;
};

class AddTeamCommandClass : public CommandClass
{
	public:
		AddTeamCommandClass(int team) : Team(team) {}

		virtual char const * Get_Unique_Name(void) const
		{
			sprintf(DAT_00A8F5B4, "TeamAddSelect_%d", Team);
			return DAT_00A8F5B4;
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_ADD_SELECT_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x18B6), Team);
			return DAT_00A8F654;
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x18B8);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_ADD_SELECT_TEAM_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x18BA), Team);
			return DAT_00A8F654;
		}
		virtual void Execute(int key) const
		{
			FUN_007314C0(Team);
		}

	private:
		int Team;
};

class CenterTeamCommandClass : public CommandClass
{
	public:
		CenterTeamCommandClass(int team) : Team(team) {}

		virtual char const * Get_Unique_Name(void) const
		{
			sprintf(DAT_00A8F5B4, "TeamCenter_%d", Team);
			return DAT_00A8F5B4;
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_CENTER_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x18D9), Team);
			return DAT_00A8F654;
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_TEAM", NULL, "D:\\ra2mdpost\\Init.CPP", 0x18DB);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_CENTER_TEAM_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x18DD), Team);
			return DAT_00A8F654;
		}
		virtual void Execute(int key) const
		{
			FUN_007313A0(Team);
		}

	private:
		int Team;
};

class TauntCommandClass : public CommandClass
{
	public:
		TauntCommandClass(int team) : Team(team) {}

		virtual char const * Get_Unique_Name(void) const
		{
			sprintf(DAT_00A8F5B4, "Taunt_%d", Team);
			return DAT_00A8F5B4;
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_TAUNT_NUMBER", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1900), Team);
			return DAT_00A8F654;
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_TAUNT", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1905);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			swprintf(DAT_00A8F654, StringTable::LoadString("TXT_TAUNT_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1909), Team);
			return DAT_00A8F654;
		}
		virtual void Execute(int key) const;

	private:
		int Team;
};

class NextObjectCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "NextObject";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_NEXT_OBJECT", NULL, "D:\\ra2mdpost\\Init.CPP", 0x197E);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1980);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_NEXT_OBJECT_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1982);
		}
		virtual void Execute(int key) const
		{
			Map.Power_Mode_Control(0);
			Map.Waypoint_Mode_Control(0, false);
			Map.Repair_Mode_Control(0);
			Map.Sell_Mode_Control(0);
			ObjectClass * obj = Map.Next_Object(CurrentObject.Count() ? CurrentObject[0] : NULL);
			if (obj != NULL) {
				Unselect_All();
				obj->Select();
				Map.Center_Map();
				Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
			}
		}
};

class PlanningModeCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "PlanningMode";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:PlanningMode", NULL, "D:\\ra2mdpost\\Init.CPP", 0x19B4);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x19B6);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:PlanningModeDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x19B8);
		}
		virtual bool Unknown14(int key) const
		{
			return (key & WWKEY_SHIFT_BIT) || (key & WWKEY_CTRL_BIT) || (key & WWKEY_ALT_BIT);
		}
		virtual bool Unknown18(int key) const
		{
			return true;
		}
		virtual bool Unknown1C(int key) const
		{
			return !(key & WWKEY_RLS_BIT);
		}
		virtual void Execute(int key) const
		{
			if (key & WWKEY_RLS_BIT) {
				FUN_00731A70(false);
			} else {
				FUN_00731A50(false);
			}
		}
};

class CombatantSelectCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "CombatantSelect";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:CombatantSelect", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A08);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A0A);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:CombatantSelectDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A0C);
		}
		virtual bool Unknown14(int key) const
		{
			return (key & WWKEY_SHIFT_BIT) != 0;
		}
		virtual void Execute(int key) const
		{
			FUN_00732280((key & WWKEY_SHIFT_BIT) == 0);
		}
};

class TypeSelectCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "TypeSelect";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:TypeSelect", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A22);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A24);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:TypeSelectDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A26);
		}
		virtual bool Unknown14(int key) const
		{
			return (key & WWKEY_SHIFT_BIT) != 0;
		}
		virtual bool Unknown18(int key) const
		{
			return true;
		}
		virtual bool Unknown1C(int key) const
		{
			return !(key & WWKEY_RLS_BIT);
		}
		virtual void Execute(int key) const
		{
			if (key & WWKEY_RLS_BIT) {
				FUN_00732CC0();
			} else {
				FUN_00732CA0();
			}
		}
};

class HealthNavCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "HealthNav";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:HealthNavigation", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A48);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A4A);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:HealthNavigationDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A4C);
		}
		virtual bool Unknown14(int key) const
		{
			return (key & WWKEY_SHIFT_BIT) != 0;
		}
		virtual void Execute(int key) const
		{
			FUN_00733380((key & WWKEY_SHIFT_BIT) == 0);
		}
};

class VeterancyNavCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "VeterancyNav";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:VetNavigation", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A62);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A64);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:VetNavigationDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1A66);
		}
		virtual bool Unknown14(int key) const
		{
			return (key & WWKEY_SHIFT_BIT) != 0;
		}
		virtual void Execute(int key) const
		{
			FUN_007336C0((key & WWKEY_SHIFT_BIT) == 0);
		}
};

class PrevObjectCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "PreviousObject";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_PREV_OBJECT", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AA1);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AA3);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_PREV_OBJECT_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AA5);
		}
		virtual void Execute(int key) const
		{
			Map.Power_Mode_Control(0);
			Map.Waypoint_Mode_Control(0, false);
			Map.Repair_Mode_Control(0);
			Map.Sell_Mode_Control(0);
			ObjectClass * obj = Map.Prev_Object(CurrentObject.Count() ? CurrentObject[0] : NULL);
			if (obj != NULL) {
				Unselect_All();
				obj->Select();
				Map.Center_Map();
				Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
			}
		}
};

class StopCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "StopObject";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_STOP_OBJECT", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1ACA);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1ACC);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_STOP_OBJECT_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1ACE);
		}
		virtual void Execute(int key) const
		{
			FUN_00730EA0();
		}
};

class AllToCheerCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "AllToCheer";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:AllCheer", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1ADC);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1ADE);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:AllCheerDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AE0);
		}
		virtual void Execute(int key) const
		{
			FUN_00730F30();
		}
};

class DeployCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "DeployObject";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_DEPLOY_OBJECT", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AF7);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AF9);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_DEPLOY_OBJECT_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1AFB);
		}
		virtual void Execute(int key) const
		{
			FUN_00730AF0();
		}
};

class GuardCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "GuardObject";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_GUARD", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B13);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B15);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_GUARD_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B17);
		}
		virtual void Execute(int key) const
		{
			FUN_00730D60();
		}
};

class ScatterCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "ScatterObject";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SCATTER", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B30);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B32);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SCATTER_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B34);
		}
		virtual void Execute(int key) const
		{
			FUN_00730FE0();
		}
};

class CenterViewCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "CenterView";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_CENTER_VIEW", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B4B);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B4D);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_CENTER_VIEW_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B4F);
		}
		virtual void Execute(int key) const
		{
			if (CurrentObject.Count() > 0) {
				Map.Center_Map();
				Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
			}
		}
};

class CenterBaseCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "CenterBase";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_CENTER_BASE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B6A);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_SELECTION", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B6C);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_CENTER_BASE_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B6E);
		}
		virtual void Execute(int key) const
		{
			FUN_007315A0();
		}
};

class AllianceCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "ToggleAlliance";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_ALLIANCE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B85);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_CONTROL", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B87);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_ALLIANCE_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1B89);
		}
		virtual void Execute(int key) const
		{
			FUN_007318F0();
		}
};

class ToggleRepairCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "ToggleRepair";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_REPAIR_MODE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1BBF);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1BC1);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_REPAIR_MODE_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1BC3);
		}
		virtual void Execute(int key) const
		{
			Map.Repair_Mode_Control(-1);
		}
};

class ToggleSellCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "ToggleSell";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SELL_MODE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1BD9);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1BDB);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SELL_MODE_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1BDD);
		}
		virtual void Execute(int key) const
		{
			Map.Sell_Mode_Control(-1);
		}
};

class BeaconPlacementCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "PlaceBeacon";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_PLACE_BEACON", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C04);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C06);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_PLACE_BEACON_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C08);
		}
		virtual void Execute(int key) const
		{
			if (Session.Type != GAME_NORMAL && Session.Type != 5) {
				FUN_00731A30();
			}
		}
};

class CenterREventCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "CenterOnRadarEvent";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_RADAR_EVENT", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C1B);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C1D);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_RADAR_EVENT_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C1F);
		}
		virtual void Execute(int key) const
		{
			FUN_00734120();
		}
};

class SidebarUpCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "SidebarUp";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SIDEBAR_UP", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C51);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C53);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SIDEBAR_UP_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1C55);
		}
		virtual void Execute(int key) const
		{
			Map.Scroll(true, -1);
		}
};

class SidebarDownCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "SidebarDown";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SIDEBAR_DOWN", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1CDD);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1CDF);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SIDEBAR_DOWN_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1CE1);
		}
		virtual void Execute(int key) const
		{
			Map.Scroll(false, -1);
		}
};

class OptionsCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "Options";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_OPTIONS", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1D6A);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1D6C);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_OPTIONS_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1D6E);
		}
		virtual void Execute(int key) const
		{
			Queue_Options();
		}
};

class View1CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "View1";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK1", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E1B);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E1D);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK1_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E1F);
		}
		virtual void Execute(int key) const
		{
			Coord coord(Scen->Views[0]);
			coord.Z = Map.Get_Height_GL(coord);
			TacticalMap->Set_Tactical_Position(coord);
			if (Map.PendingObject) {
				Map.Set_Cursor_Pos();
			}
		}
};

class View2CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "View2";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK2", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E3F);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E41);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK2_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E43);
		}
		virtual void Execute(int key) const
		{
			Coord coord(Scen->Views[1]);
			coord.Z = Map.Get_Height_GL(coord);
			TacticalMap->Set_Tactical_Position(coord);
			if (Map.PendingObject) {
				Map.Set_Cursor_Pos();
			}
		}
};

class View3CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "View3";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK3", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E63);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E65);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK3_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E67);
		}
		virtual void Execute(int key) const
		{
			Coord coord(Scen->Views[2]);
			coord.Z = Map.Get_Height_GL(coord);
			TacticalMap->Set_Tactical_Position(coord);
			if (Map.PendingObject) {
				Map.Set_Cursor_Pos();
			}
		}
};

class View4CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "View4";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK4", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E87);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E89);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_VIEW_BOOKMARK4_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1E8B);
		}
		virtual void Execute(int key) const
		{
			Coord coord(Scen->Views[3]);
			coord.Z = Map.Get_Height_GL(coord);
			TacticalMap->Set_Tactical_Position(coord);
			if (Map.PendingObject) {
				Map.Set_Cursor_Pos();
			}
		}
};

class SetView1CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "SetView1";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK1", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EAB);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EAD);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK1_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EAF);
		}
		virtual void Execute(int key) const
		{
			Point2D pixel = TacticalMap->Get_Relative_Tactical_Position();
			Scen->Views[0] = TacticalMap->Pixel_To_Cell(pixel);
		}
};

class SetView2CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "SetView2";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK2", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EC6);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EC8);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK2_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1ECA);
		}
		virtual void Execute(int key) const
		{
			Point2D pixel = TacticalMap->Get_Relative_Tactical_Position();
			Scen->Views[1] = TacticalMap->Pixel_To_Cell(pixel);
		}
};

class SetView3CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "SetView3";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK3", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EE1);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EE3);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK3_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EE5);
		}
		virtual void Execute(int key) const
		{
			Point2D pixel = TacticalMap->Get_Relative_Tactical_Position();
			Scen->Views[2] = TacticalMap->Pixel_To_Cell(pixel);
		}
};

class SetView4CommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "SetView4";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK4", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EFC);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1EFE);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SET_BOOKMARK4_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F00);
		}
		virtual void Execute(int key) const
		{
			Point2D pixel = TacticalMap->Get_Relative_Tactical_Position();
			Scen->Views[3] = TacticalMap->Pixel_To_Cell(pixel);
		}
};

class FollowCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "Follow";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_FOLLOW", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F17);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F19);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_FOLLOW_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F1B);
		}
		virtual void Execute(int key) const
		{
			if (CurrentObject.Count() != 0 && Map.Object_To_Follow() == NULL) {
				Map.Set_To_Follow(CurrentObject[0]);
			} else {
				Map.Break_Follow_Mode();
			}
		}
};

class MultiplayerDebugCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "MultiplayerDebug";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return L"Multiplayer debug info";
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return L"Debug";
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return L"Turns multiplayer debug on or off.";
		}
		virtual void Execute(int key) const
		{
			if (Session.EnableMultiplayerDebug) {
				DAT_00A8B8B5 = !DAT_00A8B8B5;
			}
		}
};

class MultiplayerSyncCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "ExtraSyncChecking";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return L"Game sync logging";
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return L"Debug";
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return L"Turns extra game sync logging on or off.";
		}
		virtual void Execute(int key) const
		{
			DAT_00B04880 = !DAT_00B04880;
			if (DAT_00B04880) {
				Session.Messages.Add_Message(NULL, 0, FUN_00735120("Extra sync info ON"), 3, TextPrintType(0x4046), 0x96, false);
			} else {
				Session.Messages.Add_Message(NULL, 0, FUN_00735120("Extra sync info OFF"), 3, TextPrintType(0x4046), 0x96, false);
			}
		}
};

class ScreenCaptureCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "ScreenCapture";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_SCRNCAP", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F88);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F87);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_SCRNCAP_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1F89);
		}
		virtual void Execute(int key) const
		{
			RECT win_rect;
			
			if (GetClientRect(MainWindow, &win_rect)) {
			
				POINT upper_left;
				upper_left.x = win_rect.left;
				upper_left.y = win_rect.top;
			
				if (!ClientToScreen(MainWindow, &upper_left)) {
					return;
				}
			
				POINT lower_right;
				lower_right.x = win_rect.right;
				lower_right.y = win_rect.bottom;
			
				if (!ClientToScreen(MainWindow, &lower_right)) {
					return;
				}
			
				Rect dest_rect(upper_left.x, upper_left.y, win_rect.right + 1, win_rect.bottom + 1);
				dest_rect.Width = MIN(dest_rect.Width, HiddenSurface->Get_Width());
				dest_rect.Height = MIN(dest_rect.Height, HiddenSurface->Get_Height());
			
				Hide_Mouse();
			
				HiddenSurface->Blit_From(Rect(0, 0, HiddenSurface->Get_Width(), HiddenSurface->Get_Height()),
					*VisibleSurface, dest_rect);
			
				Show_Mouse();
			
				char fname[128];
				int index = -1;
			
				do {
					index++;
					sprintf(fname, "SCRN%04d.pcx", index);
				} while (CCFileClass(fname).Is_Available());
			
				CCFileClass file(fname);
				Write_PCX_File(file, *HiddenSurface, &GamePalette);
			}
		}
};

class PageUserCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "PageUser";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("TXT_PAGEUSER", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1D8B);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1D8D);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("TXT_PAGEUSER_DESC", NULL, "D:\\ra2mdpost\\Init.CPP", 0x1D8F);
		}
		virtual void Execute(int key) const
		{
			if (Session.Type == 4) {
				Queue_PageUser();
			}
		}
};

class CursorPositionCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "CursorCheat";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("GUI:CursorCheat", NULL, "D:\\ra2mdpost\\Init.CPP", 0x2027);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("GUI:Debug", NULL, "D:\\ra2mdpost\\Init.CPP", 0x2028);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("DESC:CursorCheat", NULL, "D:\\ra2mdpost\\Init.CPP", 0x2029);
		}
		virtual void Execute(int key) const
		{
			DAT_00A8F7D8 = !DAT_00A8F7D8;
		}
};

class DeleteCommandClass : public CommandClass
{
	public:
		virtual char const * Get_Unique_Name(void) const
		{
			return "Delete";
		}
		virtual wchar_t const * Get_Display_Name(void) const
		{
			return StringTable::LoadString("Cmnd:Delete", NULL, "D:\\ra2mdpost\\Init.CPP", 0x199F);
		}
		virtual wchar_t const * Get_Category(void) const
		{
			return StringTable::LoadString("TXT_INTERFACE", NULL, "D:\\ra2mdpost\\Init.CPP", 0x19A1);
		}
		virtual wchar_t const * Get_Description(void) const
		{
			return StringTable::LoadString("Cmnd:DeleteDesc", NULL, "D:\\ra2mdpost\\Init.CPP", 0x19A3);
		}
		virtual void Execute(int key) const
		{
			FUN_00731A10();
		}
};

// FUNCTION: 0x00532150
void Init_Commands(void)
{
	if (Session.EnableMultiplayerDebug) {
		AllCommands.Add(new MultiplayerDebugCommandClass);
		AllCommands.Add(new MultiplayerSyncCommandClass);
	}

	AllCommands.Add(new FollowCommandClass);

	AllCommands.Add(new View1CommandClass);
	AllCommands.Add(new View2CommandClass);
	AllCommands.Add(new View3CommandClass);
	AllCommands.Add(new View4CommandClass);

	AllCommands.Add(new SetView1CommandClass);
	AllCommands.Add(new SetView2CommandClass);
	AllCommands.Add(new SetView3CommandClass);
	AllCommands.Add(new SetView4CommandClass);

	const CommandClass * optcmd = new OptionsCommandClass;
	AllCommands.Add(optcmd);

	AllCommands.Add(new SidebarUpCommandClass);
	AllCommands.Add(new SidebarDownCommandClass);

	const CommandClass * reventcmd = new CenterREventCommandClass;
	AllCommands.Add(reventcmd);

	AllCommands.Add(new BeaconPlacementCommandClass);
	AllCommands.Add(new ToggleSellCommandClass);
	AllCommands.Add(new ToggleRepairCommandClass);
	AllCommands.Add(new AllianceCommandClass);
	AllCommands.Add(new CenterBaseCommandClass);
	AllCommands.Add(new CenterViewCommandClass);
	AllCommands.Add(new ScatterCommandClass);
	AllCommands.Add(new GuardCommandClass);
	AllCommands.Add(new StopCommandClass);
	AllCommands.Add(new AllToCheerCommandClass);
	AllCommands.Add(new DeployCommandClass);
	AllCommands.Add(new PrevObjectCommandClass);
	AllCommands.Add(new NextObjectCommandClass);
	AllCommands.Add(new PlanningModeCommandClass);
	AllCommands.Add(new CombatantSelectCommandClass);
	AllCommands.Add(new TypeSelectCommandClass);
	AllCommands.Add(new HealthNavCommandClass);
	AllCommands.Add(new VeterancyNavCommandClass);
	AllCommands.Add(new SetStructureTabCommandClass);
	AllCommands.Add(new SetDefenseTabCommandClass);
	AllCommands.Add(new SetUnitTabCommandClass);
	AllCommands.Add(new SetInfantryTabCommandClass);

	AllCommands.Add(new CreateTeamCommandClass(1));
	AllCommands.Add(new CreateTeamCommandClass(2));
	AllCommands.Add(new CreateTeamCommandClass(3));
	AllCommands.Add(new CreateTeamCommandClass(4));
	AllCommands.Add(new CreateTeamCommandClass(5));
	AllCommands.Add(new CreateTeamCommandClass(6));
	AllCommands.Add(new CreateTeamCommandClass(7));
	AllCommands.Add(new CreateTeamCommandClass(8));
	AllCommands.Add(new CreateTeamCommandClass(9));
	AllCommands.Add(new CreateTeamCommandClass(10));

	AllCommands.Add(new SelectTeamCommandClass(1));
	AllCommands.Add(new SelectTeamCommandClass(2));
	AllCommands.Add(new SelectTeamCommandClass(3));
	AllCommands.Add(new SelectTeamCommandClass(4));
	AllCommands.Add(new SelectTeamCommandClass(5));
	AllCommands.Add(new SelectTeamCommandClass(6));
	AllCommands.Add(new SelectTeamCommandClass(7));
	AllCommands.Add(new SelectTeamCommandClass(8));
	AllCommands.Add(new SelectTeamCommandClass(9));
	AllCommands.Add(new SelectTeamCommandClass(10));

	AllCommands.Add(new AddTeamCommandClass(1));
	AllCommands.Add(new AddTeamCommandClass(2));
	AllCommands.Add(new AddTeamCommandClass(3));
	AllCommands.Add(new AddTeamCommandClass(4));
	AllCommands.Add(new AddTeamCommandClass(5));
	AllCommands.Add(new AddTeamCommandClass(6));
	AllCommands.Add(new AddTeamCommandClass(7));
	AllCommands.Add(new AddTeamCommandClass(8));
	AllCommands.Add(new AddTeamCommandClass(9));
	AllCommands.Add(new AddTeamCommandClass(10));

	AllCommands.Add(new CenterTeamCommandClass(1));
	AllCommands.Add(new CenterTeamCommandClass(2));
	AllCommands.Add(new CenterTeamCommandClass(3));
	AllCommands.Add(new CenterTeamCommandClass(4));
	AllCommands.Add(new CenterTeamCommandClass(5));
	AllCommands.Add(new CenterTeamCommandClass(6));
	AllCommands.Add(new CenterTeamCommandClass(7));
	AllCommands.Add(new CenterTeamCommandClass(8));
	AllCommands.Add(new CenterTeamCommandClass(9));
	AllCommands.Add(new CenterTeamCommandClass(10));

	AllCommands.Add(new TauntCommandClass(1));
	AllCommands.Add(new TauntCommandClass(2));
	AllCommands.Add(new TauntCommandClass(3));
	AllCommands.Add(new TauntCommandClass(4));
	AllCommands.Add(new TauntCommandClass(5));
	AllCommands.Add(new TauntCommandClass(6));
	AllCommands.Add(new TauntCommandClass(7));
	AllCommands.Add(new TauntCommandClass(8));

	AllCommands.Add(new ScreenCaptureCommandClass);
	AllCommands.Add(new PageUserCommandClass);
	AllCommands.Add(new CursorPositionCommandClass);

	const CommandClass * delcmd = new DeleteCommandClass;
	AllCommands.Add(delcmd);

	Init_Hotkeys();

	if (HotkeyCommands.Is_Present(KN_DELETE)) {
		HotkeyCommands.Remove_Index(KN_DELETE);
	}
	HotkeyCommands.Add_Index(KN_DELETE, delcmd);

	if (HotkeyCommands.Is_Present(KN_ESC)) {
		HotkeyCommands.Remove_Index(KN_ESC);
	}
	HotkeyCommands.Add_Index(KN_ESC, optcmd);

	if (HotkeyCommands.Is_Present(KN_SPACE)) {
		HotkeyCommands.Remove_Index(KN_SPACE);
	}
	HotkeyCommands.Add_Index(KN_SPACE, reventcmd);
}

// FUNCTION: 0x00533D20
bool Init_Hotkeys(void)
{
	CCINIClass ini;
	CCFileClass file("KEYBOARDMD.INI");

	if (ini.Load(file, false)) {

		HotkeyCommands.Clear();

		for (int index = 0; index < ini.Entry_Count("Hotkey"); index++) {
			const char *entry = ini.Get_Entry("Hotkey", index);
			int id = ini.Get_Int("Hotkey", entry, 0);
			const CommandClass *command = NULL;

			for (int cindex = 0; cindex < AllCommands.Count(); cindex++) {
				if (strcmp(AllCommands[cindex]->Get_Unique_Name(), entry) == 0) {
					command = AllCommands[cindex];
					break;
				}
			}

			if ((command != NULL) && (id != 0)) {
				HotkeyCommands.Add_Index(id, command);
			}
		}

		return(true);
	}

	DebugString("Unable to load KEYBOARDMD.INI\n");
	return(false);
}

// FUNCTION: 0x00533F50
void Execute_Command(char const * name)
{
	for (int command = 0; command < AllCommands.Count(); command++) {
		if (strcmp(AllCommands[command]->Get_Unique_Name(), name) == 0) {
			AllCommands[command]->Execute(0);
			break;
		}
	}
}

void __fastcall FUN_0069E210(void);
void __fastcall FUN_004C29E0(void);
void __fastcall FUN_00550000(void);
void __fastcall FUN_00565B00(void * map);
void __fastcall FUN_00430980(void * beacon);
int __cdecl FUN_0041C610(const void * a, const void * b);
extern int DAT_00A8E7AC;
extern DynamicVectorClass<AbstractClass *> DAT_00B0F698;
struct TargetEntryStruct {
	int ID;
	AbstractClass * Data;
};
extern TargetEntryStruct * DAT_00B0E840;
extern int DAT_00B0E844;
extern bool DAT_00B0E84C;
extern int DAT_00B0E850;
extern char DAT_0089C3B0[];
void __fastcall FUN_00725C70(void);
void __fastcall FUN_00556DF0(void);
class SpotlightClass {
	public:
		~SpotlightClass(void);
};
extern DynamicVectorClass<AbstractClass *> DAT_00A8ED40;
extern DynamicVectorClass<AbstractClass *> DAT_00A8E360;
extern DynamicVectorClass<AbstractClass *> DAT_00B0E720;
extern DynamicVectorClass<AbstractClass *> DAT_00A8EAE8;
extern DynamicVectorClass<AbstractClass *> DAT_008B4138;
extern DynamicVectorClass<AbstractClass *> DAT_008B4190;
extern DynamicVectorClass<AbstractClass *> DAT_00A8EC50;
extern DynamicVectorClass<AbstractClass *> DAT_00A80208;
extern DynamicVectorClass<AbstractClass *> DAT_00A8EC38;
extern DynamicVectorClass<AbstractClass *> DAT_00A83E30;
extern DynamicVectorClass<AbstractClass *> DAT_008B4120;
extern DynamicVectorClass<AbstractClass *> DAT_008B40E8;
extern DynamicVectorClass<AbstractClass *> DAT_00A80228;
extern DynamicVectorClass<AbstractClass *> DAT_00A8E9A8;
extern DynamicVectorClass<AbstractClass *> DAT_00B04BD0;
extern DynamicVectorClass<AbstractClass *> DAT_00ABCA10;
extern DynamicVectorClass<AbstractClass *> DAT_008A3870;
extern DynamicVectorClass<AbstractClass *> DAT_0089E0F0;
extern DynamicVectorClass<AbstractClass *> DAT_008A0208;
extern DynamicVectorClass<AbstractClass *> DAT_00AC4910;
extern DynamicVectorClass<AbstractClass *> DAT_00B0EC60;
extern DynamicVectorClass<AbstractClass *> DAT_00889FB8;
extern DynamicVectorClass<AbstractClass *> DAT_00B0B880;
extern DynamicVectorClass<AbstractClass *> DAT_0089C668;
extern DynamicVectorClass<AbstractClass *> DAT_008B3D10;
extern DynamicVectorClass<AbstractClass *> DAT_0088A0F0;
extern DynamicVectorClass<AbstractClass *> DAT_00A8E988;
extern DynamicVectorClass<AbstractClass *> DAT_00A8E968;
extern DynamicVectorClass<SpotlightClass *> DAT_00AC1678;

// FUNCTION: 0x00534450
void Delete_All_Objects(void)
{
	DAT_00A8E7AC++;
	DAT_00B0F698.Reset_Active();
	DAT_00B0F698.Clear();
	FUN_0069E210();
	while (DAT_00B0E844) {
		if (!DAT_00B0E84C) {
			qsort(DAT_00B0E840, DAT_00B0E844, 8, FUN_0041C610);
			DAT_00B0E850 = 0;
			DAT_00B0E84C = true;
		}
		delete DAT_00B0E840[0].Data;
	}
	FUN_00725C70();
	while (DAT_00A8ED40.Count()) {
		DAT_00A8ED40[0]->Release();
	}
	FUN_00725C70();
	while (DAT_00A8E360.Count()) {
		delete DAT_00A8E360[0];
	}
	FUN_00725C70();
	while (DAT_00B0E720.Count()) {
		delete DAT_00B0E720[0];
	}
	FUN_00725C70();
	while (DAT_00A8EAE8.Count()) {
		delete DAT_00A8EAE8[0];
	}
	FUN_00725C70();
	while (DAT_008B4138.Count()) {
		delete DAT_008B4138[0];
	}
	FUN_00725C70();
	while (DAT_008B4190.Count()) {
		delete DAT_008B4190[0];
	}
	FUN_00725C70();
	while (DAT_00A8EC50.Count()) {
		delete DAT_00A8EC50[0];
	}
	FUN_00725C70();
	while (DAT_00A80208.Count()) {
		delete DAT_00A80208[0];
	}
	FUN_00725C70();
	while (DAT_00A8EC38.Count()) {
		delete DAT_00A8EC38[0];
	}
	FUN_00725C70();
	while (DAT_00A83E30.Count()) {
		delete DAT_00A83E30[0];
	}
	FUN_00725C70();
	while (DAT_008B4120.Count()) {
		delete DAT_008B4120[0];
	}
	FUN_00725C70();
	while (DAT_008B40E8.Count()) {
		delete DAT_008B40E8[0];
	}
	FUN_00725C70();
	while (DAT_00A80228.Count()) {
		delete DAT_00A80228[0];
	}
	FUN_00725C70();
	while (DAT_00A8E9A8.Count()) {
		delete DAT_00A8E9A8[0];
	}
	FUN_00725C70();
	while (Scripts.Count()) {
		delete Scripts[0];
	}
	FUN_00725C70();
	while (DAT_00B04BD0.Count()) {
		delete DAT_00B04BD0[0];
	}
	FUN_00725C70();
	while (DAT_00ABCA10.Count()) {
		delete DAT_00ABCA10[0];
	}
	FUN_00725C70();
	while (DAT_008A3870.Count()) {
		delete DAT_008A3870[0];
	}
	FUN_00725C70();
	while (DAT_0089E0F0.Count()) {
		delete DAT_0089E0F0[0];
	}
	FUN_00725C70();
	while (DAT_008A0208.Count()) {
		delete DAT_008A0208[0];
	}
	FUN_00725C70();
	while (DAT_00AC4910.Count()) {
		delete DAT_00AC4910[0];
	}
	FUN_00725C70();
	while (DAT_00B0EC60.Count()) {
		delete DAT_00B0EC60[0];
	}
	FUN_00725C70();
	while (DAT_00889FB8.Count()) {
		delete DAT_00889FB8[0];
	}
	FUN_00725C70();
	while (DAT_00B0B880.Count()) {
		delete DAT_00B0B880[0];
	}
	FUN_00725C70();
	while (DAT_0089C668.Count()) {
		delete DAT_0089C668[0];
	}
	FUN_00725C70();
	while (DAT_00AC1678.Count()) {
		delete DAT_00AC1678[0];
	}
	FUN_00725C70();
	while (DAT_008B3D10.Count()) {
		delete DAT_008B3D10[0];
	}
	FUN_00725C70();
	while (DAT_0088A0F0.Count()) {
		delete DAT_0088A0F0[0];
	}
	FUN_00725C70();
	while (DAT_00A8E988.Count()) {
		delete DAT_00A8E988[0];
	}
	FUN_00725C70();
	FUN_004C29E0();
	FUN_00556DF0();
	FUN_00550000();
	while (DAT_00A8E968.Count()) {
		delete DAT_00A8E968[0];
	}
	FUN_00725C70();
	delete TacticalMap;
	TacticalMap = NULL;
	FUN_00565B00(&Map);
	FUN_00725C70();
	FUN_00430980(&DAT_0089C3B0);
	DAT_00A8E7AC--;
}

extern MFCD * DAT_00884E0C;
extern MFCD * DAT_00884E08;
extern MFCD * DAT_00884E10;
extern MFCD * DAT_00884E20;
extern MFCD * DAT_00884E1C;
extern PaletteClass DAT_00885A80;
extern PaletteClass DAT_00886380;
void const * __fastcall FUN_005B40B0(char const * name, bool flag);
void __fastcall FUN_006267A0(TheaterType theater);
void __fastcall FUN_00717840(void);

// FUNCTION: 0x005349C0
void __fastcall Init_Theater(TheaterType theater)
{
	struct {
		char fullname[16];
		char rootmd[16];
		char shortname[16];
		char palmd[16];
		char isofullname[16];
	} n;

	wsprintf(n.fullname, "%s.MIX", Theaters[theater].Root);
	wsprintf(n.isofullname, "%s.MIX", Theaters[theater].IsoRoot);
	wsprintf(n.shortname, "%s.MIX", Theaters[theater].Suffix);
	wsprintf(n.palmd, "%sMD.MIX", Theaters[theater].PaletteFileName);
	wsprintf(n.rootmd, "%sMD.MIX", Theaters[theater].Root);

	FUN_004068E0("Init theater %s\n", Theaters[theater].Name);

	Scen->Theater = theater;
	Session.Update_Progress(8);

	if (Scen->Theater != LastTheater) {
		if (DAT_00884E0C != NULL) {
			delete DAT_00884E0C;
		}
		DAT_00884E0C = NULL;
		if (Scen->Theater == THEATER_SNOW) {
			DAT_00884E0C = new MFCD(n.rootmd, &FastKey);
			DAT_00884E0C->Cache((Buffer const *)NULL);
		}

		if (DAT_00884E08 != NULL) {
			delete DAT_00884E08;
		}
		DAT_00884E08 = new MFCD(n.fullname, &FastKey);

		if (DAT_00884E10 != NULL) {
			delete DAT_00884E10;
		}
		DAT_00884E10 = new MFCD(n.shortname, &FastKey);
		DAT_00884E10->Cache((Buffer const *)NULL);

		DAT_00884E08->Cache((Buffer const *)NULL);
		Session.Update_Progress(6);

		if (DAT_00884E20 != NULL) {
			delete DAT_00884E20;
		}
		DAT_00884E20 = new MFCD(n.palmd, &FastKey);

		if (DAT_00884E1C != NULL) {
			delete DAT_00884E1C;
		}
		DAT_00884E1C = new MFCD(n.isofullname, &FastKey);

		Session.Update_Progress(12);

		wsprintf(n.fullname, "%s.PAL", Theaters[theater].Root);

		unsigned char * ptr = (unsigned char *)FUN_005B40B0(n.fullname, false);
		if (ptr != NULL) {
			for (int color = 0; color < 256; color++) {
				unsigned char r = (unsigned char)((*ptr++)<<2);
				unsigned char g = (unsigned char)((*ptr++)<<2);
				unsigned char b = (unsigned char)((*ptr++)<<2);
				GamePalette[color] = RGBClass(r, g, b);
			}
		} else {
			for (int color = 0; color < 256; color++) {
				GamePalette[color] = RGBClass(color, 255 - color, ((color << 2) & 0xff));
			}
		}

		DAT_00885A80 = GamePalette;

		wsprintf(n.fullname, "UNIT%s.PAL", Theaters[theater].Suffix);
		PaletteClass * unitpal = (PaletteClass *)FUN_005B40B0(n.fullname, false);

		Game::CallBack();

		if (unitpal != NULL) {
			DAT_00886380 = *unitpal;
		}

		for (int index = 0; index < 256; index++) {
			DAT_00886380[index] = RGBClass(
				(unsigned char)(DAT_00886380[index].Get_Red()<<2),
				(unsigned char)(DAT_00886380[index].Get_Green()<<2),
				(unsigned char)(DAT_00886380[index].Get_Blue()<<2));
		}

		int last_percent = 12;
		int prog_step = ColorSchemes.Count() / (25 - 12);
		for (int s = 0; s < ColorSchemes.Count(); s++) {
			ColorSchemes[s]->Build_Light_Converters(DAT_00886380, GamePalette);
			int percent = (min(12 + (s / prog_step), 25));
			if (percent != last_percent) {
				Session.Update_Progress(percent);
				last_percent = percent;
			}
			Game::CallBack();
		}

		FUN_006267A0(theater);
		FUN_00717840();
		Session.Update_Progress(25);
	}
}

extern MFCD * DAT_00884E5C;
extern MFCD * DAT_00884E58;

// FUNCTION: 0x00534DE0
bool FUN_00534DE0(void)
{
	if (DAT_00884E5C != NULL) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E5C->Filename);
		delete DAT_00884E5C;
		DAT_00884E5C = NULL;
	}
	if (DAT_00884E58 != NULL) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E58->Filename);
		delete DAT_00884E58;
		DAT_00884E58 = NULL;
	}
	return(true);
}

extern MFCD * DAT_00884E68;
extern MFCD * DAT_00884E70;
extern MFCD * DAT_00884E74;
extern MFCD * DAT_00884E78;
extern int DAT_0089E414;
extern CCINIClass DAT_00887208;
void FUN_007534E0(void);
void FUN_0072F350(void);
void __fastcall FUN_0072F440(int side);
void __fastcall FUN_006D02B0(void * map);
int __fastcall FUN_004A80D0(int drive, int arg);

// FUNCTION: 0x00534FA0
bool __fastcall FUN_00534FA0(int side)
{
	char name[64];

	FUN_007534E0();
	if (side == 2) {
		side = 1;
	}
	int disk = side;
	if (side == -1) {
		int cd = FUN_004A80D0(DAT_0089E414, 0x3C);
		if (cd != 0 && cd != 1) {
			FUN_004068E0("     FAILED!  This was not disk one or two!!!\n");
			return(false);
		}
		disk = cd;
	}
	FUN_004068E0("Preparing Mixfiles for Side %02d.\n", disk);

	if (DAT_00884E68 != NULL) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E68->Filename);
		delete DAT_00884E68;
		DAT_00884E68 = NULL;
	}
	if (DAT_00884E74 != NULL) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E74->Filename);
		delete DAT_00884E74;
		DAT_00884E74 = NULL;
	}
	if (DAT_00884E70 != NULL) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E70->Filename);
		delete DAT_00884E70;
		DAT_00884E70 = NULL;
	}
	if (DAT_00884E78 != NULL) {
		FUN_004068E0("     Releasing %s\n", DAT_00884E78->Filename);
		delete DAT_00884E78;
		DAT_00884E78 = NULL;
	}

	sprintf(name, "SIDEC%02dMD.MIX", disk + 1);
	FUN_004068E0("     Initilizing %s\n", name);
	if (CCFileClass(name).Is_Available()) {
		DAT_00884E70 = new MFCD(name, &FastKey);
	}
	DAT_00884E70->Cache();

	sprintf(name, "SIDEC%02d.MIX", disk + 1);
	FUN_004068E0("     Initilizing %s\n", name);
	if (CCFileClass(name).Is_Available()) {
		DAT_00884E74 = new MFCD(name, &FastKey);
		if (DAT_00884E74 == NULL) {
			FUN_004068E0("     FAILED!\n");
			return(false);
		}
	}
	if (DAT_00884E74 == NULL) {
		FUN_004068E0("     FAILED!\n");
		return(false);
	}
	DAT_00884E74->Cache();

	sprintf(name, "SIDENC%02d.MIX", disk + 1);
	FUN_004068E0("     Initilizing %s\n", name);
	if (CCFileClass(name).Is_Available()) {
		DAT_00884E78 = new MFCD(name, &FastKey);
	}

	FUN_0072F350();
	FUN_0072F440(disk);

	CCFileClass uimd("UIMD.INI");
	if (DAT_00887208.ReadCCFile(uimd, false, false)) {
		Rule->Read_AdvancedCommandBar(DAT_00887208, Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH);
		FUN_006D02B0(&Map);
		return(true);
	}
	FUN_004068E0("Failed to load UIMD.INI!\n");
	return(false);
}

// FUNCTION: 0x005353D0
ObjectClass * __fastcall FUN_005353D0(Cell const * cell, ObjectClass const * target)
{
	ObjectClass * best_obj = NULL;
	int best_value = -1;
	int best_distance = 0x7FFFFFFF;

	if (CurrentObject.Count() > 0) {
		for (int index = 0; index < CurrentObject.Count(); index++) {
			ObjectClass * obj = CurrentObject[index];
			int value = 0;

			if (obj->Is_Techno()) {
				value = 2;
				TechnoClass * tech = (TechnoClass *)obj;
				if (tech->Berzerk) {
					value = 0;
				} else if (tech->IsUnderEMP()) {
					value = 1;
				} else if (tech->Is_Weapon_Equipped()) {
					value = 3;
					if (tech->RTTI != RTTI_BUILDING) {
						value = 4;
						if (tech->Combat_Damage() > 0) {
							value = 5;
						}
					}
				}
			}

			if (value >= best_value) {
				int distance = 0x7FFFFFFF;
				bool has_cell = cell != NULL;
				if (has_cell || target != NULL) {
					Coord here = obj->Center_Coord();
					Coord there;
					if (has_cell) {
						there = cell->As_Coord(0);
					} else {
						there = target->Center_Coord();
					}
					distance = here.Distance_To(there);
				}
				if (value > best_value) {
					best_value = value;
					best_obj = obj;
					best_distance = distance;
				} else if (distance < best_distance) {
					best_obj = obj;
					best_distance = distance;
				}
			}
		}
	}
	return(best_obj);
}

// FUNCTION: 0x005355C0
ThemeType FUN_005355C0(void)
{
	return(Theme.From_Name("INTRO"));
}

extern char DAT_00A8F394[];
extern char DAT_00A8F314[];
extern char DAT_00A8F4A0[];
extern char DAT_00A8F420[];
extern char DAT_00A8F520[];
extern IDirectDraw * DAT_008A0094;
int DAT_00825C8C = -1;
int FUN_004E6190(void);
int FUN_005CAFC0(void);
char const * FUN_004E62B0(void);

// FUNCTION: 0x005355D0
int FUN_005355D0(void)
{
	if (DAT_00825C8C == -1) {
		DAT_00A8F394[0] = 0;
		DAT_00A8F314[0] = 0;
		DAT_00A8F4A0[0] = 0;
		DAT_00A8F420[0] = 0;
		DAT_00A8F520[0] = 0;

		int cpu_type = FUN_004E6190();
		int mhz = FUN_005CAFC0();
		int cpu_rating;
		int ram_rating;
		int vram_rating;
		cpu_rating = 5;
		switch (cpu_type) {
			case -1:
				break;
			case 3:
			case 5:
				cpu_rating = mhz / 100 + 1;
				break;
			case 4:
			case 7:
				cpu_rating = mhz / 100 + 2;
				break;
			case 6:
				cpu_rating = mhz / 100 + 3;
				break;
			default:
				cpu_rating = 1;
				break;
		}
		FUN_004068E0("Profile: CPU:%d (%dMhz %s)\n", cpu_rating, mhz, FUN_004E62B0());
		sprintf(DAT_00A8F314, "CPU: %d Mhz %s", mhz, FUN_004E62B0());

		MEMORYSTATUS memory;
		memory.dwLength = sizeof(memory);
		GlobalMemoryStatus(&memory);
		unsigned int ram = (memory.dwTotalPhys + 0xA0000) >> 20;
		ram_rating = 2;
		if (ram >= 64) {
			ram_rating = 5;
		}
		if (ram >= 128) {
			ram_rating += 2;
		}
		if (ram == 256) {
			ram_rating += 2;
		} else if (ram > 256) {
			ram_rating = 10;
		}
		FUN_004068E0("Profile: RAM:%d (%dMb)\n", ram_rating, ram);
		sprintf(DAT_00A8F4A0, "RAM reported: %d Mb", ram);

		unsigned int vram = 4;
		vram_rating = 5;
		if (DAT_008A0094 != NULL) {
			DDCAPS_DX3 caps;
			caps.dwSize = 0x13C;
			if (DAT_008A0094->GetCaps((LPDDCAPS)&caps, NULL) == 0) {
				DWORD total = caps.dwVidMemTotal;
				if (total <= caps.dwVidMemFree) {
					total = caps.dwVidMemFree;
				}
				vram = (total + 0xFFFFF) >> 20;
			}
		}
		if (vram >= 32) {
			vram_rating = 10;
		} else if (vram >= 16) {
			vram_rating = 9;
		} else if (vram >= 8) {
			vram_rating = 7;
		} else if (vram >= 4) {
			vram_rating = 5;
		} else if (vram <= 2) {
			vram_rating = 3;
		}
		FUN_004068E0("Profile: VRAM:%d (%dMb)\n", vram_rating, vram);
		sprintf(DAT_00A8F420, "VRAM reported: %d Mb", vram);

		int blits = 0;
		int speed_rating = 5;
		if (TileSurface != NULL && ((DSurface *)TileSurface)->In_Video_Ram() && !((DSurface *)AlternateSurface)->In_Video_Ram()) {
			Rect area = TileSurface->Get_Rect();
			Rect other = AlternateSurface->Get_Rect();
			if (area.Width >= other.Width) {
				area.Width = other.Width;
			}
			if (area.Height >= other.Height) {
				area.Height = other.Height;
			}
			if (area.Width >= 480) {
				area.Width = 480;
			}
			if (area.Height >= 400) {
				area.Height = 400;
			}
			for (int pass = 0; pass < 4; pass++) {
				TileSurface->Blit_From(area, *AlternateSurface, area);
				AlternateSurface->Blit_From(area, *TileSurface, area);
				Sleep(0);
			}
			DWORD start = timeGetTime();
			if ((start & 0xFF000000) != 0xFF) {
				TileSurface->Blit_From(area, *AlternateSurface, area);
				AlternateSurface->Blit_From(area, *TileSurface, area);
				blits = 1;
				while (timeGetTime() - start <= 1000 && blits <= 5000) {
					TileSurface->Blit_From(area, *AlternateSurface, area);
					AlternateSurface->Blit_From(area, *TileSurface, area);
					blits++;
				}
				speed_rating = MAX(1, blits / 4);
			}
		}
		FUN_004068E0("Profile: VRAM speed:%d (%d blits per second)\n", speed_rating, blits * 2);
		sprintf(DAT_00A8F520, "%d blits per second\n", blits * 2);

		DAT_00825C8C = cpu_rating;
		if (DAT_00825C8C >= ram_rating) {
			DAT_00825C8C = ram_rating;
		}
		if (DAT_00825C8C >= vram_rating) {
			DAT_00825C8C = vram_rating;
		}
		if (DAT_00825C8C >= speed_rating) {
			DAT_00825C8C = speed_rating;
		}
		FUN_004068E0("Overall performance profile = %d\n", DAT_00825C8C);
		sprintf(DAT_00A8F394, "Profile: %d\n", DAT_00825C8C);
	}
	return(DAT_00825C8C);
}

// FUNCTION: 0x00535AA0 INIClass::INIClass

// FUNCTION: 0x00535BD0 CommandClass::Unknown14
// FUNCTION: 0x00535BE0 CommandClass::Unknown18
// FUNCTION: 0x00535BF0 CommandClass::Unknown1C
// FUNCTION: 0x00535C00 CreateTeamCommandClass::CreateTeamCommandClass
// FUNCTION: 0x00535C20 CreateTeamCommandClass::Get_Unique_Name
// FUNCTION: 0x00535C40 CreateTeamCommandClass::Get_Display_Name
// FUNCTION: 0x00535C70 CreateTeamCommandClass::Get_Category
// FUNCTION: 0x00535C90 CreateTeamCommandClass::Get_Description
// FUNCTION: 0x00535CC0 CreateTeamCommandClass::Execute
// FUNCTION: 0x00535CD0 CreateTeamCommandClass::~CreateTeamCommandClass
// FUNCTION: 0x00535CF0 CommandClass::Get_Display_Name
// FUNCTION: 0x00535D00 CommandClass::~CommandClass
// FUNCTION: 0x00535D20 SetStructureTabCommandClass::Get_Unique_Name
// FUNCTION: 0x00535D30 SetStructureTabCommandClass::Get_Display_Name
// FUNCTION: 0x00535D50 SetStructureTabCommandClass::Get_Category
// FUNCTION: 0x00535D70 SetStructureTabCommandClass::Get_Description
// FUNCTION: 0x00535D90 SetStructureTabCommandClass::Execute
// FUNCTION: 0x00535DE0 SetDefenseTabCommandClass::Get_Unique_Name
// FUNCTION: 0x00535DF0 SetDefenseTabCommandClass::Get_Display_Name
// FUNCTION: 0x00535E10 SetDefenseTabCommandClass::Get_Category
// FUNCTION: 0x00535E30 SetDefenseTabCommandClass::Get_Description
// FUNCTION: 0x00535E50 SetDefenseTabCommandClass::Execute
// FUNCTION: 0x00535EA0 SetInfantryTabCommandClass::Get_Unique_Name
// FUNCTION: 0x00535EB0 SetInfantryTabCommandClass::Get_Display_Name
// FUNCTION: 0x00535ED0 SetInfantryTabCommandClass::Get_Category
// FUNCTION: 0x00535EF0 SetInfantryTabCommandClass::Get_Description
// FUNCTION: 0x00535F10 SetInfantryTabCommandClass::Execute
// FUNCTION: 0x00535F30 SetUnitTabCommandClass::Get_Unique_Name
// FUNCTION: 0x00535F40 SetUnitTabCommandClass::Get_Display_Name
// FUNCTION: 0x00535F60 SetUnitTabCommandClass::Get_Category
// FUNCTION: 0x00535F80 SetUnitTabCommandClass::Get_Description
// FUNCTION: 0x00535FA0 SetUnitTabCommandClass::Execute
// FUNCTION: 0x00535FC0 SelectTeamCommandClass::SelectTeamCommandClass
// FUNCTION: 0x00535FE0 SelectTeamCommandClass::Get_Unique_Name
// FUNCTION: 0x00536000 SelectTeamCommandClass::Get_Display_Name
// FUNCTION: 0x00536030 SelectTeamCommandClass::Get_Category
// FUNCTION: 0x00536050 SelectTeamCommandClass::Get_Description
// FUNCTION: 0x00536080 SelectTeamCommandClass::Execute
// FUNCTION: 0x00536090 SelectTeamCommandClass::~SelectTeamCommandClass
// FUNCTION: 0x005360B0 AddTeamCommandClass::AddTeamCommandClass
// FUNCTION: 0x005360D0 AddTeamCommandClass::Get_Unique_Name
// FUNCTION: 0x005360F0 AddTeamCommandClass::Get_Display_Name
// FUNCTION: 0x00536120 AddTeamCommandClass::Get_Category
// FUNCTION: 0x00536140 AddTeamCommandClass::Get_Description
// FUNCTION: 0x00536170 AddTeamCommandClass::Execute
// FUNCTION: 0x00536180 AddTeamCommandClass::~AddTeamCommandClass
// FUNCTION: 0x005361A0 CenterTeamCommandClass::CenterTeamCommandClass
// FUNCTION: 0x005361C0 CenterTeamCommandClass::Get_Unique_Name
// FUNCTION: 0x005361E0 CenterTeamCommandClass::Get_Display_Name
// FUNCTION: 0x00536210 CenterTeamCommandClass::Get_Category
// FUNCTION: 0x00536230 CenterTeamCommandClass::Get_Description
// FUNCTION: 0x00536260 CenterTeamCommandClass::Execute
// FUNCTION: 0x00536270 CenterTeamCommandClass::~CenterTeamCommandClass
// FUNCTION: 0x00536290 TauntCommandClass::TauntCommandClass
// FUNCTION: 0x005362B0 TauntCommandClass::Get_Unique_Name
// FUNCTION: 0x005362D0 TauntCommandClass::Get_Display_Name
// FUNCTION: 0x00536300 TauntCommandClass::Get_Category
// FUNCTION: 0x00536320 TauntCommandClass::Get_Description
// FUNCTION: 0x00536580 TauntCommandClass::~TauntCommandClass
// FUNCTION: 0x005365A0 NextObjectCommandClass::Get_Unique_Name
// FUNCTION: 0x005365B0 NextObjectCommandClass::Get_Display_Name
// FUNCTION: 0x005365D0 NextObjectCommandClass::Get_Category
// FUNCTION: 0x005365F0 NextObjectCommandClass::Get_Description
// FUNCTION: 0x00536610 NextObjectCommandClass::Execute
// FUNCTION: 0x005366A0 PlanningModeCommandClass::Get_Unique_Name
// FUNCTION: 0x005366B0 PlanningModeCommandClass::Get_Display_Name
// FUNCTION: 0x005366D0 PlanningModeCommandClass::Get_Category
// FUNCTION: 0x005366F0 PlanningModeCommandClass::Get_Description
// FUNCTION: 0x00536710 PlanningModeCommandClass::Unknown14
// FUNCTION: 0x00536730 PlanningModeCommandClass::Unknown18
// FUNCTION: 0x00536740 PlanningModeCommandClass::Unknown1C
// FUNCTION: 0x00536750 PlanningModeCommandClass::Execute
// FUNCTION: 0x00536770 CombatantSelectCommandClass::Get_Unique_Name
// FUNCTION: 0x00536780 CombatantSelectCommandClass::Get_Display_Name
// FUNCTION: 0x005367A0 CombatantSelectCommandClass::Get_Category
// FUNCTION: 0x005367C0 CombatantSelectCommandClass::Get_Description
// FUNCTION: 0x005367E0 CombatantSelectCommandClass::Unknown14
// FUNCTION: 0x005367F0 CombatantSelectCommandClass::Execute
// FUNCTION: 0x00536810 TypeSelectCommandClass::Get_Unique_Name
// FUNCTION: 0x00536820 TypeSelectCommandClass::Get_Display_Name
// FUNCTION: 0x00536840 TypeSelectCommandClass::Get_Category
// FUNCTION: 0x00536860 TypeSelectCommandClass::Get_Description
// FUNCTION: 0x00536880 TypeSelectCommandClass::Unknown14
// FUNCTION: 0x00536890 TypeSelectCommandClass::Unknown18
// FUNCTION: 0x005368A0 TypeSelectCommandClass::Unknown1C
// FUNCTION: 0x005368B0 TypeSelectCommandClass::Execute
// FUNCTION: 0x005368D0 HealthNavCommandClass::Get_Unique_Name
// FUNCTION: 0x005368E0 HealthNavCommandClass::Get_Display_Name
// FUNCTION: 0x00536900 HealthNavCommandClass::Get_Category
// FUNCTION: 0x00536920 HealthNavCommandClass::Get_Description
// FUNCTION: 0x00536940 HealthNavCommandClass::Unknown14
// FUNCTION: 0x00536950 HealthNavCommandClass::Execute
// FUNCTION: 0x00536970 VeterancyNavCommandClass::Get_Unique_Name
// FUNCTION: 0x00536980 VeterancyNavCommandClass::Get_Display_Name
// FUNCTION: 0x005369A0 VeterancyNavCommandClass::Get_Category
// FUNCTION: 0x005369C0 VeterancyNavCommandClass::Get_Description
// FUNCTION: 0x005369E0 VeterancyNavCommandClass::Unknown14
// FUNCTION: 0x005369F0 VeterancyNavCommandClass::Execute
// FUNCTION: 0x00536A10 PrevObjectCommandClass::Get_Unique_Name
// FUNCTION: 0x00536A20 PrevObjectCommandClass::Get_Display_Name
// FUNCTION: 0x00536A40 PrevObjectCommandClass::Get_Category
// FUNCTION: 0x00536A60 PrevObjectCommandClass::Get_Description
// FUNCTION: 0x00536A80 PrevObjectCommandClass::Execute
// FUNCTION: 0x00536B10 StopCommandClass::Get_Unique_Name
// FUNCTION: 0x00536B20 StopCommandClass::Get_Display_Name
// FUNCTION: 0x00536B40 StopCommandClass::Get_Category
// FUNCTION: 0x00536B60 StopCommandClass::Get_Description
// FUNCTION: 0x00536B80 StopCommandClass::Execute
// FUNCTION: 0x00536B90 AllToCheerCommandClass::Get_Unique_Name
// FUNCTION: 0x00536BA0 AllToCheerCommandClass::Get_Display_Name
// FUNCTION: 0x00536BC0 AllToCheerCommandClass::Get_Category
// FUNCTION: 0x00536BE0 AllToCheerCommandClass::Get_Description
// FUNCTION: 0x00536C00 AllToCheerCommandClass::Execute
// FUNCTION: 0x00536C10 DeployCommandClass::Get_Unique_Name
// FUNCTION: 0x00536C20 DeployCommandClass::Get_Display_Name
// FUNCTION: 0x00536C40 DeployCommandClass::Get_Category
// FUNCTION: 0x00536C60 DeployCommandClass::Get_Description
// FUNCTION: 0x00536C80 DeployCommandClass::Execute
// FUNCTION: 0x00536C90 GuardCommandClass::Get_Unique_Name
// FUNCTION: 0x00536CA0 GuardCommandClass::Get_Display_Name
// FUNCTION: 0x00536CC0 GuardCommandClass::Get_Category
// FUNCTION: 0x00536CE0 GuardCommandClass::Get_Description
// FUNCTION: 0x00536D00 GuardCommandClass::Execute
// FUNCTION: 0x00536D10 ScatterCommandClass::Get_Unique_Name
// FUNCTION: 0x00536D20 ScatterCommandClass::Get_Display_Name
// FUNCTION: 0x00536D40 ScatterCommandClass::Get_Category
// FUNCTION: 0x00536D60 ScatterCommandClass::Get_Description
// FUNCTION: 0x00536D80 ScatterCommandClass::Execute
// FUNCTION: 0x00536D90 CenterViewCommandClass::Get_Unique_Name
// FUNCTION: 0x00536DA0 CenterViewCommandClass::Get_Display_Name
// FUNCTION: 0x00536DC0 CenterViewCommandClass::Get_Category
// FUNCTION: 0x00536DE0 CenterViewCommandClass::Get_Description
// FUNCTION: 0x00536E00 CenterViewCommandClass::Execute
// FUNCTION: 0x00536E30 CenterBaseCommandClass::Get_Unique_Name
// FUNCTION: 0x00536E40 CenterBaseCommandClass::Get_Display_Name
// FUNCTION: 0x00536E60 CenterBaseCommandClass::Get_Category
// FUNCTION: 0x00536E80 CenterBaseCommandClass::Get_Description
// FUNCTION: 0x00536EA0 CenterBaseCommandClass::Execute
// FUNCTION: 0x00536EB0 AllianceCommandClass::Get_Unique_Name
// FUNCTION: 0x00536EC0 AllianceCommandClass::Get_Display_Name
// FUNCTION: 0x00536EE0 AllianceCommandClass::Get_Category
// FUNCTION: 0x00536F00 AllianceCommandClass::Get_Description
// FUNCTION: 0x00536F20 AllianceCommandClass::Execute
// FUNCTION: 0x00536F30 ToggleRepairCommandClass::Get_Unique_Name
// FUNCTION: 0x00536F40 ToggleRepairCommandClass::Get_Display_Name
// FUNCTION: 0x00536F60 ToggleRepairCommandClass::Get_Category
// FUNCTION: 0x00536F80 ToggleRepairCommandClass::Get_Description
// FUNCTION: 0x00536FA0 ToggleRepairCommandClass::Execute
// FUNCTION: 0x00536FB0 ToggleSellCommandClass::Get_Unique_Name
// FUNCTION: 0x00536FC0 ToggleSellCommandClass::Get_Display_Name
// FUNCTION: 0x00536FE0 ToggleSellCommandClass::Get_Category
// FUNCTION: 0x00537000 ToggleSellCommandClass::Get_Description
// FUNCTION: 0x00537020 ToggleSellCommandClass::Execute
// FUNCTION: 0x00537030 BeaconPlacementCommandClass::Get_Unique_Name
// FUNCTION: 0x00537040 BeaconPlacementCommandClass::Get_Display_Name
// FUNCTION: 0x00537060 BeaconPlacementCommandClass::Get_Category
// FUNCTION: 0x00537080 BeaconPlacementCommandClass::Get_Description
// FUNCTION: 0x005370A0 BeaconPlacementCommandClass::Execute
// FUNCTION: 0x005370C0 CenterREventCommandClass::Get_Unique_Name
// FUNCTION: 0x005370D0 CenterREventCommandClass::Get_Display_Name
// FUNCTION: 0x005370F0 CenterREventCommandClass::Get_Category
// FUNCTION: 0x00537110 CenterREventCommandClass::Get_Description
// FUNCTION: 0x00537130 CenterREventCommandClass::Execute
// FUNCTION: 0x00537140 SidebarUpCommandClass::Get_Unique_Name
// FUNCTION: 0x00537150 SidebarUpCommandClass::Get_Display_Name
// FUNCTION: 0x00537170 SidebarUpCommandClass::Get_Category
// FUNCTION: 0x00537190 SidebarUpCommandClass::Get_Description
// FUNCTION: 0x005371B0 SidebarUpCommandClass::Execute
// FUNCTION: 0x005371D0 SidebarDownCommandClass::Get_Unique_Name
// FUNCTION: 0x005371E0 SidebarDownCommandClass::Get_Display_Name
// FUNCTION: 0x00537200 SidebarDownCommandClass::Get_Category
// FUNCTION: 0x00537220 SidebarDownCommandClass::Get_Description
// FUNCTION: 0x00537240 SidebarDownCommandClass::Execute
// FUNCTION: 0x00537260 OptionsCommandClass::Get_Unique_Name
// FUNCTION: 0x00537270 OptionsCommandClass::Get_Display_Name
// FUNCTION: 0x00537290 OptionsCommandClass::Get_Category
// FUNCTION: 0x005372B0 OptionsCommandClass::Get_Description
// FUNCTION: 0x005372D0 OptionsCommandClass::Execute
// FUNCTION: 0x005372E0 View1CommandClass::Get_Unique_Name
// FUNCTION: 0x005372F0 View1CommandClass::Get_Display_Name
// FUNCTION: 0x00537310 View1CommandClass::Get_Category
// FUNCTION: 0x00537330 View1CommandClass::Get_Description
// FUNCTION: 0x00537350 View1CommandClass::Execute
// FUNCTION: 0x005373D0 View2CommandClass::Get_Unique_Name
// FUNCTION: 0x005373E0 View2CommandClass::Get_Display_Name
// FUNCTION: 0x00537400 View2CommandClass::Get_Category
// FUNCTION: 0x00537420 View2CommandClass::Get_Description
// FUNCTION: 0x00537440 View2CommandClass::Execute
// FUNCTION: 0x005374C0 View3CommandClass::Get_Unique_Name
// FUNCTION: 0x005374D0 View3CommandClass::Get_Display_Name
// FUNCTION: 0x005374F0 View3CommandClass::Get_Category
// FUNCTION: 0x00537510 View3CommandClass::Get_Description
// FUNCTION: 0x00537530 View3CommandClass::Execute
// FUNCTION: 0x005375B0 View4CommandClass::Get_Unique_Name
// FUNCTION: 0x005375C0 View4CommandClass::Get_Display_Name
// FUNCTION: 0x005375E0 View4CommandClass::Get_Category
// FUNCTION: 0x00537600 View4CommandClass::Get_Description
// FUNCTION: 0x00537620 View4CommandClass::Execute
// FUNCTION: 0x005376A0 SetView1CommandClass::Get_Unique_Name
// FUNCTION: 0x005376B0 SetView1CommandClass::Get_Display_Name
// FUNCTION: 0x005376D0 SetView1CommandClass::Get_Category
// FUNCTION: 0x005376F0 SetView1CommandClass::Get_Description
// FUNCTION: 0x00537710 SetView1CommandClass::Execute
// FUNCTION: 0x00537760 SetView2CommandClass::Get_Unique_Name
// FUNCTION: 0x00537770 SetView2CommandClass::Get_Display_Name
// FUNCTION: 0x00537790 SetView2CommandClass::Get_Category
// FUNCTION: 0x005377B0 SetView2CommandClass::Get_Description
// FUNCTION: 0x005377D0 SetView2CommandClass::Execute
// FUNCTION: 0x00537820 SetView3CommandClass::Get_Unique_Name
// FUNCTION: 0x00537830 SetView3CommandClass::Get_Display_Name
// FUNCTION: 0x00537850 SetView3CommandClass::Get_Category
// FUNCTION: 0x00537870 SetView3CommandClass::Get_Description
// FUNCTION: 0x00537890 SetView3CommandClass::Execute
// FUNCTION: 0x005378E0 SetView4CommandClass::Get_Unique_Name
// FUNCTION: 0x005378F0 SetView4CommandClass::Get_Display_Name
// FUNCTION: 0x00537910 SetView4CommandClass::Get_Category
// FUNCTION: 0x00537930 SetView4CommandClass::Get_Description
// FUNCTION: 0x00537950 SetView4CommandClass::Execute
// FUNCTION: 0x005379A0 FollowCommandClass::Get_Unique_Name
// FUNCTION: 0x005379B0 FollowCommandClass::Get_Display_Name
// FUNCTION: 0x005379D0 FollowCommandClass::Get_Category
// FUNCTION: 0x005379F0 FollowCommandClass::Get_Description
// FUNCTION: 0x00537A10 FollowCommandClass::Execute
// FUNCTION: 0x00537A50 MultiplayerDebugCommandClass::Get_Unique_Name
// FUNCTION: 0x00537A60 MultiplayerDebugCommandClass::Get_Category
// FUNCTION: 0x00537A70 MultiplayerDebugCommandClass::Get_Display_Name
// FUNCTION: 0x00537A80 MultiplayerDebugCommandClass::Get_Description
// FUNCTION: 0x00537A90 MultiplayerDebugCommandClass::Execute
// FUNCTION: 0x00537AB0 MultiplayerSyncCommandClass::Get_Unique_Name
// FUNCTION: 0x00537AC0 MultiplayerSyncCommandClass::Get_Category
// FUNCTION: 0x00537AD0 MultiplayerSyncCommandClass::Get_Display_Name
// FUNCTION: 0x00537AE0 MultiplayerSyncCommandClass::Get_Description
// FUNCTION: 0x00537AF0 MultiplayerSyncCommandClass::Execute
// FUNCTION: 0x00537B40 ScreenCaptureCommandClass::ScreenCaptureCommandClass
// FUNCTION: 0x00537B50 ScreenCaptureCommandClass::Get_Unique_Name
// FUNCTION: 0x00537B60 ScreenCaptureCommandClass::Get_Category
// FUNCTION: 0x00537B80 ScreenCaptureCommandClass::Get_Display_Name
// FUNCTION: 0x00537BA0 ScreenCaptureCommandClass::Get_Description
// FUNCTION: 0x00537BC0 ScreenCaptureCommandClass::Execute
// FUNCTION: 0x00537DD0 PageUserCommandClass::PageUserCommandClass
// FUNCTION: 0x00537DE0 PageUserCommandClass::Get_Unique_Name
// FUNCTION: 0x00537DF0 PageUserCommandClass::Get_Display_Name
// FUNCTION: 0x00537E10 PageUserCommandClass::Get_Category
// FUNCTION: 0x00537E30 PageUserCommandClass::Get_Description
// FUNCTION: 0x00537E50 PageUserCommandClass::Execute
// FUNCTION: 0x00537E70 CursorPositionCommandClass::CursorPositionCommandClass
// FUNCTION: 0x00537E80 CursorPositionCommandClass::Get_Unique_Name
// FUNCTION: 0x00537E90 CursorPositionCommandClass::Get_Display_Name
// FUNCTION: 0x00537EB0 CursorPositionCommandClass::Get_Category
// FUNCTION: 0x00537ED0 CursorPositionCommandClass::Get_Description
// FUNCTION: 0x00537EF0 CursorPositionCommandClass::Execute
// FUNCTION: 0x00537F10 DeleteCommandClass::DeleteCommandClass
// FUNCTION: 0x00537F20 DeleteCommandClass::Get_Unique_Name
// FUNCTION: 0x00537F30 DeleteCommandClass::Get_Display_Name
// FUNCTION: 0x00537F50 DeleteCommandClass::Get_Category
// FUNCTION: 0x00537F70 DeleteCommandClass::Get_Description
// FUNCTION: 0x00537F90 DeleteCommandClass::Execute
// FUNCTION: 0x00537FA0 MultiplayerDebugCommandClass::~MultiplayerDebugCommandClass
// FUNCTION: 0x00537FC0 MultiplayerSyncCommandClass::~MultiplayerSyncCommandClass
// FUNCTION: 0x00537FE0 FollowCommandClass::~FollowCommandClass
// FUNCTION: 0x00538000 View1CommandClass::~View1CommandClass
// FUNCTION: 0x00538020 View2CommandClass::~View2CommandClass
// FUNCTION: 0x00538040 View3CommandClass::~View3CommandClass
// FUNCTION: 0x00538060 View4CommandClass::~View4CommandClass
// FUNCTION: 0x00538080 SetView1CommandClass::~SetView1CommandClass
// FUNCTION: 0x005380A0 SetView2CommandClass::~SetView2CommandClass
// FUNCTION: 0x005380C0 SetView3CommandClass::~SetView3CommandClass
// FUNCTION: 0x005380E0 SetView4CommandClass::~SetView4CommandClass
// FUNCTION: 0x00538100 OptionsCommandClass::~OptionsCommandClass
// FUNCTION: 0x00538120 SidebarUpCommandClass::~SidebarUpCommandClass
// FUNCTION: 0x00538140 SidebarDownCommandClass::~SidebarDownCommandClass
// FUNCTION: 0x00538160 CenterREventCommandClass::~CenterREventCommandClass
// FUNCTION: 0x00538180 BeaconPlacementCommandClass::~BeaconPlacementCommandClass
// FUNCTION: 0x005381A0 ToggleSellCommandClass::~ToggleSellCommandClass
// FUNCTION: 0x005381C0 ToggleRepairCommandClass::~ToggleRepairCommandClass
// FUNCTION: 0x005381E0 AllianceCommandClass::~AllianceCommandClass
// FUNCTION: 0x00538200 CenterBaseCommandClass::~CenterBaseCommandClass
// FUNCTION: 0x00538220 CenterViewCommandClass::~CenterViewCommandClass
// FUNCTION: 0x00538240 ScatterCommandClass::~ScatterCommandClass
// FUNCTION: 0x00538260 GuardCommandClass::~GuardCommandClass
// FUNCTION: 0x00538280 StopCommandClass::~StopCommandClass
// FUNCTION: 0x005382A0 AllToCheerCommandClass::~AllToCheerCommandClass
// FUNCTION: 0x005382C0 DeployCommandClass::~DeployCommandClass
// FUNCTION: 0x005382E0 PrevObjectCommandClass::~PrevObjectCommandClass
// FUNCTION: 0x00538300 NextObjectCommandClass::~NextObjectCommandClass
// FUNCTION: 0x00538320 PlanningModeCommandClass::~PlanningModeCommandClass
// FUNCTION: 0x00538340 CombatantSelectCommandClass::~CombatantSelectCommandClass
// FUNCTION: 0x00538360 TypeSelectCommandClass::~TypeSelectCommandClass
// FUNCTION: 0x00538380 HealthNavCommandClass::~HealthNavCommandClass
// FUNCTION: 0x005383A0 VeterancyNavCommandClass::~VeterancyNavCommandClass
// FUNCTION: 0x005383C0 SetStructureTabCommandClass::~SetStructureTabCommandClass
// FUNCTION: 0x005383E0 SetDefenseTabCommandClass::~SetDefenseTabCommandClass
// FUNCTION: 0x00538400 SetUnitTabCommandClass::~SetUnitTabCommandClass
// FUNCTION: 0x00538420 SetInfantryTabCommandClass::~SetInfantryTabCommandClass
// FUNCTION: 0x00538440 ScreenCaptureCommandClass::~ScreenCaptureCommandClass
// FUNCTION: 0x00538460 PageUserCommandClass::~PageUserCommandClass
// FUNCTION: 0x00538480 CursorPositionCommandClass::~CursorPositionCommandClass
// FUNCTION: 0x005384A0 DeleteCommandClass::~DeleteCommandClass

// FUNCTION: 0x005384C0 ?$DynamicVectorClass@PAVCCINIClass@@::Clear
template class DynamicVectorClass<CCINIClass *>;

// FUNCTION: 0x005384F0 ?$DynamicVectorClass@PAVCCINIClass@@::ID
// FUNCTION: 0x00538510 ?$VectorClass@PAVCCINIClass@@::GetItem
// FUNCTION: 0x00538520 ?$VectorClass@PAVCCINIClass@@::operator==
// FUNCTION: 0x00538560 ?$VectorClass@PAVCCINIClass@@::Resize
// FUNCTION: 0x00538610 ?$VectorClass@PAVCCINIClass@@::Clear
// FUNCTION: 0x00538640 ?$VectorClass@PAVCCINIClass@@::ID
// FUNCTION: 0x00538680 ?$VectorClass@PAVCCINIClass@@::ID
// FUNCTION: 0x005386A0 ?$DynamicVectorClass@PAVCCINIClass@@::Resize
// FUNCTION: 0x00538750 ?$DynamicVectorClass@PAVCCINIClass@@::ID
// FUNCTION: 0x00538780 ?$DynamicVectorClass@PBVCommandClass@@::Add
template class DynamicVectorClass<CommandClass const *>;

// FUNCTION: 0x005387D0 ?$IndexClass@HPBVCommandClass@@::Add_Index
template class IndexClass<int, CommandClass const *>;
