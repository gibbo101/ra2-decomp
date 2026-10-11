// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "serialed.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "modemreg.h"
#include "msgbox.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "trim.h"
#include "winfix.h"

// FUNCTION: 0x006966D0
void SerialEdit::Set_Dial_Method(void)
{
	HWND handle;

	bool is_tone_dialing = TempSettings.DialMethod == DIAL_TOUCH_TONE;
	handle = GetDlgItem(_Dialog, IDC_SERIAL_TONE);
	if (handle) {
		Button_SetCheck(handle, is_tone_dialing == true);
	}
	handle = GetDlgItem(_Dialog, IDC_SERIAL_PULSE);
	if (handle) {
		Button_SetCheck(handle, is_tone_dialing == false);
	}
}

// FUNCTION: 0x00696C40
void SerialEdit::Delete_Init_String(void)
{
	int initstr_index;

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);
	if (handle) {
		if (ListBox_GetCount(handle) > 0) {
			initstr_index = ListBox_GetCurSel(handle);
			if (initstr_index != LB_ERR) {
				ListBox_DeleteString(handle, initstr_index);
				Session.InitStrings.Delete_Index(initstr_index);
				if (initstr_index >= ListBox_GetCount(handle)) {
					initstr_index--;
				}
				ListBox_SetCurSel(handle, initstr_index);
				ListBox_SetTopIndex(handle, initstr_index);
				TempSettings.InitStringIndex = initstr_index;
			}
		}
	}
}
