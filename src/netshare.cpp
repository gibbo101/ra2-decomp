// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
#include "always.h"
#include "geometry.h"
#include "netshare.h"
#include "_rules.h"
#include "bitfont.h"
#include "cd.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "lzopipe.h"
#include "lzostraw.h"
#include "mapgen.h"
#include "modemhst.h"
#include "netdlg.h"
#include "netdlg2.h"
#include "newmenu.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "rules.h"
#include "scenario.h"
#include "sendfile.h"
#include "session.h"
#include "stimer.h"
#include "wdtnet.h"
#include "windlg.h"
#include "wolapi\wolapi.h"
#include "wonline.h"
#include "worlddom.h"
#include "wstring.h"
#include "xpipe.h"
#include "xstraw.h"
#include "houstype.h"
#include "multiplayer.h"
#include "msgbox.h"
#include "mplayer.h"
#include <ctime>
#include <stddef.h>
#include "stringtable.h"
#include <stdarg.h>
#include <wchar.h>
#include "widewstring.h"

// SYNTHETIC: GAMEMD 0x005E2410 SYMBOL
// _$E51

void FUN_00623F60(HWND listbox, int index, WideWstring & text);
void FUN_0060B350(HWND button);
void FUN_00607FD0(HWND dialog);
int FUN_007759E0(HWND window, bool (*callback)(void), int a, int b, int c);
namespace UI { void RegisterWindow(HWND window, int id); }
namespace Game { extern HWND hWnd; extern HINSTANCE hInstance; extern int PlayerColor; }

class GameModeOptionsClass { public: static int Instance; };
MultiplayerGameMode * First_Game_Mode(void);
HWND FUN_00775B10(void);
int FUN_00775B20(void);

void __fastcall FUN_0077E430(char * buffer);
void __fastcall FUN_005DBB60(char * buffer);
unsigned int __fastcall FUN_005DAC00(wchar_t const * name);
int __fastcall FUN_0069B720(SessionClass * session);
void FUN_007A2730(void);
void FUN_007A2750(void);
void FUN_007A27D0(void);
void FUN_005E2340(void);
void FUN_005E23A0(void);
void __cdecl FUN_004068E0(char const * format, ...);

rc_ptr<WDTTerritory> __fastcall FUN_007B0390(int id);

// SYNTHETIC: GAMEMD 0x005E2430 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005E2460 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005E2480 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005E24A0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005E24C0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x005E24E0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x005E2500 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005E2540 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005E2570 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005E25A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005E25D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x005E25E0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005E2600 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x005E2620
unsigned int Wstring_Hash(Wstring & string)
{
	unsigned int hash = 0;

	hash = string.length();

	for (unsigned int i = 0; i < string.length(); i++) {
		hash += *(string.get() + i);
		hash += i;
		hash = (hash << 8) ^ (hash >> 24);
	}
	return(hash);
}

// FUNCTION: GAMEMD 0x005E2670
unsigned int Wstring_Hash(WideWstring & string)
{
	unsigned int hash = 0;

	hash = string.length();

	for (unsigned int i = 0; i < string.length(); i++) {
		hash += *(string.get() + i);
		hash += i;
		hash = (hash << 8) ^ (hash >> 24);
	}
	return(hash);
}

// FUNCTION: GAMEMD 0x005E26C0
HWND GameoptWindow(void)
{
	HWND dialog;

	dialog = WS_Find_Dialog(IDD_WOL_GAMEOPT);
	if (dialog) {
		return(dialog);
	}
	dialog = WS_Find_Dialog(IDD_WOL_GUEST);
	if (dialog) {
		return(dialog);
	}
	dialog = WS_Find_Dialog(IDD_MPLAYER_HOST);
	if (dialog) {
		return(dialog);
	}
	dialog = WS_Find_Dialog(IDD_MPLAYER_GUEST);
	if (dialog) {
		return(dialog);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x005E2700
void __cdecl PMessagePrintf(int color, const wchar_t * fmt, ...)
{
	va_list va;
	wchar_t buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	va_start(va, fmt);
	vswprintf(buffer, fmt, va);
	va_end(va);

	if (WS_Top_Window() != 0) {
		HWND top = WS_Top_Window();
		HWND msg = GetDlgItem(top, 0x53f);
		while (msg == 0) {
			top = WS_Next_Lower_Dialog(top);
			if (top == 0) {
				msg = 0;
				break;
			}
			msg = GetDlgItem(top, 0x53f);
		}

		if (msg != 0) {
			_DrawMessage(color, buffer, msg);
		}
	}
}

// FUNCTION: GAMEMD 0x005E2790
void __cdecl SMessagePrintf(int color, const wchar_t * fmt, ...)
{
	va_list va;
	wchar_t buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	va_start(va, fmt);
	vswprintf(buffer, fmt, va);
	va_end(va);

	if (WS_Top_Window() != 0) {
		HWND top = WS_Top_Window();
		HWND msg = GetDlgItem(top, 0x5b5);
		while (msg == 0) {
			top = WS_Next_Lower_Dialog(top);
			if (top == 0) {
				msg = 0;
				break;
			}
			msg = GetDlgItem(top, 0x5b5);
		}

		if (msg != 0) {
			_DrawMessage(color, buffer, msg);
		}
	}
}

// FUNCTION: GAMEMD 0x005E2820
void _DrawMessage(int color, const wchar_t * message, HWND window)
{
	RECT rect;
	struct { HDC hdc; SIZE size; } l;

	int length = wcslen(message);

	int offset = 18;
	Get_Display_Rect(window, &rect);
	if (SendMessage(window, OD_HASATTACHED, 0, 0)) {
		offset = 1;
	}

	l.hdc = GetDC(window);
	SendMessage(window, OD_RESTOREDC, 0, (LPARAM)l.hdc);

	while (length) {
		if (message != NULL) {
			wchar_t const * newline = wcschr(message, L'\n');
			if (newline != NULL) {
				int linelen = newline - message + 1;
				if (length >= linelen) {
					length = linelen;
				}
			}
		}

		wchar_t * copy = _wcsdup(message);
		copy[length] = 0;
		BitFont::Instance->GetTextDimension(copy, (int *)&l.size.cx, (int *)&l.size.cy, 0);
		free(copy);
		int maxWidth = rect.right - rect.left - offset;

		if (l.size.cx >= maxWidth - 4) {
			int reduceBy;
			if (l.size.cx / 2 > maxWidth) {
				reduceBy = 10;
				length -= reduceBy;
			} else {
				reduceBy = 1;
			}

			int found = -1;
			int idx = length - 1;

			while (idx > 0) {
				if (!iswgraph(message[idx])) {
					found = idx;
					break;
				}
				idx--;
			}

			if (found == -1) {
				length -= reduceBy;
				found = length;
			}
			length = found;
		} else {
			_SetMessageString(window, message, length, color);
			message += length;
			while (*message == L' ') {
				message++;
			}
			length = wcslen(message);
		}
	}

	ReleaseDC(window, l.hdc);
}

// FUNCTION: GAMEMD 0x005E29A0
void _SetMessageString(HWND window, const wchar_t * message, int length, int color)
{
	static wchar_t buffer[512];
	memset(buffer, 0, sizeof(buffer));
	wcsncpy(buffer, message, length);
	wchar_t * line_end = wcschr(buffer, L'\r');
	if (line_end != NULL) {
		line_end[0] = L'\0';
	} else {
		line_end = wcschr(buffer, L'\n');
		if (line_end != NULL) {
			line_end[0] = L'\0';
		}
	}

	int old = SendMessage(window, OD_DISABLEPAINT, 0, 1);
	int topindex = SendMessage(window, LB_GETTOPINDEX, 0, 0);
	int count = SendMessage(window, LB_GETCOUNT, 0, 0);
	int itemheight = SendMessage(window, LB_GETITEMHEIGHT, topindex, 0);
	RECT rect;
	GetWindowRect(window, &rect);
	bool atbottom = (count - topindex) <= (rect.bottom - rect.top) / itemheight;
	if (count > 500) {
		SendMessage(window, LB_DELETESTRING, 0, 0);
		count--;
	}

	int index = SendMessage(window, 0x4cd, 0, (LPARAM)buffer);
	if (color != -1) {
		SendMessage(window, OD_SETCOLOR, index, color);
	}

	if (atbottom) {
		SendMessage(window, LB_SETTOPINDEX, count, 0);
	}
	SendMessage(window, OD_DISABLEPAINT, 0, old);
}

// FUNCTION: GAMEMD 0x005E2AC0
void FUN_005E2AC0(void)
{
	if (WS_Top_Window() != 0) {
		HWND top = WS_Top_Window();
		HWND msg = GetDlgItem(top, 0x53f);
		while (msg == 0) {
			top = WS_Next_Lower_Dialog(top);
			if (top == 0) {
				return;
			}
			msg = GetDlgItem(top, 0x53f);
		}

		int count = SendMessage(msg, LB_GETCOUNT, 0, 0);
		WideWstring text;
		if (count > 0) {
			FUN_00623F60(msg, count - 1, text);
		}
		if (text.length() < 0x24) {
			SendMessage(msg, LB_DELETESTRING, count - 1, 0);
			text += L".";
		} else {
			text = L".";
		}
		_DrawMessage(-1, text.get(), msg);
		InvalidateRect(msg, NULL, FALSE);
	}
}

// FUNCTION: GAMEMD 0x005E2BA0
int CountOtherUndefeatedHumanHouses(HouseClass * house)
{
	int count = 0;
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * other = Houses[i];
		if (other != NULL && other->IsHuman && !other->IsDefeated && other != house) {
			count++;
		}
	}
	return(count);
}

// FUNCTION: GAMEMD 0x005E2BE0
int CountAliveTeams(HouseClass * house)
{
	int count = 0;
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house1 = Houses[i];
		if (house1 != NULL && !house1->IsDefeated && !house1->Class->IsMultiplayPassive && house1 != house) {
			bool has_ally = false;
			for (int j = 0; j < Houses.Count(); j++) {
				if (j != i) {
					HouseClass * house2 = Houses[j];
					if (house2 != NULL && !house2->IsDefeated && !house2->Class->IsMultiplayPassive) {
						if (house2 != house && house1->Is_Ally(Houses[j]) && house2->Is_Ally(house1)) {
							if (j > i && !has_ally) {
								count++;
							}
							has_ally = true;
						}
					}
				}
			}
			if (!has_ally) {
				count++;
			}
		}
	}
	return(count);
}

// FUNCTION: GAMEMD 0x005E2CD0
int ODMessageBox(const wchar_t * text, int type, bool (*callback)(void), bool large)
{
	if (text != NULL && wcslen(text) != 0) {
		int id;
		if (large) {
			id = 0xfb;
		} else if (type == 0) {
			id = 0xd0;
		} else if (type == 1) {
			id = 0xfc;
		} else {
			id = (type != 4) ? 0xfb : 0x126;
		}
		HWND dialog = WS_Create_Dialog(Game::hInstance, id, Game::hWnd, ODMessageBox_Proc, FALSE);
		UI::RegisterWindow(dialog, id);
		if (type == 0) {
			HWND ok = GetDlgItem(dialog, 1);
			SetWindowLong(ok, GWL_STYLE, GetWindowLong(ok, GWL_STYLE) | WS_VISIBLE);
			if (large) {
				FUN_0060B350(ok);
			}
		} else if (type == 1) {
			HWND ok = GetDlgItem(dialog, 1);
			SetWindowLong(ok, GWL_STYLE, GetWindowLong(ok, GWL_STYLE) | WS_VISIBLE);
			HWND cancel = GetDlgItem(dialog, 2);
			SetWindowLong(cancel, GWL_STYLE, GetWindowLong(cancel, GWL_STYLE) | WS_VISIBLE);
		} else if (type == 4) {
			HWND yes = GetDlgItem(dialog, 6);
			SetWindowLong(yes, GWL_STYLE, GetWindowLong(yes, GWL_STYLE) | WS_VISIBLE);
			HWND no = GetDlgItem(dialog, 7);
			SetWindowLong(no, GWL_STYLE, GetWindowLong(no, GWL_STYLE) | WS_VISIBLE);
		}
		HWND label = GetDlgItem(dialog, 0x5b0);
		SendMessage(label, 0x4b2, 0, (LPARAM)text);
		ShowWindow(dialog, SW_NORMAL);
		return(FUN_007759E0(dialog, callback, 0, 1, 1));
	}
	return(0);
}

// FUNCTION: GAMEMD 0x005E2E40
int CALLBACK ODMessageBox_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::Draw_Item((LPDRAWITEMSTRUCT)lparam);
			return(TRUE);

		case WM_PAINT:
			OwnerDraw::Draw_Dialog_Back(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return(TRUE);

		case WM_ERASEBKGND:
			return(TRUE);

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
				case IDCANCEL:
				case IDYES:
				case IDNO:
					WS_Destroy_Dialog(window, LOWORD(wparam));
					return(TRUE);
			}
			break;
	}
	return(FALSE);
}

// FUNCTION: GAMEMD 0x005E2EF0
void FUN_005E2EF0(HWND dialog, MultiplayerGameMode * mode)
{
	if (mode != NULL) {
		HWND item = GetDlgItem(dialog, 0x6ec);
		SendMessage(item, 0x4b2, 0, (LPARAM)mode->UIName.text());
	}
}

// FUNCTION: GAMEMD 0x005E2F20
void __fastcall FUN_005E2F20(HWND dialog, int index)
{
	if (index >= 0 && index < Session.Scenarios.Count()) {
		MultiMission * map = Session.Scenarios[index];
		if (map != NULL) {
			HWND item = GetDlgItem(dialog, 0x5a8);
			SendMessage(item, 0x4b2, 0, (LPARAM)map);
		}
	}
}

// FUNCTION: GAMEMD 0x005E2F60
void __fastcall FUN_005E2F60(HWND dialog)
{
	HWND item = GetDlgItem(dialog, 0x5a8);
	SendMessage(item, 0x4b2, 0, (LPARAM)Session.Options.ScenarioDescription);
}

// FUNCTION: GAMEMD 0x005E2F80
MultiplayerGameMode * FUN_005E2F80(void)
{
	MultiplayerGameMode * mode = MultiplayerGameMode::Get(GameModeOptionsClass::Instance);
	if (mode == NULL) {
		mode = First_Game_Mode();
	}
	return(mode);
}

// FUNCTION: GAMEMD 0x005E2FA0
void FUN_005E2FA0(void)
{
	if (FUN_00775B20() == 0x6b) {
		HWND dialog = FUN_00775B10();
		int sel = SendDlgItemMessage(dialog, 0x6eb, LB_GETCURSEL, 0, 0);
		if (sel != -1) {
			MultiplayerGameMode * mode = (MultiplayerGameMode *)SendDlgItemMessage(dialog, 0x6eb, LB_GETITEMDATA, sel, 0);
			if (mode != NULL) {
				mode->vt_entry_94();
			}
		}
	} else if (Session.MPGameMode != NULL) {
		Session.MPGameMode->vt_entry_94();
	}
}

// FUNCTION: GAMEMD 0x005E3010
void __fastcall DisplayGameopts(HWND window, BOOL initialize)
{
	int unit_count = Session.Options.UnitCount;
	int credits = Session.Options.Credits;
	int game_speed = Session.Options.GameSpeed;

	if (initialize) {
		rc_ptr<WDTTerritory> handle;
		if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
			handle = FUN_007B0390(Session.unknown_08);
		}
		if (handle.Get() != NULL) {
			EnableWindow(GetDlgItem(window, 0x544), FALSE);
			EnableWindow(GetDlgItem(window, 0x6d4), FALSE);
			if (!(handle->UserModBooleans & 0x20000)) {
				EnableWindow(GetDlgItem(window, 0x402), FALSE);
			}
			if (!(handle->UserModBooleans & 0x8000)) {
				EnableWindow(GetDlgItem(window, 0x54f), FALSE);
			}
			if (!(handle->UserModBooleans & 0x400)) {
				EnableWindow(GetDlgItem(window, 0x54e), FALSE);
			}
			if (!(handle->UserModBooleans & 0x20)) {
				EnableWindow(GetDlgItem(window, 0x3e8), FALSE);
			}
			if (!(handle->UserModBooleans & 0x100)) {
				EnableWindow(GetDlgItem(window, 0x54d), FALSE);
			}
		}
		SendDlgItemMessage(window, 0x529, TBM_SETRANGE, TRUE, MAKELONG(0, 6));
		SendDlgItemMessage(window, 0x402, TBM_SETRANGE, TRUE, MAKELONG(Rule->MinMoney, Rule->MPMaxMoney));
		SendDlgItemMessage(window, 0x402, OD_SETTRACKSTEP, 0, Rule->MoneyIncrement);
		SendDlgItemMessage(window, 0x54f, TBM_SETRANGE, TRUE, MAKELONG(Rule->MinUnitCount, Rule->MaxUnitCount));
	}

	SendDlgItemMessage(window, 0x529, TBM_SETPOS, TRUE, 6 - game_speed);
	SendDlgItemMessage(window, 0x402, TBM_SETPOS, TRUE, credits);
	SendDlgItemMessage(window, 0x54f, TBM_SETPOS, TRUE, unit_count);

	int button_state[] = { BST_UNCHECKED, BST_CHECKED };
	SendDlgItemMessage(window, 0x54e, BM_SETCHECK, button_state[Session.Options.ShortGame], 0);
	SendDlgItemMessage(window, 0x69a, BM_SETCHECK, button_state[Session.Options.SWAllowed], 0);
	SendDlgItemMessage(window, 0x69d, BM_SETCHECK, button_state[Session.Options.BuildOffAlly], 0);
	SendDlgItemMessage(window, 0x3e8, BM_SETCHECK, button_state[Session.Options.MCVRedeploy], 0);
	SendDlgItemMessage(window, 0x54d, BM_SETCHECK, button_state[Session.Options.Goodies], 0);
}

// FUNCTION: GAMEMD 0x005E32D0
void PumpGameopts(bool force, bool now)
{
	static int _last_pump_time;
	static bool _need_to_pump;
	static bool _last_short_game;
	static bool _last_sw_allowed;
	static bool _last_build_off_ally;
	static bool _last_mcv_redeploy;
	static bool _last_crates;
	static int _last_game_speed = -1;
	static int _last_credits = -1;
	static int _last_unit_count = -1;
	static bool _last_scenario_is_official = true;
	static unsigned int _last_scenario_file_length = -1;
	static wchar_t _last_scenario_description[45];
	static char _last_scenario_file_name[14];
	static char _last_scenario_digest[33];

	bool do_pump = false;
	bool it_is_time = false;

	if (time(NULL) - _last_pump_time != 0) {
		it_is_time = true;
	}

	if (!it_is_time) {
		if (force) {
			_need_to_pump = true;
		}
	}

	if (it_is_time || now) {
		if (_need_to_pump) {
			force = true;
		}
		_need_to_pump = false;

		if (g_pChat != NULL) {
			if (time(NULL) - _last_pump_time >= 5) {
				do_pump = true;
			}
		} else {
			if (time(NULL) - _last_pump_time >= 2) {
				do_pump = true;
			}
		}

		if (force) {
			do_pump = true;
		}

		if (_last_game_speed != Session.Options.GameSpeed) do_pump = true;
		if (_last_credits != Session.Options.Credits) do_pump = true;
		if (_last_unit_count != Session.Options.UnitCount) do_pump = true;
		if (_last_short_game != Session.Options.ShortGame) do_pump = true;
		if (_last_sw_allowed != Session.Options.SWAllowed) do_pump = true;
		if (_last_build_off_ally != Session.Options.BuildOffAlly) do_pump = true;
		if (_last_mcv_redeploy != Session.Options.MCVRedeploy) do_pump = true;
		if (_last_crates != Session.Options.Goodies) do_pump = true;

		_last_scenario_description[43] = 0;
		if (wcsicmp(_last_scenario_description, Session.Options.ScenarioDescription)) do_pump = true;

		_last_scenario_file_name[13] = 0;
		if (stricmp(_last_scenario_file_name, Session.ScenarioFileName)) do_pump = true;

		_last_scenario_digest[32] = 0;
		if (stricmp(_last_scenario_digest, Session.ScenarioDigest)) do_pump = true;

		if (_last_scenario_file_length != Session.ScenarioFileLength) do_pump = true;
		if (_last_scenario_is_official != Session.ScenarioIsOfficial) do_pump = true;

		if (do_pump) {
			_last_pump_time = time(0);
			_last_game_speed = Session.Options.GameSpeed;
			_last_credits = Session.Options.Credits;
			_last_unit_count = Session.Options.UnitCount;
			_last_short_game = Session.Options.ShortGame;
			_last_sw_allowed = Session.Options.SWAllowed;
			_last_build_off_ally = Session.Options.BuildOffAlly;
			_last_mcv_redeploy = Session.Options.MCVRedeploy;
			_last_crates = Session.Options.Goodies;
			wcscpy(_last_scenario_description, Session.Options.ScenarioDescription);
			strcpy(_last_scenario_digest, Session.ScenarioDigest);
			strcpy(_last_scenario_file_name, Session.ScenarioFileName);
			_last_scenario_file_length = Session.ScenarioFileLength;
			_last_scenario_is_official = Session.ScenarioIsOfficial;

			char buffer[520];
			memset(buffer, '\0', sizeof(buffer));

			if (g_pChat != NULL) {
				FUN_0077E430(buffer);
				SendPublicGameopts(buffer);
			} else {
				FUN_005DBB60(buffer);
				SendPublicGameopts(buffer);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x005E35A0
void FUN_005E35A0(void)
{
	GlobalPacketType packet;
	memset(&packet, 0, sizeof(packet));
	packet.Command = (NetCommandType)0x28;
	strcpy(packet.Name, Session.Handle);
	int count = Session.Players.Count();
	for (int i = 1; i < count; i++) {
		Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &Session.Players[i]->Address);
		Call_Back();
	}
}

// FUNCTION: GAMEMD 0x005E3630
void FUN_005E3630(void)
{
	switch (Session.Type) {
		case GAME_IPX:
			FUN_005E35A0();
			break;

		case GAME_INTERNET:
			FUN_007A2730();
			break;
	}
}

// FUNCTION: GAMEMD 0x005E36E0
void FUN_005E36E0(void)
{
	Session.unknown_2878 = true;
	if (g_pChat != NULL) {
		FUN_007A2750();
	} else {
		FUN_005E2340();
	}
}

// FUNCTION: GAMEMD 0x005E3700
void FUN_005E3700(void)
{
	Session.unknown_2878 = false;
	if (g_pChat != NULL) {
		FUN_007A27D0();
	} else {
		FUN_005E23A0();
	}
}

// FUNCTION: GAMEMD 0x005E3720
void SendPublicGameopts(char const * options)
{
	if (g_pChat != NULL) {
		char buffer[0x400];
		strncpy(buffer, options, 0x3ff);
		g_pChat->RequestPublicGameOptions((LPCSTR)buffer);
	} else {
		GlobalPacketType packet;
		memset(&packet, 0, sizeof(packet));
		packet.Command = NET_PUB_GAMEOPT;
		strcpy(packet.Name, Session.Handle);
		strcpy(packet.Options.Buf, options);
		packet.Options.Color = Game::PlayerColor;
		packet.Options.NameCRC = FUN_005DAC00(Session.GameName);
		for (int i = 1; i < Session.Players.Count(); i++) {
			NodeNameType * node = Session.Players[i];
			FUN_004068E0("Sending public game options to %s\n", FUN_007350C0(node->Name));
			Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &node->Address, 0, 0);
			Call_Back();
		}
	}
}

// FUNCTION: GAMEMD 0x005E3850
void SendPrivateGameopts(char const * player, char const * options)
{
	if (g_pChat != NULL) {
		User user;
		memset(&user, '\0', sizeof(user));
		strcpy((char *)user.name, player);
		g_pChat->RequestPrivateGameOptions(&user, (LPCSTR)options);
	} else {
		GlobalPacketType packet;
		memset(&packet, 0, sizeof(packet));
		packet.Command = NET_PRIV_GAMEOPT;
		strcpy(packet.Name, Session.Handle);
		strcpy(packet.Options.Buf, options);
		Session.GPacket.Options.Color = FUN_0069B720(&Session);
		Session.GPacket.Options.NameCRC = FUN_005DAC00(Session.GameName);
		for (int i = 1; i < Session.Players.Count(); i++) {
			if (stricmp(FUN_007350C0(Session.Players[i]->Name), player) == 0) {
				FUN_004068E0("Sending private game options to %ls\n", Session.Players[i]);
				Ipx.Send_Global_Message(&packet, sizeof(packet), 1, &Session.Players[i]->Address, 0, 0);
				Call_Back();
			}
		}
	}
}

extern bool DAT_00AC11C2;
int __fastcall FUN_0077EBA0(Wstring const & who);
void __fastcall FUN_0077E900(Wstring const & who, int status);
int __fastcall FUN_005DBEC0(char * who);
void __fastcall FUN_005DBE20(char * who, int status);
HWND __fastcall FUN_00775940(int id);

// FUNCTION: GAMEMD 0x005E39C0
void FUN_005E39C0(char * name, int status)
{
	if (name == NULL) {
		name = Session.Handle;
	}
	if (wcscmp(FUN_00735120(name), Session.GameName) == 0) {
		status = 1;
	}
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (stricmp(FUN_007350C0(Session.Players[i]->Name), name) == 0) {
			if (i != -1) {
				Session.Players[i]->Slot.Unknown7F = status;
				if (status == 0) {
#line 1169 "D:\\ra2mdpost\\netshare.cpp"
					PMessagePrintf(-1, StringTable::LoadString("GUI:HostNoMap", NULL, __FILE__, __LINE__), Session.Players[i], Session.Options.ScenarioDescription);
				} else if (status == 2) {
#line 1173 "D:\\ra2mdpost\\netshare.cpp"
					PMessagePrintf(-1, StringTable::LoadString("GUI:HostMapTransfer", NULL, __FILE__, __LINE__), Session.Players[i], Session.Options.ScenarioDescription);
				}
			}
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x005E3AB0
void FUN_005E3AB0(void)
{
	int accept;
	if (g_pChat != NULL) {
		accept = FUN_0077EBA0(NULL);
	} else {
		accept = FUN_005DBEC0(NULL);
	}
	FUN_005E3700();
	if (accept == 1) {
		char buffer[64];
		if (g_pChat != NULL) {
			FUN_0077E900(NULL, 0);
		} else {
			FUN_005DBE20(NULL, 0);
		}
		sprintf(buffer, "A0");
		SendPublicGameopts(buffer);
		if (DAT_00AC11C2) {
			EnableWindow(GetDlgItem(GameoptWindow(), 0x59f), TRUE);
		} else {
			EnableWindow(GetDlgItem(GameoptWindow(), 0x59f), FALSE);
		}
		InvalidateRect(GetDlgItem(GameoptWindow(), 0x59f), NULL, FALSE);
	} else {
		if (!IsWindowEnabled(GetDlgItem(GameoptWindow(), 0x59f)) && DAT_00AC11C2) {
			EnableWindow(GetDlgItem(GameoptWindow(), 0x59f), TRUE);
			InvalidateRect(GetDlgItem(GameoptWindow(), 0x59f), NULL, FALSE);
		}
	}
}

void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);

static inline void LBGetText(HWND win, int index, wchar_t * wide, char * narrow)
{
	if (SendMessage(win, 0x4cf, index, 0) != 0) {
		SendMessage(win, 0x4b6, index, (LPARAM)wide);
	} else {
		SendMessage(win, 0x4b7, index, (LPARAM)narrow);
		FUN_00735060(wide, narrow, -1);
	}
}

static inline void LBSelectText(HWND win, WideWstring & key, wchar_t * wide)
{
	wcscpy(wide, key.get());
	int index = SendMessage(win, 0x4ca, -1, (LPARAM)wide);
	if (index != LB_ERR) {
		ListBox_SetCurSel(win, index);
	}
}

// FUNCTION: GAMEMD 0x005E6230
void LBRestoreSelections(HWND win, Dictionary<WideWstring,bool> & lbdict)
{
	int count = ListBox_GetCount(win);
	WideWstring key;
	wchar_t wide[0x400];
	char narrow[0x400];
	bool value;

	if (count) {
		int style = GetWindowLong(win, GWL_STYLE);
		if (style & LBS_MULTIPLESEL) {
			for (int i = 0; i < count; i++) {
				LBGetText(win, i, wide, narrow);
				key = wide;
				if (lbdict.contains(key)) {
					ListBox_SetSel(win, TRUE, i);
				}
			}
		} else if (!(style & LBS_NOSEL)) {
			if (lbdict.removeAny(key, value)) {
				LBSelectText(win, key, wide);
			}
		}
	}
}

bool __cdecl FUN_0077D940(void);
bool __cdecl FUN_0077D970(void);
// FUNCTION: GAMEMD 0x005E6440
int FUN_005E6440(void)
{
	if (Session.Type == GAME_INTERNET) {
		if (FUN_0077D940() && !FUN_0077D970()) {
			return(2);
		}
		return(6);
	}
	return(8);
}

// FUNCTION: GAMEMD 0x005E6470
int FUN_005E6470(void)
{
	int count = FUN_005E6440();
	if (Session.Type == GAME_INTERNET) {
		if (count >= 6) {
			return(count);
		}
	} else if (count >= 8) {
		return(count);
	}
	return(count + 1);
}

// FUNCTION: GAMEMD 0x005E64C0
int FUN_005E64C0(int index)
{
	if (index == -1) {
		return(8);
	}
	int count = RandomMapWaypointCount(index);
	if (Session.MPGameMode != NULL) {
		int limit = Session.MPGameMode->vt_entry_98();
		if (limit != -1 && limit < count) {
			count = limit;
		}
	}
	if (Session.Type == GAME_INTERNET) {
		if (FUN_0077D970()) {
			count &= ~1;
		} else if (FUN_0077D940()) {
			count = 2;
		}
	}
	return(count);
}

// FUNCTION: GAMEMD 0x005E6520
int RandomMapWaypointCount(int index)
{
	char wp[32];
	char buf[132];
	if (index >= 0 && index < Session.Scenarios.Count()) {
		strcpy(buf, Session.Scenarios[index]->Get_Filename());
		CCFileClass file(buf);

		INIClass ini;
		if (!ini.Load(file)) {
			return(0);
		}

		int count = 0;

		for (int i = 0; i < 8; i++) {
			sprintf(wp, "%d", i);
			if (ini.Get_Int("Waypoints", wp, -1) != -1) {
				count++;
			}
		}

		if (count == 0) {
			count = ini.Get_Int("RandomMap", "NumPlayers", 0);
			if (count == 0) {
				count = 8;
			}
		}

		return(count);
	}
	return(0);
}

extern HWND DAT_00AC0D40;
extern int DAT_00AC0D30;
extern int DAT_00AC10E0;
extern wchar_t DAT_00AC0EC8[];
void __fastcall FUN_005E7BF0(int slot);
void __fastcall FUN_005E74E0(HWND dialog);
void FUN_0058BB30(void);
bool FUN_0077F900(void);
int CALLBACK FUN_005E6920(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void FUN_0072D120(void);
void FUN_0072D170(void);

// FUNCTION: GAMEMD 0x005E66D0
int FUN_005E66D0(void)
{
	return(SendDlgItemMessage(DAT_00AC0D40, 0x553, LB_GETCURSEL, 0, 0));
}

bool FUN_005DAF20(void);
bool FUN_005BB260(void);

// FUNCTION: GAMEMD 0x005E66F0
bool FUN_005E66F0(void)
{
	int index = SendDlgItemMessage(DAT_00AC0D40, 0x553, LB_GETCURSEL, 0, 0);
	if (index != -1) {
		MultiMission * data = (MultiMission *)SendDlgItemMessage(DAT_00AC0D40, 0x553, 0x199, index, 0);
		int found = -1;
		for (int i = 0; i < Session.Scenarios.Count(); i++) {
			if (data == Session.Scenarios[i]) {
				found = i;
				break;
			}
		}
		if (found != -1 && found != DAT_00AC0D30) {
			FUN_005E7BF0(found);
			if (Session.Scenarios[found]->Is_Random_Map()) {
				delete MultiplayerMapPreview;
				MultiplayerMapPreview = new MapPreviewClass;
				MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
				if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
					FUN_005E74E0(DAT_00AC0D40);
				}
				InvalidateRect(DAT_00AC0D40, NULL, FALSE);
				FUN_0058BB30();
			} else {
				FUN_005E74E0(DAT_00AC0D40);
			}
			DAT_00AC0D30 = found;
			Session.Options.ScenarioIndex = DAT_00AC10E0;
			FUN_005E7BF0(DAT_00AC10E0);
		}
	}
	HWND list = DAT_00AC0D40;
	int sel = SendDlgItemMessage(DAT_00AC0D40, 0x553, LB_GETCURSEL, 0, 0);
	if (sel != -1) {
		wchar_t * title = (wchar_t *)SendDlgItemMessage(list, 0x553, 0x199, sel, 0);
		if (title != NULL) {
			wcscpy(DAT_00AC0EC8, title);
		} else {
			DAT_00AC0EC8[0] = 0;
		}
	} else {
		DAT_00AC0EC8[0] = 0;
	}
	if (Session.Type == GAME_IPX) {
		return(FUN_005DAF20());
	}
	if (Session.Type == GAME_INTERNET) {
		return(FUN_0077F900());
	}
	if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
		return(FUN_005BB260());
	}
	Call_Back();
	return(false);
}

// FUNCTION: GAMEMD 0x005E68A0
int Scenario_Dialog(HWND top)
{
	FUN_0052FEC0(NULL, true);
	FUN_0072D120();
	DAT_00AC0D40 = WS_Create_Dialog(Game::hInstance, 0x6b, top, FUN_005E6920, FALSE);
	UI::RegisterWindow(DAT_00AC0D40, 0x6b);
	SendMessage(DAT_00AC0D40, 0x4a9, 0, 1);
	ShowWindow(DAT_00AC0D40, SW_NORMAL);
	int result = FUN_007759E0(DAT_00AC0D40, FUN_005E66F0, 0, 1, 1);
	FUN_0072D170();
	return(result);
}

// FUNCTION: GAMEMD 0x005E70D0
int FUN_005E70D0(HWND dialog, MultiMission * map)
{
	if (map == NULL) {
		return(-1);
	}
	int found = 0;
	int count = SendDlgItemMessage(dialog, 0x553, LB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		if (SendDlgItemMessage(dialog, 0x553, 0x199, i, 0) == (LRESULT)map) {
			found = i;
		}
	}
	SendDlgItemMessage(dialog, 0x553, LB_SETCURSEL, found, 0);
	SendDlgItemMessage(dialog, 0x553, 0x197, found, 0);
	return(found);
}

bool FUN_0048D1D0(void);
wchar_t * __fastcall FUN_00735120(char const * narrow);

void __fastcall FUN_007757E0(HWND window, int flag);
bool __fastcall FUN_005DB650(int count, bool full, bool ai);
bool __fastcall FUN_007A2800(int count, bool full, bool ai);
bool __fastcall FUN_006ACCA0(int count);
wchar_t * __fastcall FUN_007B7140(WideWstring * text);

static inline int Find_Map_Index(MultiMission * data)
{
	for (int i = 0; i < Session.Scenarios.Count(); i++) {
		if (data == Session.Scenarios[i]) {
			return(i);
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x005E7160
bool FUN_005E7160(HWND dialog)
{
	int index = -1;
	int selected = SendDlgItemMessage(DAT_00AC0D40, 0x553, LB_GETCURSEL, 0, 0);
	if (selected != -1) {
		MultiMission * data = (MultiMission *)SendDlgItemMessage(DAT_00AC0D40, 0x553, 0x199, selected, 0);
		index = Find_Map_Index(data);
	}
	if (index == -1) {
		return(false);
	}

	HWND list = DAT_00AC0D40;
	MultiplayerGameMode * previous = Session.MPGameMode;
	MultiplayerGameMode * mode = NULL;
	int modeindex = SendDlgItemMessage(DAT_00AC0D40, 0x6eb, LB_GETCURSEL, 0, 0);
	if (modeindex != -1) {
		mode = (MultiplayerGameMode *)SendDlgItemMessage(list, 0x6eb, 0x199, modeindex, 0);
	}
	Session.MPGameMode = mode;
	bool first = mode->vt_entry_40();
	bool second = mode->vt_entry_BC();
	int count = RandomMapWaypointCount(index);
	if (Session.MPGameMode != NULL) {
		int limit = Session.MPGameMode->vt_entry_98();
		if (limit != -1 && limit < count) {
			count = limit;
		}
	}
	if (Session.Type == GAME_INTERNET) {
		if (FUN_0077D970()) {
			count &= ~1;
		} else if (FUN_0077D940()) {
			count = 2;
		}
	}

	Session.MPGameMode = previous;
	bool ai = false;
	bool eject;
	switch (Session.Type) {
		case GAME_IPX:
			eject = FUN_005DB650(count, first, second);
			break;
		case GAME_INTERNET:
			eject = FUN_007A2800(count, first, second);
			break;
		case GAME_SKIRMISH:
			eject = FUN_006ACCA0(count);
			ai = true;
			break;
		default:
			goto apply;
	}
	if (eject) {
#line 2963 "D:\\ra2mdpost\\netshare.cpp"
		wchar_t const * message = StringTable::LoadString(!ai ? "GUI:EjectPlayers" : "GUI:EjectAIPlayers", NULL, __FILE__, __LINE__);
		wchar_t const * ok = StringTable::LoadString("GUI:Ok", NULL, __FILE__, __LINE__);
		wchar_t const * cancel = StringTable::LoadString("GUI:Cancel", NULL, __FILE__, __LINE__);
		if (WWMessageBox().Process(message, ok, cancel, NULL) != 0) {
			return(false);
		}
	}
apply:
	if (mode != Session.MPGameMode) {
		if (Session.MPGameMode != NULL) {
			Session.MPGameMode->vt_entry_9C();
		}
		if (mode != NULL) {
			Session.MPGameMode = mode;
			GameModeOptionsClass::Instance = mode->MPModeIndex;
			Session.Options.ScenarioIndex = index;
			mode->vt_entry_20();
		}
	}
	Session.Options.ScenarioIndex = index;
	if (mode != NULL) {
		mode->vt_entry_24();
	}
	mode->vt_entry_18();
	mode->UnfixAlliances();

	FUN_007757E0(dialog, 1);
	HWND window = FUN_00775940(0xc2);
	if (window == NULL) {
		window = FUN_00775940(0xc9);
		if (window == NULL) {
			window = FUN_00775940(0xbc);
			if (window == NULL) {
				window = FUN_00775940(0xbd);
			}
		}
	}

	if (mode != NULL) {
		HWND item = GetDlgItem(window, 0x6ec);
		SendMessage(item, 0x4b2, 0, (LPARAM)FUN_007B7140(&mode->UIName));
	}
	if (Session.Options.ScenarioIndex >= 0 && Session.Options.ScenarioIndex < Session.Scenarios.Count()) {
		MultiMission * map = Session.Scenarios[Session.Options.ScenarioIndex];
		if (map != NULL) {
			HWND item = GetDlgItem(window, 0x5a8);
			SendMessage(item, 0x4b2, 0, (LPARAM)map);
		}
	}
	mode->vt_entry_60();
	return(true);
}

// FUNCTION: GAMEMD 0x005E7460
void PregameSetup(void)
{
	Scen->Scenario = Session.Options.ScenarioIndex;
	Frame = 0;
	Session.NumPlayers = Session.Players.Count();
	FUN_004068E0("Pregame setup for %d players.\n", Session.NumPlayers);
	Options.GameSpeed = Session.Options.GameSpeed;
	Session.CommProtocol = (CommProtocolType)2;
	Special.IsHarvesterImmune = Session.Options.HarvTruce;
	Special.IsDestroyBridges = Session.Options.BridgeDestruction;
	Special.IsTGrowth = true;
	Special.IsTSpread = true;
	Special.Apply_To_Game();
}

bool __fastcall FUN_006951F0(char const * name, unsigned int length, char const * digest, bool official, bool flag);
void FUN_0058BB30(void);
void * FUN_007931A0(void);

static inline int Send_To_Host(void * packet, int size, int sent)
{
	if (Session.Type == GAME_IPX) {
		sent = Ipx.Send_Global_Message(packet, size, 1, &Session.HostAddress);
	} else {
		void * owner = FUN_007931A0();
		if (owner != NULL) {
			sent = Ipx.Send_Global_Message_To_Name(packet, size, 1, FUN_00735120((char *)owner + 0x24));
		}
	}
	return(sent);
}

bool __fastcall FUN_00693B70(char * name, int type, bool flag);

// FUNCTION: GAMEMD 0x005E7920
void Receive_Random_Map_Preview(void)
{
	Ipx.Set_Timing(50, -1, 5000);
	FUN_004068E0("Starting map preview download\n");

	GlobalPacketType packet;
	memset(&packet, 0, sizeof(packet));
	packet.Command = NET_PREVIEW_ACK;
	FUN_004068E0("Sending preview mode acks\n");
	while (true) {
		if (Send_To_Host(&packet, sizeof(packet), 1)) {
			break;
		}
		Call_Back();
	}
	while (Ipx.Global_Num_Send() != 0) {
		Call_Back();
	}
	FUN_004068E0("Preview mode acks sent\n");

	Session.GAddress = Session.HostAddress;
	FUN_004068E0("Calling Get_File_From_Host to receive the file download\n");
	char preview_name[256];
	bool got_file = FUN_00693B70(preview_name, GAME_IPX, false);
	if (!got_file) {
		FUN_004068E0("got_file is false. Download failed\n");
		Ipx.Set_Timing(30, -1, 600);
		return;
	}

	FUN_004068E0("Loading the compressed preview image\n");
	RawFileClass file(preview_name);
	int size = file.Size();
	char * buffer = new char[size];
	file.Read(buffer, size);
	int preview_size = ((int *)buffer)[0];

	FUN_004068E0("Decompressing the preview image\n");

	BufferStraw bstraw(&((int *)buffer)[1], size);
	LZOStraw lzostraw(LZOStraw::DECOMPRESS);
	lzostraw.Get_From(&bstraw);
	char * preview = new char[2 * preview_size];
	lzostraw.Get(preview, preview_size);

	FUN_004068E0("Creating the new preview surface\n");
	if (MultiplayerMapPreview) {
		delete MultiplayerMapPreview;
	}
	MultiplayerMapPreview = new MapPreviewClass;
	MultiplayerMapPreview->Create_Preview_Surface(preview);
	InvalidateRect(FUN_00775B10(), NULL, FALSE);

	FUN_004068E0("Cleaning up the temporary decompression buffers\n");
	delete [] preview;
	delete [] buffer;

	Ipx.Set_Timing(30, -1, 600);
}

extern int DAT_0081C1D0;
int __fastcall FUN_004790A0(int timeout);

// FUNCTION: GAMEMD 0x005E7BF0
bool Set_Scenario_Info_From_Index(int index)
{
	if (index == -1) {
		Session.ScenarioFileName[0] = '\0';
		Session.ScenarioDigest[0] = '\0';
		Session.ScenarioFileLength = 0;
		Session.ScenarioIsOfficial = 0;
		Session.unknown_8D4 = 0;
		Session.Options.ScenarioDescription[0] = L'\0';
		return(false);
	}

	bool unavailable = CCFileClass(Session.Scenarios[index]->Get_Filename()).Is_Available() == false;

	if (unavailable) {
		if (DAT_0081C1D0 != -2) {
			if (!Session.Scenarios[index]->Is_On_CD((DiskID)FUN_004790A0(60)) && !CD().Force_Available(Session.Scenarios[index]->On_Which_CD())) {
				return(false);
			}
		} else {
#line 3627 "D:\\ra2mdpost\\netshare.cpp"
			WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_READ_SCENARIO", NULL, __FILE__, __LINE__), StringTable::LoadString("TXT_OK", NULL, __FILE__, __LINE__), NULL, NULL);
			return(false);
		}
	}

	wcscpy(Session.Options.ScenarioDescription, Session.Scenarios[index]->Description());
	strcpy(Session.ScenarioDigest, Session.Scenarios[index]->Get_Digest());
	strcpy(Session.ScenarioFileName, Session.Scenarios[index]->Get_Filename());
	strcpy(Scen->ScenarioName, Session.ScenarioFileName);
	Session.ScenarioIsOfficial = Session.Scenarios[index]->Get_Official();
	Session.unknown_8D4 = FUN_005E64C0(index);
	Session.ScenarioFileLength = CCFileClass(Session.ScenarioFileName).Size();
	Session.ScenarioFileLength = CCFileClass(Session.ScenarioFileName).Size();
	return(true);
}

// FUNCTION: GAMEMD 0x005E8590
int CreateRandomMap(void)
{
	int result;
	if (Session.Type == GAME_IPX) {
		result = Do_Random_Map_Dialog(FUN_005DAF20);
	} else if (Session.Type == GAME_INTERNET) {
		result = Do_Random_Map_Dialog(FUN_0077F900);
	} else if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
		result = Do_Random_Map_Dialog(FUN_005BB260);
	} else {
		result = Do_Random_Map_Dialog(FUN_0048D1D0);
	}

	if (result != 1) {
		return(-1);
	}

	IsRandomMap = true;
	RandomMapGen.SeedData.Save("RandMap.Sed");

	delete MultiplayerMapPreview;
	MultiplayerMapPreview = new MapPreviewClass;
	MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");

	int index;
	bool found = false;
	for (index = 0; index < Session.Scenarios.Count(); index++) {
		if (Session.Scenarios[index]->Is_Random_Map()) {
			found = true;
			break;
		}
	}

	if (!found) {
		char * digest = CalcRandomMapDigest();
		MultiMission * map = new MultiMission("RandMap.Sed", RandomMapGen.SeedData.MapDescription, digest, true, NULL, 2, 4);
		Session.Scenarios.Add(map);
		delete digest;
	} else {
		Session.Scenarios[index]->Set_Description(RandomMapGen.SeedData.MapDescription);
		MultiMission ** entry = &Session.Scenarios[index];
		(*entry)->Set_Digest(CalcRandomMapDigest());
	}

	return(index);
}

// FUNCTION: GAMEMD 0x005E8720
int __fastcall FUN_005E8720(wchar_t const * name)
{
	return(wcscmp(name, Session.GameName) == 0);
}

// FUNCTION: GAMEMD 0x005E8740
int __fastcall FUN_005E8740(int index)
{
	return(wcscmp(Session.Players[index]->Name, Session.GameName) == 0);
}

// FUNCTION: GAMEMD 0x005E8760
int FUN_005E8760(void)
{
	wchar_t * name = FUN_00735120(Session.Handle);
	return(wcscmp(name, Session.GameName) == 0);
}

// FUNCTION: GAMEMD 0x005E8780
int __fastcall FUN_005E8780(wchar_t const * name)
{
	return(wcscmp(name, FUN_00735120(Session.Handle)) == 0);
}

extern wchar_t const * DAT_00AC117C;
extern wchar_t const * DAT_00AC1180;
extern wchar_t const * DAT_00AC1184;
extern wchar_t const * DAT_00AC1188;
extern wchar_t const * DAT_00AC118C;
extern wchar_t const * DAT_00AC1190;
extern wchar_t const * DAT_00AC1194;
extern NodeNameType * DAT_00AC119C;
extern NodeNameType * DAT_00AC11A0;
extern NodeNameType * DAT_00AC11A4;
extern NodeNameType * DAT_00AC11A8;
extern NodeNameType * DAT_00AC11AC;
extern NodeNameType * DAT_00AC11B0;
extern NodeNameType * DAT_00AC11B4;

// FUNCTION: GAMEMD 0x005E87A0
void FUN_005E87A0(void)
{
	if (DAT_00AC117C == NULL) {
#line 4078 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC117C = StringTable::LoadString("GUI:Open", NULL, __FILE__, __LINE__);
#line 4079 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC1180 = StringTable::LoadString("GUI:Closed", NULL, __FILE__, __LINE__);
#line 4080 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC1184 = StringTable::LoadString("GUI:AIEasy", NULL, __FILE__, __LINE__);
#line 4081 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC1188 = StringTable::LoadString("GUI:AINormal", NULL, __FILE__, __LINE__);
#line 4082 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC118C = StringTable::LoadString("GUI:AIHard", NULL, __FILE__, __LINE__);
#line 4083 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC1190 = StringTable::LoadString("GUI:Waiting", NULL, __FILE__, __LINE__);
#line 4084 "D:\\ra2mdpost\\netshare.cpp"
		DAT_00AC1194 = StringTable::LoadString("GUI:OpenObserver", NULL, __FILE__, __LINE__);
	}
	if (DAT_00AC119C == NULL) {
		DAT_00AC119C = new NodeNameType;
		wcscpy(DAT_00AC119C->Name, DAT_00AC117C);
	}
	if (DAT_00AC11A0 == NULL) {
		DAT_00AC11A0 = new NodeNameType;
		wcscpy(DAT_00AC11A0->Name, DAT_00AC1180);
	}
	if (DAT_00AC11A4 == NULL) {
		DAT_00AC11A4 = new NodeNameType;
		wcscpy(DAT_00AC11A4->Name, DAT_00AC1184);
	}
	if (DAT_00AC11A8 == NULL) {
		DAT_00AC11A8 = new NodeNameType;
		wcscpy(DAT_00AC11A8->Name, DAT_00AC1188);
	}
	if (DAT_00AC11AC == NULL) {
		DAT_00AC11AC = new NodeNameType;
		wcscpy(DAT_00AC11AC->Name, DAT_00AC118C);
	}
	if (DAT_00AC11B0 == NULL) {
		DAT_00AC11B0 = new NodeNameType;
		wcscpy(DAT_00AC11B0->Name, DAT_00AC1190);
	}
	if (DAT_00AC11B4 == NULL) {
		DAT_00AC11B4 = new NodeNameType;
		wcscpy(DAT_00AC11B4->Name, DAT_00AC1194);
	}
}


void __fastcall FUN_0069B540(GameOptionsType * options);
extern wchar_t DAT_00AC0D44[];
extern wchar_t DAT_00AC0B94[];
extern char DAT_00AC0AC8[];
extern char DAT_00AC0AEC[];
extern char DAT_00AC0EA4[];
extern char DAT_00AC0B10[];

// FUNCTION: GAMEMD 0x005E8A20
void FUN_005E8A20(void)
{
	if (DAT_00AC119C != NULL) {
		delete DAT_00AC119C;
		DAT_00AC119C = NULL;
	}
	if (DAT_00AC11A0 != NULL) {
		delete DAT_00AC11A0;
		DAT_00AC11A0 = NULL;
	}
	if (DAT_00AC11A4 != NULL) {
		delete DAT_00AC11A4;
		DAT_00AC11A4 = NULL;
	}
	if (DAT_00AC11A8 != NULL) {
		delete DAT_00AC11A8;
		DAT_00AC11A8 = NULL;
	}
	if (DAT_00AC11AC != NULL) {
		delete DAT_00AC11AC;
		DAT_00AC11AC = NULL;
	}
	if (DAT_00AC11B0 != NULL) {
		delete DAT_00AC11B0;
		DAT_00AC11B0 = NULL;
	}
	if (DAT_00AC11B4 != NULL) {
		delete DAT_00AC11B4;
		DAT_00AC11B4 = NULL;
	}
	Clear_Vector(&Session.Players);
}

// FUNCTION: GAMEMD 0x005E8AE0
void FUN_005E8AE0(void)
{
	int count = FUN_005E6440();
	int i;
	for (i = 0; i < count; i++) {
		Session.Slots[i] = DAT_00AC119C;
	}
	for (i = count; i < 8; i++) {
		Session.Slots[i] = DAT_00AC11A0;
	}
}

// FUNCTION: GAMEMD 0x005E8B40
void FUN_005E8B40(void)
{
	DAT_00AC0AC8[0] = 0;
	DAT_00AC0AEC[0] = 0;
	DAT_00AC0EA4[0] = 0;
	DAT_00AC0B10[0] = 0;
}

// FUNCTION: GAMEMD 0x005E8B60
void FUN_005E8B60(void)
{
	int count = FUN_005E6440();
	int i;
	for (i = 0; i < count; i++) {
		Session.Slots[i] = DAT_00AC119C;
	}
	for (i = count; i < 8; i++) {
		Session.Slots[i] = DAT_00AC11A0;
	}
	Session.unknown_2854 = -1;
	FUN_0069B540(&Session.Options);
	DAT_00AC0D44[0] = 0;
	DAT_00AC0B94[0] = 0;
}

// FUNCTION: GAMEMD 0x005E8BF0
NodeNameType * __fastcall FUN_005E8BF0(wchar_t const * name)
{
	if (wcscmp(DAT_00AC119C->Name, name) == 0) {
		return(DAT_00AC119C);
	}
	if (wcscmp(DAT_00AC11A0->Name, name) == 0) {
		return(DAT_00AC11A0);
	}
	if (wcscmp(DAT_00AC11A4->Name, name) == 0) {
		return(DAT_00AC11A4);
	}
	if (wcscmp(DAT_00AC11A8->Name, name) == 0) {
		return(DAT_00AC11A8);
	}
	if (wcscmp(DAT_00AC11AC->Name, name) == 0) {
		return(DAT_00AC11AC);
	}
	if (wcscmp(DAT_00AC11B0->Name, name) == 0) {
		return(DAT_00AC11B0);
	}
	if (wcscmp(DAT_00AC11B4->Name, name) == 0) {
		return(DAT_00AC11B4);
	}
	int count = Session.Players.Count();
	for (int i = 0; i < count; i++) {
		NodeNameType * node = Session.Players[i];
		if (wcscmp(node->Name, name) == 0) {
			return(node);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x005E8D00
int __fastcall FUN_005E8D00(wchar_t const * name)
{
	for (int i = 0; i < 8; i++) {
		if (Session.Slots[i] != NULL && wcsicmp(name, Session.Slots[i]->Name) == 0) {
			return(i);
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x005E8D40
int FUN_005E8D40(void)
{
	if (!Session.Players.Count()) {
		return(-1);
	}
	return(FUN_005E8D00(Session.Players[0]->Name));
}

// FUNCTION: GAMEMD 0x005E8D90
void __fastcall FUN_005E8D90(int slot, NodeNameType * name)
{
	Session.Slots[slot] = name;
}

// FUNCTION: GAMEMD 0x005E8DA0
void __fastcall FUN_005E8DA0(int slot)
{
	Session.unknown_2854 = slot;
	Session.Slots[slot] = Session.Players[0];
}

extern wchar_t DAT_008304FC[];
Wstring __fastcall FUN_005EDEF0(NodeNameType * slot);
WideWstring __fastcall FUN_007B7ED0(Wstring const & text);

// FUNCTION: GAMEMD 0x005E8DC0
void __fastcall FUN_005E8DC0(wchar_t * buffer)
{
	buffer[0] = 0;
	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		WideWstring name;
		if (slot == NULL || slot == DAT_00AC119C || slot == DAT_00AC11A0 || slot == DAT_00AC11B0 || slot == DAT_00AC11B4 || slot == DAT_00AC11A4 || slot == DAT_00AC11A8 || slot == DAT_00AC11AC) {
			Wstring raw = FUN_005EDEF0(slot);
			name = FUN_007B7ED0(raw);
		} else {
			name = slot->Name;
		}
		wcscat(buffer, name.get());
		wcscat(buffer, DAT_008304FC);
	}
}

int __fastcall FUN_004E37D0(int side);
int __fastcall FUN_004E41D0(int side);
int __fastcall FUN_004E4170(HWND dialog, int id, int flag);
void __fastcall FUN_004E3A00(HWND dialog, int id);
void __fastcall FUN_004E3B00(HWND dialog, int id);
void __fastcall FUN_004E45A0(HWND dialog, int id);
void __fastcall FUN_004E4770(HWND dialog, int id);
void __fastcall FUN_004E49A0(HWND dialog, int id, int flag);
NodeNameType * __fastcall FUN_005EDFC0(WideWstring * name);

// FUNCTION: GAMEMD 0x005E8EA0
void __fastcall FUN_005E8EA0(wchar_t * list)
{
	wchar_t text[170];
	bool observer[8];
	int i;

	if (list[0] == 0) {
		return;
	}
	wcscpy(text, list);
	NodeNameType * obs = DAT_00AC11B4;
	NodeNameType * wait = DAT_00AC11B0;
	NodeNameType * open = DAT_00AC119C;
	NodeNameType * closed = DAT_00AC11A0;
	for (i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		observer[i] = (slot == obs || (slot != NULL && slot != open && slot != closed && slot != wait && slot->Slot.SpectatorFlag == -1));
	}
	FUN_005E8AE0();

	i = 0;
	wchar_t * token = wcstok(text, DAT_008304FC);
	while (token != NULL) {
		NodeNameType * node;
		{
			WideWstring wide(token);
			node = FUN_005EDFC0(&wide);
		}
		if (node == NULL) {
			node = FUN_005E8BF0(token);
			if (node == NULL) {
				node = DAT_00AC11B0;
			}
		}
		if (node == DAT_00AC11B4 || (node != NULL && node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B0 && node != DAT_00AC11A4 && node != DAT_00AC11A8 && node != DAT_00AC11AC && node->Slot.SpectatorFlag == -1)) {
			HWND dialog = FUN_00775940(0xc2);
			if (dialog == NULL) {
				dialog = FUN_00775940(0xc9);
				if (dialog == NULL) {
					dialog = FUN_00775940(0xbc);
					if (dialog == NULL) {
						dialog = FUN_00775940(0xbd);
					}
				}
			}
			if (!observer[i]) {
				Session.Slots[i] = node;
				FUN_004E3B00(dialog, FUN_004E37D0(i));
				int id;
				if (i == 0) {
					id = 0x411;
				} else if (i == 1) {
					id = 0x412;
				} else if (i == 2) {
					id = 0x413;
				} else if (i == 3) {
					id = 0x414;
				} else if (i == 4) {
					id = 0x415;
				} else if (i == 5) {
					id = 0x416;
				} else if (i == 6) {
					id = 0x417;
				} else if (i == 7) {
					id = 0x418;
				} else {
					id = -1;
				}
				SendMessage(GetDlgItem(dialog, id), 0x4f1, 0, 1);
				int combo = FUN_004E41D0(i);
				FUN_004E4770(dialog, combo);
				FUN_004E49A0(dialog, combo, 8);
			}
		} else {
			HWND dialog = FUN_00775940(0xc2);
			if (dialog == NULL) {
				dialog = FUN_00775940(0xc9);
				if (dialog == NULL) {
					dialog = FUN_00775940(0xbc);
					if (dialog == NULL) {
						dialog = FUN_00775940(0xbd);
					}
				}
			}
			if (observer[i]) {
				int combo = FUN_004E37D0(i);
				if (FUN_004E4170(dialog, combo, -1) == -3) {
					FUN_004E3A00(dialog, combo);
					int id;
					if (i == 0) {
						id = 0x411;
					} else if (i == 1) {
						id = 0x412;
					} else if (i == 2) {
						id = 0x413;
					} else if (i == 3) {
						id = 0x414;
					} else if (i == 4) {
						id = 0x415;
					} else if (i == 5) {
						id = 0x416;
					} else if (i == 6) {
						id = 0x417;
					} else if (i == 7) {
						id = 0x418;
					} else {
						id = -1;
					}
					SendMessage(GetDlgItem(dialog, id), 0x4f1, 0, 0);
					combo = FUN_004E41D0(i);
					FUN_004E45A0(dialog, combo);
					FUN_004E49A0(dialog, combo, -2);
				}
			}
		}
		Session.Slots[i] = node;
		i++;
		token = wcstok(NULL, DAT_008304FC);
	}
}

// FUNCTION: GAMEMD 0x005E9240
int __fastcall FUN_005E9240(NodeNameType * node)
{
	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		if ((slot == DAT_00AC119C && node->Slot.SpectatorFlag != -1) || (slot == DAT_00AC11B4 && node->Slot.SpectatorFlag == -1)) {
			Session.Slots[i] = node;
			return(i);
		}
	}
	return(-1);
}

extern int DAT_008316A4;

// FUNCTION: GAMEMD 0x005E9290
void __fastcall FUN_005E9290(IPXAddressClass * address)
{
	int limit;

	if (Session.Type == GAME_INTERNET) {
		if (FUN_0077D940() && !FUN_0077D970()) {
			limit = 2;
		} else {
			limit = 6;
		}
	} else {
		limit = 8;
	}
	if (!DAT_008316A4 && limit < 7) {
		limit++;
	}
	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		if (slot->Address == *address) {
			if (Session.Slots[i]->Slot.SpectatorFlag == -1) {
				Session.Slots[i] = DAT_00AC11B4;
			} else {
				NodeNameType * open = DAT_00AC119C;
				if (i >= limit) {
					open = DAT_00AC11A0;
				}
				Session.Slots[i] = open;
			}
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x005E9340
void __fastcall FUN_005E9340(char const * name)
{
	int limit;

	if (Session.Type == GAME_INTERNET) {
		if (FUN_0077D940() && !FUN_0077D970()) {
			limit = 2;
		} else {
			limit = 6;
		}
	} else {
		limit = 8;
	}
	if (!DAT_008316A4 && limit < 7) {
		limit++;
	}
	for (int i = 0; i < 8; i++) {
		if (Session.Slots[i] && stricmp(name, FUN_007350C0(Session.Slots[i]->Name)) == 0) {
			if (Session.Slots[i]->Slot.SpectatorFlag == -1) {
				Session.Slots[i] = DAT_00AC11B4;
			} else {
				NodeNameType * open = DAT_00AC119C;
				if (i >= limit) {
					open = DAT_00AC11A0;
				}
				Session.Slots[i] = open;
			}
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x005E93F0
int __fastcall FUN_005E93F0(NodeNameType * slot)
{
	if (slot == NULL || slot == DAT_00AC119C || slot == DAT_00AC11A0 || slot == DAT_00AC11B0 || slot == DAT_00AC11B4 || slot == DAT_00AC11A4 || slot == DAT_00AC11A8 || slot == DAT_00AC11AC) {
		return(0);
	}
	return(1);
}

// FUNCTION: GAMEMD 0x005E9440
int __fastcall FUN_005E9440(NodeNameType * slot)
{
	if (slot != NULL && (slot == DAT_00AC11A4 || slot == DAT_00AC11A8 || slot == DAT_00AC11AC)) {
		return(1);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x005E9470
int __fastcall FUN_005E9470(NodeNameType * slot)
{
	if (slot == NULL || slot == DAT_00AC119C || slot == DAT_00AC11A0 || slot == DAT_00AC11B4 || slot == DAT_00AC11B0) {
		return(0);
	}
	return(1);
}

// FUNCTION: GAMEMD 0x005E94A0
int __fastcall FUN_005E94A0(NodeNameType * slot)
{
	int result = -1;

	if (slot == DAT_00AC11A4) {
		return(2);
	}
	if (slot == DAT_00AC11A8) {
		return(1);
	}
	if (slot == DAT_00AC11AC) {
		result = 0;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005E94D0
int FUN_005E94D0(void)
{
	NodeNameType * ai0 = DAT_00AC11A4;
	NodeNameType * ai1 = DAT_00AC11A8;
	NodeNameType * ai2 = DAT_00AC11AC;
	int count = 0;

	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		if (slot && (slot == ai0 || slot == ai1 || slot == ai2)) {
			count++;
		}
	}
	return(count);
}

// FUNCTION: GAMEMD 0x005E9510
bool FUN_005E9510(void)
{
	int count = (Session.MaxPlayers < Session.unknown_8D4) ? Session.MaxPlayers : Session.unknown_8D4;
	int i;

	for (i = 0; i < count; i++) {
		if (Session.Slots[i] == DAT_00AC119C) {
			return(false);
		}
	}
	if (!DAT_008316A4 && count < 8 && Session.Slots[count] == DAT_00AC119C) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005E9560
bool FUN_005E9560(void)
{
	for (int i = 0; i < 8; i++) {
		if (Session.Slots[i] == DAT_00AC11B4) {
			return(false);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x005E9580
bool FUN_005E9580(void)
{
	int index = DAT_008316A4;
	NodeNameType * open = DAT_00AC119C;
	NodeNameType * closed = DAT_00AC11A0;

	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		if (slot && slot != open && slot != closed && slot != DAT_00AC11B4 && slot != DAT_00AC11B0 && i == index && slot->Slot.SpectatorFlag == -1) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005E95E0
bool FUN_005E95E0(void)
{
	int count = (Session.MaxPlayers < Session.unknown_8D4) ? Session.MaxPlayers : Session.unknown_8D4;

	int players = Session.Players.Count();

	if (players - FUN_005E9580() > count) {
		FUN_004068E0("Channel full - Session.ScenarioMaxPlayers = %d\n", Session.unknown_8D4);
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005E96A0
bool __fastcall FUN_005E96A0(int id)
{
	if (id != -2) {
		for (int i = 0; i < 8; i++) {
			NodeNameType * slot = Session.Slots[i];
			if (FUN_005E93F0(slot) && slot->Get_Color() == id) {
				return(true);
			}
		}
		for (int j = 0; j < 8; j++) {
			if (Session.Options.AISlots.Colors[j] == id) {
				return(true);
			}
		}
	}
	return(false);
}


// FUNCTION: GAMEMD 0x005E9720
bool __fastcall FUN_005E9720(int id)
{
	if (id != -2) {
		for (int i = 0; i < 8; i++) {
			NodeNameType * slot = Session.Slots[i];
			if (FUN_005E93F0(slot) && slot->Get_Start() == id) {
				return(true);
			}
		}
		for (int j = 0; j < 8; j++) {
			if (Session.Options.AISlots.Starts[j] == id) {
				return(true);
			}
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x005E97A0
int __fastcall FUN_005E97A0(NodeNameType * slot)
{
	if (slot == DAT_00AC119C) return(2);
	if (slot == DAT_00AC11A0) return(3);
	if (slot == DAT_00AC11A4) return(4);
	if (slot == DAT_00AC11A8) return(5);
	if (slot == DAT_00AC11AC) return(6);
	if (slot == DAT_00AC11B4) return(7);
	return(2);
}

// FUNCTION: GAMEMD 0x005E9800
NodeNameType * __fastcall FUN_005E9800(int kind)
{
	if (kind == 2) return(DAT_00AC119C);
	if (kind == 3) return(DAT_00AC11A0);
	if (kind == 4) return(DAT_00AC11A4);
	if (kind == 5) return(DAT_00AC11A8);
	if (kind == 6) return(DAT_00AC11AC);
	if (kind == 7) return(DAT_00AC11B4);
	return(NULL);
}

// FUNCTION: GAMEMD 0x005E9850
void FUN_005E9850(void)
{
	if (FUN_00775B20() != 0xbc) {
		HWND window = FUN_00775940(0xc2);
		if (!window) {
			window = FUN_00775940(0xc9);
			if (!window) {
				window = FUN_00775940(0xbc);
				if (!window) {
					window = FUN_00775940(0xbd);
				}
			}
		}
		EnableWindow(GetDlgItem(window, 0x59f), TRUE);
	}
	int count = Session.Players.Count();
	for (int i = 0; i < count; i++) {
		if (wcscmp(Session.Players[i]->Name, Session.GameName) != 0 && Session.Players[i]->Slot.Unknown77) {
			Session.Players[i]->Slot.Unknown77 = 0;
		}
	}
}

// FUNCTION: GAMEMD 0x005E98F0
int __fastcall FUN_005E98F0(int kind)
{
	if (kind == 0) return(0x411);
	if (kind == 1) return(0x412);
	if (kind == 2) return(0x413);
	if (kind == 3) return(0x414);
	if (kind == 4) return(0x415);
	if (kind == 5) return(0x416);
	if (kind == 6) return(0x417);
	if (kind == 7) return(0x418);
	return(-1);
}

// FUNCTION: GAMEMD 0x005E9950
int __fastcall FUN_005E9950(int id)
{
	if (id == 0x411) return(0);
	if (id == 0x412) return(1);
	if (id == 0x413) return(2);
	if (id == 0x414) return(3);
	if (id == 0x415) return(4);
	if (id == 0x416) return(5);
	if (id == 0x417) return(6);
	if (id == 0x418) return(7);
	return(-1);
}

// FUNCTION: GAMEMD 0x005E99C0
wchar_t const * __fastcall FUN_005E99C0(NodeNameType * slot)
{
	if (slot == DAT_00AC119C) {
#line 4948 "D:\\ra2mdpost\\netshare.cpp"
		return(StringTable::LoadString("STT:PlayerOpen", NULL, __FILE__, __LINE__));
	}
	if (slot == DAT_00AC11A0) {
#line 4950 "D:\\ra2mdpost\\netshare.cpp"
		return(StringTable::LoadString("STT:PlayerClosed", NULL, __FILE__, __LINE__));
	}
	if (slot == DAT_00AC11A4) {
#line 4952 "D:\\ra2mdpost\\netshare.cpp"
		return(StringTable::LoadString("STT:PlayerDumbAI", NULL, __FILE__, __LINE__));
	}
	if (slot == DAT_00AC11A8) {
#line 4954 "D:\\ra2mdpost\\netshare.cpp"
		return(StringTable::LoadString("STT:PlayerSmartAI", NULL, __FILE__, __LINE__));
	}
	if (slot == DAT_00AC11AC) {
#line 4956 "D:\\ra2mdpost\\netshare.cpp"
		return(StringTable::LoadString("STT:PlayerGeniusAI", NULL, __FILE__, __LINE__));
	}
	if (slot == DAT_00AC11B4) {
#line 4958 "D:\\ra2mdpost\\netshare.cpp"
		return(StringTable::LoadString("STT:PlayerOpenObserver", NULL, __FILE__, __LINE__));
	}
#line 4960 "D:\\ra2mdpost\\netshare.cpp"
	return(StringTable::LoadString("STT:PlayerHuman", NULL, __FILE__, __LINE__));
}

// FUNCTION: GAMEMD 0x005E9AA0
void __fastcall FUN_005E9AA0(HWND dialog, int id, int data)
{
	HWND item = GetDlgItem(dialog, id);
	ShowWindow(item, SW_HIDE);
	int count = SendDlgItemMessage(dialog, id, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		if (SendDlgItemMessage(dialog, id, CB_GETITEMDATA, i, 0) == data) {
			SendDlgItemMessage(dialog, id, CB_SETCURSEL, i, 0);
			break;
		}
	}
	ShowWindow(item, SW_SHOW);
}

// FUNCTION: GAMEMD 0x005E9B20
int __fastcall FUN_005E9B20(HWND dialog, int id, int index)
{
	if (index == -1) {
		index = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
	}
	int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, index, 0);
	return((data == -1) ? 0 : data);
}

int __fastcall FUN_004E3830(int id);
int __fastcall FUN_004E4E20(HWND dialog, int id, int flag);
int __fastcall FUN_004E4230(int id);
void __fastcall FUN_004E4C20(HWND dialog, int id);
int __fastcall FUN_004E5900(HWND dialog, int id, int flag);
int __fastcall FUN_004E4EC0(int id);
void __fastcall FUN_004E5700(HWND dialog, int id);

// FUNCTION: GAMEMD 0x005E9B60
void __fastcall FUN_005E9B60(HWND dialog, int combo)
{
	int country = FUN_004E4170(dialog, combo, -1);
	int slot = FUN_004E3830(combo);
	if (slot == Session.unknown_2854) {
		Session.FUN_0069B760(country, false);
		if (Session.Players.Count()) {
			Session.Players[0]->Set_Country(country);
		}
		return;
	}
	NodeNameType * node = Session.Slots[slot];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		Session.Options.AISlots.Countries[slot] = country;
		return;
	}
	Session.Options.AISlots.Countries[slot] = -1;
	Session.Options.AISlots.Difficulties[slot] = -1;
	Session.Options.AISlots.Colors[slot] = -1;
}

// FUNCTION: GAMEMD 0x005E9C00
void __fastcall FUN_005E9C00(HWND dialog, int combo)
{
	int color = FUN_004E4E20(dialog, combo, -1);
	int slot = FUN_004E4230(combo);
	if (slot == Session.unknown_2854) {
		FUN_004E4C20(dialog, combo);
		if (FUN_005E96A0(color)) {
			color = -2;
		}
		Session.FUN_0069B7E0(color, false);
		if (Session.Players.Count()) {
			Session.Players[0]->Set_Color(color);
		}
		return;
	}
	Session.Options.AISlots.Colors[slot] = color;
}

// FUNCTION: GAMEMD 0x005E9CE0
void __fastcall FUN_005E9CE0(HWND dialog, int combo)
{
	int start = FUN_004E5900(dialog, combo, -1);
	int slot = FUN_004E4EC0(combo);
	if (slot == Session.unknown_2854) {
		FUN_004E5700(dialog, combo);
		if (FUN_005E9720(start)) {
			start = -2;
		}
		if (Session.Players.Count()) {
			Session.Players[0]->Set_Start(start);
		}
		return;
	}
	Session.Options.AISlots.Starts[slot] = start;
}

void __fastcall FUN_005EB060(HWND dialog);
void FUN_007A0E50(void);
void __fastcall FUN_005EAB30(char * buffer);
void __fastcall FUN_005EAC00(char * buffer);
void __fastcall FUN_005EACD0(char * buffer);
void __fastcall FUN_005EAEE0(HWND dialog);
void __fastcall FUN_005EADA0(HWND dialog);
void __fastcall FUN_005EAE40(HWND dialog);
extern char DAT_00AC0B6C[];
extern char DAT_00AC0A9C[];
extern char DAT_00AC0B48[];
extern char DAT_00AC0278[];
int __fastcall FUN_004E3320(int index);
int __fastcall FUN_004E3560(int index);
int __fastcall FUN_005EA920(int index);
int __fastcall FUN_005EA9B0(int index);
int __fastcall FUN_005EAA10(int index);
int __fastcall FUN_005EAA70(int index);
int __fastcall FUN_004E6030(HWND dialog, int id, int flag);
int __fastcall FUN_004E59A0(int id);
void __fastcall FUN_004E5ED0(HWND dialog, int id, int value);
int __fastcall FUN_004E5940(int id);
int __fastcall FUN_004E4E60(int id);
void __fastcall FUN_004E3F70(HWND dialog, int id, int value);
void __fastcall FUN_004E5480(HWND dialog, int id, int value);

// FUNCTION: GAMEMD 0x005E9DB0
void __fastcall FUN_005E9DB0(HWND dialog, int combo)
{
	int value = FUN_004E6030(dialog, combo, -1);
	int slot = FUN_004E59A0(combo);
	if (slot == Session.unknown_2854) {
		FUN_004E5ED0(dialog, combo, value);
		if (Session.Players.Count()) {
			Session.Players[0]->Set_Team(value);
		}
		return;
	}
	Session.Options.AISlots.Allies[slot] = value;
}

// FUNCTION: GAMEMD 0x005E9E00
void __fastcall FUN_005E9E00(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int combo = FUN_004E5940(i);
		int value = FUN_004E6030(dialog, combo, -1);
		int slot = FUN_004E59A0(combo);
		if (slot == Session.unknown_2854) {
			FUN_004E5ED0(dialog, combo, value);
			if (Session.Players.Count()) {
				Session.Players[0]->Set_Team(value);
			}
		} else {
			Session.Options.AISlots.Allies[slot] = value;
		}
	}
}

// FUNCTION: GAMEMD 0x005E9E70
void __fastcall FUN_005E9E70(HWND dialog, int slot, bool enable)
{
	int country = FUN_004E37D0(slot);
	int color = FUN_004E41D0(slot);
	int start = FUN_004E4E60(slot);
	int team = FUN_004E5940(slot);

	if (DAT_008316A4 >= 0 && slot == DAT_008316A4 && (Session.Slots[slot] == DAT_00AC11B4 || slot == 0 || Session.Slots[slot]->Slot.SpectatorFlag == -1)) {
		FUN_004E3F70(dialog, country, -3);
		FUN_004E49A0(dialog, color, -2);
		FUN_004E5480(dialog, start, -2);
		FUN_004E5ED0(dialog, team, -2);
		HWND item = GetDlgItem(dialog, country);
		EnableWindow(item, 0);
		item = GetDlgItem(dialog, color);
		EnableWindow(item, 0);
		ShowWindow(item, SW_HIDE);
		item = GetDlgItem(dialog, start);
		EnableWindow(item, 0);
		ShowWindow(item, SW_HIDE);
		item = GetDlgItem(dialog, team);
		EnableWindow(item, 0);
		ShowWindow(item, SW_HIDE);
		return;
	}
	if (!enable) {
		FUN_004E3F70(dialog, country, -2);
		FUN_004E49A0(dialog, color, -2);
		FUN_004E5480(dialog, start, -2);
		FUN_004E5ED0(dialog, team, -2);
	}
	HWND countryItem = GetDlgItem(dialog, country);
	EnableWindow(countryItem, enable);
	HWND colorItem = GetDlgItem(dialog, color);
	EnableWindow(colorItem, enable);
	HWND startItem = GetDlgItem(dialog, start);
	EnableWindow(startItem, enable);
	HWND teamItem = GetDlgItem(dialog, team);
	if (enable && Session.MPGameMode && !Session.MPGameMode->AlliesAllowed) {
		EnableWindow(teamItem, 0);
	} else {
		EnableWindow(teamItem, enable);
	}

	int limit = FUN_005E64C0(Session.Options.ScenarioIndex);
	if (!DAT_008316A4 && limit < 7) {
		limit++;
	}
	if (slot < limit || slot == DAT_008316A4) {
		ShowWindow(colorItem, SW_SHOW);
		ShowWindow(startItem, SW_SHOW);
		if (Session.Type != GAME_INTERNET || !FUN_0077D970()) {
			ShowWindow(teamItem, SW_SHOW);
		}
	}
}

static inline int Slot_Ctrl(int i)
{
	return(FUN_005E98F0(i));
}

// FUNCTION: GAMEMD 0x005EA0D0
void __fastcall FUN_005EA0D0(HWND dialog, int arg)
{
	for (int i = 0; i < 8; i++) {
		HWND item = GetDlgItem(dialog, FUN_005E98F0(i));
		EnableWindow(item, 1);
		FUN_005E9E70(dialog, i, false);
	}
	FUN_005E9E70(dialog, arg, true);
}

// FUNCTION: GAMEMD 0x005EA180
void __fastcall FUN_005EA180(HWND dialog, int arg)
{
	for (int i = 0; i < 8; i++) {
		HWND item = GetDlgItem(dialog, FUN_005E98F0(i));
		EnableWindow(item, 0);
		FUN_005E9E70(dialog, i, false);
	}
	HWND last = GetDlgItem(dialog, FUN_005E98F0(arg));
	EnableWindow(last, 1);
	FUN_005E9E70(dialog, arg, true);
}

struct PCX
{
	static PCX Instance;
	int GetSurface(char const * name, void * palette);
};
extern char * DAT_008316CC;
extern char * DAT_008316D0;
void __fastcall FUN_00603D30(HWND item, int surface);
int __fastcall FUN_005EAAD0(int index);
int __fastcall FUN_005EA8E0(int index);

// FUNCTION: GAMEMD 0x005EA2A0
void __fastcall FUN_005EA2A0(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		HWND item = GetDlgItem(dialog, FUN_005EAAD0(i));
		int surface = 0;
		NodeNameType * node = Session.Slots[i];
		if (FUN_005E93F0(node) && wcscmp(node->Name, Session.GameName) == 0) {
			surface = PCX::Instance.GetSurface(DAT_008316CC, NULL);
		} else if (FUN_005EA8E0(i) == 1) {
			surface = PCX::Instance.GetSurface(DAT_008316D0, NULL);
		}
		FUN_00603D30(item, surface);
	}
}

// FUNCTION: GAMEMD 0x005EA8E0
int __fastcall FUN_005EA8E0(int index)
{
	int result = 0;
	NodeNameType * node = Session.Slots[index];
	if ((node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) || node->Slot.Unknown77 == 1) {
		result = 1;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x005EA920
int __fastcall FUN_005EA920(int index)
{
	int result = -2;
	NodeNameType * node = Session.Slots[index];
	if (Session.MPGameMode && (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET)) {
		result = Session.MPGameMode->vt_entry_28();
	}
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Countries[index]);
	}
	if (node == DAT_00AC119C || node == DAT_00AC11A0 || node == DAT_00AC11B0) {
		return(result);
	}
	if (node == DAT_00AC11B4) {
		return(-3);
	}
	return(node->Get_Country());
}

// FUNCTION: GAMEMD 0x005EA9B0
int __fastcall FUN_005EA9B0(int index)
{
	NodeNameType * node = Session.Slots[index];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Colors[index]);
	}
	if (node == DAT_00AC119C || node == DAT_00AC11A0 || node == DAT_00AC11B0 || node == DAT_00AC11B4) {
		return(-2);
	}
	return(node->Get_Color());
}

// FUNCTION: GAMEMD 0x005EAA10
int __fastcall FUN_005EAA10(int index)
{
	NodeNameType * node = Session.Slots[index];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Starts[index]);
	}
	if (node == DAT_00AC119C || node == DAT_00AC11A0 || node == DAT_00AC11B0 || node == DAT_00AC11B4) {
		return(-2);
	}
	return(node->Get_Start());
}

// FUNCTION: GAMEMD 0x005EAA70
int __fastcall FUN_005EAA70(int index)
{
	NodeNameType * node = Session.Slots[index];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Allies[index]);
	}
	if (node == DAT_00AC119C || node == DAT_00AC11A0 || node == DAT_00AC11B0 || node == DAT_00AC11B4) {
		return(-2);
	}
	return(node->Get_Team());
}



// FUNCTION: GAMEMD 0x005EAAD0
int __fastcall FUN_005EAAD0(int index)
{
	if (index == 0) return(0x6d2);
	if (index == 1) return(0x6d3);
	if (index == 2) return(0x6d4);
	if (index == 3) return(0x6d5);
	if (index == 4) return(0x6d6);
	if (index == 5) return(0x6d7);
	if (index == 6) return(0x6d8);
	if (index == 7) return(0x6d9);
	return(-1);
}

static inline int SlotA(int i)
{
	NodeNameType * node = Session.Slots[i];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Colors[i]);
	}
	if (node == DAT_00AC119C) {
		return(-2);
	}
	if (node == DAT_00AC11A0) {
		return(-2);
	}
	if (node == DAT_00AC11B0) {
		return(-2);
	}
	if (node == DAT_00AC11B4) {
		return(-2);
	}
	return(node->Get_Color());
}

static inline int SlotB(int i)
{
	NodeNameType * node = Session.Slots[i];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Starts[i]);
	}
	if (node == DAT_00AC119C) {
		return(-2);
	}
	if (node == DAT_00AC11A0) {
		return(-2);
	}
	if (node == DAT_00AC11B0) {
		return(-2);
	}
	if (node == DAT_00AC11B4) {
		return(-2);
	}
	return(node->Get_Start());
}

static inline int SlotC(int i)
{
	NodeNameType * node = Session.Slots[i];
	if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
		return(Session.Options.AISlots.Allies[i]);
	}
	if (node == DAT_00AC119C) {
		return(-2);
	}
	if (node == DAT_00AC11A0) {
		return(-2);
	}
	if (node == DAT_00AC11B0) {
		return(-2);
	}
	if (node == DAT_00AC11B4) {
		return(-2);
	}
	return(node->Get_Team());
}

// FUNCTION: GAMEMD 0x005EAB30
void __fastcall FUN_005EAB30(char * buffer)
{
	buffer[0] = '\0';
	for (int i = 0; i < 8; i++) {
		char text[4];
		sprintf(text, "%d,", SlotA(i));
		strcat(buffer, text);
	}
}

// FUNCTION: GAMEMD 0x005EAC00
void __fastcall FUN_005EAC00(char * buffer)
{
	buffer[0] = '\0';
	for (int i = 0; i < 8; i++) {
		char text[4];
		sprintf(text, "%d,", SlotB(i));
		strcat(buffer, text);
	}
}

// FUNCTION: GAMEMD 0x005EACD0
void __fastcall FUN_005EACD0(char * buffer)
{
	buffer[0] = '\0';
	for (int i = 0; i < 8; i++) {
		char text[4];
		sprintf(text, "%d,", SlotC(i));
		strcat(buffer, text);
	}
}

// FUNCTION: GAMEMD 0x005EADA0
void __fastcall FUN_005EADA0(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int id = FUN_004E4E60(i);
		FUN_004E5480(dialog, id, SlotB(i));
	}
}

// FUNCTION: GAMEMD 0x005EAE40
void __fastcall FUN_005EAE40(HWND dialog)
{
	for (int i = 0; i < 8; i++) {
		int id = FUN_004E5940(i);
		FUN_004E5ED0(dialog, id, SlotC(i));
	}
}

// FUNCTION: GAMEMD 0x005EAEE0
void __fastcall FUN_005EAEE0(HWND dialog)
{
	int self = FUN_005E8D40();
	if (self != -1) {
		int id = FUN_004E41D0(self);
		FUN_004E49A0(dialog, id, SlotA(self));
	}
	for (int i = 0; i < 8; i++) {
		if (i != self) {
			int id = FUN_004E41D0(i);
			FUN_004E49A0(dialog, id, SlotA(i));
		}
	}
}


static inline void AddEntry(HWND combo, wchar_t const * text, NodeNameType * const & data)
{
	int index = SendMessage(combo, 0x4c2, 0, (LPARAM)text);
	SendMessage(combo, 0x151, index, (LPARAM)data);
}

// FUNCTION: GAMEMD 0x005EB060
void __fastcall FUN_005EB060(HWND dialog)
{
	bool aiAllowed = (Session.MPGameMode == NULL || Session.MPGameMode->vt_entry_40()) && !FUN_0077D940();
	int limit = FUN_005E6440();
	if (!DAT_008316A4) {
		if (aiAllowed || limit < FUN_005E6470()) {
			limit++;
		}
	}
	for (int i = 0; i < 8; i++) {
		bool capped = false;
		if (i == DAT_008316A4) {
			int count = FUN_005E64C0(Session.Options.ScenarioIndex);
			if (i > count) {
				NodeNameType * node = Session.Slots[i];
				if (node != DAT_00AC119C) {
					if (node == NULL || (node != DAT_00AC11A4 && node != DAT_00AC11A8 && node != DAT_00AC11AC)) {
						capped = true;
					}
				}
			}
			if (i == DAT_008316A4) {
				if (Session.MPGameMode != NULL) {
					Session.MPGameMode->vt_entry_BC();
				}
			}
		}
		int id = FUN_005E98F0(i);
		HWND combo = GetDlgItem(dialog, id);
		bool wasHidden = ShowWindow(combo, SW_HIDE) == 0;
		SendDlgItemMessage(dialog, id, CB_RESETCONTENT, 0, 0);
		NodeNameType * node = Session.Slots[i];
		wchar_t const * text = node->Name;
		if (node == DAT_00AC119C) {
#line 5436 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:Open", NULL, __FILE__, __LINE__);
		} else if (node == DAT_00AC11A0) {
#line 5438 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:Closed", NULL, __FILE__, __LINE__);
		} else if (node == DAT_00AC11A4) {
#line 5440 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:AIEasy", NULL, __FILE__, __LINE__);
		} else if (node == DAT_00AC11A8) {
#line 5442 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:AINormal", NULL, __FILE__, __LINE__);
		} else if (node == DAT_00AC11AC) {
#line 5444 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:AIHard", NULL, __FILE__, __LINE__);
		} else if (node == DAT_00AC11B0) {
#line 5446 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:Waiting", NULL, __FILE__, __LINE__);
		} else if (node == DAT_00AC11B4) {
#line 5448 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:OpenObserver", NULL, __FILE__, __LINE__);
		}
		if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC) && !aiAllowed) {
#line 5455 "D:\\ra2mdpost\\netshare.cpp"
			text = StringTable::LoadString("GUI:Open", NULL, __FILE__, __LINE__);
		}
		WideWstring shown(text);
		wchar_t * localName = FUN_00735120(Session.Handle);
		if (wcscmp(localName, Session.GameName) == 0) {
			if (node && node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B0 && node != DAT_00AC11B4 && node != DAT_00AC11A4 && node != DAT_00AC11A8 && node != DAT_00AC11AC) {
				AddEntry(combo, text, node);
			}
			if (wcscmp(text, FUN_00735120(Session.Handle)) != 0) {
				if (i < limit) {
					if (capped && i == DAT_008316A4) {
						int count = FUN_005E64C0(Session.Options.ScenarioIndex);
						if (i >= count) {
							goto openDone;
						}
					}
#line 5479 "D:\\ra2mdpost\\netshare.cpp"
					AddEntry(combo, StringTable::LoadString("GUI:Open", NULL, __FILE__, __LINE__), DAT_00AC119C);
				}
openDone:
				if (i == DAT_008316A4) {
#line 5489 "D:\\ra2mdpost\\netshare.cpp"
					AddEntry(combo, StringTable::LoadString("GUI:OpenObserver", NULL, __FILE__, __LINE__), DAT_00AC11B4);
				}
#line 5496 "D:\\ra2mdpost\\netshare.cpp"
				AddEntry(combo, StringTable::LoadString("GUI:Closed", NULL, __FILE__, __LINE__), DAT_00AC11A0);
				if (aiAllowed && !capped) {
#line 5504 "D:\\ra2mdpost\\netshare.cpp"
					AddEntry(combo, StringTable::LoadString("GUI:AIEasy", NULL, __FILE__, __LINE__), DAT_00AC11A4);
#line 5510 "D:\\ra2mdpost\\netshare.cpp"
					AddEntry(combo, StringTable::LoadString("GUI:AINormal", NULL, __FILE__, __LINE__), DAT_00AC11A8);
#line 5516 "D:\\ra2mdpost\\netshare.cpp"
					AddEntry(combo, StringTable::LoadString("GUI:AIHard", NULL, __FILE__, __LINE__), DAT_00AC11AC);
				}
			}
		} else {
			AddEntry(combo, text, node);
		}
		SendMessage(combo, 0x4c0, -1, (LPARAM)shown.text());
		if (!wasHidden) {
			ShowWindow(combo, SW_SHOW);
		}
	}
}

// FUNCTION: GAMEMD 0x005EB660
bool __fastcall FUN_005EB660(HWND dialog)
{
	bool changed = false;
	int limit = FUN_005E6440();
	for (int i = 0; i < 8; i++) {
		int id = FUN_005E98F0(i);
		int selected = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
		NodeNameType * node = (NodeNameType *)SendDlgItemMessage(dialog, id, CB_GETITEMDATA, selected, 0);
		if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
			if (i < limit) {
				Session.Slots[i] = DAT_00AC119C;
			} else {
				Session.Slots[i] = DAT_00AC11A0;
			}
			FUN_005E9E70(dialog, i, false);
			Session.Options.AISlots.Difficulties[i] = -1;
			Session.Options.AISlots.Countries[i] = -1;
			Session.Options.AISlots.Colors[i] = -1;
			changed = true;
		}
	}
	return(changed);
}

int __fastcall FUN_004E37D0(int id);

// FUNCTION: GAMEMD 0x005EB7B0
void __fastcall FUN_005EB7B0(HWND dialog, int start, int count)
{
	if (start < 0) {
		start = 0;
	}
	int end = start + count;
	if (end > 8) {
		end = 8;
	}
	for (int i = start; i < end; i++) {
		int labelId = FUN_005EAAD0(i);
		int comboId = FUN_005E98F0(i);
		int id1 = FUN_004E3320(i);
		int id2 = FUN_004E41D0(i);
		int id3 = FUN_004E37D0(i);
		int id4 = FUN_004E4E60(i);
		int id5 = FUN_004E5940(i);
		HWND label = GetDlgItem(dialog, labelId);
		HWND combo = GetDlgItem(dialog, comboId);
		HWND item1 = GetDlgItem(dialog, id1);
		HWND item2 = GetDlgItem(dialog, id2);
		HWND item3 = GetDlgItem(dialog, id3);
		HWND item4 = GetDlgItem(dialog, id4);
		HWND item5 = GetDlgItem(dialog, id5);
		NodeNameType * node = Session.Slots[i];
		bool closed;
		if (node == DAT_00AC11B4 || (node && node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B0 && node->Slot.SpectatorFlag == -1)) {
			closed = true;
		} else {
			closed = false;
		}
		ShowWindow(label, SW_SHOW);
		ShowWindow(combo, SW_SHOW);
		ShowWindow(item1, SW_SHOW);
		if (closed) {
			ShowWindow(item2, SW_HIDE);
			ShowWindow(item4, SW_HIDE);
			ShowWindow(item5, SW_HIDE);
		} else {
			ShowWindow(item2, SW_SHOW);
			ShowWindow(item4, SW_SHOW);
			if (!(Session.Type == GAME_INTERNET && FUN_0077D970())) {
				ShowWindow(item5, SW_SHOW);
			}
		}
		ShowWindow(item3, SW_SHOW);
	}
}

// FUNCTION: GAMEMD 0x005EBA00
void __fastcall FUN_005EBA00(HWND dialog)
{
	int i = DAT_008316A4;
	if (i >= 0) {
		if (i == 0) {
			i = Session.unknown_8D4;
		}
		if (i < 8) {
			int labelId;
			int comboId;
			if (i == 0) {
				labelId = 0x6d2;
				comboId = 0x411;
			} else if (i == 1) {
				labelId = 0x6d3;
				comboId = 0x412;
			} else if (i == 2) {
				labelId = 0x6d4;
				comboId = 0x413;
			} else if (i == 3) {
				labelId = 0x6d5;
				comboId = 0x414;
			} else if (i == 4) {
				labelId = 0x6d6;
				comboId = 0x415;
			} else if (i == 5) {
				labelId = 0x6d7;
				comboId = 0x416;
			} else if (i == 6) {
				labelId = 0x6d8;
				comboId = 0x417;
			} else if (i == 7) {
				labelId = 0x6d9;
				comboId = 0x418;
			} else {
				labelId = -1;
				comboId = -1;
			}
			int id1 = FUN_004E3320(i);
			int id2 = FUN_004E41D0(i);
			int id3 = FUN_004E37D0(i);
			int id4 = FUN_004E4E60(i);
			int id5 = FUN_004E5940(i);
			NodeNameType * node = Session.Slots[i];
			bool closed;
			if (node == DAT_00AC11B4 || (node && node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B0 && node->Slot.SpectatorFlag == -1)) {
				closed = true;
			} else {
				closed = false;
			}
			HWND label = GetDlgItem(dialog, labelId);
			HWND combo = GetDlgItem(dialog, comboId);
			HWND item1 = GetDlgItem(dialog, id1);
			HWND item2 = GetDlgItem(dialog, id2);
			HWND item3 = GetDlgItem(dialog, id3);
			HWND item4 = GetDlgItem(dialog, id4);
			HWND item5 = GetDlgItem(dialog, id5);
			ShowWindow(label, SW_SHOW);
			ShowWindow(combo, SW_SHOW);
			ShowWindow(item1, SW_SHOW);
			if (!closed) {
				ShowWindow(item2, SW_SHOW);
				ShowWindow(item4, SW_SHOW);
				if (!(Session.Type == GAME_INTERNET && FUN_0077D970())) {
					ShowWindow(item5, SW_SHOW);
				}
			} else {
				ShowWindow(item2, SW_HIDE);
				ShowWindow(item4, SW_HIDE);
				ShowWindow(item5, SW_HIDE);
			}
			ShowWindow(item3, SW_SHOW);
		}
	}
}

// FUNCTION: GAMEMD 0x005EBC00
void __fastcall FUN_005EBC00(HWND dialog, int start)
{
	for (int i = start; i < 8; i++) {
		if (i != DAT_008316A4) {
			int labelId;
			int comboId;
			if (i == 0) {
				labelId = 0x6d2;
				comboId = 0x411;
			} else if (i == 1) {
				labelId = 0x6d3;
				comboId = 0x412;
			} else if (i == 2) {
				labelId = 0x6d4;
				comboId = 0x413;
			} else if (i == 3) {
				labelId = 0x6d5;
				comboId = 0x414;
			} else if (i == 4) {
				labelId = 0x6d6;
				comboId = 0x415;
			} else if (i == 5) {
				labelId = 0x6d7;
				comboId = 0x416;
			} else if (i == 6) {
				labelId = 0x6d8;
				comboId = 0x417;
			} else if (i == 7) {
				labelId = 0x6d9;
				comboId = 0x418;
			} else {
				labelId = -1;
				comboId = -1;
			}
			int id1 = FUN_004E3320(i);
			int id2 = FUN_004E37D0(i);
			int id3 = FUN_004E41D0(i);
			int id4 = FUN_004E4E60(i);
			int id5 = FUN_004E5940(i);
			HWND label = GetDlgItem(dialog, labelId);
			HWND combo = GetDlgItem(dialog, comboId);
			HWND item1 = GetDlgItem(dialog, id1);
			HWND item2 = GetDlgItem(dialog, id2);
			HWND item3 = GetDlgItem(dialog, id3);
			HWND item4 = GetDlgItem(dialog, id4);
			HWND item5 = GetDlgItem(dialog, id5);
			ShowWindow(label, SW_HIDE);
			ShowWindow(combo, SW_HIDE);
			ShowWindow(item1, SW_HIDE);
			ShowWindow(item2, SW_HIDE);
			ShowWindow(item3, SW_HIDE);
			ShowWindow(item4, SW_HIDE);
			ShowWindow(item5, SW_HIDE);
		}
	}
}

// FUNCTION: GAMEMD 0x005EBDD0
void __fastcall FUN_005EBDD0(HWND dialog, int start, int end, MultiplayerGameMode * mode)
{
	if (Session.MPGameMode != NULL && !Session.MPGameMode->vt_entry_40() && !FUN_0077D940()) {
		int limit = FUN_005E6440();
		if (start > limit) {
			start = limit;
		}
	}
	if ((mode != NULL && !mode->vt_entry_40()) || FUN_0077D940()) {
		int limit = FUN_005E6440();
		if (end > limit) {
			end = limit;
		}
	}
	int count = end - start;
	if (count > 0) {
		FUN_005EB7B0(dialog, start, count);
	} else if (count < 0) {
		FUN_005EBC00(dialog, end);
	}
	FUN_005EBA00(dialog);
}

void __fastcall FUN_005EC9F0(HWND dialog, int id, NodeNameType * node, int mode);
void FUN_005DAFD0(void);

// FUNCTION: GAMEMD 0x005EBEC0
void __fastcall FUN_005EBEC0(HWND dialog, int start, int count)
{
	int end = start + count;
	int limit = FUN_005E6440();
	for (int i = start; i < end; i++) {
		int id = FUN_005E98F0(i);
		int selected = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
		NodeNameType * node = (NodeNameType *)SendDlgItemMessage(dialog, id, CB_GETITEMDATA, selected, 0);
		if ((int)node != -1 && node && node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B4 && node != DAT_00AC11B0) {
			FUN_005EC9F0(dialog, id, node, false);
		} else if (i != DAT_008316A4) {
			if (i < limit) {
				FUN_005EC9F0(dialog, id, DAT_00AC119C, false);
			} else {
				FUN_005EC9F0(dialog, id, DAT_00AC11A0, false);
			}
		}
	}
	FUN_005EB7B0(dialog, start, count);
	FUN_005DAFD0();
	PumpGameopts(true, false);
}

// FUNCTION: GAMEMD 0x005EC030
void __fastcall FUN_005EC030(HWND dialog)
{
	bool changed = false;
	if (DAT_008316A4 < 1) {
		int limit = FUN_005E6440();
		for (int i = 1; i < 8; i++) {
			int id = FUN_005E98F0(i);
			NodeNameType * node = Session.Slots[i];
			if (i < limit) {
				if (node == DAT_00AC11B4 || node->Slot.SpectatorFlag == -1) {
					FUN_005EC9F0(dialog, id, DAT_00AC119C, true);
					changed = true;
				}
			} else {
				if (node == DAT_00AC11B4 || node->Slot.SpectatorFlag == -1) {
					FUN_005EC9F0(dialog, id, DAT_00AC11A0, true);
					changed = true;
				}
			}
		}
	} else {
		NodeNameType * node = Session.Slots[DAT_008316A4];
		int limit = FUN_005E64C0(Session.Options.ScenarioIndex);
		if (DAT_008316A4 > limit) {
			bool replace = false;
			if (node) {
				if (node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B4 && node != DAT_00AC11B0 && node->Slot.SpectatorFlag != -1) {
					replace = true;
				} else if (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC) {
					replace = true;
				}
			}
			if (replace || node == DAT_00AC119C) {
				node = DAT_00AC11A0;
				changed = true;
			}
		}
		FUN_005EBA00(dialog);
		FUN_005EC9F0(dialog, FUN_005E98F0(DAT_008316A4), node, true);
	}
	if (changed) {
		FUN_005E9B60(dialog, FUN_004E37D0(DAT_008316A4));
	}
	FUN_005DAFD0();
	PumpGameopts(true, false);
}

void FUN_00785360(void);
extern int DAT_00AC116C;

// FUNCTION: GAMEMD 0x005EC290
void __fastcall FUN_005EC290(HWND dialog, int start)
{
	int limit = FUN_005E6440();
	for (int i = start; i < 8; i++) {
		int id = FUN_005E98F0(i);
		if (i < limit) {
			if (i != DAT_008316A4) {
				FUN_005EC9F0(dialog, id, DAT_00AC119C, true);
			}
		} else if (i != DAT_008316A4) {
			FUN_005EC9F0(dialog, id, DAT_00AC11A0, true);
		}
	}
	FUN_005EBC00(dialog, start);
	if (DAT_00AC116C) {
		FUN_00785360();
	} else {
		FUN_005DAFD0();
	}
	PumpGameopts(true, false);
}

extern int DAT_00831700;
extern int DAT_00AC11CC;

inline void Format_Slot(char * buffer, Wstring const & narrow) { sprintf(buffer, "O%d%s", DAT_008316A4, narrow.c_str()); }

// FUNCTION: GAMEMD 0x005EC3A0
void __fastcall FUN_005EC3A0(HWND dialog, int sel, int scenario)
{
	int maxslots = FUN_005E64C0(scenario);
	bool unlimited;
	if ((Session.MPGameMode == NULL || Session.MPGameMode->vt_entry_40()) && !FUN_0077D940()) {
		unlimited = true;
	} else {
		unlimited = false;
		int limit = FUN_005E6440();
		if (maxslots > limit) {
			maxslots = limit;
		}
	}
	if (sel == -1) {
		DAT_00831700 = 8;
		DAT_00AC11CC = 0;
	}
	int delta = maxslots - DAT_00831700;
	int flag = 0;
	if (DAT_008316A4 == 0) {
		if (unlimited || maxslots < FUN_005E6470()) {
			flag = 1;
			if (DAT_00AC11CC == 0 && delta + maxslots < 7) {
				delta++;
			}
		}
	}
	if (sel == -1 && DAT_008316A4 == 0) {
		int id = FUN_004E37D0(0);
		FUN_004E3B00(dialog, id);
		FUN_004E3F70(dialog, id, -3);
		SendMessage(GetDlgItem(dialog, 0x411), 0x4f1, 0, 1);
		id = FUN_004E41D0(0);
		FUN_004E4770(dialog, id);
		FUN_004E49A0(dialog, id, 8);
	}
	if (delta > 0) {
		FUN_005EBEC0(dialog, DAT_00AC11CC + DAT_00831700, delta);
	} else if (delta < 0) {
		FUN_005EC290(dialog, flag + maxslots);
	} else if (flag + maxslots >= 7) {
		FUN_005EBEC0(dialog, 7, 1);
	}
	FUN_005EC030(dialog);
	if (DAT_008316A4 == -1 && Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_BC()) {
		DAT_008316A4 = 7;
		FUN_005EC030(dialog);
		FUN_004068E0("Sending observer slot of 7\n");
		NodeNameType * node = Session.Slots[DAT_008316A4];
		WideWstring name;
		if (FUN_005E93F0(node)) {
			name = node->Name;
		} else {
			Wstring raw = FUN_005EDEF0(node);
			name = FUN_007B7ED0(raw);
		}
		char buffer[0x100];
		Format_Slot(buffer, FUN_007B7F90(name.get()));
		SendPublicGameopts(buffer);
	}
	DAT_00831700 = maxslots;
	DAT_00AC11CC = flag;
}

// FUNCTION: GAMEMD 0x005EC880
bool __fastcall FUN_005EC880(HWND dialog, int limit, bool arg1, bool arg2)
{
	if (DAT_008316A4 == 0) {
		limit++;
	}
	for (int i = 0; i < 8; i++) {
		int id = FUN_005E98F0(i);
		int selected = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
		int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, selected, 0);
		NodeNameType * node;
		if (data == -1) {
			node = NULL;
		} else {
			node = (NodeNameType *)data;
		}
		if (node && (node == DAT_00AC11A4 || node == DAT_00AC11A8 || node == DAT_00AC11AC)) {
			if (Session.MPGameMode && !arg1) {
				return(true);
			}
		}
		if (i >= limit) {
			bool real = node != NULL && node != DAT_00AC119C && node != DAT_00AC11A0 && node != DAT_00AC11B4 && node != DAT_00AC11B0;
			if (real && node->Get_Country() != -3) {
				return(true);
			}
			if (Session.MPGameMode && !arg2 && real && node->Get_Country() == -3) {
				return(true);
			}
		}
	}
	return(false);
}

namespace Game { extern int ObserverMode; }
void __fastcall FUN_004E5260(HWND dialog, int id);
void __fastcall FUN_004E50C0(HWND dialog, int id);
void __fastcall FUN_004E5B60(HWND dialog, int id);
void __fastcall FUN_005EE510(int arg);
void FUN_0078F000(void);
void __fastcall FUN_007A28B0(NodeNameType * node);
void __fastcall FUN_005DB600(NodeNameType * node);
void FUN_0077DB90(void);
void FUN_005DAF30(void);
void __fastcall FUN_005ED270(HWND dialog, int id, int kind);

static inline void Select_Node_Item(HWND dialog, int id, NodeNameType * node)
{
	HWND item = GetDlgItem(dialog, id);
	ShowWindow(item, SW_HIDE);
	int count = SendDlgItemMessage(dialog, id, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < count; i++) {
		if ((NodeNameType *)SendDlgItemMessage(dialog, id, CB_GETITEMDATA, i, 0) == node) {
			SendDlgItemMessage(dialog, id, CB_SETCURSEL, i, 0);
			break;
		}
	}
	ShowWindow(item, SW_SHOW);
}

static inline void Select_Slot_Node(HWND dialog, int id, int kind)
{
	NodeNameType * node;
	if (kind == 2) {
		node = DAT_00AC119C;
	} else if (kind == 3) {
		node = DAT_00AC11A0;
	} else if (kind == 4) {
		node = DAT_00AC11A4;
	} else if (kind == 5) {
		node = DAT_00AC11A8;
	} else if (kind == 6) {
		node = DAT_00AC11AC;
	} else if (kind == 7) {
		node = DAT_00AC11B4;
	} else {
		node = NULL;
	}
	Select_Node_Item(dialog, id, node);
	FUN_005EC9F0(dialog, id, node, false);
}

// FUNCTION: GAMEMD 0x005ED270
void __fastcall FUN_005ED270(HWND dialog, int id, int kind)
{
	Select_Slot_Node(dialog, id, kind);
}

// FUNCTION: GAMEMD 0x005ED370
void __fastcall FUN_005ED370(HWND dialog)
{
	if (Session.Scenarios[Session.Options.ScenarioIndex]->Is_Random_Map()) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = new MapPreviewClass;
		MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
		if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
			FUN_005E74E0(dialog);
		}
		InvalidateRect(dialog, NULL, FALSE);
	} else {
		FUN_005E74E0(dialog);
	}
}

// FUNCTION: GAMEMD 0x005ED400
void __fastcall FUN_005ED400(HWND dialog, SessionOptionsClass * out)
{
	out->MPGameMode = GameModeOptionsClass::Instance;
	out->ScenIndex = Session.Options.ScenarioIndex;
	for (int i = 1; i < 8; i++) {
		int id = FUN_005E98F0(i);
		int selected = SendDlgItemMessage(dialog, id, CB_GETCURSEL, 0, 0);
		int data = SendDlgItemMessage(dialog, id, CB_GETITEMDATA, selected, 0);
		NodeNameType * node;
		if (data == -1) {
			node = NULL;
		} else {
			node = (NodeNameType *)data;
		}
		int kind;
		if (node == DAT_00AC119C) {
			kind = 2;
		} else if (node == DAT_00AC11A0) {
			kind = 3;
		} else if (node == DAT_00AC11A4) {
			kind = 4;
		} else if (node == DAT_00AC11A8) {
			kind = 5;
		} else if (node == DAT_00AC11AC) {
			kind = 6;
		} else if (node == DAT_00AC11B4) {
			kind = 7;
		} else {
			kind = 2;
		}
		out->SlotData[i][0] = kind;
		int countryId = FUN_004E37D0(i);
		out->SlotData[i][1] = FUN_004E4170(dialog, countryId, -1);
		int colorId = FUN_004E41D0(i);
		out->SlotData[i][2] = FUN_004E4E20(dialog, colorId, -1);
	}
	out->GameSpeed = Session.Options.GameSpeed;
	out->Credits = Session.Options.Credits;
	out->UnitCount = Session.Options.UnitCount;
	out->ShortGame = Session.Options.ShortGame;
	out->SuperWeaponsAllowed = Session.Options.SWAllowed;
	out->BuildOffAlly = Session.Options.BuildOffAlly;
	out->MCVRepacks = Session.Options.MCVRedeploy;
	out->CratesAppear = Session.Options.Goodies;
}

int Game_Mode_Total(void);
int FUN_0058B7D0(void);

static inline MultiplayerGameMode * Mode_Or_First(MultiplayerGameMode * mode)
{
	return(mode ? mode : First_Game_Mode());
}

// FUNCTION: GAMEMD 0x005ED5A0
void __fastcall FUN_005ED5A0(HWND dialog, SessionOptionsClass * opts)
{
	if (opts->MPGameMode < 1 || opts->MPGameMode > Game_Mode_Total()) {
		opts->MPGameMode = First_Game_Mode()->MPModeIndex;
	}
	MultiplayerGameMode * mode = MultiplayerGameMode::Get(GameModeOptionsClass::Instance = opts->MPGameMode);
	if (mode == NULL) {
		mode = First_Game_Mode();
	}
	if ((FUN_0077D940() || FUN_0077D970()) && mode && mode->vt_entry_04()) {
		mode = Mode_Or_First(MultiplayerGameMode::Get(GameModeOptionsClass::Instance = opts->MPGameMode = First_Game_Mode()->MPModeIndex));
	}
	opts->MPGameMode = mode->MPModeIndex;
	GameModeOptionsClass::Instance = mode->MPModeIndex;

	if (opts->ScenIndex < 0 || opts->ScenIndex >= Session.Scenarios.Count()) {
		opts->ScenIndex = 0;
	}
	Session.Options.ScenarioIndex = Set_Scenario_Info_From_Index(opts->ScenIndex) ? opts->ScenIndex : 0;

	bool regenerate = false;
	MultiMission * map = Session.Scenarios[Session.Options.ScenarioIndex];
	if (map != NULL && map->Is_Random_Map()) {
		if (FUN_0058B7D0() == 0) {
			regenerate = true;
		} else {
			FUN_0058BB30();
		}
	}
	if (!mode->FUN_005D63E0(map) || regenerate) {
		int index = mode->FirstValidMapIndex();
		if (index == -1) {
			mode = First_Game_Mode();
			opts->MPGameMode = mode->MPModeIndex;
			GameModeOptionsClass::Instance = mode->MPModeIndex;
			index = mode->FirstValidMapIndex();
		}
		opts->ScenIndex = index;
		Session.Options.ScenarioIndex = index;
		Set_Scenario_Info_From_Index(index);
	}

	Session.MPGameMode = mode;
	Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex);
	Session.MPGameMode->vt_entry_20();
	Session.MPGameMode->vt_entry_24();
	mode->vt_entry_18();
	mode->UnfixAlliances();

	int limit = 8;
	if (Session.Type == GAME_INTERNET) {
		limit = (FUN_0077D940() && !FUN_0077D970()) ? 2 : 6;
	}
	if (DAT_008316A4 == 0 && limit < 8) {
		limit++;
	}
	for (int i = 1; i < 8; i++) {
		int id = FUN_005E98F0(i);
		int kind = opts->SlotData[i][0];
		if (FUN_0077D940() || DAT_008316A4 == 0 || Game::ObserverMode) {
			if (kind == 4 || kind == 5 || kind == 6) {
				kind = 2;
			}
		}
		NodeNameType * node;
		if (i >= limit) {
			node = DAT_00AC11A0;
			kind = 3;
		} else if (kind == 7) {
			node = DAT_00AC119C;
			kind = 2;
		} else if (kind == 2) {
			node = DAT_00AC119C;
		} else if (kind == 3) {
			node = DAT_00AC11A0;
		} else if (kind == 4) {
			node = DAT_00AC11A4;
		} else if (kind == 5) {
			node = DAT_00AC11A8;
		} else if (kind == 6) {
			node = DAT_00AC11AC;
		} else {
			node = NULL;
		}
		Select_Node_Item(dialog, id, node);
		FUN_005EC9F0(dialog, id, node, false);

		int country_id = FUN_004E37D0(i);
		int color_id = FUN_004E41D0(i);
		int start_id = FUN_004E4E60(i);
		int team_id = FUN_004E5940(i);
		if (kind == 2 || kind == 3) {
			FUN_004E3F70(dialog, country_id, Session.MPGameMode->vt_entry_28());
			FUN_004E49A0(dialog, color_id, -2);
			FUN_004E5480(dialog, start_id, -2);
			FUN_004E5ED0(dialog, team_id, Session.MPGameMode->vt_entry_2C());
			HWND country_box = GetDlgItem(dialog, country_id);
			HWND color_box = GetDlgItem(dialog, color_id);
			HWND start_box = GetDlgItem(dialog, start_id);
			HWND team_box = GetDlgItem(dialog, team_id);
			EnableWindow(country_box, FALSE);
			EnableWindow(color_box, FALSE);
			EnableWindow(start_box, FALSE);
			EnableWindow(team_box, FALSE);
		} else if (kind == 7) {
			FUN_004E3F70(dialog, country_id, -3);
			FUN_004E49A0(dialog, color_id, -2);
			FUN_004E5480(dialog, start_id, -2);
			FUN_004E5ED0(dialog, team_id, Session.MPGameMode->vt_entry_2C());
			HWND country_box = GetDlgItem(dialog, country_id);
			HWND color_box = GetDlgItem(dialog, color_id);
			HWND start_box = GetDlgItem(dialog, start_id);
			HWND team_box = GetDlgItem(dialog, team_id);
			EnableWindow(country_box, FALSE);
			EnableWindow(color_box, FALSE);
			EnableWindow(start_box, FALSE);
			EnableWindow(team_box, FALSE);
		} else {
			FUN_004E3F70(dialog, country_id, opts->SlotData[i][1]);
			FUN_005E9B60(dialog, country_id);
			FUN_004E49A0(dialog, color_id, opts->SlotData[i][2]);
			FUN_005E9C00(dialog, color_id);
			FUN_004E5480(dialog, start_id, -2);
			FUN_005E9CE0(dialog, start_id);
			FUN_004E5ED0(dialog, team_id, Session.MPGameMode->vt_entry_30());
			FUN_005E9DB0(dialog, team_id);
		}
	}
	Session.Options.GameSpeed = opts->GameSpeed;
	Session.Options.Credits = opts->Credits;
	Session.Options.UnitCount = opts->UnitCount;
	Session.Options.ShortGame = opts->ShortGame;
	Session.Options.SWAllowed = opts->SuperWeaponsAllowed;
	Session.Options.BuildOffAlly = opts->BuildOffAlly;
	Session.Options.MCVRedeploy = opts->MCVRepacks;
	Session.Options.Goodies = opts->CratesAppear;
}

int __fastcall FUN_00623EA0(HWND window, WideWstring * text);
unsigned char __fastcall FUN_007B7D90(WideWstring * text);

// FUNCTION: GAMEMD 0x005EDD00
WideWstring __fastcall FUN_005EDD00(HWND edit)
{
	WideWstring text;
	FUN_00623EA0(edit, &text);
	unsigned int limit = 0x78;
	if (!FUN_007B7D90(&text)) {
		for (limit = 0; limit < text.length(); limit++) {
			if (text.getChar(limit) > 0x100) {
				break;
			}
		}
		limit--;
		if (limit <= 0x28) {
			limit = 0x28;
		}
	}
	if (text.length() > limit) {
		text.truncate(limit);
		SendMessage(edit, 0x4b2, 0, (LPARAM)text.text());
	}
	{
		WideWstring delim(L"\n\r");
		text.trimTrailing(delim);
	}
	return(text);
}

static inline void Enable_Item(HWND dialog, int id, bool enable)
{
	HWND item = GetDlgItem(dialog, id);
	if (item) {
		EnableWindow(item, enable);
	}
}

static inline void Enable_Team(HWND dialog, int id, bool enable)
{
	HWND item = GetDlgItem(dialog, id);
	if (item) {
		if (enable == 1 && Session.MPGameMode && !Session.MPGameMode->AlliesAllowed) {
			EnableWindow(item, 0);
		} else {
			EnableWindow(item, enable);
		}
	}
}

// FUNCTION: GAMEMD 0x005EDDE0
void __fastcall FUN_005EDDE0(HWND dialog, bool enable)
{
	if (dialog) {
		int self = FUN_005E8D40();
		if (self != DAT_008316A4) {
			Enable_Item(dialog, FUN_004E37D0(self), enable);
			Enable_Item(dialog, FUN_004E41D0(self), enable);
			Enable_Item(dialog, FUN_004E4E60(self), enable);
			Enable_Team(dialog, FUN_004E5940(self), enable);
		}
	}
}

extern char const * DAT_00831704;
extern char const * DAT_00831708;
extern char const * DAT_0083170C;
extern char const * DAT_00831710;
extern char const * DAT_00831714;
extern char const * DAT_00831718;
extern char const * DAT_0083171C;

// FUNCTION: GAMEMD 0x005EDEF0
Wstring __fastcall FUN_005EDEF0(NodeNameType * slot)
{
	if (slot == DAT_00AC119C) {
		return(Wstring(DAT_00831704));
	}
	if (slot == DAT_00AC11A0) {
		return(Wstring(DAT_00831708));
	}
	if (slot == DAT_00AC11B0) {
		return(Wstring(DAT_0083170C));
	}
	if (slot == DAT_00AC11A4) {
		return(Wstring(DAT_00831710));
	}
	if (slot == DAT_00AC11A8) {
		return(Wstring(DAT_00831714));
	}
	if (slot == DAT_00AC11AC) {
		return(Wstring(DAT_00831718));
	}
	if (slot == DAT_00AC11B4) {
		return(Wstring(DAT_0083171C));
	}
	return(Wstring(DAT_0083170C));
}

extern NodeNameType * DAT_00AC11A0;
extern NodeNameType * DAT_00AC11A4;
extern NodeNameType * DAT_00AC11A8;
extern NodeNameType * DAT_00AC11AC;
extern NodeNameType * DAT_00AC11B0;
extern NodeNameType * DAT_00AC11B4;

// FUNCTION: GAMEMD 0x005EDFC0
NodeNameType * __fastcall FUN_005EDFC0(WideWstring * name)
{
	if (FUN_007B7D90(name)) {
		Wstring text = FUN_007B7F90(*name);
		if (text == DAT_00831704) {
			return(DAT_00AC119C);
		}
		if (text == DAT_00831708) {
			return(DAT_00AC11A0);
		}
		if (text == DAT_0083170C) {
			return(DAT_00AC11B0);
		}
		if (text == DAT_00831710) {
			return(DAT_00AC11A4);
		}
		if (text == DAT_00831714) {
			return(DAT_00AC11A8);
		}
		if (text == DAT_00831718) {
			return(DAT_00AC11AC);
		}
		if (text == DAT_0083171C) {
			return(DAT_00AC11B4);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x005EE100
int __fastcall FUN_005EE100(Wstring const & name)
{
	if (name == DAT_00831704 || name == DAT_00831708 || name == DAT_0083170C || name == DAT_00831710 || name == DAT_00831714 || name == DAT_00831718 || name == DAT_0083171C) {
		return(1);
	}
	return(0);
}

extern CCINIClass DAT_008870C0;
namespace Game { extern int ObserverMode; }

// FUNCTION: GAMEMD 0x005EE190
void FUN_005EE190(void)
{
	Game::ObserverMode = DAT_008870C0.Get_Int("MultiPlayer", "ObserverMode", 0);
}

// FUNCTION: GAMEMD 0x005EE1B0
void FUN_005EE1B0(void)
{
	RawFileClass file("RA2MD.INI");
	DAT_008870C0.Put_Int("MultiPlayer", "ObserverMode", Game::ObserverMode, 0);
	DAT_008870C0.Save(file, false);
}

void __fastcall FUN_00603D30(HWND item, int surface);

// FUNCTION: GAMEMD 0x005EE200
void __fastcall FUN_005EE200(HWND dialog)
{
	if (dialog) {
		HWND item = GetDlgItem(dialog, 0x760);
		if (item) {
			SendMessage(item, 0xf1, Game::ObserverMode != 0, 0);
		}
		int surface;
		if (Game::ObserverMode) {
			surface = PCX::Instance.GetSurface("wolob.pcx", NULL);
		} else {
			surface = PCX::Instance.GetSurface("wolnob.pcx", NULL);
		}
		item = GetDlgItem(dialog, 0x75f);
		FUN_00603D30(item, surface);
		item = GetDlgItem(dialog, 0x6cd);
		if (item) {
			wchar_t const * text;
			if (Game::ObserverMode) {
#line 6751 "D:\\ra2mdpost\\netshare.cpp"
				text = StringTable::LoadString("GUI:Observe", NULL, __FILE__, __LINE__);
			} else {
#line 6753 "D:\\ra2mdpost\\netshare.cpp"
				text = StringTable::LoadString("GUI:Join", NULL, __FILE__, __LINE__);
			}
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			InvalidateRect(item, 0, 0);
		}
		item = GetDlgItem(dialog, 0x62d);
		if (item) {
			wchar_t const * text;
			if (Game::ObserverMode) {
#line 6762 "D:\\ra2mdpost\\netshare.cpp"
				text = StringTable::LoadString("GUI:Observe", NULL, __FILE__, __LINE__);
			} else if (FUN_00775940(0x113) == dialog) {
#line 6766 "D:\\ra2mdpost\\netshare.cpp"
				text = StringTable::LoadString("GUI:JoinGame", NULL, __FILE__, __LINE__);
			} else {
#line 6768 "D:\\ra2mdpost\\netshare.cpp"
				text = StringTable::LoadString("GUI:Join", NULL, __FILE__, __LINE__);
			}
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			InvalidateRect(item, 0, 0);
		}
	}
}
