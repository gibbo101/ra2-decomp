// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "motlib.h"
#include "wwfile.h"
#include "voxel.hh"
#include "voc.h"
#include "audioctl.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <math.h>

// FUNCTION: 0x005BBF90 $E2
// FUNCTION: 0x005BBFC0 $E5
// FUNCTION: 0x005BBFE0 $E8
// FUNCTION: 0x005BC000 $E11
// FUNCTION: 0x005BC020 $E14
// FUNCTION: 0x005BC040 $E17
// FUNCTION: 0x005BC060 $E20
// FUNCTION: 0x005BC0A0 $E23
// FUNCTION: 0x005BC0D0 $E26
// FUNCTION: 0x005BC100 $E29
// FUNCTION: 0x005BC130 $E32

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

struct DebugPoint {
	int X;
	int Y;
	DebugPoint * Cell_Origin(int col, int row);
};

extern int DAT_00ABF1D8;
extern int DAT_00ABF1E0;
extern int DAT_00ABF188;
extern int DAT_00ABF18C;
extern int DAT_00ABF190;
extern int DAT_00ABF200;
extern int DAT_00ABF204;
extern int DAT_00ABF208;
extern DebugPage * DAT_00ABF168[8];
extern HINSTANCE DAT_00ABF230;
extern HWND DAT_00ABF234;
extern bool DAT_00ABF238;
extern bool DAT_00ABF239;
extern bool DAT_00ABF23A;
extern DebugPage * DAT_00ABF23C;
extern HWND DAT_00ABF240;
extern HGDIOBJ DAT_00ABF244;
extern TEXTMETRIC DAT_00ABF1C8;
extern void FUN_005BC530(void);
extern void __fastcall FUN_0053E420(HWND);
extern void __fastcall FUN_0053E3C0(HWND);
extern int DAT_00ABF128;
extern int DAT_00ABF110;
extern BOOL CALLBACK FUN_005BC6E0(HWND, UINT, WPARAM, LPARAM);
extern AudioController DAT_00ABF198;
extern AudioController * DAT_00ABF224;
extern int DAT_00ABF220;
extern int DAT_00ABF228;
extern int DAT_00ABF22C;
extern int DAT_00ABF21C;
extern int DAT_00B1D4B0;
extern DebugPage * DAT_00ABF17C;
extern DebugPage * DAT_00ABF180;
extern DebugPage * DAT_00ABF184;
extern void __fastcall FUN_005BCC90(DebugPage * page);
extern void __fastcall FUN_00752480(int index, int a, int b);
struct AudioStream {
	void FUN_00407A90(void);
	void FUN_00407F40(void);
};
struct AudioSource {
	AudioStream * FUN_00407860(void);
};
extern AudioSource * __stdcall FUN_00407010(int a, int b);
extern int __fastcall FUN_00407B60(AudioStream * stream, char const * name, int flag);
extern void FUN_00406EE0(void);
extern AudioStream * DAT_00ABF114[5];

// FUNCTION: 0x005BC140
void FUN_005BC140(void)
{
	DAT_00ABF188 = 0;
	DAT_00ABF18C = 0;
	DAT_00ABF190 = 0;
}

// FUNCTION: 0x005BC160
void FUN_005BC160(void)
{
	DAT_00ABF208 = 0;
	DAT_00ABF200 = 128;
	DAT_00ABF204 = 128;
}

// FUNCTION: 0x005BC180
void __fastcall FUN_005BC180(HINSTANCE instance, HWND window, bool enabled)
{
	DAT_00ABF230 = instance;
	DAT_00ABF234 = window;
	DAT_00ABF238 = enabled;
	for (int i = 0; i < 8; i++) {
		DAT_00ABF168[i] = new DebugPage(i);
	}
	DAT_00ABF23C = DAT_00ABF168[0];
	for (int j = 0; j < 8; j++) {
		DAT_00ABF168[j]->Reset();
	}
	if (DAT_00ABF239 && DAT_00ABF238) {
		if (DAT_00ABF240 == NULL) {
			FUN_005BC530();
		}
		DAT_00ABF23A = true;
		ShowWindow(DAT_00ABF240, SW_SHOWNORMAL);
		SetFocus(DAT_00ABF234);
	}
}

// FUNCTION: 0x005BC300
void FUN_005BC300(void)
{
	for (int i = 0; i < 8; i++) {
		if (DAT_00ABF168[i] != NULL) {
			delete DAT_00ABF168[i];
			DAT_00ABF168[i] = NULL;
		}
	}
	if (DAT_00ABF240 != NULL) {
		DeleteObject(DAT_00ABF244);
		FUN_0053E420(DAT_00ABF240);
		DestroyWindow(DAT_00ABF240);
		DAT_00ABF244 = NULL;
		DAT_00ABF240 = NULL;
	}
}

// FUNCTION: 0x005BC370
void FUN_005BC370(void)
{
	if (DAT_00ABF234 == NULL) {
		DAT_00ABF239 = true;
		return;
	}
	if (DAT_00ABF238) {
		if (DAT_00ABF240 == NULL) {
			FUN_005BC530();
		}
		DAT_00ABF23A = true;
		ShowWindow(DAT_00ABF240, SW_SHOWNORMAL);
		SetFocus(DAT_00ABF234);
	}
}

// FUNCTION: 0x005BC3C0
void FUN_005BC3C0(void)
{
	if (DAT_00ABF238) {
		if (DAT_00ABF240 == NULL) {
			FUN_005BC530();
		}
		DAT_00ABF23A = true;
		ShowWindow(DAT_00ABF240, SW_SHOWNORMAL);
		SetFocus(DAT_00ABF234);
	}
}

// FUNCTION: 0x005BC400
void FUN_005BC400(void)
{
	DAT_00ABF239 = false;
	if (DAT_00ABF240 != NULL) {
		DAT_00ABF23A = false;
		ShowWindow(DAT_00ABF240, SW_HIDE);
	}
}

// FUNCTION: 0x005BC420
int FUN_005BC420(void)
{
	if (DAT_00ABF23A || DAT_00ABF239) {
		return 1;
	}
	return 0;
}

// FUNCTION: 0x005BC440
DebugPage * FUN_005BC440(void)
{
	return DAT_00ABF23C;
}

// FUNCTION: 0x005BC450
DebugPage * __fastcall FUN_005BC450(int index)
{
	return DAT_00ABF168[index];
}

// FUNCTION: 0x005BC460
void __fastcall FUN_005BC460(int index)
{
	DebugPage * page = DAT_00ABF168[index];
	DAT_00ABF23C = page;
	page->Reset();
	DAT_00ABF23C->Refresh(0, 0, 80, 25);
}

// FUNCTION: 0x005BC4D0
void FUN_005BC4D0(void)
{
	for (int i = 0; i < 8; i++) {
		DAT_00ABF168[i]->Reset();
	}
}

// FUNCTION: 0x005BC530
void FUN_005BC530(void)
{
	DAT_00ABF240 = CreateDialogParamA(DAT_00ABF230, MAKEINTRESOURCE(248), DAT_00ABF234, (DLGPROC)FUN_005BC6E0, 0);
	FUN_0053E3C0(DAT_00ABF240);
	HDC dc = GetDC(DAT_00ABF240);

	LOGFONT lf;
	lf.lfHeight = -MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;
	strcpy(lf.lfFaceName, "Courier");
	DAT_00ABF244 = CreateFontIndirect(&lf);
	HGDIOBJ old = SelectObject(dc, DAT_00ABF244);
	GetTextMetrics(dc, &DAT_00ABF1C8);
	SelectObject(dc, old);
	DAT_00ABF128 = GetSysColor(COLOR_BTNFACE);
	DAT_00ABF110 = GetSysColor(COLOR_WINDOWTEXT);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = DAT_00ABF1C8.tmMaxCharWidth * 79 + 20;
	rect.bottom = (DAT_00ABF1C8.tmHeight + DAT_00ABF1C8.tmExternalLeading) * 25 + 20;
	AdjustWindowRect(&rect, WS_CAPTION, FALSE);
	RECT parent;
	GetWindowRect(DAT_00ABF234, &parent);
	MoveWindow(DAT_00ABF240, parent.right + 5, 0, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	ShowWindow(DAT_00ABF240, SW_HIDE);
	SetFocus(DAT_00ABF234);
	ReleaseDC(DAT_00ABF240, dc);
}

// FUNCTION: 0x005BC6E0
BOOL CALLBACK FUN_005BC6E0(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_PAINT) {
		InvalidateRect(hwnd, NULL, TRUE);
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		DebugPage * page = DAT_00ABF23C;
		if (page != NULL && DAT_00ABF23A) {
			if (DAT_00ABF23C != page) {
				DAT_00ABF23C = page;
				FUN_005BCC90(page);
			}
			page->Refresh(0, 0, 80, 25);
		}
		EndPaint(hwnd, &ps);
		return TRUE;
	}
	if (msg == WM_CHAR) {
		if (DAT_00ABF224 == NULL) {
			AudioController * controller = &DAT_00ABF198;
			DAT_00ABF224 = controller;
			controller->FUN_00405BE0();
		}
		switch (wparam) {
		case '1':
		case '2':
		case '3':
		case '4':
			DAT_00ABF22C = wparam - '1';
			return FALSE;
		case 'Q':
		case 'q':
			if (DAT_00ABF220 > 16 || DAT_00ABF220 >= DAT_00B1D4B0) {
				DAT_00ABF220 = 0;
			}
			FUN_00752480(DAT_00ABF220, 1, DAT_00ABF22C);
			DAT_00ABF220++;
			return FALSE;
		case 'W':
		case 'w':
			if (DAT_00ABF220 > 16 || DAT_00ABF220 >= DAT_00B1D4B0) {
				DAT_00ABF220 = 0;
			}
			FUN_00752480(DAT_00ABF220, 0, DAT_00ABF22C);
			DAT_00ABF220++;
			return FALSE;
		case 'E':
		case 'e':
			if (DAT_00ABF220 > 16 || DAT_00ABF220 >= DAT_00B1D4B0) {
				DAT_00ABF220 = 0;
			}
			FUN_00752480(DAT_00ABF220, 2, DAT_00ABF22C);
			DAT_00ABF220++;
			return FALSE;
		case 'O':
		case 'o':
			if (DAT_00ABF220 > 16 || DAT_00ABF220 >= DAT_00B1D4B0) {
				DAT_00ABF220 = 0;
			}
			FUN_00752480(DAT_00ABF220, -1, -1);
			DAT_00ABF220++;
			return FALSE;
		case 'L':
		case 'l':
			if (DAT_00ABF224->GetEvent() != NULL) {
				DAT_00ABF224->End();
				return FALSE;
			}
			VocClass::PlayGlobal(VocClass::FindIndex("TestEnvelopeFShift"), 1.0f, 0x2000, DAT_00ABF224);
			return FALSE;
		case 'T':
		case 't':
			VocClass::PlayGlobal(VocClass::FindIndex("TestEffect"), 1.0f, 0x2000, NULL);
			return FALSE;
		case '[':
			if (DAT_00ABF228 < 5) {
				AudioStream * stream = FUN_00407010(5000, 0)->FUN_00407860();
				DAT_00ABF114[DAT_00ABF228] = stream;
				if (stream != NULL) {
					if (FUN_00407B60(stream, "intro.wav", 1) != 0) {
						DAT_00ABF228++;
						return FALSE;
					}
					DAT_00ABF114[DAT_00ABF228]->FUN_00407A90();
				}
			}
			return FALSE;
		case ']':
			if (DAT_00ABF228 > 0) {
				AudioStream * last = DAT_00ABF114[DAT_00ABF228 - 1];
				DAT_00ABF228--;
				last->FUN_00407F40();
				DAT_00ABF114[DAT_00ABF228]->FUN_00407A90();
			}
			return FALSE;
		case 'N':
		case 'n':
			DAT_00ABF21C = (DAT_00ABF21C == 0);
			return FALSE;
		case 'S':
		case 's':
			FUN_00406EE0();
			return FALSE;
		case 'A':
		case 'a':
			DAT_00ABF23C = DAT_00ABF17C;
			FUN_005BCC90(DAT_00ABF23C);
			DAT_00ABF23C->Refresh(0, 0, 80, 25);
			return FALSE;
		case 'I':
		case 'i':
			DAT_00ABF23C = DAT_00ABF184;
			FUN_005BCC90(DAT_00ABF23C);
			DAT_00ABF23C->Refresh(0, 0, 80, 25);
			return FALSE;
		case 'M':
		case 'm':
			DAT_00ABF23C = DAT_00ABF180;
			FUN_005BCC90(DAT_00ABF23C);
			DAT_00ABF23C->Refresh(0, 0, 80, 25);
			return FALSE;
		case ',':
		case '<': {
			int next = DAT_00ABF23C->Index + 1;
			if (next == 8) {
				next = 0;
			}
			DAT_00ABF23C = DAT_00ABF168[next];
			DAT_00ABF23C->Clear();
			DAT_00ABF23C->Refresh(0, 0, 80, 25);
			return FALSE;
		}
		case '.':
		case '>': {
			int prev = DAT_00ABF23C->Index - 1;
			if (prev < 0) {
				prev = 7;
			}
			DAT_00ABF23C = DAT_00ABF168[prev];
			DAT_00ABF23C->Clear();
			DAT_00ABF23C->Refresh(0, 0, 80, 25);
			return FALSE;
		}
		default:
			return FALSE;
		}
	}
	if (msg == WM_COMMAND) {
		DAT_00ABF239 = false;
		if (DAT_00ABF240 != NULL) {
			DAT_00ABF23A = false;
			ShowWindow(DAT_00ABF240, SW_HIDE);
		}
		return TRUE;
	}
	return FALSE;
}

// FUNCTION: 0x005BCB60
DebugPoint * DebugPoint::Cell_Origin(int col, int row)
{
	DebugPoint p;
	p.X = DAT_00ABF1C8.tmMaxCharWidth * col + 10;
	p.Y = (DAT_00ABF1C8.tmHeight + DAT_00ABF1C8.tmExternalLeading) * row + 10;
	*this = p;
	return this;
}

// FUNCTION: 0x005BCB90
DebugPage::DebugPage(int index) :
	Index(index), Field271C(0), Field2720(0), Field2724(0), Field2728(0), Columns(80), Rows(25)
{
	Reset();
}

// FUNCTION: 0x005BCC20
void FUN_005BCC20(void)
{
}

// FUNCTION: 0x005BCC30
int DebugPage::Get_Index(void)
{
	return Index;
}

// FUNCTION: 0x005BCC40
void DebugPage::Set_Cursor(int x, int y)
{
	Field271C = x;
	Field2720 = y;
}

// FUNCTION: 0x005BCC60
void DebugPage::Set_Window(int x, int y, int w, int h)
{
	Field2724 = x;
	Field2728 = y;
	Columns = w;
	Rows = h;
}

// FUNCTION: 0x005BCC90
void DebugPage::Reset(void)
{
	Blank();
	Dirty = true;
	Refresh(0, 0, 80, 25);
}

// FUNCTION: 0x005BCCE0
bool DebugPage::Is_Dirty(void)
{
	return Dirty;
}

// FUNCTION: 0x005BCCF0
void DebugPage::Show(void)
{
	if (DAT_00ABF23A) {
		if (DAT_00ABF23C != this) {
			DAT_00ABF23C = this;
			Reset();
		}
		Refresh(0, 0, 80, 25);
	}
}

// FUNCTION: 0x005BCD70
void DebugPage::Set_Pad(int value)
{
	if (DAT_00ABF23A) {
		Pad[0] = value;
	}
}

// FUNCTION: 0x005BCD90
void DebugPage::Fill(int x, int y, int w, int h, int attr)
{
	if (DAT_00ABF23A) {
		int cx = Field2724 + x;
		int cy = Field2728 + y;
		int ex = cx + w;
		int ey = cy + h;
		for (int row = cy; row < ey; row++) {
			for (int col = cx; col < ex; col++) {
				Attr[col][row] = attr;
			}
		}
		Refresh(cx, cy, w, h);
	}
}

// FUNCTION: 0x005BCE10
void DebugPage::Scroll_Vertical(int amount)
{
	if (DAT_00ABF23A) {
		int x = Field2724;
		int y = Field2728;
		int w = Columns;
		int h = Rows;
		if (amount > 0) {
			for (int col = x; col < x + w; col++) {
				for (int row = y; row < y + h - 1; row++) {
					Text[col][row] = Text[col][row + 1];
					Attr[col][row] = Attr[col][row + 1];
				}
				Text[col][y + h - 1] = ' ';
				Attr[col][y + h - 1] = 0;
			}
		} else if (amount < 0) {
			for (int col = x; col < x + w; col++) {
				for (int row = y + h - 1; row > y; row--) {
					Text[col][row] = Text[col][row - 1];
					Attr[col][row] = Attr[col][row - 1];
				}
				Text[col][y] = ' ';
				Attr[col][y] = 0;
			}
		}
		Refresh(0, 0, 80, 25);
	}
}

// FUNCTION: 0x005BCF90
void DebugPage::Scroll_Horizontal(int amount)
{
	if (DAT_00ABF23A) {
		int x = Field2724;
		int w = Columns;
		int y = Field2728;
		int h = Rows;
		if (amount > 0) {
			for (int row = y; row < y + h; row++) {
				for (int col = x; col < x + w - 1; col++) {
					Text[col][row] = Text[col + 1][row];
					Attr[col][row] = Attr[col + 1][row];
				}
				Text[x + w - 1][row] = ' ';
				Attr[x + w - 1][row] = 0;
			}
		} else if (amount < 0) {
			for (int row = y; row < y + h; row++) {
				for (int col = x + w - 1; col > x; col--) {
					Text[col][row] = Text[col - 1][row];
					Attr[col][row] = Attr[col - 1][row];
				}
				Text[x][row] = ' ';
				Attr[x][row] = 0;
			}
		}
		Refresh(0, 0, 80, 25);
	}
}

// FUNCTION: 0x005BD0D0
void DebugPage::Print_Raw(char const * text)
{
	if (DAT_00ABF23A) {
		int cx = Field271C;
		int start = Field2724 + cx;
		int row = Field2728 + Field2720;
		int limit = Columns + Field2724;
		Dirty = false;
		int end = (int)strlen(text) + start;
		if (end > limit) {
			end = limit;
		}
		for (int col = start, i = 0; col < end; col++, i++) {
			char c = text[i];
			Text[col][row] = Translate(c);
			Attr[col][row] = Pad[0];
			if (c == '\n') {
				Field271C = 0;
				Field2720++;
			} else {
				Field271C++;
			}
		}
		Refresh(start, row, end - start, 1);
	}
}

// FUNCTION: 0x005BD1F0
void __cdecl FUN_005BD1F0(DebugPage * page, char const * format, ...)
{
	char buffer[256];
	if (DAT_00ABF23A) {
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		buffer[255] = '\0';
		page->Print_Raw(buffer);
	}
}

// FUNCTION: 0x005BD240
void __cdecl FUN_005BD240(DebugPage * page, wchar_t const * format, ...)
{
	char buffer[256];
	wchar_t wide[256];
	if (DAT_00ABF23A) {
		va_list args;
		va_start(args, format);
		vswprintf(wide, format, args);
		sprintf(buffer, "%S", wide);
		page->Print_Raw(buffer);
	}
}

// FUNCTION: 0x005BD2A0
void DebugPage::Print_At(char const * text, int x, int y, int attr)
{
	if (DAT_00ABF23A) {
		Field271C = x;
		Field2720 = y;
		Set_Pad(attr);
		Print_Raw(text);
	}
}

// FUNCTION: 0x005BD2E0
void DebugPage::Print_At(wchar_t const * text, int x, int y, int attr)
{
	char buffer[1024];
	if (DAT_00ABF23A) {
		Field271C = x;
		Field2720 = y;
		Set_Pad(attr);
		sprintf(buffer, "%S", text);
		Print_Raw(buffer);
	}
}

// FUNCTION: 0x005BD360
char DebugPage::Translate(char c)
{
	if (c == '\n') {
		return ' ';
	}
	if ((unsigned char)c == 0xB3) {
		return '|';
	}
	if ((unsigned char)c == 0xC4) {
		return '-';
	}
	if ((unsigned char)c == 0xCD) {
		return '=';
	}
	if ((unsigned char)c == 0xDA || (unsigned char)c == 0xC2 || (unsigned char)c == 0xBF ||
		(unsigned char)c == 0xC6 || (unsigned char)c == 0xC3 || (unsigned char)c == 0xD1 ||
		(unsigned char)c == 0xC5 || (unsigned char)c == 0xD8 || (unsigned char)c == 0xB5 ||
		(unsigned char)c == 0xC0 || (unsigned char)c == 0xC1 || (unsigned char)c == 0xD9 ||
		(unsigned char)c == 0xD5 || (unsigned char)c == 0xB4) {
		return '+';
	}
	return c;
}

// FUNCTION: 0x005BD3D0
void DebugPage::Draw_Cell(HDC dc, int col, int row)
{
	int attr = Attr[col][row];
	if (attr != Pad[1]) {
		if (attr == 1) {
			SetBkColor(dc, DAT_00ABF110);
			SetTextColor(dc, DAT_00ABF128);
		} else {
			SetBkColor(dc, DAT_00ABF128);
			SetTextColor(dc, DAT_00ABF110);
		}
		Pad[1] = attr;
	}
	char ch = Text[col][row];
	DebugPoint origin;
	origin.Cell_Origin(col, row);
	TextOutA(dc, origin.X, origin.Y, &ch, 1);
}

// FUNCTION: 0x005BD490
void DebugPage::Refresh(int x, int y, int w, int h)
{
	if (DAT_00ABF23A && this == DAT_00ABF23C) {
		HDC dc = GetDC(DAT_00ABF240);
		SetBkMode(dc, OPAQUE);
		SetBkColor(dc, DAT_00ABF128);
		SetTextColor(dc, DAT_00ABF110);
		if (dc) {
			HGDIOBJ old = SelectObject(dc, DAT_00ABF244);
			for (int row = y; row < y + h; row++) {
				for (int col = x; col < x + w; col++) {
					Draw_Cell(dc, col, row);
				}
			}
			SelectObject(dc, old);
			ReleaseDC(DAT_00ABF240, dc);
		}
	}
}

// FUNCTION: 0x005BD560
MotionLibrary::MotionLibrary() :
	LoadFailed(false),
	LayerCount(0),
	FrameCount(0),
	LayerMatrices(NULL)
{
}

// FUNCTION: 0x005BD570
MotionLibrary::MotionLibrary(FileClass & file) :
	LoadFailed(false),
	LayerCount(0),
	FrameCount(0),
	LayerMatrices(NULL)
{
	if (!Read_File(file)) {
		LoadFailed = true;
	}
}

// FUNCTION: 0x005BD5A0
MotionLibrary::~MotionLibrary()
{
	Clear();
}

// FUNCTION: 0x005BD5C0
BOOL MotionLibrary::Read_File(FileClass & file)
{
	Clear();

	bool result = file.Open(FileClass::READ) != 0;
	if (!result) {
		return(false);
	}

	VoxelAnimFileHeaderStruct header;

	file.Read(&header, sizeof(header));
	LayerCount = header.LayerCount;
	FrameCount = header.FrameCount;
	if (LayerMatrices != NULL) {
		delete [] LayerMatrices;
	}

	int count = LayerCount * FrameCount;
	LayerMatrices = new Matrix3D[count];

	if (LayerMatrices == NULL) {
		LayerMatrices = NULL;
		file.Close();
		return(false);
	}

	file.Seek(MAX_VOXEL_NAME_LENGTH * LayerCount);

	for (unsigned frame = 0; frame < FrameCount; frame++) {
		for (unsigned layer = 0; layer < LayerCount; layer++) {
			float m[12];
			if (file.Read(&m, sizeof(m)) == sizeof(m)) {
				LayerMatrices[layer + frame * LayerCount] = Matrix3D(m);
			} else {
				file.Close();
				Clear();
				return(false);
			}
		}
	}

	file.Close();
	return(true);
}

// FUNCTION: 0x005BD730
void MotionLibrary::Scale(float scale)
{
	for (unsigned frame = 0; frame < FrameCount; frame++) {
		for (unsigned layer = 0; layer < LayerCount; layer++) {
			LayerMatrices[layer + frame * LayerCount][0].W = scale * LayerMatrices[layer + frame * LayerCount][0].W;
			LayerMatrices[layer + frame * LayerCount][1].W = scale * LayerMatrices[layer + frame * LayerCount][1].W;
			LayerMatrices[layer + frame * LayerCount][2].W = scale * LayerMatrices[layer + frame * LayerCount][2].W;
		}
	}
}

// FUNCTION: 0x005BD7C0
void MotionLibrary::Clear()
{
	if (LayerMatrices != NULL) {
		delete [] LayerMatrices;
	}
	LayerMatrices = NULL;
}

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
extern const double DAT_007EDF98;
extern const double DAT_007EDFA0;

// FUNCTION: 0x005BD7E0 $E42
double DAT_00ABF270 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x005BD810 $E45
double DAT_00ABF2D0 = DAT_00ABF270 / DAT_007EDF98;

// FUNCTION: 0x005BD830 $E48
double DAT_00ABF2B8 = DAT_007E1718 / DAT_00ABF2D0;

// FUNCTION: 0x005BD850 $E51
double DAT_00ABF2B0 = DAT_007E1720 * DAT_007EDFA0;

// FUNCTION: 0x005BD870 $E54
double DAT_00ABF278 = DAT_007E1728 * DAT_007EDFA0;

// FUNCTION: 0x005BD890 $E57
double DAT_00ABF248 = DAT_007E1730 * DAT_007EDFA0;

// FUNCTION: 0x005BD8B0 $E60
int DAT_00ABF2D8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABF248 - DAT_00ABF278) * DAT_00ABF270));

// FUNCTION: 0x005BD8F0 $E63
double DAT_00ABF280 = FUN_004CADE0(DAT_00ABF2D8 * DAT_007E1740);

// FUNCTION: 0x005BD920 $E66
double DAT_00ABF298 = FUN_004CADE0((DAT_00ABF2D8 * 2) / DAT_00ABF270);

extern int DAT_00ABF2CC;
extern short DAT_00ABF268;
extern short DAT_00ABF26A;
extern int DAT_00ABF288;
extern int DAT_00ABF28C;
extern int DAT_00ABF290;
extern int DAT_00ABF2C0;
extern int DAT_00ABF2C4;
extern int DAT_00ABF2C8;
extern int DAT_00ABF258;
extern int DAT_00ABF25C;
extern int DAT_00ABF260;
extern int DAT_00ABF264;

// FUNCTION: 0x005BD950
void FUN_005BD950(void)
{
	DAT_00ABF2CC = (int)(DAT_00ABF2D8 * 4 + DAT_007E1738);
}

// FUNCTION: 0x005BD980
void FUN_005BD980(void)
{
	DAT_00ABF268 = 0;
	DAT_00ABF26A = 0;
}

// FUNCTION: 0x005BD990
void FUN_005BD990(void)
{
	DAT_00ABF288 = 0;
	DAT_00ABF28C = 0;
	DAT_00ABF290 = 0;
}

// FUNCTION: 0x005BD9B0
void FUN_005BD9B0(void)
{
	DAT_00ABF2C8 = 0;
	DAT_00ABF2C0 = 128;
	DAT_00ABF2C4 = 128;
}

// FUNCTION: 0x005BD9D0
void FUN_005BD9D0(void)
{
	DAT_00ABF258 = 0;
	DAT_00ABF25C = 0;
	DAT_00ABF260 = 0;
	DAT_00ABF264 = 0;
}
