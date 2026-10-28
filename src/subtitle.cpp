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
#include <cstddef>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void __cdecl FUN_004068E0(char const * format, ...);
wchar_t * __fastcall wcstrim(wchar_t * buffer);
void __stdcall Dispatch_Variable(wchar_t const * name, wchar_t * value, SubTitleClass * subtitle);

void __fastcall FUN_006CA6A0(wchar_t * value, SubTitleClass * subtitle);
void __fastcall FUN_006CA730(wchar_t * value, SubTitleClass * subtitle);
void __fastcall FUN_006CA7B0(wchar_t * value, SubTitleClass * subtitle);
void __fastcall FUN_006CA670(wchar_t * value, SubTitleClass * subtitle);
void __fastcall FUN_006CA680(wchar_t * value, SubTitleClass * subtitle);

struct SubtitleVariable
{
	wchar_t const * Name;
	void (__fastcall * Handler)(wchar_t *, SubTitleClass *);
};

SubtitleVariable SubtitleVariables[] = {
	{ L"Time", FUN_006CA670 },
	{ L"Duration", FUN_006CA680 },
	{ L"Position", FUN_006CA6A0 },
	{ L"Color", FUN_006CA730 },
	{ L"Text", FUN_006CA7B0 },
	{ NULL, NULL },
};

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
			SubTitleClass * subtitle = (*mSubTitles)[i];
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

// FUNCTION: GAMEMD 0x006C9C60
bool MovieSubtitles::Process(unsigned long movietime)
{
	if (mSubTitles == NULL) {
		return false;
	}

	bool changed = false;
	while (mSubTitleIndex < mSubTitles->Count()) {
		SubTitleClass * subtitle = (*mSubTitles)[mSubTitleIndex];
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

// FUNCTION: GAMEMD 0x006C9EC0
SubTitleParserClass::SubTitleParserClass(Straw & input)
	:
	mInput(input),
	mLineNumber(0)
{
	wchar_t bom = 0;
	mInput.Get(&bom, sizeof(wchar_t));
	if (bom != 0xFEFF) {
		FUN_004068E0("Error: Subtitle control file is not unicode!\n");
	}
}

// FUNCTION: GAMEMD 0x006C9F00
SubTitleParserClass::~SubTitleParserClass(void)
{
}

// FUNCTION: GAMEMD 0x006CA310
bool SubTitleParserClass::Parse_Sub_Title(wchar_t * string, SubTitleClass * subtitle)
{
	for (;;) {
		wchar_t * separator = wcschr(string, L'=');

		if (separator == NULL) {
			FUN_004068E0("Error on line %d: syntax error\n", mLineNumber);
			return false;
		}

		*separator++ = 0;

		wchar_t * token = string;
		wcstrim(token);

		wchar_t * param = separator;
		wcstrim(param);

		if (param[0] == L'"') {
			param++;
			separator = wcschr(param, L'"');

			if (separator == NULL) {
				FUN_004068E0("Error on line %d: mismatched quotes\n", mLineNumber);
				return false;
			}

			*separator++ = 0;
			wcstrim(separator);

			if (*separator == L',') {
				separator++;
			}
			string = separator;
		} else {
			separator = wcspbrk(param, L", ");

			if (separator != NULL) {
				*separator++ = 0;
				string = separator;
			} else {
				string = L"";
			}
		}

		if (wcslen(token) == 0) {
			FUN_004068E0("Error on line %d: missing token\n", mLineNumber);
			return false;
		}

		Dispatch_Variable(token, param, subtitle);

		wcstrim(string);

		if (wcslen(string) == 0) {
			break;
		}
	}

	return true;
}

// FUNCTION: GAMEMD 0x006CA4A0
void __stdcall Dispatch_Variable(wchar_t const * name, wchar_t * value, SubTitleClass * subtitle)
{
	if (name != NULL && SubtitleVariables[0].Name != NULL) {
		SubtitleVariable * var = SubtitleVariables;
		SubtitleVariable * entry = var;
		while (entry->Name != NULL) {
			if (wcsicmp(var->Name, name) == 0) {
				var->Handler(value, subtitle);
				break;
			}
			entry++;
			var = entry;
		}
	}
}

// FUNCTION: GAMEMD 0x006CA5A0
unsigned long __fastcall FUN_006CA5A0(wchar_t * text)
{
	wchar_t buf[0xc];
	wchar_t * colon;

	wcsncpy(buf, text, 0xc);
	buf[0xb] = 0;

	colon = wcschr(buf, L':');
	*colon = 0;
	unsigned long hours = wcstoul(buf, NULL, 10);

	wchar_t * minutes = colon + 1;
	colon = wcschr(minutes, L':');
	*colon = 0;
	unsigned long mins = wcstoul(minutes, NULL, 10);

	wchar_t * seconds = colon + 1;
	colon = wcschr(seconds, L':');
	*colon = 0;
	unsigned long secs = wcstoul(seconds, NULL, 10);

	wchar_t * frames = colon + 1;
	unsigned long hundredths = wcstoul(frames, NULL, 10);

	return (((hours * 60 + mins) * 60 + secs) * 60) + (hundredths * 60) / 100;
}

// FUNCTION: GAMEMD 0x006CA670
void __fastcall FUN_006CA670(wchar_t * value, SubTitleClass * subtitle)
{
	subtitle->Set_Display_Time(FUN_006CA5A0(value));
}

// FUNCTION: GAMEMD 0x006CA680
void __fastcall FUN_006CA680(wchar_t * value, SubTitleClass * subtitle)
{
	unsigned long duration = FUN_006CA5A0(value);
	if (duration > 0) {
		subtitle->Set_Display_Duration(duration);
	}
}
