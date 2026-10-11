// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
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

int score_font_count;

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
