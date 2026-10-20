// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007ECF50;
extern const double DAT_007ECF58;
extern const double DAT_007ECF68;
extern const double DAT_007ECF70;
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;

// FUNCTION: 0x00552850 $E2
double DAT_00ABC960 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x00552880 $E5
double DAT_00ABC9B0 = DAT_00ABC960 / DAT_007ECF50;

// FUNCTION: 0x005528A0 $E8
double DAT_00ABC998 = 1.0 / DAT_00ABC9B0;

// FUNCTION: 0x005528C0 $E11
double DAT_00ABC990 = DAT_007ECF58 * 45.0;

// FUNCTION: 0x005528E0 $E14
double DAT_00ABC968 = DAT_007ECF58 * 60.0;

// FUNCTION: 0x00552900 $E17
double DAT_00ABC940 = DAT_007ECF58 * 90.0;

// FUNCTION: 0x00552920 $E20
int DAT_00ABC9B8 = (int)(FUN_004CAD50(DAT_00ABC940 - DAT_00ABC968) * DAT_00ABC960 * 0.5);

// FUNCTION: 0x00552960 $E23
double DAT_00ABC970 = FUN_004CADE0(DAT_00ABC9B8 * 0.00390625);

// FUNCTION: 0x00552990 $E26
double DAT_00ABC988 = FUN_004CADE0((DAT_00ABC9B8 * 2) / DAT_00ABC960);

// FUNCTION: 0x005529C0 $E29
int DAT_00ABC9AC = (int)(DAT_00ABC9B8 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005529F0 $E32
ShortPair DAT_00ABC958;

// FUNCTION: 0x00552A00 $E35
IntTriple DAT_00ABC978;

#include "ldprgmgr.h"
#include "mixfile.h"
#include "session.h"
#include "scenario.h"
#include "bitfont.h"
#include "surface.h"
#include "dsurface.h"
#include "_surface.h"

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
	static unsigned short RedShiftLeft;
	static int RedShiftRight;
	static unsigned short GreenShiftLeft;
	static int GreenShiftRight;
	static unsigned short BlueShiftLeft;
	static int BlueShiftRight;
};

extern int DAT_007F5BE0;
extern int DAT_007F5BE4;
extern int DAT_007F5BEC;
extern int DAT_007F5BF0;
extern int DAT_00ABC9A0;
extern int DAT_00ABC9A4;
extern int DAT_00ABC9A8;
extern PKey DAT_00886980;

void __cdecl FUN_0072B460(void);
void __cdecl FUN_0072B870(void);
void __cdecl FUN_004A4AC0(const char * text);
Point2D __fastcall FUN_0072A9C0(Rect const * rect);
void __fastcall FUN_0072A9E0(Rect * rect, int amount);
void __fastcall FUN_0072B3E0(int side);
void __fastcall FUN_0072B530(int country);
void __fastcall FUN_00621B80(Rect * rect, Surface * surface, unsigned short color, unsigned char alpha);

LoadProgressManager * LoadProgressManager::Instance;

// FUNCTION: 0x00552A20
void FUN_00552A20(void)
{
	DAT_00ABC9A8 = 0;
	DAT_00ABC9A0 = 128;
	DAT_00ABC9A4 = 128;
}

// FUNCTION: 0x00552A40
void LoadProgressManager::Create(void)
{
	if (Instance == NULL) {
		LoadProgressManager * manager = new LoadProgressManager;
		Instance = manager;
	}
}

// FUNCTION: 0x00552AA0
void LoadProgressManager::Destroy(void)
{
	if (Instance != NULL) {
		Instance->FUN_00554370();
		operator delete(Instance);
	}
	Instance = NULL;
}

// FUNCTION: 0x00552AD0
LoadProgressManager::LoadProgressManager(void)
{
	LoadMix = 0;
	LoadMDMix = 0;
	LoadMessage = 0;
	LoadBriefing = 0;
	TitleBarSHP = 0;
	LoadScreenSHP = 0;
	LoadBarSHP = 0;
	TitleBarSHP_loaded = false;
	LoadScreenSHP_loaded = false;
	LoadBarSHP_loaded = false;
	Field54 = 0;
	Field58 = 0;
	Field5C = 0;
	ProgressSurface = 0;
}

// FUNCTION: 0x00552BE0
Point2D LoadProgressManager::Text_Origin(void)
{
	Point2D p;
	if (Session.Type == GAME_NORMAL) {
		p.X = LoadScreenSHPRect.X;
		if (Drawing::RenderWidth == DAT_007F5BE0) {
			p.X += 0x54;
		} else {
			p.X += 0xa4;
		}
		p.Y = LoadScreenSHPRect.Y + 7;
	} else {
		p = FUN_0072A9C0(&LoadBarSHPRect);
		if (Drawing::RenderWidth == DAT_007F5BE0) {
			p.X += 0xc;
			p.Y += 0x100;
		} else {
			p.X += 0x10;
			p.Y += 0x141;
		}
	}
	return p;
}

// FUNCTION: 0x00552C90
int FUN_00552C90(void)
{
	int result = -1;
	if (Session.Type != GAME_NORMAL) {
		result = 0x146 + ((Drawing::RenderWidth != DAT_007F5BE0) ? 0x50 : 0);
	}
	return result;
}

// FUNCTION: 0x00552CC0
void LoadProgressManager::Load_Mixes(void)
{
	if (LoadMDMix == NULL) {
		FUN_004A4AC0(" loadmd.MIX");
		LoadMDMix = new MixFileClass("LOADMD.MIX", &DAT_00886980);
		if (LoadMDMix == NULL) {
			return;
		}
	}
	if (LoadMix == NULL) {
		LoadMix = new MixFileClass("LOAD.MIX", &DAT_00886980);
		if (LoadMix == NULL) {
			return;
		}
	}
	if (Session.Type == GAME_NORMAL) {
		FUN_0072B3E0(Scen->PlayerSideIndex);
	} else {
		FUN_0072B530(Session.Players[0]->Get_Country());
	}
}

// FUNCTION: 0x00554100
void __fastcall FUN_00554100(Point2D * point)
{
	int width = Drawing::RenderWidth;
	int box_width = DAT_007F5BE4;
	if (width >= box_width) {
		int dy = (Drawing::RenderHeight - DAT_007F5BF0) / 2;
		int dx = (width - box_width) / 2;
		point->X += dx;
		point->Y += dy;
	}
}

namespace OwnerDraw {
void __fastcall DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int a, int flags, int b, int c, int d);
}

struct ColourBytes {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned Packed(void) const { return (((B << 8) | G) << 8) | R; }
};

// FUNCTION: 0x00554280
void __fastcall FUN_00554280(wchar_t const * text, Surface * surface, Rect * rect, unsigned short color, int a)
{
	LTRBStruct bounds;
	bounds.Left = rect->X;
	bounds.Top = rect->Y;
	bounds.Right = rect->X + rect->Width;
	bounds.Bottom = rect->Y + rect->Height;
	ColourBytes c;
	c.R = color >> Drawing::RedShiftLeft << Drawing::RedShiftRight;
	c.G = color >> Drawing::GreenShiftLeft << Drawing::GreenShiftRight;
	c.B = color >> Drawing::BlueShiftLeft << Drawing::BlueShiftRight;
	OwnerDraw::DrawWideText(surface, text, &bounds, BitFont::Instance, c.Packed(), a, 0xc, 0, 0, 0);
}

// FUNCTION: 0x00554350
void LoadProgressManager::FUN_00554350(Surface * target)
{
	if (ProgressSurface != NULL) {
		target->Blit_From(*ProgressSurface, false, true);
	}
}

// FUNCTION: 0x00554370
void LoadProgressManager::FUN_00554370(void)
{
	if (TitleBarSHP_loaded && TitleBarSHP != NULL) {
		operator delete(TitleBarSHP);
	}
	TitleBarSHP = NULL;
	if (LoadScreenSHP_loaded && LoadScreenSHP != NULL) {
		operator delete(LoadScreenSHP);
	}
	LoadScreenSHP = NULL;
	if (LoadBarSHP_loaded && LoadBarSHP != NULL) {
		operator delete(LoadBarSHP);
	}
	LoadBarSHP = NULL;
	FUN_0072B460();
	FUN_0072B870();
	if (LoadMix != NULL) {
		delete LoadMix;
		LoadMix = NULL;
	}
	if (LoadMDMix != NULL) {
		delete LoadMDMix;
		LoadMDMix = NULL;
	}
	if (ProgressSurface != NULL) {
		delete ProgressSurface;
	}
	ProgressSurface = NULL;
}

// FUNCTION: 0x00554400
bool LoadProgressManager::FUN_00554400(int message, int extra)
{
	if (message == 0x11ae) {
		FUN_00554350(HiddenSurface);
		return true;
	}
	return false;
}

// FUNCTION: 0x00554430 $E96
double DAT_00ABC9E8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x00554460 $E99
double DAT_00ABCA68 = DAT_00ABC9E8 / DAT_007ECF68;

// FUNCTION: 0x00554480 $E102
double DAT_00ABCA38 = DAT_007E1718 / DAT_00ABCA68;

// FUNCTION: 0x005544A0 $E105
double DAT_00ABCA30 = DAT_007E1720 * DAT_007ECF70;

// FUNCTION: 0x005544C0 $E108
double DAT_00ABC9F0 = DAT_007E1728 * DAT_007ECF70;

// FUNCTION: 0x005544E0 $E111
double DAT_00ABC9C8 = DAT_007E1730 * DAT_007ECF70;

// FUNCTION: 0x00554500 $E114
int DAT_00ABCA70 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABC9C8 - DAT_00ABC9F0) * DAT_00ABC9E8));

// FUNCTION: 0x00554540 $E117
double DAT_00ABC9F8 = FUN_004CADE0(DAT_00ABCA70 * DAT_007E1740);

// FUNCTION: 0x00554570 $E120
double DAT_00ABCA28 = FUN_004CADE0((DAT_00ABCA70 * 2) / DAT_00ABC9E8);
