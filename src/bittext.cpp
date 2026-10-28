// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "bitfont.h"
#include "rgb.h"

struct Drawing {
	static int RedShiftLeft;
	static int RedShiftRight;
	static int GreenShiftLeft;
	static int GreenShiftRight;
	static int BlueShiftLeft;
	static int BlueShiftRight;
	static int RGB_To_Int(int red, int green, int blue);
};

// FUNCTION: GAMEMD 0x00435530
void __stdcall Release_Font(BitFont * font)
{
	delete font;
	if (font == BitFont::Instance) {
		BitFont::Instance = 0;
	}
}

// FUNCTION: GAMEMD 0x004354D0
void Create_Font(void)
{
	BitFont::Instance = new BitFont("GAME.FNT");
}

// FUNCTION: GAMEMD 0x00435500
void Destroy_Font(void)
{
	Release_Font(BitFont::Instance);
}

// SYNTHETIC: GAMEMD 0x00434AB0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x00434AD0
BitText::BitText(void)
{
	Create_Font();
}

// FUNCTION: GAMEMD 0x00434B10
BitText::~BitText(void)
{
	Destroy_Font();
}

// FUNCTION: GAMEMD 0x00434B40
int BitText::PrintChar(BitFont * font, Surface * surface, unsigned short ch, int x, int y)
{
	int result = x;
	if (font) {
		font->Lock(surface);
		font->FUN_00434110(x);
		result = font->Blit(ch, x, y, -1);
		font->UnLock(surface);
	}
	return result;
}

// FUNCTION: GAMEMD 0x00434B90
int BitText::Print(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h)
{
	int result = x;
	if (font) {
		font->Lock(surface);
		font->FUN_00434110(x);
		result = font->FUN_00434500(text, x, y, w, h);
		font->UnLock(surface);
	}
	return result;
}

// FUNCTION: GAMEMD 0x00434BF0
int BitText::PrintBold(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w)
{
	int result = 0;
	if (font) {
		if (font->Bool_40) {
			font->State_2C = 2;
			font->Lock(surface);
			font->FUN_00434110(x);
			font->FUN_00434500(text, x, y, w, 0);
			font->UnLock(surface);
			int x1 = x + 1;
			font->Lock(surface);
			font->FUN_00434110(x1);
			result = font->FUN_00434500(text, x1, y, w, 0);
			font->UnLock(surface);
			font->State_2C = 1;
		} else {
			font->Lock(surface);
			font->FUN_00434110(x);
			result = font->FUN_00434500(text, x, y, w, 0);
			font->UnLock(surface);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x00435340
bool BitText::DrawShadow(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10)
{
	if (!font) {
		return false;
	}
	font->FUN_00434110(x);
	unsigned short c = font->DefaultColor2;
	unsigned short old = font->SetColor(c);
	DrawText(font, surface, text, x + 1, y + 1, w, h, a8, a9, a10);
	font->SetColor(old);
	return DrawText(font, surface, text, x, y, w, h, a8, a9, a10);
}

// FUNCTION: GAMEMD 0x004353F0
bool BitText::DrawBold(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10)
{
	if (!font) {
		return false;
	}
	font->FUN_00434110(x);
	bool result;
	if (font->Bool_40) {
		font->State_2C = 2;
		DrawText(font, surface, text, x, y, w, h, a8, a9, a10);
		x++;
		result = DrawText(font, surface, text, x, y, w, h, a8, a9, a10);
		font->State_2C = 1;
	} else {
		result = DrawText(font, surface, text, x, y, w, h, a8, a9, a10);
	}
	return result;
}

// SYNTHETIC: GAMEMD 0x00435560
// ??_GBitText@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x004355D0
int Drawing::RGB_To_Int(int red, int green, int blue)
{
	return ((red >> RedShiftRight) << RedShiftLeft)
		| ((green >> GreenShiftRight) << GreenShiftLeft)
		| ((blue >> BlueShiftRight) << BlueShiftLeft);
}
