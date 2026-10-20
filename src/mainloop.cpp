// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mainloop.h"
#include "_bench.h"
#include "_command.h"
#include "_logic.h"
#include "_map.h"
#include "_palette.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_timer.h"
#include "_xmouse.h"
#include "bench.h"
#include "command.h"
#include "conquer.h"
#include "data.h"
#include "debug.h"
#include "dialog.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "fog.h"
#include "globals.h"
#include "goptions.h"
#include "internet.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "logic.h"
#include "misc.h"
#include "mpscore.h"
#include "msgbox.h"
#include "msgloop.h"
#include "mstimer.h"
#include "netdlg.h"
#include "nullmgr.h"
#include "pcx.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "stats.h"
#include "stimer.h"
#include "surface.h"
#include "tactical.h"
#include "theme.h"
#include "timer.h"
#include "tracker.h"
#include "wonline.h"
#include "multiplayer.h"
#include "wsproto.h"
#include "bench.hh"
#include "special.hh"

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
extern const double DAT_007ED3A0;
extern const double DAT_007ED3A8;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0055CA00 $E106
double DAT_00ABCD80 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0055CA30 $E109
double DAT_00ABCDF0 = DAT_00ABCD80 / DAT_007ED3A0;

// FUNCTION: 0x0055CA50 $E112
double DAT_00ABCDD0 = DAT_007E1718 / DAT_00ABCDF0;

// FUNCTION: 0x0055CA70 $E115
double DAT_00ABCDC8 = DAT_007E1720 * DAT_007ED3A8;

// FUNCTION: 0x0055CA90 $E118
double DAT_00ABCD98 = DAT_007E1728 * DAT_007ED3A8;

// FUNCTION: 0x0055CAB0 $E121
double DAT_00ABCD60 = DAT_007E1730 * DAT_007ED3A8;

// FUNCTION: 0x0055CAD0 $E124
int DAT_00ABCDF8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCD60 - DAT_00ABCD98) * DAT_00ABCD80));

// FUNCTION: 0x0055CB10 $E127
double DAT_00ABCDA0 = FUN_004CADE0(DAT_00ABCDF8 * DAT_007E1740);

// FUNCTION: 0x0055CB40 $E130
double DAT_00ABCDC0 = FUN_004CADE0((DAT_00ABCDF8 * 2) / DAT_00ABCD80);

// FUNCTION: 0x0055CB70 $E133
int DAT_00ABCDEC = (int)(DAT_00ABCDF8 * 4 + DAT_007E1738);

// FUNCTION: 0x0055CBA0 $E136
ShortPair DAT_00ABCD78;

// FUNCTION: 0x0055CBB0 $E139
IntTriple DAT_00ABCDB0;

extern int DAT_00ABCDE0, DAT_00ABCDE4, DAT_00ABCDE8;
extern bool DAT_00ABCD58;

// FUNCTION: 0x0055CBD0
void FUN_0055CBD0(void)
{
	DAT_00ABCDE8 = 0;
	DAT_00ABCDE0 = 128;
	DAT_00ABCDE4 = 128;
}

// FUNCTION: 0x0055CBF0
bool FUN_0055CBF0(void)
{
	return DAT_00ABCD58;
}

// FUNCTION: 0x0055CC00
void FUN_0055CC00(void)
{
	DAT_00ABCD58 = true;
}

// FUNCTION: 0x0055CC10
void FUN_0055CC10(void)
{
	DAT_00ABCD58 = false;
}

// FUNCTION: 0x0055CC20
void Motion_Capture(void)
{
	if (Debug_MotionCapture) {
		static Surface ** _array = NULL;
		static int _sequence = 0;
		static int _seqsize = Rule->MovieTime * TICKS_PER_MINUTE;

		if (_array == NULL) {
			_array = new Surface * [_seqsize];
			memset(_array, '\0', _seqsize * sizeof(Surface*));
		}

		if (_array == NULL) {
			Debug_MotionCapture = false;
		}

		RECT client_rect;

		GetClientRect(MainWindow, &client_rect);

		POINT screen_pt;

		screen_pt.x = client_rect.left;
		screen_pt.y = client_rect.top;
		if (!ClientToScreen(MainWindow, &screen_pt)) {
			return;
		}

		POINT screen_pt2;
		screen_pt2.x = client_rect.right;
		screen_pt2.y = client_rect.bottom;
		if (!ClientToScreen(MainWindow, &screen_pt2)) {
			return;
		}

		Rect rect(screen_pt.x, screen_pt.y, client_rect.right, client_rect.bottom);

		if (Map.ScreenX != 0 || Map.ScreenY != 0) {
			if (Map.ScreenX > 0) {
				rect.X += Map.ScreenX;
				rect.Width -= Map.ScreenX;
			} else if (Map.ScreenX < 0) {
				rect.Width += Map.ScreenX;
			}
			if (Map.ScreenY > 0) {
				rect.Y += Map.ScreenY;
				rect.Height -= Map.ScreenY;
			} else if (Map.ScreenY < 0) {
				rect.Height += Map.ScreenY;
			}
		}

		if (_sequence < _seqsize) {
			if (_array[_sequence] == NULL) {
				_array[_sequence] = new BSurface(rect.Width, rect.Height, 2);
			}

			if (_array[_sequence] != NULL) {
				_array[_sequence]->Blit_From(Rect(0, 0, rect.Width, rect.Height), *VisibleSurface, rect);
			}
			_sequence++;

		} else {
			Debug_MotionCapture = false;

			DSurface temp_page(rect.Width, rect.Height);

			for (int index = 0; index < _sequence; index++) {
				char filename[30];
				sprintf(filename, "cap%04d.pcx", index);
				CCFileClass file(filename);
				temp_page.Blit_From(*_array[index], false, true);
				Write_PCX_File(file, temp_page, & GamePalette);
			}

			_sequence = 0;
		}
	}
}

struct Unknown_00B052D0 {
	void FUN_00664530(void);
	bool FUN_00664610(int, int);
};
extern Unknown_00B052D0 DAT_00B052D0;
extern int DAT_00A8EA0C;
extern WinsockInterfaceClass * DAT_00887628;
extern void __cdecl FUN_004068E0(char const * format, ...);
void FUN_006523A0(void);

// FUNCTION: 0x0055CF10
void FUN_0055CF10(void)
{
	FUN_004068E0("Disconnect_Gracefully\n");
	if (Session.Type == GAME_INTERNET && DAT_00887628 && Session.ViaPacketRouter) {
		if (DAT_00A8EA0C) {
			DAT_00B052D0.FUN_00664530();
			CDTimerClass<SystemTimerClass> timer(480);
			while (timer()) {
				Call_Back();
				if (DAT_00B052D0.FUN_00664610(10, 0)) {
					break;
				}
			}
		}
		if (Session.MPGameMode == NULL || !Session.MPGameMode->vt_entry_08()) {
			FUN_006523A0();
		}
	}
}

extern bool UserInputLocked;
extern bool GameInFocus;
extern unsigned DAT_00ABCE14;
extern int DAT_00ABCD40;
namespace FPSCounter { extern unsigned CurrentFrameRate; extern unsigned TotalFramesElapsed; extern unsigned TotalTimeElapsed; }

class InputManagerClass : public KeyboardClass {
	public:
		static InputManagerClass * Instance;
};

static inline const CommandClass * Hotkey_Command(KeyNumType key)
{
	return HotkeyCommands[key];
}

// FUNCTION: 0x0055DEE0
void Keyboard_Process(KeyNumType & input)
{
	Message_Input(input);
	if (input == KN_NONE) {
		return;
	}

	KeyNumType key = KeyNumType(input & ~KN_RLSE_BIT);
	KeyNumType plain = KeyNumType(key & ~(WWKEY_SHIFT_BIT|WWKEY_ALT_BIT|WWKEY_CTRL_BIT|WWKEY_VK_BIT));

	const CommandClass * cmd = HotkeyCommands[plain];
	if (cmd == NULL) {
		cmd = HotkeyCommands[key];
	} else if (!cmd->Unknown14(key)) {
		cmd = Hotkey_Command(key);
	}

	if (cmd != NULL && (!UserInputLocked || wcsicmp(cmd->Get_Category(), L"Debug") == 0)) {
		if (cmd->Unknown18(input)) {
			cmd->Execute(input);
		}
		if (cmd->Unknown1C(input)) {
			int count = 0;
			int next = InputManagerClass::Instance->Check() & 0xFFFF;
			if (next == input) {
				do {
					InputManagerClass::Instance->Get();
					next = InputManagerClass::Instance->Check() & 0xFFFF;
					count++;
				} while (count < 10 && next == input);
			}
		}
		return;
	}

	if (plain == KN_ESC) {
		Queue_Options();
	} else if (plain == KN_TAB) {
		Map.Zoom_Mode_Control();
	} else if (plain == KN_LEFT) {
		if (input & KN_RLSE_BIT) {
			DAT_00ABCE14 &= ~0x100;
		} else {
			DAT_00ABCE14 |= 0x100;
		}
	} else if (plain == KN_RIGHT) {
		if (input & KN_RLSE_BIT) {
			DAT_00ABCE14 &= ~0x1000;
		} else {
			DAT_00ABCE14 |= 0x1000;
		}
	} else if (plain == KN_UP) {
		if (input & KN_RLSE_BIT) {
			DAT_00ABCE14 &= ~1;
		} else {
			DAT_00ABCE14 |= 1;
		}
	} else if (plain == KN_DOWN) {
		if (input & KN_RLSE_BIT) {
			DAT_00ABCE14 &= ~0x10;
		} else {
			DAT_00ABCE14 |= 0x10;
		}
	}
}

// FUNCTION: 0x0055E160
void Sync_Delay(void)
{
	SpareTicks += FrameTimer;

	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		while (NetFrameTimer) {
			Call_Back();
			if (SpecialDialog == SDLG_NONE && GameInFocus == true) {
				KeyNumType input = KN_NONE;
				int x, y;
				if (NetFrameTimer > 10) {
					Map.Input(input, x, y);
					Keyboard_Process(input);
					TacticalMap->AI();
					Map.Render();
				} else {
					Sleep(0);
				}
				if (!NetFrameTimer()) {
					break;
				}
			}
			Sleep(0);
		}
	} else {
		if (FrameTimer) {
			Wait_Blit();
			while (FrameTimer) {
				Sleep(FrameTimer);
			}
		}
	}

	static CDTimerClass<SystemTimerClass> fps_timer;
	if (!fps_timer) {
		FPSCounter::CurrentFrameRate = DAT_00ABCD40;
		DAT_00ABCD40 = 0;
		FPSCounter::TotalFramesElapsed += FPSCounter::CurrentFrameRate;
		FPSCounter::TotalTimeElapsed++;
		if (FPSCounter::TotalFramesElapsed > 0x7FFFFFFF) {
			FPSCounter::TotalFramesElapsed = 0;
			FPSCounter::TotalTimeElapsed = 0;
		}
		fps_timer = TIMER_SECOND;
	}
}

// FUNCTION: 0x0055E410 $E144

extern unsigned DAT_008373A0;
extern int DAT_00ABCE04;

static inline Point2D * Scratch(Point2D const & p) { return (Point2D *)&p; }

// FUNCTION: 0x0055F1E0
void Multiplayer_Debug_Print(void)
{
	wchar_t buffer[256];

	if (DAT_008373A0 != -1) {
		double seconds = (double)DAT_008373A0 * 0.001;
		Fancy_Text_Print_Wide(Scratch(Point2D()), L"%.2f", DSurface::Composite, &Rect(0, 0, 500, 15), &Point2D(100, 0), ColorSchemes[GadgetClass::Get_Color_Scheme()], 0x1F, TextPrintType(6), seconds, Scen->ScenarioName);
	}

	if (Session.DrawMPDebugStats && Session.Type != GAME_NORMAL && Session.Type != GAME_MODEM && Session.Type != GAME_NULL_MODEM) {
		Surface * surface = DSurface::Composite;
		MouseCursor->Hide_Mouse();
		surface->Fill_Rect(Rect(0, 300, 639, 230), 0);

		swprintf(buffer, L"Frame : %d", Frame);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 302), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"FPS : %d", FPSCounter::CurrentFrameRate);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 312), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"MaxAhead : %d", Session.MaxAhead);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 322), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"Resp Time : %d ms", (int)(Ipx.Response_Time() * 1000) / TIMER_SECOND);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 332), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"Req fps : %d", Session.DesiredFrameRate);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 342), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"Process : %d", Session.Players[0]->Player.ProcessTime);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 352), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"Rout resp: %d", (unsigned)(Session.unknown_309C * 1000) / TIMER_SECOND);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 362), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"NFT : %d", NetFrameTimer);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 372), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		swprintf(buffer, L"LNFT : %d", DAT_00ABCE04);
		Fancy_Text_Print_Wide(Scratch(Point2D()), buffer, surface, &surface->Get_Rect(), &Point2D(0, 382), Fetch_Scheme_By_Name("Grey"), 0, TextPrintType(0x19));

		Ipx.Multiplayer_Debug_Print();
		MouseCursor->Show_Mouse();
	}
}

// FUNCTION: 0x0055F6C0 ?$IndexClass@HPBVCommandClass@@::Is_Archive_Same
template class IndexClass<int, CommandClass const *>;
