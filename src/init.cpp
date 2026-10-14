// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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
extern unsigned char DAT_00A8B8B4;
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
			DAT_00A8B8B4 = true;
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
extern DynamicVectorClass<AbstractClass *> DAT_008872B0;
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
	while (DAT_008872B0.Count()) {
		delete DAT_008872B0[0];
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
