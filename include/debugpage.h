// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include <windows.h>

struct DebugPage {
	char Text[80][25];
	int Attr[80][25];
	int Pad[2];
	int Index;
	int Field271C;
	int Field2720;
	int Field2724;
	int Field2728;
	int Columns;
	int Rows;
	bool Dirty;

	DebugPage(int index);
	void Reset(void);
	void Blank(void)
	{
		for (int row = 0; row < 25; row++) {
			for (int col = 0; col < 80; col++) {
				Text[col][row] = ' ';
				Attr[col][row] = 0;
			}
		}
	}
	void Refresh(int x, int y, int w, int h);
	void Clear(void)
	{
		Blank();
		Dirty = true;
		Refresh(0, 0, 80, 25);
	}
	int Get_Index(void);
	void Set_Cursor(int x, int y);
	void Set_Window(int x, int y, int w, int h);
	bool Is_Dirty(void);
	void Show(void);
	void Set_Pad(int value);
	void Fill(int x, int y, int w, int h, int attr);
	void Scroll_Vertical(int amount);
	void Scroll_Horizontal(int amount);
	void Print_Raw(char const * text);
	void Print_At(char const * text, int x, int y, int attr);
	void Print_At(wchar_t const * text, int x, int y, int attr);
	void Draw_Cell(HDC dc, int col, int row);
	char Translate(char c);
};
