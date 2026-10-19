// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).
#pragma once

#include "addon.h"
#include "coord.h"
#include "ftimer.h"
#include "random.h"
#include "special.h"
#include "stimer.h"
#include "timer.h"
#include "typelist.h"
#include "types.h"

#include "campaign.hh"
#include "diff.hh"
#include "house.hh"
#include "side.hh"
#include "theater.hh"
#include "theme.hh"
#include "vq.hh"
#include "waypoint.hh"

#include <cstdlib>

#define SCEN_LOCAL_COUNT 100
#define SCEN_GLOBAL_COUNT 50

class CRCEngine;
class INIClass;
class TechnoTypeClass;
class AbstractClass;
class CellClass;
class ObjectClass;

class MissionTimerString {
	public:
		MissionTimerString(void) { Buffer[0] = '\0'; }
		MissionTimerString(NoInitClass const &) {}
		MissionTimerString(char const * text)
		{
			if (!text) {
				Buffer[0] = '\0';
			} else {
				strncpy(Buffer, text, sizeof(Buffer) - 1);
				Buffer[sizeof(Buffer) - 1] = '\0';
			}
		}
		MissionTimerString & operator=(MissionTimerString const & that)
		{
			if (&that != this) {
				memcpy(Buffer, that.Buffer, sizeof(Buffer));
			}
			return *this;
		}
		operator char const *(void) const { return Buffer; }
		char const & operator[](int index) const { return Buffer[index]; }
	private:
		char Buffer[32];
};

class ScenarioClass {
	public:

		ScenarioClass(void);
		ScenarioClass(NoInitClass const &);
		void Set_Scenario_Name(char const * name);
		void Reset(void);

		int Get_Unique_ID(void);

		bool Fetch_Global_Value(int global, bool & value);
		bool Fetch_Global_Value(char const * variable_name, bool & value);
		bool Set_Global_To(int global, bool value);
		bool Set_Global_To(char const * variable_name, bool value);
		int Global_From_Name(char const * variable_name);
		bool Read_Global_INI(CCINIClass const & ini);

		int Used_Local_Count(void);
		bool Fetch_Local_Value(int index, bool & value);
		bool Fetch_Local_Value(char const * variable_name, bool & value);
		bool Set_Local_To(int index, bool value);
		bool Set_Local_To(char const * variable_name, bool value);
		int First_Unused_Local(void) const;
		int Local_From_Name(char const * variable_name);
		bool Read_Local_INI(CCINIClass const & ini);
		bool Write_Local_INI(CCINIClass & ini) const;

		bool Read_INI(CCINIClass const & ini);
		bool Write_INI(CCINIClass & ini, bool mplayer=false) const;

		void Save(IStream * stream) const;
		void Load(IStream * stream);

		void Compute_CRC(CRCEngine & crc) const;

		Cell Get_Waypoint_Cell(WAYPOINT waypoint) const;
		Coord Get_Waypoint_Coord(WAYPOINT waypoint) const;
		CellClass * Get_Waypoint_CellClass(WAYPOINT waypoint) const;
		AbstractClass * Get_Waypoint_Target(WAYPOINT waypoint) const;

		char const * First_Unused_Waypoint_Name(void) const;

		void Read_Waypoints(CCINIClass const & ini);
		void Write_Waypoints(CCINIClass & ini) const;

		bool Is_Valid_Waypoint(WAYPOINT waypoint) const;
		void Set_Waypoint(WAYPOINT waypoint, Cell cell);
		void Clear_Waypoint(WAYPOINT waypoint);
		void Clear_All_Waypoints(void);

		void FUN_0068C050(void);

	public:

		SpecialClass Special;

		char NextScenarioName[_MAX_PATH];
		char AltNextScenarioName[_MAX_PATH];

		WAYPOINT Home;
		WAYPOINT AltHome;

		int UniqueID;

		Random2Class RandomNumber;

		DiffType Difficulty;
		DiffType CDifficulty;

		mutable TTimerClass<SystemTimerClass> ElapsedTimer;
		CDTimerClass<SystemTimerClass> PauseTimer;
		int unknown_62C;
		bool IsGamePaused;

		Cell Waypoint[WAYPT_COUNT];

		int StartX;
		int StartY;
		int Width;
		int Height;
		int NumberStartingPoints;
		Point2D StartingPoints[8];
		int HouseIndices[16];
		Cell HouseHomeCells[8];
		bool TeamsPresent;
		int NumCoopHumanStartSpots;

		CDTimerClass<FrameTimerClass> MissionTimer;
		wchar_t const * MissionTimerTextCSF;
		MissionTimerString MissionTimerText;

		CDTimerClass<FrameTimerClass> ShroudTimer;

		CDTimerClass<FrameTimerClass> FogTimer;

		CDTimerClass<FrameTimerClass> IceGrowthTimer;

		CDTimerClass<FrameTimerClass> VeinGrowthTimer;

		CDTimerClass<FrameTimerClass> AmbientChangeTimer;

		int Scenario;

		TheaterType Theater;

		char ScenarioName[_MAX_PATH];

		wchar_t Description[45];
		char UIName[32];
		wchar_t UINameLoaded[45];

		VQType IntroMovie;

		VQType BriefMovie;

		VQType WinMovie;

		VQType LoseMovie;

		VQType ActionMovie;

		VQType PostScoreMovie;

		VQType PreMapSelectMovie;

		wchar_t BriefingText[1024];
		char BriefingCSF[32];

		ThemeType TransitTheme;

		HousesType PlayerHouse;

		double CarryOverPercent;

		int CarryOverCap;

		int Percent;

		struct ScenarioFlagType {

			char VariableName[40];
			bool Value;
		};

		ScenarioFlagType GlobalFlags[SCEN_GLOBAL_COUNT];

		ScenarioFlagType LocalFlags[SCEN_LOCAL_COUNT];

		Cell Views[4];

		int BridgeCount;

		bool IsFreeRadar;

		bool IsTrainCargo;

		bool IsTibGrowth;
		bool IsVeinGrowth;
		bool IsIceGrowth;

		bool IsBridgeChanged;

		bool IsGlobalChanged;

		bool IsAmbientLightChanged;

		bool IsEndOfGame;

		bool IsInheritTimer;

		bool IsSkipScore;

		bool IsOneTimeOnly;

		bool IsNoMapSel;

		bool IsTruckCrate;

		bool IsMoneyTiberium;

		bool IsTiberiumDeathToVisceroid;

		bool IsIgnoreGlobalAITriggers;
		bool unknown_bool_34B5;
		bool unknown_bool_34B6;
		bool unknown_bool_34B7;

		int PlayerSideIndex;

		bool IsMultiplayerOnly;

		bool IsRandom;

		bool IsCrateBeenPickedUp;

		CDTimerClass<FrameTimerClass> FadeTimer;

		CampaignType Campaign;

		int StartingDropships;

		TypeList<TechnoTypeClass *> AllowableUnits;
		TypeList<int> AllowableUnitMaximums;
		TypeList<int> AllowableUnitCounts;

		int AmbientLight;

		int CurrentAmbientLight;

		int DesiredAmbientLight;

		struct LightingStruct {
			int RedTint;
			int GreenTint;
			int BlueTint;
			int GroundLight;
			int LevelLight;
		};

		LightingStruct NormalLighting;

		int IonAmbientLight;
		LightingStruct IonLighting;

		int NukeAmbient;
		LightingStruct NukeLighting;
		int NukeAmbientChangeRate;

		int DominatorAmbient;
		LightingStruct DominatorLighting;
		int DominatorAmbientChangeRate;

		bool IsReadingScenario;

		int InitTime;

		unsigned short Stage;

		bool IsInputLocked;
		bool unknown_35A3;

		int ParTimeEasy;
		int ParTimeMedium;
		int ParTimeDifficult;
		char UnderParTitle[31];
		char UnderParMessage[31];
		char OverParTitle[31];
		char OverParMessage[31];
		char LSLoadMessage[31];
		char LSBrief[31];
		int LS640BriefLocX;
		int LS640BriefLocY;
		int LS800BriefLocX;
		int LS800BriefLocY;
		char LS640BkgdName[64];
		char LS800BkgdName[64];
		char LS800BkgdPal[64];
};

void Write_Scenario_INI(char const * root, bool mplayer=false);
bool Read_Scenario_INI(char const * root, bool fresh=true);
bool Read_Scenario_INI(CCINIClass const & ini, bool is_mapgen=false);
int Scan_Place_Object(ObjectClass * obj, Cell const & cell);
void Assign_Houses(void);

void Post_Load_Game(void);
bool End_Game(void);
bool Read_Scenario(char const * root);
bool Start_Scenario(char const * name, bool briefing, CampaignType campaign);
HousesType Select_House(void);
void Clear_Scenario(void);
void Do_Lose(void);
void Do_Win(void);
void Do_Restart(void);
void Do_Abort(void);
void Fill_In_Data(void);
bool Restate_Mission(char const * name, int button1, int button2);

void Lock_Scenario_Input(void);
void Unlock_Scenario_Input(void);

void Pause_Scenario(void);
void Resume_Scenario(void);

int Adjust_To_CPU_Timing(int time);
void Toggle_Display_Mode(bool ingame);

extern ScenarioClass * Scen;
extern unsigned int ScenarioCRC;
