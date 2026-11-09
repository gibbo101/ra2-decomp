// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "session.h"

// SYNTHETIC: GAMEMD 0x0077B090 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x0077B0C0 SYMBOL
// _$E5

// SYNTHETIC: GAMEMD 0x0077B0E0 SYMBOL
// _$E8

// SYNTHETIC: GAMEMD 0x0077B100 SYMBOL
// _$E11

// SYNTHETIC: GAMEMD 0x0077B120 SYMBOL
// _$E14

// SYNTHETIC: GAMEMD 0x0077B140 SYMBOL
// _$E17

// SYNTHETIC: GAMEMD 0x0077B160 SYMBOL
// _$E20

// SYNTHETIC: GAMEMD 0x0077B1A0 SYMBOL
// _$E23

// SYNTHETIC: GAMEMD 0x0077B1D0 SYMBOL
// _$E26

// SYNTHETIC: GAMEMD 0x0077B200 SYMBOL
// _$E29

// SYNTHETIC: GAMEMD 0x0077B230 SYMBOL
// _$E32

// SYNTHETIC: GAMEMD 0x0077B240 SYMBOL
// _$E35

// SYNTHETIC: GAMEMD 0x0077B260 SYMBOL
// _$E38

// FUNCTION: GAMEMD 0x0077B280
int __fastcall FUN_0077B280(void)
{
	int state = Session.unknown_0C;
	if (state == 0 || state == 6) {
		return 0;
	}
	return (state == 3) + 1;
}

#include "theme.h"
#include "wolopt.h"
#include "wstring.h"
extern Wstring DAT_00B73E40;
extern int DAT_00B73678;
Wstring FUN_00779B50(void);
Wstring FUN_00779DD0(void);
void FUN_00799CD0(void);
void FUN_00799D10(void);
int FUN_00799D30(void);
void FUN_0077DED0(void);
void FUN_0077E110(void);
int FUN_00787010(void);
void FUN_00785EA0(void);
void FUN_00786080(void);
int FUN_00798DE0(void);
int FUN_0079FC40(void);
int FUN_0079F140(bool first_time);
int FUN_007AFB90(bool first_time);
void FUN_007A2970(void);
int FUN_00787770(HWND window, bool first_time);

// FUNCTION: GAMEMD 0x0077B2A0
int __fastcall FUN_0077B2A0(bool first_time)
{
	while (MouseCursor->Get_Mouse_State() < 0) {
		MouseCursor->Show_Mouse();
	}
	FUN_00799CD0();
	ThemeType theme = Theme.What_Is_Playing();
	if (theme == Theme.From_Name("INTRO")) {
		Theme.Queue_Song(THEME_QUIET);
	}
	FUN_0077DED0();
	bool shuffle = Theme.Is_Shuffle();
	Theme.Set_Shuffle(true);
	FUN_0077E110();
	if (DAT_00B73E40.length() == 0) {
		DAT_00B73E40 = FUN_00779B50();
		if (DAT_00B73E40.length() == 0) {
			WOLOptionsClass options;
			if (options.Is_Bit1_Clear()) {
				DAT_00B73E40 = FUN_00779DD0();
			}
		}
	}
	int result = 0;
	bool done = false;
	for (;;) {
		int kind = FUN_0077B280();
		if (DAT_00B73678 == 0 && kind != 0) {
			result = FUN_00787010();
			switch (result) {
			case 0:
				if (Session.unknown_0C == 0) {
					Session.Type = GAME_NORMAL;
					FUN_00786080();
					Theme.Set_Shuffle(shuffle);
					FUN_00799D10();
					return result;
				}
				Session.unknown_0C = 0;
				Session.unknown_10 = 0;
				break;
			case 1:
				DAT_00B73678 = kind;
				break;
			default:
				Session.Type = GAME_NORMAL;
				Session.unknown_0C = 0;
				Session.unknown_10 = 0;
				FUN_00786080();
				Theme.Set_Shuffle(shuffle);
				FUN_00799D10();
				return result;
			}
		}
		FUN_0077E110();
		switch (Session.unknown_0C) {
		case 0:
			first_time = false;
			result = FUN_00798DE0();
			done = result != 1;
			break;
		case 1:
			if (first_time) {
				FUN_007A2970();
				Session.unknown_0C = 0;
			} else {
				result = FUN_0079FC40();
				first_time = false;
				if (result == 0) {
					Session.unknown_0C = 0;
				} else {
					done = true;
				}
			}
			break;
		case 2:
			if (first_time) {
				FUN_007A2970();
				Session.unknown_0C = 0;
			} else {
				result = FUN_0079FC40();
				first_time = false;
				if (result == 0) {
					Session.unknown_0C = 0;
				} else {
					done = true;
				}
			}
			break;
		case 4:
			result = FUN_00787770(0, first_time);
			first_time = false;
			if (result == 0) {
				if (Session.unknown_0C == 4) {
					Session.unknown_0C = 0;
				}
			} else {
				done = true;
			}
			break;
		case 3:
			if (first_time) {
				FUN_007A2970();
			}
			result = FUN_007AFB90(first_time);
			first_time = false;
			if (result == 0) {
				Session.unknown_0C = 0;
			} else {
				done = true;
			}
			break;
		case 5:
			result = FUN_0079F140(first_time);
			first_time = false;
			if (result == 0) {
				if (Session.unknown_0C == 5) {
					Session.unknown_0C = Session.unknown_10;
					Session.unknown_10 = 0;
				}
			} else {
				done = true;
			}
			break;
		case 6:
			result = FUN_00799D30();
			first_time = false;
			if (result == -2 || result == -1) {
				Session.Type = GAME_NORMAL;
				Session.unknown_0C = 0;
				Session.unknown_10 = 0;
				FUN_00786080();
				Theme.Set_Shuffle(shuffle);
				FUN_00799D10();
				return result;
			}
			if (result == 0) {
				Session.unknown_0C = 0;
			}
			break;
		}
		if (result != 1 && DAT_00B73678 != 0) {
			bool force = false;
			if (done) {
				force = true;
			}
			int kind = FUN_0077B280();
			if (DAT_00B73678 != kind || force) {
				FUN_00785EA0();
				DAT_00B73678 = 0;
			}
		}
		if (done) {
			if (result == 1) {
				FUN_00799D10();
				return result;
			}
			FUN_00786080();
			Session.Type = GAME_NORMAL;
			Theme.Set_Shuffle(shuffle);
			Session.unknown_0C = 0;
			Session.unknown_10 = 0;
			FUN_00799D10();
			return 0;
		}
	}
}
