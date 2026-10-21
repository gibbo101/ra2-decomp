// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "win.h"

BOOL Get_Display_Rect(HWND window, LPRECT rect);

HWND WS_Create_Dialog(HINSTANCE instance, int id, HWND parent, DLGPROC proc, BOOL force_show);
bool WS_Destroy_Dialog(HWND window, int id);

HWND WS_Find_Dialog(int id);
BOOL WS_Has_Dialog(HWND window);

int WS_Wait_Dialog(HWND window, bool (*callback)(void), bool=false, bool place_on_top=true, bool show=false);

HWND WS_Top_Window(void);
int WS_Top_Window_ID(void);
extern HWND g_TopWindow;

int WS_Get_Saved_Value(int control_id, unsigned char * dest, int dest_size);
void WS_Clear_Saved_Values(void);

HWND WS_Next_Upper_Dialog(HWND window);
HWND WS_Next_Lower_Dialog(HWND window);

void Resize_Dialogs(HWND window);

HFONT WS_Get_Font(HDC hdc, const char * face_name, int decipt_width, int decipt_height, int attributes);

struct WSDialogStruct {

	HWND handle;

	int id;
};

extern WSDialogStruct g_Dialogs[64];
extern int g_DialogCount;
extern HWND g_TopWindow;
extern int g_TopWindowID;
extern int g_LastResponse;
