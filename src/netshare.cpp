// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

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
#include <stdarg.h>
#include <wchar.h>
#include "widewstring.h"

void FUN_00623F60(HWND listbox, int index, WideWstring & text);
void FUN_0060B350(HWND button);
void FUN_00607FD0(HWND dialog);
int FUN_007759E0(HWND window, bool (*callback)(void), int a, int b, int c);
namespace UI { void RegisterWindow(HWND window, int id); }
namespace Game { extern HWND hWnd; extern HINSTANCE hInstance; }

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
