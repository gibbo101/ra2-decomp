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
			unsigned short * SymbolTable;
			char * Bitmaps;
			int ValidSymbolCount;

			void Destroy(void);
		};

		BitFont(char const * filename);
		virtual ~BitFont(void);

		bool Load(char const * filename);
		void Make_Missing_Char(void);
		bool GetTextDimension(wchar_t const * text, int * width, int * height, int maxwidth);
		int FUN_00433ED0(wchar_t const * text, int maxwidth);
		int Bold_String_Pixel_Width(wchar_t const * text, int maxwidth);
		int FUN_00433F50(wchar_t const * text, int maxwidth, int unknown1, int unknown2);
		int FUN_004349B0(wchar_t ch);
		unsigned short SetColor(unsigned short color);
		unsigned short SetShadowColor(unsigned short color);
		bool SetClipMode(bool clipped);
		bool SetFlag40(bool flag);
		void Lock(Surface * surface);
		void UnLock(Surface * surface);
		bool Bold_Text_Dimensions(wchar_t const * text, int * width, int * height, int maxwidth);
		int Set_Tab_Width(int x);
		void Set_Tab_Origin(int x);
		unsigned char * GetCharacterBitmap(wchar_t ch);
		void * Missing_Char(unsigned short * color);
		void Widen_Space(void);
		void Set_Wide_Space(void);
		void FUN_004349F0(void);
		int FUN_00434500(wchar_t const * text, int x, int y, int w, int h);
		bool Clip_Rect(LTRBStruct * out, LTRBStruct * rect, LTRBStruct * bounds);
		int Blit(unsigned short ch, int x, int y, int unknown);
		void SetBounds(LTRBStruct * bounds);

		static InternalData * LoadInternalData(char const * filename);

		static BitFont * Instance;

		InternalData * InternalPTR;
		void * Pointer_8;
		short * pGraphBuffer;
		int PitchDiv2;
		int Unknown_14;
		int field_18;
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
		bool DrawText(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10);
		int PrintChar(BitFont * font, Surface * surface, unsigned short ch, int x, int y);
		int PrintBold(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w);
		bool DrawShadow(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10);
		bool DrawBold(BitFont * font, Surface * surface, wchar_t const * text, int x, int y, int w, int h, int a8, int a9, int a10);

		static BitText * Instance;
};
