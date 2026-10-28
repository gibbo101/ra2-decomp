// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <windows.h>
#undef LoadString
#include "csf.h"

// The game's localised text, keyed by label; the source file and line of each lookup
// are passed for its error report.
class StringTable
{
	public:
		static CSFString * LastLoadedString;
		static int MaxLabelLen;
		static int LabelCount;
		static int ValueCount;
		static CSFLanguages Language;
		static int IsLoaded;
		static char * FileName;
		static CSFLabel * Labels;
		static wchar_t ** Values;
		static char ** ExtraValues;

		static wchar_t const * LoadString(char const * label, char ** extra, char const * file, int line);
		static int __fastcall LoadFile(char const * filename);
		static int __fastcall ReadFile(char const * filename);
		static CSFLanguage const * __fastcall GetLanguage(CSFLanguages language);
		static char const * __fastcall GetLanguageName(CSFLanguages language);
		static void Unload(void);
};

char * __fastcall FUN_007350C0(wchar_t const * text);
wchar_t * __fastcall FUN_00735120(char const * text);

// SYNTHETIC: GAMEMD 0x00734430 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00734460 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00734480 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x007344A0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x007344C0 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x007344E0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00734500 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00734540 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00734570 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x007345A0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x007345D0 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x007345E0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00734600 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00734620 SYMBOL
// _$E41

extern CSFLanguage DAT_00845678[];

// FUNCTION: GAMEMD 0x00734640
CSFLanguage const * __fastcall StringTable::GetLanguage(CSFLanguages language)
{
	for (CSFLanguage const * entry = DAT_00845678; entry->Index != CSF_UNKNOWN; entry++) {
		if (entry->Index == language) {
			return entry;
		}
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x00734670
char const * __fastcall StringTable::GetLanguageName(CSFLanguages language)
{
	CSFLanguage const * entry = GetLanguage(language);
	if (entry) {
		return entry->Name;
	}
	return "unknown";
}

extern void * DAT_00B1CF8C;
void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x00734D30
void StringTable::Unload(void)
{
	void ** node;
	while ((node = (void **)DAT_00B1CF8C) != NULL) {
		DAT_00B1CF8C = *node;
		delete node;
	}
	DAT_00B1CF8C = NULL;

	if (Values) {
		for (int i = 0; i < ValueCount; i++) {
			if (Values[i]) {
				delete Values[i];
			}
		}
		delete Values;
		Values = NULL;
	}
	if (ExtraValues) {
		for (int i = 0; i < ValueCount; i++) {
			if (ExtraValues[i]) {
				delete ExtraValues[i];
			}
		}
		delete ExtraValues;
		ExtraValues = NULL;
	}
	if (Labels) {
		delete Labels;
		Labels = NULL;
	}
	CSFString * string = LastLoadedString;
	while (string) {
		CSFString * previous = string->PreviousEntry;
		delete string;
		string = previous;
	}
	LastLoadedString = NULL;
	IsLoaded = 0;
}

// FUNCTION: GAMEMD 0x00734E30
int __fastcall FUN_00734E30(wchar_t const * text)
{
	while (*text) {
		if (*text++ > 0xFF) {
			return 0;
		}
	}
	return 1;
}

// FUNCTION: GAMEMD 0x00734E60
wchar_t const * StringTable::LoadString(char const * label, char ** extra, char const * file, int line)
{
	if (extra) {
		*extra = NULL;
	}
	if (!Labels) {
		return L"***FATAL*** String Manager failed to initilaized properly";
	}
	CSFLabel * found = (CSFLabel *)bsearch(label, Labels, LabelCount, sizeof(CSFLabel), (int (__cdecl *)(const void *, const void *))stricmp);
	if (found) {
		if (extra) {
			*extra = ExtraValues[found->FirstValueIndex];
		}
		return Values[found->FirstValueIndex];
	}
	CSFString * string = new CSFString;
	swprintf(string->Text, L"MISSING:\'%hs\'", label, file, line);
	FUN_004068E0("***NO_STRING*** '%s' -> file:%s line:%d\n", label, file, line);
	string->PreviousEntry = LastLoadedString;
	LastLoadedString = string;
	return string->Text;
}

// FUNCTION: GAMEMD 0x00734F20
int FUN_00734F20(void)
{
	char const * letter = DAT_00845678[StringTable::Language].Letter;
	if (!letter) {
		return 1;
	}
	return stricmp(letter, "e") == 0;
}

extern int DAT_00B1CF84;
extern int DAT_00B1CF60;

// FUNCTION: GAMEMD 0x00734F50
void __fastcall FUN_00734F50(int value)
{
	if (DAT_00B1CF84 != value) {
		StringTable::Unload();
		DAT_00B1CF84 = value;
		StringTable::LoadFile(StringTable::FileName);
	}
}

// FUNCTION: GAMEMD 0x00734F80
void __fastcall FUN_00734F80(int value)
{
	if (DAT_00B1CF60 != value) {
		StringTable::Unload();
		DAT_00B1CF60 = value;
		StringTable::LoadFile(StringTable::FileName);
	}
}

// FUNCTION: GAMEMD 0x00734FB0
int FUN_00734FB0(void)
{
	return DAT_00B1CF84;
}

// FUNCTION: GAMEMD 0x00734FC0
int FUN_00734FC0(void)
{
	return DAT_00B1CF60;
}

// FUNCTION: GAMEMD 0x00734FD0
int FUN_00734FD0(void)
{
	return StringTable::IsLoaded;
}

// FUNCTION: GAMEMD 0x00734FE0
int __fastcall FUN_00734FE0(HWND window, wchar_t const * text, wchar_t const * caption, UINT type)
{
	char text_buffer[0x1400];
	char caption_buffer[0x800];
	WideCharToMultiByte(0, 0, text, -1, text_buffer, sizeof(text_buffer), 0, 0);
	WideCharToMultiByte(0, 0, caption, -1, caption_buffer, sizeof(caption_buffer), 0, 0);
	return MessageBoxA(window, text_buffer, caption_buffer, type);
}

// FUNCTION: GAMEMD 0x00735060
void __fastcall FUN_00735060(wchar_t * dest, char const * src, int max)
{
	while (*src && max) {
		*dest++ = (unsigned char)*src++;
		if (max != -1) {
			max--;
		}
	}
	*dest = 0;
}

// FUNCTION: GAMEMD 0x00735090
void __fastcall WideToCharString(char * dest, wchar_t const * src, int max)
{
	while (*src && max) {
		*dest++ = (char)*src++;
		if (max != -1) {
			max--;
		}
	}
	*dest = 0;
}

extern char DAT_00B0FE94[8][0x400];
extern wchar_t DAT_00B13F24[8][0x400];
extern int DAT_00B1CF90;
extern int DAT_00B1CF94;

// FUNCTION: GAMEMD 0x007350C0
char * __fastcall FUN_007350C0(wchar_t const * text)
{
	if (!text) {
		return NULL;
	}
	char * buffer = DAT_00B0FE94[DAT_00B1CF90];
	WideToCharString(buffer, text, 0x400);
	buffer[0x3FF] = 0;
	DAT_00B1CF90 = (DAT_00B1CF90 + 1) & 7;
	return buffer;
}

// FUNCTION: GAMEMD 0x00735120
wchar_t * __fastcall FUN_00735120(char const * text)
{
	if (!text) {
		return NULL;
	}
	wchar_t * buffer = DAT_00B13F24[DAT_00B1CF94];
	FUN_00735060(buffer, text, 0x400);
	buffer[0x3FF] = 0;
	DAT_00B1CF94 = (DAT_00B1CF94 + 1) & 7;
	return buffer;
}
