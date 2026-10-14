// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "loaddlg.h"
#include "campaign.h"
#include "cd.h"
#include "conquer.h"
#include "data.h"
#include "geometry.h"
#include "globals.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "msgbox.h"
#include "ownrdraw.h"
#include "saveload.h"
#include "savever.h"
#include "scenario.h"
#include "session.h"
#include "win.h"
#include "ccfile.h"
#include "vector.h"


namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
bool On_WM_MOVING(HWND window, WPARAM wparam, LPARAM lparam);
void FUN_00607FD0(HWND dialog);

// FUNCTION: 0x00558550 $E32
// FUNCTION: 0x00558580 $E35
// FUNCTION: 0x005585A0 $E38
// FUNCTION: 0x005585C0 $E41
// FUNCTION: 0x005585E0 $E44
// FUNCTION: 0x00558600 $E47
// FUNCTION: 0x00558620 $E50
// FUNCTION: 0x00558660 $E53
// FUNCTION: 0x00558690 $E56
// FUNCTION: 0x005586C0 $E59
// FUNCTION: 0x005586F0 $E62
// FUNCTION: 0x00558700 $E65

// FUNCTION: 0x00558720 $E68

// FUNCTION: 0x00558740
LoadOptionsClass::LoadOptionsClass(void) :
	Files(0,0),
	Style(NONE),
	Description(NULL),
	Callback(NULL)
{
	Style = NONE;
	Description = NULL;
	Callback = NULL;
	Extension = "SAV";
	MinSpaceRequired = 2048;
	Files.Clear();
}

// FUNCTION: 0x00558790
LoadOptionsClass::~LoadOptionsClass(void)
{
	for (int i = 0; i < Files.Count(); i++) {
		delete Files[i];
	}
	Files.Clear();
}

// FUNCTION: 0x005587F0
bool LoadOptionsClass::Load(void)
{
	Style = LOAD;
	Description = NULL;
	return(Dialog());
}

// FUNCTION: 0x00558810
bool LoadOptionsClass::Save(wchar_t const * description)
{
	Style = SAVE;
	if (description == NULL) {
		description = Scen->Description;
	}
	Description = (wchar_t *)description;
	return(Dialog());
}

// FUNCTION: 0x00558840
bool LoadOptionsClass::Delete(void)
{
	Style = WWDELETE;
	Description = NULL;
	return(Dialog());
}

// FUNCTION: 0x00558860
void LoadOptionsClass::Load_Dialog_On_WM_COMMAND(HWND window, WPARAM wparam, LPARAM lparam, int id)
{
	LoadOptionsClass * _this = (LoadOptionsClass *)GetWindowLong(window, DWL_USER);
	switch ((int)wparam) {
		case 0x525:
			if (id == 2 && ListBox_GetCount((HWND)lparam) > 0) {
				_this->State = (LoadDialogState)0x40F;
			}
			break;

		case 0x40F:
			if (id == 0) {
				_this->State = (LoadDialogState)wparam;
			}
			break;

		case 0x686:
			_this->State = (LoadDialogState)2;
			break;
	}
}

// FUNCTION: 0x005588E0
void LoadOptionsClass::Save_Dialog_On_WM_COMMAND(HWND window, WPARAM wparam, LPARAM lparam, int id)
{
	LoadOptionsClass * _this = (LoadOptionsClass *)GetWindowLong(window, DWL_USER);
	switch ((int)wparam) {
		case 0x527:
			if (id == 1 && ListBox_GetCount((HWND)lparam) > 0) {
				HWND list = (HWND)lparam;
				int row = ListBox_GetCurSel(list);
				if (row != LB_ERR) {
					FileEntryClass * fdata = (FileEntryClass *)ListBox_GetItemData(list, row);
					if (!fdata->Valid || fdata->Old) {
						wchar_t * text = _this->Description;
						if (text != NULL) {
							HWND edit = GetDlgItem(window, 0x526);
							SendMessage(edit, 0x4B2, 0, (LPARAM)text);
						}
					} else {
						HWND edit = GetDlgItem(window, 0x526);
						SendMessage(edit, 0x4B2, 0, (LPARAM)fdata->Descr);
					}
					SetFocus(GetDlgItem(window, 0x526));
					Edit_SetSel(GetDlgItem(window, 0x526), 0, -1);
				}
			}
			break;

		case 0x686:
			if (id == 0) {
				_this->State = (LoadDialogState)2;
			}
			break;

		case 0x6C7:
			if (id == 0) {
				_this->State = (LoadDialogState)1;
			}
			break;
	}
}

// FUNCTION: 0x005589F0
void LoadOptionsClass::Delete_Dialog_On_WM_COMMAND(HWND window, WPARAM wparam, LPARAM lparam, int id)
{
	LoadOptionsClass * _this = (LoadOptionsClass *)GetWindowLong(window, DWL_USER);
	switch ((int)wparam) {
		case 0x686:
			if (id == 0) {
				_this->State = (LoadDialogState)2;
			}
			break;

		case 0x6C8:
			if (id == 0) {
				_this->State = (LoadDialogState)1;
			}
			break;
	}
}

// FUNCTION: 0x00558A30
LRESULT CALLBACK LoadOptionsClass::Load_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);

	if (rc == 0) {

		switch (message) {
			case WM_PAINT:
				FUN_00607FD0(window);
				return(TRUE);

			case WM_MOVING:
				return(On_WM_MOVING(window, wparam, lparam));

			case WM_COMMAND:
				Load_Dialog_On_WM_COMMAND(window, LOWORD(wparam), lparam, HIWORD(wparam));
				break;

			case OD_SUBCLASSED:
				SendDlgItemMessage(window, 0x525, OD_ADDCOLUMN, 0xF9, 2);
				SendDlgItemMessage(window, 0x525, OD_ADDCOLUMN, 0x38, 255);
				SendDlgItemMessage(window, 0x525, OD_ADDCOLUMN, 0, 315);
				break;
		}
		return(FALSE);
	}
	return(rc);
}

// FUNCTION: 0x00558B90
LRESULT CALLBACK LoadOptionsClass::Save_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);

	if (rc == 0) {

		switch (message) {
			case WM_PAINT:
				FUN_00607FD0(window);
				return(TRUE);

			case WM_MOVING:
				return(On_WM_MOVING(window, wparam, lparam));

			case WM_COMMAND:
				Save_Dialog_On_WM_COMMAND(window, LOWORD(wparam), lparam, HIWORD(wparam));
				break;

			case 0x4DA:
				((LoadOptionsClass *)GetWindowLong(window, DWL_USER))->State = (LoadDialogState)1;
				return(FALSE);

			case OD_SUBCLASSED:
				SendMessage(GetDlgItem(window, 0x526), EM_SETLIMITTEXT, 79, 0);
				SendDlgItemMessage(window, 0x527, OD_ADDCOLUMN, 0xF9, 2);
				SendDlgItemMessage(window, 0x527, OD_ADDCOLUMN, 0x38, 255);
				SendDlgItemMessage(window, 0x527, OD_ADDCOLUMN, 0, 315);
				break;
		}
		return(FALSE);
	}
	return(rc);
}

// FUNCTION: 0x00558CB0
LRESULT CALLBACK LoadOptionsClass::Delete_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);

	if (rc == 0) {

		switch (message) {
			case WM_PAINT:
				FUN_00607FD0(window);
				return(TRUE);

			case WM_MOVING:
				return(On_WM_MOVING(window, wparam, lparam));

			case WM_COMMAND:
				Delete_Dialog_On_WM_COMMAND(window, LOWORD(wparam), lparam, HIWORD(wparam));
				break;

			case OD_SUBCLASSED:
				SendDlgItemMessage(window, 0x528, OD_ADDCOLUMN, 0xF9, 2);
				SendDlgItemMessage(window, 0x528, OD_ADDCOLUMN, 0x38, 255);
				SendDlgItemMessage(window, 0x528, OD_ADDCOLUMN, 0, 315);
				break;
		}
		return(FALSE);
	}
	return(rc);
}

// FUNCTION: 0x005595F0
void LoadOptionsClass::Pick_Filename(char * file_name)
{
	CCFileClass file;
	do {
		sprintf(file_name, "SAVE%04lX.%3s", rand(), Extension);
		file.Set_Name(file_name);
	} while (file.Is_Available() == true);
}

// FUNCTION: 0x00559670
void LoadOptionsClass::Clear_List(void)
{
	for (int i = 0; i < Files.Count(); i++) {
		delete Files[i];
	}
	Files.Clear();
}

// FUNCTION: 0x00559EB0
bool LoadOptionsClass::Delete_File(const char * file_name)
{
	if (DeleteFile(file_name) == TRUE) {
		return(true);
	}
	return(false);
}
