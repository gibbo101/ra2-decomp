// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <windows.h>
#include "bitfont.h"
#include "ccfile.h"
#include "surface.h"
#include "rgb.h"
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

extern int DAT_0089C4A8;
extern int DAT_0089C4AC;
extern int DAT_0089C4B0;
extern int DAT_0089C4B4;

// FUNCTION: GAMEMD 0x00433860
void FUN_00433860(void)
{
	DAT_0089C4A8 = 0;
	DAT_0089C4AC = 0;
	DAT_0089C4B0 = 0;
	DAT_0089C4B4 = 0;
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

// FUNCTION: GAMEMD 0x00433EF0
int BitFont::FUN_00433EF0(wchar_t const * text, int maxwidth)
{
	if (Bool_40) {
		State_2C = 2;
		GetTextDimension(text, &maxwidth, 0, maxwidth);
		State_2C = 1;
	} else {
		GetTextDimension(text, &maxwidth, 0, maxwidth);
	}
	return maxwidth;
}

// FUNCTION: GAMEMD 0x004340A0
bool BitFont::FUN_004340A0(wchar_t const * text, int * width, int * height, int maxwidth)
{
	if (Bool_40) {
		State_2C = 2;
		bool r = GetTextDimension(text, width, height, maxwidth);
		State_2C = 1;
		return r;
	}
	return GetTextDimension(text, width, height, maxwidth);
}

// FUNCTION: GAMEMD 0x00434100
int BitFont::FUN_00434100(int x)
{
	int old = Unknown_28;
	Unknown_28 = x;
	return old;
}

// FUNCTION: GAMEMD 0x00434110
void BitFont::FUN_00434110(int x)
{
	field_20 = x;
}

struct Drawing {
	static int RedShiftLeft;
	static int RedShiftRight;
	static int BlueShiftLeft;
	static int BlueShiftRight;
	static int GreenShiftLeft;
	static int GreenShiftRight;
};

extern RGBClass const DAT_00A80220;

// FUNCTION: GAMEMD 0x00434500
int BitFont::FUN_00434500(wchar_t const * text, int x, int y, int w, int h)
{
	int count = 0;
	unsigned short ch = *text++;
	unsigned short saved = Color;
	if (ch) {
		int t = h - 1;
		int ratio = (9 - h) * 31;
		while (ch) {
			if (h) {
				if (count >= h) {
					Color = saved;
					int width;
					GetTextDimension(text, &width, 0, 0);
					return x + width;
				}
				if (t < 8) {
					unsigned char r = (saved >> Drawing::RedShiftLeft) << Drawing::RedShiftRight;
					unsigned char b = (saved >> Drawing::BlueShiftLeft) << Drawing::BlueShiftRight;
					unsigned char g = (saved >> Drawing::GreenShiftLeft) << Drawing::GreenShiftRight;
					RGBClass c = RGBClass(r, g, b);
					c.Adjust(ratio, DAT_00A80220);
					Color = (unsigned short)(((c.Get_Green() >> Drawing::GreenShiftRight) << Drawing::GreenShiftLeft)
						| ((c.Get_Blue() >> Drawing::BlueShiftRight) << Drawing::BlueShiftLeft)
						| ((c.Get_Red() >> Drawing::RedShiftRight) << Drawing::RedShiftLeft));
				}
				count++;
				ratio += 31;
				t--;
			}
			if (ch != 13 && ch != 10) {
				x = Blit(ch, x, y, -1);
			}
			if (w && --w == 0) break;
			ch = *text++;
		}
	}
	Color = saved;
	return x;
}

// FUNCTION: GAMEMD 0x004346C0
unsigned char * BitFont::GetCharacterBitmap(wchar_t ch)
{
	unsigned char * result = 0;
	InternalData * data = InternalPTR;
	if (data) {
		int index = data->SymbolTable[ch];
		if (index) {
			result = (unsigned char *)(data->SymbolDataSize * (index - 1) + data->Bitmaps);
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x00433F50
int BitFont::FUN_00433F50(wchar_t const * text, int maxwidth, int maxchars, int wrap)
{
	int width = 0;
	int count = 0;
	if (!InternalPTR || !text) return 0;
	{
		unsigned short ch = *text;
		while (ch) {
			switch (ch) {
				case 9:
					width += Unknown_28;
					width -= width % Unknown_28;
					break;
				case 10:
				case 13:
					break;
				default: {
					unsigned char * bitmap = GetCharacterBitmap(ch);
					if (!bitmap) bitmap = (unsigned char *)Pointer_8;
					if (bitmap) width += *bitmap + State_2C;
					break;
				}
			}
			++count;
			if (width > maxwidth) {
				--count;
				if (wrap && ch > 32) {
					--text;
					int split = count;
					while (split && *text > 32) {
						--split;
						--text;
					}
					if (split) count = split;
				}
				break;
			}
			if (maxchars && count == maxchars) break;
			if (wrap && (ch == 13 || ch == 10)) break;
			ch = *++text;
		}
	}
	return count;
}

// FUNCTION: GAMEMD 0x00434700
void BitFont::FUN_00434700(void)
{
	Pointer_8 = malloc(InternalPTR->FontHeight * InternalPTR->Stride + 2);
	if (InternalPTR) {
		unsigned char * src = GetCharacterBitmap(0x58);
		if (src) {
			*(unsigned char *)Pointer_8 = *src++;
			unsigned char * dst = (unsigned char *)Pointer_8 + 1;
			for (unsigned y = 0; y < (unsigned)InternalPTR->FontHeight; y++) {
				for (unsigned x = 0; x < (unsigned)InternalPTR->Stride; x++) {
					*dst++ = ~*src++;
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00434790
void * BitFont::FUN_00434790(unsigned short * color)
{
	if (color) *color ^= 0x5555;
	return Pointer_8;
}

// FUNCTION: GAMEMD 0x004347B0
void BitFont::FUN_004347B0(void)
{
	if (InternalPTR) {
		field_1C++;
		unsigned char * p = 0;
		int index = InternalPTR->SymbolTable[0x20];
		if (index) p = (unsigned char *)(InternalPTR->SymbolDataSize * (index - 1) + InternalPTR->Bitmaps);
		(*p)++;
	}
}

// FUNCTION: GAMEMD 0x004347E0
void BitFont::FUN_004347E0(void)
{
	if (InternalPTR) {
		field_1C++;
		unsigned char * space = 0;
		int index = InternalPTR->SymbolTable[0x20];
		if (index) space = (unsigned char *)(InternalPTR->SymbolDataSize * (index - 1) + InternalPTR->Bitmaps);
		unsigned char * dest = 0;
		index = InternalPTR->SymbolTable[0x3000];
		if (index) dest = (unsigned char *)(InternalPTR->SymbolDataSize * (index - 1) + InternalPTR->Bitmaps);
		if (dest) {
			if (space) *dest = *space << 1;
			else *dest = (unsigned char)InternalPTR->FontWidth;
		}
	}
}

// FUNCTION: GAMEMD 0x00434840
bool BitFont::FUN_00434840(LTRBStruct * out, LTRBStruct * rect, LTRBStruct * bounds)
{
	if (rect->Top > bounds->Bottom || bounds->Top > rect->Bottom || rect->Left > bounds->Right || bounds->Left > rect->Right) {
		out->Left = 0;
		out->Top = 0;
		out->Right = -1;
		out->Bottom = -1;
		return false;
	}
	*out = *rect;
	if (out->Top < bounds->Top) out->Top = bounds->Top;
	if (out->Left < bounds->Left) out->Left = bounds->Left;
	if (out->Bottom > bounds->Bottom) out->Bottom = bounds->Bottom;
	if (out->Right > bounds->Right) out->Right = bounds->Right;
	return true;
}

// FUNCTION: GAMEMD 0x004348F0
void BitFont::Lock(Surface * surface)
{
	pGraphBuffer = (short *)surface->Lock(Point2D(0, 0));
	PitchDiv2 = (unsigned)surface->Stride() >> 1;
	LTRBStruct area;
	area.Left = 0;
	area.Top = 0;
	area.Right = surface->Get_Width() - 1;
	area.Bottom = surface->Get_Height() - 1;
	if (!(Bounds.Right | Bounds.Bottom | Bounds.Top | Bounds.Left)) {
		Bounds.Left = area.Left;
		Bounds.Top = area.Top;
		Bounds.Right = area.Right;
		Bounds.Bottom = area.Bottom;
	} else {
		FUN_00434840(&Bounds, &Bounds, &area);
	}
}

// FUNCTION: GAMEMD 0x00434990
void BitFont::UnLock(Surface * surface)
{
	surface->Unlock();
	pGraphBuffer = 0;
	PitchDiv2 = 0;
}

// FUNCTION: GAMEMD 0x004349B0
int BitFont::FUN_004349B0(wchar_t ch)
{
	InternalData * data = InternalPTR;
	if (data) {
		int index = data->SymbolTable[ch];
		if (index) {
			unsigned char * bitmap = (unsigned char *)(data->SymbolDataSize * (index - 1) + data->Bitmaps);
			if (bitmap) return *bitmap;
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x004349F0
void BitFont::FUN_004349F0(void)
{
	InternalPTR = 0;
	field_20 = 0;
	pGraphBuffer = 0;
	Pointer_8 = 0;
	Color = 0x7FFF;
	DefaultColor2 = 0x3555;
	Unknown_28 = 0x40;
	Bool_40 = true;
	field_41 = true;
	field_18 = 1;
	field_1C = 1;
	SetRect((RECT *)&Bounds, 0, 0, 0, 0);
	State_2C = 1;
}

// SYNTHETIC: GAMEMD 0x00434A40
// ??_GBitFont@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x00433CF0
bool BitFont::GetTextDimension(wchar_t const * text, int * width, int * height, int maxwidth)
{
	int break_width = 0;
	if (!InternalPTR || !text) {
		if (height) *height = 0;
		if (width) *width = 0;
		return false;
	}
	int max_line_width = 0;
	int line_width = 0;
	int line_height = field_1C;
	int total_height = line_height;
	int count = 0;
	wchar_t const * break_text = 0;
	unsigned short previous = 0;
	unsigned short ch = *text++;
	while (ch) {
		switch (ch) {
		case 10:
		case 13:
			if (previous != 13) {
				line_width = 0;
				count = 0;
				total_height += line_height;
			}
			break;
		case 9:
			line_width += Unknown_28;
			line_width -= line_width % Unknown_28;
			break;
		case 32:
			break_text = text;
			break_width = max_line_width;
		default: {
			unsigned char * bitmap = GetCharacterBitmap(ch);
			if (!bitmap) bitmap = (unsigned char *)Pointer_8;
			if (bitmap) {
				int char_width = *bitmap;
				++count;
				line_width += char_width + State_2C;
				if (maxwidth && line_width > maxwidth) {
					if (break_text) {
						if (count > 1) {
							text = break_text;
							max_line_width = break_width;
						}
					} else {
						if (count > 1) {
							line_width -= char_width + State_2C;
							--text;
						}
						if (line_width > max_line_width) max_line_width = line_width;
					}
					line_width = 0;
					count = 0;
					break_text = 0;
					total_height += line_height;
				} else if (line_width > max_line_width) max_line_width = line_width;
			}
			break;
		}
		}
		previous = ch;
		ch = *text++;
	}
	if (width) *width = max_line_width;
	if (height) *height = total_height;
	return true;
}
