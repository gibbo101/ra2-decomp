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

BOOL CALLBACK Resize_Dialog(HWND window, LPARAM lParam);

BOOL CALLBACK Resize_Dialog_Proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	return(FALSE);
}

struct EzFont {
	char FaceName[128];
	int DeciPtWidth;
	int DeciPtHeight;
	int Attributes;
	HFONT FontHandle;
};

ArrayList<EzFont> g_EzFonts;

#define EZ_ATTR_BOLD		  1
#define EZ_ATTR_ITALIC		  2
#define EZ_ATTR_UNDERLINE	  4
#define EZ_ATTR_STRIKEOUT	  8

HFONT Ez_Create_Font (HDC hdc, const char * face_name, int decipt_width, int decipt_height, int attributes);

// FUNCTION: 0x00775690
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

// FUNCTION: 0x00775910
BOOL WS_Has_Dialog(HWND window)
{
	for (int i = 0; i < g_DialogCount; i++) {
		if (g_Dialogs[i].handle == window) {
			return(true);
		}
	}
	return(false);
}

// FUNCTION: 0x00775940
HWND WS_Find_Dialog(int id)
{
	for (int i = g_DialogCount - 1; i >= 0; i--) {
		if (id == g_Dialogs[i].id) {
			return(g_Dialogs[i].handle);
		}
	}
	return(NULL);
}

// FUNCTION: 0x00775970
HWND WS_Next_Upper_Dialog(HWND window)
{
	for (int i = 0; i < g_DialogCount; i++) {
		if (window == g_Dialogs[i].handle && i < g_DialogCount - 1) {
			return(g_Dialogs[i + 1].handle);
		}
	}
	return(NULL);
}

// FUNCTION: 0x007759B0
HWND WS_Next_Lower_Dialog(HWND window)
{
	for (int i = g_DialogCount - 1; i >= 0; i--) {
		if (window == g_Dialogs[i].handle && i > 0) {
			return(g_Dialogs[i - 1].handle);
		}
	}
	return(NULL);
}

// FUNCTION: 0x00775B50
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

// FUNCTION: 0x00775BA0
void Resize_Dialogs(HWND window)
{
	EnumChildWindows(window, Resize_Dialog, 1);
	Resize_Dialog(window, 0);
}

// FUNCTION: 0x00775BC0
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

// FUNCTION: 0x00775DB0
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

// FUNCTION: 0x007760B0
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
