// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#undef LoadString
#include "nulldlg.h"
#include "_map.h"
#include "_rules.h"
#include "_timer.h"
#include "data.h"
#include "dbgprint.h"
#include "geometry.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "init.h"
#include "modemgst.h"
#include "modemhst.h"
#include "modemreg.h"
#include "msgbox.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "phone.h"
#include "phoneed.h"
#include "rules.h"
#include "serialed.h"
#include "stimer.h"
#include "stringtable.h"
#include "wincomm.h"
#include <ctime>

// SYNTHETIC: GAMEMD 0x005EF2D0 SYMBOL
// _$E9
// SYNTHETIC: GAMEMD 0x005EF2F0 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x005EF320 SYMBOL
// _$E72
// SYNTHETIC: GAMEMD 0x005EF340 SYMBOL
// _$E75
// SYNTHETIC: GAMEMD 0x005EF360 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x005EF380 SYMBOL
// _$E81
// SYNTHETIC: GAMEMD 0x005EF3A0 SYMBOL
// _$E84
// SYNTHETIC: GAMEMD 0x005EF3C0 SYMBOL
// _$E87
// SYNTHETIC: GAMEMD 0x005EF400 SYMBOL
// _$E90
// SYNTHETIC: GAMEMD 0x005EF430 SYMBOL
// _$E93
// SYNTHETIC: GAMEMD 0x005EF460 SYMBOL
// _$E96
// SYNTHETIC: GAMEMD 0x005EF490 SYMBOL
// _$E99
// SYNTHETIC: GAMEMD 0x005EF4A0 SYMBOL
// _$E102
// SYNTHETIC: GAMEMD 0x005EF4C0 SYMBOL
// _$E105

BOOL CALLBACK Select_Serial_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void Select_Serial_On_INITDIALOG(HWND window);
void Modem_Echo(char c);
bool Reconnect_Null_Modem(void);
bool Dial_Modem(SerialSettingsType *settings, bool reconnect);
bool Answer_Modem(SerialSettingsType *settings, bool reconnect);
static char DialString[CWAITSTRBUF_MAX + PhoneEntryClass::PHONE_MAX_NUM - 1];
SerialSettingsType *DialSettings;

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

static int Test_Null_Modem(void);

// FUNCTION: GAMEMD 0x005EF4E0
GameType Select_Serial_Dialog(void)
{
	int rc = -1;
	GameType retval = (GameType)-1;
	SerialSettingsType *settings;

	HWND dialog = OwnerDraw::Begin_Dialog(IDD_SELECT_SERIAL, Select_Serial_Dialog_Proc, 0);

	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		OwnerDraw::Move_Dialog(dialog, -1, ((HiddenSurface->Get_Height() - 400) / 2) + 147);
		OwnerDraw::Display_Dialog(dialog);

		if (Session.SerialDefaults.Port == 0 ||
			Session.SerialDefaults.IRQ == -1 ||
			Session.SerialDefaults.Baud == -1) {
			rc = IDC_SERIAL_SETTINGS_BTN;

		} else if (NullModem.Detect_Port(&Session.SerialDefaults) != PORT_VALID) {
			rc = IDC_SERIAL_SETTINGS_BTN;
		}

		while (retval == -1) {
			while (rc < 0) {
				if (OwnerDraw::Dialog_Message_Handler() == true) {
					break;
				}
				Title_Screen_Restore();
			}

			switch (rc) {

				case IDC_SERIAL_DIAL:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);
					if (PhoneEdit().Dialog() == true) {
						if (Session.PhoneBook[Session.CurPhoneIdx]->Settings.Port == 0) {
							settings = &Session.SerialDefaults;
						} else {
							settings = &(Session.PhoneBook[Session.CurPhoneIdx]->Settings);
						}

						if (SerialPort) {
							delete SerialPort;
						}
						SerialPort = new WinModemClass;

						if (Init_Null_Modem(settings)) {

							if (settings->CallWaitStringIndex == CALL_WAIT_CUSTOM) {
								strcpy(DialString, settings->CallWaitString);
							} else {
								OwnerDraw::WideToCharString(DialString, Session.CallWaitStrings[settings->CallWaitStringIndex], -1);
							}
							strcat(DialString, Session.PhoneBook[Session.CurPhoneIdx]->Number);

							if (Dial_Modem(settings, false)) {
								Session.ModemType = MODEM_DIALER;
								Session.Type = GAME_MODEM;
								if (ModemHost().Dialog()) {
									retval = GAME_MODEM;
									break;
								} else {
									Session.Type = GAME_NORMAL;
									retval = GAME_NORMAL;
								}
							}
							NullModem.Change_IRQ_Priority(0);
						} else {
							WWMessageBox().Process(StringTable::LoadString("TXT_SELECT_SETTINGS", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5798), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5798), NULL, NULL);
						}
					}
					ShowWindow(dialog, SW_SHOW);
					break;

				case IDC_SERIAL_ANSWER:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);

					settings = &Session.SerialDefaults;

					if (SerialPort) {
						delete SerialPort;
					}
					SerialPort = new WinModemClass;

					if (Init_Null_Modem(settings)) {
						if (Answer_Modem(settings, false)) {
							Session.ModemType = MODEM_ANSWERER;
							Session.Type = GAME_MODEM;
							if (ModemGuest().Dialog()) {
								retval = GAME_MODEM;
								break;
							} else {
								Session.Type = GAME_NORMAL;
								retval = GAME_NORMAL;
							}
						}
						NullModem.Change_IRQ_Priority(0);
					} else {
						WWMessageBox().Process(StringTable::LoadString("TXT_SELECT_SETTINGS", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5833), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5833), NULL, NULL);
					}

					ShowWindow(dialog, SW_SHOW);
					break;

				case IDC_SERIAL_NULLMODEM:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);

					if (Init_Null_Modem(&Session.SerialDefaults)) {
						switch (Test_Null_Modem()) {
							case (1):
								Session.ModemType = MODEM_NULL_HOST;
								Session.Type = GAME_NULL_MODEM;
								if (ModemHost().Dialog() == 1) {
									retval = GAME_NULL_MODEM;
									break;
								} else {
									Session.Type = GAME_NORMAL;
								}
								break;

							case (2):
								Session.ModemType = MODEM_NULL_JOIN;
								Session.Type = GAME_NULL_MODEM;
								if (ModemGuest().Dialog() == 1) {
									retval = GAME_NULL_MODEM;
									break;
								} else {
									Session.Type = GAME_NORMAL;
								}
								break;

							case (3):
								WWMessageBox().Process(StringTable::LoadString("TXT_MODEM_OR_LOOPBACK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5868), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5868), NULL, NULL);
								break;
						}
						if (retval == -1) {
							NullModem.Change_IRQ_Priority(0);
							ShowWindow(dialog, SW_SHOW);
						}
					} else {
						WWMessageBox().Process(StringTable::LoadString("TXT_SELECT_SETTINGS", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5877), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 5877), NULL, NULL);
						ShowWindow(dialog, SW_SHOW);
					}
					break;

				case IDC_SERIAL_SETTINGS_BTN:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);
					if (SerialEdit().Dialog(&Session.SerialDefaults)) {
						Session.Write_MultiPlayer_Settings();
					}
					Select_Serial_On_INITDIALOG(dialog);
					ShowWindow(dialog, SW_SHOW);
					break;

				default:
					retval = GAME_NORMAL;
					break;
			}

			rc = -1;
		}

		OwnerDraw::End_Dialog(dialog);
		return(retval);
	}

	return(GAME_NORMAL);
}

// FUNCTION: GAMEMD 0x005EFF00
BOOL CALLBACK Select_Serial_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int *retval;
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		switch (message) {
			case WM_COMMAND:
				retval = (int *)GetWindowLong(window, DWL_USER);
				if (wparam == 2 || wparam > IDC_DISCONNECT_MESSAGES && wparam <= IDC_SERIAL_NULLMODEM) {
					*retval = wparam;
				}
				break;

			case 0x497:
				Select_Serial_On_INITDIALOG(window);
				break;
		}
		return(0);
	}
	return(rc);
}

// FUNCTION: GAMEMD 0x005EFF70
void Select_Serial_On_INITDIALOG(HWND window)
{
	HWND handle;

	bool enabled = true;
	if (Session.SerialDefaults.Port == 0) {
		enabled = false;
	} else if (Session.SerialDefaults.IRQ == -1) {
		enabled = false;
	} else if (Session.SerialDefaults.Baud == -1 || NullModem.Detect_Port(&Session.SerialDefaults)) {
		enabled = false;
	}

	handle = GetDlgItem(window, IDC_SERIAL_DIAL);
	if (handle) {
		EnableWindow(handle, enabled);
	}

	handle = GetDlgItem(window, IDC_SERIAL_ANSWER);
	if (handle) {
		EnableWindow(handle, enabled);
	}

	handle = GetDlgItem(window, IDC_SERIAL_NULLMODEM);
	if (handle) {
		EnableWindow(handle, enabled);
	}
}

// FUNCTION: GAMEMD 0x005F0010
bool Init_Null_Modem( SerialSettingsType *settings )
{
	if ( NullModem.Init( settings->Port, settings->IRQ,
								settings->ModemName,
								settings->Baud, 0,  8, 1 ) ) {
		return(true);
	} else {
		return(false);
	}
}

// FUNCTION: GAMEMD 0x005F0040
void Shutdown_Modem( void )
{
	if (!Session.Play) {
		if (Session.Type == GAME_MODEM) {
			NullModem.Hangup_Modem();
		}
	}

	NullModem.Shutdown();
}

// FUNCTION: GAMEMD 0x005F0070
void Modem_Signoff(void)
{
	unsigned int starttime;
	EventClass event;

	if (!Session.Play) {
		memset(&event, 0, sizeof(EventClass));
		event.Type = EventClass::EXIT;
		NullModem.Send_Message(&event, sizeof(EventClass), 0);
		NullModem.Send_Message(&event, sizeof(EventClass), 0);

		starttime = TickCount;
		while ((TickCount - starttime) < TIMER_SECOND / 2) {
			NullModem.Service();
		}
	}
}

static int Test_Null_Modem(void)
{
	bool process;

	int retval = 0;
	unsigned int starttime;
	int packetlen;

	char buffer[80*3];
	SerialPacketType SendPacket;
	SerialPacketType ReceivePacket;

	process = true;

	SetCommBreak(SerialPort->Get_Port_Handle());

	SerialPort->Write_To_Serial_Port((unsigned char*)"ATH\r", strlen("ATH\r"));
	CDTimerClass<SystemTimerClass> time;
	time = (2*TIMER_SECOND);
	while (time) {}

	ClearCommBreak(SerialPort->Get_Port_Handle());

	EscapeCommFunction(SerialPort->Get_Port_Handle(), CLRDTR);

	starttime = TickCount;
	while (TickCount - starttime < 80) {
		NullModem.Service();
		if (NullModem.Get_Message(&ReceivePacket, &packetlen) > 0) {
			if (ReceivePacket.Command == SERIAL_CONNECT) {
				starttime = TickCount;
				while (TickCount - starttime < TIMER_SECOND / 2)
					NullModem.Service();
				process = false;
				retval = 2;
				break;
			}
		}
	}

	memset(&SendPacket, 0, sizeof(SerialPacketType));
	if (process == true) {
		SendPacket.Command = SERIAL_CONNECT;
		SendPacket.ScenarioInfo.Seed = TickCount;
		SendPacket.ID = (int) buffer;

		NullModem.Send_Message(&SendPacket, sizeof(SendPacket), 1);

		starttime = TickCount;
		while (TickCount - starttime < 80) {
			NullModem.Service();
			if (NullModem.Get_Message(&ReceivePacket, &packetlen) > 0) {
				if (ReceivePacket.Command == SERIAL_CONNECT) {
					starttime = TickCount;
					while (TickCount - starttime < TIMER_SECOND / 2)
						NullModem.Service();

					if (ReceivePacket.ScenarioInfo.Seed > SendPacket.ScenarioInfo.Seed) {
						process = false;
						retval = 2;
					} else if (ReceivePacket.ScenarioInfo.Seed == SendPacket.ScenarioInfo.Seed) {
						if (ReceivePacket.ID > SendPacket.ID) {
							process = false;
							retval = 2;
						} else if (ReceivePacket.ID == SendPacket.ID) {
							process = false;
							retval = 3;
						}
					}

					break;
				}
			}
		}

		if (process) {
			ModemDialogIsOpen = false;
			HWND dialog = OwnerDraw::Custom_Message_Box(StringTable::LoadString("TXT_WAITING_CONNECT", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6293), StringTable::LoadString("TXT_CANCEL", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6293), &ModemDialogIsOpen);

			if (dialog) {

				OwnerDraw::Display_Dialog(dialog);

				starttime = TickCount;

				while (true) {

					if (OwnerDraw::Dialog_Message_Handler() == 1 || ModemDialogIsOpen) {
						retval = 0;
						break;
					}

					Title_Screen_Restore();

					NullModem.Service();
					if (NullModem.Num_Send() == 0) {
						if (NullModem.Get_Message(&ReceivePacket, &packetlen) > 0) {
							if (ReceivePacket.Command == SERIAL_CONNECT) {
								starttime = TickCount;
								while (TickCount - starttime < TIMER_SECOND / 2)
									NullModem.Service();

								if (ReceivePacket.ScenarioInfo.Seed > SendPacket.ScenarioInfo.Seed) {
									process = false;
									retval = 2;

								} else if (ReceivePacket.ScenarioInfo.Seed == SendPacket.ScenarioInfo.Seed) {
									if (ReceivePacket.ID > SendPacket.ID) {
										process = false;
										retval = 2;

									} else if (ReceivePacket.ID == SendPacket.ID) {
										process = false;
										retval = 3;
									}
								}

							} else {
								retval = 0;
								process = false;
							}
						} else {
							retval = 1;
							process = false;
						}
					}

					if (TickCount - starttime > TIMER_MINUTE) {
						retval = 0;
						break;
					}

					if (process != true) {
						break;
					}
				}

				OwnerDraw::End_Dialog(dialog);
			}
		}
	}

	return(retval);
}

// FUNCTION: GAMEMD 0x005F0120
bool Reconnect_Modem(void)
{
	bool status = false;
	int modemstatus;

	switch (Session.ModemType) {
		case (MODEM_NULL_HOST):
		case (MODEM_NULL_JOIN):
			status = Reconnect_Null_Modem();
			break;

		case (MODEM_DIALER):
			modemstatus = NullModem.Get_Modem_Status();
			if ((modemstatus & WinModemClass::CD_SET)) {
				status = Reconnect_Null_Modem();
			} else {
				status = Dial_Modem(DialSettings, true);
			}
			break;

		case (MODEM_ANSWERER):
			modemstatus = NullModem.Get_Modem_Status();
			if ((modemstatus & WinModemClass::CD_SET)) {
				status = Reconnect_Null_Modem();
			} else {
				status = Answer_Modem(DialSettings, true);
			}
			break;
	}

	return(status);
}

// FUNCTION: GAMEMD 0x005F0180
void Destroy_Null_Connection(int id, int error)
{
	int i;
	HouseClass *housep;
	wchar_t txt[80];

	if (Session.NumPlayers == 1) {
		return;
	}

	housep = Houses[id];
	if (!housep || !housep->IsHuman) {
		return;
	}

	txt[0] = L'\0';
	switch (error) {
		case 1:
			swprintf(txt, StringTable::LoadString("TXT_CONNECTION_LOST", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x190c), housep->UIName);
			break;

		case 0:
			swprintf(txt, StringTable::LoadString("TXT_LEFT_GAME", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x1910), housep->UIName);
			break;

		case -1:
			NullModem.Delete_Connection();
			break;
	}

	if (wcslen(txt)) {
		Session.Messages.Add_Message(NULL, 0, txt, housep->Class->Scheme, TextPrintType(0x4046), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw();
	}

	for (i = 0; i < Session.Players.Count(); i++) {
		if (!wcsicmp(Session.Players[i]->Name, housep->UIName)) {
			delete Session.Players[i];
			Session.Players.Delete(Session.Players[i]);
			break;
		}
	}

	housep->AI_Takeover();

	Session.NumPlayers--;

	if (Session.NumPlayers == 1) {
		swprintf(txt, L"%s", StringTable::LoadString("TXT_JUST_YOU_AND_ME", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x1935));
		Session.Messages.Add_Message(NULL, 0, txt, housep->Class->Scheme, TextPrintType(0x4046), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw();
	}
}

// FUNCTION: GAMEMD 0x005F03B0
bool Reconnect_Null_Modem(void)
{
	bool retval = false;
	bool process = true;
	ModemDialogIsOpen = false;

	unsigned int starttime;
	unsigned int lastmsgtime;
	int packetlen;
	SerialPacketType SendPacket;
	SerialPacketType ReceivePacket;

	HWND dialog = OwnerDraw::Custom_Message_Box(StringTable::LoadString("TXT_NULL_CONNERR_CHECK_CABLES", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x1959), StringTable::LoadString("TXT_CANCEL", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x1959), &ModemDialogIsOpen);

	if (dialog != NULL) {
		OwnerDraw::Display_Dialog(dialog);

		starttime = lastmsgtime = TickCount;
		while (process == true) {
			if (OwnerDraw::Dialog_Message_Handler() == true || ModemDialogIsOpen != false) {
				retval = false;
				break;
			}

			NullModem.Service();

			if (TickCount - starttime > PACKET_RETRANS_TIME) {
				starttime = TickCount;
				memset(&SendPacket, 0, sizeof(SerialPacketType));
				SendPacket.Command = SERIAL_CONNECT;
				SendPacket.ID = Session.PrefColor;
				NullModem.Send_Message(&SendPacket, sizeof(SendPacket), 0);
			}

			if (NullModem.Get_Message(&ReceivePacket, &packetlen) > 0) {
				lastmsgtime = TickCount;

				if (ReceivePacket.Command == SERIAL_CONNECT) {
					if (ReceivePacket.ID == Session.PrefColor) {
						WWMessageBox().Process(StringTable::LoadString("TXT_SYSTEM_NOT_RESPONDING", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x1982), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 0x1982), NULL, NULL);
						retval = false;
						process = false;
						break;
					}

					memset(&SendPacket, 0, sizeof(SerialPacketType));
					SendPacket.Command = SERIAL_CONNECT;
					SendPacket.ID = Session.PrefColor;
					NullModem.Send_Message(&SendPacket, sizeof(SendPacket), 1);
					starttime = TickCount;
					while (TickCount - starttime < TIMER_SECOND) {
						NullModem.Service();
					}
					retval = true;
					process = false;
				}
			}

			if (TickCount - lastmsgtime > PACKET_CANCEL_TIMEOUT) {
				retval = false;
				process = false;
			}
		}

		OwnerDraw::End_Dialog(dialog);
	}

	return(retval);
}

// FUNCTION: GAMEMD 0x005F0690
bool Dial_Modem(SerialSettingsType *settings, bool reconnect)
{
	bool connected = false;
	DialStatusType dialstatus;
	int modemstatus;

	DebugString("Dial Modem\n");

	Session.ModemService = false;

	DialSettings = settings;

	modemstatus = NullModem.Get_Modem_Status();

	DebugString("Get_Modem_Status indicates the following...\n");
	DebugString("CTS: %s\n", (modemstatus & WinModemClass::CTS_SET) ? "SET" : "CLEAR");
	DebugString("DSR: %s\n", (modemstatus & WinModemClass::DSR_SET) ? "SET" : "CLEAR");
	DebugString(" RI: %s\n", (modemstatus & WinModemClass::RI_SET) ? "SET" : "CLEAR");
	DebugString(" CD: %s\n", (modemstatus & WinModemClass::CD_SET) ? "SET" : "CLEAR");

	if (reconnect) {
		if ((modemstatus & WinModemClass::CD_SET)) {
			connected = true;
			Session.ModemService = true;
			return(connected);
		}
	} else if ((modemstatus & WinModemClass::CD_SET)) {
		DebugString("Modem already on-line - hanging up\n");
		NullModem.Hangup_Modem();
		Session.ModemService = false;
	}

	NullModem.Setup_Modem_Echo(Modem_Echo);

	modemstatus = NullModem.Detect_Modem(settings, reconnect);
	if (!modemstatus) {
		DebugString("Detect_Modem failed\n");
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();

		switch (settings->Baud) {
			case 14400:
				DebugString("Upshifting to 19200\n");
				settings->Baud = 19200;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo(Modem_Echo);
				modemstatus = NullModem.Detect_Modem(settings, reconnect);
				if (!modemstatus) {
					DebugString("Detect_Modem failed at 19200\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_FIND_MODEM", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6664), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6664), NULL, NULL);
					Session.ModemService = true;
					return(connected);
				}
				break;

			case 28800:
				DebugString("Upshifting to 38400\n");
				settings->Baud = 38400;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo(Modem_Echo);
				modemstatus = NullModem.Detect_Modem(settings, reconnect);
				if (!modemstatus) {
					DebugString("Detect_Modem failed at 38400\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_FIND_MODEM", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6685), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6685), NULL, NULL);
					Session.ModemService = true;
					return(connected);
				}
				break;

			default:
				WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_FIND_MODEM", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6692), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6692), NULL, NULL);
				Session.ModemService = true;
				return(connected);
		}
	} else if (modemstatus == -1) {
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();
		WWMessageBox().Process(StringTable::LoadString("TXT_ERROR_IN_INITSTRING", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6705), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6705), NULL, NULL);
		Session.ModemService = true;
		return(connected);
	}

	DebugString("Ready to dial\n");

	dialstatus = NullModem.Dial_Modem(DialString, settings->DialMethod, reconnect);

	if (reconnect) {
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		Map.Render();
	}

	switch (dialstatus) {
		case DIAL_CONNECTED:
			DebugString("Dial_Modem returned 'DIAL_CONNECTED'\n");
			connected = true;
			break;

		case DIAL_NO_CARRIER:
			DebugString("Dial_Modem returned 'DIAL_NO_CARRIER'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_NO_CARRIER", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6758), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6758), NULL, NULL);
			connected = false;
			break;

		case DIAL_BUSY:
			DebugString("Dial_Modem returned 'DIAL_BUSY'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_LINE_BUSY", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6764), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6764), NULL, NULL);
			connected = false;
			break;

		case DIAL_ERROR:
			DebugString("Dial_Modem returned 'DIAL_ERROR'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_NUMBER_INVALID", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6770), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6770), NULL, NULL);
			connected = false;
			break;

		case DIAL_TIMEOUT:
			DebugString("Dial_Modem returned 'DIAL_TIMEOUT'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_ERROR_TIMEOUT", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6776), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6776), NULL, NULL);
			connected = false;
			break;

		case DIAL_NO_DIAL_TONE:
			DebugString("Dial_Modem returned 'DIAL_NO_DIAL_TONE'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_NO_DIAL_TONE", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6782), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6782), NULL, NULL);
			connected = false;
			break;

		case DIAL_CANCELED:
			DebugString("Dial_Modem returned 'DIAL_CANCELLED'\n");
			NullModem.Hangup_Modem();
			Session.ModemService = false;
			WWMessageBox().Process(StringTable::LoadString("TXT_DIALING_CANCELED", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6790), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6790), NULL, NULL);
			connected = false;
			break;
	}

	NullModem.Remove_Modem_Echo();
	NullModem.Print_EchoBuf();
	NullModem.Reset_EchoBuf();

	Session.ModemService = true;

	return(connected);
}

// FUNCTION: GAMEMD 0x005F0D60
bool Answer_Modem(SerialSettingsType *settings, bool reconnect)
{
	bool connected = false;

	DialStatusType dialstatus;
	int modemstatus;

	DebugString("Answer Modem\n");

	Session.ModemService = false;

	DialSettings = settings;

	modemstatus = NullModem.Get_Modem_Status();

	DebugString("Get_Modem_Status indicates the following...\n");
	DebugString("CTS: %s\n", (modemstatus & WinModemClass::CTS_SET) ? "SET" : "CLEAR");
	DebugString("DSR: %s\n", (modemstatus & WinModemClass::DSR_SET) ? "SET" : "CLEAR");
	DebugString(" RI: %s\n", (modemstatus & WinModemClass::RI_SET) ? "SET" : "CLEAR");
	DebugString(" CD: %s\n", (modemstatus & WinModemClass::CD_SET) ? "SET" : "CLEAR");

	if (reconnect) {
		if ((modemstatus & WinModemClass::CD_SET)) {
			connected = true;
			Session.ModemService = true;
			return(connected);
		}
	} else if ((modemstatus & WinModemClass::CD_SET)) {
		DebugString("Modem already on-line - hanging up\n");
		NullModem.Hangup_Modem();
		Session.ModemService = false;
	}

	NullModem.Setup_Modem_Echo(Modem_Echo);

	modemstatus = NullModem.Detect_Modem(settings, reconnect);
	if (!modemstatus) {
		DebugString("Detect_Modem failed\n");
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();

		switch (settings->Baud) {
			case 14400:
				DebugString("Upshifting to 19200\n");
				settings->Baud = 19200;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo(Modem_Echo);
				modemstatus = NullModem.Detect_Modem(settings, reconnect);
				if (!modemstatus) {
					DebugString("Detect_Modem failed at 19200\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_FIND_MODEM", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6887), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6887), NULL, NULL);
					Session.ModemService = true;
					return(connected);
				}
				break;

			case 28800:
				DebugString("Upshifting to 38400\n");
				settings->Baud = 38400;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo(Modem_Echo);
				modemstatus = NullModem.Detect_Modem(settings, reconnect);
				if (!modemstatus) {
					DebugString("Detect_Modem failed at 38400\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_FIND_MODEM", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6908), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6908), NULL, NULL);
					Session.ModemService = true;
					return(connected);
				}
				break;

			default:
				WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_FIND_MODEM", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6915), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6915), NULL, NULL);
				Session.ModemService = true;
				return(connected);
		}
	} else if (modemstatus == -1) {
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();
		WWMessageBox().Process(StringTable::LoadString("TXT_ERROR_IN_INITSTRING", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6925), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6925), NULL, NULL);
		Session.ModemService = true;
		return(connected);
	}

	DebugString("Ready to answer\n");

	dialstatus = NullModem.Answer_Modem(reconnect);

	switch (dialstatus) {
		case DIAL_CONNECTED:
			DebugString("Answer_Modem returned 'DIAL_CONNECTED'\n");
			connected = true;
			break;

		case DIAL_NO_CARRIER:
			DebugString("Answer_Modem returned 'DIAL_NO_CARRIER'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_NO_CARRIER", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6941), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6941), NULL, NULL);
			connected = false;
			break;

		case DIAL_ERROR:
			DebugString("Answer_Modem returned 'DIAL_ERROR'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_NUMBER_INVALID", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6947), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6947), NULL, NULL);
			connected = false;
			break;

		case DIAL_TIMEOUT:
			DebugString("Answer_Modem returned 'DIAL_TIMEOUT'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_ERROR_TIMEOUT", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6953), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6953), NULL, NULL);
			connected = false;
			break;

		case DIAL_CANCELED:
			DebugString("Answer_Modem returned 'DIAL_CANCELLED'\n");
			WWMessageBox().Process(StringTable::LoadString("TXT_ANSWERING_CANCELED", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6959), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\NullDlg.CPP", 6959), NULL, NULL);
			connected = false;
			break;
	}

	NullModem.Remove_Modem_Echo();
	NullModem.Print_EchoBuf();
	NullModem.Reset_EchoBuf();

	Session.ModemService = true;

	return(connected);
}

// FUNCTION: GAMEMD 0x005F1350
void Modem_Echo(char c)
{
	if (NullModem.EchoCount < (NullModem.EchoSize - 1)) {
		*(NullModem.EchoBuf + NullModem.EchoCount) = c;
		*(NullModem.EchoBuf + NullModem.EchoCount + 1) = 0;
		NullModem.EchoCount++;
	}
}

// TEMPLATE: GAMEMD 0x005F1390
// ??B?$TTimerClass@VSystemTimerClass@@@@QBEHXZ
template class TTimerClass<SystemTimerClass>;
