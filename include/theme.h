// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "stimer.h"
#include "vector.h"

#include "side.hh"
#include "theme.hh"

class CCINIClass;
class AudioStream;

struct ThemeControl {
	ThemeControl(void);
	bool Fill_In(CCINIClass & ini);

	char Name[256];
	char Sound[256];
	wchar_t UIName[64];
	int Scenario;
	float Duration;
	bool Normal;
	bool Repeat;
	bool Available;
	int Owner;
};

class ThemeClass
{
	private:
		char const * Theme_File_Name(ThemeType theme);

		int Current;
		ThemeType Score;
		ThemeType Pending;

		int Volume;

		bool IsRepeat;
		bool IsFading;

		bool IsShuffle;

		DynamicVectorClass<ThemeControl *> Themes;
		AudioStream * Stream;

		enum {
			THEME_DELAY=TIMER_SECOND
		};

	public:
		ThemeClass(void);

		ThemeType From_Name(char const * name) const;
		ThemeType Next_Song(ThemeType index) const;
		ThemeType What_Is_Playing(void) const {return(Score == THEME_NONE ? Pending : Score);}
		bool Is_Allowed(ThemeType index) const;
		bool Is_Regular(ThemeType theme) const {return(theme != THEME_NONE && Themes[theme]->Normal);}
		char const * Base_Name(ThemeType index) const;
		wchar_t const * Full_Name(ThemeType index) const;
		char const * GetID(ThemeType index) const;
		int Max_Themes(void) const {return(Themes.Count());}
		int Play_Song(ThemeType index);
		bool Still_Playing(void) const;
		int Track_Length(ThemeType index) const;
		void Scan(void);
		void AI(void);
		void Fade_Out(void) {Queue_Song(THEME_QUIET);}
		void Queue_Song(ThemeType index);
		void Set_Theme_Data(ThemeType theme, int scenario, int owners);
		void Stop(bool fade = false);
		void Suspend(void);
		void Set_Shuffle(bool on) {IsShuffle = on;}
		void Set_Repeat(bool on) {IsRepeat = on;}
		bool Is_Shuffle(void) const {return(IsShuffle);}

		void Set_Volume(int volume);
		bool IsNormal(ThemeType index) const;
		long Save(struct IStream * stream);
		long Load(struct IStream * stream);
		void Fade_Stream(void);

		void Init_Themes(CCINIClass & ini);
		void Free_Themes(void);
};

extern ThemeClass Theme;
