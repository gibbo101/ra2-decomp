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

extern RGBClass DAT_00A80220;

inline RGBClass Int_To_RGB(int color)
{
	return RGBClass(
		(unsigned char)((unsigned short)color >> Drawing::RedShiftLeft) << Drawing::RedShiftRight,
		(unsigned char)((unsigned short)color >> Drawing::GreenShiftLeft) << Drawing::GreenShiftRight,
		(unsigned char)((unsigned short)color >> Drawing::BlueShiftLeft) << Drawing::BlueShiftRight);
}

inline int RGB_To_Int(RGBClass const & rgb)
{
	return Drawing::RGB_To_Int(rgb.Get_Red(), rgb.Get_Green(), rgb.Get_Blue());
}

inline int Align_Offset(int w, int width, int flags)
{
	int offset = 0;
	if (w > width) {
		if (flags & 1) {
			offset = (w - width) / 2;
		} else if (flags & 2) {
			offset = w - width;
		}
	}
	return offset;
}

inline unsigned char * Char_Bitmap(BitFont * font, wchar_t ch)
{
	unsigned char * bmp = font->GetCharacterBitmap(ch);
	if (!bmp) {
		bmp = font->GetCharacterBitmap('?');
	}
	return bmp;
}

inline bool Draw_Line(BitFont * font, wchar_t const * & start, wchar_t const * end, int x, int y, int w, int width, int flags, int & index, int & xpos, int color, int count, int fade)
{
	xpos = x + Align_Offset(w, width, flags);
	while (start < end) {
		index++;
		int c = color;
		if (count) {
			if (index >= count) {
				c = -1;
			} else {
				int remain = count - index - 1;
				if (remain < fade) {
					RGBClass rgb = Int_To_RGB(color);
					rgb.Adjust((255 / fade) * (fade - remain), DAT_00A80220);
					c = RGB_To_Int(rgb);
				}
			}
		}
		if (c == -1) {
			return false;
		}
		xpos = font->Blit(*start++, xpos, y, c);
	}
	return true;
}

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

// FUNCTION: GAMEMD 0x00434CD0
bool BitText::DrawText(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10)
{
	int width = 0;
	if (!font || !text) {
		return false;
	}

	font->Lock(surface);
	font->FUN_00434110(x);

	wchar_t const * start = text;
	int height = 0;
	int savedwidth = 0;
	wchar_t const * saved = 0;
	wchar_t prev = 0;
	int count = 0;
	int color = font->Color;
	int index = 0;
	int xpos;
	wchar_t ch = *text;

	while (ch) {
		switch (ch) {
			case '\r':
			case '\n':
				if (prev != '\r') {
					if (!Draw_Line(font, start, text, x, y, w, width, a8, index, xpos, color, a9, a10)) {
						font->UnLock(surface);
						return true;
					}
					width = 0;
					count = 0;
					height += font->field_1C;
					if (h && height >= h) {
						font->UnLock(surface);
						return true;
					}
					y += font->field_1C;
				}
				start++;
				break;

			case '\t':
				width += font->Unknown_28;
				width -= width % font->Unknown_28;
				break;

			case ' ':
				savedwidth = width;
				saved = text;

			default:
				{
					unsigned char * bmp = Char_Bitmap(font, ch);
					if (!bmp) {
						break;
					}
					count++;
					int cw = *bmp + font->State_2C;
					width += cw;
					if (w && width > w) {
						if (saved) {
							if (count > 1) {
								width = savedwidth;
								text = saved;
							}
						} else if (count > 1) {
							width -= cw;
							text--;
						}
						xpos = x + Align_Offset(w, width, a8);
						while (start < text) {
							index++;
							int c = color;
							if (a9) {
								if (index >= a9) {
									c = -1;
								} else {
									int remain = a9 - index - 1;
									if (remain < a10) {
										RGBClass rgb = Int_To_RGB(color);
										rgb.Adjust((255 / a10) * (a10 - remain), DAT_00A80220);
										c = RGB_To_Int(rgb);
									}
								}
							}
							if (c == -1) {
								font->UnLock(surface);
								return true;
							}
							xpos = font->Blit(*start++, xpos, y, c);
						}
						if (text == saved) {
							saved = 0;
							start++;
						}
						width = 0;
						count = 0;
						height += font->field_1C;
						if (h && height >= h) {
							font->UnLock(surface);
							return true;
						}
						y += font->field_1C;
					}
				}
				break;
		}
		prev = ch;
		text++;
		ch = *text;
	}

	xpos = x + Align_Offset(w, width, a8);
	while (start < text) {
		index++;
		int c = color;
		if (a9) {
			if (index >= a9) {
				c = -1;
			} else {
				int remain = a9 - index - 1;
				if (remain < a10) {
					RGBClass rgb = Int_To_RGB(color);
					rgb.Adjust((255 / a10) * (a10 - remain), DAT_00A80220);
					c = RGB_To_Int(rgb);
				}
			}
		}
		if (c == -1) {
			font->UnLock(surface);
			return true;
		}
		xpos = font->Blit(*start++, xpos, y, c);
	}
	font->UnLock(surface);
	return true;
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

// FUNCTION: GAMEMD 0x004355B0
// ??0RGBClass@@QAE@EEE@Z

// FUNCTION: GAMEMD 0x004355D0
int Drawing::RGB_To_Int(int red, int green, int blue)
{
	return ((red >> RedShiftRight) << RedShiftLeft)
		| ((green >> GreenShiftRight) << GreenShiftLeft)
		| ((blue >> BlueShiftRight) << BlueShiftLeft);
}
