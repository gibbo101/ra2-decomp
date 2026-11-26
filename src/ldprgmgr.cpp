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
#include "bsurface.h"
#include "rgb.h"

extern int VideoModeWidth;
extern int VideoModeHeight;

extern int DAT_007F5BE0;
extern int DAT_007F5BE4;
extern int DAT_007F5BEC;
extern int DAT_007F5BF0;
extern PKey FastKey;

void __cdecl FUN_0072B460(void);
void __cdecl FUN_0072B870(void);
void __cdecl FUN_004A4AC0(const char * text);
Point2D __fastcall FUN_0072A9C0(Rect const * rect);
void __fastcall FUN_0072A9E0(Rect * rect, int amount);
void __fastcall FUN_0072B3E0(int side);
void __fastcall FUN_0072B530(int country);
void __fastcall ODFillRectTrans(Rect * rect, Surface * surface, unsigned short color, int alpha);

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


// FUNCTION: GAMEMD 0x00552B10
void LoadProgressManager::Compute_Rects(void)
{
	int render_width = VideoModeWidth;
	bool low_res = render_width == 640;
	int width = low_res ? DAT_007F5BE0 : DAT_007F5BE4;
	if (Session.Type == GAME_NORMAL) {
		int height = (low_res ? DAT_007F5BEC : DAT_007F5BF0) - 80;
		int x = (render_width - width) / 2;
		int y = (VideoModeHeight - height - 80) / 2;
		LoadScreenSHPRect.Width = LoadBarSHPRect.Width = TitleBarRect.Width = width;
		LoadBarSHPRect.X = TitleBarRect.X = x;
		TitleBarRect.Y = y;
		TitleBarRect.Height = 40;
		y += 40;
		LoadBarSHPRect.Y = y;
		LoadScreenSHPRect.X = x;
		LoadBarSHPRect.Height = height;
		y += height;
		LoadScreenSHPRect.Y = y;
		LoadScreenSHPRect.Height = 40;
	} else {
		int height = low_res ? DAT_007F5BEC : DAT_007F5BF0;
		LoadBarSHPRect.Set((render_width - width) / 2, (VideoModeHeight - height) / 2, width, height);
	}
}

// FUNCTION: GAMEMD 0x00552BE0
Point2D LoadProgressManager::Text_Origin(void)
{
	Point2D p;
	if (Session.Type == GAME_NORMAL) {
		p.X = LoadScreenSHPRect.X;
		if (VideoModeWidth == DAT_007F5BE0) {
			p.X += 0x54;
		} else {
			p.X += 0xa4;
		}
		p.Y = LoadScreenSHPRect.Y + 7;
	} else {
		p = FUN_0072A9C0(&LoadBarSHPRect);
		if (VideoModeWidth == DAT_007F5BE0) {
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
		result = 0x146 + ((VideoModeWidth != DAT_007F5BE0) ? 0x50 : 0);
	}
	return result;
}

// FUNCTION: GAMEMD 0x00552CC0
void LoadProgressManager::Load_Mixes(void)
{
	if (LoadMDMix == NULL) {
		FUN_004A4AC0(" loadmd.MIX");
		LoadMDMix = new MixFileClass("LOADMD.MIX", &FastKey);
		if (LoadMDMix == NULL) {
			return;
		}
	}
	if (LoadMix == NULL) {
		LoadMix = new MixFileClass("LOAD.MIX", &FastKey);
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

#include <string.h>
#include <stdio.h>
#include <wchar.h>
#undef LoadString
#include "stringtable.h"
#include "draw.h"
#include "dialog.h"
#include "scheme.h"
#include "hsv.h"
#include "progress.h"
#include "preview.h"
#include "ccini.h"
#include "ccfile.h"
#include "coopcamp.h"
#include "multiplayer.h"

extern char const * DAT_008297A8;
extern char const * DAT_008297AC;
extern char const * DAT_008297B0;
extern char const * DAT_008297B4;
extern char const * DAT_008297B8;
extern char const * DAT_008297BC;
extern char const * DAT_008297C0;
extern char const * DAT_008297C4;
extern char const * DAT_008297C8;
extern char const * DAT_008297CC;
extern char const * DAT_008297D0;
extern char const * DAT_008297D4;
extern char const * DAT_008297D8;

void * __fastcall FileSystem_LoadWholeFileEx(char const * name, bool & allocated);
char const * __fastcall FUN_0072B2F0(int width, int height, int side);
char const * __fastcall FUN_0072B310(int width, int height, int side);
ConvertClass * FUN_0072B340(void);
ConvertClass * FUN_0072B360(void);
ConvertClass * FUN_0072B380(void);
void __fastcall FUN_0072B390(char const * name);
ConvertClass * FUN_0072B500(void);
ConvertClass * FUN_0072B520(void);
void __fastcall FUN_0072B820(char const * name);

// FUNCTION: GAMEMD 0x00554100
inline void __fastcall FUN_00554100(Rect * rect)
{
	int width = VideoModeWidth;
	int box_width = DAT_007F5BE4;
	if (width >= box_width) {
		int dy = (VideoModeHeight - DAT_007F5BF0) / 2;
		int dx = (width - box_width) / 2;
		rect->X += dx;
		rect->Y += dy;
	}
}

// FUNCTION: GAMEMD 0x00554150
Rect __fastcall FUN_00554150(void)
{
	Rect rect;
	if (VideoModeWidth == DAT_007F5BE0) {
		rect = Rect(0x10, 0xeb, 200, 20);
	} else {
		rect = Rect(0x14, 0x12c, 200, 20);
	}
	FUN_00554100(&rect);
	return rect;
}

// FUNCTION: GAMEMD 0x005541C0
inline void __fastcall FUN_005541C0(wchar_t const * text, Surface * surface, Point2D point, int max_width, unsigned char align, bool fixed_width)
{
	int width = 0;
	int height = 0;
	BitFont::Instance->GetTextDimension(text, &width, &height, max_width);
	if (fixed_width) {
		width = max_width;
	}
	Rect area(point.X, point.Y, width, height);
	Rect rect(area);
	if (align & 1) {
		rect.X = point.X + (max_width - width) / 2;
	} else if (align & 2) {
		rect.X = point.X - width + max_width;
	}
	FUN_0072A9E0(&rect, -4);
	ODFillRectTrans(&rect, surface, 0, 0x9f);
}

namespace OwnerDraw {
void __fastcall DrawWideText(Surface * surface, wchar_t const * text, LTRBStruct * rect, BitFont * font, unsigned color, int a, int flags, int b, int c, int d);
}

inline RGBClass Int_To_RGB(unsigned short color)
{
	return RGBClass(color >> DSurface::RedRight << DSurface::RedLeft, color >> DSurface::GreenRight << DSurface::GreenLeft, color >> DSurface::BlueRight << DSurface::BlueLeft);
}

// FUNCTION: GAMEMD 0x00554280
inline void __fastcall FUN_00554280(wchar_t const * text, Surface * surface, Rect * rect, unsigned short color, int a)
{
	BitFont * font = BitFont::Instance;
	LTRBStruct bounds;
	bounds.Left = rect->X;
	bounds.Top = rect->Y;
	bounds.Right = rect->X + rect->Width;
	bounds.Bottom = rect->Y + rect->Height;
	RGBClass rgb = Int_To_RGB(color);
	OwnerDraw::DrawWideText(surface, text, &bounds, font, (((rgb.Get_Blue() << 8) | rgb.Get_Green()) << 8) | rgb.Get_Red(), a, 0xc, 0, 0, 0);
}

static inline bool Is_Observer(void)
{
	return (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.Players[0]->Slot.SpectatorFlag == -1;
}

static void Background_Name(char * name, int country)
{
	char const * res = (VideoModeWidth == DAT_007F5BE0) ? DAT_008297D4 : DAT_008297D8;
	if (Is_Observer()) {
		sprintf(name, DAT_008297CC, res);
	} else if (country == 0) {
		sprintf(name, DAT_008297C8, res);
	} else if (country == 1) {
		sprintf(name, DAT_008297BC, res);
	} else if (country == 2) {
		sprintf(name, DAT_008297B0, res);
	} else if (country == 3) {
		sprintf(name, DAT_008297B4, res);
	} else if (country == 4) {
		sprintf(name, DAT_008297A8, res);
	} else if (country == 5) {
		sprintf(name, DAT_008297C0, res);
	} else if (country == 6) {
		sprintf(name, DAT_008297B8, res);
	} else if (country == 7) {
		sprintf(name, DAT_008297AC, res);
	} else if (country == 8) {
		sprintf(name, DAT_008297C4, res);
	} else if (country == 9) {
		sprintf(name, DAT_008297D0, res);
	}
}

static wchar_t const * Country_Name(int country)
{
	wchar_t const * name;
	if (Is_Observer()) {
		name = StringTable::LoadString("Name:Observer", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xb2);
	} else if (country == 0) {
		name = StringTable::LoadString("Name:Americans", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xb7);
	} else if (country == 1) {
		name = StringTable::LoadString("Name:Alliance", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xb9);
	} else if (country == 2) {
		name = StringTable::LoadString("Name:French", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xbb);
	} else if (country == 3) {
		name = StringTable::LoadString("Name:Germans", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xbd);
	} else if (country == 4) {
		name = StringTable::LoadString("Name:British", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xbf);
	} else if (country == 5) {
		name = StringTable::LoadString("Name:Africans", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xc1);
	} else if (country == 6) {
		name = StringTable::LoadString("Name:Arabs", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xc3);
	} else if (country == 7) {
		name = StringTable::LoadString("Name:Confederation", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xc5);
	} else if (country == 8) {
		name = StringTable::LoadString("Name:Russians", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xc7);
	} else if (country == 9) {
		name = StringTable::LoadString("Name:YuriCountry", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xc9);
	} else {
		name = NULL;
	}
	return name;
}

static wchar_t const * Country_Unit_Name(int country)
{
	wchar_t const * name;
	if (country == 0) {
		name = StringTable::LoadString("Name:Para", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xf7);
	} else if (country == 1) {
		name = StringTable::LoadString("Name:BEAGLE", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xf9);
	} else if (country == 2) {
		name = StringTable::LoadString("Name:GTGCAN", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xfb);
	} else if (country == 3) {
		name = StringTable::LoadString("Name:TNKD", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xfd);
	} else if (country == 4) {
		name = StringTable::LoadString("Name:SNIPE", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0xff);
	} else if (country == 5) {
		name = StringTable::LoadString("Name:DTRUCK", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x101);
	} else if (country == 6) {
		name = StringTable::LoadString("Name:DESO", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x103);
	} else if (country == 7) {
		name = StringTable::LoadString("Name:TERROR", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x105);
	} else if (country == 8) {
		name = StringTable::LoadString("Name:TTNK", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x107);
	} else if (country == 9) {
		name = StringTable::LoadString("Name:YURI", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x109);
	} else {
		name = NULL;
	}
	return name;
}

static wchar_t const * Country_Brief(int country)
{
	wchar_t const * brief;
	if (country == 0) {
		brief = StringTable::LoadString("LoadBrief:USA", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x12a);
	} else if (country == 1) {
		brief = StringTable::LoadString("LoadBrief:Korea", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x12c);
	} else if (country == 2) {
		brief = StringTable::LoadString("LoadBrief:French", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x12e);
	} else if (country == 3) {
		brief = StringTable::LoadString("LoadBrief:Germans", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x130);
	} else if (country == 4) {
		brief = StringTable::LoadString("LoadBrief:British", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x132);
	} else if (country == 5) {
		brief = StringTable::LoadString("LoadBrief:Lybia", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x134);
	} else if (country == 6) {
		brief = StringTable::LoadString("LoadBrief:Iraq", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x136);
	} else if (country == 7) {
		brief = StringTable::LoadString("LoadBrief:Cuba", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x138);
	} else if (country == 8) {
		brief = StringTable::LoadString("LoadBrief:Russia", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x13a);
	} else if (country == 9) {
		brief = StringTable::LoadString("LoadBrief:YuriCountry", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x13c);
	} else {
		brief = NULL;
	}
	return brief;
}

static Rect Coop_Brief_Rect(void)
{
	Rect rect;
	if (VideoModeWidth == DAT_007F5BE0) {
		rect.X = 0x10;
		rect.Y = 0x7e;
		rect.Width = 0x13e;
		rect.Height = 0x68;
	} else {
		rect.X = 0x14;
		rect.Y = 0x17c;
		rect.Width = 0x18e;
		rect.Height = 0x82;
	}
	FUN_00554100(&rect);
	return rect;
}

static void Upper_Case(wchar_t * text)
{
	for (; *text; text++) {
		if ((*text >= 'a' && *text <= 'z') || (*text >= 0xe0 && *text <= 0xfe)) {
			*text -= 0x20;
		}
	}
}

static Rect Brief_Rect(void)
{
	Rect rect;
	if (VideoModeWidth == DAT_007F5BE0) {
		rect.X = 0x10;
		rect.Y = 0x7e;
		rect.Width = 0x13e;
		rect.Height = 0x68;
	} else {
		rect.X = 0x14;
		rect.Y = 0x9e;
		rect.Width = 0x18e;
		rect.Height = 0x82;
	}
	FUN_00554100(&rect);
	return rect;
}

// FUNCTION: GAMEMD 0x00552D60
void LoadProgressManager::Draw(void)
{
	bool low_res = (VideoModeWidth == DAT_007F5BE0);
	ProgressSurface = new BSurface(VideoModeWidth, VideoModeHeight, 2);
	if (ProgressSurface == NULL) {
		return;
	}
	int color = DSurface::Build_Hicolor_Pixel(BlackColor);
	Rect rect(0, 0, 0, 0);
	rect.Width = VideoModeWidth;
	rect.Height = VideoModeHeight;
	ProgressSurface->Fill_Rect(rect, rect, color);

	if (Session.Type == GAME_NORMAL) {
		char const * background = low_res ? Scen->LS640BkgdName : Scen->LS800BkgdName;
		if (*background == '\0') {
			return;
		}
		FUN_0072B390(Scen->LS800BkgdPal);
		LoadMessage = NULL;
		if (*Scen->LSLoadMessage != '\0') {
			LoadMessage = StringTable::LoadString(Scen->LSLoadMessage, NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x297);
		}
		LoadBriefing = NULL;
		if (*Scen->LSBrief != '\0') {
			LoadBriefing = StringTable::LoadString(Scen->LSBrief, NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x29b);
		}
		int side = Progress.Get_Side();
		char const * titlebar = FUN_0072B2F0(VideoModeWidth, VideoModeHeight, side);
		char const * loadbar = FUN_0072B310(VideoModeWidth, VideoModeHeight, side);
		TitleBarSHP = (SHPStruct *)FileSystem_LoadWholeFileEx(titlebar, TitleBarSHP_loaded);
		LoadBarSHP = (SHPStruct *)FileSystem_LoadWholeFileEx(loadbar, LoadBarSHP_loaded);
		LoadScreenSHP = (SHPStruct *)FileSystem_LoadWholeFileEx(background, LoadScreenSHP_loaded);
		ConvertClass * titlebar_drawer = FUN_0072B340();
		ConvertClass * screen_drawer = FUN_0072B360();
		ConvertClass * loadbar_drawer = FUN_0072B380();

		Point2D pt(TitleBarRect.X, TitleBarRect.Y);
		if (TitleBarSHP != NULL) {
			Draw_Shape(*ProgressSurface, *titlebar_drawer, (ShapeSet *)TitleBarSHP, 0, pt, rect, SHAPE_WIN_REL);
		} else {
			ProgressSurface->Fill_Rect(rect, TitleBarRect, color);
		}
		pt.Y = LoadBarSHPRect.Y;
		Draw_Shape(*ProgressSurface, *screen_drawer, (ShapeSet *)LoadScreenSHP, 0, pt, rect, SHAPE_WIN_REL);
		pt.Y = LoadScreenSHPRect.Y;
		if (LoadBarSHP != NULL) {
			Draw_Shape(*ProgressSurface, *loadbar_drawer, (ShapeSet *)LoadBarSHP, 0, pt, rect, SHAPE_WIN_REL);
		} else {
			ProgressSurface->Fill_Rect(rect, LoadScreenSHPRect, color);
		}

		pt.X = TitleBarRect.X + 10;
		pt.Y = TitleBarRect.Y + 10;
		ColorScheme * scheme = Progress.Side_Scheme();
		if (LoadMessage != NULL) {
			Fancy_Text_Print_Wide(LoadMessage, ProgressSurface, &rect, &pt, scheme, 0, TextPrintType(TPF_EFNT|TPF_NOSHADOW));
		}
		if (LoadBriefing != NULL) {
			if (low_res) {
				pt.X = Scen->LS640BriefLocX + LoadBarSHPRect.X;
				pt.Y = Scen->LS640BriefLocY + LoadBarSHPRect.Y;
			} else {
				pt.X = Scen->LS800BriefLocX + LoadBarSHPRect.X;
				pt.Y = Scen->LS800BriefLocY + LoadBarSHPRect.Y;
			}
			wchar_t buffer[2048];
			wcscpy(buffer, LoadBriefing);
			RGBClass rgb = scheme->HSV;
			int text_color = DSurface::Build_Hicolor_Pixel(rgb);
			int width = 0;
			int height = 0;
			BitFont::Instance->GetTextDimension(buffer, &width, &height, 400);
			Rect text_rect;
			text_rect = Rect(pt.X, pt.Y, width, height);
			FUN_005541C0(buffer, ProgressSurface, pt, 400, 0, false);
			FUN_00554280(buffer, ProgressSurface, &text_rect, text_color, 0);
		}
		return;
	} else {
		int country = Session.Players[0]->Get_Country();
		char name[32];
		ConvertClass * drawer;
		bool found = false;
		if (Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
			CoopCampEntry * entry = NULL;
			DAT_0089F518.FUN_0049E400(Session.ScenarioFileName, &entry);
			if (entry->Text48.length() != 0 && entry->Text44.length() != 0) {
				FUN_0072B820(entry->Text48.get());
				drawer = FUN_0072B520();
				strcpy(name, entry->Text44.get());
				found = true;
			}
		}
		if (found == false) {
			Background_Name(name, country);
			drawer = FUN_0072B500();
		}
		LoadScreenSHP = (SHPStruct *)FileSystem_LoadWholeFileEx(name, LoadScreenSHP_loaded);
		CCINIClass ini;
		CCFileClass file(Session.ScenarioFileName);
		ini.ReadCCFile(file, true, false);
		MapPreviewClass * preview = new MapPreviewClass;
		if (Scen->IsRandom) {
			preview->Read_PCX_Preview("RandMap.img");
		} else {
			preview->Read_INI_Preview(Session.ScenarioFileName);
		}
		Point2D origin = FUN_0072A9C0(&LoadBarSHPRect);
		if (drawer != NULL && LoadScreenSHP != NULL) {
			Draw_Shape(*ProgressSurface, *drawer, (ShapeSet *)LoadScreenSHP, 0, origin, rect, SHAPE_WIN_REL);
		}
		ColorScheme * scheme = Progress.Loading_Scheme();
		RGBClass rgb = scheme->HSV;
		int text_color = DSurface::Build_Hicolor_Pixel(rgb);
		preview->DrawMap(*ProgressSurface);
		delete preview;

		wchar_t const * country_name = Country_Name(country);
		Rect name_rect;
		if (VideoModeWidth == DAT_007F5BE0) {
			name_rect.X = 0x181;
			name_rect.Y = 0x1b4;
		} else {
			name_rect.X = 0x21c;
			name_rect.Y = 0x136;
		}
		name_rect.Width = 200;
		name_rect.Height = 20;
		FUN_00554100(&name_rect);
		Rect box(name_rect);
		origin = FUN_0072A9C0(&box);
		FUN_005541C0(country_name, ProgressSurface, origin, box.Width, 2, false);
		FUN_00554280(country_name, ProgressSurface, &box, text_color, 2);

		if (!Is_Observer()) {
			wchar_t unit_name[128];
			wcscpy(unit_name, Country_Unit_Name(country));
			Upper_Case(unit_name);
			Rect unit_rect;
			if (VideoModeWidth == DAT_007F5BE0) {
				unit_rect.X = 0x10;
				unit_rect.Y = 0x48;
			} else {
				unit_rect.X = 0x14;
				unit_rect.Y = 0x5a;
			}
			unit_rect.Width = 200;
			unit_rect.Height = 20;
			FUN_00554100(&unit_rect);
			box = unit_rect;
			int black = DSurface::Build_Hicolor_Pixel(BlackColor);
			FUN_00554280(unit_name, ProgressSurface, &box, black, 0);

			wchar_t const * brief = Country_Brief(country);
			if (Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
				box = Coop_Brief_Rect();
			} else {
				box = Brief_Rect();
			}
			origin = FUN_0072A9C0(&box);
			FUN_005541C0(brief, ProgressSurface, origin, box.Width, 0, true);
			FUN_00554280(brief, ProgressSurface, &box, text_color, 0);
		}

		wchar_t const * loading = StringTable::LoadString("GUI:LoadingEx", NULL, "D:\\ra2mdpost\\LdPrgMgr.cpp", 0x365);
		box = FUN_00554150();
		origin = FUN_0072A9C0(&box);
		FUN_005541C0(loading, ProgressSurface, origin, box.Width, 0, false);
		FUN_00554280(loading, ProgressSurface, &box, text_color, 0);
	}
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
