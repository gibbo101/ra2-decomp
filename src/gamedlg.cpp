// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "gamedlg.h"

#include "_map.h"
#include "_tooltip.h"
#include "cctooltip.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "ownrdraw.h"
#include "queue.h"
#include "session.h"
#include "techno.h"
#include "special.hh"
#undef LoadString
#include "stringtable.h"

#include <math.h>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E9370;
extern const double DAT_007E9378;

namespace Unsorted { extern int MuteSWLaunches; }
namespace Game { extern int SpecialDialog; }
extern char const * DAT_00822730[];
extern char const * DAT_0082274C[];
extern char const * DAT_00822768[];
extern char const * DAT_00822774[];

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x004E1B10 $E84
double DAT_008B3EB8 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004E1B40 $E87
double DAT_008B3F08 = DAT_008B3EB8 / DAT_007E9370;

// FUNCTION: 0x004E1B60 $E90
double DAT_008B3EF0 = DAT_007E1718 / DAT_008B3F08;

// FUNCTION: 0x004E1B80 $E93
double DAT_008B3EE8 = DAT_007E1720 * DAT_007E9378;

// FUNCTION: 0x004E1BA0 $E96
double DAT_008B3EC0 = DAT_007E1728 * DAT_007E9378;

// FUNCTION: 0x004E1BC0 $E99
double DAT_008B3E98 = DAT_007E1730 * DAT_007E9378;

// FUNCTION: 0x004E1BE0 $E102
int DAT_008B3F10 = (int)(fastmath::tan(DAT_008B3E98 - DAT_008B3EC0) * DAT_007E1738 * DAT_008B3EB8);

// FUNCTION: 0x004E1C20 $E105
double DAT_008B3EC8 = fastmath::atan(DAT_008B3F10 * DAT_007E1740);

// FUNCTION: 0x004E1C50 $E108
double DAT_008B3EE0 = fastmath::atan((DAT_008B3F10 * 2) / DAT_008B3EB8);

// FUNCTION: 0x004E1C80 $E111
int DAT_008B3F04 = (int)(DAT_008B3F10 * 4 + DAT_007E1738);

// FUNCTION: 0x004E1CB0 $E114
ShortPair DAT_008B3EB0;

// FUNCTION: 0x004E1CC0 $E117
IntTriple DAT_008B3ED0;

struct SizePair {
	int W, H, Z;
	SizePair() { Z = 0; W = 128; H = 128; }
};

// FUNCTION: 0x004E1CE0 $E120
SizePair DAT_008B3EF8;

namespace UI {
	HWND __fastcall BeginDialog(int id, DLGPROC proc, int extra);
	void __fastcall EndDialog(HWND window);
}

extern CCToolTip * ToolTips;
extern unsigned char DAT_00A8EDDC;
extern int FUN_00407000(void);
extern BOOL CALLBACK Game_Controls_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x004E1D00
void GameControlsClass::Dialog(void)
{
	int res = -1;

	DebugString("GameControls: GameSpeed = %d, ScrollRate = %d, Detail = %d\n", Options.GameSpeed, Options.ScrollRate, Options.DetailLevel);

	if (GameActive == true) {
		_Dialog = UI::BeginDialog(3003, (DLGPROC)Game_Controls_Dialog_Proc, 0);
	} else {
		_Dialog = UI::BeginDialog(245, (DLGPROC)Game_Controls_Dialog_Proc, 0);
	}
	if (_Dialog) {
		SetWindowLong(_Dialog, DWL_USER, (LONG)&res);
		OwnerDraw::Display_Dialog(_Dialog);

		while (res == -1) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				res = 2;
			}
			if (!GameActive) {
				Title_Screen_Restore();
			}
		}
		if (res == 1) {
			Set();
			Options.Save_Settings();
		}

		OwnerDraw::End_Dialog(_Dialog);
	}

	DebugString("GameControls: GameSpeed = %d, ScrollRate = %d, Detail = %d\n", Options.GameSpeed, Options.ScrollRate, Options.DetailLevel);
}

// FUNCTION: 0x004E1DE0
void GameControlsClass::Set(void)
{
	HWND handle;

	handle = GetDlgItem(_Dialog, 0x529);
	if (handle) {
		int gamespeed = 6 - SendMessage(handle, 0x400, 0, 0);
		if (Options.GameSpeed != gamespeed) {
			if (GameActive == true && Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
				OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::GAMESPEED, gamespeed));
			} else {
				Options.GameSpeed = gamespeed;
			}
		}
	}

	handle = GetDlgItem(_Dialog, 0x52A);
	if (handle) {
		Options.ScrollRate = 6 - SendMessage(handle, 0x400, 0, 0);
	}

	handle = GetDlgItem(_Dialog, 0x52B);
	if (handle) {
		int detail = SendMessage(handle, 0x400, 0, 0);
		if (Options.DetailLevel != detail) {
			Options.DetailLevel = detail;
			Map.Reinit_Cell_Drawers();
		}
	}

	handle = GetDlgItem(_Dialog, 0x601);
	if (handle) {
		Options.ActionLines = SendMessage(handle, 0xF0, 0, 0) == 1;
		TechnoClass::Set_Action_Lines(Options.ActionLines);
	}

	handle = GetDlgItem(_Dialog, 0x604);
	if (handle) {
		Options.ShowHidden = SendMessage(handle, 0xF0, 0, 0) == 1;
	}

	handle = GetDlgItem(_Dialog, 0x602);
	if (handle) {
		Options.ToolTips = SendMessage(handle, 0xF0, 0, 0) == 1;
		if (ToolTips != NULL && GameActive == true) {
			ToolTips->Activate(Options.ToolTips);
		}
	}

	if (GameActive == false) {
		handle = GetDlgItem(_Dialog, 0x50F);
		if (handle) {
			Options.Difficulty = SendMessage(handle, 0x400, 0, 0);
		}
	}
}

// FUNCTION: 0x004E1FE0
BOOL CALLBACK Game_Controls_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND handle;
	HWND Tmp;

	BOOL rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}

	switch (message) {
		case 0x497:
			handle = GetDlgItem(window, 0x529);
			if (Session.Type == GAME_NORMAL && !DAT_00A8EDDC) {
				ShowWindow(handle, SW_HIDE);
				Tmp = GetDlgItem(window, 0x714);
				ShowWindow(Tmp, SW_HIDE);
				Tmp = GetDlgItem(window, 0x671);
				ShowWindow(Tmp, SW_HIDE);
			} else if (handle) {
				SendMessage(handle, OD_TRACKNUMBERS, 0, 0);
				SendMessage(handle, TBM_SETRANGE, TRUE, MAKELONG(0, 6));
				SendMessage(handle, TBM_SETPOS, TRUE, 6 - Options.GameSpeed);
			}

			if (Unsorted::MuteSWLaunches) {
				Tmp = GetDlgItem(window, 0x529);
				ShowWindow(Tmp, SW_HIDE);
				Tmp = GetDlgItem(window, 0x714);
				ShowWindow(Tmp, SW_HIDE);
				Tmp = GetDlgItem(window, 0x671);
				ShowWindow(Tmp, SW_HIDE);
			}
			if (Unsorted::MuteSWLaunches) {
				Tmp = GetDlgItem(window, 0x529);
				ShowWindow(Tmp, SW_HIDE);
				Tmp = GetDlgItem(window, 0x714);
				ShowWindow(Tmp, SW_HIDE);
				Tmp = GetDlgItem(window, 0x671);
				ShowWindow(Tmp, SW_HIDE);
			}

			handle = GetDlgItem(window, 0x52A);
			if (handle) {
				SendMessage(handle, OD_TRACKNUMBERS, 0, 0);
				SendMessage(handle, TBM_SETRANGE, TRUE, MAKELONG(0, 6));
				SendMessage(handle, TBM_SETPOS, TRUE, 6 - Options.ScrollRate);
			}

			handle = GetDlgItem(window, 0x52B);
			if (handle) {
				SendMessage(handle, OD_TRACKNUMBERS, 0, 0);
				SendMessage(handle, TBM_SETRANGE, TRUE, MAKELONG(0, 2));
				SendMessage(handle, TBM_SETPOS, TRUE, Options.DetailLevel);
			}

			handle = GetDlgItem(window, 0x601);
			if (handle) {
				SendMessage(handle, 0xF1, Options.ActionLines != false, 0);
			}

			handle = GetDlgItem(window, 0x604);
			if (handle) {
				SendMessage(handle, 0xF1, Options.ShowHidden != false, 0);
			}

			handle = GetDlgItem(window, 0x602);
			if (handle) {
				SendMessage(handle, 0xF1, Options.ToolTips != false, 0);
			}

			if (GameActive == true) {
				handle = GetDlgItem(window, 0x52D);
				if (handle) {
					EnableWindow(handle, FUN_00407000());
				}
			} else {
				handle = GetDlgItem(window, 0x50F);
				if (handle) {
					SendMessage(handle, OD_TRACKNUMBERS, 0, 0);
					SendMessage(handle, TBM_SETRANGE, TRUE, MAKELONG(0, 2));
					SendMessage(handle, TBM_SETPOS, TRUE, Options.Difficulty);
				}
			}
			break;

		case WM_COMMAND: {
			int code = HIWORD(wparam);
			int * retval = (int *)GetWindowLong(window, DWL_USER);
			switch (LOWORD(wparam)) {
				case 0x52C:
					if (code == 0 && GameActive == true) {
						Game::SpecialDialog = SDLG_KEYBOARD;
						*retval = 1;
					}
					break;

				case 0x52D:
					if (code == 0 && GameActive == true) {
						Game::SpecialDialog = SDLG_SOUND;
						*retval = 1;
					}
					break;

				case 0x686:
					*retval = 1;
					break;
			}
			break;
		}

		case WM_HSCROLL:
			if (LOWORD(wparam) == SB_THUMBTRACK) {
				int index = HIWORD(wparam);
				char const * name;

				if ((HWND)lparam == GetDlgItem(window, 0x529)) {
					name = DAT_00822730[index];
					handle = GetDlgItem(window, 0x671);
				} else if ((HWND)lparam == GetDlgItem(window, 0x52A)) {
					name = DAT_0082274C[index];
					handle = GetDlgItem(window, 0x672);
				} else if ((HWND)lparam == GetDlgItem(window, 0x52B)) {
					name = DAT_00822768[index];
					handle = GetDlgItem(window, 0x673);
				} else if (GameActive == false && (HWND)lparam == GetDlgItem(window, 0x50F)) {
					name = DAT_00822774[index];
					handle = GetDlgItem(window, 0x670);
				} else {
					break;
				}

				if (handle) {
					SendMessage(handle, 0x4B2, 0, (LPARAM)StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\GameDlg.CPP", 0x1B2));
				}
			}
			break;
	}
	return FALSE;
}
