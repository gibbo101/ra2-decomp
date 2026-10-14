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

