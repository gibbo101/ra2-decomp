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
extern const double DAT_007F1180;
extern const double DAT_007F1188;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x006954E0 $E33
double DAT_00B05768 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00695510 $E36
double DAT_00B057B8 = DAT_00B05768 / DAT_007F1180;

// FUNCTION: 0x00695530 $E39
double DAT_00B057A0 = DAT_007E1718 / DAT_00B057B8;

// FUNCTION: 0x00695550 $E42
double DAT_00B05798 = DAT_007E1720 * DAT_007F1188;

// FUNCTION: 0x00695570 $E45
double DAT_00B05770 = DAT_007E1728 * DAT_007F1188;

// FUNCTION: 0x00695590 $E48
double DAT_00B05748 = DAT_007E1730 * DAT_007F1188;

// FUNCTION: 0x006955B0 $E51
int DAT_00B057C0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B05748 - DAT_00B05770) * DAT_00B05768));

// FUNCTION: 0x006955F0 $E54
double DAT_00B05778 = FUN_004CADE0(DAT_00B057C0 * DAT_007E1740);

// FUNCTION: 0x00695620 $E57
double DAT_00B05790 = FUN_004CADE0((DAT_00B057C0 * 2) / DAT_00B05768);

// FUNCTION: 0x00695650 $E60
int DAT_00B057B4 = (int)(DAT_00B057C0 * 4 + DAT_007E1738);

// FUNCTION: 0x00695680 $E63
ShortPair DAT_00B05760;

// FUNCTION: 0x00695690 $E66
IntTriple DAT_00B05780;

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
