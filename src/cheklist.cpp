// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "cheklist.h"
#include "_convert.h"
#include "_surface.h"
#include "convert.h"
#include "dialog.h"
#include "dsurface.h"
#include "scheme.h"
#include "vector.h"
#include <cstdio>

// SYNTHETIC: GAMEMD 0x00488260 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x00488280
CheckListClass::CheckListClass(int id, int x, int y, int w, int h, TextPrintType flags,
	ShapeSet const * up, ShapeSet const * down) :
	BASECLASS (id, x, y, w, h, flags, up, down),
	IsReadOnly(false)
{
}

// FUNCTION: GAMEMD 0x004882D0
CheckListClass::~CheckListClass(void)
{
	while (CheckListClass::Count()) {
		CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(0);

		BASECLASS::Remove_Item(0);
		delete obj;
	}
}

// FUNCTION: GAMEMD 0x00488310
int CheckListClass::Add_Item(wchar_t const * text)
{
	CheckObject * obj = new CheckObject(text, false);
	return(BASECLASS::Add_Item((wchar_t const *)obj));
}

// FUNCTION: GAMEMD 0x00488350
wchar_t const * CheckListClass::Current_Item(void) const
{
	CheckObject * obj = (CheckObject *)BASECLASS::Current_Item();
	if (obj) {
		return(obj->Text);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x00488360
wchar_t const * CheckListClass::Get_Item(int index) const
{
	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
	if (obj) {
		return(obj->Text);
	}
	return(0);
}

// FUNCTION: GAMEMD 0x00488380
void CheckListClass::Remove_Item(wchar_t const * text)
{
	for (int index = 0; index < Count(); index++) {
		CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
		if (obj && wcsicmp(obj->Text, text) == 0) {
			BASECLASS::Remove_Item(index);
			delete obj;
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x004883F0
void CheckListClass::Set_Selected_Index(wchar_t const * text)
{
	for (int index = 0; index < Count(); index++) {
		CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
		if (obj && wcsicmp(obj->Text, text) == 0) {
			Set_Selected_Index(index);
			break;
		}
	}
}

// FUNCTION: GAMEMD 0x00488450
void CheckListClass::Check_Item(int index, bool checked)
{
	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
	if (obj && obj->IsChecked != checked) {
		obj->IsChecked = checked;
		Flag_To_Redraw();
	}
}

// FUNCTION: GAMEMD 0x00488480
bool CheckListClass::Is_Checked(int index) const
{
	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);
	if (obj) {
		return(obj->IsChecked);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004884A0
int CheckListClass::Action(unsigned flags, KeyNumType &key, int modifier)
{
	int rc;

	if (IsReadOnly) {
		return(false);
	}

	rc = BASECLASS::Action(flags, key, 0);

	if (flags & LEFTPRESS) {
		Check_Item(SelectedIndex, !Is_Checked(SelectedIndex));
	}

	return(rc);
}

// FUNCTION: GAMEMD 0x00488520
void CheckListClass::Draw_Entry(int index, int x, int y, int width, int selected)
{
	if (index >= Count()) return;

	CheckObject * obj = (CheckObject *)BASECLASS::Get_Item(index);

	if (obj) {
		wchar_t buffer[100] = L"";

		if (obj->IsChecked) {
			buffer[0] = CHECK_CHAR;
		} else {
			buffer[0] = UNCHECK_CHAR;
		}
		buffer[1] = L' ';
		swprintf(&buffer[2], obj->Text);

		TextPrintType flags = TextFlags;
		int scheme = GadgetClass::Get_Color_Scheme();

		if (selected) {
			flags = TextPrintType(flags | TPF_BRIGHT_COLOR);
			DSurface::Temp->Fill_Rect(Rect(x, y, width, LineHeight), NormalDrawer->Convert_Pixel(ColorSchemes[scheme]->Shadow));
		} else {
			if (!(flags & TPF_USE_GRAD_PAL)) {
				flags = TextPrintType(flags | TPF_MEDIUM_COLOR);
			}
		}

		Conquer_Clip_Text_Print_Wide(buffer, DSurface::Temp, &DSurface::Temp->Get_Rect(), &Point2D(x, y), ColorSchemes[scheme], TBLACK, flags, width, Tabs);
	}
}

// FUNCTION: GAMEMD 0x00488690
// GadgetClass::Flag_List_To_Redraw

// FUNCTION: GAMEMD 0x004886A0
// GadgetClass::Is_To_Redraw

// FUNCTION: GAMEMD 0x004886B0
// ListClass::Count

// FUNCTION: GAMEMD 0x004886C0
// CheckListClass::Remove_Item

// FUNCTION: GAMEMD 0x004886D0
// CheckListClass::Set_Selected_Index

// SYNTHETIC: GAMEMD 0x004886E0
// ??_GCheckListClass@@UAEPAXI@Z
