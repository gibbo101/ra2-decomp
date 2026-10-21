// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msgbox.h"
#include "_xmouse.h"
#include "globals.h"
#include "init.h"
#include <windows.h>
#undef LoadString

class Surface;
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);

namespace UI {
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	bool Updated(void);
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}

namespace Game {
	bool __fastcall OnWindowMoving(HWND window, WPARAM wparam, LPARAM lparam);
}
BOOL CALLBACK FUN_005D36A0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void __fastcall FUN_005D3760(HWND window, int id, int control, int notify_code);
int FUN_00ABFC18;
int FUN_00ABFC1C;
int FUN_00ABFC20;
int FUN_00ABFC24;

// FUNCTION: 0x005D3490
int WWMessageBox::Process(wchar_t const * msg, wchar_t const * b1txt, wchar_t const * b2txt, wchar_t const * b3txt, bool preserve)
{
	int retval = -1;
	int numbuttons = 0;

	int mousestate = MouseCursor->Get_Mouse_State();

	bool hasmsg = (msg != NULL && msg[0] != L'\0');
	bool has1 = (b1txt != NULL && b1txt[0] != L'\0');
	bool has2 = (b2txt != NULL && b2txt[0] != L'\0');
	bool has3 = (b3txt != NULL && b3txt[0] != L'\0');

	FUN_0052FEC0(NULL, true);

	int id = 0xCE;
	if (has3) {
		id = 0x121;
	} else if (has2) {
		id = 0x120;
	}

	HWND dialog = UI::BeginDialog(id, FUN_005D36A0, 0);
	if (dialog != NULL) {
		SetWindowLong(dialog, DWL_USER, (LONG)&retval);

		if (hasmsg) {
			HWND item = GetDlgItem(dialog, 0x5B0);
			SendMessage(item, 0x4B2, 0, (LPARAM)msg);
		}
		if (has1) {
			HWND item = GetDlgItem(dialog, 0x5AE);
			SendMessage(item, 0x4B2, 0, (LPARAM)b1txt);
			numbuttons = 1;
		}
		if (has2) {
			HWND item = GetDlgItem(dialog, 2);
			SendMessage(item, 0x4B2, 0, (LPARAM)b2txt);
			numbuttons = 2;
		}
		if (has3) {
			HWND item = GetDlgItem(dialog, 0x5AF);
			SendMessage(item, 0x4B2, 0, (LPARAM)b3txt);
			numbuttons = 3;
		}

		UI::FocusOnWindow(dialog);

		if (numbuttons > 0) {
			while (MouseCursor->Get_Mouse_State()) {
				MouseCursor->Show_Mouse();
			}
			while (retval < 0) {
				if (UI::Updated()) {
					break;
				}
				if (!GameActive) {
					Title_Screen_Restore(false);
				}
			}
		} else {
			retval = 0;
		}

		UI::EndDialog(dialog);
	}

	while (mousestate < MouseCursor->Get_Mouse_State()) {
		MouseCursor->Hide_Mouse();
	}

	return retval;
}

// FUNCTION: 0x005D36A0
BOOL CALLBACK FUN_005D36A0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	BOOL rc = UI::StandardWndProc(window, message, wparam, lparam);

	if (rc == FALSE) {
		switch (message) {
			case WM_COMMAND:
				FUN_005D3760(window, LOWORD(wparam), 0, HIWORD(wparam));
				rc = FALSE;
				break;

			case WM_MOVING:
				rc = Game::OnWindowMoving(window, wparam, lparam);
				break;

			default:
				rc = FALSE;
				break;
		}
	}

	return rc;
}

// FUNCTION: 0x005D3760
void __fastcall FUN_005D3760(HWND window, int id, int control, int notify_code)
{
	int * retval = (int *)GetWindowLong(window, DWL_USER);

	switch (id) {
		case 1:
		case 2:
			if (notify_code == 0) {
				*retval = 1;
			}
			break;

		case 0x5AE:
			if (notify_code == 0) {
				*retval = 0;
			}
			break;

		case 0x5AF:
			if (notify_code == 0) {
				*retval = 2;
			}
			break;
	}
}

// FUNCTION: 0x005D37C0
void FUN_005D37C0(void)
{
	FUN_00ABFC18 = 0;
	FUN_00ABFC1C = 0;
	FUN_00ABFC20 = 0;
	FUN_00ABFC24 = 0;
}
