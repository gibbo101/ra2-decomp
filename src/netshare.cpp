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
#include "msgbox.h"
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
#include <ctime>
#include <stddef.h>
#include "stringtable.h"
#include <stdarg.h>
#include <wchar.h>
#include "widewstring.h"

void FUN_00623F60(HWND listbox, int index, WideWstring & text);
void FUN_0060B350(HWND button);
void FUN_00607FD0(HWND dialog);
int FUN_007759E0(HWND window, bool (*callback)(void), int a, int b, int c);
namespace UI { void RegisterWindow(HWND window, int id); }
namespace Game { extern HWND hWnd; extern HINSTANCE hInstance; extern int PlayerColor; }

extern int DAT_00A8B8D8;
extern MPMapEntry ** DAT_00A8B8CC;
extern wchar_t DAT_00A8B322[];
class GameModeOptionsClass { public: static int Instance; };
extern MPGameModeClass * DAT_00A8B23C;
MPGameModeClass * First_Game_Mode(void);
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
extern bool DAT_00A8DAB0;

struct NSTerritory { char Pad[0x3c]; unsigned int Flags; };
class NSTerritoryHandle : public WorldDominationTour::YRHandleBase
{
	public:
		NSTerritoryHandle(void) {}
		virtual ~NSTerritoryHandle(void) {}
		NSTerritoryHandle & operator=(NSTerritoryHandle const & that) { FUN_0065D480(that.Ptr); return(*this); }
};
NSTerritoryHandle __fastcall FUN_007B0390(int id);

// FUNCTION: 0x005E2430 $E2
// FUNCTION: 0x005E2460 $E5
// FUNCTION: 0x005E2480 $E8
// FUNCTION: 0x005E24A0 $E11
// FUNCTION: 0x005E24C0 $E14
// FUNCTION: 0x005E24E0 $E17
// FUNCTION: 0x005E2500 $E20
// FUNCTION: 0x005E2540 $E23
// FUNCTION: 0x005E2570 $E26
// FUNCTION: 0x005E25A0 $E29
// FUNCTION: 0x005E25D0 $E32
// FUNCTION: 0x005E25E0 $E35
// FUNCTION: 0x005E2600 $E38

// FUNCTION: 0x005E2620
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

// FUNCTION: 0x005E2670
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

// FUNCTION: 0x005E26C0
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

// FUNCTION: 0x005E2700
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

// FUNCTION: 0x005E2790
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

// FUNCTION: 0x005E2820
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

// FUNCTION: 0x005E29A0
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

// FUNCTION: 0x005E2AC0
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

// FUNCTION: 0x005E2BA0
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

// FUNCTION: 0x005E2BE0
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

// FUNCTION: 0x005E2CD0
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

// FUNCTION: 0x005E2E40
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

// FUNCTION: 0x005E2EF0
void FUN_005E2EF0(HWND dialog, MPGameModeClass * mode)
{
	if (mode != NULL) {
		HWND item = GetDlgItem(dialog, 0x6ec);
		SendMessage(item, 0x4b2, 0, (LPARAM)mode->UIName.text());
	}
}

// FUNCTION: 0x005E2F20
void __fastcall FUN_005E2F20(HWND dialog, int index)
{
	if (index >= 0 && index < DAT_00A8B8D8) {
		MPMapEntry * map = DAT_00A8B8CC[index];
		if (map != NULL) {
			HWND item = GetDlgItem(dialog, 0x5a8);
			SendMessage(item, 0x4b2, 0, (LPARAM)map);
		}
	}
}

// FUNCTION: 0x005E2F60
void __fastcall FUN_005E2F60(HWND dialog)
{
	HWND item = GetDlgItem(dialog, 0x5a8);
	SendMessage(item, 0x4b2, 0, (LPARAM)DAT_00A8B322);
}

// FUNCTION: 0x005E2F80
MPGameModeClass * FUN_005E2F80(void)
{
	MPGameModeClass * mode = MPGameModeClass::Get(GameModeOptionsClass::Instance);
	if (mode == NULL) {
		mode = First_Game_Mode();
	}
	return(mode);
}

// FUNCTION: 0x005E2FA0
void FUN_005E2FA0(void)
{
	if (FUN_00775B20() == 0x6b) {
		HWND dialog = FUN_00775B10();
		int sel = SendDlgItemMessage(dialog, 0x6eb, LB_GETCURSEL, 0, 0);
		if (sel != -1) {
			MPGameModeClass * mode = (MPGameModeClass *)SendDlgItemMessage(dialog, 0x6eb, LB_GETITEMDATA, sel, 0);
			if (mode != NULL) {
				mode->vt_entry_94();
			}
		}
	} else if (DAT_00A8B23C != NULL) {
		DAT_00A8B23C->vt_entry_94();
	}
}

// FUNCTION: 0x005E3010
void __fastcall DisplayGameopts(HWND window, BOOL initialize)
{
	int unit_count = Session.Options.UnitCount;
	int credits = Session.Options.Credits;
	int game_speed = Session.Options.GameSpeed;

	if (initialize) {
		NSTerritoryHandle handle;
		if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
			handle = FUN_007B0390(Session.unknown_08);
		}
		if (handle.Ptr != NULL) {
			EnableWindow(GetDlgItem(window, 0x544), FALSE);
			EnableWindow(GetDlgItem(window, 0x6d4), FALSE);
			if (!(((NSTerritory *)handle.Ptr)->Flags & 0x20000)) {
				EnableWindow(GetDlgItem(window, 0x402), FALSE);
			}
			if (!(((NSTerritory *)handle.Ptr)->Flags & 0x8000)) {
				EnableWindow(GetDlgItem(window, 0x54f), FALSE);
			}
			if (!(((NSTerritory *)handle.Ptr)->Flags & 0x400)) {
				EnableWindow(GetDlgItem(window, 0x54e), FALSE);
			}
			if (!(((NSTerritory *)handle.Ptr)->Flags & 0x20)) {
				EnableWindow(GetDlgItem(window, 0x3e8), FALSE);
			}
			if (!(((NSTerritory *)handle.Ptr)->Flags & 0x100)) {
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

// FUNCTION: 0x005E32D0
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

// FUNCTION: 0x005E35A0
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

// FUNCTION: 0x005E3630
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

// FUNCTION: 0x005E36E0
void FUN_005E36E0(void)
{
	DAT_00A8DAB0 = true;
	if (g_pChat != NULL) {
		FUN_007A2750();
	} else {
		FUN_005E2340();
	}
}

// FUNCTION: 0x005E3700
void FUN_005E3700(void)
{
	DAT_00A8DAB0 = false;
	if (g_pChat != NULL) {
		FUN_007A27D0();
	} else {
		FUN_005E23A0();
	}
}

// FUNCTION: 0x005E3720
void SendPublicGameopts(char const * options)
{
	if (g_pChat != NULL) {
		char buffer[0x400];
		strncpy(buffer, options, 0x3ff);
		g_pChat->RequestPublicGameOptions((LPCSTR)buffer);
	} else {
		GlobalPacketType packet;
		memset(&packet, 0, sizeof(packet));
		packet.Command = (NetCommandType)0x17;
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

// FUNCTION: 0x005E3850
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
		packet.Command = (NetCommandType)0x18;
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

extern wchar_t DAT_00A8D610[];
extern bool DAT_00AC11C2;
int __fastcall FUN_0077EBA0(Wstring const & who);
void __fastcall FUN_0077E900(Wstring const & who, int status);
int __fastcall FUN_005DBEC0(char * who);
void __fastcall FUN_005DBE20(char * who, int status);
HWND __fastcall FUN_00775940(int id);

// FUNCTION: 0x005E39C0
void FUN_005E39C0(char * name, int status)
{
	if (name == NULL) {
		name = Session.Handle;
	}
	if (wcscmp(FUN_00735120(name), DAT_00A8D610) == 0) {
		status = 1;
	}
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (stricmp(FUN_007350C0(Session.Players[i]->Name), name) == 0) {
			if (i != -1) {
				Session.Players[i]->Slot.Unknown7F = status;
				if (status == 0) {
#line 1169 "D:\\ra2mdpost\\netshare.cpp"
					PMessagePrintf(-1, StringTable::LoadString("GUI:HostNoMap", NULL, __FILE__, __LINE__), Session.Players[i], DAT_00A8B322);
				} else if (status == 2) {
#line 1173 "D:\\ra2mdpost\\netshare.cpp"
					PMessagePrintf(-1, StringTable::LoadString("GUI:HostMapTransfer", NULL, __FILE__, __LINE__), Session.Players[i], DAT_00A8B322);
				}
			}
			return;
		}
	}
}

// FUNCTION: 0x005E3AB0
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

// FUNCTION: 0x005E6230
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
class MPMapEntry { public: bool FUN_0069ADF0(void); char Pad[0x58]; char FileName[1]; };

// FUNCTION: 0x005E6440
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

// FUNCTION: 0x005E6470
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

// FUNCTION: 0x005E64C0
int FUN_005E64C0(int index)
{
	if (index == -1) {
		return(8);
	}
	int count = RandomMapWaypointCount(index);
	if (DAT_00A8B23C != NULL) {
		int limit = DAT_00A8B23C->vt_entry_98();
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

// FUNCTION: 0x005E6520
int RandomMapWaypointCount(int index)
{
	char wp[32];
	char buf[132];
	if (index >= 0 && index < DAT_00A8B8D8) {
		strcpy(buf, DAT_00A8B8CC[index]->FileName);
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
extern int DAT_00A8B254;
extern wchar_t DAT_00AC0EC8[];
void __fastcall FUN_005E7BF0(int slot);
void __fastcall FUN_005E74E0(HWND dialog);
void FUN_0058BB30(void);
bool FUN_0077F900(void);
int CALLBACK FUN_005E6920(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void FUN_0072D120(void);
void FUN_0072D170(void);

// FUNCTION: 0x005E66D0
int FUN_005E66D0(void)
{
	return(SendDlgItemMessage(DAT_00AC0D40, 0x553, LB_GETCURSEL, 0, 0));
}

bool FUN_005DAF20(void);
bool FUN_005BB260(void);

// FUNCTION: 0x005E66F0
bool FUN_005E66F0(void)
{
	int index = SendDlgItemMessage(DAT_00AC0D40, 0x553, LB_GETCURSEL, 0, 0);
	if (index != -1) {
		MPMapEntry * data = (MPMapEntry *)SendDlgItemMessage(DAT_00AC0D40, 0x553, 0x199, index, 0);
		int found = -1;
		for (int i = 0; i < DAT_00A8B8D8; i++) {
			if (data == DAT_00A8B8CC[i]) {
				found = i;
				break;
			}
		}
		if (found != -1 && found != DAT_00AC0D30) {
			FUN_005E7BF0(found);
			if (DAT_00A8B8CC[found]->FUN_0069ADF0()) {
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
			DAT_00A8B254 = DAT_00AC10E0;
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

// FUNCTION: 0x005E68A0
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

// FUNCTION: 0x005E70D0
int FUN_005E70D0(HWND dialog, MPMapEntry * map)
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
