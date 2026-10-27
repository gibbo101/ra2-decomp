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
#include "geometry.h"
#include "ownrdraw.h"
#include "trim.h"
#include "winfix.h"
#include "stringtable.h"

#include <math.h>


#undef Edit_SetText
#undef Edit_GetText
#undef ListBox_GetText
#undef ListBox_AddString
#undef ComboBox_GetLBText
#undef ComboBox_AddString
#undef ComboBox_FindString
#define Edit_SetText(h, s) ((int)SendMessage((h), 0x4b4, 0, (LPARAM)(s)))
#define Edit_GetText(h, s, n) ((int)SendMessage((h), 0x4b5, (n), (LPARAM)(s)))
#define ListBox_GetText(h, i, s) ((int)SendMessage((h), 0x4b7, (i), (LPARAM)(s)))
#define ListBox_AddString(h, s) ((int)SendMessage((h), 0x4c8, 0, (LPARAM)(s)))
#define ComboBox_GetLBText(h, i, s) ((int)SendMessage((h), 0x4bd, (i), (LPARAM)(s)))
static inline int ComboBox_AddString(HWND h, const void *s) { return (int)SendMessage(h, 0x4c2, 0, (LPARAM)s); }
#define ComboBox_FindString(h, i, s) ((int)SendMessage((h), 0x4b8, (i), (LPARAM)(s)))
static inline int ComboBox_AddStringA(HWND h, const void *s) { return (int)SendMessage(h, 0x4bc, 0, (LPARAM)s); }
#define LS(name, line) StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\SerialEd.cpp", line)
static wchar_t const *baudname[] = {
	L"14400",
	L"19200",
	L"28800",
	L"38400",
	L"57600",
	L"115200",
};
BOOL CALLBACK Com_Settings_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
// SYNTHETIC: GAMEMD 0x006954E0 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x00695510 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x00695530 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x00695550 SYMBOL
// _$E42
// SYNTHETIC: GAMEMD 0x00695570 SYMBOL
// _$E45
// SYNTHETIC: GAMEMD 0x00695590 SYMBOL
// _$E48
// SYNTHETIC: GAMEMD 0x006955B0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x006955F0 SYMBOL
// _$E54
// SYNTHETIC: GAMEMD 0x00695620 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x00695650 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x00695680 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x00695690 SYMBOL
// _$E66
// SYNTHETIC: GAMEMD 0x006956B0 SYMBOL
// _$E69

// FUNCTION: GAMEMD 0x006956D0
bool SerialEdit::Dialog(SerialSettingsType *settings)
{
	RC = -1;
	_Dialog = OwnerDraw::Begin_Dialog(IDD_SERIAL_SETTINGS, (DLGPROC)Com_Settings_Dialog_Proc, 0);
	if (_Dialog) {

		SetWindowLong(_Dialog, DWL_USER, (LONG)this);

		Init(settings);

		OwnerDraw::Display_Dialog(_Dialog);

		while (RC < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(_Dialog);

		if (RC == 1) {
			memcpy(settings, &TempSettings, sizeof(SerialSettingsType));
		}
	}
	if (RC != 1) {
		return(false);
	}
	return(true);
}

BOOL SerialEdit::Handle_Message(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND handle;
	int i;
	SerialEdit * _this = (SerialEdit *)GetWindowLong(window, DWL_USER);

	switch (message) {
		case OD_SUBCLASSED:
			handle = GetDlgItem(window, IDC_SERIAL_INITSTRING);
			if (handle != 0) {
				Edit_LimitText(handle, INITSTRBUF_MAX-1);
			}
			handle = GetDlgItem(window, IDC_SERIAL_PORT_EDIT);
			if (handle != 0) {
				Edit_LimitText(handle, PORTBUF_MAX-1-1);
			}
			handle = GetDlgItem(window, IDC_SERIAL_CALLWAITING_EDIT);
			if (handle != 0) {
				Edit_LimitText(handle, CWAITSTRBUF_MAX-1);
			}
			return(FALSE);

		case WM_COMMAND:
			{
				switch (LOWORD(wparam)) {
					case IDC_SERIAL_PULSE:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
							SendDlgItemMessage(window, IDC_SERIAL_TONE, BM_SETCHECK, 0, 0);
						}
						break;

					case IDC_SERIAL_DATA_COMPRESSION:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
						}
						break;

					case IDOK:
						if (HIWORD(wparam) == 0 && _this->Save() == true) {
							_this->RC = IDOK;
						}
						break;

					case IDCANCEL:
						if (HIWORD(wparam) == 0) {
							_this->RC = IDCANCEL;
						}
						break;

					case IDC_SERIAL_ERROR_CORRECTION:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
						}
						break;

					case IDC_SERIAL_PORT:
						if (HIWORD(wparam) == 1) {
							i = ComboBox_GetCurSel((HWND)lparam);
							if (ComboBox_GetItemData((HWND)lparam, i) == 4) {
								ComboBox_GetLBText((HWND)lparam, i, _this->TempBuf);
								_this->Set_String(IDC_SERIAL_PORT_EDIT);
							} else {
								SetFocus(0);
							}
						}
						break;

					case IDC_SERIAL_CALLWAITING:
						if (HIWORD(wparam) == 1) {
							i = ComboBox_GetCurSel((HWND)lparam);
							if (ComboBox_GetItemData((HWND)lparam, i) == 3) {
								ComboBox_GetLBText((HWND)lparam, i, _this->TempBuf);
								_this->Set_String(IDC_SERIAL_CALLWAITING_EDIT);
							} else {
								SetFocus(0);
							}
						}
						break;

					case IDC_SERIAL_CALLWAITING_EDIT:
						if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_KILLFOCUS) {
							_this->Set_Init_String();
						}
						break;

					case IDC_SERIAL_PORT_EDIT:
						if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_KILLFOCUS) {
							_this->Set_Port();
						}
						break;

					case IDC_SERIAL_INITLIST:
						if (HIWORD(wparam) == LBN_DBLCLK) {
							if (ListBox_GetCount((HWND)lparam) > 0) {
								i = ListBox_GetCurSel((HWND)lparam);
								if (i != LB_ERR) {
									ListBox_GetText((HWND)lparam, i, _this->TempBuf);
									handle = GetDlgItem(window, IDC_SERIAL_INITSTRING);
									if (handle != 0) {
										Edit_SetText(handle, _this->TempBuf);
										SetFocus(handle);
									}
								}
							}
						}
						break;

					case IDC_SERIAL_INITSTRING:
						if (HIWORD(wparam) == EN_MAXTEXT) {
							_this->Add_Init_String();
						}
						break;

					case IDC_SERIAL_ADD:
						if (HIWORD(wparam) == 0) {
							_this->Add_Init_String();
						}
						break;

					case IDC_SERIAL_INIT_DELETE:
						if (HIWORD(wparam) == 0) {
							_this->Delete_Init_String();
						}
						break;

					case IDC_SERIAL_TONE:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
							SendDlgItemMessage(window, IDC_SERIAL_PULSE, BM_SETCHECK, 0, 0);
						}
						break;
				}
			}
			break;
	}

	return(0);
}

// FUNCTION: GAMEMD 0x00695760
BOOL CALLBACK Com_Settings_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc;

	rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc != 0) {
		return(rc);
	}

	return(SerialEdit::Handle_Message(window, message, wparam, lparam));
}

// FUNCTION: GAMEMD 0x00695BC0
bool SerialEdit::Save(void)
{
	char * temp;
	int index;
	int initstr_index;
	int cwaitstr_index;
	int port_index;
	DetectPortType dpstatus;

	DebugString("Reading serial settings dialog\n");
	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_PORT);
	if (handle) {
		index = ComboBox_GetCurSel(handle);
		port_index = ComboBox_GetItemData(handle, index);

		TempSettings.Port = 1;
		TempSettings.ModemName[0] = '\0';
		switch (port_index) {
			case ( 0 ):
				TempSettings.Port = 0x3f8;
				break;

			case ( 1 ):
				TempSettings.Port = 0x2f8;
				break;

			case ( 2 ):
				TempSettings.Port = 0x3e8;
				break;

			case ( 3 ):
				TempSettings.Port = 0x2e8;
				break;

			case ( 4 ):
				ComboBox_GetLBText(handle, index, TempBuf);
				TempBuf[MODEM_NAME_MAX-1] = '\0';
				temp = strchr(TempBuf, '-');
				if (temp) {
					strcpy(TempSettings.ModemName, temp + 1);
				}
				break;

			default:
				ComboBox_GetLBText(handle, index, TempBuf);
				TempBuf[MODEM_NAME_MAX-1] = '\0';
				strcpy(TempSettings.ModemName, TempBuf);
				break;
		}
	}

	DebugString("Port: %lx\n", TempSettings.Port);
	DebugString("Modem: %s\n", TempSettings.ModemName);

	handle = GetDlgItem(_Dialog, IDC_SERIAL_BAUD);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		i = ComboBox_GetItemData(handle, i);
		swscanf(baudname[i], L"%d", &TempSettings.Baud);
	}

	DebugString("Baud: %ld\n", TempSettings.Baud);

	handle = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);
	if (handle) {
		initstr_index = ListBox_GetCurSel(handle);
		if (initstr_index != LB_ERR && Session.InitStrings.Count()) {
			TempSettings.InitStringIndex = initstr_index;
		} else {
			TempSettings.InitStringIndex = -1;
		}
	}

	handle = GetDlgItem(_Dialog, IDC_SERIAL_CALLWAITING);
	if (handle) {
		index = ComboBox_GetCurSel(handle);
		cwaitstr_index = ComboBox_GetItemData(handle, index);
		TempSettings.CallWaitStringIndex = cwaitstr_index;
		TempSettings.CallWaitString[0] = '\0';

		if (TempSettings.CallWaitStringIndex == CALL_WAIT_CUSTOM) {
			ComboBox_GetLBText(handle, index, TempBuf);
			TempBuf[CWAITSTRBUF_MAX-1] = '\0';
			temp = strchr(TempBuf, '-');
			if (temp) {
				strcpy(TempSettings.CallWaitString, temp + 1);
			}
		}
	}

	TempSettings.DialMethod = DIAL_TOUCH_TONE;
	handle = GetDlgItem(_Dialog, IDC_SERIAL_PULSE);
	if ( handle )
	{
		if (Button_GetCheck(handle) == TRUE) {
			TempSettings.DialMethod = DIAL_PULSE;
		}
	}
	DebugString("DialMethod: %s\n", TempSettings.DialMethod == DIAL_TOUCH_TONE ? "Tone" : "Pulse");

	handle = GetDlgItem(_Dialog, IDC_SERIAL_ERROR_CORRECTION);
	if (handle) {
		if (Button_GetCheck(handle) == 1) {
			TempSettings.ErrorCorrection = 1;
		} else {
			TempSettings.ErrorCorrection = 0;
		}
	}
	handle = GetDlgItem(_Dialog, IDC_SERIAL_DATA_COMPRESSION);
	if (handle) {
		if (Button_GetCheck(handle) == 1) {
			TempSettings.Compression = 1;
		} else {
			TempSettings.Compression = 0;
		}
	}

	dpstatus = NullModem.Detect_Port(&TempSettings);

	switch (dpstatus) {
		case PORT_VALID:
			return(true);

		case PORT_INVALID:
			WWMessageBox().Process(LS("TXT_UNABLE_TO_OPEN_PORT", 0x213), LS("TXT_OK", 0x213), NULL, NULL);
			break;

		case PORT_IRQ_INUSE:
			WWMessageBox().Process(LS("TXT_IRQ_ALREADY_IN_USE", 0x217), LS("TXT_OK", 0x217), NULL, NULL);
			break;
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00695FD0
void SerialEdit::Init(SerialSettingsType *settings)
{
	memcpy( &TempSettings, settings, sizeof(SerialSettingsType) );

	if (TempSettings.Port == 0) {
		TempSettings.Port = 0x2f8;
	}

	if (TempSettings.IRQ == -1) {
		TempSettings.IRQ = 3;
	}

	if (TempSettings.Baud == -1) {
		TempSettings.Baud = 57600;
	}

	Build_Port_Listbox();
	Build_Baud_Rate_Listbox();
	Build_Call_Wait_Listbox();

	Set_Dial_Method();

	if (!Session.InitStrings.Count()) {
		char * item = new char[8];
		strcpy(item, "ATZ");
		Session.InitStrings.Add(item);
		TempSettings.InitStringIndex = 0;
	}

	Build_Init_String_Listbox();

	HWND handle;

	handle = GetDlgItem(_Dialog, IDC_SERIAL_DATA_COMPRESSION);
	if (handle) {
		Button_SetCheck(handle, (settings->Compression != 0));
	}
	handle = GetDlgItem(_Dialog, IDC_SERIAL_ERROR_CORRECTION);
	if (handle) {
		Button_SetCheck(handle, (settings->ErrorCorrection != 0));
	}
}

// FUNCTION: GAMEMD 0x00696200
void SerialEdit::Build_Port_Listbox(void)
{
	static const char *portname[] = {
		"TXT_COM1",
		"TXT_COM2",
		"TXT_COM3",
		"TXT_COM4",
		"TXT_CUSTOM",
	};

	int i;
	int index;
	int port_index;
	wchar_t custom[128];
	wchar_t custom_port[128];

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_PORT);
	if (handle) {

		ComboBox_ResetContent(handle);
		for (i = 0; i < ARRAY_SIZE(portname)-1; i++) {
			index = ComboBox_AddString(handle, LS(portname[i], 0x287));
			ComboBox_SetItemData(handle, index, i);
		}

		ModemsFound = 0;
		for (i= 0; i < 10; i++) {
			ModemRegistryEntryClass *modemregistry = new ModemRegistryEntryClass (i);
			if (modemregistry) {
				if (modemregistry->Get_Modem_Name()) {
					strncpy(ModemNames[ModemsFound], modemregistry->Get_Modem_Name(), MODEM_NAME_MAX);
					index = ComboBox_AddStringA(handle, ModemNames[ModemsFound]);
					ComboBox_SetItemData(handle, index, ModemsFound + 5);
					ModemsFound++;
				}
				delete modemregistry;
			}
		}

		wcscpy(custom, LS("TXT_CUSTOM", 0x2a2));
		index = ComboBox_AddString(handle, custom);
		ComboBox_SetItemData(handle, index, 4);
		ComboBox_SetCurSel(handle, 0);

		if (strlen(TempSettings.ModemName)) {
			port_index = ComboBox_FindString(handle, -1, TempSettings.ModemName);
			if (port_index == CB_ERR) {
				port_index = ComboBox_Find_Item_Data(handle, -1, 4);
				if (port_index != CB_ERR) {
					swprintf(custom, L"%s %hs", LS("TXT_CUSTOM", 0x2b4), TempSettings.ModemName);
					ComboBox_DeleteString(handle, port_index);
					port_index = ComboBox_AddString(handle, custom);
					ComboBox_SetItemData(handle, port_index, 4);
				}
			}

			ComboBox_SetCurSel(handle, port_index);

		} else {
			switch (TempSettings.Port) {
				case ( 0x3f8 ):
					port_index = 0;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				case ( 0x2f8 ):
					port_index = 1;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				case ( 0x3e8 ):
					port_index = 2;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				case ( 0x2e8 ):
					port_index = 3;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				default:
					port_index = 4;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					if (port_index != CB_ERR) {
						swprintf(custom_port, L"%s %x", LS("TXT_CUSTOM", 0x2da), TempSettings.Port);
						ComboBox_DeleteString(handle, port_index);
						port_index = ComboBox_AddString(handle, custom_port);
						ComboBox_SetItemData(handle, port_index, 4);
					}
					break;
			}
			ComboBox_SetCurSel(handle, port_index);
		}

	}
}

// FUNCTION: GAMEMD 0x006964C0
void SerialEdit::Build_Baud_Rate_Listbox(void)
{
	int i;
	int index;
	int baud_index;

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_BAUD);
	if (handle) {

		ComboBox_ResetContent(handle);
		for (i = 0; i < ARRAY_SIZE(baudname); i++) {
			index = ComboBox_AddString(handle, baudname[i]);
			ComboBox_SetItemData(handle, index, i);
		}

		switch (TempSettings.Baud) {
			case 14400:
				baud_index = 0;
				break;

			case 19200:
				baud_index = 1;
				break;

			case 28800:
				baud_index = 2;
				break;

			default:
			case 38400:
				baud_index = 3;
				break;

			case 57600:
				baud_index = 4;
				break;

			case 115200:
				baud_index = 5;
				break;
		}

		index = ComboBox_Find_Item_Data(handle, -1, baud_index);
		ComboBox_SetCurSel(handle, index);
	}
}

// FUNCTION: GAMEMD 0x00696590
void SerialEdit::Build_Call_Wait_Listbox(void)
{
	int index;

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_CALLWAITING);

	if (handle) {
		ComboBox_ResetContent(handle);
		for (int i = 0; i < CALL_WAIT_STRINGS_NUM; i++) {
			index = ComboBox_AddString(handle, Session.CallWaitStrings[i]);
			ComboBox_SetItemData(handle, index, i);
		}
		index = ComboBox_Find_Item_Data(handle, -1, TempSettings.CallWaitStringIndex);
		ComboBox_SetCurSel(handle, index);
	}
}

// FUNCTION: GAMEMD 0x00696610
void SerialEdit::Build_Init_String_Listbox(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);
	if (handle) {
		ListBox_ResetContent(handle);
		for (int i = 0; i < Session.InitStrings.Count(); i++) {
			ListBox_AddString(handle, Session.InitStrings[i]);
		}
		if (ListBox_GetCount(handle) && TempSettings.InitStringIndex >= -1) {
			if (TempSettings.InitStringIndex >= ListBox_GetCount(handle)) {
				TempSettings.InitStringIndex = 0;
			}
		} else {
			TempSettings.InitStringIndex = -1;
		}
		if (TempSettings.InitStringIndex != -1) {
			ListBox_SetCurSel(handle, TempSettings.InitStringIndex);
		}
	}
}

// FUNCTION: GAMEMD 0x006966D0
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

// FUNCTION: GAMEMD 0x00696740
void SerialEdit::Set_String(int id)
{
	HWND handle = GetDlgItem(_Dialog, id);
	if (handle) {
		ShowWindow(handle, SW_SHOW);
		Edit_Enable(handle, TRUE);
		char * temp = strchr(TempBuf, '-');
		if (temp) {
			temp = temp + 1;
			strtrim(temp);
		} else {
			temp = (char *)"";
		}
		Edit_SetText(handle, temp);
		SetFocus(handle);
	}
}

// FUNCTION: GAMEMD 0x006967B0
void SerialEdit::Set_Port(void)
{
	int index;
	int port_index = -1;

	HWND handle1 = GetDlgItem(_Dialog, IDC_SERIAL_PORT);
	HWND handle2 = GetDlgItem(_Dialog, IDC_SERIAL_PORT_EDIT);
	if (handle1 && handle2) {
		ShowWindow(handle2, SW_HIDE);
		Edit_Enable(handle2, FALSE);
		Edit_GetText(handle2, TempBuf, PORTBUF_MAX);
		TempBuf[PORTBUF_MAX-1] = '\0';
		strtrim(TempBuf);
		strupr(TempBuf);

		if ( stricmp(TempBuf, "3F8") == 0 ) {
			port_index = 0;

		} else if ( stricmp(TempBuf, "2F8") == 0 ) {
			port_index = 1;

		} else if ( stricmp(TempBuf, "3E8") == 0 ) {
			port_index = 2;

		} else if ( stricmp(TempBuf, "2E8") == 0 ) {
			port_index = 3;

		} else if ( strncmp(TempBuf, "COM", 3) == 0 ) {

			switch ( (TempBuf[3]) ) {
				case '1':
					port_index = 0;
					break;

				case '2':
					port_index = 1;
					break;

				case '3':
					port_index = 2;
					break;

				case '4':
					port_index = 3;
					break;

				default:
					if (TempBuf[3] <= '9' && TempBuf[3] >'0') {
						TempBuf[4] = 0;
						Set_Custom_Port(IDC_SERIAL_PORT, 4, TempBuf);
					} else {
						WWMessageBox().Process(LS("TXT_INVALID_PORT_ADDRESS", 0x440), LS("TXT_OK", 0x440), NULL, NULL);
					}
			}
		} else {
			Set_Custom_Port(IDC_SERIAL_PORT, 4, TempBuf);
		}

		if (port_index != -1) {
			index = ComboBox_Find_Item_Data(handle1, -1, port_index);
			ComboBox_SetCurSel(handle1, index);
		}
	}
}

// FUNCTION: GAMEMD 0x00696950
void SerialEdit::Set_Init_String(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_CALLWAITING_EDIT);
	if (handle) {
		ShowWindow(handle, SW_HIDE);
		Edit_Enable(handle, FALSE);
		Edit_GetText(handle, TempBuf, CWAITSTRBUF_MAX);
		TempBuf[CWAITSTRBUF_MAX-1] = '\0';
		strtrim(TempBuf);
		strupr(TempBuf);
		Set_Custom_Port(IDC_SERIAL_CALLWAITING, 3, TempBuf);
	}
}

// FUNCTION: GAMEMD 0x00696C40
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
