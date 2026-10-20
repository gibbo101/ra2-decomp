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
#include "theme.h"
#include "stimer.h"
#include "surface.h"
#include "wspipx.h"
#include "wwmous.h"

#include "color.hh"

#undef LoadString
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
extern const double DAT_007ED3B0;
extern const double DAT_007ED3B8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0055F780 $E104
double DAT_00ABCE40 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x0055F7B0 $E107
double DAT_00ABCF48 = DAT_00ABCE40 / DAT_007ED3B0;

// FUNCTION: 0x0055F7D0 $E110
double DAT_00ABCF30 = DAT_007E1718 / DAT_00ABCF48;

// FUNCTION: 0x0055F7F0 $E113
double DAT_00ABCF28 = DAT_007E1720 * DAT_007ED3B8;

// FUNCTION: 0x0055F810 $E116
double DAT_00ABCE48 = DAT_007E1728 * DAT_007ED3B8;

// FUNCTION: 0x0055F830 $E119
double DAT_00ABCE20 = DAT_007E1730 * DAT_007ED3B8;

// FUNCTION: 0x0055F850 $E122
int DAT_00ABCF50 = (int)(FUN_004CAD50(DAT_00ABCE20 - DAT_00ABCE48) * DAT_007E1738 * DAT_00ABCE40);

// FUNCTION: 0x0055F890 $E125
double DAT_00ABCE50 = FUN_004CADE0(DAT_00ABCF50 * DAT_007E1740);

// FUNCTION: 0x0055F8C0 $E128
double DAT_00ABCE68 = FUN_004CADE0((DAT_00ABCF50 * 2) / DAT_00ABCE40);

// FUNCTION: 0x0055F8F0 $E131
int DAT_00ABCF44 = (int)(DAT_00ABCF50 * 4 + DAT_007E1738);

// FUNCTION: 0x0055F920 $E134
ShortPair DAT_00ABCE38;

// FUNCTION: 0x0055F930 $E137
IntTriple DAT_00ABCE58;

struct ModeSize {
	int Width, Height, Flag;
	ModeSize() { Flag = 0; Width = 0x80; Height = 0x80; }
};

// FUNCTION: 0x0055F950 $E140
ModeSize DAT_00ABCF38;

// FUNCTION: 0x0055F970 $E143
GameOptionsClass TempOptions;

// FUNCTION: 0x0055F980
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

// FUNCTION: 0x0055FA30
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

// FUNCTION: 0x0055FAA0
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

// FUNCTION: 0x0055FC80
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

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};

extern int DAT_007F5BF8;
Rect FUN_0072AD20(void);
void FUN_0072E1B0(int width, int height);
HWND FUN_00775B10(void);
HWND WS_Next_Lower_Dialog(HWND window);

namespace OwnerDraw {
	void __fastcall UpdateControlPosition(HWND dialog, LPARAM lparam);
}

// FUNCTION: 0x00560BF0
bool Change_Display_Mode(int width, int height)
{
	DebugString("About to set video mode\n");

	Point2D oldsize(Drawing::RenderWidth, Drawing::RenderHeight);

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
		Drawing::RenderWidth = width;
		Drawing::RenderHeight = height;
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

	Rect temp = FUN_0072AD20();

	Allocate_Surfaces(VisibleRect, Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, DAT_007F5BF8, VisibleRect.Height), false);
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
		((WWMouseClass *)MouseCursor)->FUN_007B8960();
	}

	Map.Set_View_Dimensions(temp);
	FUN_0072E1B0(Drawing::RenderWidth, Drawing::RenderHeight);
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

// FUNCTION: 0x00561180
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

// FUNCTION: 0x00561350
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
