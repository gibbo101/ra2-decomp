// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "tab.h"
#include "shapebtn.h"
#include <string.h>
#include "rect.h"
#include "shapeset.h"
#include "tooltip.h"

#include <cmath>

// FUNCTION: 0x006CF9B0 $E2
// FUNCTION: 0x006CF9E0 $E5
// FUNCTION: 0x006CFA00 $E8
// FUNCTION: 0x006CFA20 $E11
// FUNCTION: 0x006CFA40 $E14
// FUNCTION: 0x006CFA60 $E17
// FUNCTION: 0x006CFA80 $E20
// FUNCTION: 0x006CFAC0 $E23
// FUNCTION: 0x006CFAF0 $E26
// FUNCTION: 0x006CFB20 $E29
// FUNCTION: 0x006CFB50 $E32
// FUNCTION: 0x006CFB60 $E35
// FUNCTION: 0x006CFB80 $E38

struct CommandBarStatics {
	static ShapeButtonClass Buttons[25];
	static ShapeButtonClass ButtonA;
	static ShapeButtonClass ButtonB;
};

// FUNCTION: 0x006CFBA0 $E91
// FUNCTION: 0x006CFBD0 $E88
ShapeButtonClass CommandBarStatics::Buttons[25];

// FUNCTION: 0x006CFC00 $E96
// FUNCTION: 0x006CFC20 $E94
ShapeButtonClass CommandBarStatics::ButtonA;

// FUNCTION: 0x006CFC40 $E101
// FUNCTION: 0x006CFC60 $E99
ShapeButtonClass CommandBarStatics::ButtonB;

extern void * DAT_00B0C148[];
extern bool DAT_00B0CBDC[];
extern char const * DAT_008427D0[11];
extern int DAT_008427CC;
extern int DAT_00B0CB78[];

// FUNCTION: 0x006CFC80
void __fastcall FUN_006CFC80(int index)
{
	if (DAT_00B0CBDC[index]) {
		delete DAT_00B0C148[index];
		DAT_00B0CBDC[index] = false;
	}
	DAT_00B0C148[index] = NULL;
}

// FUNCTION: 0x006CFCC0
int __fastcall FUN_006CFCC0(char const * name)
{
	for (int i = 0; i < 11; i++) {
		if (strcmp(DAT_008427D0[i], name) == 0) {
			return i;
		}
	}
	return DAT_008427CC;
}

// FUNCTION: 0x006CFD20
void __fastcall FUN_006CFD20(int button, int slot)
{
	DAT_00B0CB78[button] = slot;
}

// FUNCTION: 0x006CFD30
int __fastcall FUN_006CFD30(int button)
{
	return DAT_00B0CB78[button];
}

// FUNCTION: 0x006CFD40
ShapeButtonClass * __fastcall FUN_006CFD40(int button)
{
	int slot = DAT_00B0CB78[button];
	if (slot < 0 || slot >= 25) {
		return NULL;
	}
	return &CommandBarStatics::Buttons[slot];
}

extern Rect * DAT_00B0FC68;
extern Rect * DAT_00B0FC60;
extern Rect * DAT_00B0FC64;
extern Rect * DAT_00B0FC6C;
extern ShapeSet const * DAT_00B0FA90;
extern int DAT_00B0CC38;
extern int DAT_00B0CC24;
extern int DAT_00B0CB54;
extern int DAT_00B0CC2C;
extern int DAT_00B0CC20;
extern int DAT_00B0CD28;

struct FileSystem {
	static void * __fastcall LoadWholeFileEx(char const * name, bool & allocated);
	static ConvertClass * SIDEBAR_PAL;
};

namespace Unsorted { extern bool ArmageddonMode; }

bool __fastcall FUN_0072F430(void);

// FUNCTION: 0x006CFD60
Rect __fastcall FUN_006CFD60(int button)
{
	Rect r = *DAT_00B0FC68;
	return Rect(DAT_00B0CB78[button] * DAT_00B0CC38 + r.X, DAT_00B0CC24, r.Width, r.Height);
}

// FUNCTION: 0x006CFDB0
void __fastcall FUN_006CFDB0(int count)
{
	DAT_00B0CB54 = count;
	int x = DAT_00B0FC6C->X - DAT_00B0FC68->Width * count;
	int limit = DAT_00B0FC60->X + DAT_00B0FC60->Width;
	if (x > limit) {
		DAT_00B0CC2C = x;
	} else {
		DAT_00B0CC2C = limit;
	}
}

// FUNCTION: 0x006CFDF0
bool __fastcall FUN_006CFDF0(int value)
{
	return value >= DAT_00B0CC20 && value <= DAT_00B0CD28;
}

// FUNCTION: 0x006CFE10
int __fastcall FUN_006CFE10(int value)
{
	return value - DAT_00B0CC20 + 1;
}

extern int DAT_00B0CD24;
extern int DAT_00B0CB3C;
extern int DAT_00B0C1B8;
extern int DAT_00B0CB20;
extern int DAT_00B0CB68;
extern int DAT_00B0CC1C;
extern int DAT_00B0CB6C;
extern int DAT_00B0CC28;
extern int DAT_00B0CB38;

void __fastcall FUN_00637060(INoticeSink * sink);

// FUNCTION: 0x006CFE20
TabClass::TabClass(void) :
	FlasherTimer(0),
	MoneyFlashTimer(0),
	ThumbActive(true),
	IsToRedraw(true),
	unknown_byte_5546(true)
{
	DAT_00B0CD24 = FUN_006CFCC0("AttackMove");
	DAT_00B0CB3C = FUN_006CFCC0("Beacon");
	DAT_00B0C1B8 = FUN_006CFCC0("Cheer");
	DAT_00B0CB20 = FUN_006CFCC0("Deploy");
	DAT_00B0CB68 = FUN_006CFCC0("Guard");
	DAT_00B0CC1C = FUN_006CFCC0("PlanningMode");
	DAT_00B0CB6C = FUN_006CFCC0("Stop");
	DAT_00B0CC20 = FUN_006CFCC0("Team01");
	DAT_00B0CC28 = FUN_006CFCC0("Team02");
	DAT_00B0CD28 = FUN_006CFCC0("Team03");
	DAT_00B0CB38 = FUN_006CFCC0("TypeSelect");
	FUN_00637060(this);
}

// FUNCTION: 0x006D0260
void TabClass::One_Time(void)
{
	BASECLASS::One_Time();
}

// FUNCTION: 0x006D0270
void TabClass::DisposeOfArt(void)
{
	BASECLASS::DisposeOfArt();
	for (int i = 0; i < 25; i++) {
		FUN_006CFC80(i);
	}
}

// FUNCTION: 0x006D03A0
void TabClass::Init_IO(void)
{
	BASECLASS::Init_IO();

	if (!Unsorted::ArmageddonMode && FUN_0072F430()) {
		FUN_006D0FD0();
		ShapeSet const * shape = DAT_00B0FA90;

		CommandBarStatics::ButtonA.ID = 0xF0;
		CommandBarStatics::ButtonA.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		CommandBarStatics::ButtonA.IsOn = false;
		CommandBarStatics::ButtonA.IsToggleType = 0;
		CommandBarStatics::ButtonA.Set_Flags(5);
		Rect ra = *DAT_00B0FC60;
		CommandBarStatics::ButtonA.Set_Position(ra.X, ra.Y);
		CommandBarStatics::ButtonA.Set_Shape(shape, 0, 0);

		CommandBarStatics::ButtonB.ID = 0xF1;
		CommandBarStatics::ButtonB.ShapeDrawer = FileSystem::SIDEBAR_PAL;
		CommandBarStatics::ButtonB.IsOn = false;
		CommandBarStatics::ButtonB.IsToggleType = 0;
		CommandBarStatics::ButtonB.Set_Flags(5);
		Rect rb = *DAT_00B0FC64;
		CommandBarStatics::ButtonB.Set_Position(rb.X, rb.Y);
		CommandBarStatics::ButtonB.Set_Shape(shape, 0, 0);
	}
}

// FUNCTION: 0x006D04A0
void TabClass::FUN_006D04A0(void)
{
	for (int p = (int)CommandBarStatics::Buttons; p < (int)(CommandBarStatics::Buttons + 25); p += sizeof(ShapeButtonClass)) {
		((ShapeButtonClass *)p)->Zap();
		Add_A_Button(*(ShapeButtonClass *)p);
	}
}

// FUNCTION: 0x006D04D0
void TabClass::FUN_006D04D0(void)
{
	for (int p = (int)CommandBarStatics::Buttons; p < (int)(CommandBarStatics::Buttons + 25); p += sizeof(ShapeButtonClass)) {
		Remove_A_Button(*(ShapeButtonClass *)p);
	}
}

struct CCToolTip {
	static ToolTipManager * Instance;
};

static inline void Add_Button_Tip(ShapeButtonClass * button, char const * text)
{
	if (button) {
		ToolTip tip;
		tip.ID = button->ID;
		tip.Text = (int)text;
		tip.Region.X = button->X;
		tip.Region.Y = button->Y;
		tip.Region.Width = button->Width;
		tip.Region.Height = button->Height;
		CCToolTip::Instance->Remove(tip.ID);
		CCToolTip::Instance->Add(&tip);
	}
}

// FUNCTION: 0x006D04F0
bool TabClass::Activate(int control)
{
	bool ok = BASECLASS::Activate(control);
	if (ok) {
		if (IsSidebarActive) {
			if (ThumbActive) {
				FUN_006D1200();
			} else {
				Add_A_Button(CommandBarStatics::ButtonB);
				Add_Button_Tip(&CommandBarStatics::ButtonB, "Tip:ThumbClosed");
			}
		} else if (ThumbActive) {
			FUN_006D04D0();
			Remove_A_Button(CommandBarStatics::ButtonA);
			for (int i = 0; i < 25; i++) {
				ShapeButtonClass * b = FUN_006CFD40(i);
				if (b) {
					CCToolTip::Instance->Remove(b->ID);
				}
			}
			CCToolTip::Instance->Remove(0xF0);
		} else {
			Remove_A_Button(CommandBarStatics::ButtonB);
			CCToolTip::Instance->Remove(0xF1);
		}
		Flag_To_Redraw(GS_REDRAW_ALL);
	}
	return ok;
}


// FUNCTION: 0x006D0F00
void TabClass::Redraw_Tab(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
}
