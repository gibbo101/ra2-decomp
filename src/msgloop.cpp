// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msgloop.h"
#include "_tooltip.h"
#include "cctooltip.h"
#include "vector.h"

// FUNCTION: 0x005D4C50 $E7
// FUNCTION: 0x005D4C90 $E5
DynamicVectorClass<HWND> _ModelessDialogs;

struct AcceleratorTracker {
	AcceleratorTracker(HWND window = NULL, HACCEL accelerator = NULL) : Accelerator(accelerator), Window(window) {}

	int operator == (AcceleratorTracker const & acc) const {return(Accelerator == acc.Accelerator && Window == acc.Window);}
	int operator != (AcceleratorTracker const & acc) const {return(!(*this == acc));}

	HACCEL Accelerator;
	HWND Window;
};

// FUNCTION: 0x005D4CD0 $E12
// FUNCTION: 0x005D4D10 $E10
static DynamicVectorClass<AcceleratorTracker> _Accelerators;

bool (*Message_Intercept_Handler)(MSG &msg);

namespace Game { extern HWND hWnd; }
namespace CopyProtection { void __fastcall DispatchLauncherMessage(MSG *msg); }
int __fastcall FUN_0053E770(MSG *msg, HWND window, unsigned int filter_min, unsigned int filter_max, unsigned int remove);
int __fastcall FUN_0053E730(MSG *msg, HWND window, unsigned int filter_min, unsigned int filter_max);
extern int DAT_00AA0430;

// FUNCTION: 0x005D4D50
void Windows_Message_Handler(void)
{
	if (Game::hWnd == 0) return;

	MSG msg;

	DAT_00AA0430++;
	while (FUN_0053E770(&msg, NULL, 0, 0, 0)) {
		if (!FUN_0053E730(&msg, NULL, 0, 0)) {
			return;
		}
		CopyProtection::DispatchLauncherMessage(&msg);

		if (ToolTips != NULL) {
			ToolTips->Message_Handler(&msg);
		}

		bool processed = false;
		for (int index = 0; index < _ModelessDialogs.Count(); index++) {
			if (IsDialogMessage(_ModelessDialogs[index], &msg)) {
				processed = true;
				break;
			}
		}
		if (processed) continue;

		for (int aindex = 0; aindex < _Accelerators.Count(); aindex++) {
			if (TranslateAccelerator(_Accelerators[aindex].Window, _Accelerators[aindex].Accelerator, &msg)) {
				processed = true;
				break;
			}
		}
		if (processed) continue;

		if (Message_Intercept_Handler != NULL) {
			processed = Message_Intercept_Handler(msg);
		}
		if (processed) continue;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// FUNCTION: 0x005D4E70
void Add_Modeless_Dialog(HWND dialog)
{
	_ModelessDialogs.Add(dialog);
}

// FUNCTION: 0x005D4ED0
void Remove_Modeless_Dialog(HWND dialog)
{
	_ModelessDialogs.Delete(dialog);
}

// FUNCTION: 0x005D4F20
HWND Get_Modeless_Dialog_From_Name(const char *name)
{
	static char _wname[100];

	for (int i = 0; i < _ModelessDialogs.Count(); i++) {
		GetWindowText(_ModelessDialogs[i], _wname, sizeof(_wname) - 1);
		if (!strcmp(_wname, name)) {
			return(_ModelessDialogs[i]);
		}
	}
	return(NULL);
}

// FUNCTION: 0x005D4FA0
void Add_Accelerator(HWND window, HACCEL accelerator)
{
	_Accelerators.Add(AcceleratorTracker(window, accelerator));
}

// FUNCTION: 0x005D5010
void Remove_Accelerator(HACCEL accelerator)
{
	for (int index = 0; index < _Accelerators.Count(); index++) {
		if (_Accelerators[index].Accelerator == accelerator) {
			_Accelerators.Delete_Index(index);
			break;
		}
	}
}
