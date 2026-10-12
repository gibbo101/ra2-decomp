// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "control.h"
#include "dialog.h"
#include "shapebtn.h"
#include "slider.h"
#include "vector.h"

class ShapeSet;

class ListClass : public ControlClass
{
		typedef ControlClass BASECLASS;

	public:
		ListClass(int id, int x, int y, int w, int h, TextPrintType flags, ShapeSet const * up, ShapeSet const * down);
		ListClass(ListClass const & list);
		virtual ~ListClass(void) override;

		virtual int Add_Item(char const * text);
		virtual int Add_Item(int text);
		virtual int Add_Scroll_Bar(void);
		virtual void Bump(int up);
		virtual int Count(void) const {return(List.Count());};
		virtual int Current_Index(void) const;
		virtual char const * Current_Item(void) const;
		virtual int Draw_Me(int forced) override;
		virtual char const * Get_Item(int index) const;
		virtual int Step_Selected_Index(int forward);
		virtual void Flag_To_Redraw(void) override;

		virtual void Peer_To_Peer(unsigned flags, KeyNumType & key, ControlClass & whom) override;
		virtual void Remove_Item(char const * text);
		virtual void Remove_Item(int);
		virtual int  Remove_Scroll_Bar(void);
		virtual void Set_Selected_Index(int index);
		virtual void Set_Selected_Index(char const * text);
		virtual void Set_Tabs(int const * tabs);
		virtual int  Set_View_Index(int index);
		virtual void Step(int up);
		virtual void Set_Position(int x, int y) override;

		virtual LinkClass & Add(LinkClass & object) override;
		virtual LinkClass & Add_Tail(LinkClass & object) override;
		virtual LinkClass & Add_Head(LinkClass & object) override;
		virtual LinkClass * Remove(void) override;

	protected:
		virtual int  Action(unsigned flags, KeyNumType &key, int modifier) override;
		virtual void Draw_Entry(int index, int x, int y, int width, int selected);

		TextPrintType TextFlags;

		int const *Tabs;

		DynamicVectorClass<char const *> List;

		int LineHeight;

		int LineCount;

		bool IsScrollActive;
		ShapeButtonClass UpGadget;
		ShapeButtonClass DownGadget;
		SliderClass ScrollGadget;

		int SelectedIndex;

		int CurrentTopIndex;
};

template<class T>
class TListClass : public ControlClass
{
		typedef ControlClass BASECLASS;

	public:
		TListClass(int id, int x, int y, int w, int h, TextPrintType flags, ShapeSet const * up, ShapeSet const * down);
		TListClass(TListClass<T> const & list);
		virtual ~TListClass(void) override;
		T operator [] (int index) const {return(List[index]);};
		T & operator [] (int index) {return(List[index]);};

		virtual int Add_Item(T item);
		virtual int Add_Scroll_Bar(void);
		virtual void Insert_Item(T item);
		virtual void Bump(int up);
		virtual int Count(void) const {return(List.Count());};
		virtual int Current_Index(void) const;
		virtual T Current_Item(void) const;
		virtual int Draw_Me(int forced) override;
		virtual int Step_Selected_Index(int forward);
		virtual void Flag_To_Redraw(void) override;
		virtual T Get_Item(int index) const {return(List[index]);};

		virtual void Peer_To_Peer(unsigned flags, KeyNumType & key, ControlClass & whom) override;
		virtual void Remove_Item(T item);
		virtual void Remove_Index(int index);
		virtual int  Remove_Scroll_Bar(void);
		virtual void Set_Selected_Index(int index);
		virtual void Set_Selected_Index(T item);
		virtual void Set_Tabs(int const * tabs);
		virtual int  Set_View_Index(int index);
		virtual void Step(int up);
		virtual void Set_Position(int x, int y) override;

		virtual LinkClass & Add(LinkClass & object) override;
		virtual LinkClass & Add_Tail(LinkClass & object) override;
		virtual LinkClass & Add_Head(LinkClass & object) override;
		virtual LinkClass * Remove(void) override;

	protected:
		virtual int  Action(unsigned flags, KeyNumType & key, int modifier) override;

		TextPrintType TextFlags;

		int const * Tabs;

		DynamicVectorClass<T> List;

		int LineHeight;

		int LineCount;

		bool IsScrollActive;
		ShapeButtonClass UpGadget;
		ShapeButtonClass DownGadget;
		SliderClass ScrollGadget;

		int SelectedIndex;

		int CurrentTopIndex;
};

template<class T>
TListClass<T>::TListClass(int id, int x, int y, int w, int h, TextPrintType flags, ShapeSet const * up, ShapeSet const * down) :
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
	FontClass *font = Font_From_TPF(TextFlags);
	LineHeight = font->Get_Height()-1;
	LineCount = (h-1) / LineHeight;
}

template<class T>
TListClass<T>::TListClass(TListClass<T> const & list) :
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

template<class T>
void TListClass<T>::Set_Position(int x, int y)
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

template<class T>
TListClass<T>::~TListClass(void)
{
	Remove_Scroll_Bar();
}

template<class T>
void TListClass<T>::Insert_Item(T item)
{
	if (Current_Index() >= Count()) {
		List.Add(item);
	} else {
		List.Add(item);

		for (int index = List.Count()-1; index > Current_Index(); index--) {
			List[index] = List[index-1];
		}

		List[Current_Index()] = item;
	}
}

template<class T>
int TListClass<T>::Add_Item(T item)
{
	List.Add(item);
	Flag_To_Redraw();

	if (List.Count() > LineCount) {
		Add_Scroll_Bar();
	}

	if (IsScrollActive) {
		ScrollGadget.Set_Maximum(List.Count());
	}
	return(List.Count() - 1);
}

template<class T>
void TListClass<T>::Remove_Index(int index)
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

template<class T>
void TListClass<T>::Remove_Item(T item)
{
	Remove_Index(List.ID(item));
}

template<class T>
int TListClass<T>::Action(unsigned flags, KeyNumType & key, int modifier)
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

template<class T>
int TListClass<T>::Draw_Me(int forced)
{
	if (GadgetClass::Draw_Me(forced)) {

		Conditional_Hide_Mouse(Rect(X, Y, Width, Height));

		Draw_Box(Rect(X, Y, Width, Height), BOXSTYLE_BOX, true);

		if (List.Count()) {
			for (int index = 0; index < LineCount; index++)  {
				int line = CurrentTopIndex + index;

				if (List.Count() > line) {

					List[line]->Draw_It(line, X+1, Y+(LineHeight*index)+1, Width-2, LineHeight, (line == SelectedIndex), TextFlags);
				}
			}
		}

		Conditional_Show_Mouse();
		return(true);
	}
	return(false);
}

template<class T>
void TListClass<T>::Bump(int up)
{
	if (IsScrollActive) {
		if (ScrollGadget.Step(up)) {
			CurrentTopIndex = ScrollGadget.Get_Value();
			Flag_To_Redraw();
		}
	}
}

template<class T>
void TListClass<T>::Step(int up)
{
	if (IsScrollActive) {
		if (ScrollGadget.Step(up)) {
			CurrentTopIndex = ScrollGadget.Get_Value();
			Flag_To_Redraw();
		}
	}
}

template<class T>
T TListClass<T>::Current_Item(void) const
{
	static T _temp;
	if (List.Count() <= SelectedIndex) {
		return(_temp);
	}
	return(List[SelectedIndex]);
}

template<class T>
int TListClass<T>::Current_Index(void) const
{
	return(SelectedIndex);
}

template<class T>
void TListClass<T>::Peer_To_Peer(unsigned flags, KeyNumType &, BASECLASS & whom)
{
	if (flags & LEFTRELEASE) {
		if (&whom == &UpGadget) {
			Step(true);
		}
		if (&whom == &DownGadget) {
			Step(false);
		}
	}

	if (&whom == &ScrollGadget) {
		Set_View_Index(ScrollGadget.Get_Value());
	}
}

template<class T>
int TListClass<T>::Set_View_Index(int index)
{
	index = Bound(index, 0, MAX(0, List.Count() - LineCount));
	if (index != CurrentTopIndex) {
		CurrentTopIndex = index;
		Flag_To_Redraw();
		if (IsScrollActive) {
			ScrollGadget.Set_Value(CurrentTopIndex);
		}
		return(true);
	}
	return(false);
}

template<class T>
int TListClass<T>::Add_Scroll_Bar(void)
{
	if (!IsScrollActive) {
		IsScrollActive = true;

		Flag_To_Redraw();
		Width -= ScrollGadget.Width;

		UpGadget.Make_Peer(*this);
		DownGadget.Make_Peer(*this);
		ScrollGadget.Make_Peer(*this);

		UpGadget.Add(*this);
		DownGadget.Add(*this);
		ScrollGadget.Add(*this);

		UpGadget.Flag_To_Redraw();
		DownGadget.Flag_To_Redraw();
		ScrollGadget.Flag_To_Redraw();

		ScrollGadget.Set_Maximum(List.Count());
		ScrollGadget.Set_Thumb_Size(LineCount);
		ScrollGadget.Set_Value(CurrentTopIndex);

		return(true);
	}
	return(false);
}

template<class T>
int TListClass<T>::Remove_Scroll_Bar(void)
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

template<class T>
void TListClass<T>::Set_Tabs(int const * tabs)
{
	Tabs = tabs;
}

template<class T>
LinkClass & TListClass<T>::Add(LinkClass & list)
{

	if (IsScrollActive) {
		ScrollGadget.Add(list);
		DownGadget.Add(list);
		UpGadget.Add(list);
	}

	return(BASECLASS::Add(list));
}

template<class T>
LinkClass & TListClass<T>::Add_Head(LinkClass & list)
{

	if (IsScrollActive) {
		ScrollGadget.Add_Head(list);
		DownGadget.Add_Head(list);
		UpGadget.Add_Head(list);
	}

	return(BASECLASS::Add_Head(list));
}

template<class T>
LinkClass & TListClass<T>::Add_Tail(LinkClass & list)
{

	BASECLASS::Add_Tail(list);

	if (IsScrollActive) {
		UpGadget.Add_Tail(list);
		DownGadget.Add_Tail(list);
		ScrollGadget.Add_Tail(list);
	}

	return(Head_Of_List());
}

template<class T>
LinkClass * TListClass<T>::Remove(void)
{

	if (IsScrollActive) {
		ScrollGadget.Remove();
		DownGadget.Remove();
		UpGadget.Remove();
	}

	return(BASECLASS::Remove());
}

template<class T>
void TListClass<T>::Set_Selected_Index(int index)
{
	if ((unsigned)index < (unsigned)List.Count()) {
		SelectedIndex = index;
		Flag_To_Redraw();
		if (SelectedIndex < CurrentTopIndex) {
			Set_View_Index(SelectedIndex);
		}
		if (SelectedIndex >= CurrentTopIndex+LineCount) {
			Set_View_Index(SelectedIndex-(LineCount-1));
		}
	} else {
		SelectedIndex = 0;
	}
}

template<class T>
int TListClass<T>::Step_Selected_Index(int step)
{
	int old = SelectedIndex;

	Set_Selected_Index(old + step);
	return(old);
}

template<class T>
void TListClass<T>::Flag_To_Redraw(void)
{
	if (IsScrollActive) {
		UpGadget.Flag_To_Redraw();
		DownGadget.Flag_To_Redraw();
		ScrollGadget.Flag_To_Redraw();
	}
	BASECLASS::Flag_To_Redraw();
}

template<class T>
void TListClass<T>::Set_Selected_Index(T item)
{
	for (int index = 0; index < Count(); index++) {
		if (item == Get_Item(index)) {
			Set_Selected_Index(index);
			break;
		}
	}
}
