// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "colrlist.h"
#include "_convert.h"
#include "_surface.h"
#include "convert.h"
#include "dialog.h"
#include "dsurface.h"
#include "globals.h"
#include "scheme.h"
#include "vector.h"

// FUNCTION: 0x00488760
ColorListClass::ColorListClass (int id, int x, int y, int w, int h,
	TextPrintType flags, ShapeSet const * up, ShapeSet const * down) :
	BASECLASS (id, x, y, w, h, flags, up, down),
	Style(SELECT_HIGHLIGHT),
	SelectColor(0)
{
}

// FUNCTION: 0x004887F0
ColorListClass::~ColorListClass(void)
{
	Colors.Clear();
	SelectColor = 0;
}

// FUNCTION: 0x00488850
int ColorListClass::Add_Item(wchar_t const * text, int color)
{
	Colors.Add(color);
	return(BASECLASS::Add_Item(text));
}

// FUNCTION: 0x004888B0
void ColorListClass::Remove_Item(wchar_t const * text)
{
	int index = List.ID(text);
	if (index != -1) {
		Colors.Delete_Index(index);
		BASECLASS::Remove_Item(text);
	}
}

// FUNCTION: 0x00488910
void ColorListClass::Remove_Item(int index)
{
	if (index != -1) {
		Colors.Delete_Index(index);
		BASECLASS::Remove_Item(index);
	}
}

// FUNCTION: 0x00488960
void ColorListClass::Set_Selected_Style(SelectStyleType style, int color)
{
	Style = style;
	SelectColor = color;
}

// FUNCTION: 0x00488980
void ColorListClass::Draw_Entry(int index, int x, int y, int width, int selected)
{
	int color;

	if (!selected) {
		Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[Colors[index]], TBLACK, TextFlags, width, Tabs);
		return;
	}

	if (SelectColor == 0) {
		color = int(index);
	} else {
		color = SelectColor;
	}

	switch (Style) {
		case SELECT_NORMAL:
			Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[Colors[index]], TBLACK,
				TextFlags, width, Tabs);
			break;

		case SELECT_HIGHLIGHT:
			if (TextFlags & TPF_6PT_GRAD) {
				Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[color], TBLACK, TextPrintType(TextFlags | TPF_BRIGHT_COLOR), width, Tabs);
			} else {
				Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[color], TBLACK, TextFlags, width, Tabs);
			}
			break;

		case SELECT_BOX:
			DSurface::Temp->Draw_Rect(Rect(x, y, x + width - 2, y + LineHeight - 2), NormalDrawer->Convert_Pixel(ColorSchemes[color]->Color));
			Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[Colors[index]], TBLACK, TextFlags, width, Tabs);
			break;

		case SELECT_BAR:
			if (TextFlags & TPF_6PT_GRAD) {
				DSurface::Temp->Fill_Rect(Rect(x, y, width, LineHeight), NormalDrawer->Convert_Pixel(ColorSchemes[color]->Color));
				Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[Colors[index]], TBLACK, TextPrintType(TextFlags | TPF_BRIGHT_COLOR), width, Tabs);
			} else {
				DSurface::Temp->Fill_Rect(Rect(x, y, width - 1, LineHeight - 1), NormalDrawer->Convert_Pixel(ColorSchemes[color]->Color));
				Conquer_Clip_Text_Print_Wide(List[index], DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[Colors[index]], TBLACK, TextFlags, width, Tabs);
			}
			break;

		case SELECT_INVERT:
			DSurface::Temp->Fill_Rect(Rect(x, y, width, LineHeight), NormalDrawer->Convert_Pixel(ColorSchemes[Colors[index]]->Color));
			break;
	}
}

// FUNCTION: 0x00488EE0 ColorListClass::Add_Item

// FUNCTION: 0x00488F00 ColorListClass::~ColorListClass

// FUNCTION: 0x00488F70 $E2
