// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "tab.h"
#include "shapebtn.h"
#include <string.h>

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

// FUNCTION: 0x006D0F00
void TabClass::Redraw_Tab(void)
{
	IsToRedraw = true;
	Flag_To_Redraw();
}
