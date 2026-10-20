// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
#pragma once

#include <stddef.h>

#undef DrawText

class Surface;

struct LTRBStruct
{
	int Left;
	int Top;
	int Right;
	int Bottom;
};

class BitFont
{
	public:
		struct InternalData
		{
			int FontWidth;
			int Stride;
			int FontHeight;
			int Lines;
			int Count;
			int SymbolDataSize;
			short * SymbolTable;
			char * Bitmaps;
			int ValidSymbolCount;
		};

		BitFont(char const * filename);
		virtual ~BitFont(void);

		bool GetTextDimension(wchar_t const * text, int * width, int * height, int maxwidth);
		int FUN_00433ED0(wchar_t const * text, int maxwidth);
		int FUN_004349B0(wchar_t ch);
		unsigned short SetColor(unsigned short color);
		bool SetClipMode(bool clipped);
		void SetBounds(LTRBStruct * bounds);

		static InternalData * LoadInternalData(char const * filename);

		static BitFont * Instance;

		InternalData * InternalPTR;
		void * Pointer_8;
		short * pGraphBuffer;
		int PitchDiv2;
		int Unknown_14;
		wchar_t * field_18;
		int field_1C;
		int field_20;
		unsigned short Color;
		short DefaultColor2;
		int Unknown_28;
		int State_2C;
		LTRBStruct Bounds;
		bool Bool_40;
		bool field_41;
		bool field_42;
		bool field_43;
};

class BitText
{
	public:
		BitText(void);
		virtual ~BitText(void);

		int Print(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h);
		void DrawText(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10);

		static BitText * Instance;
};
