// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "house.h"
#include "houstype.h"
#include "random.h"
#include "scenario.h"
#include "session.h"

#include <math.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#undef LoadString
#include <stddef.h>
#include "stringtable.h"

// FUNCTION: 0x005C9510 $E2
// FUNCTION: 0x005C9540 $E5
// FUNCTION: 0x005C9560 $E8
// FUNCTION: 0x005C9580 $E11
// FUNCTION: 0x005C95A0 $E14
// FUNCTION: 0x005C95C0 $E17
// FUNCTION: 0x005C95E0 $E20
// FUNCTION: 0x005C9620 $E23
// FUNCTION: 0x005C9650 $E26
// FUNCTION: 0x005C9680 $E29
// FUNCTION: 0x005C96B0 $E32
// FUNCTION: 0x005C96C0 $E35
// FUNCTION: 0x005C96E0 $E38

class Surface;
struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
};
extern Surface * DAT_0088730C;
void __fastcall FUN_0072D730(int side);
void FUN_0072D780(void);
void __fastcall FUN_0072EAD0(Surface * surface, Rect * rect);
void __fastcall FUN_004F4780(bool flag, Surface * surface, bool other);
int __fastcall FUN_0060D380(int resource, DLGPROC proc, bool focus);
int CALLBACK FUN_005C9B10(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

extern MPlayerScoreType * DAT_00ABF958[8];
extern int DAT_00ABF9F4;
extern int DAT_00ABF9F8;

class MPScoreScreen
{
	public:
		MPScoreScreen(void);
		void Present(void);
};

// FUNCTION: 0x005C9700
MPScoreScreen::MPScoreScreen(void)
{
	DAT_00ABF9F8 = DAT_00ABF9F4 = 0;
	for (int i = 0; i < 8; i++) {
		DAT_00ABF958[i] = NULL;
	}
}

// FUNCTION: 0x005C9720
void MPScoreScreen::Present(void)
{
	FUN_0072D730(Scen->PlayerSideIndex);
	Rect rect(0, 0, Drawing::RenderWidth, Drawing::RenderHeight);
	FUN_0072EAD0(DAT_0088730C, &rect);
	FUN_004F4780(true, DAT_0088730C, false);
	FUN_0060D380(0x108, FUN_005C9B10, false);
	FUN_0072EAD0(DAT_0088730C, &rect);
	FUN_004F4780(true, DAT_0088730C, false);
	FUN_0072D780();
}

// FUNCTION: 0x005C97B0
void __fastcall FUN_005C97B0(wchar_t * buffer, int seconds)
{
	if (buffer != NULL) {
		if (seconds / 3600 > 99) {
			seconds = 359999;
		}
		int hours = seconds / 3600;
		seconds -= hours * 3600;
		int minutes = seconds / 60;
		seconds -= minutes * 60;
		swprintf(buffer, StringTable::LoadString("TXT_TIME_FORMAT_HOURS", NULL, "D:\\ra2mdpost\\MPScore.cpp", 0xd9), hours, minutes, seconds);
	}
}

// FUNCTION: 0x005C9850
void __fastcall FUN_005C9850(HWND dialog, int id, int value)
{
	wchar_t text[32];
	swprintf(text, L"%d", value);
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4b2, 0, (LPARAM)text);
}

extern DynamicVectorClass<HouseClass *> DAT_00A80228;
extern HouseClass * DAT_00AC1198;
void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: 0x005C98A0
void FUN_005C98A0(void)
{
	Session.NumScores = 0;
	int count = DAT_00A80228.Count();
	for (int house = 0; house < count; house++) {
		HouseClass * hptr = DAT_00A80228[house];
		if (!hptr || hptr->Class->IsMultiplayPassive == true || hptr == DAT_00AC1198) {
			continue;
		}
		int score_index = Session.NumScores++;
		Session.Score[score_index].Wins = 0;
		wcscpy(Session.Score[score_index].Name, hptr->UIName);
		Session.Score[score_index].Lost[0] = 0;
		Session.Score[score_index].Kills[0] = 0;
		Session.Score[score_index].Built[0] = 0;
		Session.Score[score_index].Score[0] = 0;
		Session.Score[score_index].Scheme = hptr->Scheme;
		if (!hptr->IsDefeated) {
			Session.Score[score_index].Wins++;
			Session.Winner = score_index;
		}
		int kills = 0;
		int i;
		for (i = 0; i < 20; i++) {
			kills += hptr->UnitsKilled[i];
		}
		for (i = 0; i < 20; i++) {
			kills += hptr->BuildingsKilled[i];
		}
		Session.Score[score_index].Kills[0] = kills;
		Session.Score[score_index].Lost[0] = hptr->UnitsLost + hptr->BuildingsLost;
		int built = hptr->PBQuantity.Total();
		built += hptr->PUQuantity.Total();
		built += hptr->PIQuantity.Total();
		built += hptr->PAQuantity.Total();
		Session.Score[score_index].Built[0] = built;
		if (hptr->PointTotal > 0) {
			Session.Score[score_index].Score[0] += hptr->PointTotal;
		}
		if (!hptr->IsDefeated) {
			int score = Session.Score[score_index].Score[0];
			Session.Score[score_index].Score[0] = score + score / 2 + Scen->RandomNumber(score / 2, score);
		}
		FUN_004068E0("%hs: %s\n Scheme: %d\n Lost = %d\n Kills = %d\n Built = %d\n Score = %d\n",
			Session.Score[score_index].Name,
			Session.Score[score_index].Wins > 0 ? "Winner" : "Loser",
			Session.Score[score_index].Scheme,
			Session.Score[score_index].Lost[0],
			Session.Score[score_index].Kills[0],
			Session.Score[score_index].Built[0],
			Session.Score[score_index].Score[0]);
	}
}

int __cdecl FUN_005C9AE0(void const * a, void const * b);

// FUNCTION: 0x005C9AA0
void FUN_005C9AA0(void)
{
	int count = Session.NumScores;
	for (int i = 0; i < count; i++) {
		DAT_00ABF958[i] = &Session.Score[i];
	}
	if (count > 1) {
		qsort(DAT_00ABF958, count, 4, FUN_005C9AE0);
	}
}

// FUNCTION: 0x005C9AE0
int __cdecl FUN_005C9AE0(void const * a, void const * b)
{
	MPlayerScoreType * first = *(MPlayerScoreType * const *)a;
	MPlayerScoreType * second = *(MPlayerScoreType * const *)b;
	if (first->Score[0] == second->Score[0]) {
		return 0;
	}
	return first->Score[0] > second->Score[0] ? -1 : 1;
}

#include "theme.h"
#include "scheme.h"

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
struct OwnerDraw {
	static void __fastcall GetRectangle(HWND window, RECT * rect);
};
extern int DAT_00B779D4;
extern Surface * DAT_00887310;
void __fastcall FUN_00603D30(HWND item, int surface);
void __fastcall FUN_0072AA10(Rect * rect, int dx, int dy);
void __fastcall FUN_00621B80(Rect * rect, Surface * surface, unsigned short color, int alpha);
int __fastcall FUN_005CA110(int index);

static const int DAT_0082FD5C[8][5] = {
	{ 0x411, 0x419, 0x6a3, 0x6a5, 0x6a6 },
	{ 0x412, 0x41a, 0x6a4, 0x6a7, 0x6a8 },
	{ 0x413, 0x41b, 0x6a9, 0x6aa, 0x6ab },
	{ 0x414, 0x41c, 0x6ac, 0x6ad, 0x6ae },
	{ 0x415, 0x41d, 0x6af, 0x6b0, 0x6b1 },
	{ 0x416, 0x41e, 0x6b2, 0x6b3, 0x6b4 },
	{ 0x417, 0x41f, 0x6b5, 0x6b6, 0x6b7 },
	{ 0x418, 0x420, 0x6b8, 0x6b9, 0x6ba },
};
static const int DAT_0082FDFC[2] = { 0x790, 0x791 };
static const int DAT_0082FE04[8] = { 0x78f, 0x792, 0x793, 0x794, 0x795, 0x796, 0x797, 0x798 };

// FUNCTION: 0x005C9B10
int CALLBACK FUN_005C9B10(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	wchar_t text[32];
	RECT first;
	RECT last;
	HWND item;
	int i;
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc != 0) {
		return rc;
	}
	int * result = (int *)GetWindowLong(window, DWL_USER);
	switch (message) {
		case 0x497:
			Theme.Play_Song(Theme.From_Name("SCORE"));
			if (Session.Type == GAME_INTERNET) {
				swprintf(text, StringTable::LoadString("TXT_GAME", NULL, "D:\\ra2mdpost\\MPScore.cpp", 0x174), DAT_00B779D4);
			} else {
				swprintf(text, StringTable::LoadString("TXT_GAME", NULL, "D:\\ra2mdpost\\MPScore.cpp", 0x176), Session.GamesPlayed);
			}
			{
				wchar_t const * title;
				if (Session.Type == GAME_SKIRMISH) {
					title = StringTable::LoadString("GUI:SkirmishScore", NULL, "D:\\ra2mdpost\\MPScore.cpp", 0x17a);
				} else {
					title = StringTable::LoadString("GUI:MultiplayerScore", NULL, "D:\\ra2mdpost\\MPScore.cpp", 0x17c);
				}
				item = GetDlgItem(window, 0x694);
				SendMessage(item, 0x4b2, 0, (LPARAM)title);
			}
			item = GetDlgItem(window, 0x6d2);
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			FUN_005C97B0(text, Scen->ElapsedTimer.Value() / 60);
			item = GetDlgItem(window, 0x3ea);
			SendMessage(item, 0x4b2, 0, (LPARAM)text);
			FUN_005C98A0();
			for (i = 0; i < Session.NumScores; i++) {
				DAT_00ABF958[i] = &Session.Score[i];
			}
			if (Session.NumScores > 1) {
				qsort(DAT_00ABF958, Session.NumScores, 4, FUN_005C9AE0);
			}
			for (i = 0; i < 2; i++) {
				int header = DAT_0082FDFC[i];
				item = GetDlgItem(window, header);
				int surface = FUN_005CA110(i);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
			}
			for (i = 0; i < Session.NumScores; i++) {
				ColorScheme * scheme = ColorSchemes[DAT_00ABF958[i]->Scheme];
				RGBClass rgb = scheme->HSV;
				COLORREF color = RGB(rgb.Get_Red(), rgb.Get_Green(), rgb.Get_Blue());
				int flag = DAT_0082FE04[i];
				item = GetDlgItem(window, flag);
				int surface = FUN_005CA110(i + 2);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				int id = DAT_0082FD5C[i][0];
				wchar_t * name = DAT_00ABF958[i]->Name;
				item = GetDlgItem(window, id);
				SendMessage(item, 0x4b2, 0, (LPARAM)name);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0082FD5C[i][1];
				FUN_005C9850(window, id, DAT_00ABF958[i]->Kills[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0082FD5C[i][2];
				FUN_005C9850(window, id, DAT_00ABF958[i]->Lost[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0082FD5C[i][3];
				FUN_005C9850(window, id, DAT_00ABF958[i]->Built[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
				id = DAT_0082FD5C[i][4];
				FUN_005C9850(window, id, DAT_00ABF958[i]->Score[0]);
				item = GetDlgItem(window, id);
				SendMessage(item, 0x498, 0, color);
			}
			break;

		case WM_COMMAND:
			if (LOWORD(wparam) == 0x6d1) {
				*result = 1;
			}
			break;

		case WM_PAINT:
			item = GetDlgItem(window, 0x6d2);
			OwnerDraw::GetRectangle(item, &first);
			int last_id = DAT_0082FD5C[Session.NumScores - 1][4];
			item = GetDlgItem(window, last_id);
			OwnerDraw::GetRectangle(item, &last);
			Rect rect(first.left, first.top, last.right - first.left, last.bottom - first.top);
			FUN_0072AA10(&rect, -4, -8);
			FUN_00621B80(&rect, DAT_00887310, 0, 0x9f);
			break;
	}
	return 0;
}

struct PCX
{
	static PCX Instance;
	int GetSurface(char const * name, void * palette);
};
extern char * DAT_00844B24;
extern char * DAT_00844B28;
extern char * DAT_00844B2C;
extern char * DAT_00844B30;
extern char * DAT_00844B34;
extern char * DAT_00844B38;
extern char * DAT_00844B3C;
extern char * DAT_00844B40;
extern char * DAT_00844B44;
extern char * DAT_00844B48;
extern char * DAT_00844B4C;
extern char * DAT_00844B50;
extern char * DAT_00844B54;
extern char * DAT_00844B58;
extern char * DAT_00844B5C;
extern char * DAT_00844B60;
extern char * DAT_00844B64;
extern char * DAT_00844B68;
extern char * DAT_00844B6C;
extern char * DAT_00844B70;
extern char * DAT_00844B74;
extern char * DAT_00844B78;
extern char * DAT_00844B7C;
extern char * DAT_00844B80;
extern char * DAT_00844B84;
extern char * DAT_00844B88;
extern char * DAT_00844B8C;
extern char * DAT_00844B90;
extern char * DAT_00844B94;
extern char * DAT_00844B98;

// FUNCTION: 0x005CA110
int __fastcall FUN_005CA110(int index)
{
	if (Scen->PlayerSideIndex == 0) {
		if (index == 0) {
			return PCX::Instance.GetSurface(DAT_00844B24, NULL);
		}
		if (index == 1) {
			return PCX::Instance.GetSurface(DAT_00844B28, NULL);
		}
		if (index == 2) {
			return PCX::Instance.GetSurface(DAT_00844B2C, NULL);
		}
		if (index == 3) {
			return PCX::Instance.GetSurface(DAT_00844B30, NULL);
		}
		if (index == 4) {
			return PCX::Instance.GetSurface(DAT_00844B34, NULL);
		}
		if (index == 5) {
			return PCX::Instance.GetSurface(DAT_00844B38, NULL);
		}
		if (index == 6) {
			return PCX::Instance.GetSurface(DAT_00844B3C, NULL);
		}
		if (index == 7) {
			return PCX::Instance.GetSurface(DAT_00844B40, NULL);
		}
		if (index == 8) {
			return PCX::Instance.GetSurface(DAT_00844B44, NULL);
		}
		if (index == 9) {
			return PCX::Instance.GetSurface(DAT_00844B48, NULL);
		}
	} else if (Scen->PlayerSideIndex == 1) {
		if (index == 0) {
			return PCX::Instance.GetSurface(DAT_00844B4C, NULL);
		}
		if (index == 1) {
			return PCX::Instance.GetSurface(DAT_00844B50, NULL);
		}
		if (index == 2) {
			return PCX::Instance.GetSurface(DAT_00844B54, NULL);
		}
		if (index == 3) {
			return PCX::Instance.GetSurface(DAT_00844B58, NULL);
		}
		if (index == 4) {
			return PCX::Instance.GetSurface(DAT_00844B5C, NULL);
		}
		if (index == 5) {
			return PCX::Instance.GetSurface(DAT_00844B60, NULL);
		}
		if (index == 6) {
			return PCX::Instance.GetSurface(DAT_00844B64, NULL);
		}
		if (index == 7) {
			return PCX::Instance.GetSurface(DAT_00844B68, NULL);
		}
		if (index == 8) {
			return PCX::Instance.GetSurface(DAT_00844B6C, NULL);
		}
		if (index == 9) {
			return PCX::Instance.GetSurface(DAT_00844B70, NULL);
		}
	} else if (Scen->PlayerSideIndex == 2) {
		if (index == 0) {
			return PCX::Instance.GetSurface(DAT_00844B74, NULL);
		}
		if (index == 1) {
			return PCX::Instance.GetSurface(DAT_00844B78, NULL);
		}
		if (index == 2) {
			return PCX::Instance.GetSurface(DAT_00844B7C, NULL);
		}
		if (index == 3) {
			return PCX::Instance.GetSurface(DAT_00844B80, NULL);
		}
		if (index == 4) {
			return PCX::Instance.GetSurface(DAT_00844B84, NULL);
		}
		if (index == 5) {
			return PCX::Instance.GetSurface(DAT_00844B88, NULL);
		}
		if (index == 6) {
			return PCX::Instance.GetSurface(DAT_00844B8C, NULL);
		}
		if (index == 7) {
			return PCX::Instance.GetSurface(DAT_00844B90, NULL);
		}
		if (index == 8) {
			return PCX::Instance.GetSurface(DAT_00844B94, NULL);
		}
		if (index == 9) {
			return PCX::Instance.GetSurface(DAT_00844B98, NULL);
		}
	}
	return 0;
}

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007EE6E8;
namespace fastmath { double __cdecl sqrt(double x); }

// FUNCTION: 0x005CA420 $E79
double DAT_00ABFA20 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005CA450 $E82
double DAT_00ABFA70 = DAT_00ABFA20 / DAT_007EE6E8;
