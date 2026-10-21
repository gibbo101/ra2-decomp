// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "phoneed.h"

#include "data.h"
#include "dbgprint.h"
#include "init.h"
#include "language\language.h"
#include "ownrdraw.h"
#include "serialed.h"
#include "trim.h"
#include "winfix.h"
#include "stringtable.h"

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
extern const double DAT_007EFD90;
extern const double DAT_007EFD98;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x006309A0 SYMBOL
// _$E30
double DAT_00AC4AD0 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x006309D0 SYMBOL
// _$E33
double DAT_00AC4B20 = DAT_00AC4AD0 / DAT_007EFD90;

// SYNTHETIC: GAMEMD 0x006309F0 SYMBOL
// _$E36
double DAT_00AC4B08 = DAT_007E1718 / DAT_00AC4B20;

// SYNTHETIC: GAMEMD 0x00630A10 SYMBOL
// _$E39
double DAT_00AC4B00 = DAT_007E1720 * DAT_007EFD98;

// SYNTHETIC: GAMEMD 0x00630A30 SYMBOL
// _$E42
double DAT_00AC4AD8 = DAT_007E1728 * DAT_007EFD98;

// SYNTHETIC: GAMEMD 0x00630A50 SYMBOL
// _$E45
double DAT_00AC4AB0 = DAT_007E1730 * DAT_007EFD98;

// SYNTHETIC: GAMEMD 0x00630A70 SYMBOL
// _$E48
int DAT_00AC4B28 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00AC4AB0 - DAT_00AC4AD8) * DAT_00AC4AD0));

// SYNTHETIC: GAMEMD 0x00630AB0 SYMBOL
// _$E51
double DAT_00AC4AE0 = FUN_004CADE0(DAT_00AC4B28 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x00630AE0 SYMBOL
// _$E54
double DAT_00AC4AF8 = FUN_004CADE0((DAT_00AC4B28 * 2) / DAT_00AC4AD0);

// SYNTHETIC: GAMEMD 0x00630B10 SYMBOL
// _$E57
int DAT_00AC4B1C = (int)(DAT_00AC4B28 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x00630B40 SYMBOL
// _$E60
ShortPair DAT_00AC4AC8;

// SYNTHETIC: GAMEMD 0x00630B50 SYMBOL
// _$E63
IntTriple DAT_00AC4AE8;

struct IntTripleB {
	int A, B, C;
	IntTripleB() { C = 0; A = 0x80; B = 0x80; }
};

// SYNTHETIC: GAMEMD 0x00630B70 SYMBOL
// _$E66
IntTripleB DAT_00AC4B10;

#undef Edit_SetText
#undef Edit_GetText
#undef ListBox_AddString
#define Edit_SetText(h, s) ((int)SendMessage((h), 0x4b4, 0, (LPARAM)(s)))
#define Edit_GetText(h, s, n) ((int)SendMessage((h), 0x4b5, (n), (LPARAM)(s)))
#define Edit_SetTextW(h, s) ((int)SendMessage((h), 0x4b2, 0, (LPARAM)(s)))
#define Edit_GetTextW(h, s, n) ((int)SendMessage((h), 0x4b3, (n), (LPARAM)(s)))
#define ListBox_AddStringW(h, s) ((int)SendMessage((h), 0x4cd, 0, (LPARAM)(s)))

#define LS(name, line) StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\PhoneEd.cpp", line)

extern char DAT_00889F64;
wchar_t * __fastcall wcstrim(wchar_t * buffer);

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}

extern int __cdecl Phone_Compare(const void *p1, const void *p2);

class PhoneEntryEdit
{
		friend BOOL CALLBACK Edit_Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		PhoneEntryEdit(void) {};
		bool Dialog(PhoneEntryClass *phone);

	private:
		void Save(void);
		void Init(void);

	private:
		HWND _Dialog;
		int RC;
		PhoneEntryClass * Phone;
		SerialSettingsType TempSettings;
};

BOOL CALLBACK Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK Edit_Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00630B90
bool PhoneEdit::Dialog(void)
{
	_Dialog = OwnerDraw::Begin_Dialog(IDD_SERIAL_PHONE_LIST, (DLGPROC)Phone_Dialog_Proc, 0);
	if (_Dialog) {

		Changed = false;
		RC = -1;
		SetWindowLong(_Dialog, DWL_USER, (LONG)this);

		Build_Phone_Listbox();

		OwnerDraw::Display_Dialog(_Dialog);

		while (RC < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(_Dialog);

		if (Changed == true) {
			Session.Write_MultiPlayer_Settings();
		}
	}
	if (RC != 1) {
		return(false);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00630C10
BOOL CALLBACK Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND handle;

	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc == 0) {
		PhoneEdit * _this = (PhoneEdit *)GetWindowLong(window, DWL_USER);

		switch (message) {
		case OD_SUBCLASSED:
			SendDlgItemMessage(window, IDC_PHONE_NAME, EM_SETLIMITTEXT, 20, 0);
			SendDlgItemMessage(window, IDC_PHONE_LIST, OD_ADDCOLUMN, 144, 2);
			SendDlgItemMessage(window, IDC_PHONE_LIST, OD_ADDCOLUMN, 121, 150);
			SendDlgItemMessage(window, IDC_PHONE_LIST, OD_ADDCOLUMN, 0, 275);
			break;

		case WM_COMMAND: {
			switch (LOWORD(wparam)) {
			case IDC_PHONE_EDIT:
				if (HIWORD(wparam) == 0) {
					_this->Edit();
				}
				break;

			case IDC_SERIAL_ADD:
				if (HIWORD(wparam) == 0) {
					_this->Add();
				}
				break;

			case IDC_PHONE_LIST:
				if (HIWORD(wparam) == LBN_SELCHANGE) {
					handle = GetDlgItem(_this->_Dialog, IDC_PHONE_LIST);
					if (handle != 0) {
						if (ListBox_GetCount(handle) > 0) {
							Session.CurPhoneIdx = ListBox_GetCurSel(handle);
							if (Session.CurPhoneIdx > -1) {
								handle = GetDlgItem(_this->_Dialog, IDC_PHONE_NAME);
								Edit_SetText(handle, Session.PhoneBook[Session.CurPhoneIdx]->Number);
								SetFocus(handle);
							}
						}
					}
				}
				break;

			case IDOK:
				if (HIWORD(wparam) == 0) {
					if (_this->Dial() == true) {
						_this->RC = IDOK;
					}
				}
				break;

			case IDCANCEL:
				if (HIWORD(wparam) == 0) {
					_this->RC = IDCANCEL;
				}
				break;

			case IDC_PHONE_DELETE:
				if (HIWORD(wparam) == 0) {
					handle = GetDlgItem(_this->_Dialog, IDC_PHONE_LIST);
					if (handle != 0) {
						Session.CurPhoneIdx = ListBox_GetCurSel(handle);
						if (Session.CurPhoneIdx > -1) {
							Session.PhoneBook.Delete_Index(Session.CurPhoneIdx);
							_this->Changed = true;
							_this->Build_Phone_Listbox();
							if (Session.CurPhoneIdx == -1) {
								handle = GetDlgItem(_this->_Dialog, IDC_PHONE_NAME);
								if (handle != 0) {
									Edit_SetText(handle, &DAT_00889F64);
								}
							}
						}
					}
				}
				break;
			}
		} break;
		}
		rc = 0;
	}

	return(rc);
}

// FUNCTION: GAMEMD 0x00630EA0
bool PhoneEdit::Dial(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
	Edit_GetText(handle, Number, sizeof(Number));
	HWND handle2 = GetDlgItem(_Dialog, IDC_PHONE_LIST);
	Session.CurPhoneIdx = ListBox_GetCurSel(handle2);

	if (Session.CurPhoneIdx == -1 ||
		strcmp( Session.PhoneBook[Session.CurPhoneIdx]->Number, Number) ) {

		if ( strlen(Number) == 0) {
			return(false);
		}

		PhoneEntryClass *p_entry = new PhoneEntryClass();
		if (p_entry == NULL) {
			return(false);
		}

		wcscpy( p_entry->Name, LS("TXT_NONAME", 231) );
		strcpy( p_entry->Number, Number);
		p_entry->Settings.Port = 0;
		p_entry->Settings.IRQ = -1;
		p_entry->Settings.Baud = -1;
		p_entry->Settings.DialMethod = DIAL_TOUCH_TONE;
		p_entry->Settings.InitStringIndex = -1;
		p_entry->Settings.CallWaitStringIndex = CALL_WAIT_CUSTOM;
		p_entry->Settings.CallWaitString[0] = 0;

		Session.PhoneBook.Add (p_entry);
		Build_Phone_Listbox();
		Changed = true;

		Session.CurPhoneIdx = Session.PhoneBook.ID(p_entry);
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00631060
void PhoneEdit::Build_Phone_Listbox(void)
{
	HWND handle;
	int i;
	wchar_t item[80];
	char nitem[80];

	OwnerDraw::CellData thecell;

	handle = GetDlgItem(_Dialog, IDC_PHONE_LIST);
	if (handle) {

		ListBox_ResetContent(handle);

		if (Session.PhoneBook.Count() > 1) {
			qsort ((void *)(&Session.PhoneBook[0]), Session.PhoneBook.Count(), sizeof(class PhoneEntryClass *), Phone_Compare);
		}

		for (i = 0; i < Session.PhoneBook.Count(); i++) {
			if ( !(wcslen( Session.PhoneBook[i]->Name )) ) {
				wcscpy( item, L" " );
			} else {
				wcsncpy( item, Session.PhoneBook[i]->Name, PhoneEntryClass::PHONE_MAX_NAME-1 );
				item[PhoneEntryClass::PHONE_MAX_NAME-1] = '\0';
			}

			int row = ListBox_AddStringW(handle, item);

			if ( !strlen( Session.PhoneBook[i]->Number ) ) {
				wcscpy( item, L" " );
			} else {
				strncpy( nitem, Session.PhoneBook[i]->Number, PhoneEntryClass::PHONE_MAX_NAME-1 );
				nitem[PhoneEntryClass::PHONE_MAX_NAME-1] = '\0';
				swprintf( item, L"%hs", nitem );
			}

			thecell.type = OwnerDraw::CellData::TEXT;
			thecell.string.set(item);

			SendMessage(handle, OD_SETCELL, MAKEWPARAM(150, row), (LPARAM)&thecell);

			if (Session.PhoneBook[i]->Settings.Baud != -1) {
				swprintf(item, L"%d", Session.PhoneBook[i]->Settings.Baud);
			} else {
				swprintf(item, L"[%s]", LS("TXT_DEFAULT", 321));
			}

			thecell.type = OwnerDraw::CellData::TEXT;
			thecell.string.set(item);

			SendMessage(handle, OD_SETCELL, MAKEWPARAM(275, row), (LPARAM)&thecell);
		}

		if (ListBox_GetCount(handle) == 0 || Session.CurPhoneIdx < -1) {
			Session.CurPhoneIdx = -1;
		} else {
			if (Session.CurPhoneIdx >= ListBox_GetCount(handle) ) {
				Session.CurPhoneIdx = 0;
			}
		}

		if (Session.CurPhoneIdx > -1) {
			ListBox_SetCurSel(handle, Session.CurPhoneIdx);
			handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
			if (handle != 0) {
				Edit_SetText(handle, Session.PhoneBook[Session.CurPhoneIdx]->Number);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00631300
void PhoneEdit::Select(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_PHONE_LIST);

	if (handle != 0) {
		if (ListBox_GetCount(handle) > 0) {
			Session.CurPhoneIdx = ListBox_GetCurSel(handle);
			if (Session.CurPhoneIdx > -1) {
				handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
				Edit_SetText(handle, Session.PhoneBook[Session.CurPhoneIdx]->Number);
				SetFocus(handle);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00631380
void PhoneEdit::Add(void)
{
	PhoneEntryClass *p_entry = new PhoneEntryClass();
	if (p_entry == NULL) {
		return;
	}

	p_entry->Name[0] = '\0';
	p_entry->Number[0] = '\0';
	p_entry->Settings.Port = 0;
	p_entry->Settings.IRQ = -1;
	p_entry->Settings.Baud = -1;
	p_entry->Settings.DialMethod = DIAL_TOUCH_TONE;
	p_entry->Settings.InitStringIndex = -1;
	p_entry->Settings.CallWaitStringIndex = CALL_WAIT_CUSTOM;
	p_entry->Settings.CallWaitString[0] = '\0';

	ShowWindow(_Dialog, SW_HIDE);
	UpdateWindow(MainWindow);

	if ( PhoneEntryEdit().Dialog( p_entry ) == true ) {
		Session.PhoneBook.Add (p_entry);
		Changed = true;
		Build_Phone_Listbox();
		Session.CurPhoneIdx = Session.PhoneBook.ID(p_entry);

		if (Session.CurPhoneIdx > -1) {
			HWND handle = GetDlgItem(_Dialog, IDC_PHONE_LIST);
			if (handle != 0) {
				ListBox_SetCurSel(handle, Session.CurPhoneIdx);
			}
			handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
			if (handle != 0) {
				Edit_SetText(handle, Session.PhoneBook[Session.CurPhoneIdx]->Number);
			}
		}

	} else {
		delete p_entry;
	}
	ShowWindow(_Dialog, SW_SHOW);
}

// FUNCTION: GAMEMD 0x00631500
void PhoneEdit::Edit(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_PHONE_LIST);
	if (handle != 0) {
		Session.CurPhoneIdx = ListBox_GetCurSel(handle);
	}

	if (Session.CurPhoneIdx <= -1) {
		return;
	}

	PhoneEntryClass *p_entry = new PhoneEntryClass();
	if (p_entry == NULL) {
		return;
	}

	PhoneEntryClass *p_oldentry = Session.PhoneBook[Session.CurPhoneIdx];
	(*p_entry) = (*p_oldentry);

	ShowWindow(_Dialog, SW_HIDE);
	UpdateWindow(MainWindow);

	if ( PhoneEntryEdit().Dialog( p_entry ) == true ) {
		Changed = true;
		(*p_oldentry) = (*p_entry);
		Build_Phone_Listbox();
		Session.CurPhoneIdx = Session.PhoneBook.ID(p_oldentry);
		if (Session.CurPhoneIdx > -1) {
			ListBox_SetCurSel(handle, Session.CurPhoneIdx);
			handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
			if (handle != 0) {
				Edit_SetText(handle, p_oldentry->Number);
			}
		}
	}

	ShowWindow(_Dialog, SW_SHOW);
	delete p_entry;
}

// FUNCTION: GAMEMD 0x00631640
void PhoneEdit::Delete(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_PHONE_LIST);
	if (handle == 0) {
		return;
	}

	Session.CurPhoneIdx = ListBox_GetCurSel(handle);

	if (Session.CurPhoneIdx <= -1) {
		return;
	}

	Session.PhoneBook.Delete_Index(Session.CurPhoneIdx);
	Changed = true;
	Build_Phone_Listbox();

	if (Session.CurPhoneIdx == -1) {
		handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
		if (handle != 0) {
			Edit_SetText(handle, &DAT_00889F64);
		}
	}
}

// FUNCTION: GAMEMD 0x006316E0
bool PhoneEntryEdit::Dialog(PhoneEntryClass *phone)
{
	int unused = RC + 1;
	if (phone == NULL) {
		return(false);
	}
	_Dialog = OwnerDraw::Begin_Dialog(IDD_SERIAL_PHONE_ENTRY, (DLGPROC)Edit_Phone_Dialog_Proc, 0);
	if (_Dialog) {

		Phone = phone;
		RC = -1;
		SetWindowLong(_Dialog, DWL_USER, (LONG)this);

		Init();

		OwnerDraw::Display_Dialog(_Dialog);

		while (RC < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(_Dialog);
	}

	return(RC == 1 ? true : false);
}

// FUNCTION: GAMEMD 0x00631760
BOOL CALLBACK Edit_Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	wchar_t buffer[64];

	int rc;

	rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc == 0) {
		PhoneEntryEdit * _this = (PhoneEntryEdit *)GetWindowLong(window, DWL_USER);

		switch (message) {
		case OD_SUBCLASSED:
			SendDlgItemMessage(window, IDC_PHONE_NAME, EM_SETLIMITTEXT, 20, 0);
			SendDlgItemMessage(window, IDC_PHONE_NUMBER, EM_SETLIMITTEXT, 20, 0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDC_PHONE_DEFAULT:
				if (HIWORD(wparam) != 0) {
					Button_SetCheck((HWND)lparam, TRUE);
					SendDlgItemMessage(window, IDC_PHONE_CUSTOM, BM_SETCHECK, 0, 0);
				}
				break;

			case IDC_PHONE_CUSTOM:
				if (HIWORD(wparam) != 0) {
					Button_SetCheck((HWND)lparam, TRUE);
					SendDlgItemMessage(window, IDC_PHONE_DEFAULT, BM_SETCHECK, 0, 0);
					UpdateWindow(window);
					ShowWindow(window, SW_HIDE);
					UpdateWindow(MainWindow);
					SerialEdit().Dialog(&_this->TempSettings);
					ShowWindow(window, SW_SHOW);
				}
				break;

			case IDC_PHONE_NAME:
				if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_KILLFOCUS) {
					Edit_GetTextW((HWND)lparam, buffer, 64);
					wcstrim(buffer);
					Edit_SetTextW((HWND)lparam, buffer);
				}
				break;

			case IDOK:
				if (HIWORD(wparam) == 0) {
					_this->Save();
					_this->RC = IDOK;
				}
				break;

			case IDCANCEL:
				if (HIWORD(wparam) == 0) {
					_this->RC = IDCANCEL;
				}
				break;

			case IDC_PHONE_NUMBER:
				if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_KILLFOCUS) {
					Edit_GetText((HWND)lparam, buffer, 64);
					strtrim((char *)buffer);
					Edit_SetText((HWND)lparam, buffer);
				}
				break;
			}
			break;
		}
		rc = 0;
	}

	return(rc);
}

// FUNCTION: GAMEMD 0x006319C0
void PhoneEntryEdit::Save(void)
{
	bool custom;

	HWND handle = GetDlgItem(_Dialog, IDC_PHONE_NAME);
	if (handle != 0) {
		Edit_GetTextW(handle, Phone->Name, PhoneEntryClass::PHONE_MAX_NAME);
		if (wcslen( Phone->Name ) > 0) {
			wcsupr( Phone->Name );
		} else {
			wcscpy( Phone->Name, LS("TXT_NONAME", 757) );
		}
	}
	DebugString("Name: %ls\n", Phone->Name);

	handle = GetDlgItem(_Dialog, IDC_PHONE_NUMBER);
	if (handle != 0) {
		Edit_GetText(handle, Phone->Number, sizeof(Phone->Number));
	}
	DebugString("Number: %s\n", Phone->Number);

	handle = GetDlgItem(_Dialog, IDC_PHONE_CUSTOM);
	if (handle) {
		custom = Button_GetCheck(handle) == 1;
	} else {
		custom = false;
	}

	if (custom == true) {
		Phone->Settings = TempSettings;
	} else {
		Phone->Settings.Port = 0;
		Phone->Settings.IRQ = -1;
		Phone->Settings.Baud = -1;
		Phone->Settings.DialMethod = DIAL_TOUCH_TONE;
		Phone->Settings.InitStringIndex = -1;
		Phone->Settings.CallWaitStringIndex = CALL_WAIT_CUSTOM;
		Phone->Settings.CallWaitString[0] = 0;
	}

	DebugString("Settings: Port %d, Baud %d\n", Phone->Settings.Port, Phone->Settings.Baud);
}
