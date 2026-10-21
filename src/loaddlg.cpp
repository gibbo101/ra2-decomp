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
#include "stringtable.h"
#include "_xmouse.h"



namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
bool On_WM_MOVING(HWND window, WPARAM wparam, LPARAM lparam);
void FUN_00607FD0(HWND dialog);

// SYNTHETIC: GAMEMD 0x00558550 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00558580 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x005585A0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x005585C0 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x005585E0 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x00558600 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x00558620 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x00558660 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x00558690 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x005586C0 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x005586F0 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x00558700 SYMBOL
// _$E65

// SYNTHETIC: GAMEMD 0x00558720 SYMBOL
// _$E68

// FUNCTION: GAMEMD 0x00558740
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

// FUNCTION: GAMEMD 0x00558790
LoadOptionsClass::~LoadOptionsClass(void)
{
	for (int i = 0; i < Files.Count(); i++) {
		delete Files[i];
	}
	Files.Clear();
}

// FUNCTION: GAMEMD 0x005587F0
bool LoadOptionsClass::Load(void)
{
	Style = LOAD;
	Description = NULL;
	return(Dialog());
}

// FUNCTION: GAMEMD 0x00558810
bool LoadOptionsClass::Save(wchar_t const * description)
{
	Style = SAVE;
	if (description == NULL) {
		description = Scen->Description;
	}
	Description = (wchar_t *)description;
	return(Dialog());
}

// FUNCTION: GAMEMD 0x00558840
bool LoadOptionsClass::Delete(void)
{
	Style = WWDELETE;
	Description = NULL;
	return(Dialog());
}

// FUNCTION: GAMEMD 0x00558860
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

// FUNCTION: GAMEMD 0x005588E0
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

// FUNCTION: GAMEMD 0x005589F0
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

// FUNCTION: GAMEMD 0x00558A30
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

// FUNCTION: GAMEMD 0x00558B90
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

// FUNCTION: GAMEMD 0x00558CB0
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

// FUNCTION: GAMEMD 0x005595F0
void LoadOptionsClass::Pick_Filename(char * file_name)
{
	CCFileClass file;
	do {
		sprintf(file_name, "SAVE%04lX.%3s", rand(), Extension);
		file.Set_Name(file_name);
	} while (file.Is_Available() == true);
}

// FUNCTION: GAMEMD 0x00559670
void LoadOptionsClass::Clear_List(void)
{
	for (int i = 0; i < Files.Count(); i++) {
		delete Files[i];
	}
	Files.Clear();
}

// FUNCTION: GAMEMD 0x005596A0
void LoadOptionsClass::Fill_List(HWND window)
{
	OwnerDraw::CellData thecell;
	FileEntryClass * fdata = NULL;
	WIN32_FIND_DATAA ff;

	Clear_List();

	if (Style == SAVE) {
		fdata = new FileEntryClass;
		swprintf(fdata->Descr, StringTable::LoadString("TXT_EMPTY_SLOT", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x306));
		if (PlayerPtr != NULL) {
			fdata->Scenario = Scen->Scenario;
			fdata->House = Scen->PlayerHouse;
			fdata->Num = Scen->Campaign;
			wcscpy(fdata->PlayerName, PlayerPtr->Class->UIName);
		} else {
			fdata->Scenario = 0;
			fdata->House = (HousesType)Session.House;
			fdata->Num = -1;
			swprintf(fdata->PlayerName, L"%s", FUN_00735120(Session.Handle));
		}
		SYSTEMTIME time;
		GetSystemTime(&time);
		SystemTimeToFileTime(&time, &fdata->DateTime);
		fdata->Type = Session.Type;
		fdata->Valid = false;
		Files.Add(fdata);
	}

	char buffer[128];
	sprintf(buffer, "*.%3s", Extension);

	HANDLE hFind = FindFirstFile(buffer, &ff);
	fdata = NULL;

	if (hFind != INVALID_HANDLE_VALUE) {
		while (true) {
			if ((ff.dwFileAttributes & (FILE_ATTRIBUTE_TEMPORARY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN)) == 0) {
				if (fdata == NULL) {
					fdata = new FileEntryClass;
				}

				if (Read_File(fdata, &ff) == true) {
					Files.Add(fdata);
					fdata = NULL;
				}
			}

			if (!FindNextFile(hFind, &ff)) {
				break;
			}
		}

		if (fdata != NULL) {
			delete fdata;
		}

		FindClose(hFind);
	}

	if (Files.Count() > 0) {

		qsort((void *)(&Files[0]), Files.Count(), sizeof(class FileEntryClass *), LoadOptionsClass::Compare);

		ListBox_ResetContent(window);

		for (int i = 0; i < Files.Count(); i++) {
			fdata = Files[i];

			if (!fdata->Old) {

				int row = SendMessage(window, 0x4CD, 0, (LPARAM)fdata);

				if (fdata->Type != GAME_NORMAL) {
					thecell.type = OwnerDraw::CellData::TEXT;
					thecell.string.set(L"*");
					SendMessage(window, OD_SETCELL, MAKEWPARAM(200, row), (LPARAM)&thecell);
				}

				if (fdata->DateTime.dwHighDateTime != -1 && fdata->DateTime.dwLowDateTime != -1) {
					FILETIME ft;
					SYSTEMTIME time;
					wchar_t wbuffer[128];
					FileTimeToLocalFileTime(&fdata->DateTime, &ft);
					FileTimeToSystemTime(&ft, &time);
					GetDateFormat(LANG_USER_DEFAULT, TIME_NOMINUTESORSECONDS, &time, NULL, buffer, sizeof(buffer));
					thecell.type = OwnerDraw::CellData::TEXT;
					int length = MultiByteToWideChar(0, 0, buffer, -1, wbuffer, 128);
					wbuffer[127] = 0;
					wbuffer[length] = 0;
					thecell.string.set(wbuffer);
					SendMessage(window, OD_SETCELL, MAKEWPARAM(255, row), (LPARAM)&thecell);
					GetTimeFormat(LANG_USER_DEFAULT, 0, &time, NULL, buffer, sizeof(buffer));
					thecell.type = OwnerDraw::CellData::TEXT;
					length = MultiByteToWideChar(0, 0, buffer, -1, wbuffer, 128);
					wbuffer[127] = 0;
					wbuffer[length] = 0;
					thecell.string.set(wbuffer);
					SendMessage(window, OD_SETCELL, MAKEWPARAM(315, row), (LPARAM)&thecell);
				}

				ListBox_SetItemData(window, row, (LPARAM)fdata);
			}
		}

		switch (Style) {
			case LOAD: {
					for (int i = 0; i < Files.Count(); i++) {
						if (Files[i]->Valid && !Files[i]->Old) {
							ListBox_SetCurSel(window, i);
							ListBox_SetTopIndex(window, i);
							break;
						}
					}
				}
				break;

			case SAVE:
			case WWDELETE:
				ListBox_SetCurSel(window, 0);
				ListBox_SetTopIndex(window, 0);
				break;
		}
	}
}

// FUNCTION: GAMEMD 0x00559C20
bool LoadOptionsClass::Files_Present(void)
{
	bool files_found = false;
	char pattern[64];
	WIN32_FIND_DATAA find_data;
	FileEntryClass entry;

	sprintf(pattern, "*.%3s", Extension);

	HANDLE hFind = FindFirstFile(pattern, &find_data);

	if (hFind != INVALID_HANDLE_VALUE) {
		while (true) {
			if ((find_data.dwFileAttributes & (FILE_ATTRIBUTE_TEMPORARY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN)) == 0) {
				if (_stricmp(find_data.cFileName, NET_SAVE_FILE_NAME) != 0) {
					if (Read_File(&entry, &find_data) == true && !entry.Old) {
						files_found = true;
						break;
					}
				}
			}

			if (!FindNextFile(hFind, &find_data)) {
				break;
			}
		}
	}

	FindClose(hFind);

	return(files_found);
}

// FUNCTION: GAMEMD 0x00559D30
int __cdecl LoadOptionsClass::Compare(const void * p1, const void * p2)
{
	FileEntryClass * fe1, * fe2;

	fe1 = *((FileEntryClass **)p1);
	fe2 = *((FileEntryClass **)p2);

	int res = CompareFileTime(&fe1->DateTime, &fe2->DateTime);
	return(-res);
}

// FUNCTION: GAMEMD 0x00559D60
bool LoadOptionsClass::Load_File(const char * file_name)
{
	int hidden = Get_Mouse_State();
	HWND dialog = OwnerDraw::Custom_Message_Box(StringTable::LoadString("TXT_LOADING", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x41C), NULL, NULL);
	Hide_Mouse();
	if (dialog != 0) {
		OwnerDraw::Display_Dialog(dialog);
	}
	ScenarioActive = false;
	TacticalActive = false;
	Session.FUN_0069BB40();
	bool loaded = Load_Game(file_name);
	while (Get_Mouse_State() < hidden) {
		Show_Mouse();
	}
	while (Get_Mouse_State() > hidden) {
		Hide_Mouse();
	}
	if (dialog != 0) {
		OwnerDraw::End_Dialog(dialog);
	}
	return(loaded);
}

// FUNCTION: GAMEMD 0x00559E40
bool LoadOptionsClass::Save_File(const char * file_name, const wchar_t * descr)
{
	HWND dialog = OwnerDraw::Custom_Message_Box(StringTable::LoadString("TXT_SAVING_GAME", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x453), NULL, NULL);
	Hide_Mouse();
	if (dialog != 0) {
		OwnerDraw::Display_Dialog(dialog);
	}
	bool saved = Save_Game(file_name, descr);
	Show_Mouse();
	if (dialog != 0) {
		OwnerDraw::End_Dialog(dialog);
	}
	return(saved);
}

// FUNCTION: GAMEMD 0x00559EB0
bool LoadOptionsClass::Delete_File(const char * file_name)
{
	if (DeleteFile(file_name) == TRUE) {
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00559ED0
bool LoadOptionsClass::Read_File(FileEntryClass * fdata, WIN32_FIND_DATAA * ff)
{
	if (fdata == NULL && ff == NULL) {
		return(false);
	}

	if (stricmp(ff->cFileName, NET_SAVE_FILE_NAME) != 0) {

		SaveVersionInfo savever;

		bool ok = Get_Savefile_Info(ff->cFileName, &savever);
		if (!ok) {
			return(false);
		}

		if (savever.Get_Internal_Version() != ExpectedGameVersion) {
			wcscpy(fdata->Descr, savever.Get_Scenario_Description());
			fdata->Old = true;
		} else {
			wcscpy(fdata->Descr, savever.Get_Scenario_Description());
			fdata->Old = false;
		}

		fdata->Valid = ok;
		fdata->Scenario = savever.Get_Scenario_Number();
		fdata->Num = savever.Get_Campaign_Number();
		fdata->Type = (GameType)savever.Get_Game_Type();
		strcpy(fdata->Filename, ff->cFileName);
		wcscpy(fdata->PlayerName, savever.Get_Player_House());
		if (strlen(fdata->Filename) == 0) {
			strcpy(fdata->Filename, ff->cAlternateFileName);
		}
		fdata->DateTime.dwHighDateTime = ff->ftLastWriteTime.dwHighDateTime;
		fdata->DateTime.dwLowDateTime = ff->ftLastWriteTime.dwLowDateTime;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0055A050
wchar_t const * LoadOptionsClass::GetUIString_Load(void) const
{
	return(StringTable::LoadString("GUI:LoadMissionMenu", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x4C0));
}

// FUNCTION: GAMEMD 0x0055A070
wchar_t const * LoadOptionsClass::GetUIString_Save(void) const
{
	return(StringTable::LoadString("GUI:SaveMissionMenu", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x4C6));
}

// FUNCTION: GAMEMD 0x0055A090
wchar_t const * LoadOptionsClass::GetUIString_Delete(void) const
{
	return(StringTable::LoadString("GUI:DeleteMissionMenu", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x4CC));
}

// FUNCTION: GAMEMD 0x0055A0B0
wchar_t const * LoadOptionsClass::GetUIString_GameSaved(void) const
{
	return(StringTable::LoadString("TXT_GAME_WAS_SAVED", NULL, "D:\\ra2mdpost\\LoadDlg.CPP", 0x4D2));
}

// SYNTHETIC: GAMEMD 0x0055A0D0
// ??_GLoadOptionsClass@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0055A150
// ?Clear@?$DynamicVectorClass@PAVFileEntryClass@@@@UAEXXZ
template class DynamicVectorClass<FileEntryClass *>;

// TEMPLATE: GAMEMD 0x0055A180
// ?ID@?$DynamicVectorClass@PAVFileEntryClass@@@@UAEHPBQAVFileEntryClass@@@Z
// TEMPLATE: GAMEMD 0x0055A1A0
// ?GetItem@?$VectorClass@PAVFileEntryClass@@@@UBEPAVFileEntryClass@@H@Z
// TEMPLATE: GAMEMD 0x0055A1B0
// ??8?$VectorClass@PAVFileEntryClass@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x0055A1F0
// ?Resize@?$VectorClass@PAVFileEntryClass@@@@UAE_NHPBQAVFileEntryClass@@@Z
// TEMPLATE: GAMEMD 0x0055A2A0
// ?Clear@?$VectorClass@PAVFileEntryClass@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x0055A2D0
// ?ID@?$VectorClass@PAVFileEntryClass@@@@UAEHABQAVFileEntryClass@@@Z
// TEMPLATE: GAMEMD 0x0055A310
// ?ID@?$VectorClass@PAVFileEntryClass@@@@UAEHPBQAVFileEntryClass@@@Z
// TEMPLATE: GAMEMD 0x0055A330
// ?Resize@?$DynamicVectorClass@PAVFileEntryClass@@@@UAE_NHPBQAVFileEntryClass@@@Z
// TEMPLATE: GAMEMD 0x0055A3E0
// ?ID@?$DynamicVectorClass@PAVFileEntryClass@@@@UAEHABQAVFileEntryClass@@@Z
// SYNTHETIC: GAMEMD 0x0055A410
// ??_G?$VectorClass@PAVFileEntryClass@@@@UAEPAXI@Z
template class VectorClass<FileEntryClass *>;

// SYNTHETIC: GAMEMD 0x0055A460
// ??_G?$DynamicVectorClass@PAVFileEntryClass@@@@UAEPAXI@Z
