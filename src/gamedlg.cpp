// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

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
namespace Unsorted { extern int MuteSWLaunches; }
namespace Game { extern int SpecialDialog; }
extern char const * DAT_00822730[];
extern char const * DAT_0082274C[];
extern char const * DAT_00822768[];
extern char const * DAT_00822774[];

namespace UI {
	HWND __fastcall BeginDialog(int id, DLGPROC proc, int extra);
	void __fastcall EndDialog(HWND window);
}

extern CCToolTip * ToolTips;
extern unsigned char DAT_00A8EDDC;
extern int FUN_00407000(void);
extern BOOL CALLBACK Game_Controls_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// SYNTHETIC: GAMEMD 0x004E1AF0 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x004E1B10 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x004E1B40 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x004E1B60 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x004E1B80 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x004E1BA0 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x004E1BC0 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x004E1BE0 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x004E1C20 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x004E1C50 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x004E1C80 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x004E1CB0 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x004E1CC0 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x004E1CE0 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x004E1D00
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

// FUNCTION: GAMEMD 0x004E1DE0
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

// FUNCTION: GAMEMD 0x004E1FE0
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
