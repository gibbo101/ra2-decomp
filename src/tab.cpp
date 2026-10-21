// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "tab.h"
#include "shapebtn.h"
#include <string.h>
#include <stdio.h>
#include "shapeset.h"
#include "tooltip.h"
#include "xmouse.h"
#include "draw.h"
#include "dsurface.h"
#include "_map.h"
#include "_rules.h"
#include "rules.h"
#include "voc.h"

#include <cmath>

// SYNTHETIC: GAMEMD 0x006CF990 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006CF9B0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006CF9E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006CFA00 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006CFA20 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006CFA40 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006CFA60 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006CFA80 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006CFAC0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006CFAF0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006CFB20 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006CFB50 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006CFB60 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006CFB80 SYMBOL
// _$E41

struct CommandBarStatics {
	static ShapeButtonClass Buttons[25];
	static ShapeButtonClass ButtonA;
	static ShapeButtonClass ButtonB;
};

// SYNTHETIC: GAMEMD 0x006CFBA0 SYMBOL
// _$E121
// SYNTHETIC: GAMEMD 0x006CFBD0 SYMBOL
// _$E118
ShapeButtonClass CommandBarStatics::Buttons[25];

// SYNTHETIC: GAMEMD 0x006CFC00 SYMBOL
// _$E126
// SYNTHETIC: GAMEMD 0x006CFC20 SYMBOL
// _$E124
ShapeButtonClass CommandBarStatics::ButtonA;

// SYNTHETIC: GAMEMD 0x006CFC40 SYMBOL
// _$E131
// SYNTHETIC: GAMEMD 0x006CFC60 SYMBOL
// _$E129
ShapeButtonClass CommandBarStatics::ButtonB;

extern void * DAT_00B0C148[];
extern bool DAT_00B0CBDC[];
extern char const * DAT_008427D0[11];
extern int DAT_008427CC;
extern int DAT_00B0CB78[];

// FUNCTION: GAMEMD 0x006CFC80
void __fastcall FUN_006CFC80(int index)
{
	if (DAT_00B0CBDC[index]) {
		delete DAT_00B0C148[index];
		DAT_00B0CBDC[index] = false;
	}
	DAT_00B0C148[index] = NULL;
}

// FUNCTION: GAMEMD 0x006CFCC0
int __fastcall FUN_006CFCC0(char const * name)
{
	for (int i = 0; i < 11; i++) {
		if (strcmp(DAT_008427D0[i], name) == 0) {
			return i;
		}
	}
	return DAT_008427CC;
}

// FUNCTION: GAMEMD 0x006CFD20
void __fastcall FUN_006CFD20(int button, int slot)
{
	DAT_00B0CB78[button] = slot;
}

// FUNCTION: GAMEMD 0x006CFD30
int __fastcall FUN_006CFD30(int button)
{
	return DAT_00B0CB78[button];
}

// FUNCTION: GAMEMD 0x006CFD40
ShapeButtonClass * __fastcall FUN_006CFD40(int button)
{
	int slot = DAT_00B0CB78[button];
	if (slot < 0 || slot >= 25) {
		return NULL;
	}
	ShapeButtonClass * result = &CommandBarStatics::Buttons[slot];
	return result;
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

// FUNCTION: GAMEMD 0x006CFD60
Rect __fastcall FUN_006CFD60(int button)
{
	Rect r = *DAT_00B0FC68;
	return Rect(DAT_00B0CB78[button] * DAT_00B0CC38 + r.X, DAT_00B0CC24, r.Width, r.Height);
}

// FUNCTION: GAMEMD 0x006CFDB0
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

// FUNCTION: GAMEMD 0x006CFDF0
bool __fastcall FUN_006CFDF0(int value)
{
	return value >= DAT_00B0CC20 && value <= DAT_00B0CD28;
}

// FUNCTION: GAMEMD 0x006CFE10
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

// FUNCTION: GAMEMD 0x006CFE20
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

// FUNCTION: GAMEMD 0x006D0260
void TabClass::One_Time(void)
{
	BASECLASS::One_Time();
}

// FUNCTION: GAMEMD 0x006D0270
void TabClass::DisposeOfArt(void)
{
	BASECLASS::DisposeOfArt();
	for (int i = 0; i < 25; i++) {
		FUN_006CFC80(i);
	}
}

// FUNCTION: GAMEMD 0x006D03A0
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

// FUNCTION: GAMEMD 0x006D04A0
void TabClass::FUN_006D04A0(void)
{
	for (int p = (int)CommandBarStatics::Buttons; p < (int)(CommandBarStatics::Buttons + 25); p += sizeof(ShapeButtonClass)) {
		((ShapeButtonClass *)p)->Zap();
		Add_A_Button(*(ShapeButtonClass *)p);
	}
}

// FUNCTION: GAMEMD 0x006D04D0
void TabClass::FUN_006D04D0(void)
{
	for (int p = (int)CommandBarStatics::Buttons; p < (int)(CommandBarStatics::Buttons + 25); p += sizeof(ShapeButtonClass)) {
		Remove_A_Button(*(ShapeButtonClass *)p);
	}
}

struct CCToolTip {
	static ToolTipManager * Instance;
};

void __fastcall FUN_006D09C0(ShapeButtonClass * button, char const * text);

// FUNCTION: GAMEMD 0x006D04F0
bool TabClass::Activate(int control)
{
	bool ok = BASECLASS::Activate(control);
	if (ok) {
		if (IsSidebarActive) {
			if (ThumbActive) {
				FUN_006D1200();
			} else {
				Add_A_Button(CommandBarStatics::ButtonB);
				FUN_006D09C0(&CommandBarStatics::ButtonB, "Tip:ThumbClosed");
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


namespace Unsorted { extern int MuteSWLaunches; }

extern Rect * DAT_00B0FC58;
extern int DAT_00886FA8;
extern int DAT_007F5BFC;
extern int DAT_00B0CD24;
extern int DAT_00B0CB3C;
extern int DAT_00B0C1B8;
extern int DAT_00B0CB20;
extern int DAT_00B0CB68;
extern int DAT_00B0CC1C;
extern int DAT_00B0CB6C;
extern int DAT_00B0CC20;
extern int DAT_00B0CD28;
extern int DAT_00B0CB38;

void FUN_00731AF0(void);
void FUN_00731A30(void);
void FUN_00730F30(void);
void FUN_00730AF0(void);
void FUN_00730D60(void);
void __fastcall FUN_00731A50(bool flag);
void __fastcall FUN_00731A70(bool flag);
void FUN_00730EA0(void);
int __fastcall FUN_00730A10(int group);
void __fastcall FUN_00731060(int group);
bool __fastcall FUN_00730990(int group);
void __fastcall FUN_007313A0(int group);
void __fastcall FUN_007311C0(int group);
void __fastcall FUN_00732950(void);
void __fastcall FUN_007310D0(int group);

inline void TabClass::Hide_Buttons(void)
{
	FUN_006D04D0();
	Remove_A_Button(CommandBarStatics::ButtonA);
	for (int i = 0; i < 25; i++) {
		ShapeButtonClass * b = FUN_006CFD40(i);
		if (b) {
			CCToolTip::Instance->Remove(b->ID);
		}
	}
	CCToolTip::Instance->Remove(0xF0);
}

inline void TabClass::Close_Thumb(void)
{
	if (ThumbActive) {
		Hide_Buttons();
		ThumbActive = false;
		Add_A_Button(CommandBarStatics::ButtonB);
		FUN_006D09C0(&CommandBarStatics::ButtonB, "Tip:ThumbClosed");
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}

inline void TabClass::Open_Thumb(void)
{
	if (!ThumbActive) {
		Remove_A_Button(CommandBarStatics::ButtonB);
		CCToolTip::Instance->Remove(0xF1);
		ThumbActive = true;
		FUN_006D1200();
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}

// FUNCTION: GAMEMD 0x006D0680
void TabClass::AI(KeyNumType & input, Point2D const & xy)
{
	if (MoneyFlashTimer == 1) {
		IsToRedraw = true;
		Flag_To_Redraw();
	}

	Rect area(DAT_00B0FC58->X, DAT_00B0FC58->Y, DAT_00886FA8, DAT_007F5BFC - 2);
	if (area.Is_Point_Within(xy)) {
		Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	}

	int key = input;
	if (key >= 0x80D6 && key < 0x80EF) {
		int command = (key & ~0x8000) - 0xD6;
		if (command == DAT_00B0CD24) {
			FUN_00731AF0();
		} else if (command == DAT_00B0CB3C) {
			FUN_00731A30();
		} else if (command == DAT_00B0C1B8) {
			FUN_00730F30();
		} else if (command == DAT_00B0CB20) {
			FUN_00730AF0();
		} else if (command == DAT_00B0CB68) {
			FUN_00730D60();
		} else if (command == DAT_00B0CC1C) {
			if (FUN_006CFD40(DAT_00B0CC1C)->IsOn) {
				FUN_00731A50(true);
			} else {
				FUN_00731A70(true);
			}
		} else if (command == DAT_00B0CB6C) {
			FUN_00730EA0();
		} else if (FUN_006CFDF0(command)) {
			int group = FUN_006CFE10(command);
			if (FUN_00730A10(group) == 0) {
				FUN_00731060(group);
			} else if (FUN_00730990(group)) {
				FUN_007313A0(group);
			} else {
				FUN_007311C0(group);
			}
		} else if (command == DAT_00B0CB38) {
			FUN_00732950();
		}
	} else if (key == 0x80F0) {
		VocClass::PlayGlobal(Rule->GUICloseSound, 1.0f, 0x2000);
		Close_Thumb();
	} else if (key == 0x80F1) {
		if (!Unsorted::MuteSWLaunches) {
			VocClass::PlayGlobal(Rule->GUIOpenSound, 1.0f, 0x2000);
			Open_Thumb();
		}
	} else if (key >= 0xC0D6 && key < 0xC0EF) {
		int command = (key & ~0xC000) - 0xD6;
		if (FUN_006CFDF0(command)) {
			FUN_007310D0(FUN_006CFE10(command));
		}
	}

	Credits.AI(false);
	BASECLASS::AI(input, xy);
}

// FUNCTION: GAMEMD 0x006D09C0
void __fastcall FUN_006D09C0(ShapeButtonClass * button, char const * text)
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

extern DSurface * DAT_00887300;
extern ShapeSet const * DAT_00B0FB08;
extern bool DAT_00B0B518;

// FUNCTION: GAMEMD 0x006D0E60
void TabClass::Draw_Credits_Tab(void)
{
	ShapeSet const * shape = DAT_00B0FB08;
	Draw_Shape(*DAT_00887300, *FileSystem::SIDEBAR_PAL, shape, 0, Point2D(0, 0), DAT_00887300->Get_Rect());
	DAT_00B0B518 = true;
}

// FUNCTION: GAMEMD 0x006D0EC0
void TabClass::Flash_Money(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
	MoneyFlashTimer = 7;
}

// FUNCTION: GAMEMD 0x006D0F00
void TabClass::Redraw_Tab(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
}

// FUNCTION: GAMEMD 0x006D0F10
void __cdecl FUN_006D0F10(void)
{
	bool loaded = false;
	for (int i = 0; i < 25; i++) {
		char name[256];
		sprintf(name, "Button%02d.SHP", i);
		DAT_00B0C148[i] = FileSystem::LoadWholeFileEx(name, loaded);
		DAT_00B0CBDC[i] = loaded;
	}
}

// FUNCTION: GAMEMD 0x006D0F70
void __cdecl FUN_006D0F70(void)
{
	for (int i = 0; i < 25; i++) {
		if (DAT_00B0CBDC[i]) {
			delete DAT_00B0C148[i];
			DAT_00B0CBDC[i] = false;
		}
		DAT_00B0C148[i] = NULL;
	}
}

// FUNCTION: GAMEMD 0x006D0FB0
void __fastcall FUN_006D0FB0(ShapeButtonClass * button, int id, int flags)
{
	button->ID = id;
	button->ShapeDrawer = FileSystem::SIDEBAR_PAL;
	button->Set_Flags(flags);
	button->IsOn = false;
	button->IsToggleType = 0;
}

// FUNCTION: GAMEMD 0x006D0FD0
void TabClass::FUN_006D0FD0(void)
{
	for (int p = (int)CommandBarStatics::Buttons; p < (int)(CommandBarStatics::Buttons + 25); p += sizeof(ShapeButtonClass)) {
		((ShapeButtonClass *)p)->ShapeButtonClass::Set_Shape(NULL, 0, 0);
	}

	Rect r = *DAT_00B0FC64;
	int limit = r.X + r.Width;
	for (int i = 0; i < 25; i++) {
		ShapeButtonClass * button = FUN_006CFD40(i);
		if (button) {
			r = FUN_006CFD60(i);
			if (r.X + r.Width <= limit) {
				button->ID = 0xD6 + i;
				button->ShapeDrawer = FileSystem::SIDEBAR_PAL;
				button->IsOn = false;
				button->IsToggleType = 0;
				button->Set_Flags(5);
				button->Set_Position(r.X, r.Y);
				button->Set_Shape((ShapeSet const *)DAT_00B0C148[i], 0, 0);
			}
		}
	}

	ShapeButtonClass * planning = FUN_006CFD40(DAT_00B0CC1C);
	if (planning) {
		planning->IsToggleType = 1;
		planning->ReflectButtonState = true;
	}

	for (int t = DAT_00B0CC20; t <= DAT_00B0CD28; t++) {
		ShapeButtonClass * button = FUN_006CFD40(t);
		if (button) {
			button->Set_Flags(0x55);
		}
	}
}

// FUNCTION: GAMEMD 0x006D1130
void __cdecl FUN_006D1130(void)
{
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

// FUNCTION: GAMEMD 0x006D1200
void TabClass::FUN_006D1200(void)
{
	FUN_006D04A0();
	Add_A_Button(CommandBarStatics::ButtonA);

	ShapeButtonClass * button = FUN_006CFD40(DAT_00B0CD24);
	FUN_006D09C0(button, "Tip:AttackMove");
	button = FUN_006CFD40(DAT_00B0CB3C);
	FUN_006D09C0(button, "Tip:Beacon");
	button = FUN_006CFD40(DAT_00B0C1B8);
	FUN_006D09C0(button, "Tip:Cheer");
	button = FUN_006CFD40(DAT_00B0CB20);
	FUN_006D09C0(button, "Tip:Deploy");
	button = FUN_006CFD40(DAT_00B0CB68);
	FUN_006D09C0(button, "Tip:Guard");
	button = FUN_006CFD40(DAT_00B0CC1C);
	FUN_006D09C0(button, "Tip:PlanningMode");
	button = FUN_006CFD40(DAT_00B0CB6C);
	FUN_006D09C0(button, "Tip:Stop");
	button = FUN_006CFD40(DAT_00B0CC20);
	FUN_006D09C0(button, "Tip:Team01");
	button = FUN_006CFD40(DAT_00B0CC28);
	FUN_006D09C0(button, "Tip:Team02");
	button = FUN_006CFD40(DAT_00B0CD28);
	FUN_006D09C0(button, "Tip:Team03");
	button = FUN_006CFD40(DAT_00B0CB38);
	FUN_006D09C0(button, "Tip:TypeSelect");
	FUN_006D09C0(&CommandBarStatics::ButtonA, "Tip:ThumbOpen");
}

// FUNCTION: GAMEMD 0x006D14F0
void TabClass::FUN_006D14F0(void)
{
	for (int p = (int)CommandBarStatics::Buttons; p < (int)(CommandBarStatics::Buttons + 25); p += sizeof(ShapeButtonClass)) {
		Remove_A_Button(*(ShapeButtonClass *)p);
	}
	Remove_A_Button(CommandBarStatics::ButtonA);
	for (int i = 0; i < 25; i++) {
		ShapeButtonClass * b = FUN_006CFD40(i);
		if (b) {
			CCToolTip::Instance->Remove(b->ID);
		}
	}
	CCToolTip::Instance->Remove(0xF0);
}

// FUNCTION: GAMEMD 0x006D1570
void TabClass::FUN_006D1570(void)
{
	Add_A_Button(CommandBarStatics::ButtonB);
	FUN_006D09C0(&CommandBarStatics::ButtonB, "Tip:ThumbClosed");
}

// FUNCTION: GAMEMD 0x006D15F0
void TabClass::FUN_006D15F0(void)
{
	Remove_A_Button(CommandBarStatics::ButtonB);
	CCToolTip::Instance->Remove(0xF1);
}

// FUNCTION: GAMEMD 0x006D1610
void TabClass::FUN_006D1610(void)
{
	if (!ThumbActive) {
		Remove_A_Button(CommandBarStatics::ButtonB);
		CCToolTip::Instance->Remove(0xF1);
		ThumbActive = true;
		FUN_006D1200();
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}

// FUNCTION: GAMEMD 0x006D1660
void TabClass::FUN_006D1660(void)
{
	if (ThumbActive) {
		FUN_006D14F0();
		ThumbActive = false;
		Add_A_Button(CommandBarStatics::ButtonB);
		FUN_006D09C0(&CommandBarStatics::ButtonB, "Tip:ThumbClosed");
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}

void __fastcall FUN_00723EA0(ShapeButtonClass * button);
void __fastcall FUN_00723EB0(ShapeButtonClass * button);

// FUNCTION: GAMEMD 0x006D1770
bool TabClass::INoticeSink_Unknown(DWORD unknown, DWORD unknown2)
{
	if (unknown == 0x11C7) {
		ShapeButtonClass * b = FUN_006CFD40(DAT_00B0CC1C);
		if (b && !b->IsOn) {
			FUN_00723EA0(b);
		}
		return true;
	}
	if (unknown == 0x11C8) {
		ShapeButtonClass * b = FUN_006CFD40(DAT_00B0CC1C);
		if (b && b->IsOn) {
			FUN_00723EB0(b);
		}
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x006D1800
char const * TabClass::Help_Text(int id)
{
	return BASECLASS::Help_Text(id);
}

// FUNCTION: GAMEMD 0x006D1810
// ToolTip::ToolTip

extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007F4180;
extern const double DAT_007F4188;
extern double __cdecl FUN_004CAC40(double);

// SYNTHETIC: GAMEMD 0x006D1830 SYMBOL
// _$E136
double DAT_00B0CD78 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x006D1860 SYMBOL
// _$E139
double DAT_00B0CEB8 = DAT_00B0CD78 / DAT_007F4180;

// SYNTHETIC: GAMEMD 0x006D1880 SYMBOL
// _$E142
double DAT_00B0CEA0 = DAT_007E1718 / DAT_00B0CEB8;

// SYNTHETIC: GAMEMD 0x006D18A0 SYMBOL
// _$E145
double DAT_00B0CE98 = DAT_007E1720 * DAT_007F4188;

extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);

struct TabShortPair {
	unsigned short A, B;
	TabShortPair() { A = 0; B = 0; }
};

struct TabIntTriple {
	int A, B, C;
	TabIntTriple() { A = 0; B = 0; C = 0; }
};

struct TabIntQuad {
	int A, B, C, D;
	TabIntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

struct TabIntInit {
	int A, B, C;
	TabIntInit() { A = 0x80; B = 0x80; C = 0; }
};

// SYNTHETIC: GAMEMD 0x006D18C0 SYMBOL
// _$E148
double DAT_00B0CD88 = DAT_007E1728 * DAT_007F4188;

// SYNTHETIC: GAMEMD 0x006D18E0 SYMBOL
// _$E151
double DAT_00B0CD50 = DAT_007E1730 * DAT_007F4188;

// SYNTHETIC: GAMEMD 0x006D1900 SYMBOL
// _$E154
int DAT_00B0CEC0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B0CD50 - DAT_00B0CD88) * DAT_00B0CD78));

// SYNTHETIC: GAMEMD 0x006D1940 SYMBOL
// _$E157
double DAT_00B0CD90 = FUN_004CADE0(DAT_00B0CEC0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x006D1970 SYMBOL
// _$E160
double DAT_00B0CE20 = FUN_004CADE0((DAT_00B0CEC0 * 2) / DAT_00B0CD78);

// SYNTHETIC: GAMEMD 0x006D19A0 SYMBOL
// _$E163
int DAT_00B0CEB4 = (int)(DAT_00B0CEC0 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x006D19D0 SYMBOL
// _$E166
TabShortPair DAT_00B0CD74;

// SYNTHETIC: GAMEMD 0x006D19E0 SYMBOL
// _$E169
TabIntTriple DAT_00B0CE08;

// SYNTHETIC: GAMEMD 0x006D1A00 SYMBOL
// _$E172
TabIntInit DAT_00B0CEA8;

// SYNTHETIC: GAMEMD 0x006D1A20 SYMBOL
// _$E175
TabIntQuad DAT_00B0CD60;
