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
