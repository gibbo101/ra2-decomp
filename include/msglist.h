// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "keyboard.h"

#include "dialog.hh"

#define	MAX_MESSAGE_LENGTH		110 + 2

#define	MAX_NUM_MESSAGES			14

class TextLabelClass;

class MessageListClass {
	public:
		MessageListClass (void);
		~MessageListClass (void);

		void Init (int x, int y, int max_msg, int maxchars, int height,
			int edit_x, int edit_y, bool overflow_on, int over_start,
			int over_end, int width = 640);
		TextLabelClass * Add_Message (wchar_t const * name, int id, wchar_t const * txt,
			int color, TextPrintType style, int timeout, bool singleplayer);
		int Concat_Message (wchar_t const * name, int id, wchar_t const * txt, int timeout);

		char * Get_Message (int id);
		TextLabelClass * Get_Label (int id);

		TextLabelClass * Add_Edit(int color, TextPrintType style,
			char *to, char cursor = 0, int width = 640);
		void Remove_Edit (void);
		char * Get_Edit_Buf (void);
		wchar_t * Get_Overflow_Buf (void) {return(OverflowBuf);}
		void Clear_Overflow_Buf (void) {OverflowBuf[0] = 0;}
		int Is_Edit(void) {return(IsEdit);}
		void Set_Edit_Color(int color);

		int Manage (void);
		int Input (KeyNumType &input);
		void Draw(void);
		int Num_Messages(void);
		void Set_Width(int width);
		void Set_Edit_Focus(void);
		bool Has_Edit_Focus(void);

	private:
		int Trim_Message(char *dest, char *src, int min_chars, int max_chars,
			int scandir);

		void Compute_Y(void);

		TextLabelClass * MessageList;

		struct MessagePosStruct {
			int MessageX;
			int MessageY;
		};

		MessagePosStruct MessagePos;
		int MaxMessages;
		int MaxChars;
		int Height;

		bool EnableOverflow;
		bool IsEdit;
		bool AdjustEdit;

		struct EditPosStruct {
			int EditX;
			int EditY;
		};

		EditPosStruct EditPos;
		TextLabelClass *EditLabel;
		wchar_t EditBuf[MAX_MESSAGE_LENGTH + 50];
		wchar_t OverflowBuf[MAX_MESSAGE_LENGTH + 50];
		int EditCurPos;
		int EditInitPos;
		wchar_t CursorChar;
		int OverflowStart;
		int OverflowEnd;
		int Width;

		wchar_t MessageBuffers[MAX_NUM_MESSAGES][MAX_MESSAGE_LENGTH + 50];
		wchar_t BufferAvail[MAX_NUM_MESSAGES];
};
