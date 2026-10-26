// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <windows.h>
#include "bitfont.h"
#include "ccfile.h"
#include <stdlib.h>
#include <string.h>

static inline void Reset_Defaults(BitFont * font)
{
	font->InternalPTR = 0;
	font->Color = 0x7FFF;
	font->DefaultColor2 = 0x3555;
	font->Unknown_28 = 0x40;
	font->Bool_40 = true;
	font->field_41 = true;
	font->field_18 = 1;
	font->field_1C = 1;
	font->field_20 = 0;
	font->pGraphBuffer = 0;
	font->Pointer_8 = 0;
	SetRect((RECT *)&font->Bounds, 0, 0, 0, 0);
	font->State_2C = 1;
}

// FUNCTION: GAMEMD 0x00433880
BitFont::BitFont(char const * filename)
{
	Reset_Defaults(this);
	InternalPTR = LoadInternalData(filename);
	if (InternalPTR) {
		field_18 = InternalPTR->Stride;
		field_1C = InternalPTR->Lines;
		FUN_00434700();
	} else {
		Reset_Defaults(this);
	}
}

// FUNCTION: GAMEMD 0x00433940
BitFont::~BitFont(void)
{
	if (Pointer_8) free(Pointer_8);
	if (InternalPTR) InternalPTR->Destroy();
}

// FUNCTION: GAMEMD 0x00433C00
void BitFont::InternalData::Destroy(void)
{
	if (SymbolTable) delete SymbolTable;
	if (Bitmaps) delete Bitmaps;
	delete this;
}

// FUNCTION: GAMEMD 0x00433C30
bool BitFont::Load(char const * filename)
{
	InternalPTR = LoadInternalData(filename);
	if (InternalPTR) {
		field_18 = InternalPTR->Stride;
		field_1C = InternalPTR->Lines;
		FUN_00434700();
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00433C70
unsigned short BitFont::SetColor(unsigned short color)
{
	unsigned short old = Color;
	Color = color;
	return old;
}

// FUNCTION: GAMEMD 0x00433C80
unsigned short BitFont::SetShadowColor(unsigned short color)
{
	DefaultColor2 = color;
	return Color;
}

// FUNCTION: GAMEMD 0x00433C90
bool BitFont::SetClipMode(bool clipped)
{
	bool old = field_41;
	field_41 = clipped;
	return old;
}

// FUNCTION: GAMEMD 0x00433CA0
void BitFont::SetBounds(LTRBStruct * bounds)
{
	if (bounds) {
		Bounds.Left = bounds->Left;
		Bounds.Top = bounds->Top;
		Bounds.Right = bounds->Right;
		Bounds.Bottom = bounds->Bottom;
	} else {
		Bounds.Bottom = 0;
		Bounds.Right = 0;
		Bounds.Top = 0;
		Bounds.Left = 0;
	}
}

// FUNCTION: GAMEMD 0x00433CE0
bool BitFont::SetFlag40(bool flag)
{
	bool old = Bool_40;
	Bool_40 = flag;
	return old;
}

// FUNCTION: GAMEMD 0x00433ED0
int BitFont::FUN_00433ED0(wchar_t const * text, int maxwidth)
{
	GetTextDimension(text, &maxwidth, 0, maxwidth);
	return maxwidth;
}
