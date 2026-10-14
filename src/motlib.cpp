// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "motlib.h"
#include "wwfile.h"
#include "voxel.hh"
#include <windows.h>

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

	DebugPage(int index) :
		Index(index), Field271C(0), Field2720(0), Field2724(0), Field2728(0), Columns(80), Rows(25)
	{
		Reset();
	}
	void Reset(void)
	{
		Blank();
		Dirty = true;
		Refresh(0, 0, 80, 25);
	}
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
};

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
extern void FUN_005BC530(void);
extern void __fastcall FUN_0053E420(HWND);

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
