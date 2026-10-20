// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "list.h"
#include "_convert.h"
#include "_surface.h"
#include "bound.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "dsurface.h"
#include "font.h"
#include "bitfont.h"
#include "language\language.h"
#include "scheme.h"
#include "vector.h"
#include "wwmouse.h"

// FUNCTION: 0x00557230
ListClass::ListClass(int id, int x, int y, int w, int h, TextPrintType flags, ShapeSet const * up, ShapeSet const * down) :
	BASECLASS(id, x, y, w, h, LEFTPRESS | LEFTRELEASE | KEYBOARD, false),
	UpGadget(0, up, x+w, y),
	DownGadget(0, down, x+w, y+h),
	ScrollGadget(0, x+w, y, 0, h, true)
{
	UpGadget.X -= UpGadget.Width;
	DownGadget.X -= DownGadget.Width;
	DownGadget.Y -= DownGadget.Height;
	ScrollGadget.X -= MAX(UpGadget.Width, DownGadget.Width);
	ScrollGadget.Y = Y+UpGadget.Height;
	ScrollGadget.Height -= UpGadget.Height + DownGadget.Height;
	ScrollGadget.Width = MAX(UpGadget.Width, DownGadget.Width);

	TextFlags = flags;
	IsScrollActive = false;
	Tabs = 0;
	SelectedIndex = 0;
	CurrentTopIndex = 0;
	BitFont * font = Font_From_TPF(TextFlags);
	LineHeight = font->field_1C-1;
	LineCount = (h-1) / LineHeight;
}

// FUNCTION: 0x00557380
ListClass::ListClass(ListClass const & list) :
	BASECLASS(list),
	TextFlags(list.TextFlags),
	Tabs(list.Tabs),
	List(list.List),
	LineHeight(list.LineHeight),
	LineCount(list.LineCount),
	IsScrollActive(list.IsScrollActive),
	UpGadget(list.UpGadget),
	DownGadget(list.DownGadget),
	ScrollGadget(list.ScrollGadget),
	SelectedIndex(list.SelectedIndex),
	CurrentTopIndex(list.CurrentTopIndex)
{
	UpGadget.Make_Peer(*this);
	DownGadget.Make_Peer(*this);
	ScrollGadget.Make_Peer(*this);
}

// FUNCTION: 0x00557570
void ListClass::Set_Position(int x, int y)
{
	UpGadget.X = x + Width - UpGadget.Width;
	UpGadget.Y = y;
	DownGadget.X = x + Width - DownGadget.Width;
	DownGadget.Y = y + Height - DownGadget.Height;
	ScrollGadget.X = x + Width - MAX(UpGadget.Width, DownGadget.Width);
	ScrollGadget.Y = y + UpGadget.Height;
	ScrollGadget.Height = Height - (UpGadget.Height + DownGadget.Height);
	ScrollGadget.Width = MAX(UpGadget.Width, DownGadget.Width);
}

// FUNCTION: 0x00557600
ListClass::~ListClass(void)
{
	Remove_Scroll_Bar();
}

// FUNCTION: 0x005576A0
int ListClass::Add_Item(wchar_t const * text)
{
	if (text) {
		List.Add(text);
		Flag_To_Redraw();

		if (List.Count() > LineCount) {
			Add_Scroll_Bar();
		}

		if (IsScrollActive) {
			ScrollGadget.Set_Maximum(List.Count());
		}
	}
	return(List.Count() - 1);
}

// FUNCTION: 0x00557730
void ListClass::Remove_Item(int index)
{
	if ((unsigned)index < (unsigned)List.Count()) {
		List.Delete_Index(index);

		if (List.Count() <= LineCount) {
			Remove_Scroll_Bar();
		}

		if (IsScrollActive) {
			ScrollGadget.Set_Maximum(List.Count());
		}

		if (SelectedIndex >= List.Count()) {
			SelectedIndex--;
			if (SelectedIndex < 0) {
				SelectedIndex = 0;
			}
		}

		if (CurrentTopIndex >= List.Count()) {
			CurrentTopIndex--;
			if (CurrentTopIndex < 0)
				CurrentTopIndex = 0;
			if (IsScrollActive)
				ScrollGadget.Step(1);
		}
	}
}

// FUNCTION: 0x00557800
void ListClass::Remove_Item(wchar_t const * text)
{
	if (text) {
		Remove_Item(List.ID(text));
	}
}

// FUNCTION: 0x00557830
int ListClass::Action(unsigned flags, KeyNumType & key, int modifier)
{
	if (flags & LEFTRELEASE) {
		key = KN_NONE;
		flags &= (~LEFTRELEASE);
		BASECLASS::Action(flags, key, 0);
		return(true);
	} else {
		if (flags & KEYBOARD) {
			if (key == KN_UP) {
				Step_Selected_Index(-1);
				key = KN_NONE;
			} else if (key == KN_DOWN) {
				Step_Selected_Index(1);
				key = KN_NONE;
			} else {
				flags &= ~KEYBOARD;
			}
		} else {
			int index = Get_Mouse_Y() - (Y+1);
			index = index / LineHeight;
			SelectedIndex = CurrentTopIndex + index;
			SelectedIndex = MIN(SelectedIndex, List.Count()-1);
			if (SelectedIndex == -1) SelectedIndex = 0;
		}
	}
	return(BASECLASS::Action(flags, key, 0));
}

// FUNCTION: 0x00557920
int ListClass::Draw_Me(int forced)
{
	if (GadgetClass::Draw_Me(forced)) {
		Conditional_Hide_Mouse(Rect(X, Y, Width, Height));

		Draw_Box(Rect(X, Y, Width, Height), BOXSTYLE_BOX, true);

		if (List.Count()) {
			for (int index = 0; index < LineCount; index++)  {
				int line = CurrentTopIndex + index;

				if (List.Count() > line) {
					Draw_Entry(line, X+1, Y+(LineHeight*index)+1, Width-2, (line == SelectedIndex));
				}
			}
		}

		Conditional_Show_Mouse();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00557A20
void ListClass::Bump(int up)
{
	if (IsScrollActive) {
		if (ScrollGadget.Step(up)) {
			CurrentTopIndex = ScrollGadget.Get_Value();
			Flag_To_Redraw();
		}
	}
}

// FUNCTION: 0x00557A70
void ListClass::Step(int up)
{
	if (IsScrollActive) {
		if (ScrollGadget.Step(up)) {
			CurrentTopIndex = ScrollGadget.Get_Value();
			Flag_To_Redraw();
		}
	}
}

// FUNCTION: 0x00557AC0
wchar_t const * ListClass::Get_Item(int index) const
{
	if (List.Count() == 0) {
		return(NULL);
	}
	index = MIN(index, List.Count()-1);
	return(List[index]);
}

// FUNCTION: 0x00557AE0
wchar_t const * ListClass::Current_Item(void) const
{
	if (List.Count() <= SelectedIndex) {
		return(0);
	}
	return(List[SelectedIndex]);
}

// FUNCTION: 0x00557CB0
int ListClass::Remove_Scroll_Bar(void)
{
	if (IsScrollActive) {
		IsScrollActive = false;
		Width += ScrollGadget.Width;
		ScrollGadget.Remove();
		UpGadget.Remove();
		DownGadget.Remove();
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}

// FUNCTION: 0x00557D10
void ListClass::Set_Tabs(int const * tabs)
{
	Tabs = tabs;
}
