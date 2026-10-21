// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "progress.h"
#include "gscreen.h"
#include "scheme.h"
#include "surface.h"
#include <string.h>
#include "session.h"
#include "msanim.h"
#include "bitfont.h"

struct FileSystem {
	static void * __fastcall LoadWholeFileEx(char const * name, bool & allocated);
};
void __fastcall Load_Title_Page(char const * name, bool visible);
Surface * __fastcall FUN_004E3560(int index);
Rect __fastcall FUN_0072AD20(void);
BitFont * __fastcall FUN_004A60D0(int flags);
wchar_t * __fastcall FUN_00735120(char const * text);

// SYNTHETIC: GAMEMD 0x00642790 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006427C0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006427E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00642800 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00642820 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00642840 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00642860 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006428A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006428D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00642900 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00642930 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00642940 SYMBOL
// _$E35

ProgressScreenClass Progress;

// SYNTHETIC: GAMEMD 0x00642960 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00642980 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x006429A0 SYMBOL
// _$E78
// FUNCTION: GAMEMD 0x006429E0
// ProgressScreenClass::ProgressScreenClass
ProgressScreenClass::ProgressScreenClass(void) :
	MainProgress(0), Field50(0), Buffer(0), BufferOwned(false), Field5C(0), IsActive(false),
	PlayerCount(0), Field62(false), Dialog(0), Pos(0, 0), Field70(false),
	Field78(-1), Percentage(0), Side(-1)
{
	int count = MAX_PLAYERS;
	for (int i = 0; i < count; i++) {
		PlayerProgress[i] = 0;
	}
}

// FUNCTION: GAMEMD 0x00642A30
// ProgressScreenClass::~ProgressScreenClass
ProgressScreenClass::~ProgressScreenClass(void)
{
	if (BufferOwned) {
		delete Buffer;
		BufferOwned = false;
	}
	Buffer = 0;
}

// FUNCTION: GAMEMD 0x00642A60
// ProgressScreenClass::Initialize
void ProgressScreenClass::Initialize(double progress, char count, void * dialog)
{
	MainProgress = progress;
	PlayerCount = count;
	for (int i = 0; i < PlayerCount; i++) {
		PlayerProgress[i] = 0;
	}
	IsActive = true;
	if (dialog != 0) {
		Dialog = dialog;
	} else {
		HiddenSurface->Fill(0);
	}
	Field50 = 0;
	Percentage = -1;
}

// FUNCTION: GAMEMD 0x00642AD0
// ProgressScreenClass::End
void ProgressScreenClass::End(void)
{
	IsActive = false;
}

// FUNCTION: GAMEMD 0x00642AE0
// ProgressScreenClass::Free_Buffer
void ProgressScreenClass::Free_Buffer(void)
{
	if (BufferOwned) {
		delete Buffer;
		BufferOwned = false;
	}
	Buffer = 0;
}

// FUNCTION: GAMEMD 0x00642B10
// ProgressScreenClass::Set_Side
void ProgressScreenClass::Set_Side(int side)
{
	Side = side;
}

// FUNCTION: GAMEMD 0x00642B20
// ProgressScreenClass::Get_Side
int ProgressScreenClass::Get_Side(void) const
{
	return Side;
}

// FUNCTION: GAMEMD 0x00642B30
// ProgressScreenClass::Side_Scheme
ColorScheme * ProgressScreenClass::Side_Scheme(void) const
{
	if (Side == 0) {
		return Fetch_Scheme_By_Name("AlliedLoad");
	}
	return Fetch_Scheme_By_Name("SovietLoad");
}

// FUNCTION: GAMEMD 0x00642B60
// ProgressScreenClass::Loading_Scheme
ColorScheme * ProgressScreenClass::Loading_Scheme(void) const
{
	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		if (Session.Players[0]->Slot.SpectatorFlag == -1) {
			return Fetch_Scheme_By_Name("LightGrey");
		}
	}
	if (Side == 0) {
		return Fetch_Scheme_By_Name("AlliedLoad");
	}
	return Fetch_Scheme_By_Name("SovietLoad");
}

// FUNCTION: GAMEMD 0x00642BB0
// ProgressScreenClass::Player_Scheme
ColorScheme * ProgressScreenClass::Player_Scheme(int index)
{
	ColorScheme * result = 0;
	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		if (Session.Players[index]->Slot.SpectatorFlag == -1) {
			int scheme = Session.Color_Index_To_Scheme(-2);
			return ColorSchemes[scheme];
		}
	}
	if (index >= 0 && index < 8) {
		result = ColorSchemes[Session.Color_Index_To_Scheme(Session.Players[index]->Get_Color())];
	}
	return result;
}

// FUNCTION: GAMEMD 0x00642C20
// ProgressScreenClass::Set_Graphic_Data
void ProgressScreenClass::Set_Graphic_Data(char const * progbar, char const * background, int string)
{
	if (progbar != 0) {
		if (Buffer != 0) {
			if (BufferOwned) {
				delete Buffer;
				BufferOwned = false;
			}
			Buffer = 0;
		}
		Buffer = FileSystem::LoadWholeFileEx(progbar, BufferOwned);
		if (Buffer != 0) {
			if (background != 0) {
				Field5C = (int)background;
				Load_Title_Page(background, false);
			}
			Field74 = string;
		}
	}
}

// FUNCTION: GAMEMD 0x00642DF0
// ProgressScreenClass::Set_Field78
void ProgressScreenClass::Set_Field78(int value)
{
	Field78 = value;
}

// FUNCTION: GAMEMD 0x00642E00
// ProgressScreenClass::Bar_Size
void ProgressScreenClass::Bar_Size(int * width, int * height)
{
	Rect rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	*width = rect.Width + 6;
	*height = rect.Height + 6;
}

// FUNCTION: GAMEMD 0x00642E40
// ProgressScreenClass::Dialog_Size
void ProgressScreenClass::Dialog_Size(int * width, int * height)
{
	Surface * surface = FUN_004E3560(0);
	if (surface == 0) {
		*width = 0;
		*height = 0;
		return;
	}
	*width = surface->Get_Width();
	*height = surface->Get_Height();
}

// FUNCTION: GAMEMD 0x00642E80
// Font_Cell_Size
void __fastcall Font_Cell_Size(int * width, int * height)
{
	BitFont * font = FUN_004A60D0(0x46);
	wchar_t text[16];
	for (int i = 0; i < 15; i++) {
		text[i] = *FUN_00735120("W");
	}
	text[15] = 0;
	*width = font->FUN_00433ED0(text, 0);
	*height = font->field_1C;
}

// FUNCTION: GAMEMD 0x00642EF0
// ProgressScreenClass::Fit_Size
void ProgressScreenClass::Fit_Size(int * width, int * height, int count, bool usedialog)
{
	Rect rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	int barw = rect.Width + 6;
	int barh = rect.Height + 6;
	int dw = 0;
	int dh = 0;
	if (usedialog) {
		Dialog_Size(&dw, &dh);
	}
	int tw, th;
	Font_Cell_Size(&tw, &th);
	*width = tw + dw + barw + 20;
	*height = (barh > dh) ? barh : dh;
	*height = (*height > th) ? *height : th;
	*width += 10;
	*height += 4;
	if (count > 1) {
		*height *= count;
	}
}

// FUNCTION: GAMEMD 0x00642FF0
// ProgressScreenClass::Total_Height
int ProgressScreenClass::Total_Height(int start, int count, bool usedialog)
{
	Rect rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	int barh = rect.Height + 6;
	int dw = 0;
	int dh = 0;
	if (usedialog) {
		Dialog_Size(&dw, &dh);
	}
	int tw, th;
	Font_Cell_Size(&tw, &th);
	int h = (barh > dh) ? barh : dh;
	h = (h > th) ? h : th;
	return (h + 4) * count + start;
}

// FUNCTION: GAMEMD 0x006430C0
// ProgressScreenClass::Bar_Y_Offset
int ProgressScreenClass::Bar_Y_Offset(bool usedialog)
{
	Rect rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	int barh = rect.Height + 6;
	int dw = 0;
	int dh = 0;
	if (usedialog) {
		Dialog_Size(&dw, &dh);
	}
	int tw, th;
	Font_Cell_Size(&tw, &th);
	int h = (barh > dh) ? barh : dh;
	h = (h > th) ? h : th;
	rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	return (h - rect.Height - 2) / 2;
}

// FUNCTION: GAMEMD 0x006431C0
// ProgressScreenClass::Label_Y_Offset
int ProgressScreenClass::Label_Y_Offset(bool usedialog)
{
	Rect rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	int barh = rect.Height + 6;
	int dw = 0;
	int dh = 0;
	if (usedialog) {
		Dialog_Size(&dw, &dh);
	}
	int tw, th;
	Font_Cell_Size(&tw, &th);
	int h = (barh > dh) ? barh : dh;
	h = (h > th) ? h : th;
	h += 4;
	Dialog_Size(&dw, &dh);
	return (h - dh) / 2;
}

// FUNCTION: GAMEMD 0x006432C0
// ProgressScreenClass::Label_X_Offset
int ProgressScreenClass::Label_X_Offset(bool usedialog)
{
	Rect rect = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	int barh = rect.Height + 6;
	int dw = 0;
	int dh = 0;
	if (usedialog) {
		Dialog_Size(&dw, &dh);
	}
	int tw, th;
	Font_Cell_Size(&tw, &th);
	int h = (barh > dh) ? barh : dh;
	h = (h > th) ? h : th;
	h += 4;
	Font_Cell_Size(&tw, &th);
	return (h - th) / 2;
}
