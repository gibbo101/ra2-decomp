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
#include "bitfont.h"
#include "dsurface.h"
#include <cstdio>
#include <math.h>

void FUN_00778170(void);
void FUN_00778180(void);
void FUN_007781A0(void);
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

// SYNTHETIC: GAMEMD 0x005D37E0 SYMBOL
// _$E37
double DAT_00ABFC30 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// SYNTHETIC: GAMEMD 0x005D3810 SYMBOL
// _$E40
double DAT_00ABFC80 = DAT_00ABFC30 / DAT_007EEC78;

// SYNTHETIC: GAMEMD 0x005D3830 SYMBOL
// _$E43
double DAT_00ABFC68 = DAT_007E1718 / DAT_00ABFC80;

// SYNTHETIC: GAMEMD 0x005D3850 SYMBOL
// _$E46
double DAT_00ABFC60 = DAT_007E1720 * DAT_007EEC80;

// SYNTHETIC: GAMEMD 0x005D3870 SYMBOL
// _$E49
double DAT_00ABFC38 = DAT_007E1728 * DAT_007EEC80;

// SYNTHETIC: GAMEMD 0x005D3890 SYMBOL
// _$E52
double DAT_00ABFC10 = DAT_007E1730 * DAT_007EEC80;

// SYNTHETIC: GAMEMD 0x005D38B0 SYMBOL
// _$E55
int DAT_00ABFC88 = (int)(FUN_004CAD50(DAT_00ABFC10 - DAT_00ABFC38) * DAT_007E1738 * DAT_00ABFC30);

// SYNTHETIC: GAMEMD 0x005D38F0 SYMBOL
// _$E58
double DAT_00ABFC40 = FUN_004CADE0(DAT_00ABFC88 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005D3920 SYMBOL
// _$E61
double DAT_00ABFC58 = FUN_004CADE0((DAT_00ABFC88 * 2) / DAT_00ABFC30);

// SYNTHETIC: GAMEMD 0x005D3950 SYMBOL
// _$E64
int DAT_00ABFC7C = (int)(DAT_00ABFC88 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005D3980 SYMBOL
// _$E67
ShortPair DAT_00ABFC28;

// SYNTHETIC: GAMEMD 0x005D3990 SYMBOL
// _$E70
IntTriple DAT_00ABFC48;

struct HalfTriple {
	int A, B, C;
	HalfTriple() { C = 0; A = 0x80; B = 0x80; }
};

// SYNTHETIC: GAMEMD 0x005D39B0 SYMBOL
// _$E73
HalfTriple DAT_00ABFC70;

// FUNCTION: GAMEMD 0x005D39D0
MessageListClass::MessageListClass(void)
{
	int i;

	MessageList = 0;
	MessagePos.MessageX = 0;
	MessagePos.MessageY = 0;
	MaxMessages = 0;
	MaxChars = 0;
	Height = 0;
	EnableOverflow = 0;
	IsEdit = 0;
	AdjustEdit = 0;
	EditPos.EditX = 0;
	EditPos.EditY = 0;
	EditLabel = 0;
	EditCurPos = 0;
	EditInitPos = 0;
	CursorChar = 0;
	OverflowStart = 0;
	OverflowEnd = 0;
	Width = 0;
	EditBuf[0] = 0;
	OverflowBuf[0] = 0;
	for (i = 0; i < MAX_NUM_MESSAGES; i++) {
		BufferAvail[i] = 1;
	}
}

// FUNCTION: GAMEMD 0x005D3A40
MessageListClass::~MessageListClass(void)
{
	Init(0,0,0,0,0,0,0,0,0,0);
}

// FUNCTION: GAMEMD 0x005D3A60
void MessageListClass::Init(int x, int y, int max_msg, int maxchars,
	int height, int edit_x, int edit_y, bool overflow_on, int over_start,
	int over_end, int width)
{
	TextLabelClass * txtlabel;
	int i;

	Width = width - 8;
	DebugString("MessageListClass::Init(Width: %d)\n", Width);

	txtlabel = MessageList;
	while (txtlabel) {
		MessageList = (TextLabelClass *)txtlabel->Remove();
		delete txtlabel;
		txtlabel = MessageList;
	}

	for (i = 0; i < MAX_NUM_MESSAGES; i++) {
		BufferAvail[i] = 1;
	}

	if (IsEdit) {
		delete EditLabel;
		EditLabel = 0;
	}

	MessageList = 0;
	MessagePos.MessageX = x;
	MessagePos.MessageY = y;
	MaxMessages = max_msg;
	if (MaxMessages > MAX_NUM_MESSAGES)
		MaxMessages = MAX_NUM_MESSAGES;
	MaxChars = maxchars;
	if (MaxChars > MAX_MESSAGE_LENGTH)
		MaxChars = MAX_MESSAGE_LENGTH;
	Height = 19;
	EnableOverflow = overflow_on;
	IsEdit = 0;
	if (edit_x == -1 || edit_y == -1) {
		AdjustEdit = 1;
		EditPos.EditX = x;
		EditPos.EditY = y;
	}
	else {
		AdjustEdit = 0;
		EditPos.EditX = edit_x;
		EditPos.EditY = edit_y;
	}
	EditLabel = 0;
	EditBuf[0] = 0;
	OverflowBuf[0] = 0;
	EditCurPos = 0;
	EditInitPos = 0;
	CursorChar = 0;
	OverflowStart = over_start;
	OverflowEnd = over_end;
	if (OverflowEnd >= MaxChars) {
		OverflowEnd = MaxChars - 1;
	}
	if (OverflowStart >= OverflowEnd) {
		OverflowStart = OverflowEnd - 1;
	}
}

// FUNCTION: GAMEMD 0x005D3F60
wchar_t * MessageListClass::Get_Message(int id)
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

// FUNCTION: GAMEMD 0x005D3F90
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

// FUNCTION: GAMEMD 0x005D3FC0
int MessageListClass::Concat_Message(wchar_t const * name, int id, wchar_t const * txt, int timeout)
{
	int min_chars;
	int max_chars;
	wchar_t * msg;
	TextLabelClass * tlabel = NULL;
	int found;

	if (name == NULL || txt == NULL || !EnableOverflow) {
		return(0);
	}

	found = 0;
	if (MessageList) {
		tlabel = MessageList;
		while (tlabel) {
			if ((int)tlabel->UserData2 == id &&
				!memcmp(tlabel->Text, name, wcslen(name) * 2)) {
				found = 1;
				break;
			}
			tlabel = (TextLabelClass *)tlabel->Get_Next();
		}
	}
	if (!found) {
		return(0);
	}

	msg = tlabel->Text + wcslen(name) + 1;

	if ((wcslen(msg) + wcslen(txt)) < (unsigned)MaxChars) {
		wchar_t * concat_test = new wchar_t [MaxChars + 1];
		BitFont * font = Font_From_TPF(tlabel->Style);
		int name_width = font->FUN_00433ED0(tlabel->Text, 0) - font->FUN_00433ED0(msg, 0);
		int width;

		wcscpy(concat_test, msg);
		wcscat(concat_test, txt);
		width = font->FUN_00433ED0(concat_test, 0) + name_width;
		min_chars = 10;
		while (width >= Width) {
			max_chars = wcslen(msg);
			if (max_chars < min_chars) {
				max_chars = min_chars;
			}
			Trim_Message(NULL, msg, min_chars, max_chars, 0);
			wcscpy(concat_test, msg);
			wcscat(concat_test, txt);
			width = font->FUN_00433ED0(concat_test, 0) + name_width;
		}
		delete [] concat_test;
		wcscat(msg, txt);
	}
	else {
		min_chars = (wcslen(msg) + wcslen(txt)) - MaxChars;
		max_chars = wcslen(msg);
		if (max_chars < min_chars) {
			max_chars = min_chars;
		}
		Trim_Message(NULL, msg, min_chars, max_chars, 0);
		wcscat(msg, txt);
	}

	if (timeout == -1) {
		tlabel->UserData1 = 0;
	}
	else {
		tlabel->UserData1 = TickCount + timeout;
	}
	return(1);
}

// FUNCTION: GAMEMD 0x005D41E0
void MessageListClass::Set_Edit_Focus (void)
{
	if (IsEdit) EditLabel->Set_Focus();
}

// FUNCTION: GAMEMD 0x005D41F0
bool MessageListClass::Has_Edit_Focus (void)
{
	if (IsEdit){
		return(EditLabel->Has_Focus());
	}else{
		return(false);
	}
}

// FUNCTION: GAMEMD 0x005D4210
TextLabelClass * MessageListClass::Add_Edit(int color,
	TextPrintType style, wchar_t * to, char cursor, int width)
{
	int i;
	TextLabelClass * txtlabel;

	if (IsEdit) {
		EditLabel->Set_Focus();
		return(NULL);
	}

	if (AdjustEdit && ((Num_Messages() + 1) > MaxMessages)) {
		txtlabel = MessageList;
		MessageList = (TextLabelClass *)txtlabel->Remove();
		for (i = 0; i < MAX_NUM_MESSAGES; i++) {
			if (txtlabel->Text == MessageBuffers[i])
				BufferAvail[i] = 1;
		}
		delete txtlabel;
	}

	if (!to) {
		to = (wchar_t *)L"";
	}
	CursorChar = cursor;
	memset(EditBuf, 0, sizeof(EditBuf));
	wcscpy(EditBuf, to);
	OverflowBuf[0] = 0;
	EditCurPos = EditInitPos = wcslen(to);
	EditLabel = new TextLabelClass(EditBuf, EditPos.EditX, EditPos.EditY,
		color, (TextPrintType)(style | 0x8000));
	if (width > 0) {
		EditLabel->PixWidth = width;
	} else {
		EditLabel->PixWidth = Width;
	}
	if (EditLabel) {
		IsEdit = 1;
		EditLabel->Set_Focus();
	}
	else {
		IsEdit = 0;
	}
	FUN_00778180();
	if (AdjustEdit) {
		Compute_Y();
		MaxMessages--;
	}
	return(EditLabel);
}

// FUNCTION: GAMEMD 0x005D4390
void MessageListClass::Remove_Edit(void)
{
	FUN_007781A0();
	if (IsEdit) {
		IsEdit = 0;
		delete EditLabel;
		FUN_00778170();
		if (AdjustEdit) {
			Compute_Y();
			MaxMessages++;
		}
	}
}

// FUNCTION: GAMEMD 0x005D4400
wchar_t * MessageListClass::Get_Edit_Buf(void)
{
	return(EditBuf + EditInitPos);
}

// FUNCTION: GAMEMD 0x005D4410
void MessageListClass::Set_Edit_Color(int color)
{
	if (IsEdit) {
		EditLabel->Color = color;
	}
}

// FUNCTION: GAMEMD 0x005D4430
int MessageListClass::Manage(void)
{
	TextLabelClass * txtlabel;
	TextLabelClass * next;
	int changed = 0;
	int i;

	txtlabel = MessageList;
	while (txtlabel) {
		if (txtlabel->UserData1 != 0 && TickCount > (int)txtlabel->UserData1) {
			next = (TextLabelClass *)txtlabel->Get_Next();
			MessageList = (TextLabelClass *)txtlabel->Remove();
			for (i = 0; i < MAX_NUM_MESSAGES; i++) {
				if (txtlabel->Text == MessageBuffers[i]) {
					BufferAvail[i] = 1;
				}
			}
			delete txtlabel;
			changed = 1;
			txtlabel = next;
		}
		else {
			txtlabel = (TextLabelClass *)txtlabel->Get_Next();
		}
	}

	if (changed) {
		Compute_Y();
	}
	return(changed);
}

// FUNCTION: GAMEMD 0x005D4510
int MessageListClass::Input(KeyNumType &input, wchar_t definput)
{
	int retcode = 0;
	int ascii;
	int numchars;
	BitFont * font;
	int width;

	if (definput == 0) {
		if (input == KN_NONE) {
			return(0);
		}

		if ((input & (~KN_RLSE_BIT)) == KN_LMOUSE ||
			(input & (~KN_RLSE_BIT)) == KN_RMOUSE) {
			if ((input & 2) && IsEdit) {
				Remove_Edit();
				return(2);
			}
			return(0);
		}
	}

	if (IsEdit) {
		if (definput == 0) {
			ascii = Keyboard->FUN_0054F530(input) & 0x00ff;

			if ((input & WWKEY_VK_BIT) && ascii >= '0' && ascii <= '9') {
				input = (KeyNumType)(input & ~WWKEY_VK_BIT);
			} else {
				if ((!(input & WWKEY_VK_BIT) && !(input & KN_BUTTON) && ascii >= ' ')
					|| (input & 0xff) == (KN_RETURN & 0xff)
					|| (input & 0xff) == (KN_BACKSPACE & 0xff)
					|| (input & 0xff) == (KN_ESC & 0xff)
					|| (char)ascii == 8) {
				} else {
					input = KN_NONE;
					return(0);
				}
			}
		} else {
			ascii = definput & 0xffff;
		}

		switch (ascii) {
			case KA_ESC & 0xff:
				Remove_Edit();
				retcode = 2;
				input = KN_NONE;
				break;

			case KA_RETURN & 0xff:
				if (EditCurPos < EditInitPos) {
					EditCurPos = EditInitPos;
				}
				while (EditBuf[EditCurPos] != 0 && (EditCurPos - EditInitPos) < (MaxChars - 1)) {
					EditCurPos++;
				}
				if (EditCurPos == EditInitPos) {
					Remove_Edit();
					retcode = 2;
					input = KN_NONE;
					break;
				}
				if ((EditCurPos - EditInitPos) < (MaxChars - 1)) {
					EditBuf[EditCurPos] = ' ';
					EditCurPos++;
					EditBuf[EditCurPos] = 0;
				}
				Remove_Edit();
				retcode = 3;
				input = KN_NONE;
				break;

			case KA_BACKSPACE & 0xff:
				if (input != 8) {
					if (EditCurPos > EditInitPos) {
						EditCurPos--;
						EditBuf[EditCurPos] = 0;
						retcode = 2;
					}
				}
				input = KN_NONE;
				EditLabel->Set_Focus();
				break;

			default:
				EditLabel->Set_Focus();
				if (ascii > 0x1f && ascii < 0x10000) {
					while (EditBuf[EditCurPos] != 0 && (EditCurPos - EditInitPos) < (MaxChars - 1)) {
						EditCurPos++;
					}
					if ((EditCurPos - EditInitPos) < (MaxChars - 1)) {
						EditBuf[EditCurPos] = (wchar_t)ascii;
						EditCurPos++;
						EditBuf[EditCurPos] = 0;
						retcode = 1;
						font = Font_From_TPF(EditLabel->Style);
						width = font->FUN_00433ED0(EditBuf, 0);
						if (width < Width - 10) {
							goto key_done;
						}
						EditCurPos--;
						EditBuf[EditCurPos] = 0;
					}
					if (EnableOverflow) {
						numchars = Trim_Message(OverflowBuf, EditBuf + EditInitPos,
							OverflowStart, OverflowEnd, 1);
						EditCurPos -= numchars;
						EditBuf[EditCurPos] = (wchar_t)ascii;
						EditCurPos++;
						EditBuf[EditCurPos] = 0;
						retcode = 4;
					} else {
						retcode = 0;
					}
				}
key_done:
				input = KN_NONE;
				break;
		}
	}
	return(retcode);
}

// FUNCTION: GAMEMD 0x005D49A0
void MessageListClass::Draw(void)
{
	wchar_t txt[2] = {0,0};

	if (IsEdit) {
		Hide_Mouse();
		EditLabel->Draw_Me(true);
		BitFont * font = Font_From_TPF(EditLabel->Style);
		int textwidth = font->FUN_00433ED0(EditLabel->Text, 0);
		if (CursorChar && (EditCurPos - EditInitPos) < (MaxChars - 1) && EditLabel->Has_Focus()) {
			txt[0] = CursorChar;
			Point2D ret;
			Point2D pt(EditLabel->X + textwidth, EditLabel->Y);
			Fancy_Text_Print_Wide(&ret, txt, DSurface::Temp, &DSurface::Temp->Get_Rect(), &pt,
				ColorSchemes[EditLabel->Color], 0, EditLabel->Style);
		}
		Show_Mouse();
	}

	if (MessageList) {
		MessageList->Draw_All();
	}
}

// FUNCTION: GAMEMD 0x005D4AA0
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

// FUNCTION: GAMEMD 0x005D4AD0
void MessageListClass::Set_Width(int width)
{
	GadgetClass * gadg;

	width = width - 8;
	Width = width;
	DebugString("MessageListClass::Set_Width(%d)\n", width);

	if (MessageList) {
		gadg = MessageList;
		while (gadg) {
			((TextLabelClass *)gadg)->PixWidth = width;
			gadg = (GadgetClass *)gadg->Get_Next();
		}
	}
	if (IsEdit) {
		EditLabel->PixWidth = width;
	}
}

// FUNCTION: GAMEMD 0x005D4BF0
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
