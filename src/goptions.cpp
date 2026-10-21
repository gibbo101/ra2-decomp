// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "goptions.h"
#include "_keyboar.h"
#include "_map.h"
#include "data.h"
#include "dbgprint.h"
#include "gamedlg.h"
#include "language\language.h"
#include "loaddlg.h"
#include "ownrdraw.h"
#include "queue.h"
#include "restate.h"
#include "scenario.h"
#include "wonline.h"
#include "special.hh"
#undef LoadString
#include "stringtable.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007EA5E0;
extern const double DAT_007EA5E8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x004F0EF0 SYMBOL
// _$E70
double DAT_00A8EE00 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004F0F20 SYMBOL
// _$E73
double DAT_00A8EE70 = DAT_00A8EE00 / DAT_007EA5E0;

// SYNTHETIC: GAMEMD 0x004F0F40 SYMBOL
// _$E76
double DAT_00A8EE58 = DAT_007E1718 / DAT_00A8EE70;

// SYNTHETIC: GAMEMD 0x004F0F60 SYMBOL
// _$E79
double DAT_00A8EE50 = DAT_007E1720 * DAT_007EA5E8;

// SYNTHETIC: GAMEMD 0x004F0F80 SYMBOL
// _$E82
double DAT_00A8EE08 = DAT_007E1728 * DAT_007EA5E8;

// SYNTHETIC: GAMEMD 0x004F0FA0 SYMBOL
// _$E85
double DAT_00A8EDE0 = DAT_007E1730 * DAT_007EA5E8;

// SYNTHETIC: GAMEMD 0x004F0FC0 SYMBOL
// _$E88
int DAT_00A8EE78 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00A8EDE0 - DAT_00A8EE08) * DAT_00A8EE00));

// SYNTHETIC: GAMEMD 0x004F1000 SYMBOL
// _$E91
double DAT_00A8EE10 = FUN_004CADE0(DAT_00A8EE78 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x004F1030 SYMBOL
// _$E94
double DAT_00A8EE28 = FUN_004CADE0((DAT_00A8EE78 * 2) / DAT_00A8EE00);

// SYNTHETIC: GAMEMD 0x004F1060 SYMBOL
// _$E97
int DAT_00A8EE6C = (int)(DAT_00A8EE78 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x004F1090 SYMBOL
// _$E100
ShortPair DAT_00A8EDFC;

// SYNTHETIC: GAMEMD 0x004F10A0 SYMBOL
// _$E103
IntTriple DAT_00A8EE18;

struct SizePair {
	int W, H, Z;
	SizePair() { Z = 0; W = 128; H = 128; }
};

// SYNTHETIC: GAMEMD 0x004F10C0 SYMBOL
// _$E106
SizePair DAT_00A8EE60;

namespace Unsorted { extern int MuteSWLaunches; }
extern BOOL CALLBACK Game_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern BOOL CALLBACK Abort_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern void __fastcall FUN_0065F520(ScenarioClass * scen);

// FUNCTION: GAMEMD 0x004F10E0
void Game_Options_Dialog(void)
{
	int rc = 0;

	HWND dialog;
	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		dialog = OwnerDraw::Begin_Dialog(181, Game_Options_Dialog_Proc, 0);
	} else if (Session.Type == GAME_INTERNET) {
		if (Unsorted::MuteSWLaunches) {
			dialog = OwnerDraw::Begin_Dialog(3002, Game_Options_Dialog_Proc, 0);
		} else {
			dialog = OwnerDraw::Begin_Dialog(255, Game_Options_Dialog_Proc, 0);
		}
	} else {
		dialog = OwnerDraw::Begin_Dialog(3002, Game_Options_Dialog_Proc, 0);
	}

	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		OwnerDraw::Display_Dialog(dialog);
		while (rc == 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				rc = IDOK;
			}
		}
		OwnerDraw::End_Dialog(dialog);
	}

	if (rc == 1004) {
		FUN_0065F520(Scen);
	}

	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}

void Game_Options_On_INITDIALOG(HWND window);

extern char const * DAT_00822730[];
extern char const * DAT_00823FD4[];
namespace Game { namespace Network { extern int Tournament; } extern int SpecialDialog; }

static BOOL Game_Options_On_COMMAND(HWND window, WPARAM wparam)
{
	HWND handle;

	int code = HIWORD(wparam);
	int * retval = (int *)GetWindowLong(window, DWL_USER);

	static CDTimerClass<SystemTimerClass> SpeedTimer;
	static CDTimerClass<SystemTimerClass> LatencyTimer;

	switch (LOWORD(wparam)) {

		case IDC_DELETE_GAME:
			if (!code) {
				ShowWindow(window, SW_HIDE);
				LoadOptionsClass().Delete();
				Game_Options_On_INITDIALOG(window);
				ShowWindow(window, SW_SHOW);
				UpdateWindow(window);
			}
			break;

		case IDC_SAVE_GAME:
			if (!code) {
				if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
					ShowWindow(window, SW_HIDE);
					wchar_t description[512];
					if (Session.Type == GAME_NORMAL) {
						swprintf(description, L"%s", Scen->UINameLoaded);
					} else {
						swprintf(description, L"%s", Scen->Description);
					}
					LoadOptionsClass().Save(description);
					Game_Options_On_INITDIALOG(window);
					ShowWindow(window, SW_SHOW);
					UpdateWindow(window);
				} else {
					OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::SAVEGAME));
					*retval = IDC_SAVE_GAME;
				}
			}
			break;

		case IDC_LOAD_GAME:
			if (!code) {
				ShowWindow(window, SW_HIDE);
				if (LoadOptionsClass().Load()) {
					*retval = IDC_LOAD_GAME;
				} else {
					ShowWindow(window, SW_SHOW);
					UpdateWindow(window);
				}
			}
			break;

		case IDC_BRIEFING:
			if (!code) {
				*retval = IDC_BRIEFING;
			}
			break;

		case IDC_RESUME_MISSION:
		case 0x686:
			if (!code) {
				if (Session.Type == GAME_INTERNET && !Unsorted::MuteSWLaunches) {
					handle = GetDlgItem(window, IDC_CTRLWOL_CONNECTION);
					if (handle) {
						int fudge = 3 - SendMessage(handle, TBM_GETPOS, 0, 0);
						if (fudge != Session.LatencyFudge) {
							if (!LatencyTimer) {
								CDTimerClass<SystemTimerClass> fresh(900);
								LatencyTimer = fresh;
								OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::LATENCYFUDGE, fudge));
								DebugString("LATENCYFUDGE event created - %d\n", fudge);
							} else {
								DebugString("Ignoring latency change to avoid cheating\n");
							}
						}
					}
					handle = GetDlgItem(window, IDC_GAME_SPEED_SLIDER);
					if (handle) {
						int speed = 6 - SendMessage(handle, TBM_GETPOS, 0, 0);
						if (Options.GameSpeed != speed) {
							if (!SpeedTimer) {
								CDTimerClass<SystemTimerClass> fresh(900);
								SpeedTimer = fresh;
								OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::GAMESPEED, speed));
							} else {
								DebugString("Ignoring speed change to avoid cheating\n");
							}
						}
					}
				}
				*retval = IDOK;
			}
			break;

		case IDC_ABORT_MISSION:
			if (!code) {
				if (Session.Type == GAME_INTERNET) {
					Game::SpecialDialog = SDLG_SURRENDER;
					if (!Game::Network::Tournament) {
						Game::SpecialDialog = SDLG_ABORT;
					}
				} else {
					Game::SpecialDialog = SDLG_ABORT;
				}
				*retval = IDCANCEL;
			}
			break;

		case IDC_GAME_CONTROLS:
			if (!code) {
				Game::SpecialDialog = SDLG_SETTINGS;
				*retval = IDOK;
			}
			break;

		default:
			break;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x004F11B0
BOOL CALLBACK Game_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	BOOL rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	HWND handle;

	if (rc) {
		return rc;
	}

	switch (message) {

		case 0x497:
			Game_Options_On_INITDIALOG(window);
			break;

		case WM_COMMAND:
			return Game_Options_On_COMMAND(window, wparam);

		case WM_HSCROLL: {
			if (LOWORD(wparam) == SB_THUMBTRACK) {
				int pos = HIWORD(wparam);
				char const * textid;

				if ((HWND)lparam == GetDlgItem(window, IDC_GAME_SPEED_SLIDER)) {
					textid = DAT_00822730[pos];
					handle = GetDlgItem(window, IDC_GAME_SPEED_LABEL);
				} else if ((HWND)lparam == GetDlgItem(window, IDC_CTRLWOL_CONNECTION)) {
					textid = DAT_00823FD4[pos];
					handle = GetDlgItem(window, IDC_SCROLL_SPEED_LABEL);
				} else {
					break;
				}

				if (handle) {
					SendMessage(handle, 0x4b2, 0, (LPARAM)StringTable::LoadString(textid, NULL, "D:\\ra2mdpost\\GOptions.CPP", 0xC4));
				}
			}
			break;
		}

		default:
			break;
	}

	return FALSE;
}

// SYNTHETIC: GAMEMD 0x004F1700 SYMBOL
// _$E110
// SYNTHETIC: GAMEMD 0x004F1710 SYMBOL
// _$E109

// FUNCTION: GAMEMD 0x004F1720
void Game_Options_On_INITDIALOG(HWND window)
{
	HWND handle;

	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		bool present = LoadOptionsClass().Files_Present();

		handle = GetDlgItem(window, IDC_LOAD_GAME);
		if (handle) {
			EnableWindow(handle, present);
		}

		handle = GetDlgItem(window, IDC_DELETE_GAME);
		if (handle) {
			EnableWindow(handle, present);
		}
	} else if (Session.Type == GAME_INTERNET && !Unsorted::MuteSWLaunches) {
		handle = GetDlgItem(window, 0x3EF);
		if (handle) {
			int fudge = 3 - Session.LatencyFudge;
			Slider_SetRange(handle, 0, 3);
			Slider_SetPos(handle, fudge);
		}

		handle = GetDlgItem(window, 0x529);
		if (handle) {
			Slider_SetRange(handle, 0, 6);
			Slider_SetPos(handle, 6 - Options.GameSpeed);
		}
	}
}

// FUNCTION: GAMEMD 0x004F1840
int Abort_Dialog(void)
{
	int rc = 0;

	HWND dialog = OwnerDraw::Begin_Dialog(182, Abort_Dialog_Proc, 0);

	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		OwnerDraw::Display_Dialog(dialog);
		while (rc == 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				rc = IDOK;
			}
		}
		OwnerDraw::End_Dialog(dialog);
	}
	return rc;
}

int DAT_00A8EE7C;
int DAT_00A8EE80;
extern void __fastcall FUN_00603E40(HWND dialog, int value);

// FUNCTION: GAMEMD 0x004F18B0
BOOL CALLBACK Abort_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND leave;
	HWND restart;

	BOOL rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}

	switch (message) {
	case WM_COMMAND: {
		int * retval = (int *)GetWindowLong(window, DWL_USER);
		if (HIWORD(wparam) == 0) {
			switch (LOWORD(wparam)) {
				case 1:
				case 0x686:
					*retval = 1;
					break;

				case 0x6C9:
					*retval = DAT_00A8EE7C;
					break;

				case 0x712:
					*retval = DAT_00A8EE80;
					break;
			}
		}
		break;
	}
	case 0x497:
		leave = GetDlgItem(window, 0x6C9);
		SendMessage(leave, 0x4b2, 0, (LPARAM)StringTable::LoadString("GUI:Leave", NULL, "D:\\ra2mdpost\\GOptions.CPP", 0x223));
		DAT_00A8EE7C = 2;
		restart = GetDlgItem(window, 0x712);
		DAT_00A8EE80 = 0;
		if (Session.Type == GAME_NORMAL) {
			SendMessage(restart, 0x4b2, 0, (LPARAM)StringTable::LoadString("GUI:Restart", NULL, "D:\\ra2mdpost\\GOptions.CPP", 0x22E));
			DAT_00A8EE80 = 5;
		} else if (Session.Type == GAME_SKIRMISH) {
			ShowWindow(restart, SW_HIDE);
		} else if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
			int humans = 0;
			for (int i = 0; i < Houses.Count(); i++) {
				if (Houses[i]->IsHuman && !Houses[i]->IsDefeated) {
					humans++;
				}
			}
			if (humans >= 2 && !PlayerPtr->IsDefeated) {
				DAT_00A8EE7C = 3;
				SendMessage(restart, 0x4b2, 0, (LPARAM)StringTable::LoadString("GUI:Observe", NULL, "D:\\ra2mdpost\\GOptions.CPP", 0x24A));
				DAT_00A8EE80 = 4;
			} else {
				ShowWindow(restart, SW_HIDE);
			}
		}
		FUN_00603E40(leave, DAT_00A8EE7C);
		FUN_00603E40(restart, DAT_00A8EE80);
		break;
	}
	return FALSE;
}

// TEMPLATE: GAMEMD 0x004F1AA0
// ?Add@?$QueueClass@VEventClass@@$0IA@@@QAEHABVEventClass@@@Z
template class QueueClass<EventClass, 128>;

// TEMPLATE: GAMEMD 0x004F1B00
// ??0?$CDTimerClass@VSystemTimerClass@@@@QAE@H@Z
// TEMPLATE: GAMEMD 0x004F1B20
// ?Value@?$CDTimerClass@VSystemTimerClass@@@@QBEHXZ
template class CDTimerClass<SystemTimerClass>;

// SYNTHETIC: GAMEMD 0x004F1B60 SYMBOL
// _$E112
struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};
IntQuad DAT_00A8EE88;
