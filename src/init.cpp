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

extern unsigned char DAT_00A8E960;
bool __fastcall FUN_0052FEC0(void * unused, bool flag);
void __fastcall FUN_0046CE10(CCINIClass * ini);
struct Game {
	static void __fastcall PlayMovie(const char * name, int theme, int a, int b, int c, int d);
	static bool __cdecl CallBack(void);
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

// FUNCTION: 0x0052FF90
void Load_Title_Page(const char * name, bool visible)
{
	HiddenSurface->Fill(0);
	Load_Title_Screen(name, HiddenSurface, &CCPalette);
	if (visible)
		Update_Visible_Surface(true, HiddenSurface);
}
