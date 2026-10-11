// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "gadget.h"
#include "_keyboar.h"
#include "globals.h"
#include "keyboard.h"
#include "scheme.h"

// FUNCTION: 0x004E12D0
int GadgetClass::Get_Color_Scheme(void)
{
	ColorScheme * scheme = Fetch_Scheme_By_Name(CurrentColorScheme);

	if (scheme != NULL) {
		return(scheme->ID);
	}

	return(0);
}

// FUNCTION: 0x004E12F0
GadgetClass::GadgetClass(int x, int y, int w, int h, unsigned flags, bool sticky) :
	X(x), Y(y), Width(w), Height(h), IsToRepaint(false), IsSticky(sticky), IsDisabled(false), Flags(flags)
{
	if (IsSticky) {
		Flags |= LEFTPRESS|LEFTRELEASE;
	}
}

// FUNCTION: 0x004E1340
GadgetClass::GadgetClass(GadgetClass const & gadget) :
	X(gadget.X),
	Y(gadget.Y),
	Width(gadget.Width),
	Height(gadget.Height),
	IsToRepaint(gadget.IsToRepaint),
	IsSticky(gadget.IsSticky),
	IsDisabled(gadget.IsDisabled),
	Flags(gadget.Flags)
{
}

// FUNCTION: 0x004E1390
GadgetClass::~GadgetClass(void)
{
	if (Has_Focus()) {
		Clear_Focus();
	}

	if (this == StuckOn) {
		StuckOn = NULL;
	}

	if (this == LastList) {
		LastList = NULL;
	}

	if (this == Focused) {
		Focused = NULL;
	}
}

// FUNCTION: 0x004E1450
void GadgetClass::Enable(void)
{
	IsDisabled = false;
	IsToRepaint = true;
	Clear_Focus();
}

// FUNCTION: 0x004E1460
void GadgetClass::Disable(void)
{
	IsDisabled = true;
	IsToRepaint = true;
	Clear_Focus();
}

// FUNCTION: 0x004E1480
LinkClass * GadgetClass::Remove(void)
{
	Clear_Focus();
	return(BASECLASS::Remove());
}

// FUNCTION: 0x004E14C0
void GadgetClass::Delete_List(void)
{
	GadgetClass * g = this;

	while (g->Get_Prev()) {
		g = (GadgetClass *)g->Get_Prev();
	}

	while (g) {
		g->Clear_Focus();

		GadgetClass * temp = g;
		g = (GadgetClass *)g->Get_Next();
		delete temp;
	}
}

// FUNCTION: 0x004E1550
int GadgetClass::Draw_Me(int forced)
{
	if (forced || IsToRepaint) {
		IsToRepaint = false;
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004E1570
void GadgetClass::Draw_All(bool forced)
{
	GadgetClass * gadget = this;

	while (gadget != NULL) {
		gadget->Draw_Me(forced);
		gadget = (GadgetClass *)gadget->Get_Next();
	}
}

// FUNCTION: 0x004E1920
ControlClass * GadgetClass::Extract_Gadget(unsigned id)
{
	GadgetClass * g = this;

	if (id != 0) {
		while (g != NULL) {
			if (g->Get_ID() == id) {
				return((ControlClass *)g);
			}
			g = (GadgetClass *)g->Get_Next();
		}
	}
	return(0);
}

// FUNCTION: 0x004E1970
void GadgetClass::Sticky_Process(unsigned flags)
{
	if (IsSticky && (flags & (LEFTPRESS|RIGHTPRESS))) {
		StuckOn = this;
	}
	if (StuckOn == this && (flags & (LEFTRELEASE|RIGHTRELEASE))) {
		StuckOn = 0;
	}
}

// FUNCTION: 0x004E19A0
void GadgetClass::Set_Focus(void)
{
	if (Focused) {
		Focused->Flag_To_Redraw();
		Focused->Clear_Focus();
	}
	Flags |= KEYBOARD;
	Focused = this;
}

// FUNCTION: 0x004E19D0
void GadgetClass::Clear_Focus(void)
{
	if (Focused == this) {
		Flags &= ~KEYBOARD;
		Focused = 0;
	}
}

// FUNCTION: 0x004E19F0
bool GadgetClass::Has_Focus(void)
{
	return(this == Focused);
}

// FUNCTION: 0x004E1A00
int GadgetClass::Is_List_To_Redraw(void)
{
	GadgetClass * gadget = this;

	while (gadget != NULL) {
		if (gadget->IsToRepaint) {
			return(true);
		}
		gadget = (GadgetClass *)gadget->Get_Next();
	}
	return(false);
}

// FUNCTION: 0x004E1A20
void GadgetClass::Set_Position(int x, int y)
{
	X = x;
	Y = y;
}

// FUNCTION: 0x004E1A40
void GadgetClass::Set_Size(int w, int h)
{
	Width = w;
	Height = h;
}
