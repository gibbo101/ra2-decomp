// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "windlg.h"
#include "arraylist.h"
#include "data.h"
#include "globals.h"
#include "init.h"
#include "msgloop.h"
#include "ownrdraw.h"
#include "win.h"
#include <commctrl.h>
#include <windows.h>

// SYNTHETIC: GAMEMD 0x00775670 SYMBOL
// _$E2

BOOL CALLBACK Resize_Dialog(HWND window, LPARAM lParam);

extern bool g_PlayingNetGame;
extern int DAT_00AA0430;
int __fastcall FUN_0053E770(MSG *msg, HWND wnd, UINT min, UINT max, UINT flags);

BOOL CALLBACK Resize_Dialog_Proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

struct EzFont {
	char FaceName[128];
	int DeciPtWidth;
	int DeciPtHeight;
	int Attributes;
	HFONT FontHandle;
};

#define EZ_ATTR_BOLD		  1
#define EZ_ATTR_ITALIC		  2
#define EZ_ATTR_UNDERLINE	  4
#define EZ_ATTR_STRIKEOUT	  8

HFONT Ez_Create_Font (HDC hdc, const char * face_name, int decipt_width, int decipt_height, int attributes);

// FUNCTION: GAMEMD 0x00775690
BOOL Get_Display_Rect(HWND window, LPRECT rect)
{
	RECT client;
	BOOL res = GetWindowRect(window, rect);
	if (!res) {
		return(res);
	}
	GetClientRect(MainWindow, &client);
	ClientToScreen(MainWindow, (LPPOINT)&client);
	rect->left -= client.left;
	rect->right -= client.left;
	rect->top -= client.top;
	rect->bottom -= client.top;
	return(res);
}

// FUNCTION: GAMEMD 0x00775700
HWND WS_Create_Dialog(HINSTANCE instance, int id, HWND parent, DLGPROC proc, BOOL force_show)
{
	WSDialogStruct *slot = &g_Dialogs[g_DialogCount];
	g_Dialogs[g_DialogCount].handle = 0;
	g_Dialogs[g_DialogCount].id = 0;

	LPCDLGTEMPLATE templ = (LPCDLGTEMPLATE)Fetch_Resource((LPCSTR)id, (LPCSTR)RT_DIALOG);

	if (templ == NULL) {
		return(NULL);
	}

	g_DialogCount++;

	HWND window = CreateDialogIndirectParam(instance, (LPCDLGTEMPLATE)templ, parent, proc, 0);

	if (window == NULL) {
		g_DialogCount--;
		return(NULL);
	}

	_dialog_count++;

	Add_Modeless_Dialog(window);

	EnumChildWindows(window, Resize_Dialog, TRUE);

	Resize_Dialog(window, 0);

	slot->handle = window;
	slot->id = id;

	if (force_show) {
		ShowWindow(window, SW_SHOWNORMAL);
	}

	SetForegroundWindow(window);
	SetFocus(window);
	g_TopWindow = window;
	g_TopWindowID = id;
	return(window);
}

// FUNCTION: GAMEMD 0x00775910
BOOL WS_Has_Dialog(HWND window)
{
	for (int i = 0; i < g_DialogCount; i++) {
		if (g_Dialogs[i].handle == window) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00775940
HWND WS_Find_Dialog(int id)
{
	for (int i = g_DialogCount - 1; i >= 0; i--) {
		if (id == g_Dialogs[i].id) {
			return(g_Dialogs[i].handle);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x00775970
HWND WS_Next_Upper_Dialog(HWND window)
{
	for (int i = 0; i < g_DialogCount; i++) {
		if (window == g_Dialogs[i].handle && i < g_DialogCount - 1) {
			return(g_Dialogs[i + 1].handle);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x007759B0
HWND WS_Next_Lower_Dialog(HWND window)
{
	for (int i = g_DialogCount - 1; i >= 0; i--) {
		if (window == g_Dialogs[i].handle && i > 0) {
			return(g_Dialogs[i - 1].handle);
		}
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x007759E0
int WS_Wait_Dialog(HWND window, bool (*callback)(void), bool, bool place_on_top, bool show)
{
	MSG msg;

	if (place_on_top) {
		SetForegroundWindow(window);
		SendMessage(window, OD_SETTOP, 0, TRUE);
	}

	if (show) {
		ShowWindow(window, SW_SHOWNORMAL);
	}

	while (true) {
		if (!WS_Has_Dialog(window)) {
			break;
		}
		if (callback != NULL) {
			if (callback() == TRUE) {
				WS_Destroy_Dialog(window, IDCANCEL);
			}
		}
		if (!g_PlayingNetGame) {
			Title_Screen_Restore(false);
		}

		DAT_00AA0430++;
		while (FUN_0053E770(&msg, NULL, 0, 0, 1)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (!WS_Has_Dialog(window)) {
				break;
			}
		}

		Sleep(0);
	}

	if (place_on_top) {
		SendMessage(window, OD_SETTOP, 0, FALSE);
	}

	return(g_LastResponse);
}

// FUNCTION: GAMEMD 0x00775B10
HWND WS_Top_Window(void)
{
	return(g_TopWindow);
}

// FUNCTION: GAMEMD 0x00775B20
int WS_Top_Window_ID(void)
{
	return(g_TopWindowID);
}

// FUNCTION: GAMEMD 0x00775B30
WPARAM WS_Last_Response(void)
{
	return(g_LastResponse);
}

// FUNCTION: GAMEMD 0x00775B40
BOOL CALLBACK Resize_Dialog_Proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	return(FALSE);
}

// FUNCTION: GAMEMD 0x00775B50
BOOL Get_Dialog_Resolution(unsigned short template_id, BOOL (__stdcall *dialog_proc)(HWND, UINT, WPARAM, LPARAM), int, POINT &pt)
{
	HWND window;
	tagRECT rcl;

	window = CreateDialogParam(ProgramInstance, (LPCSTR)template_id, 0, dialog_proc, 0);
	GetClientRect(window, &rcl);
	DestroyWindow(window);
	pt.x = rcl.right;
	pt.y = rcl.bottom;
	return(1);
}

// FUNCTION: GAMEMD 0x00775BA0
void Resize_Dialogs(HWND window)
{
	EnumChildWindows(window, Resize_Dialog, 1);
	Resize_Dialog(window, 0);
}

// FUNCTION: GAMEMD 0x00775BC0
BOOL CALLBACK Resize_Dialog(HWND window, LPARAM lParam)
{
	static int resize_dialog_width;
	static int resize_dialog_height;
	static int resize_dialog_scale_x = 300;
	static int resize_dialog_scale_y = 163;

	LONG w;
	LONG wheight;
	RECT rcl;
	RECT wrcl;

	char class_name[128];
	GetWindowRect(window, &rcl);
	GetClassName(window, class_name, sizeof(class_name));

	if (strcmp(class_name, WC_COMBOBOX) == 0) {
		ComboBox_GetDroppedControlRect(window, &rcl);
	}

	if (lParam) {
		HWND win = (HWND)GetWindowLong(window, GWL_HWNDPARENT);
		GetWindowRect(win, &wrcl);
		rcl.left -= wrcl.left;
		rcl.right -= wrcl.left;
		rcl.top -= wrcl.top;
		rcl.bottom -= wrcl.top;
	}

	if (resize_dialog_width == 0) {
		HWND win = CreateDialogParam(ProgramInstance, (LPCSTR)198, NULL, Resize_Dialog_Proc, NULL);
		GetClientRect(win, &wrcl);
		DestroyWindow(win);
		w = wrcl.right;
		wheight = wrcl.bottom;
		resize_dialog_width = w;
		resize_dialog_height = wheight;
	} else {
		w = resize_dialog_width;
		wheight = resize_dialog_height;
	}

	int width = resize_dialog_scale_x * (rcl.right - rcl.left + 1) / w;
	int height = resize_dialog_scale_y * (rcl.bottom - rcl.top + 1) / wheight;

	int x = resize_dialog_scale_x * rcl.left;
	int y = resize_dialog_scale_y * rcl.top;

	rcl.left = x / w;
	rcl.top = y / wheight;
	rcl.right = width + rcl.left - 1;
	rcl.bottom = height + rcl.top - 1;

	MoveWindow(window, rcl.left, rcl.top, width, height, TRUE);

	return(TRUE);
}

// FUNCTION: GAMEMD 0x00775D70
// FUNCTION: GAMEMD 0x00775D90
ArrayList<EzFont> g_EzFonts;

// FUNCTION: GAMEMD 0x00775DB0
HFONT WS_Get_Font(HDC hdc, const char * face_name, int decipt_width, int decipt_height, int attributes)
{
	EzFont font;

	for (int index = 0; index < g_EzFonts.length(); index++) {
		g_EzFonts.get(font, index);
		if (!strcmp(font.FaceName, face_name) && font.DeciPtWidth == decipt_width && font.DeciPtHeight == decipt_height && font.Attributes == attributes) {
			return(font.FontHandle);
		}
	}

	if (hdc == NULL) {
		return(NULL);
	}

	HFONT hFont = Ez_Create_Font(hdc, face_name, decipt_width, decipt_height, attributes);

	if (hFont == NULL) {
		return(NULL);
	}

	strcpy(font.FaceName, face_name);
	font.DeciPtWidth = decipt_width;
	font.DeciPtHeight = decipt_height;
	font.Attributes = attributes;
	font.FontHandle = hFont;

	if (g_EzFonts.addTail(font)) {
		return(hFont);
	}

	return(NULL);
}

// FUNCTION: GAMEMD 0x007760B0
HFONT Ez_Create_Font (HDC hdc, const char * face_name, int decipt_width,
					int decipt_height, int attributes)
{
	HFONT		hFont ;
	LOGFONT	lf ;
	POINT		pt ;
	TEXTMETRIC tm ;

	SaveDC (hdc) ;

	SetGraphicsMode (hdc, GM_ADVANCED) ;
	ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
	SetViewportOrgEx (hdc, 0, 0, NULL) ;
	SetWindowOrgEx   (hdc, 0, 0, NULL) ;

	pt.x = decipt_width ;
	pt.y = decipt_height ;

	DPtoLP (hdc, &pt, 1) ;

	lf.lfHeight			= -pt.y ;
	lf.lfWidth			= 0 ;
	lf.lfEscapement		= 0 ;
	lf.lfOrientation	= 0 ;
	lf.lfWeight		 = attributes & EZ_ATTR_BOLD	   ? 700 : 0 ;
	lf.lfItalic		 = attributes & EZ_ATTR_ITALIC    ?   1 : 0 ;
	lf.lfUnderline 	 = attributes & EZ_ATTR_UNDERLINE ?   1 : 0 ;
	lf.lfStrikeOut 	 = attributes & EZ_ATTR_STRIKEOUT ?   1 : 0 ;
	lf.lfCharSet		= ANSI_CHARSET ;
	lf.lfOutPrecision	= 0 ;
	lf.lfClipPrecision	= 0 ;
	lf.lfQuality		= 0 ;
	lf.lfPitchAndFamily	= 0 ;

	strcpy (lf.lfFaceName, face_name) ;

	hFont = CreateFontIndirect (&lf) ;

	if (decipt_width != 0) {
		hFont = (HFONT) SelectObject (hdc, hFont) ;
		GetTextMetrics (hdc, &tm) ;
		DeleteObject (SelectObject (hdc, hFont)) ;
		lf.lfWidth = (int) (tm.tmAveCharWidth *
									fabs (pt.x) / fabs (pt.y) + 0.5);
		hFont = CreateFontIndirect (&lf) ;
	}

	RestoreDC (hdc, -1);
	return(hFont);
}
