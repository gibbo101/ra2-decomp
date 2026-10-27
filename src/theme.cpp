// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "theme.h"
#include "ccfile.h"
#include "ccini.h"
#include "ccrand.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "session.h"
#include "geometry.h"
#include "house.h"
#include "scenario.h"
#include "vector.h"
#include <string.h>
#include <stdlib.h>

// SYNTHETIC: GAMEMD 0x00720230 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x00720260 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x00720280 SYMBOL
// _$E42
// SYNTHETIC: GAMEMD 0x007202A0 SYMBOL
// _$E45
// SYNTHETIC: GAMEMD 0x007202C0 SYMBOL
// _$E48
// SYNTHETIC: GAMEMD 0x007202E0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x00720300 SYMBOL
// _$E54
// SYNTHETIC: GAMEMD 0x00720340 SYMBOL
// _$E57
// SYNTHETIC: GAMEMD 0x00720370 SYMBOL
// _$E60
// SYNTHETIC: GAMEMD 0x007203A0 SYMBOL
// _$E63
// SYNTHETIC: GAMEMD 0x007203D0 SYMBOL
// _$E66
// SYNTHETIC: GAMEMD 0x007203E0 SYMBOL
// _$E69
// SYNTHETIC: GAMEMD 0x00720400 SYMBOL
// _$E72
// SYNTHETIC: GAMEMD 0x00720420 SYMBOL
// _$E14


struct AudioStream {
	int FUN_004080D0(void);
	void FUN_00407F40(void);
	int FUN_00408070(void);
	void FUN_004080C0(void);
	void FUN_004080F0(void);
	void FUN_00407A90(void);
};
struct AudioFormat {
	char data[32];
	void FUN_004085A0(void);
	unsigned __int64 __fastcall FUN_00408560(int);
};
struct AudioSource {
	AudioStream * FUN_00407860(int a, int b);
};
extern int FUN_00407000(void);
extern AudioSource * FUN_00407010(void);
extern void __fastcall FUN_00408080(AudioStream * stream, char const * name);
extern void __fastcall FUN_00407B40(AudioStream * stream, int value);
extern void __fastcall FUN_00407B50(AudioStream * stream, int value);
extern int __fastcall FUN_00407B60(AudioStream * stream, char const * name, int flag);
extern int __fastcall FUN_00408610(CCFileClass * file, AudioFormat * format, int * length);
extern int __fastcall FUN_004790A0(int timeout);
extern void __cdecl FUN_004068E0(char const * format, ...);
extern char DAT_00889F64[];
extern char DAT_00B0F1F8[];
extern bool DAT_00A8ED64;
extern bool DAT_0089E3A0;
extern int DAT_0087E744;
extern int DAT_0087E754;

// FUNCTION: GAMEMD 0x00720440
ThemeControl::ThemeControl(void) :
	Scenario(0),
	Duration(0),
	Normal(true),
	Repeat(false),
	Available(false),
	Owner(-1)
{
	Name[0] = '\0';
	Sound[0] = '\0';
	UIName[0] = L'\0';
}

// FUNCTION: GAMEMD 0x00720480
bool ThemeControl::Fill_In(CCINIClass & ini)
{
	ini.Reset();
	if (ini.Is_Present(Name)) {
		ini.Get_String(Name, "Sound", DAT_00889F64, Sound, sizeof(Sound));
		char * ptr = Sound;
		while (*ptr == '$' || *ptr == '#') {
			ptr++;
		}
		strcpy(Sound, ptr);
		Scenario = ini.Get_Int(Name, "Scenario", Scenario);
		Normal = ini.Get_Bool(Name, "Normal", Normal);
		Repeat = ini.Get_Bool(Name, "Repeat", Repeat);
		Owner = ini.Get_Side(Name, "Side", (SideType)Owner);
		if (Normal) {
			ini.ReadStringtableEntry(Name, "Name", UIName, 64);
		}
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00720590
void ThemeClass::Init_Themes(CCINIClass & ini)
{
	ThemeControl *ctrl;
	int count = ini.Entry_Count("Themes");
	for (int i = 0; i < count; i++) {
		char name[32];
		if (ini.Get_String("Themes", ini.Get_Entry("Themes", i), DAT_00889F64, name, sizeof(name))) {

			ThemeType theme = From_Name(name);
			if (theme == THEME_NONE) {
				ctrl = new ThemeControl;
				strcpy(ctrl->Name, name);
				Themes.Add(ctrl);
			} else {
				ctrl = Themes[theme];
			}

			ctrl->Fill_In(ini);
		}
	}

	if (FUN_00407010()) {
		Stream = FUN_00407010()->FUN_00407860(20000, 0);
		if (Stream) {
			FUN_00408080(Stream, "Music Stream");
			FUN_00407B40(Stream, DAT_0087E744);
			FUN_00407B50(Stream, DAT_0087E754);
		}
	}
}

// FUNCTION: GAMEMD 0x00720770
void ThemeClass::Free_Themes(void)
{
	while (Themes.Count() > 0) {
		delete Themes[0];
		Themes.Delete_Index(0);
	}
	if (Stream) {
		Stream->FUN_004080C0();
		Stream->FUN_004080F0();
		Stream->FUN_00407F40();
		Stream->FUN_00407A90();
		Stream = NULL;
	}
}

// FUNCTION: GAMEMD 0x007207F0
void ThemeClass::Scan(void)
{
	char name[512];

	if (ScoresPresent && FUN_00407000() && !DAT_00A8ED64) {
		for (ThemeType theme = THEME_FIRST; theme < Themes.Count(); theme++) {
			_makepath(name, NULL, NULL, Themes[theme]->Sound, ".WAV");
			CCFileClass file(name);
			Themes[theme]->Available = file.Is_Available(0);
			if (Themes[theme]->Available) {
				AudioFormat format;
				int length;
				format.FUN_004085A0();
				file.Open(1);
				if (FUN_00408610(&file, &format, &length)) {
					Themes[theme]->Duration = (int)(format.FUN_00408560(length) / 1000);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00720940
char const * ThemeClass::Base_Name(ThemeType theme) const
{
	if ((unsigned)theme < (unsigned)Themes.Count()) {
		return(Themes[theme]->Name);
	}
	return("No theme");
}

// FUNCTION: GAMEMD 0x00720960
ThemeClass::ThemeClass(void) :
	Current(-1),
	Score(THEME_NONE),
	Pending(THEME_NONE),
	Volume(255),
	IsRepeat(false),
	IsFading(false),
	IsShuffle(false),
	Stream(NULL)
{
}

// FUNCTION: GAMEMD 0x007209B0
wchar_t const * ThemeClass::Full_Name(ThemeType theme) const
{
	if ((unsigned)theme < (unsigned)Themes.Count()) {
		return(Themes[theme]->UIName);
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x007209D0
void ThemeClass::AI(void)
{
	if (FUN_00407000() && !DAT_00A8ED64) {
		if (ScoresPresent && Volume > 0 && !Still_Playing()) {
			if (Pending != THEME_NONE && Pending != THEME_QUIET && !ScenarioInit) {
				if (Pending == THEME_PICK_ANOTHER) {
					Pending = Next_Song(Score);
					FUN_004068E0("Theme::AI(Next song = %d)\n", Pending);
				}
				Play_Song(Pending);
				Pending = THEME_PICK_ANOTHER;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00720A80
ThemeType ThemeClass::Next_Song(ThemeType theme) const
{
	int i;

	if (theme > THEME_NONE && (!Themes[theme]->Repeat && !IsRepeat) || theme < THEME_FIRST) {
		if (IsShuffle == true) {
			ThemeType newtheme;
			i = 0;
			do {
				newtheme = (ThemeType)NonCriticalRandomNumber(THEME_FIRST, Themes.Count() - 1);
				i++;
			} while (i < 1000 && (newtheme == theme || !Is_Allowed(newtheme)));
			if (i == 1000) {
				newtheme = THEME_FIRST;
			}
			return(newtheme);

		} else {
			i = Themes.Count();
			i++;
			do {
				theme = ThemeType(theme + 1);
				if (theme >= Themes.Count()) {
					theme = THEME_FIRST;
				}
				i--;
				if (i == 0) {
					return(THEME_FIRST);
				}
			} while (!Is_Allowed(theme));
		}
	}
	return(theme);
}

// FUNCTION: GAMEMD 0x00720B20
void ThemeClass::Queue_Song(ThemeType theme)
{
	if (!ScoresPresent) return;
	if (!FUN_00407000() || DAT_00A8ED64) return;

	Pending = theme;
	if (theme == THEME_NONE || theme == THEME_PICK_ANOTHER || theme == Score) return;

	FUN_004068E0("Theme::QueueSong(%d)\n", theme);
	if (Still_Playing() == true) {
		if (Stream) Stream->FUN_004080C0();
		IsFading = true;
	}
}

// FUNCTION: GAMEMD 0x00720BB0
int ThemeClass::Play_Song(ThemeType theme)
{
	if (ScoresPresent && FUN_00407000() && !DAT_00A8ED64) {
		if (Still_Playing() && Score == theme) return(Current);
		Stop(false);
		IsFading = false;
		if (theme != THEME_NONE && theme != THEME_QUIET) {
			if (theme > THEME_NONE && Volume > 0) {
				Score = theme;
				int cd = DAT_0089E3A0 ? 0 : FUN_004790A0(0);
				if (Stream && cd > -1) {
					FUN_00407B60(Stream, Theme_File_Name(theme), 1);
					Current = theme;
				} else {
					Current = -1;
					if (Stream && cd <= -1 && Pending == THEME_NONE) {
						Theme.Queue_Song(theme);
					}
				}
				FUN_004068E0("Theme::PlaySong(%d) - %s\n", Score, IsRepeat == true || Themes[theme]->Repeat == true ? "Repeating" : "Playing");
				if (IsRepeat == true || Themes[theme]->Repeat == true) {
					Pending = theme;
				}
			} else {
				Pending = theme;
			}
		}
	}
	return(Current);
}

// FUNCTION: GAMEMD 0x00720E10
char const * ThemeClass::Theme_File_Name(ThemeType theme)
{
	if ((unsigned)theme < (unsigned)Themes.Count()) {
		_makepath(DAT_00B0F1F8, NULL, NULL, Themes[theme]->Sound, ".WAV");
		return(DAT_00B0F1F8);
	}
	return(DAT_00889F64);
}

// FUNCTION: GAMEMD 0x00720E50
int ThemeClass::Track_Length(ThemeType theme) const
{
	if ((unsigned)theme < (unsigned)Themes.Count()) {
		return (int)Themes[theme]->Duration;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00720E80
void ThemeClass::Fade_Stream(void)
{
	if (Stream) Stream->FUN_004080C0();
	IsFading = true;
}

// FUNCTION: GAMEMD 0x00720EA0
void ThemeClass::Stop(bool fade)
{
	if (ScoresPresent && FUN_00407000() && !DAT_00A8ED64 && Current != -1) {
		if (fade && Still_Playing() == true) {
			FUN_004068E0("Theme::Stop(%d) - Fading\n", Score);
			if (Stream) Stream->FUN_004080C0();
			IsFading = true;
		} else {
			FUN_004068E0("Theme::Stop(%d)\n", Score);
			if (Stream) Stream->FUN_00407F40();
		}
		Current = -1;
		Score = THEME_NONE;
		Pending = THEME_NONE;
	}
}

// FUNCTION: GAMEMD 0x00720F70
void ThemeClass::Suspend(void)
{
	if (ScoresPresent && FUN_00407000() && !DAT_00A8ED64 && Current != -1) {
		FUN_004068E0("Theme::Suspend(%d)\n", Score);
		if (Stream) Stream->FUN_00407F40();
		Current = -1;
		Pending = Score;
		Score = THEME_NONE;
	}
}

// FUNCTION: GAMEMD 0x00720FD0
bool ThemeClass::Still_Playing(void) const
{
	if (Stream) {
		if (IsFading && !Stream->FUN_004080D0()) {
			Stream->FUN_00407F40();
		}
		return(Stream->FUN_00408070() != 0);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00721010
long ThemeClass::Save(IStream * stream)
{
	long hr = stream->Write(&Score, 4, 0);
	if (hr >= 0) {
		hr = stream->Write(&Pending, 4, 0);
	}
	return hr;
}

// FUNCTION: GAMEMD 0x00721040
long __stdcall Theme_Load(IStream * stream)
{
	int score;
	int pending;
	long hr = stream->Read(&score, 4, 0);
	if (hr >= 0) {
		hr = stream->Read(&pending, 4, 0);
		if (hr >= 0 && Options.InGameMusic) {
			Theme.Play_Song((ThemeType)score);
			Theme.Queue_Song((ThemeType)pending);
		}
	}
	return hr;
}

// FUNCTION: GAMEMD 0x00721140
bool ThemeClass::Is_Allowed(ThemeType index) const
{
	if (index == THEME_QUIET || index == THEME_PICK_ANOTHER) return true;
	if ((unsigned)index >= (unsigned)Themes.Count()) return false;
	if (!Themes[index]->Available) return false;
	if (!Themes[index]->Normal) return false;
	if (PlayerPtr != NULL && Themes[index]->Owner != -1 && PlayerPtr->Class->Side != Themes[index]->Owner) return false;
	if (Session.Type == GAME_NORMAL && Scen->Scenario < Themes[index]->Scenario) return false;
	return true;
}

// FUNCTION: GAMEMD 0x007211E0
bool ThemeClass::IsNormal(ThemeType index) const
{
	if (index != THEME_QUIET && index != THEME_PICK_ANOTHER && (unsigned)index < (unsigned)Themes.Count()) {
		if (Themes[index]->Normal) return true;
		return false;
	}
	return false;
}

// FUNCTION: GAMEMD 0x00721210
ThemeType ThemeClass::From_Name(char const * name) const
{
	if (name && strlen(name) > 0) {
		ThemeType theme;
		for (theme = THEME_FIRST; theme < Themes.Count(); theme = ThemeType(theme + 1)) {
			if (stricmp(Themes[theme]->Name, name) == 0) {
				return(theme);
			}
		}
	}
	return(THEME_NONE);
}

// FUNCTION: GAMEMD 0x00721270
char const * ThemeClass::GetID(ThemeType index) const
{
	if (index >= 0 && index < Themes.Count()) {
		return Themes[index]->Name;
	}
	return "none";
}

// FUNCTION: GAMEMD 0x00721290
void ThemeClass::Set_Volume(int volume)
{
	Volume = MIN(volume, 255);
}

// TEMPLATE: GAMEMD 0x007212B0
// ?Clear@?$DynamicVectorClass@PAUThemeControl@@@@UAEXXZ
template class DynamicVectorClass<ThemeControl *>;

// TEMPLATE: GAMEMD 0x007212E0
// ?ID@?$DynamicVectorClass@PAUThemeControl@@@@UAEHPBQAUThemeControl@@@Z

// TEMPLATE: GAMEMD 0x00721300
// ?Resize@?$DynamicVectorClass@PAUThemeControl@@@@UAE_NHPBQAUThemeControl@@@Z

// TEMPLATE: GAMEMD 0x007213B0
// ?ID@?$DynamicVectorClass@PAUThemeControl@@@@UAEHABQAUThemeControl@@@Z

// SYNTHETIC: GAMEMD 0x007213E0
// ??_G?$DynamicVectorClass@PAUThemeControl@@@@UAEPAXI@Z

template class VectorClass<ThemeControl *>;
