// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include "mplayer.h"
#include "_surface.h"
#include "addon.h"
#include "init.h"
#include "msgbox.h"
#include "nulldlg.h"
#include "ownrdraw.h"
#include "session.h"
#include "worlddom.h"
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"
#include "geometry.h"

namespace UI { int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }

// SYNTHETIC: GAMEMD 0x005C5E70 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x005C5E90 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x005C5EC0 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x005C5EE0 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x005C5F00 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x005C5F20 SYMBOL
// _$E42
// SYNTHETIC: GAMEMD 0x005C5F40 SYMBOL
// _$E45
// SYNTHETIC: GAMEMD 0x005C5F60 SYMBOL
// _$E48
// SYNTHETIC: GAMEMD 0x005C5FA0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x005C5FD0 SYMBOL
// _$E54
// SYNTHETIC: GAMEMD 0x005C6000 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x005C6030 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x005C6040 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x005C6060 SYMBOL
// _$E66

// FUNCTION: GAMEMD 0x005C6080
BOOL CALLBACK Surrender_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);

	if (rc == 0) {
		if (message == WM_COMMAND) {
			*(int *)GetWindowLong(window, DWL_USER) = LOWORD(wparam);
		}
		return 0;
	}
	return rc;
}

// FUNCTION: GAMEMD 0x005C60D0
int __fastcall Surrender_Dialog(wchar_t const * text)
{
	return WWMessageBox().Process(text, StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MPLayer.CPP", 0x6c), StringTable::LoadString("TXT_CANCEL", NULL, "D:\\ra2mdpost\\MPLayer.CPP", 0x6c), NULL) == 0;
}

// FUNCTION: GAMEMD 0x005C6120
void Clear_Vector(DynamicVectorClass <NodeNameType *> * vector)
{
	int i;

	for (i = 0; i < vector->Count(); i++) {
		delete (*vector)[i];
	}
	vector->Clear();
}
