// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "score.h"
#include "_keyboar.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "_timer.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "goptions.h"
#include "houstype.h"
#include "keyboard.h"
#include "language\language.h"
#include "misc.h"
#include "mixfile.h"
#include "movie.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "scenario.h"
#include "session.h"
#include "shapeset.h"
#include "surface.h"
#include "theme.h"
#include "windlg.h"
#include "winstub.h"
#define SIZEGBAR			140
#define HALLFAME_X		11
#define HALLFAME_Y		242
#define NUMFAMENAMES				9
#define MAX_FAMENAME_LENGTH	104

bool StillUpdating;

// FUNCTION: 0x0068CB30 $E2
// FUNCTION: 0x0068CB60 $E5
// FUNCTION: 0x0068CB80 $E8
// FUNCTION: 0x0068CBA0 $E11
// FUNCTION: 0x0068CBC0 $E14
// FUNCTION: 0x0068CBE0 $E17
// FUNCTION: 0x0068CC00 $E20
// FUNCTION: 0x0068CC40 $E23
// FUNCTION: 0x0068CC70 $E26
// FUNCTION: 0x0068CCA0 $E29
// FUNCTION: 0x0068CCD0 $E32
// FUNCTION: 0x0068CCE0 $E35

// FUNCTION: 0x0068CD00 $E38
Coord DAT_00B05568(128, 128, 0);

int score_font_count;

int FUN_00407000(void);

inline void ScoreClass::DoSound(const char * name, int volume)
{
	if (name != NULL && FUN_00407000()) {

		for (int i = 0; i < ScoreSnds.Count(); i++) {
			SfxEntry * snd = ScoreSnds[i];

			if (stricmp(name, snd->Get_Name()) == 0) {
				break;
			}
		}
	}
}

// FUNCTION: 0x0068EB90
int ScoreClass::Alloc_Object(ScoreAnimClass *obj)
{
	return(ScoreObjs.Add(obj));
}

// FUNCTION: 0x0068EC00
bool ScoreClass::Score_Object_Not_Present(ScoreAnimClass * obj)
{
	if (ScoreObjs.ID(obj) == -1) {
		return(true);
	}
	return(false);
}

// FUNCTION: 0x0068EC20
void ScoreClass::Cycle_Wait_Click(bool cycle)
{
	int counter = 0;
	int minclicks = 20;
	unsigned int timingtime = TickCount;
	SerialPacketType sendpacket;
	SerialPacketType receivepacket;
	int packetlen;

	Keyboard->Clear();
	while (minclicks || (!Keyboard->Check()) ) {

		if (Session.Type == GAME_NULL_MODEM ||
			Session.Type == GAME_MODEM) {

			if ( (TickCount - timingtime) > PACKET_TIMING_TIMEOUT) {
				memset (&sendpacket, 0, sizeof(SerialPacketType));
				sendpacket.Command = SERIAL_SCORE_SCREEN;
				sendpacket.ScenarioInfo.ResponseTime = NullModem.Response_Time();
				sendpacket.ID = Session.ModemType;

				NullModem.Send_Message (&sendpacket, sizeof(sendpacket), 0);
				timingtime = TickCount;
			}

			if (NullModem.Get_Message (&receivepacket, &packetlen) > 0) {
				packetlen = packetlen;
			}

			NullModem.Service();
		}

		Call_Back_Delay(1);
		if (minclicks) {
			minclicks--;
			Keyboard->Clear();
		}
	}
	Keyboard->Clear();
}

void ScoreClass::Wait_For_Print(ScoreAnimClass * obj)
{
	if (!Score_Object_Not_Present(obj)) {
		do {
			Call_Back_Delay(1);
		} while (!Score_Object_Not_Present(obj));
	}
}

// FUNCTION: 0x0068ED80
unsigned int ScoreClass::Do_Calc(HouseClass * house)
{
	int remain = 0;
	int total = 0;

	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * hptr = Houses[i];
		if (house->Is_Ally(hptr) && hptr->Is_Ally(house) || house == hptr) {
			remain += hptr->ABQuantity.Total();
			remain += hptr->AUQuantity.Total();
			remain += hptr->AIQuantity.Total();
			remain += hptr->AAQuantity.Total();
			total += hptr->BuildingsLost;
			total += hptr->UnitsLost;
		}
	}

	total += remain;
	DebugString("Remaining: %ld, Total: %ld\n", remain, total);

	float build_eco = total > 0 ? ((float)remain / total) : 0.0f;

	DebugString("BuildEconomy = %f\n", build_eco);

	DebugString("Money: %ld, Harvested: %ld, Initial: %ld\n", house->Available_Money(), house->HarvestedCredits, house->Control.InitialCredits);

	total = house->HarvestedCredits + house->Control.InitialCredits;
	float money_eco = total > 0 ? (float)house->Available_Money() / (float)total : 0.0f;

	DebugString("MoneyEconomy = %f\n", money_eco);

	float eco = money_eco + build_eco;

	unsigned int efficiency = (unsigned int)((eco / 2.0) * 100.0);

	DebugString("Efficiency: %ld\n", efficiency);

	return(efficiency);
}

// FUNCTION: 0x0068EF20
void ScoreClass::Do_Graphs(void)
{
	int allyu = 0;
	int allyb = 0;
	int enemyu = 0;
	int enemyb = 0;

	for (int hous = 0; hous < Houses.Count(); hous++) {

		HouseClass *hows = Houses[hous];
		DebugString("Stats: %s - UnitsLost %ld, BuildingsLost %ld\n", (char *)hows->Class->IniName, hows->UnitsLost, hows->BuildingsLost);

		if ((PlayerPtr->Is_Ally(hows) && hows->Is_Ally(PlayerPtr)) || PlayerPtr == hows) {
			allyu += hows->UnitsLost;
			allyb += hows->BuildingsLost;
		} else {
			enemyu += hows->UnitsLost;
			enemyb += hows->BuildingsLost;
		}

		DebugString("AllyUnits %ld, AllyBuildings %ld, EnemyUnits %ld, EnemyBldgs %ld\n", allyu, allyb, enemyu, enemyb);
	}

	if (PlayerPtr->Class->Side == SIDE_GDI) {
		Do_Graph(allyb, enemyb, 227);
		Do_Graph(allyu, enemyu, 283);
	} else {
		Do_Graph(enemyb, allyb, 227);
		Do_Graph(enemyu, allyu, 283);
	}
}

// FUNCTION: 0x0068F8A0
void ScoreClass::Show_Credits(void)
{
	char str[20];

	sprintf(str, "%d", PlayerPtr->Available_Money());

	int x = 590 - (12 * strlen(str));

	ScoreAnimClass *obj = new ScorePrintClass(str, XPos + x, YPos + 214, FullFont, false);
	Alloc_Object(obj);

	Wait_For_Print(obj);
}

// FUNCTION: 0x0068F9B0
void ScoreClass::Print_Minutes(int time)
{
	wchar_t str[20];

	if (((unsigned)time/3600) > 99) time = 359999;

	int hours	 = int((unsigned)time / 3600);
	time		-= hours * 3600;
	int minutes	 = int((unsigned)time / 60);
	time		-= minutes * 60;
	int seconds	 = (unsigned)time;

	swprintf(str, L"%02ld:%02ld:%02ld", hours, minutes, seconds);

	int x = XPos - (BigFont->String_Width(str) / 2) + 542;

	ScoreAnimClass *obj = new ScorePrintClass(str, x, YPos + 322, BigFont, false);
	Alloc_Object(obj);

	Wait_For_Print(obj);
}

// FUNCTION: 0x0068FB10
Rect ScoreClass::Count_Up_Print(Surface * surf, wchar_t *str, int percent, int maxval, int xpos, int ypos)
{
	wchar_t destbuf[64];

	swprintf(destbuf, str, percent >= maxval ? maxval : percent);
	ypos = ypos - FullFont->Get_Height();
	FullFont->Print_String(surf, destbuf, xpos, ypos, 2);
	return(Rect(xpos, ypos, FullFont->String_Width(destbuf), FullFont->Get_Height()));
}

// FUNCTION: 0x0068FBB0
void ScoreClass::Input_Name(char str[], int xpos, int ypos)
{
	int x = xpos;
	int key = 0;
	int index = 0;

	do {

		Timing();

		if (Keyboard->Check()) {
			key = Keyboard->WWKeyboardClass::To_ASCII(Keyboard->WWKeyboardClass::Get()) & 0xFF;
			Call_Back();

			if (x >= xpos + 90) {
				while (Keyboard->Check()) {
					Keyboard->Get();
				}
			}

			if (key == KN_BACKSPACE && x >= xpos + 84 && str[index] != 0 && str[index] != 32) {
				Rect rect(x, ypos + 1, FullFont->Char_Width(str[index]) + 1, 16);
				HiddenSurface->Blit_From(rect, *SurfacePtr, rect);
				AlternateSurface->Blit_From(rect, *SurfacePtr, rect);
				key = 32;
			}

			if (key == KN_BACKSPACE) {

				if (index) {
					char letter = str[--index];
					int cw = FullFont->Char_Width(letter) + 1;
					x -= cw;
					Rect rect(x, ypos + 1, cw, 16);
					HiddenSurface->Blit_From(rect, *SurfacePtr, rect);
					AlternateSurface->Blit_From(rect, *SurfacePtr, rect);
					str[index] = 0;
					DoSound("Back", 192);
				}

			} else if (key != KN_KEYPAD_RETURN && key >= KN_SPACE) {

				int cw = FullFont->Char_Width(key) + 1;
				if (x + FullFont->Get_Width() <= xpos + 90) {
					if (x + cw >= xpos + 84) {
						Rect rect(x, ypos + 1, FullFont->Char_Width(str[index]) + 1, 16);
						HiddenSurface->Blit_From(rect, *SurfacePtr, rect);
						AlternateSurface->Blit_From(rect, *SurfacePtr, rect);
					}
					str[index] = key;
					str[index + 1] = 0;
					Alloc_Object(new ScorePrintClass(str + index, x, ypos, FullFont, false));
					DoSound("Type", 128);
					if (x < xpos + 87) {
						index++;
						x += FullFont->Char_Width(key) + 1;
					}
				}
			}
		}

		Animate_Cursor(x, ypos);
		Call_Back_Delay(1);

	} while (key != KN_RETURN);
}

// FUNCTION: 0x0068FFE0
void ScoreClass::Animate_Cursor(int pos, int ypos)
{
	static int _lastpos = 0, _state;
	static CDTimerClass<SystemTimerClass> _timer;

	Rect rect;

	ypos += 14;

	if (pos != _lastpos) {
		rect.Set(_lastpos, ypos, HALLFAME_X, 1);
		HiddenSurface->Blit_From(rect, *AlternateSurface, rect);
		_lastpos = pos;
	}

	if (_state) {
		HiddenSurface->Draw_Line(Point2D(pos, ypos), Point2D(pos + HALLFAME_X - 1, ypos), Color);
	} else {
		rect.Set(pos, ypos, HALLFAME_X, 1);
		HiddenSurface->Blit_From(rect, *SurfacePtr, rect);
	}

	if (!_timer) {
		_state ^= 1;
		_timer = 5;
	}
}

// FUNCTION: 0x00690150 $E107

// FUNCTION: 0x00690160
void ScoreClass::Call_Back_Delay(int time)
{
	Rect rect;
	CDTimerClass<SystemTimerClass> cd = 1;

	do {
		Animate_Score_Objs();

		Draw();

		do {

			Call_Back();
			Windows_Message_Handler();

			if (!GameInFocus) {

				cd.Stop();

				Timing();

				cd.Start();
			}

			Sleep(0);

		} while (cd > 0);

		cd = 1;
		time--;

	} while (time > 0);
}

// FUNCTION: 0x00690360
void ScoreClass::Animate_Score_Objs(void)
{
	StillUpdating = false;

	if (SurfacesRestored) {
		SurfacesRestored=FALSE;
		HiddenSurface->Fill(0);
		HiddenSurface->Blit_From(*AlternateSurface);
	}

	for (int i = 0; i < ScoreObjs.Count(); i++) {
		if (ScoreObjs[i]->Update(SurfacePtr) == true) {
			delete ScoreObjs[i];
			ScoreObjs.Delete_Index(i);
		}
	}
}

// FUNCTION: 0x00690400
void ScoreClass::Draw(void)
{
	RECT r;
	GetClientRect(MainWindow, &r);
	ClientToScreen(MainWindow, (LPPOINT)&r);

	Rect rect1(XPos + r.left, YPos + r.top, 640, 400);
	Rect rect2(XPos, YPos, 640, 400);

	if (MouseCursor != NULL) MouseCursor->Draw_Mouse(HiddenSurface);

	VisibleSurface->Blit_From(rect1, *HiddenSurface, rect2);

	if (MouseCursor != NULL) MouseCursor->Erase_Mouse(HiddenSurface);
}

// FUNCTION: 0x006904C0
void ScoreClass::Timing(void)
{
	if (GameInFocus == true) {
		return;
	}

	int i;

	for (i = 0; i < ScoreObjs.Count(); i++) {
		ScoreObjs[i]->Stop();
	}

	while (!GameInFocus) {
		Sleep(500);
		Windows_Message_Handler();
	}

	for (i = 0; i < ScoreObjs.Count(); i++) {
		ScoreObjs[i]->Start();
	}
}

// FUNCTION: 0x00690580
ScoreFontClass::ScoreFontClass(void) :
	Width(0),
	Height(0),
	ShapePtr(NULL),
	Drawer(NULL)
{
	Load_Sounds();
	score_font_count++;
}

// FUNCTION: 0x006905B0
ScoreFontClass::ScoreFontClass(int w, int h, void const * data, ConvertClass * drawer) :
	Width(w),
	Height(h),
	ShapePtr((ShapeSet const *)data),
	Drawer(drawer)
{
	Load_Sounds();
	score_font_count++;
}

// FUNCTION: 0x00690BB0
ScoreTimeClass::ScoreTimeClass(int xpos, int ypos, void const * data, int maxval, int xtimer, ConvertClass * drawer) :
	ScoreAnimClass(xpos, ypos, data),
	Stage(0),
	MaxStage(maxval),
	TimerReset(xtimer),
	Drawer(drawer)
{
}

// FUNCTION: 0x00690D10
ScorePrintClass::ScorePrintClass(void const * string, int xpos, int ypos, ScoreFontClass * font, bool is_fully_lit) :
	ScoreAnimClass(xpos, ypos, string),
	Pos(0),
	Stage(0),
	State(is_fully_lit),
	Font(font)
{
}

// FUNCTION: 0x00690F30
ScoreAnimClass::ScoreAnimClass(int x, int y, void const * data) :
	XPos(x),
	YPos(y),
	Timer(0),
	DataPtr(data)
{
}
