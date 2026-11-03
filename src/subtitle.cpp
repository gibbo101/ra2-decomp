// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "movies.h"
#include "subtitle.h"
#include "bsurface.h"
#include "straw.h"
#include "vector.h"
#include "bitfont.h"
#include "dsurface.h"
#include "ccfile.h"
#include "xstraw.h"
#undef DrawText
#include <cstddef>
#include <stdlib.h>
#include <string.h>

// FUNCTION: GAMEMD 0x006C99D0
MovieSubtitles * __fastcall Subtitles_Open(char const * name)
{
	char fname[0x100];

	if (name != NULL && strlen(name) != 0) {
		MovieSubtitles * subs = new MovieSubtitles;
		if (subs != NULL) {
			_splitpath(name, NULL, NULL, fname, NULL);
			if (subs->Load_Sub_Titles(fname) == true) {
				BSurface * surface = new BSurface(640, 255, 2);
				if (surface != NULL) {
					surface->Fill(0);
					subs->SubSurface = surface;
				}
			}
		}
		return subs;
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x006C9AC0
MovieSubtitles::MovieSubtitles(void)
	:
	mSubTitles(NULL),
	mSubTitleIndex(0),
	mActiveSubTitle(NULL),
	Callback(NULL),
	SubSurface(NULL),
	CaptionRect(0, 0, 0, 0)
{
}

// FUNCTION: GAMEMD 0x006C9AE0
MovieSubtitles::~MovieSubtitles(void)
{
	if (mSubTitles != NULL) {
		for (int i = 0; i < mSubTitles->Count(); i++) {
			SubTitle * subtitle = (*mSubTitles)[i];
			if (subtitle != NULL) {
				delete subtitle;
			}
		}
		delete mSubTitles;
	}
	if (SubSurface != NULL) {
		delete SubSurface;
	}
}

// FUNCTION: GAMEMD 0x006C9B40
bool MovieSubtitles::Load_Sub_Titles(char const * moviename)
{
	if (moviename == NULL || strlen(moviename) == 0) {
		return false;
	}

	CCFileClass file("subtitlemd.txt");

	if (!file.Is_Available()) {
		return false;
	}

	FileStraw input(file);
	SubTitleParserClass parser(input);

	mSubTitles = parser.Get_Sub_Titles(moviename);

	if (mSubTitles == NULL) {
		return false;
	}

	return true;
}

// FUNCTION: GAMEMD 0x006C9C60
bool MovieSubtitles::Process(unsigned long movietime)
{
	if (mSubTitles == NULL) {
		return false;
	}

	bool changed = false;
	while (mSubTitleIndex < mSubTitles->Count()) {
		SubTitle * subtitle = (*mSubTitles)[mSubTitleIndex];
		if (subtitle->Get_Display_Time() > movietime) {
			break;
		}
		mActiveSubTitle = subtitle;
		mSubTitleIndex++;
		if (Callback != NULL) {
			Callback(subtitle);
		} else {
			Draw_Sub_Title(subtitle);
			changed = true;
		}
	}

	if (mActiveSubTitle != NULL && movietime >= mActiveSubTitle->Get_Display_Time() + mActiveSubTitle->Get_Display_Duration()) {
		mActiveSubTitle = NULL;
		if (Callback == NULL && SubSurface != NULL) {
			Rect & caption = CaptionRect;
			SubSurface->Fill_Rect(caption, 0);
			changed = true;
			PreviousRect = caption;
			caption.X = 0;
			caption.Y = 0;
			caption.Width = 0;
			caption.Height = 0;
		}
	}
	return changed;
}

// FUNCTION: GAMEMD 0x006C9D20
void MovieSubtitles::Reset(void)
{
	mSubTitleIndex = 0;
	mActiveSubTitle = NULL;
}

// FUNCTION: GAMEMD 0x006C9D30
void MovieSubtitles::SetCallback(void (__fastcall * callback)(void *))
{
	Callback = callback;
}

// FUNCTION: GAMEMD 0x006C9D40
void MovieSubtitles::Draw_Sub_Title(SubTitle * subtitle)
{
	if (SubSurface != NULL) {
		Rect & caption = CaptionRect;
		SubSurface->Fill_Rect(caption, 0);

		unsigned long color = subtitle->Get_RGB_Color();
		BitFont * font = BitFont::Instance;
		font->SetColor(DSurface::Build_Hicolor_Pixel((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));

		int width = SubSurface->Get_Width();
		int height = SubSurface->Get_Height();
		LTRBStruct bounds;
		bounds.Left = 0;
		bounds.Top = 0;
		bounds.Right = width;
		bounds.Bottom = height;
		font->SetBounds(&bounds);

		int text_width;
		int text_height;
		wchar_t const * text = subtitle->Get_Caption();
		font->GetTextDimension(text, &text_width, &text_height, width);

		int y = (subtitle->Get_Line_Position() - 1) * font->field_1C;
		int x = 0;
		if (subtitle->Get_Alignment() == SubTitle::Center) {
			x = (width - text_width) / 2;
		} else if (subtitle->Get_Alignment() == SubTitle::Right) {
			x = width - text_width;
		}
		BitText::Instance->DrawText(font, SubSurface, text, x, y, width, height, 0, 0, 0);

		PreviousRect = caption;
		caption.X = x;
		caption.Y = y;
		caption.Width = text_width;
		caption.Height = text_height;
	}
}
