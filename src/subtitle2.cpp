// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "movies.h"
#include "subtitle.h"
#include "straw.h"
#include "vector.h"
#include <cstddef>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void __cdecl FUN_004068E0(char const * format, ...);
wchar_t * __fastcall wcstrim(wchar_t * buffer);
void __stdcall Dispatch_Variable(wchar_t const * name, wchar_t * value, SubTitle * subtitle);

void __fastcall FUN_006CA6A0(wchar_t * value, SubTitle * subtitle);
void __fastcall FUN_006CA730(wchar_t * value, SubTitle * subtitle);
void __fastcall FUN_006CA7B0(wchar_t * value, SubTitle * subtitle);
void __fastcall FUN_006CA670(wchar_t * value, SubTitle * subtitle);
void __fastcall FUN_006CA680(wchar_t * value, SubTitle * subtitle);

struct SubtitleVariable
{
	wchar_t const * Name;
	void (__fastcall * Handler)(wchar_t *, SubTitle *);
};

SubtitleVariable SubtitleVariables[] = {
	{ L"Time", FUN_006CA670 },
	{ L"Duration", FUN_006CA680 },
	{ L"Position", FUN_006CA6A0 },
	{ L"Color", FUN_006CA730 },
	{ L"Text", FUN_006CA7B0 },
	{ NULL, NULL },
};

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

// FUNCTION: GAMEMD 0x006C9F10
DynamicVectorClass<SubTitle *> * SubTitleParserClass::Get_Sub_Titles(char const * moviename)
{
	DynamicVectorClass<SubTitle *> * subtitles = NULL;

	if (Find_Movie_Entry(moviename) == true) {
		subtitles = new DynamicVectorClass<SubTitle *>;

		if (subtitles != NULL) {
			for (;;) {
				wchar_t * string = Get_Next_Line();

				if (string != NULL && wcslen(string) > 0) {
					if (string[0] == L'<' && string[wcslen(string) - 1] == L'>') {
						string++;
						string[wcslen(string) - 1] = 0;
						wcstrim(string);

						if (wcslen(string) == 0) {
							continue;
						}

						SubTitle * subtitle = new SubTitle();

						if (subtitle == NULL) {
							FUN_004068E0("***** Failed to create SubTitle!\n");
							break;
						}

						if (Parse_Sub_Title(string, subtitle) == true) {
							subtitles->Add(subtitle);
						} else {
							delete subtitle;
						}

						continue;
					}

					if (wcsnicmp(string, L"EndMovie", wcslen(L"EndMovie")) == 0) {
						break;
					}
				}
			}

			if (subtitles->Count() == 0) {
				delete subtitles;
				subtitles = NULL;
			}
		}
	}

	return subtitles;
}

// FUNCTION: GAMEMD 0x006CA310
bool SubTitleParserClass::Parse_Sub_Title(wchar_t * string, SubTitle * subtitle)
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
void __stdcall Dispatch_Variable(wchar_t const * name, wchar_t * value, SubTitle * subtitle)
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
void __fastcall FUN_006CA670(wchar_t * value, SubTitle * subtitle)
{
	subtitle->Set_Display_Time(FUN_006CA5A0(value));
}

// FUNCTION: GAMEMD 0x006CA680
void __fastcall FUN_006CA680(wchar_t * value, SubTitle * subtitle)
{
	unsigned long duration = FUN_006CA5A0(value);
	if (duration > 0) {
		subtitle->Set_Display_Duration(duration);
	}
}

struct SubtitleAlignment
{
	wchar_t const * Name;
	int Value;
};

SubtitleAlignment SubtitleAlignments[] = {
	{ L"Left", 0 },
	{ L"Right", 1 },
	{ L"Center", 2 },
	{ NULL, 0 },
};

// FUNCTION: GAMEMD 0x006CA6A0
void __fastcall FUN_006CA6A0(wchar_t * value, SubTitle * subtitle)
{
	wchar_t * colon = wcschr(value, L':');
	if (colon != NULL) {
		*colon = 0;
		subtitle->Set_Line_Position(wcstol(value, NULL, 0));
		value = colon + 1;
	}

	int alignment = SubTitle::Center;
	for (int i = 0; SubtitleAlignments[i].Name != NULL; i++) {
		if (wcsicmp(value, SubtitleAlignments[i].Name) == 0) {
			alignment = SubtitleAlignments[i].Value;
			break;
		}
	}
	subtitle->Set_Alignment((SubTitle::Alignment)alignment);
}

// FUNCTION: GAMEMD 0x006CA730
void __fastcall FUN_006CA730(wchar_t * value, SubTitle * subtitle)
{
	wchar_t * colon = wcschr(value, L':');
	*colon = 0;
	unsigned char red = (unsigned char)wcstoul(value, NULL, 10);

	wchar_t * next = colon + 1;
	colon = wcschr(next, L':');
	*colon = 0;
	unsigned char green = (unsigned char)wcstoul(next, NULL, 10);

	unsigned char blue = (unsigned char)wcstoul(colon + 1, NULL, 10);

	subtitle->Set_RGB_Color(red, green, blue);
}

// FUNCTION: GAMEMD 0x006CA7B0
void __fastcall FUN_006CA7B0(wchar_t * value, SubTitle * subtitle)
{
	subtitle->Set_Caption(value);
}

// TEMPLATE: GAMEMD 0x006CA7C0
// ?Clear@?$DynamicVectorClass@PAVSubTitle@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006CA7F0
// ?ID@?$DynamicVectorClass@PAVSubTitle@@@@UAEHPBQAVSubTitle@@@Z
// TEMPLATE: GAMEMD 0x006CA810
// ?GetItem@?$VectorClass@PAVSubTitle@@@@UBEPAVSubTitle@@H@Z
template class DynamicVectorClass<SubTitle *>;

// TEMPLATE: GAMEMD 0x006CA820
// ??8?$VectorClass@PAVSubTitle@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x006CA860
// ?Resize@?$DynamicVectorClass@PAVSubTitle@@@@UAE_NHPBQAVSubTitle@@@Z
// TEMPLATE: GAMEMD 0x006CA910
// ?ID@?$DynamicVectorClass@PAVSubTitle@@@@UAEHABQAVSubTitle@@@Z
template class VectorClass<SubTitle *>;

// FUNCTION: GAMEMD 0x006CA940
bool __fastcall FUN_006CA940(Straw * straw, wchar_t * buffer, int length)
{
	buffer[0] = 0;
	bool eof = false;
	int count = 0;

	while (count < length) {
		wchar_t c;
		if (straw->Get(&c, 2) != 2) {
			eof = true;
			break;
		}
		if (c == 0xA) {
			break;
		}
		if (c != 0xD) {
			buffer[count++] = c;
		}
	}
	buffer[count] = 0;
	return eof;
}

// SYNTHETIC: GAMEMD 0x006CA9C0
// ??_G?$DynamicVectorClass@PAVSubTitle@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x006CAA10
// ?Resize@?$VectorClass@PAVSubTitle@@@@UAE_NHPBQAVSubTitle@@@Z
// TEMPLATE: GAMEMD 0x006CAAC0
// ?Clear@?$VectorClass@PAVSubTitle@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x006CAAF0
// ?ID@?$VectorClass@PAVSubTitle@@@@UAEHABQAVSubTitle@@@Z
// TEMPLATE: GAMEMD 0x006CAB30
// ?ID@?$VectorClass@PAVSubTitle@@@@UAEHPBQAVSubTitle@@@Z
// SYNTHETIC: GAMEMD 0x006CAB50
// ??_G?$VectorClass@PAVSubTitle@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x006CABA0
// ??0?$VectorClass@PAVSubTitle@@@@QAE@HPBQAVSubTitle@@@Z

// FUNCTION: GAMEMD 0x006CA4F0
wchar_t * SubTitleParserClass::Get_Next_Line(void)
{
	bool eof = false;
	while (!eof) {
		eof = FUN_006CA940(&mInput, mBuffer, 1024);
		++mLineNumber;
		wchar_t * line = wcstrim(mBuffer);
		if (wcslen(line) > 0 && *line != L';') return line;
	}
	return NULL;
}

// FUNCTION: GAMEMD 0x006CA1E0
bool SubTitleParserClass::Find_Movie_Entry(char const * moviename)
{
	wchar_t movie[32];
	mbstowcs(movie, moviename, 32);
	wchar_t * line;
	while ((line = Get_Next_Line()) != NULL) {
		if (_wcsnicmp(line, L"BeginMovie", wcslen(L"BeginMovie")) == 0) {
			wchar_t * name = wcschr(line, L' ');
			if (name) {
				wcstrim(name);
				if (_wcsicmp(name, movie) == 0) {
					FUN_004068E0("Found movie entry %s\n", moviename);
					return true;
				}
			}
		}
	}
	return false;
}
