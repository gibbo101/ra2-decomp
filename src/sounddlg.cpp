// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "sounddlg.h"
#include "globals.h"
#include "init.h"
#include "goptions.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x006B6210 SYMBOL
// _$E2


void __cdecl FUN_004068E0(char const * format, ...);
extern bool DAT_00B0B800;

namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
}

// FUNCTION: GAMEMD 0x006B6230
void SoundControlsClass::Dialog(void)
{
	int rc = -1;
	FUN_004068E0("SoundControls: GameSpeed = %d, ScrollRate = %d, Detail = %d\n", Options.GameSpeed, Options.ScrollRate, Options.DetailLevel);
	DAT_00B0B800 = false;

	HWND dialog;
	if (!GameActive) {
		dialog = UI::BeginDialog(0xd6, Sound_Option_Dialog_Func, 0);
	} else {
		dialog = UI::BeginDialog(0xb8, Sound_Option_Dialog_Func, 0);
	}

	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		UI::FocusOnWindow(dialog);
		while (rc == -1) {
			if (UI::Updated() == true) {
				rc = 2;
			}
			if (!GameActive) {
				Title_Screen_Restore(false);
			}
		}
		UI::EndDialog(dialog);
	}

	FUN_004068E0("SoundControls: GameSpeed = %d, ScrollRate = %d, Detail = %d\n", Options.GameSpeed, Options.ScrollRate, Options.DetailLevel);
}
