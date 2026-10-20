// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msglist.h"
#include "_keyboar.h"
#include "_rules.h"
#include "_surface.h"
#include "_timer.h"
#include "_xmouse.h"
#include "dbgprint.h"
#include "dialog.h"
#include "font.h"
#include "keyboard.h"
#include "rules.h"
#include "scheme.h"
#include "surface.h"
#include "txtlabel.h"
#include "vector.h"
#include <cstdio>
#include <math.h>

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
extern const double DAT_007EEC78;
extern const double DAT_007EEC80;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x005D37E0 $E17
double DAT_00ABFC30 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// FUNCTION: 0x005D3810 $E20
double DAT_00ABFC80 = DAT_00ABFC30 / DAT_007EEC78;

// FUNCTION: 0x005D3830 $E23
double DAT_00ABFC68 = DAT_007E1718 / DAT_00ABFC80;

// FUNCTION: 0x005D3850 $E26
double DAT_00ABFC60 = DAT_007E1720 * DAT_007EEC80;

// FUNCTION: 0x005D3870 $E29
double DAT_00ABFC38 = DAT_007E1728 * DAT_007EEC80;

// FUNCTION: 0x005D3890 $E32
double DAT_00ABFC10 = DAT_007E1730 * DAT_007EEC80;

// FUNCTION: 0x005D38B0 $E35
int DAT_00ABFC88 = (int)(FUN_004CAD50(DAT_00ABFC10 - DAT_00ABFC38) * DAT_007E1738 * DAT_00ABFC30);

// FUNCTION: 0x005D38F0 $E38
double DAT_00ABFC40 = FUN_004CADE0(DAT_00ABFC88 * DAT_007E1740);

// FUNCTION: 0x005D3920 $E41
double DAT_00ABFC58 = FUN_004CADE0((DAT_00ABFC88 * 2) / DAT_00ABFC30);

// FUNCTION: 0x005D3950 $E44
int DAT_00ABFC7C = (int)(DAT_00ABFC88 * 4 + DAT_007E1738);

// FUNCTION: 0x005D3980 $E47
ShortPair DAT_00ABFC28;

// FUNCTION: 0x005D3990 $E50
IntTriple DAT_00ABFC48;

// FUNCTION: 0x005D3A40
MessageListClass::~MessageListClass(void)
{
	Init(0,0,0,0,0,0,0,0,0,0);
}

// FUNCTION: 0x005D3F60
char * MessageListClass::Get_Message(int id)
{
	TextLabelClass * gadg;

	if (MessageList) {
		gadg = MessageList;
		while (gadg) {
			if ((int)gadg->UserData2 == id) {
				return(gadg->Text);
			}
			gadg = (TextLabelClass *)gadg->Get_Next();
		}
	}

	return(NULL);
}

// FUNCTION: 0x005D3F90
TextLabelClass * MessageListClass::Get_Label(int id)
{
	TextLabelClass * gadg;

	if (MessageList) {
		gadg = MessageList;
		while (gadg) {
			if ((int)gadg->UserData2 == id) {
				return(gadg);
			}
			gadg = (TextLabelClass *)gadg->Get_Next();
		}
	}

	return(NULL);
}

// FUNCTION: 0x005D41E0
void MessageListClass::Set_Edit_Focus (void)
{
	if (IsEdit) EditLabel->Set_Focus();
}

// FUNCTION: 0x005D41F0
bool MessageListClass::Has_Edit_Focus (void)
{
	if (IsEdit){
		return(EditLabel->Has_Focus());
	}else{
		return(false);
	}
}

// FUNCTION: 0x005D4410
void MessageListClass::Set_Edit_Color(int color)
{
	if (IsEdit) {
		EditLabel->Color = color;
	}
}

// FUNCTION: 0x005D4AA0
int MessageListClass::Num_Messages(void)
{
	GadgetClass * gadg;
	int num;

	num = 0;

	if (MessageList) {
		gadg = MessageList;
		while (gadg) {
			num++;
			gadg = (GadgetClass *)gadg->Get_Next();
		}
	}

	if (IsEdit && AdjustEdit) {
		num++;
	}

	return(num);
}

// FUNCTION: 0x005D4BF0
void MessageListClass::Compute_Y(void)
{
	GadgetClass * gadg;
	int y;

	if (IsEdit && AdjustEdit) {
		y = MessagePos.MessageY + Height;
	}
	else {
		y = MessagePos.MessageY;
	}
	if (MessageList) {
		gadg = MessageList;
		while (gadg) {
			gadg->Y = y;
			gadg = (GadgetClass *)gadg->Get_Next();
			y += Height;
		}
	}
}
