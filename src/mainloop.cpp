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
#include "cctooltip.h"
#include "_tooltip.h"
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

// SYNTHETIC: GAMEMD 0x0055C9E0 SYMBOL
// _$E2

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

// SYNTHETIC: GAMEMD 0x0055CA00 SYMBOL
// _$E107
double DAT_00ABCD80 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0055CA30 SYMBOL
// _$E110
double DAT_00ABCDF0 = DAT_00ABCD80 / DAT_007ED3A0;

// SYNTHETIC: GAMEMD 0x0055CA50 SYMBOL
// _$E113
double DAT_00ABCDD0 = DAT_007E1718 / DAT_00ABCDF0;

// SYNTHETIC: GAMEMD 0x0055CA70 SYMBOL
// _$E116
double DAT_00ABCDC8 = DAT_007E1720 * DAT_007ED3A8;

// SYNTHETIC: GAMEMD 0x0055CA90 SYMBOL
// _$E119
double DAT_00ABCD98 = DAT_007E1728 * DAT_007ED3A8;

// SYNTHETIC: GAMEMD 0x0055CAB0 SYMBOL
// _$E122
double DAT_00ABCD60 = DAT_007E1730 * DAT_007ED3A8;

// SYNTHETIC: GAMEMD 0x0055CAD0 SYMBOL
// _$E125
int DAT_00ABCDF8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABCD60 - DAT_00ABCD98) * DAT_00ABCD80));

// SYNTHETIC: GAMEMD 0x0055CB10 SYMBOL
// _$E128
double DAT_00ABCDA0 = FUN_004CADE0(DAT_00ABCDF8 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0055CB40 SYMBOL
// _$E131
double DAT_00ABCDC0 = FUN_004CADE0((DAT_00ABCDF8 * 2) / DAT_00ABCD80);

// SYNTHETIC: GAMEMD 0x0055CB70 SYMBOL
// _$E134
int DAT_00ABCDEC = (int)(DAT_00ABCDF8 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0055CBA0 SYMBOL
// _$E137
ShortPair DAT_00ABCD78;

// SYNTHETIC: GAMEMD 0x0055CBB0 SYMBOL
// _$E140
IntTriple DAT_00ABCDB0;

extern int DAT_00ABCDE0, DAT_00ABCDE4, DAT_00ABCDE8;
extern bool DAT_00ABCD58;

// FUNCTION: GAMEMD 0x0055CBD0
void FUN_0055CBD0(void)
{
	DAT_00ABCDE8 = 0;
	DAT_00ABCDE0 = 128;
	DAT_00ABCDE4 = 128;
}

// FUNCTION: GAMEMD 0x0055CBF0
bool FUN_0055CBF0(void)
{
	return DAT_00ABCD58;
}

// FUNCTION: GAMEMD 0x0055CC00
void FUN_0055CC00(void)
{
	DAT_00ABCD58 = true;
}

// FUNCTION: GAMEMD 0x0055CC10
void FUN_0055CC10(void)
{
	DAT_00ABCD58 = false;
}

// FUNCTION: GAMEMD 0x0055CC20
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

#include "ownertalk.h"
extern int DAT_00A8EA0C;
extern WinsockInterfaceClass * DAT_00887628;
extern void __cdecl FUN_004068E0(char const * format, ...);
void FUN_006523A0(void);

// FUNCTION: GAMEMD 0x0055CF10
void Disconnect_Gracefully(void)
{
	FUN_004068E0("Disconnect_Gracefully\n");
	if (Session.Type == GAME_INTERNET && DAT_00887628 && Session.ViaPacketRouter) {
		if (DAT_00A8EA0C) {
			OwnerTalk.Send_Shutdown();
			CDTimerClass<SystemTimerClass> timer(480);
			while (timer()) {
				Call_Back();
				if (OwnerTalk.Wait_For_Router_Shutdown(10, false)) {
					break;
				}
			}
		}
		if (Session.MPGameMode == NULL || !Session.MPGameMode->vt_entry_08()) {
			FUN_006523A0();
		}
	}
}

extern unsigned char DAT_00A8F900;
void __fastcall FUN_0072DFB0(void);
void __fastcall Send_Statistics_Packet(bool);
namespace CopyProtection { bool CheckProtectedData(void); }

static inline void Report_End_Game_Statistics(void)
{
	if (Session.Type == GAME_INTERNET && !DAT_00A8F900) {
		if (WestwoodOnline_Tournament) {
			Session.SawGameCompletion = true;
		}
		Register_Game_End_Time();
		Send_Statistics_Packet(false);
	}
}

// FUNCTION: GAMEMD 0x0055CFD0
bool __fastcall FUN_0055CFD0(void)
{
	if ((Frame & 1023) == 0 && !CopyProtection::CheckProtectedData()) {
		OutList.Add(EventClass(PlayerPtr->HeapID, EventClass::DESTRUCT));
	}

	if (PlayerWins || PlayerLoses || PlayerRestarts || PlayerAborts) {
		++Session.Suspended;
		Lock_Scenario_Input();
		if (PlayerWins || PlayerLoses) {
			if (ToolTips != NULL) {
				ToolTips->Activate(false);
			}

			if (PlayerWins || PlayerLoses) {
				if (Session.MPGameMode != NULL && (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET)) {
					Session.MPGameMode->vt_entry_A0(PlayerWins);
				}

				if (PlayerWins) {
					Report_End_Game_Statistics();
					PlayerLoses = false;
					PlayerWins = false;
					PlayerRestarts = false;
					PlayerAborts = false;
					FUN_0072DFB0();
					Session.Pause();
					Do_Win();
					Session.Resume();
					DAT_00ABCD58 = false;
					if (ToolTips != NULL) {
						ToolTips->Activate(Options.ToolTips);
					}
					goto done;
				} else if (PlayerLoses) {
					Report_End_Game_Statistics();
					PlayerWins = false;
					PlayerLoses = false;
					PlayerRestarts = false;
					PlayerAborts = false;
					FUN_0072DFB0();
					Session.Pause();
					Do_Lose();
					Session.Resume();
					DAT_00ABCD58 = false;
					if (ToolTips != NULL) {
						ToolTips->Activate(Options.ToolTips);
					}
					goto done;
				}
			}
		}

		if (PlayerRestarts) {
			PlayerWins = false;
			PlayerLoses = false;
			PlayerRestarts = false;
			PlayerAborts = false;
			Session.Pause();
			Do_Restart();
			Session.Resume();
			DAT_00ABCD58 = false;
		} else if (PlayerAborts) {
			PlayerWins = false;
			PlayerLoses = false;
			PlayerRestarts = false;
			PlayerAborts = false;
			FUN_0072DFB0();
			Session.Pause();
			Do_Abort();
			Session.Resume();
			DAT_00ABCD58 = false;
			Disconnect_Gracefully();
		}
	done:
		--Session.Suspended;
	}
	return(!GameActive);
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

#include "saveload.h"
#include "cell.h"
#include "object.h"
#include "target.h"
#include "_keyboar.h"
extern int DAT_00ABCE04;
extern bool DAT_00A8EDDC;
extern int DAT_0082A030;
extern float DAT_0082A034;
extern int DAT_00B07784;
extern int TeamEvent;
extern int TeamNumber;
extern bool Debug_Trap_Check_Heap;
void Release_Scenario(void);
int FUN_00407000(void);
void FUN_0053B560(void);
void FUN_00637550(void);
void FUN_00637270(void);
void FUN_00725C70(void);

static inline void Do_Record_Playback()
{
	int count;
	int tgt;
	int i;
	Point2D coord;
	ObjectClass *obj;
	unsigned int sum;
	unsigned int sum2;
	unsigned int ltgt;

	if (Session.Record) {
		coord = TacticalMap->Get_Tactical_Position();
		Session.RecordFile.Write(&coord, sizeof(coord));

		count = CurrentObject.Count();
		Session.RecordFile.Write(&count, sizeof(count));

		sum = 0;
		for (i = 0; i < count; i++) {
			ltgt = TargetClass(CurrentObject[i]).Encode();
			sum += ltgt;
		}
		Session.RecordFile.Write(&sum, sizeof(sum));

		for (i = 0; i < count; i++) {
			tgt = TargetClass(CurrentObject[i]).Encode();
			Session.RecordFile.Write(&tgt, sizeof(tgt));
		}

		Session.RecordFile.Write(&TeamEvent, sizeof(TeamEvent));
		Session.RecordFile.Write(&TeamNumber, sizeof(TeamNumber));
		TeamEvent = 0;
		TeamNumber = 0;
	}

	if (Session.Play) {
		if (Session.RecordFile.Read(&coord, sizeof(coord)) == sizeof(coord)) {
			TacticalMap->Set_Tactical_Position(coord);
		}

		if (Session.RecordFile.Read(&count, sizeof(count)) == sizeof(count)) {
			sum = 0;
			for (i = 0; i < CurrentObject.Count(); i++) {
				ltgt = TargetClass(CurrentObject[i]).Encode();
				sum += ltgt;
			}

			Session.RecordFile.Read(&sum2, sizeof(sum2));
			if (sum2 != sum) {
				Unselect_All();
			}

			AllowVoice = true;

			for (i = 0; i < count; i++) {
				if (Session.RecordFile.Read(&tgt, sizeof(tgt)) == sizeof(tgt)) {
					TargetClass tmp;
					tmp.Decode(tgt);
					obj = tmp.As_Object();
					if (obj && (sum2 != sum)) {
						obj->Select();
						AllowVoice = false;
					}
				}
			}

			AllowVoice = true;
		}

		Session.RecordFile.Read(&TeamEvent, sizeof(TeamEvent));
		Session.RecordFile.Read(&TeamNumber, sizeof(TeamNumber));

		Map.Flag_To_Redraw();
		Map.Render();
	}
}

// FUNCTION: GAMEMD 0x0055D360
bool Main_Loop(void)
{
	KeyNumType input;
	int y;
	int x;

	if (!GameActive) {
		return !GameActive;
	}

	DAT_00ABCD58 = true;

	while (!GameInFocus) {
		if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
			Sleep(500);
			Windows_Message_Handler();
		} else {
			Sleep(10);
			Windows_Message_Handler();
			break;
		}
	}

	if (Frame >= Session.TrapFrame) {
		Session.Trap_Object();
	}

	Session.ProcessTimer = timeGetTime();

	if (Session.TrapCheckHeap) {
		Debug_Trap_Check_Heap = true;
	}

	if (TacticalActive == true && FUN_00407000()) {
		ThemeType theme = Theme.What_Is_Playing();
		if (Options.InGameMusic) {
			if (theme == THEME_NONE) {
				Theme.Queue_Song(THEME_PICK_ANOTHER);
			}
		} else if (theme != THEME_QUIET) {
			Theme.Stop(true);
			Theme.Queue_Song(THEME_QUIET);
		}
	}

	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH && Session.CommProtocol == 2) {
		if (Session.Play) {
			FrameTimer = 0;
		} else {
			if (Session.DesiredFrameRate == 0) {
				FrameTimer = 2;
				NetFrameTimer = 33;
			} else {
				FrameTimer = 60 / Session.DesiredFrameRate;
				NetFrameTimer = 1000 / Session.DesiredFrameRate;
			}
			DAT_00ABCE04 = NetFrameTimer;

			int maxahead = Session.MaxAhead;
			int worst_latency = 0;
			if (Session.Type == GAME_INTERNET) {
				if (maxahead > 30) {
					for (int i = 0; i < Ipx.Num_Connections(); i++) {
						if (worst_latency <= Session.PlayerLatency[i]) {
							worst_latency = Session.PlayerLatency[i];
						}
					}
					if (worst_latency) {
						if (worst_latency >= maxahead / 4) {
							NetFrameTimer = NetFrameTimer + 10;
						}
						if (worst_latency >= maxahead / 2) {
							NetFrameTimer = NetFrameTimer + 10;
						}
						if (worst_latency >= (3 * maxahead) / 4) {
							NetFrameTimer = NetFrameTimer + 10;
						}
					}
				}
			}
		}
	} else if (Session.Type == GAME_NORMAL && !DAT_00A8EDDC) {
		Options.GameSpeed = 2;
		FrameTimer = 2;
	} else {
		FrameTimer = Options.GameSpeed;
	}

	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		if (Scen->IsGamePaused && Scen->PauseTimer.Value() <= 0) {
			Scen->IsGamePaused = false;
			Release_Scenario();
		}
		if (Scen->unknown_62C) {
			Windows_Message_Handler();
			Call_Back();
			FUN_0053B560();
			TacticalMap->AI();
			Map.Render();
			Sync_Delay();
			DAT_00ABCD58 = false;
			return !GameActive;
		}
	}

	if (!Session.Play) {
		if (SpecialDialog == 0 && GameInFocus) {
			Map.Input(input, x, y);
			Keyboard_Process(input);
			if (Session.EnableMultiplayerDebug) {
				Multiplayer_Debug_Print(false);
			}
			if ((Frame & 7) == 7 && Session.Type == GAME_INTERNET) {
				Ipx.Store_Stats();
			}
			Update_Fogged_Objects();
			Map.Render();
		}
	}

	drag_select_aborted = false;

	if (Session.Record || Session.Play) {
		Do_Record_Playback();
	}

	DisplayClass::Layer[2].Sort();

	if (Session.Type == GAME_NORMAL && !ScenarioClass::WasGameSaved) {
		wchar_t description[56];
		char name[261];
		ScenarioClass::WasGameSaved = true;
		strcpy(name, Scen->ScenarioName);
		name[260] = '\0';
		char *dot = strrchr(name, '.');
		if (dot != 0) {
			*dot = '\0';
		}
		strcat(name, ".sav");
		swprintf(description, L"[%s]", Scen->UINameLoaded);
		Save_Game(name, description, 0);
	}

	Logic.AI();

	int scroll = DAT_0082A030;
	if (InputManagerClass::Instance->Down(Options.KeySelect1) || InputManagerClass::Instance->Down(Options.KeySelect2)) {
		scroll = scroll * DAT_0082A034;
	} else if (InputManagerClass::Instance->Down(Options.KeyForceAttack1) || InputManagerClass::Instance->Down(Options.KeyForceAttack2)) {
		scroll = Map.PlayRect.Width > Map.PlayRect.Height ? Map.PlayRect.Width : Map.PlayRect.Height;
		scroll <<= 8;
	}
	if (DAT_00ABCE14 & 0x100) {
		Map.Scroll_Map(FACING_W, scroll, true);
	}
	if (DAT_00ABCE14 & 0x1000) {
		Map.Scroll_Map(FACING_E, scroll, true);
	}
	if (DAT_00ABCE14 & 0x01) {
		Map.Scroll_Map(FACING_N, scroll, true);
	}
	if (DAT_00ABCE14 & 0x10) {
		Map.Scroll_Map(FACING_S, scroll, true);
	}

	FUN_00637550();
	Session.Messages.Manage();

	if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET)
		&& Session.MPGameMode != NULL && !Session.MPGameMode->vt_entry_04()) {
		CellClass *const &cell = Map.Array[(Map.PlayRect.Width + 5) * MAP_CELL_W + 7];
		if (cell && (cell->ShroudCounter != 1 || cell->IsMapped || cell->IsVisible)) {
			Scen->RandomNumber(0, 2);
		}
	}

	Session.ProcessTicks += min<int>(1000, timeGetTime() - Session.ProcessTimer);
	Session.ProcessFrames++;

	Queue_AI();
	FUN_00637550();
	Call_Back();

	if (!PlayerWins && !PlayerLoses && !PlayerRestarts && !PlayerAborts) {
		Frame++;
		if (DAT_00B07784 != 0 && DAT_00B07784 < Frame) {
			Unlock_Scenario_Input();
			DAT_00B07784 = 0;
		}
		Sync_Delay();
		FUN_00725C70();
		FUN_00637270();
		DAT_00ABCD58 = false;
		return !GameActive;
	}

	DAT_00ABCD58 = false;
	return true;
}

// FUNCTION: GAMEMD 0x0055DEE0
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
	} else if (!cmd->PreventCombinationOverride(key)) {
		cmd = Hotkey_Command(key);
	}

	if (cmd != NULL && (!UserInputLocked || wcsicmp(cmd->Get_Category(), L"Debug") == 0)) {
		if (cmd->ExtraTriggerCondition(input)) {
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

// FUNCTION: GAMEMD 0x0055E160
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

// SYNTHETIC: GAMEMD 0x0055E410 SYMBOL
// _$E145

extern unsigned DAT_008373A0;

static inline Point2D * Scratch(Point2D const & p) { return (Point2D *)&p; }

// FUNCTION: GAMEMD 0x0055F1E0
void Multiplayer_Debug_Print(bool)
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

// FUNCTION: GAMEMD 0x0055F690
// xTargetClass::Decode

// TEMPLATE: GAMEMD 0x0055F6C0
// ?Is_Archive_Same@?$IndexClass@HPBVCommandClass@@@@ABE_NABH@Z
template class IndexClass<int, CommandClass const *>;

// TEMPLATE: GAMEMD 0x0055F6E0
// ?Search_For_Node@?$IndexClass@HPBVCommandClass@@@@ABEPBUNodeElement@1@ABH@Z
