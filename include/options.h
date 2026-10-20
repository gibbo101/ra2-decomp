// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "keyboard.h"

#include "diff.hh"

class OptionsClass {
	public:
		enum {
			MAX_SCROLL_SETTING=7,
			MAX_SPEED_SETTING=7,
			MAX_DETAIL_SETTING=3,
			MAX_DIFFICULTY_SETTING=DIFF_COUNT
		};

		OptionsClass(void);

		void One_Time(void);

		void Fixup_Palette(void) const;
		void Set_Shuffle(bool on);
		void Set_Repeat(bool on);
		void Set_Score_Volume(float volume, bool feedback);
		void Set_Sound_Volume(float volume, bool feedback);
		void Set_Voice_Volume(float volume, bool feedback);
		int Normalize_Delay(int delay) const;
		int Normalize_Volume(int volume) const;
		int Get_Scroll_Method(void) const;

		bool Hotkey_Dialog(void);
		void FUN_005FBF80(char const * name);
		void FUN_005FC000(HWND list);

		void Load_Settings(void);
		void Save_Settings(void);

		void Setup_Dest_Net(void);
		void Set(void);

		int GameSpeed;
		int Difficulty;
		int CampDifficulty;
		int ScrollMethod;
		int ScrollRate;
		bool AutoScroll;
		int DetailLevel;
		bool IsSidebarOnRight;
		bool SidebarCameoText;
		bool ActionLines;
		bool ShowHidden;
		bool ToolTips;
		int ScreenWidth;
		int ScreenHeight;
		int ShellWidth;
		int ShellHeight;
		bool StretchMovies;
		bool AllowHiResModes;
		bool AllowVRAMSidebar;
		float SoundVolume;
		float VoiceVolume;
		float ScoreVolume;
		bool IsScoreRepeat;
		bool InGameMusic;
		bool IsScoreShuffle;
		unsigned short SoundLatency;
		unsigned short Socket;
		int LastUnlockedSovMovie;
		int LastUnlockedAllMovie;
		int NetCard;
		char DestNet[64];

		KeyNumType KeyForceMove1;
		KeyNumType KeyForceMove2;
		KeyNumType KeyForceAttack1;
		KeyNumType KeyForceAttack2;
		KeyNumType KeySelect1;
		KeyNumType KeySelect2;
		KeyNumType KeyQueueMove1;
		KeyNumType KeyQueueMove2;

	private:

		static char const * const HotkeyName;
};
