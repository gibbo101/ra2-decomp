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
#include <windows.h>
#undef LoadString
#include "session.h"
#include "msanim.h"
#include "bitfont.h"
#include "dsurface.h"
#include "dialog.h"
#include "draw.h"
#include "hsv.h"
#include "_convert.h"

// SYNTHETIC: GAMEMD 0x00642770 SYMBOL
// _$E41

struct FileSystem {
	static void * __fastcall LoadWholeFileEx(char const * name, bool & allocated);
};
void __fastcall Load_Title_Page(char const * name, bool visible);
Surface * __fastcall FUN_004E3560(int index);
Rect __fastcall Get_Title_Screen_Area(void);
BitFont * __fastcall FUN_004A60D0(int flags);
wchar_t * __fastcall FUN_00735120(char const * text);
void __fastcall OwnerDraw_GetRectangle(HWND window, Rect * rect);
void __fastcall FUN_0072A9E0(Rect * rect, int amount);
void __fastcall FUN_004F4780(bool flag, Surface * surface, bool other);
extern Surface * AlternateSurface;
extern wchar_t DAT_00887734[];

#include "srfcache.h"

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
// _$E98
// SYNTHETIC: GAMEMD 0x006429A0 SYMBOL
// _$E96
// FUNCTION: GAMEMD 0x006429E0
// ProgressScreenClass::ProgressScreenClass
ProgressScreenClass::ProgressScreenClass(void) :
	MainProgress(0), String(0), Buffer(0), BufferOwned(false), Background(0), IsActive(false),
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
	String = 0;
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

static inline bool Is_Spectator(int index)
{
	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		if (Session.Players[index]->Slot.SpectatorFlag == -1) {
			return true;
		}
	}
	return false;
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
	if (Is_Spectator(0)) {
		return Fetch_Scheme_By_Name("LightGrey");
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
	if (index >= 0 && index < MAX_PLAYERS) {
		int color = Session.Players[index]->Get_Color();
		int scheme = Session.Color_Index_To_Scheme(color);
		result = ColorSchemes[scheme];
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
				Background = (int)background;
				Load_Title_Page(background, false);
			}
			Field74 = string;
		}
	}
}

// FUNCTION: GAMEMD 0x00642C80
// ProgressScreenClass::Set_Layout
void ProgressScreenClass::Set_Layout(Point2D pt, wchar_t const * title, bool usedialog, bool flag)
{
	if (pt.X == -1 && pt.Y == -1) {
		int width, height;
		Fit_Size(&width, &height, PlayerCount, usedialog);
		Rect rect = Get_Title_Screen_Area();
		Pos = Point2D((rect.Width - width) / 2, (rect.Height - height) / 2);
	} else {
		Pos.X = pt.X;
		Pos.Y = pt.Y;
	}
	String = title;
	Field70 = usedialog;
	Field71 = flag;
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
	int barw = 0;
	int barh = 0;
	Bar_Size(&barw, &barh);
	int dw = 0;
	int dh = 0;
	if (usedialog) {
		Dialog_Size(&dw, &dh);
	}
	int tw = 0;
	int th = 0;
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
	int width = 0;
	int height = 0;
	Fit_Size(&width, &height, 1, usedialog);
	return height * count + start;
}

// FUNCTION: GAMEMD 0x006430C0
// ProgressScreenClass::Bar_Y_Offset
int ProgressScreenClass::Bar_Y_Offset(bool usedialog)
{
	int width = 0;
	int height = 0;
	Fit_Size(&width, &height, 1, usedialog);
	int barw = 0;
	int barh = 0;
	Bar_Size(&barw, &barh);
	return (height - barh) / 2;
}

// FUNCTION: GAMEMD 0x006431C0
// ProgressScreenClass::Label_Y_Offset
int ProgressScreenClass::Label_Y_Offset(bool usedialog)
{
	int width = 0;
	int height = 0;
	Fit_Size(&width, &height, 1, usedialog);
	int dw = 0;
	int dh = 0;
	Dialog_Size(&dw, &dh);
	return (height - dh) / 2;
}

// FUNCTION: GAMEMD 0x006432C0
// ProgressScreenClass::Label_X_Offset
int ProgressScreenClass::Label_X_Offset(bool usedialog)
{
	int width = 0;
	int height = 0;
	Fit_Size(&width, &height, 1, usedialog);
	int tw = 0;
	int th = 0;
	Font_Cell_Size(&tw, &th);
	return (height - th) / 2;
}

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
		drawer = NormalDrawer;
	} else {
		drawer = (ConvertClass *)scheme->Converter;
	}
	if (Field74 != 0) {
		drawer = (ConvertClass *)Field74;
	}
	if (Field71) {
		RGBClass color = scheme->HSV;
		surface->Draw_Rect(fill, ((color.Get_Red() >> (unsigned char)DSurface::RedLeft) << DSurface::RedRight)
			| ((color.Get_Green() >> (unsigned char)DSurface::GreenLeft) << DSurface::GreenRight)
			| ((color.Get_Blue() >> (unsigned char)DSurface::BlueLeft) << DSurface::BlueRight));
	}
	Rect bar = shape;
	bar.Width = (int)(shape.Width * ratio);
	Draw_Shape(*surface, *drawer, (ShapeSet *)Buffer, 0, Point2D(0, 0), bar, ShapeFlags_Type(0x400));
}

// FUNCTION: GAMEMD 0x006435B0
// ProgressScreenClass::Draw_Flag
void ProgressScreenClass::Draw_Flag(int x, int y, Surface * flag)
{
	int width = 0;
	int height = 0;
	Dialog_Size(&width, &height);
	Rect rect(x, y, width, height);
	int color = DSurface::Build_Hicolor_Pixel(255, 0, 255);
	if (HiddenSurface != 0 && !((DSurface *)HiddenSurface)->Is_Lost()) {
		SurfaceCache.DrawTrans(rect, *HiddenSurface, *flag, color);
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
	Rect bounds(0, 0, pt.X + w, pt.Y + h);
	Fancy_Text_Print_Wide(name, HiddenSurface, &bounds, &pt, scheme, 0, TextPrintType(0x19));
}

// FUNCTION: GAMEMD 0x00643720
// ProgressScreenClass::Draw_Player
void ProgressScreenClass::Draw_Player(ColorScheme * scheme, wchar_t const * name, Point2D * pos, int index, bool usedialog)
{
	double ratio = Get_Current_Progress(index);
	int y = Total_Height(pos->Y, index, usedialog);
	int width = 0;
	int height = 0;
	Fit_Size(&width, &height, 1, usedialog);
	if (Field78 != -1) {
		width = Field78;
	}
	int barw;
	int barh;
	Bar_Size(&barw, &barh);
	Point2D barpos = *pos;
	int bary = Bar_Y_Offset(usedialog);
	barpos.X += 5;
	bary += y;
	Point2D labelpos(pos->X, pos->Y);
	labelpos.X += barw + 15;
	Point2D flagpos = labelpos;
	if (usedialog) {
		flagpos.Y = y + Label_Y_Offset(usedialog);
	}
	Point2D label = labelpos;
	int yoff = Label_X_Offset(usedialog);
	if (usedialog) {
		int dw;
		int dh;
		Dialog_Size(&dw, &dh);
		label.X = labelpos.X + dw + 10;
	}
	label.Y = y + yoff;
	Draw_Bar(Point2D(barpos.X, bary), ratio, HiddenSurface, scheme);
	if (usedialog) {
		int country = Session.Players[index]->Get_Country();
		if (Is_Spectator(index)) {
			country = -3;
		}
		Surface * flag = FUN_004E3560(country);
		if (flag != 0) {
			Draw_Flag(flagpos.X, flagpos.Y, flag);
		}
	}
	Draw_Label(label, name, scheme, pos->X - label.X + width - 3);
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
			Draw_Bar(pt, Get_Current_Progress(0), AlternateSurface, 0);
			return;
		}
		pt = xpt;
		if (pt.X == -1 && pt.Y == -1) {
			pt = Pos;
		}
		wchar_t const * name = String;
		if (PlayerCount == 1) {
			ColorScheme * scheme;
			if (Session.Type == 0) {
				if (name == 0) {
					name = DAT_00887734;
				}
				scheme = Side_Scheme();
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
