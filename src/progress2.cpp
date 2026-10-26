// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "progress.h"
#include "gscreen.h"
#include "scheme.h"
#include "surface.h"
#include <string.h>
#include <windows.h>
#undef LoadString
#include "session.h"
#include "msanim.h"
#include "bitfont.h"
#include "dsurface.h"
#include "dialog.h"
#include "draw.h"
#include "hsv.h"

struct FileSystem {
	static ConvertClass * PALETTE_PAL;
};
Surface * __fastcall FUN_004E3560(int index);
wchar_t * __fastcall FUN_00735120(char const * text);
void __fastcall OwnerDraw_GetRectangle(HWND window, Rect * rect);
void __fastcall FUN_0072A9E0(Rect * rect, int amount);
int __fastcall FUN_004355D0(int red, int green, int blue);
void __fastcall FUN_004F4780(bool flag, Surface * surface, bool other);
void __fastcall Font_Cell_Size(int * width, int * height);
extern Surface * DAT_00887310;
extern wchar_t DAT_00887734[];

struct Drawing {
	static int RedShiftLeft;
	static int RedShiftRight;
	static int GreenShiftLeft;
	static int GreenShiftRight;
	static int BlueShiftLeft;
	static int BlueShiftRight;
};

class PCX {
	public:
		bool BlitToSurface(Rect * rect, Surface * dest, Surface * source, int color);
		static PCX Instance;
};

// FUNCTION: GAMEMD 0x006433D0
// ProgressScreenClass::Get_Current_Progress
double ProgressScreenClass::Get_Current_Progress(void) const
{
	double total = 0;
	for (int i = 0; i < PlayerCount; i++) {
		total += PlayerProgress[i];
	}
	return total / PlayerCount / MainProgress;
}

// FUNCTION: GAMEMD 0x00643400
// ProgressScreenClass::Draw_Bar
void ProgressScreenClass::Draw_Bar(Point2D pos, double ratio, Surface * surface, ColorScheme * scheme)
{
	Rect shape = ((SHPStruct *)Buffer)->GetFrameBounds(0);
	shape.X = pos.X + 3;
	shape.Y = pos.Y + 3;
	Rect fill = shape;
	FUN_0072A9E0(&fill, -3);
	ConvertClass * drawer;
	if (scheme == 0) {
		scheme = Fetch_Scheme_By_Name("DarkRed");
		drawer = FileSystem::PALETTE_PAL;
	} else {
		drawer = (ConvertClass *)scheme->Converter;
	}
	if (Field74 != 0) {
		drawer = (ConvertClass *)Field74;
	}
	if (Field71) {
		RGBClass color = scheme->HSV;
		surface->Draw_Rect(fill, ((color.Get_Red() >> (unsigned char)Drawing::RedShiftRight) << Drawing::RedShiftLeft)
			| ((color.Get_Green() >> (unsigned char)Drawing::GreenShiftRight) << Drawing::GreenShiftLeft)
			| ((color.Get_Blue() >> (unsigned char)Drawing::BlueShiftRight) << Drawing::BlueShiftLeft));
	}
	Rect bar = shape;
	bar.Width = (int)(shape.Width * ratio);
	Draw_Shape(*surface, *drawer, (ShapeSet *)Buffer, 0, Point2D(0, 0), bar, ShapeFlags_Type(0x400));
}

// FUNCTION: GAMEMD 0x006435B0
// ProgressScreenClass::Draw_Flag
void ProgressScreenClass::Draw_Flag(int x, int y, Surface * flag)
{
	Surface * surface = FUN_004E3560(0);
	int width;
	int height;
	if (surface == 0) {
		width = 0;
		height = 0;
	} else {
		width = surface->Get_Width();
		height = surface->Get_Height();
	}
	Rect rect(x, y, width, height);
	int color = ((255 >> Drawing::RedShiftRight) << Drawing::RedShiftLeft)
		| ((0 >> Drawing::GreenShiftRight) << Drawing::GreenShiftLeft)
		| ((255 >> Drawing::BlueShiftRight) << Drawing::BlueShiftLeft);
	if (HiddenSurface != 0 && !((DSurface *)HiddenSurface)->Is_Lost()) {
		PCX::Instance.BlitToSurface(&rect, HiddenSurface, flag, color);
	}
}

// FUNCTION: GAMEMD 0x00643670
// ProgressScreenClass::Draw_Label
void ProgressScreenClass::Draw_Label(Point2D pt, wchar_t const * name, ColorScheme * scheme, int width)
{
	BitFont * font = Font_From_TPF(TextPrintType(0x46));
	wchar_t text[16];
	for (int i = 0; i < 15; i++) {
		text[i] = *FUN_00735120("W");
	}
	text[15] = 0;
	int w = font->FUN_00433ED0(text, 0);
	int h = font->field_1C;
	if (width != -1) {
		w = width;
	}
	Point2D scratch;
	Fancy_Text_Print_Wide(&scratch, name, HiddenSurface, &Rect(0, 0, pt.X + w, pt.Y + h), &pt, scheme, 0, TextPrintType(0x19));
}

// FUNCTION: GAMEMD 0x00643AE0
// ProgressScreenClass::Display_Progress
void ProgressScreenClass::Display_Progress(Point2D xpt)
{
	if (IsActive && Buffer != 0) {
		Point2D pt;
		if (Dialog != 0) {
			Rect rect;
			HWND item = GetDlgItem((HWND)Dialog, 0x639);
			OwnerDraw_GetRectangle(item, &rect);
			pt = Point2D(rect.X, rect.Y);
			Draw_Bar(pt, Get_Current_Progress(0), DAT_00887310, 0);
			return;
		}
		pt = xpt;
		if (pt.X == -1 && pt.Y == -1) {
			pt = Pos;
		}
		wchar_t const * name = Field50;
		if (PlayerCount == 1) {
			ColorScheme * scheme;
			if (Session.Type == 0) {
				if (name == 0) {
					name = DAT_00887734;
				}
				if (Side == 0) {
					scheme = Fetch_Scheme_By_Name("AlliedLoad");
				} else {
					scheme = Fetch_Scheme_By_Name("SovietLoad");
				}
			} else {
				if (name == 0) {
					name = Session.Players[0]->Name;
				}
				scheme = Player_Scheme(0);
			}
			Draw_Player(scheme, name, &pt, 0, Field70);
		} else {
			for (int i = 0; i < PlayerCount; i++) {
				ColorScheme * scheme = Player_Scheme(i);
				Draw_Player(scheme, Session.Players[i]->Name, &pt, i, Field70);
			}
		}
		FUN_004F4780(true, HiddenSurface, false);
	}
}

static inline void Fill_Notice(int * data, int before, double progress)
{
	data[0] = before;
	data[1] = (int)(progress * 100.0);
}

// FUNCTION: GAMEMD 0x00643C50
// ProgressScreenClass::Set_Progress_Percent
void ProgressScreenClass::Set_Progress_Percent(int index, double value, Point2D pt)
{
	int before = (int)(Get_Current_Progress() * 100.0);
	double prog1 = PlayerProgress[index];
	PlayerProgress[index] = (MainProgress * 0.01) * value;
	if (PlayerProgress[index] > MainProgress) {
		PlayerProgress[index] = MainProgress;
	}
	if (PlayerProgress[index] != prog1) {
		int data[2];
		Fill_Notice(data, before, Get_Current_Progress());
		if (Callback != 0 && IsActive) {
			Callback->INoticeSink_Unknown(0x11AE, (DWORD)data);
		}
		if (Dialog != 0) {
			SendMessage((HWND)Dialog, WM_PAINT, 0, 0);
		} else {
			Display_Progress(pt);
		}
	}
}

// FUNCTION: GAMEMD 0x00643D60
// ProgressScreenClass::Add_Progress_Percent
void ProgressScreenClass::Add_Progress_Percent(int index, double value, Point2D pt)
{
	int before = (int)(Get_Current_Progress() * 100.0);
	double prog1 = PlayerProgress[index];
	PlayerProgress[index] += (MainProgress * 0.01) * value;
	if (PlayerProgress[index] > MainProgress) {
		PlayerProgress[index] = MainProgress;
	}
	if (PlayerProgress[index] != prog1) {
		if (Dialog != 0) {
			SendMessage((HWND)Dialog, WM_PAINT, 0, 0);
		} else {
			Display_Progress(pt);
		}
	}
	int data[2];
	Fill_Notice(data, before, Get_Current_Progress());
	if (Callback != 0) {
		Callback->INoticeSink_Unknown(0x11AE, (DWORD)data);
	}
}

// FUNCTION: GAMEMD 0x00643E60
// ProgressScreenClass::Has_Dialog
bool ProgressScreenClass::Has_Dialog(void) const
{
	return Dialog != 0;
}

// FUNCTION: GAMEMD 0x00643E70
// ProgressScreenClass::Clear_Dialog
void ProgressScreenClass::Clear_Dialog(void)
{
	Dialog = 0;
}

// FUNCTION: GAMEMD 0x00643E80
// ProgressNotifier::Set_Callback
void ProgressNotifier::Set_Callback(INoticeSink * callback)
{
	Callback = callback;
}

// FUNCTION: GAMEMD 0x00643E90
// ProgressScreenClass::Get_Current_Progress
double ProgressScreenClass::Get_Current_Progress(int index) const
{
	double progress;
	if (index >= PlayerCount) {
		progress = MainProgress;
	} else {
		progress = PlayerProgress[index] / MainProgress;
	}
	return progress;
}
