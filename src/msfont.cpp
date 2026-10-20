// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "msfont.h"
#include "msanim.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "mixfile.h"
#include "palette.h"
#include "shapeset.h"

#include <cstring>

struct FileSystem {
	static void const * LoadFile(char const * name, bool flag);
};

struct Drawing {
	static int RedShiftRight;
	static int RedShiftLeft;
	static int GreenShiftRight;
	static int GreenShiftLeft;
	static int BlueShiftRight;
	static int BlueShiftLeft;
};

void __cdecl FUN_004068E0(char const * format, ...);
int FUN_00407000(void);

static int InstanceCount = 0;

static struct {
	void * Sample;
	bool AllocLoaded;
} Sounds[3];

// FUNCTION: 0x005D2E00
MSFont::MSFont(bool use_side_palette) :
	Red(0),
	Green(0),
	Blue(0)
{
	Init(use_side_palette);
	InstanceCount++;
}

// FUNCTION: 0x005D2E30
MSFont::~MSFont(void)
{
	InstanceCount--;
	if (InstanceCount == 0) {
		for (int i = 0; i < ARRAY_SIZE(Sounds); i++) {
			if (Sounds[i].Sample != NULL) {
				Sounds[i].Sample = NULL;
				Sounds[i].AllocLoaded = false;
			}
		}

		if (Drawer != NULL) {
			delete Drawer;
		}

		if (FontFile != NULL) {
			if (AllocLoaded == true) {
				delete (ShapeSet *)FontFile;
			}
		}

		FontFile = NULL;
		AllocLoaded = false;
	}
}

// FUNCTION: 0x005D2E90
bool MSFont::Init(bool use_side_palette)
{
	CCFileClass file;

	if (InstanceCount == 0) {
		AllocLoaded = false;
		FontFile = (SHPStruct *)FileSystem::LoadFile("FULLFNT3.SHP", false);

		if (FontFile == NULL) {
			file.Set_Name("FULLFNT3.SHP");
			FontFile = (SHPStruct *)Load_Alloc_Data(file);
			AllocLoaded = true;
			FUN_004068E0("MSFont: AllocLoaded FULLFNT3.SHP\n");
		}

		if (FontFile == NULL) {
			return(false);
		}

		if (use_side_palette == true) {
			file.Set_Name("SIDEFNT3.PAL");
		} else {
			file.Set_Name("FULLFNT3.PAL");
		}
		PaletteClass * palette = (PaletteClass *)Load_Alloc_Data(file);

		if (palette == NULL) {
			return(false);
		}

		for (int gindex = 0; gindex < 256; gindex++) {
			CCPalette[gindex] = RGBClass(
				((unsigned char *)*palette)[gindex*3]<<2,
				((unsigned char *)*palette)[gindex*3+1]<<2,
				((unsigned char *)*palette)[gindex*3+2]<<2);
		}

		unsigned char red = (*palette)[68].Get_Red()<<2;
		unsigned char blue = (*palette)[68].Get_Blue()<<2;
		unsigned char green = (*palette)[68].Get_Green()<<2;

		Red = red;
		Blue = blue;
		Green = green;

		Color = ((((*palette)[68].Get_Green()<<2) >> Drawing::GreenShiftRight) << Drawing::GreenShiftLeft)
			| ((((*palette)[68].Get_Blue()<<2) >> Drawing::BlueShiftRight) << Drawing::BlueShiftLeft)
			| ((((*palette)[68].Get_Red()<<2) >> Drawing::RedShiftRight) << Drawing::RedShiftLeft);

		delete palette;

		Drawer = new ConvertClass(CCPalette, CCPalette, *VisibleSurface);

		if (Drawer == NULL) {
			return(false);
		}

		FontWidth = FontFile->Get_Width();
		FontHeight = FontFile->Get_Height();
	}

	FUN_00407000();
	return(true);
}

// FUNCTION: 0x005D30C0
void MSFont::Get_String_Rect(wchar_t const * string, Rect & rect)
{
	int max_width = 0;

	if (string != NULL && wcslen(string) != 0) {

		int height = FontHeight;

		do {
			int width = 0;
			while (*string && *string != '\n') {
				width += Get_Character_Width(*string++);
			}

			if (width > max_width) {
				max_width = width;
			}

			if (*string == '\n') {
				string++;
				height += FontHeight;
			}

		} while (*string);

		rect.Set(0, 0, max_width + FontWidth, height);
		return;
	}

	rect.Set(0, 0, 0, 0);
}

// FUNCTION: 0x005D3170
int MSFont::Get_String_Width(wchar_t const * string)
{
	int max_width = 0;

	if (string != NULL) {
		do {
			int width = 0;
			while (*string && *string != '\n') {
				width += Get_Character_Width(*string++);
			}

			if (*string == '\n') {
				string++;
			}

			if (width > max_width) {
				max_width = width;
			}

		} while (*string);
	}

	return(max_width);
}

// FUNCTION: 0x005D31D0
int MSFont::Get_Character_Width(wchar_t character)
{
	unsigned char ch = (unsigned char)character;
	CharToOemBuff((LPCSTR)&ch, (LPSTR)&ch, 1);

	if (ch == ' ') {
		return(8);
	}

	if (ch > ' ') {
		int index = (0 >= ch - 33) ? 0 : ch - 33;
		index = (216 <= index) ? 216 : index;

		int shape_frame = index * 3;
		return(FontFile->GetFrameBounds(shape_frame + 2).Width + 1);
	}
	return(0);
}

// FUNCTION: 0x005D3250
void MSFont::Draw_Character(Surface * surface, wchar_t character, int x, int y, int frame, bool do_sound)
{
	char ch = (char)character;
	if (ch == 176) {
		FUN_004068E0("Denzil!\n");
	}

	CharToOemBuff((LPCSTR)&ch, (LPSTR)&ch, 1);

	if ((unsigned char)ch > ' ') {

		int index = (0 >= (unsigned char)ch - 33) ? 0 : (unsigned char)ch - 33;
		index = (216 <= index) ? 216 : index;

		int shape_frame = index * 3;

		Draw_Shape(*surface, *Drawer, (ShapeSet *)FontFile, shape_frame + frame, Point2D(x - FontFile->GetFrameBounds(shape_frame + 2).X, y), surface->Get_Rect(), SHAPE_WIN_REL);
	}
}

// FUNCTION: 0x005D3330
void MSFont::Draw_String(Surface * surface, wchar_t const * string, int x, int y, int frame)
{
	int current_x = x;

	while (*string) {
		if (*string == '\n') {
			current_x = x;
			y += FontHeight;
		} else {
			if (*string > ' ') {
				char source = (char)*string;
				char character;
				CharToOemBuff((LPCSTR)&source, (LPSTR)&character, 1);

				int index = (0 >= character - 33u) ? 0 : character - 33u;
				index = (216 <= index) ? 216 : index;

				int shape_frame = index * 3;

				Draw_Shape(*surface, *Drawer, (ShapeSet *)FontFile, shape_frame + frame, Point2D(current_x - FontFile->GetFrameBounds(shape_frame + 2).X, y), surface->Get_Rect(), SHAPE_WIN_REL);
			}

			current_x += Get_Character_Width(*string);
		}

		string++;
	}
}

// FUNCTION: 0x005D3450 MSFont::~MSFont

// FUNCTION: 0x005D3470 $E2
