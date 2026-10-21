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
extern const double DAT_007EE4F8;
extern const double DAT_007EE500;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

int DAT_00ABF580, DAT_00ABF584, DAT_00ABF588;
namespace UI { int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }

// FUNCTION: 0x005C5E90 $E30
double DAT_00ABF540 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005C5EC0 $E33
double DAT_00ABF590 = DAT_00ABF540 / DAT_007EE4F8;

// FUNCTION: 0x005C5EE0 $E36
double DAT_00ABF578 = DAT_007E1718 / DAT_00ABF590;

// FUNCTION: 0x005C5F00 $E39
double DAT_00ABF570 = DAT_007E1720 * DAT_007EE500;

// FUNCTION: 0x005C5F20 $E42
double DAT_00ABF548 = DAT_007E1728 * DAT_007EE500;

// FUNCTION: 0x005C5F40 $E45
double DAT_00ABF520 = DAT_007E1730 * DAT_007EE500;

// FUNCTION: 0x005C5F60 $E48
int DAT_00ABF598 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF520 - DAT_00ABF548) * DAT_00ABF540));

// FUNCTION: 0x005C5FA0 $E51
double DAT_00ABF550 = FUN_004CADE0(DAT_00ABF598 * DAT_007E1740);

// FUNCTION: 0x005C5FD0 $E54
double DAT_00ABF568 = FUN_004CADE0((DAT_00ABF598 * 2) / DAT_00ABF540);

// FUNCTION: 0x005C6000 $E57
int DAT_00ABF58C = (int)(DAT_00ABF598 * 4 + DAT_007E1738);

// FUNCTION: 0x005C6030 $E60
ShortPair DAT_00ABF538;

// FUNCTION: 0x005C6040 $E63
IntTriple DAT_00ABF558;

// FUNCTION: 0x005C6060
void Reset_Window_Size(void)
{
	DAT_00ABF588 = 0;
	DAT_00ABF580 = 128;
	DAT_00ABF584 = 128;
}

// FUNCTION: 0x005C6080
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

// FUNCTION: 0x005C60D0
int __fastcall Surrender_Dialog(wchar_t const * text)
{
	return WWMessageBox().Process(text, StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MPLayer.CPP", 0x6c), StringTable::LoadString("TXT_CANCEL", NULL, "D:\\ra2mdpost\\MPLayer.CPP", 0x6c), NULL) == 0;
}

// FUNCTION: 0x005C6120
void Clear_Vector(DynamicVectorClass <NodeNameType *> * vector)
{
	int i;

	for (i = 0; i < vector->Count(); i++) {
		delete (*vector)[i];
	}
	vector->Clear();
}
