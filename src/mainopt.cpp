// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "mainopt.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_surface.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "misc.h"
#include "mixfile.h"
#include "msgbox.h"
#include "newmenu.h"
#include "ownrdraw.h"
#include "session.h"
#include "sidebar.h"
#include "sounddlg.h"
#include "techno.h"
#include "geometry.h"
#include "theme.h"
#include "stimer.h"
#include "surface.h"
#include "winfix.h"
#include "wspipx.h"
#include "wwmous.h"

#include "color.hh"

#undef LoadString
#include "stringtable.h"

#include <math.h>

// SYNTHETIC: GAMEMD 0x0055F760 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x0055F780 SYMBOL
// _$E104
// SYNTHETIC: GAMEMD 0x0055F7B0 SYMBOL
// _$E107
// SYNTHETIC: GAMEMD 0x0055F7D0 SYMBOL
// _$E110
// SYNTHETIC: GAMEMD 0x0055F7F0 SYMBOL
// _$E113
// SYNTHETIC: GAMEMD 0x0055F810 SYMBOL
// _$E116
// SYNTHETIC: GAMEMD 0x0055F830 SYMBOL
// _$E119
// SYNTHETIC: GAMEMD 0x0055F850 SYMBOL
// _$E122
// SYNTHETIC: GAMEMD 0x0055F890 SYMBOL
// _$E125
// SYNTHETIC: GAMEMD 0x0055F8C0 SYMBOL
// _$E128
// SYNTHETIC: GAMEMD 0x0055F8F0 SYMBOL
// _$E131
// SYNTHETIC: GAMEMD 0x0055F920 SYMBOL
// _$E134
// SYNTHETIC: GAMEMD 0x0055F930 SYMBOL
// _$E137
// SYNTHETIC: GAMEMD 0x0055F950 SYMBOL
// _$E140
// SYNTHETIC: GAMEMD 0x0055F970 SYMBOL
// _$E143
GameOptionsClass TempOptions;

// FUNCTION: GAMEMD 0x0055F980
BOOL CALLBACK Mode_Prompt_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		int * result = (int *)GetWindowLong(window, DWL_USER);
		switch (message) {
			case WM_COMMAND:
				switch (LOWORD(wparam)) {
					case 0x5C1:
						*result = 1;
						break;

					case 0x6CA:
						*result = 2;
						return(0);
				}
				break;

			case 0x497: {
				wchar_t const * text = StringTable::LoadString("TXT_ABOUT_TO_TRY_MODE", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0xA2);
				HWND item = GetDlgItem(window, 0x5DB);
				SendMessage(item, 0x4B2, 0, (LPARAM)text);
				return(0);
			}
		}
		return(0);
	}
	return(rc);
}

namespace UI {
	HWND __fastcall BeginDialog(int id, DLGPROC proc, int extra);
	void __fastcall EndDialog(HWND window);
}

// FUNCTION: GAMEMD 0x0055FA30
bool Mode_Prompt_Dialog(void)
{
	int res = -1;

	HWND dialog = UI::BeginDialog(0xD9, (DLGPROC)Mode_Prompt_Dialog_Proc, 0);
	if (dialog != NULL) {
		SetWindowLong(dialog, DWL_USER, (LONG)&res);
		OwnerDraw::Display_Dialog(dialog);

		while (res < 0) {
			if (OwnerDraw::Dialog_Message_Handler()) {
				break;
			}
			Title_Screen_Restore();
		}
		UI::EndDialog(dialog);
	}
	return(res == 1);
}

extern const float DAT_007E1748;
extern const double DAT_007E3860;

// FUNCTION: GAMEMD 0x0055FAA0
void Apply_Game_Settings(HWND window)
{
	HWND item = GetDlgItem(window, 0x52B);
	if (item) {
		int v = SendMessage(item, 0x400, 0, 0) ? 2 : 0;
		if (Options.DetailLevel != v) {
			Options.DetailLevel = v;
			Map.Reinit_Cell_Drawers();
		}
	}

	item = GetDlgItem(window, 0x50F);
	if (item) {
		Options.Difficulty = SendMessage(item, 0x400, 0, 0);
	}

	item = GetDlgItem(window, 0x601);
	if (item) {
		Options.ActionLines = SendMessage(item, 0xF0, 0, 0) == 1;
		TechnoClass::Set_Action_Lines(Options.ActionLines);
	}

	item = GetDlgItem(window, 0x604);
	if (item) {
		Options.ShowHidden = SendMessage(item, 0xF0, 0, 0) == 1;
	}

	item = GetDlgItem(window, 0x602);
	if (item) {
		Options.ToolTips = SendMessage(item, 0xF0, 0, 0) == 1;
	}

	item = GetDlgItem(window, 0x52A);
	if (item) {
		Options.ScrollRate = 6 - SendMessage(item, 0x400, 0, 0);
	}

	item = GetDlgItem(window, 0x52F);
	if (item) {
		Options.Set_Score_Volume(SendMessage(item, 0x400, 0, 0) * DAT_007E3860, false);
		if (Options.ScoreVolume == DAT_007E1748) {
			Theme.Queue_Song(Theme.What_Is_Playing());
			Theme.Stop(false);
		}
	}

	item = GetDlgItem(window, 0x532);
	if (item) {
		Options.Set_Sound_Volume(SendMessage(item, 0x400, 0, 0) * DAT_007E3860, false);
	}

	item = GetDlgItem(window, 0x536);
	if (item) {
		Options.Set_Voice_Volume(SendMessage(item, 0x400, 0, 0) * DAT_007E3860, false);
	}
}

BOOL CALLBACK FUN_0055FDB0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK FUN_00560480(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x0055FC80
void Main_Options_Dialog(void)
{
	bool old_game_active = GameActive;
	GameActive = false;

	HWND handle;
	int rc;
	int in_rc;

	while (true) {
		do {
			TempOptions = Options;
			rc = -1;
			handle = UI::BeginDialog(0xD5, (DLGPROC)FUN_0055FDB0, 0);
		} while (handle == NULL);
		SetWindowLong(handle, DWL_USER, (LONG)&rc);
		OwnerDraw::Display_Dialog(handle);

		while (rc < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		Apply_Game_Settings(handle);
		UI::EndDialog(handle);

		switch (rc) {
			case 0x5CD:
				in_rc = -1;
				Session.Read_MultiPlayer_Settings();
				handle = UI::BeginDialog(0xD7, (DLGPROC)FUN_00560480, 0);
				if (handle) {
					SetWindowLong(handle, DWL_USER, (LONG)&in_rc);
					OwnerDraw::Display_Dialog(handle);

					while (in_rc < 0) {
						if (OwnerDraw::Dialog_Message_Handler() == true) {
							break;
						}
						Title_Screen_Restore();
					}

					UI::EndDialog(handle);
				}
				break;

			case 0x5CE:
				Options.Hotkey_Dialog();
				break;

			default:
				Options.Save_Settings();
				GameActive = old_game_active;
				return;
		}
	}
}

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
extern char const * DAT_0082A248[];
extern char const * DAT_0082A254[];
extern char const * DAT_0082A260[];
extern int FUN_00407000(void);

// FUNCTION: GAMEMD 0x0055FDB0
BOOL CALLBACK FUN_0055FDB0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int * _modes = NULL;
	static int _current_mode = -1;
	static bool _initialized = false;
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc == 0) {
		int * result = (int *)GetWindowLong(window, DWL_USER);
		switch (message) {
			case WM_COMMAND:
				switch (LOWORD(wparam)) {
					case 0x5CD:
					case 0x5CE:
						delete [] _modes;
						_modes = NULL;
						*result = LOWORD(wparam);
						break;
					case 0x686:
						if (HIWORD(wparam) == 0) {
							delete [] _modes;
							_modes = NULL;
							*(int *)GetWindowLong(window, DWL_USER) = 0x5CB;
						}
						break;
					case 0x6ED:
						if (HIWORD(wparam) == 1) {
							Center_Window_Within_Window(window, MainWindow);
							HWND list = GetDlgItem(window, 0x6ED);
							if (list) {
								int selected = ComboBox_GetCurSel(list);
								int index = ComboBox_GetItemData(list, selected);
								TempOptions.ScreenWidth = _modes[2 * index];
								TempOptions.ScreenHeight = _modes[2 * index + 1];
								if (TempOptions.ScreenWidth != Options.ScreenWidth || TempOptions.ScreenHeight != Options.ScreenHeight) {
									Options.ScreenWidth = TempOptions.ScreenWidth;
									Options.ScreenHeight = TempOptions.ScreenHeight;
								}
							}
						}
						break;
				}
				break;
			case WM_DESTROY:
				if (_modes != NULL) {
					delete [] _modes;
					_modes = NULL;
				}
				break;
			case WM_HSCROLL:
				if (LOWORD(wparam) == SB_THUMBTRACK) {
					int index = HIWORD(wparam);
					char const * name;
					HWND handle = NULL;
					if ((HWND)lparam == GetDlgItem(window, 0x52B)) {
						int detail = index;
						if (index) detail = 2;
						name = DAT_0082A248[detail];
						handle = GetDlgItem(window, 0x673);
					} else if ((HWND)lparam == GetDlgItem(window, 0x50F)) {
						name = DAT_0082A254[index];
						handle = GetDlgItem(window, 0x670);
					} else if ((HWND)lparam == GetDlgItem(window, 0x52A)) {
						name = DAT_0082A260[index];
						handle = GetDlgItem(window, 0x672);
					} else if ((HWND)lparam == GetDlgItem(window, 0x52F)) {
						if (_initialized) Options.Set_Score_Volume(SendMessage((HWND)lparam, 0x400, 0, 0) * DAT_007E3860, true);
					} else if ((HWND)lparam == GetDlgItem(window, 0x532)) {
						if (_initialized) Options.Set_Sound_Volume(SendMessage((HWND)lparam, 0x400, 0, 0) * DAT_007E3860, true);
					} else if ((HWND)lparam == GetDlgItem(window, 0x536)) {
						if (_initialized) Options.Set_Voice_Volume(SendMessage((HWND)lparam, 0x400, 0, 0) * DAT_007E3860, true);
					}
					if (handle) {
						wchar_t const * text = StringTable::LoadString(name, NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x229);
						SendMessage(handle, 0x4B2, 0, (LPARAM)text);
					}
				}
				break;
			case 0x497: {
				_initialized = false;
				HWND handle = GetDlgItem(window, 0x52B);
				if (handle) {
					SendMessage(handle, 0x4AC, 0, 0);
					Slider_SetRange(handle, 0, 1);
					Slider_SetPos(handle, Options.DetailLevel != 0);
				}
				HWND list = GetDlgItem(window, 0x6ED);
				if (Options.AllowHiResModes) {
					_modes = EnumDisplayModes(640, 480, 4096, 4096, 16);
				} else {
					_modes = EnumDisplayModes(640, 480, 1024, 768, 16);
				}
				int * modes = _modes;
				int initial_mode = -1;
				int mode_index = 0;
				int item_index = 0;
				if (modes != NULL) {
					while (*modes != 0) {
						int width = *modes++;
						int height = *modes++;
						if (Options.AllowHiResModes || width == 640 || width == 800 || width == 1024) {
							if (width == TempOptions.ScreenWidth && height == TempOptions.ScreenHeight) initial_mode = mode_index;
							char buffer[64];
							sprintf(buffer, "%d x %d x 16", width, height);
							int index = SendMessage(list, 0x4BC, 0, (LPARAM)buffer);
							ComboBox_SetItemData(list, index, item_index);
							mode_index++;
							item_index++;
						} else {
							item_index++;
						}
					}
				}
				ComboBox_SetCurSel(list, initial_mode);
				_current_mode = initial_mode;
				handle = GetDlgItem(window, 0x50F);
				if (handle) {
					SendMessage(handle, 0x4AC, 0, 0);
					Slider_SetRange(handle, 0, 2);
					Slider_SetPos(handle, Options.Difficulty);
				}
				handle = GetDlgItem(window, 0x601);
				if (handle) Button_SetCheck(handle, Options.ActionLines != false);
				handle = GetDlgItem(window, 0x604);
				if (handle) Button_SetCheck(handle, Options.ShowHidden != false);
				handle = GetDlgItem(window, 0x602);
				if (handle) Button_SetCheck(handle, Options.ToolTips != false);
				handle = GetDlgItem(window, 0x52A);
				if (handle) {
					SendMessage(handle, 0x4AC, 0, 0);
					Slider_SetRange(handle, 0, 6);
					Slider_SetPos(handle, 6 - Options.ScrollRate);
				}
				int available = FUN_00407000();
				handle = GetDlgItem(window, 0x52F);
				if (handle) {
					SendMessage(handle, 0x4AE, 0, 0);
					Slider_SetRange(handle, 0, 10);
					Slider_SetPos(handle, (int)(Options.ScoreVolume * 10.0 + 0.5));
					EnableWindow(handle, available);
				}
				handle = GetDlgItem(window, 0x532);
				if (handle) {
					SendMessage(handle, 0x4AE, 0, 0);
					Slider_SetRange(handle, 0, 10);
					Slider_SetPos(handle, (int)(Options.SoundVolume * 10.0 + 0.5));
					EnableWindow(handle, available);
				}
				handle = GetDlgItem(window, 0x536);
				if (handle) {
					SendMessage(handle, 0x4AE, 0, 0);
					Slider_SetRange(handle, 0, 10);
					Slider_SetPos(handle, (int)(Options.VoiceVolume * 10.0 + 0.5));
					EnableWindow(handle, available);
				}
				_initialized = true;
				break;
			}
		}
		return 0;
	}
	return rc;
}


#include "ccini.h"
#include "rawfile.h"
#include "firewall.h"

extern WinsockInterfaceClass * PacketTransport;
extern CCINIClass INI_RA2MD;

bool __fastcall Focus_Next_Tab_Item(HWND window, bool previous);

// FUNCTION: GAMEMD 0x00560480
BOOL CALLBACK FUN_00560480(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	static int _current_ipx = -1;

	HWND handle;
	int i;
	bool inited;
	int *result;
	char socket_string[16];
	char addr_string[32];

	int rc = UI::StandardWndProc(window, message, wparam, lparam);

	if (rc == 0) {
		result = (int *)GetWindowLong(window, DWL_USER);

		switch (message) {
			case 0x497: {
				char addr_str[64];
				HWND combobox = GetDlgItem(window, 0x5D4);

				inited = false;
				if (PacketTransport == NULL) {
					PacketTransport = new IPXInterfaceClass;
					PacketTransport->Init();
					inited = true;
				}

				if (PacketTransport != NULL) {
					DebugString("Creating IPX Interface\n");
					IPXInterfaceClass *ipxi = (IPXInterfaceClass *)PacketTransport;
					for (i = 0; i < 20; i++) {
						if (ipxi->Get_Network_Card_Address_As_String(i, addr_str, sizeof(addr_str))) {
							DebugString("Got address for network card %d\n", i);
							DebugString("Address is %s\n", addr_str);
							if (SendMessage(combobox, 0x4B8, 0, (LPARAM)addr_str) == -1) {
								SendMessage(combobox, 0x4BC, 0, (LPARAM)addr_str);
							}
						}
					}
				}

				if (inited) {
					delete PacketTransport;
					PacketTransport = NULL;
				}

				if (SendMessage(combobox, CB_GETCOUNT, 0, 0) == 0) {
					SendMessage(combobox, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_NO_BOUND_NET_CARD", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x37E));
				}
				int netcard = Options.NetCard;
				if (netcard < 0 || netcard >= SendMessage(combobox, CB_GETCOUNT, 0, 0)) {
					netcard = 0;
				}
				SendMessage(combobox, CB_SETCURSEL, netcard, 0);
				SendMessage(window, 0x464, 0, 0);

				_current_ipx = -1;
				handle = GetDlgItem(window, 0x5D1);
				if (Options.Socket != 0xFFFF) {
					sprintf(socket_string, "%d", Options.Socket);
					SendMessage(handle, 0x4B4, 0, (LPARAM)socket_string);
				}
				SendMessage(handle, EM_LIMITTEXT, 5, 0);
				handle = GetDlgItem(window, 0x5D3);
				if (strlen(Options.DestNet)) {
					SendMessage(handle, 0x4B4, 0, (LPARAM)Options.DestNet);
				}
				SendMessage(handle, EM_LIMITTEXT, 11, 0);
				if (Session.SendDelay) {
					SendDlgItemMessage(window, 0x762, BM_SETCHECK, 1, 0);
				} else {
					SendDlgItemMessage(window, 0x762, BM_SETCHECK, 0, 0);
				}
				handle = GetDlgItem(window, 0x5D7);
				if (handle != (HWND)-1 && Session.PortNumberOverride) {
					sprintf(socket_string, "%d", Session.PortNumberOverride);
					SendMessage(handle, 0x4B4, 0, (LPARAM)socket_string);
				}
				return(0);
			}

			case 0x4DB:
				Focus_Next_Tab_Item((HWND)lparam, wparam != 0);
				return(0);

			case WM_COMMAND: {
				switch (LOWORD(wparam)) {
					case IDOK: {
						handle = GetDlgItem(window, 0x5D1);
						SendMessage(handle, 0x4B5, sizeof(socket_string), (LPARAM)socket_string);
						unsigned short socket;
						if (strlen(socket_string)) {
							socket = atoi(socket_string);
							if (strlen(socket_string) > 5 || socket > 16383) {
								WWMessageBox().Process(StringTable::LoadString("TXT_INVALID_SOCKET", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x3D7), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x3D7), NULL, NULL);
								return(0);
							}
						} else {
							socket = -1;
						}
						handle = GetDlgItem(window, 0x5D3);
						SendMessage(handle, 0x4B5, sizeof(addr_string), (LPARAM)addr_string);
						char dest_net[32];
						strncpy(dest_net, addr_string, sizeof(addr_string));

						if (strlen(addr_string)) {
							int i = 0;
							char *octet = strtok(addr_string, ".");
							int net[4];
							net[0] = -1;
							net[1] = -1;
							net[2] = -1;
							net[3] = -1;
							if (octet != NULL) {
								while (octet) {
									sscanf(octet, "%x", &net[i]);
									i++;
									octet = strtok(NULL, ".");
								}
							}
							if ((unsigned char)net[0] != net[0] || (unsigned char)net[1] != net[1] || (unsigned char)net[2] != net[2] || (unsigned char)net[3] != net[3]) {
								WWMessageBox().Process(StringTable::LoadString("TXT_DESTNET_ADDR_ERROR", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x3F9), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x3F9), NULL, NULL);
								return(0);
							}
						}

						handle = GetDlgItem(window, 0x5D4);
						Options.NetCard = SendMessage(handle, CB_GETCURSEL, 0, 0);
						Options.Socket = socket;
						strcpy(Options.DestNet, dest_net);

						handle = GetDlgItem(window, 0x5D7);
						char port_string[32];
						memset(port_string, 0, sizeof(port_string));
						SendMessage(handle, 0x4B5, 8, (LPARAM)port_string);
						char port_copy[32];
						strncpy(port_copy, port_string, sizeof(port_string));
						int port = atoi(port_copy);
						if (port > 0xFFFF) {
							WWMessageBox().Process(StringTable::LoadString("TXT_PORT_NUMBER_ERROR", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x422), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x422), NULL, NULL);
							return(0);
						}

						if (SendDlgItemMessage(window, 0x762, BM_GETCHECK, 0, 0) == 1) {
							if (!Session.SendDelay) {
								WWMessageBox().Process(StringTable::LoadString("TXT_FIREWALL_SETTINGS_WARNING1", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x42B), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x42B), NULL, NULL);
							}
							Session.SendDelay = true;
						} else {
							Session.SendDelay = false;
						}

						if (port > 0 && Session.PortNumberOverride == 0) {
							WWMessageBox().Process(StringTable::LoadString("TXT_FIREWALL_SETTINGS_WARNING2", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x436), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x436), NULL, NULL);
						}
						Session.PortNumberOverride = port;
						FirewallHelper.Clear();

						RawFileClass file("RA2MD.INI");
						INI_RA2MD.Put_Bool("MultiPlayer", "SendDelay", Session.SendDelay);
						INI_RA2MD.Put_Int("MultiPlayer", "PortNumberOverride", Session.PortNumberOverride, 0);
						INI_RA2MD.Save(file, false);
						break;
					}

					case IDCANCEL:
						break;

					case 0x5D4:
						handle = GetDlgItem(window, 0x5D4);
						_current_ipx = SendMessage(handle, CB_GETCURSEL, 0, 0);
						return(0);

					default:
						return(0);
				}
				*result = 2;
			}
		}
		rc = 0;
	}

	return(rc);
}

#include "filesystem.h"

extern int SidebarWidth;
Rect Get_Title_Screen_Area(void);
void FUN_0072E1B0(int width, int height);
HWND FUN_00775B10(void);
HWND WS_Next_Lower_Dialog(HWND window);

namespace OwnerDraw {
	void __fastcall UpdateControlPosition(HWND dialog, LPARAM lparam);
}

// FUNCTION: GAMEMD 0x00560BF0
bool Change_Display_Mode(int width, int height)
{
	DebugString("About to set video mode\n");

	Point2D oldsize(VideoModeWidth, VideoModeHeight);

	int redleft = DSurface::Get_Red_Left();
	int redright = DSurface::Get_Red_Right();
	int greenleft = DSurface::Get_Green_Left();
	int greenright = DSurface::Get_Green_Right();
	int blueleft = DSurface::Get_Blue_Left();
	int blueright = DSurface::Get_Blue_Right();

	Hide_Mouse();

	Wait_Blit();

	if (WindowedMode == true) {
		VisibleRect = Rect(0, 0, width, height);
		VideoModeWidth = width;
		VideoModeHeight = height;
		DebugString("VisibleRect: %dx%d\n", width, height);
	} else {
		if (MouseCursor != NULL) {
			DebugString("Removing mouse handler\n");
			delete MouseCursor;
			MouseCursor = NULL;
		}

		if (VisibleSurface != NULL) {
			DebugString("Deleting primary surface\n");
			delete VisibleSurface;
			VisibleSurface = NULL;
		}

		if (!Set_Video_Mode(MainWindow, width, height, 16)) {
			DebugString("Set_Video_Mode failed.\n");

			DebugString("Restoring primary surface\n");
			VisibleSurface = DSurface::Create_Primary();

			DebugString("Restoring mouse handler\n");
			MouseCursor = new WWMouseClass((DSurface *)VisibleSurface, MainWindow);

			void const * mouseshp = FileSystem::LoadFile("MOUSE.SHA", false);
			if (mouseshp != NULL) {
				Point2D hotspot = Point2D(0, 0);
				Set_Mouse_Cursor(hotspot, (ShapeSet const *)mouseshp, 0);
			}

			Map.Set_Default_Mouse(MOUSE_NORMAL);
			Map.Override_Mouse_Shape(MOUSE_NORMAL);

			Show_Mouse();

			return(false);
		}

		DebugString("Mode apparently set OK\n");
		VisibleRect = Rect(0, 0, width, height);
		DebugString("VisibleRect: %dx%d\n", width, height);
	}

	if (AlternateSurface != NULL) {
		DebugString("Deleting AlternateSurface\n");
		delete AlternateSurface;
		AlternateSurface = NULL;
	}

	if (HiddenSurface != NULL) {
		DebugString("Deleting HiddenSurface\n");
		delete HiddenSurface;
		HiddenSurface = NULL;
	}

	if (TileSurface != NULL) {
		DebugString("Deleting TileSurface\n");
		delete TileSurface;
		TileSurface = NULL;
	}

	if (SidebarSurface != NULL) {
		DebugString("Deleting SidebarSurface\n");
		delete SidebarSurface;
		SidebarSurface = NULL;
	}

	if (CompositeSurface != NULL) {
		DebugString("Deleting CompositeSurface\n");
		delete CompositeSurface;
		CompositeSurface = NULL;
	}

	if (WindowedMode == false) {
		DebugString("Setting new window size to %dx%d\n", width, height);
		SetWindowPos(MainWindow, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
		DebugString("Creating primary surface\n");
		VisibleSurface = DSurface::Create_Primary();
	} else {
		DebugString("Setting new window size to %dx%d\n", width, height);
		RECT windowrect;
		SetRect(&windowrect, 0, 0, width, height);
		AdjustWindowRectEx(&windowrect, GetWindowLong(MainWindow, GWL_STYLE), GetMenu(MainWindow) != 0, GetWindowLong(MainWindow, GWL_EXSTYLE));
		MoveWindow(MainWindow, 0, 0, windowrect.right - windowrect.left, windowrect.bottom - windowrect.top, TRUE);
	}

	Rect temp = Get_Title_Screen_Area();

	Allocate_Surfaces(VisibleRect, Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, SidebarWidth, VisibleRect.Height), false);
	LogicalSurface = HiddenSurface;

	DebugString("Recalc color remap tables\n");
	ConvertClass::Reinitialize_Hicolor_Tables(redleft, redright, greenleft, greenright, blueleft, blueright);

	if (MouseCursor == NULL) {
		MouseCursor = new WWMouseClass((DSurface *)VisibleSurface, MainWindow);

		void const * mouseshp = FileSystem::LoadFile("MOUSE.SHA", false);
		if (mouseshp != NULL) {
			Point2D hotspot = Point2D(0, 0);
			Set_Mouse_Cursor(hotspot, (ShapeSet const *)mouseshp, 0);
		}

		Map.Set_Default_Mouse(MOUSE_NORMAL);
		Map.Override_Mouse_Shape(MOUSE_NORMAL);

		Show_Mouse();
	} else {
		((WWMouseClass *)MouseCursor)->Calc_Confining_Rect();
	}

	Map.Set_View_Dimensions(temp);
	FUN_0072E1B0(VideoModeWidth, VideoModeHeight);
	Map.Init_IO();
	Map.Activate(1);
	Map.Reposition_Sidebar();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);

	HWND dialog = FUN_00775B10();
	while (dialog != NULL) {
		On_WM_NCDESTROY(dialog);
		OwnerDraw::UpdateControlPosition(dialog, (LPARAM)&oldsize);
		dialog = WS_Next_Lower_Dialog(dialog);
	}

	Show_Mouse();

	DebugString("Mode change complete.\n");

	return(true);
}

BOOL CALLBACK Confirm_Mode_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);

// FUNCTION: GAMEMD 0x00561180
bool Test_Display_Mode_Dialog(int width, int height)
{
	int rc = -1;

	DebugString("Testing display mode @ %dx%d\n", width, height);
	Hide_Mouse();
	HiddenSurface->Fill(0);
	Update_Visible_Surface();

	if (!Change_Display_Mode(width, height)) {
		return(false);
	}

	HiddenSurface->Fill(0);
	Update_Visible_Surface();
	Show_Mouse();
	FUN_0052FEC0(NULL, true);

	HWND dialog = UI::BeginDialog(0xD9, (DLGPROC)Confirm_Mode_Dialog_Proc, 0);
	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		OwnerDraw::Display_Dialog(dialog);

		CDTimerClass<SystemTimerClass> timer = 10 * TIMER_SECOND;
		while (rc < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
			if (timer <= 0) {
				PostMessage(dialog, WM_COMMAND, 0x6CA, 0);
				timer = 5 * TIMER_SECOND;
			}
		}

		UI::EndDialog(dialog);
		if (rc != 1) {
			DebugString("Resetting display mode @ %dx%d\n", Options.ScreenWidth, Options.ScreenHeight);
			Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
			LogicalSurface = HiddenSurface;
			return(false);
		}
	}

	DebugString("Keeping display mode @ %dx%d\n", width, height);
	LogicalSurface = HiddenSurface;
	return(true);
}

// FUNCTION: GAMEMD 0x00561350
BOOL CALLBACK Confirm_Mode_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		int * result = (int *)GetWindowLong(window, DWL_USER);
		switch (message) {
			case WM_COMMAND:
				switch (LOWORD(wparam)) {
					case 0x5C1:
						*result = 1;
						break;

					case 0x6CA:
						*result = 2;
						return(0);
				}
				break;

			case 0x497: {
				wchar_t const * text = StringTable::LoadString("GUI:ConfirmModeChange", NULL, "D:\\ra2mdpost\\MainOpt.cpp", 0x5DD);
				HWND item = GetDlgItem(window, 0x5DB);
				SendMessage(item, 0x4B2, 0, (LPARAM)text);
				return(0);
			}
		}
		return(0);
	}
	return(rc);
}


static __forceinline BOOL Display_Options_Dialog_Body(HWND window, UINT message, WPARAM wparam)
{
	enum {
		MIN_WIDTH = 640,
		MIN_HEIGHT = 480,
		MAX_WIDTH = 1024,
		MAX_HEIGHT = 768,
		MAX_HI_WIDTH = 4096,
		MAX_HI_HEIGHT = 4096,
	};

	static int * _modes = NULL;
	static int _current_mode = -1;
	static int _previous_mode = -1;
	static bool _initialized = true;

	int * result = (int *)GetWindowLong(window, DWL_USER);
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				default:
					return(0);

				case 0x5D9: {
					HWND list = GetDlgItem(window, 0x5D9);
					_current_mode = ListBox_GetCurSel(list);
				}
				return(0);

				case 1:
				case 0x6CB: {
					if (_previous_mode != _current_mode) {
						Center_Window_Within_Window(window, MainWindow);
						HWND list = GetDlgItem(window, 0x5D9);
						if (list) {
							int index = ListBox_GetItemData(list, _current_mode);
							int * modes = &_modes[2 * index];
							TempOptions.ScreenWidth = modes[0];
							TempOptions.ScreenHeight = modes[1];
						}
					}
					HWND button = GetDlgItem(window, 0x674);
					if (button) {
						Options.StretchMovies = Button_GetCheck(button) == BST_CHECKED;
					}
				}
				break;

				case 0x686:
					delete [] _modes;
					_modes = NULL;
					*result = 2;
					return(0);
			}
			delete [] _modes;
			_modes = NULL;
			*result = 1;
			break;

		case 0x497: {
			HWND list = GetDlgItem(window, 0x5D9);
			if (Options.AllowHiResModes) {
				_modes = EnumDisplayModes(MIN_WIDTH, MIN_HEIGHT, MAX_HI_WIDTH, MAX_HI_HEIGHT, 16);
			} else {
				_modes = EnumDisplayModes(MIN_WIDTH, MIN_HEIGHT, MAX_WIDTH, MAX_HEIGHT, 16);
			}
			int * modes = _modes;
			int item_index = 0;
			int initial_mode = -1;
			int mode_index = 0;
			if (modes != NULL) {
				while (*modes != 0) {
					int width = *modes++;
					int height = *modes++;
					if (Options.AllowHiResModes || width == MIN_WIDTH || width == 800 || width == MAX_WIDTH) {
						if (width == TempOptions.ScreenWidth && height == TempOptions.ScreenHeight) {
							initial_mode = mode_index;
						}
						char buffer[64];
						sprintf(buffer, "%d x %d x 16", width, height);
						int index = (int)SendMessage(list, 0x4C8, 0, (LPARAM)buffer);
						ListBox_SetItemData(list, index, item_index);
						mode_index++;
						item_index++;
					} else {
						item_index++;
					}
				}
			}
			ListBox_SetCurSel(list, initial_mode);
			_initialized = true;
			_current_mode = initial_mode;
			_previous_mode = initial_mode;

			HWND button = GetDlgItem(window, 0x674);
			if (button) {
				bool stretch = Options.StretchMovies == true && DSurface::AllowStretchBlits == true;
				Button_SetCheck(button, stretch != false);
				EnableWindow(button, (BOOL)DSurface::AllowStretchBlits);
			}
		}
		break;

	}
	return(0);
}

// FUNCTION: GAMEMD 0x00561400
BOOL CALLBACK Display_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc == 0) {
		return Display_Options_Dialog_Body(window, message, wparam);
	}
	return rc;
}
