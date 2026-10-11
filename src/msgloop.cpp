// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msgloop.h"
#include "_tooltip.h"
#include "cctooltip.h"
#include "vector.h"

DynamicVectorClass<HWND> _ModelessDialogs;

struct AcceleratorTracker {
	AcceleratorTracker(HWND window = NULL, HACCEL accelerator = NULL) : Accelerator(accelerator), Window(window) {}

	int operator == (AcceleratorTracker const & acc) const {return(Accelerator == acc.Accelerator && Window == acc.Window);}
	int operator != (AcceleratorTracker const & acc) const {return(!(*this == acc));}

	HACCEL Accelerator;
	HWND Window;
};

static DynamicVectorClass<AcceleratorTracker> _Accelerators;

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
