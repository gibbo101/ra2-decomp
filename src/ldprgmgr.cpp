// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"

// SYNTHETIC: GAMEMD 0x00552830 SYMBOL
// _$E41
// SYNTHETIC: GAMEMD 0x00552850 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00552880 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005528A0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x005528C0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x005528E0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00552900 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00552920 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00552960 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00552990 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x005529C0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x005529F0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00552A00 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00552A20 SYMBOL
// _$E38

#include "ldprgmgr.h"
#include "mixfile.h"
#include "session.h"
#include "scenario.h"
#include "bitfont.h"
#include "surface.h"
#include "dsurface.h"
#include "_surface.h"
#include "rgb.h"

struct Drawing {
	static int RenderWidth;
	static int RenderHeight;
	static int RedShiftLeft;
	static int RedShiftRight;
	static int GreenShiftLeft;
	static int GreenShiftRight;
	static int BlueShiftLeft;
	static int BlueShiftRight;
};

extern int DAT_007F5BE0;
extern int DAT_007F5BE4;
extern int DAT_007F5BEC;
extern int DAT_007F5BF0;
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

// FUNCTION: GAMEMD 0x00552A40
LoadProgressManager * LoadProgressManager::Create(void)
{
	if (Instance == NULL) {
		LoadProgressManager * manager = new LoadProgressManager;
		Instance = manager;
	}
	return Instance;
}

// FUNCTION: GAMEMD 0x00552AA0
void LoadProgressManager::Destroy(void)
{
	if (Instance != NULL) {
		Instance->FUN_00554370();
		operator delete(Instance);
	}
	Instance = NULL;
}

// FUNCTION: GAMEMD 0x00552AD0
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

// FUNCTION: GAMEMD 0x00552BE0
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

// FUNCTION: GAMEMD 0x00552C90
int LoadProgressManager::FUN_00552C90(void)
{
	int result = -1;
	if (Session.Type != GAME_NORMAL) {
		result = 0x146 + ((Drawing::RenderWidth != DAT_007F5BE0) ? 0x50 : 0);
	}
	return result;
}

// FUNCTION: GAMEMD 0x00552CC0
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

// FUNCTION: GAMEMD 0x00554100
void __fastcall FUN_00554100(Rect * rect)
{
	int width = Drawing::RenderWidth;
	int box_width = DAT_007F5BE4;
	if (width >= box_width) {
		int dy = (Drawing::RenderHeight - DAT_007F5BF0) / 2;
		int dx = (width - box_width) / 2;
		rect->X += dx;
		rect->Y += dy;
	}
}

// FUNCTION: GAMEMD 0x00554150
Rect __fastcall FUN_00554150(void)
{
	Rect rect;
	if (Drawing::RenderWidth == DAT_007F5BE0) {
		rect = Rect(0x10, 0xeb, 200, 20);
	} else {
		rect = Rect(0x14, 0x12c, 200, 20);
	}
	FUN_00554100(&rect);
	return rect;
}

// FUNCTION: GAMEMD 0x005541C0
void __fastcall FUN_005541C0(wchar_t const * text, Surface * surface, Point2D point, int max_width, unsigned char align, bool fixed_width)
{
	int width = 0;
	int height = 0;
	BitFont::Instance->GetTextDimension(text, &width, &height, max_width);
	if (fixed_width) {
		width = max_width;
	}
	Rect rect;
	rect = Rect(point.X, point.Y, width, height);
	if (align & 1) {
		rect.X = point.X + (max_width - width) / 2;
	} else if (align & 2) {
		rect.X = point.X - width + max_width;
	}
	FUN_0072A9E0(&rect, -4);
	FUN_00621B80(&rect, surface, 0, 0x9f);
}

namespace OwnerDraw {
void __fastcall DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int a, int flags, int b, int c, int d);
}

inline RGBClass Int_To_RGB(unsigned short color)
{
	return RGBClass(color >> Drawing::RedShiftLeft << Drawing::RedShiftRight, color >> Drawing::GreenShiftLeft << Drawing::GreenShiftRight, color >> Drawing::BlueShiftLeft << Drawing::BlueShiftRight);
}

// FUNCTION: GAMEMD 0x00554280
void __fastcall FUN_00554280(wchar_t const * text, Surface * surface, Rect * rect, unsigned short color, int a)
{
	LTRBStruct bounds;
	bounds.Left = rect->X;
	bounds.Top = rect->Y;
	bounds.Right = rect->X + rect->Width;
	bounds.Bottom = rect->Y + rect->Height;
	RGBClass rgb = Int_To_RGB(color);
	OwnerDraw::DrawWideText(surface, text, &bounds, BitFont::Instance, (((rgb.Get_Blue() << 8) | rgb.Get_Green()) << 8) | rgb.Get_Red(), a, 0xc, 0, 0, 0);
}

// FUNCTION: GAMEMD 0x00554350
void LoadProgressManager::FUN_00554350(Surface * target)
{
	if (ProgressSurface != NULL) {
		target->Blit_From(*ProgressSurface, false, true);
	}
}

// FUNCTION: GAMEMD 0x00554370
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

// FUNCTION: GAMEMD 0x00554400
bool LoadProgressManager::INoticeSink_Unknown(DWORD message, DWORD extra)
{
	if (message == 0x11ae) {
		FUN_00554350(HiddenSurface);
		return true;
	}
	return false;
}
