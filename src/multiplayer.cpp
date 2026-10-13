// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "multiplayer.h"
#include "ccini.h"
#include "ccfile.h"
#include "session.h"
#include <windows.h>
#undef LoadString

extern char const * DAT_007EED18;

// FUNCTION: 0x005D5AC0
DynamicVectorClass<MPGameModeClass *> & Game_Modes(void)
{
	static DynamicVectorClass<MPGameModeClass *> modes;
	return(modes);
}

// FUNCTION: 0x005D5B20 $E69

static int Game_Mode_Count(void)
{
	return(Game_Modes().Count());
}

static inline bool Has_Section(CCINIClass * ini)
{
	if (ini == NULL) {
		return(false);
	}
	return(ini->GetSection(DAT_007EED18) != NULL);
}

static inline CCINIClass * New_INI(void)
{
	return(new CCINIClass);
}

static inline bool Must_Ally_Without_Allies(bool must, bool allies)
{
	return(must && !allies);
}

// FUNCTION: 0x005D5B60
MPGameModeClass::MPGameModeClass(WideWstring const & title, WideWstring const & tooltip, Wstring const & inifile, Wstring const & mapfilter, bool aiallowed, int modeindex) :
	MPTeams(),
	UIName(title),
	CSFTooltip(tooltip),
	MPModeIndex(modeindex),
	INIFilename(inifile),
	MapFilter(mapfilter),
	AIAllowed(aiallowed),
	INI(NULL)
{
	unknown_4 = false;
	wolTourney = true;
	wolClanTourney = true;
	AlliesAllowed = true;
	MustAlly = false;
	INI = New_INI();
	{
		CCFileClass file(INIFilename);
		if (file.Is_Available(false)) {
			INI->ReadCCFile(file, false, false);
		}
	}
	if (Has_Section(INI)) {
		wolTourney = INI->Get_Bool(DAT_007EED18, "WonlineTournamentAllowed", wolTourney);
		wolClanTourney = INI->Get_Bool(DAT_007EED18, "WonlineClanTournamentAllowed", wolClanTourney);
		AlliesAllowed = INI->Get_Bool(DAT_007EED18, "AlliesAllowed", AlliesAllowed);
		MustAlly = INI->Get_Bool(DAT_007EED18, "MustAlly", MustAlly);
		if (Must_Ally_Without_Allies(MustAlly, AlliesAllowed)) {
			MustAlly = false;
		}
	}
}

// FUNCTION: 0x005D5D20
MPGameModeClass::~MPGameModeClass(void)
{
	for (int i = 0; i < MPTeams.Count(); i++) {
		delete MPTeams[i];
	}
	MPTeams.Clear();
	delete INI;
}

// FUNCTION: 0x005D5DB0
int MPGameModeClass::vt_entry_28(void)
{
	return(-2);
}

// FUNCTION: 0x005D5DC0
int MPGameModeClass::vt_entry_2C(void)
{
	return(MustAlly ? 0 : -2);
}

// FUNCTION: 0x005D5DD0
int MPGameModeClass::vt_entry_30(void)
{
	return(AlliesAllowed ? 3 : -2);
}

// FUNCTION: 0x005D5DE0
bool MPGameModeClass::CanAllyWith(int index)
{
	if (MustAlly && index == -2) {
		return(false);
	}
	return(index >= 0 && index <= 3 ? 1 : 0);
}

// FUNCTION: 0x005D5E10
MPGameModeClass * First_Game_Mode(void)
{
	if (Game_Modes().Count() == 0) {
		return(NULL);
	}
	return(Game_Modes()[0]);
}

// FUNCTION: 0x005D5ED0
int Game_Mode_Total(void)
{
	return(Game_Mode_Count());
}

// FUNCTION: 0x005D5F30
MPGameModeClass * __fastcall MPGameModeClass::Get(int index)
{
	for (int i = 0; i < Game_Mode_Count(); i++) {
		MPGameModeClass * mode = Game_Modes()[i];
		if (mode->MPModeIndex == index) {
			return(mode);
		}
	}
	return(NULL);
}
