// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "drop.h"
#include "font.h"
#include "shapeset.h"
#include "bitfont.h"
#include "geometry.h"
#include "fastmath.h"

// FUNCTION: 0x004B4E10
DropListClass::DropListClass(int id, wchar_t * text, int max_len, TextPrintType flags, int x, int y, int w, int h, ShapeSet const * up, ShapeSet const * down) :
	BASECLASS(id, text, max_len, flags, x, y, w, 18, ALPHANUMERIC),
	IsDropped(false),
	ListHeight(h),
	DropButton(0, down, x+w, y),
	List(0, x, y+down->Get_Height(), w+down->Get_Width(), h, flags, up, down)
{
	BitFont * font = Font_From_TPF(flags);
	BASECLASS::Height = font->field_1C+1;
	List.Make_Peer(*this);
	DropButton.Make_Peer(*this);
}

// FUNCTION: 0x004B4EE0
void DropListClass::Zap(void)
{
	Collapse();
	List.Zap();
	DropButton.Zap();
	BASECLASS::Zap();
}

// FUNCTION: 0x004B4F20
LinkClass & DropListClass::Add(LinkClass & object)
{
	DropButton.Add(object);
	return((DropListClass &)BASECLASS::Add(object));
}

// FUNCTION: 0x004B4F40
LinkClass & DropListClass::Add_Tail(LinkClass & object)
{
	DropButton.Add_Tail(object);
	return((DropListClass &)BASECLASS::Add_Tail(object));
}

// FUNCTION: 0x004B4F60
LinkClass & DropListClass::Add_Head(LinkClass & object)
{
	DropButton.Add_Head(object);
	return((DropListClass &)BASECLASS::Add_Head(object));
}

// FUNCTION: 0x004B4F80
LinkClass * DropListClass::Remove(void)
{
	if (IsDropped) {
		Collapse();
	}
	DropButton.Remove();
	return((DropListClass *)BASECLASS::Remove());
}

// FUNCTION: 0x004B4FB0
int DropListClass::Add_Item(wchar_t const * text)
{
	wcsncpy(String, text, MaxLength);
	Flag_To_Redraw();
	return(List.Add_Item(text));
}

// FUNCTION: 0x004B4FF0
wchar_t const * DropListClass::Current_Item(void)
{
	return(List.Current_Item());
}

// FUNCTION: 0x004B5010
int DropListClass::Current_Index(void)
{
	return(List.Current_Index());
}

// FUNCTION: 0x004B5030
void DropListClass::Set_Selected_Index(int index)
{
	if ((unsigned)index < (unsigned)List.Count()) {
		List.Set_Selected_Index(index);
		wcscpy(String, List.Get_Item(Current_Index()));
	} else {
		String[0] = '\0';
	}
}

// FUNCTION: 0x004B50A0
void DropListClass::Clear_Focus(void)
{
	Collapse();
}

// FUNCTION: 0x004B50C0
void DropListClass::Peer_To_Peer(unsigned flags, KeyNumType & key, ControlClass & whom)
{
	if (&whom == &DropButton) {
		if (flags & LEFTRELEASE) {
			if (IsDropped) {
				Collapse();
				key = (KeyNumType)(ID | KN_BUTTON);
			} else {
				Expand();
			}
		}
	}

	if (&whom == &List) {
		wcsncpy(String, List.Current_Item(), MaxLength);
		Flag_To_Redraw();
		key = (KeyNumType)(ID | KN_BUTTON);
	}
}

// FUNCTION: 0x004B5190
void DropListClass::Expand(void)
{
	if (!IsDropped) {
		List.X = X;
		List.Y = Y+18;
		List.Width = Width;
		List.Height = ListHeight;
		List.Add(Head_Of_List());
		List.Flag_To_Redraw();
		IsDropped = true;
	}
}

// FUNCTION: 0x004B51F0
void DropListClass::Collapse(void)
{
	if (IsDropped) {
		List.Remove();
		IsDropped = false;
	}
}

// FUNCTION: 0x004B5210
DropListClass & DropListClass::operator = (DropListClass const & list)
{
	if (this == &list) return(*this);
	BASECLASS::operator =(list);
	List = list.List;
	IsDropped = list.IsDropped;
	ListHeight = list.ListHeight;
	DropButton = list.DropButton;
	List.Make_Peer(*this);
	DropButton.Make_Peer(*this);
	return(*this);
}

// FUNCTION: 0x004B53E0
DropListClass::DropListClass(DropListClass const & list) :
	BASECLASS(list),
	IsDropped(list.IsDropped),
	ListHeight(list.ListHeight),
	DropButton(list.DropButton),
	List(list.List)
{
	List.Make_Peer(*this);
	DropButton.Make_Peer(*this);
}

// FUNCTION: 0x004B54E0
void DropListClass::Set_Position(int x, int y)
{
	BASECLASS::Set_Position(x, y);
	List.Set_Position(x, y + DropButton.Get_Shape_Data()->Get_Height());
	DropButton.Set_Position(x + Width, y);
}

// FUNCTION: 0x004B5530
void DropListClass::Set_Selected_Index(wchar_t const * text)
{
	if (text) {
		for (int index = 0; index < Count(); index++) {
			if (wcsicmp(text, List.Get_Item(index)) == 0) {
				Set_Selected_Index(index);
				break;
			}
		}
	}
}

// FUNCTION: 0x004B55A0 EditClass::Get_Text

// FUNCTION: 0x004B55B0 DropListClass::Count

// FUNCTION: 0x004B55D0 DropListClass::Get_Item

// FUNCTION: 0x004B55F0 DropListClass::~DropListClass

// FUNCTION: 0x004B5630 ShapeButtonClass::operator=

// FUNCTION: 0x004B56E0 SliderClass::operator=

// FUNCTION: 0x004B5780 GadgetClass::operator=

// FUNCTION: 0x004B57D0 EditClass::~EditClass

// FUNCTION: 0x004B57F0 ShapeButtonClass::~ShapeButtonClass

// FUNCTION: 0x004B5810 ToggleClass::~ToggleClass

// FUNCTION: 0x004B5830 ?$VectorClass@PBG::operator=

// FUNCTION: 0x004B58A0 $E11

// FUNCTION: 0x004B58D0 $E14

// FUNCTION: 0x004B58F0 $E17

// FUNCTION: 0x004B5910 $E20

// FUNCTION: 0x004B5930 $E23

// FUNCTION: 0x004B5950 $E26

// FUNCTION: 0x004B5970 $E29

// FUNCTION: 0x004B59B0 $E32

// FUNCTION: 0x004B59E0 $E35

// FUNCTION: 0x004B5A10 $E38
