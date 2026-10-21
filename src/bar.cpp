// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "bar.h"
#include "_convert.h"
#include "_surface.h"
#include "_xmouse.h"
#include "convert.h"
#include "dsurface.h"
#include "xmouse.h"
#include <math.h>

struct ShortPair {
	short A;
	short B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A;
	int B;
	int C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

struct IntQuad {
	int A;
	int B;
	int C;
	int D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

extern int DAT_0089C358;
extern int DAT_0089C35C;
extern int DAT_0089C360;

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E3868;
extern const double DAT_007E3870;

// FUNCTION: GAMEMD 0x0042E0B0
ProgressBarClass::ProgressBarClass(int x, int y, int width, int height, int forecolor, int backcolor, int bordercolor) :
	X(x),
	Y(y),
	Width(width),
	Height(height),
	BarColor(forecolor),
	BackColor(backcolor),
	BorderColor(bordercolor),
	CurrentValue(0),
	LastDisplayCurrent(0),
	IsDrawn(false)
{
}

// FUNCTION: GAMEMD 0x0042E100
bool ProgressBarClass::Is_Horizontal(void) const
{
	if (Width > Height) return(true);
	return(false);
}

// FUNCTION: GAMEMD 0x0042E110
bool ProgressBarClass::Update(double value)
{
	CurrentValue = value;

	if (!IsDrawn || value - LastDisplayCurrent >= .1) {
		Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x0042E150
void ProgressBarClass::Outline(void) const
{
	if (Is_Outlined()) {
		LogicalSurface->Draw_Rect(Rect(X, Y, Width, Height), NormalDrawer->Convert_Pixel(BorderColor));
	}
}

// FUNCTION: GAMEMD 0x0042E1C0
void ProgressBarClass::Redraw(void) const
{
	Hide_Mouse();

	Outline();

	int x = X;
	int y = Y;
	int w = Width;
	int h = Height;
	if (Is_Outlined()) {
		x += 1;
		y += 1;
		w -= 2;
		h -= 2;
	}

	int size = Is_Horizontal() ? w : h;

	int fill = CurrentValue * size;

	if (fill > 0) {
		if (Is_Horizontal()) {
			LogicalSurface->Fill_Rect(Rect(x, y, fill, h), NormalDrawer->Convert_Pixel(BarColor));
		} else {
			LogicalSurface->Fill_Rect(Rect(x, y+fill, w, h), NormalDrawer->Convert_Pixel(BarColor));
		}
	}

	if (w-fill > 0) {
		if (Is_Horizontal()) {
			LogicalSurface->Fill_Rect(Rect(x+fill, y, w, h), NormalDrawer->Convert_Pixel(BackColor));
		} else {
			LogicalSurface->Fill_Rect(Rect(x, y, w, fill-1), NormalDrawer->Convert_Pixel(BackColor));
		}
	}

	Show_Mouse();

	ProgressBarClass * bar = (ProgressBarClass *)this;
	bar->LastDisplayCurrent = CurrentValue;
	bar->IsDrawn = true;
}

// TEMPLATE: GAMEMD 0x0042E410
// ??0?$TRect@H@@QAE@HHHH@Z
template class TRect<int>;

// SYNTHETIC: GAMEMD 0x0042E430 SYMBOL
// _$E24
double DAT_0089C318 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0042E460 SYMBOL
// _$E27
double DAT_0089C368 = DAT_0089C318 / DAT_007E3868;

// SYNTHETIC: GAMEMD 0x0042E480 SYMBOL
// _$E30
double DAT_0089C350 = DAT_007E1718 / DAT_0089C368;

// SYNTHETIC: GAMEMD 0x0042E4A0 SYMBOL
// _$E33
double DAT_0089C348 = DAT_007E1720 * DAT_007E3870;

// SYNTHETIC: GAMEMD 0x0042E4C0 SYMBOL
// _$E36
double DAT_0089C320 = DAT_007E1728 * DAT_007E3870;

// SYNTHETIC: GAMEMD 0x0042E4E0 SYMBOL
// _$E39
double DAT_0089C2F8 = DAT_007E1730 * DAT_007E3870;

// SYNTHETIC: GAMEMD 0x0042E500 SYMBOL
// _$E42
int DAT_0089C370 = (int)(FUN_004CAD50(DAT_0089C2F8 - DAT_0089C320) * DAT_007E1738 * DAT_0089C318);

// SYNTHETIC: GAMEMD 0x0042E540 SYMBOL
// _$E45
double DAT_0089C328 = FUN_004CADE0(DAT_0089C370 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0042E570 SYMBOL
// _$E48
double DAT_0089C340 = FUN_004CADE0((DAT_0089C370 * 2) / DAT_0089C318);

// SYNTHETIC: GAMEMD 0x0042E5A0 SYMBOL
// _$E51
int DAT_0089C364 = (int)(DAT_0089C370 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0042E5D0 SYMBOL
// _$E54
ShortPair DAT_0089C310;

// SYNTHETIC: GAMEMD 0x0042E5E0 SYMBOL
// _$E57
IntTriple DAT_0089C330;

// FUNCTION: GAMEMD 0x0042E600
// Reset_Bar_View
void Reset_Bar_View(void)
{
	DAT_0089C360 = 0;
	DAT_0089C358 = 0x80;
	DAT_0089C35C = 0x80;
}

// SYNTHETIC: GAMEMD 0x0042E620 SYMBOL
// _$E60
IntQuad DAT_0089C300;
