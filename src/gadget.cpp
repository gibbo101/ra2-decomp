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
#include "xmouse.h"
#include "rect.h"

class InputManagerClass
{
	public:
		static InputManagerClass * Instance;
		int MouseQX;
		int MouseQY;
		short Check(void);
		short Get(void);
		bool IsKeyPressed(int key);
};

extern int DAT_007F5BE8;
extern int DAT_007F5BF4;
extern int DAT_00A8EC08;
extern int DAT_00A8EC0C;
extern int DAT_00A8EC00;
extern int DAT_00A8EC04;
extern int DAT_00A8EBF8;
extern int DAT_00A8EBFC;
extern GadgetClass * DAT_008B3E94;

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

// FUNCTION: 0x004E13F0
int GadgetClass::Clicked_On(KeyNumType & key, unsigned flags, int mousex, int mousey, int modifier)
{
	flags &= Flags;

	if (this == StuckOn ||
		(flags & KEYBOARD) ||
		(flags && (unsigned)(mousex - X) < (unsigned)Width  && (unsigned)(mousey - Y) < (unsigned)Height)) {

		return(Action(flags, key, modifier));
	}
	return(false);
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

// FUNCTION: 0x004E1470
bool GadgetClass::Is_Enabled(void) const
{
	return(!IsDisabled);
}

// FUNCTION: 0x004E1480
LinkClass * GadgetClass::Remove(void)
{
	Clear_Focus();
	return(BASECLASS::Remove());
}

// FUNCTION: 0x004E14A0
LinkClass * GadgetClass::Get_Next(void) const
{
	return(BASECLASS::Get_Next());
}

// FUNCTION: 0x004E14B0
LinkClass * GadgetClass::Get_Prev(void) const
{
	return(BASECLASS::Get_Prev());
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

// FUNCTION: 0x004E1510
void GadgetClass::OnMouseEnter(void)
{
}

// FUNCTION: 0x004E1520
void GadgetClass::OnMouseLeave(void)
{
}

// FUNCTION: 0x004E1530
int GadgetClass::Action(unsigned flags, KeyNumType &, int)
{
	if (flags) {
		IsToRepaint = true;
		Sticky_Process(flags);
		return(true);
	}
	return(false);
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

static inline bool Key_Down(int key)
{
	return InputManagerClass::Instance->IsKeyPressed(key);
}

// FUNCTION: 0x004E1640
KeyNumType GadgetClass::Input(void)
{
	int mousex, mousey;
	KeyNumType key;
	unsigned flags;
	int modifier;
	int forced = false;

	if (LastList != this) {
		LastList = this;
		forced = true;
		StuckOn = NULL;
		Focused = NULL;
	}

	key = (KeyNumType)(unsigned short)InputManagerClass::Instance->Check();
	if (key != 0) {
		key = (KeyNumType)(unsigned short)InputManagerClass::Instance->Get();
	}

	if (((key&0xFF) == KN_LMOUSE) || ((key&0xFF) == KN_RMOUSE)) {
		mousex = InputManagerClass::Instance->MouseQX;
		mousey = InputManagerClass::Instance->MouseQY;
	} else {
		mousex = MouseCursor->Get_Mouse_X();
		mousey = MouseCursor->Get_Mouse_Y();
	}

	GadgetClass * over = Extract_Gadget_At(mousex, mousey);
	if (over != DAT_008B3E94) {
		if (DAT_008B3E94) {
			DAT_008B3E94->OnMouseLeave();
		}
		DAT_008B3E94 = over;
		if (over) {
			over->OnMouseEnter();
		}
	}

	flags = 0;
	if (key) {
		if (key == KN_LMOUSE) {
			flags |= LEFTPRESS;
		} else if (key == KN_RMOUSE) {
			flags |= RIGHTPRESS;
		} else if (key == (KN_LMOUSE | KN_RLSE_BIT)) {
			flags |= LEFTRELEASE;
		} else if (key == (KN_RMOUSE | KN_RLSE_BIT)) {
			flags |= RIGHTRELEASE;
		}
	}

	modifier = 0;
	if (Key_Down(DAT_00A8EC08) || Key_Down(DAT_00A8EC0C)) {
		modifier |= 1;
	}
	if (Key_Down(DAT_00A8EC00) || Key_Down(DAT_00A8EC04)) {
		modifier |= 2;
	}
	if (Key_Down(DAT_00A8EBF8) || Key_Down(DAT_00A8EBFC)) {
		modifier |= 4;
	}

	if (key) {
		if (!flags) {
			flags |= KEYBOARD;
		}
	} else {
		if (Key_Down(KN_LMOUSE)) {
			flags |= LEFTHELD;
		} else {
			flags |= LEFTUP;
		}
		if (Key_Down(KN_RMOUSE)) {
			flags |= RIGHTHELD;
		} else {
			flags |= RIGHTUP;
		}
	}

	if (StuckOn) {
		StuckOn->Draw_Me(false);
		GadgetClass * oldstuck = StuckOn;
		StuckOn->Clicked_On(key, flags, mousex, mousey, 0);
		if (StuckOn) {
			StuckOn->Draw_Me(false);
		} else {
			oldstuck->Draw_Me(false);
		}
	} else {
		if (Focused && (flags & KEYBOARD)) {
			Focused->Draw_Me(false);
			Focused->Clicked_On(key, flags, mousex, mousey, 0);
			if (Focused) {
				Focused->Draw_Me(false);
			}
		} else {
			GadgetClass * next_button = this;
			while (next_button != NULL) {
				next_button->Draw_Me(forced);

				if (!next_button->IsDisabled) {
					if (next_button->Clicked_On(key, flags, mousex, mousey, modifier)) {
						next_button->Draw_Me(false);
						break;
					}
				}

				next_button = (GadgetClass *)next_button->Get_Next();
			}
		}
	}
	return(key);
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

// FUNCTION: 0x004E1960
void GadgetClass::Flag_To_Redraw(void)
{
	IsToRepaint = true;
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

// FUNCTION: 0x004E1A60 GadgetClass::~GadgetClass

// FUNCTION: 0x004E1AD0 LinkClass::~LinkClass
