// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "tmission.h"
#include "mission.h"
#include "quarry.h"
#include "teamtype.h"
#include "need.hh"
#include "target.hh"
#include "tmission.hh"
#include "unload.hh"
#include <cstdio>

// SYNTHETIC: GAMEMD 0x00723A90 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00723AC0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00723AE0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00723B00 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00723B20 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00723B40 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00723B60 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00723BA0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00723BD0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00723C00 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00723C30 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00723C40 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00723C60 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00723C80 SYMBOL
// _$E55

// FUNCTION: GAMEMD 0x00723CA0
void TeamMissionClass::Fill_In(char const * entry)
{
	if (entry != NULL) {
		TeamMissionType tmission;
		int value;

		sscanf(entry, "%d,%d", &tmission, &value);

		Mission = tmission;
		Data.Value = value;
	}
}

// FUNCTION: GAMEMD 0x00723CE0
int TeamMissionClass::Build_INI_Entry(char * ptr) const
{
	if (ptr != NULL) {
		sprintf(ptr, "%d,%d", Mission, Data.Value);
		return(strlen(ptr));
	}

	return(0);
}

// FUNCTION: GAMEMD 0x00723D20
NeedType TeamMission_Needs(TeamMissionType tmtype)
{
	switch (tmtype) {
		case TMISSION_ATTACK:
		case 57:
			return(NEED_QUARRY);

		case TMISSION_MOVECELL:
			return(NEED_HEX_NUMBER);

		case TMISSION_CENTER_VIEWPOINT:
			return(NEED_VELOCITY);

		case TMISSION_SET_GLOBAL:
		case TMISSION_CLEAR_GLOBAL:
			return(NEED_GLOBAL);

		case TMISSION_SET_LOCAL:
		case TMISSION_CLEAR_LOCAL:
			return(NEED_LOCAL);

		case TMISSION_GUARD:
		case TMISSION_LOOP:
		case TMISSION_IDLE_ANIM:
		case TMISSION_ION_STORM_START:
		case TMISSION_FORCE_FACING:
		case TMISSION_FLASH:
			return(NEED_NUMBER);

		case TMISSION_PATROL:
		case TMISSION_MOVE:
		case TMISSION_ATT_WAYPT:
		case TMISSION_SPY:
		case 59:
			return(NEED_WAYPOINT);

		case TMISSION_CHANGE_HOUSE:
			return(NEED_HOUSE);

		case TMISSION_DO:
			return(NEED_MISSION);

		case TMISSION_SCRIPT:
			return(NEED_SCRIPT);

		case TMISSION_TEAMCHANGE:
			return(NEED_TEAM);

		case TMISSION_PLAY_SPEECH:
			return(NEED_SPEECH);

		case TMISSION_PLAY_SOUND:
			return(NEED_SOUND);

		case TMISSION_PLAY_MOVIE:
			return(NEED_MOVIE);

		case TMISSION_PLAY_MUSIC:
			return(NEED_THEME);

		case TMISSION_ATTACK_BUILDING_WITH_PROPERTY:
		case TMISSION_MOVETO_BUILDING_WITH_PROPERTY:
		case 56:
		case 58:
			return(NEED_BUILDING_ATTACK);

		case TMISSION_UNLOAD:
			return(NEED_SPLIT);

		case TMISSION_PLAY_ANIM:
			return(NEED_ANIM);

		case TMISSION_TALK_BUBBLE:
			return(NEED_TALK_BUBBLE);

		default:
			break;
	}
	return(NEED_NONE);
}
