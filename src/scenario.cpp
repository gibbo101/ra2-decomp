// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "scenario.h"
#include "_bench.h"
#include "_keyboar.h"
#include "_logic.h"
#include "_map.h"
#include "_palette.h"
#include "_rect.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tactica.h"
#include "_timer.h"
#include "_tooltip.h"
#include "_wsproto.h"
#include "addon.h"
#include "aircraft.h"
#include "aitrig.h"
#include "anim.h"
#include "astar.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "campaign.h"
#include "ccrand.h"
#include "cctooltip.h"
#include "cd.h"
#include "cell.h"
#include "conquer.h"
#include "convert.h"
#include "crc.h"
#include "data.h"
#include "dbgprint.h"
#include "egos.h"
#include "empulse.h"
#include "enviro.h"
#include "getcpu.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "incdec.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "inline.h"
#include "ion.h"
#include "isotype.h"
#include "language\language.h"
#include "light.h"
#include "logic.h"
#include "mainopt.h"
#include "mapgen.h"
#include "mapsel.h"
#include "misc.h"
#include "mouse.h"
#include "movie.h"
#include "mpu.h"
#include "msgbox.h"
#include "netdlg.h"
#include "newmenu.h"
#include "overlay.h"
#include "overtype.h"
#include "partsys.h"
#include "preview.h"
#include "progress.h"
#include "psystype.h"
#include "queue.h"
#include "revent.h"
#include "rules.h"
#include "scheme.h"
#include "score.h"
#include "script.h"
#include "session.h"
#include "smudge.h"
#include "surface.h"
#include "swizzle.h"
#include "tactical.h"
#include "tag.h"
#include "tagtype.h"
#include "taskforc.h"
#include "teamtype.h"
#include "terrain.h"
#include "theme.h"
#include "tiberium.h"
#include "tracker.h"
#include "trigger.h"
#include "trigtype.h"
#include "tube.h"
#include "unit.h"
#include "unittype.h"
#include "vein.h"
#include "vox.h"
#include "wave.h"
#include "waypoint.h"
#include "wincomm.h"
#include "wonline.h"
#include "wsproto.h"
#include "bench.hh"
#include <stddef.h>
#include "stringtable.h"

bool Read_Scenario(char const * fname);
int __fastcall FUN_004790A0(int timeout);
void __fastcall FUN_005BF260(int movie, int theme, bool a, bool b, bool c);
void __fastcall FUN_004B6C30(void);
void __fastcall FUN_0072DEF0(void);
void __fastcall FUN_007781A0(void);
void __fastcall FUN_0078ABF0(bool a, wchar_t const * text);
extern bool ScenarioStarted;
extern void * DAT_0087E758;
extern int DAT_0083D834;
extern bool DAT_00B054CC;
extern int DAT_00B07784;
extern bool UserInputLocked;
void __fastcall FUN_00406F00(void);
void __fastcall FUN_00406F40(void);
int __fastcall FUN_004072B0(void * p);
void __fastcall FUN_00407150(void * p, int value);
void __fastcall FUN_005BF500(bool pause);
class InputManagerClass {
	public:
		void DoSomething(void);
		static InputManagerClass * Instance;
};

// FUNCTION: 0x00683280
// FUNCTION: 0x006832B0
CDTimerClass<SystemTimerClass> ScenUnusedTimer;

// FUNCTION: 0x006832C0
ScenarioClass::ScenarioClass(void)
{
	Reset();
	IsReadingScenario = false;
	IsRandom = false;
	Special.Init();

	Scenario = 1;
	Campaign = CAMPAIGN_NONE;
	NextScenarioName[0] = '\0';
	AltNextScenarioName[0] = '\0';

	int i;
	for (i = 0; i < ARRAY_SIZE(Waypoint); i++) {
		Waypoint[i] = CELL_NONE;
	}
	wcscpy(Description, L"");
	wcscpy(UINameLoaded, L"");
	strcpy(ScenarioName, "");
	strcpy(BriefingCSF, "");
	strcpy(UIName, "");
	wcscpy(BriefingText, L"");
	for (i = 0; i < ARRAY_SIZE(GlobalFlags); i++) {
		GlobalFlags[i].VariableName[0] = '\0';
		Set_Global_To(i, false);
	}
	for (i = 0; i < ARRAY_SIZE(LocalFlags); i++) {
		LocalFlags[i].VariableName[0] = '\0';
		Set_Local_To(i, false);
	}
	memset(Views, '\0', sizeof(Views));
}

// FUNCTION: 0x00683560
ScenarioClass::ScenarioClass(NoInitClass const & x) :
	ElapsedTimer(x),
	MissionTimer(x),
	MissionTimerText(x),
	ShroudTimer(x),
	FogTimer(x),
	IceGrowthTimer(x),
	VeinGrowthTimer(x),
	AmbientChangeTimer(x),
	FadeTimer(x)
{
	if (MissionTimerText[0]) {
		MissionTimerTextCSF = StringTable::LoadString(MissionTimerText, NULL, "D:\\ra2mdpost\\Scenario.CPP", 344);
	} else {
		MissionTimerTextCSF = L"";
	}
}

// FUNCTION: 0x00683610
void ScenarioClass::Reset(void)
{
	Home = WAYPT_HOME;
	AltHome = WAYPT_HOME;
	UniqueID = 1000 * 1000;
	Difficulty = DIFF_NORMAL;
	CDifficulty = DIFF_NORMAL;
	ElapsedTimer = 0;
	ElapsedTimer.Stop();
	PauseTimer = 0;
	PauseTimer.Stop();
	unknown_62C = 0;
	IsGamePaused = false;
	MissionTimer = 0;
	MissionTimer.Stop();
	MissionTimerTextCSF = L"";
	MissionTimerText = "";
	ShroudTimer = 0;
	FogTimer = 0;
	IceGrowthTimer = 0;
	VeinGrowthTimer = 0;
	AmbientChangeTimer = 0;
	Theater = THEATER_NONE;
	IntroMovie = VQ_NONE;
	BriefMovie = VQ_NONE;
	WinMovie = VQ_NONE;
	LoseMovie = VQ_NONE;
	ActionMovie = VQ_NONE;
	PostScoreMovie = VQ_NONE;
	PreMapSelectMovie = VQ_NONE;
	TransitTheme = THEME_NONE;
	PlayerHouse = HOUSE_FIRST;
	CarryOverPercent = 0;
	CarryOverCap = 0;
	Percent = 0;
	BridgeCount = 0;
	IsFreeRadar = false;
	IsTrainCargo = false;
	IsTibGrowth = true;
	IsVeinGrowth = true;
	IsIceGrowth = true;
	IsBridgeChanged = false;
	IsGlobalChanged = false;
	IsAmbientLightChanged = false;
	IsEndOfGame = false;
	IsInheritTimer = false;
	IsSkipScore = false;
	IsOneTimeOnly = false;
	IsNoMapSel = false;
	IsTruckCrate = false;
	IsMoneyTiberium = false;
	IsIgnoreGlobalAITriggers = false;
	PlayerSideIndex = 0;
	IsMultiplayerOnly = false;
	IsCrateBeenPickedUp = false;
	FadeTimer = 0;
	StartingDropships = 0;
	AmbientLight = 100;
	CurrentAmbientLight = 100;
	DesiredAmbientLight = 100;
	NormalLighting.RedTint = 100;
	NormalLighting.GreenTint = 100;
	NormalLighting.BlueTint = 100;
	NormalLighting.GroundLight = 50;
	NormalLighting.LevelLight = 8;
	IonAmbientLight = 87;
	IonLighting.RedTint = 30;
	IonLighting.GreenTint = 40;
	IonLighting.BlueTint = 75;
	IonLighting.GroundLight = 0;
	IonLighting.LevelLight = 0;
	NukeAmbient = 200;
	NukeLighting.RedTint = 175;
	NukeLighting.GreenTint = 150;
	NukeLighting.BlueTint = 125;
	NukeLighting.GroundLight = 100;
	NukeLighting.LevelLight = 100;
	NukeAmbientChangeRate = 1;
	DominatorAmbient = 150;
	DominatorLighting.RedTint = 85;
	DominatorLighting.GreenTint = 20;
	DominatorLighting.BlueTint = 30;
	DominatorLighting.GroundLight = 0;
	DominatorLighting.LevelLight = 0;
	DominatorAmbientChangeRate = 1;
	InitTime = 10000;
	Stage = 0;
	IsInputLocked = false;
	IsTiberiumDeathToVisceroid = true;
	ParTimeEasy = 3600;
	ParTimeMedium = 3600;
	ParTimeDifficult = 3600;
	UnderParTitle[0] = '\0';
	UnderParMessage[0] = '\0';
	OverParTitle[0] = '\0';
	OverParMessage[0] = '\0';
	LSLoadMessage[0] = '\0';
	LSBrief[0] = '\0';
	LS640BriefLocX = 0;
	LS640BriefLocY = 0;
	LS800BriefLocX = 0;
	LS800BriefLocY = 0;
	LS640BkgdName[0] = '\0';
	LS800BkgdName[0] = '\0';
	LS800BkgdPal[0] = '\0';
	TeamsPresent = false;
	NumCoopHumanStartSpots = 4;

	AllowableUnits.Clear();
	AllowableUnitMaximums.Clear();
	AllowableUnitCounts.Clear();

	if (Session.Type == GAME_SKIRMISH) {
		wcscpy(Scen->Description, StringTable::LoadString("GUI:SkirmishGame", NULL, "D:\\ra2mdpost\\Scenario.CPP", 522));
	}
}

// FUNCTION: 0x00683AB0
bool Start_Scenario(char const * name, bool briefing, CampaignType campaign)
{
	if ((name == NULL || strlen(name) == 0) && campaign != CAMPAIGN_NONE) {
		if (Debug_ForceScenario == true) {
			name = Debug_ScenarioName;
		} else {
			name = Campaigns[campaign]->ScenarioName;
		}
	}

	Scen->Campaign = campaign;

	DebugString("\n----- Starting scnenario: %s -----\n", name);
	DebugString("Player Count: %d\n", Session.Players.Count());

	strcpy(Scen->ScenarioName, name);
	strupr(Scen->ScenarioName);

	CD::Set_Required_Disk(DISK_ANY);

	if (Session.Type == GAME_NORMAL) {
		if (Scen->Campaign != CAMPAIGN_NONE) {
			CD::Set_Required_Disk((DiskID)Campaigns[Scen->Campaign]->CDNumber);
		}
	} else {
		if (Session.Options.ScenarioIndex != -1) {
			MultiMission * multi = Session.Scenarios[Session.Options.ScenarioIndex];
			if (multi->Is_On_CD((DiskID)FUN_004790A0(60)) == false) {
				CD::Set_Required_Disk(multi->On_Which_CD());
			}
		}
	}

	Session.Suspended++;

	if (!CD::Force_Available()) {
		if (Session.Type == GAME_INTERNET) {
			FUN_0078ABF0(false, L"");
		}
		return(false);
	}
	Session.Suspended--;

	Hide_Mouse();

	{
		CCINIClass ini;
		CCFileClass file(name);
		if (ini.ReadCCFile(file, true, false)) {
			VQType movie = ini.Get_VQType("Basic", "Intro", VQ_NONE);
			if (movie == VQ_NONE) {
				movie = ini.Get_VQType("Basic", "Brief", movie);
			}
			if (movie != VQ_NONE) {
				Theme.Stop(false);
				FUN_005BF260(movie, -1, true, true, true);
			}
		}
	}

	DebugString("Reading scenario: %s\n", name);

	Theme.Play_Song(Theme.From_Name("LOADING"));

	if (!Read_Scenario(name)) {
		Show_Mouse();
		if (Session.Type == GAME_INTERNET) {
			FUN_0078ABF0(false, L"");
		}
		return(false);
	}

	char buffer[25];

	if (Scen->BriefMovie != VQ_NONE) {
		wsprintf(buffer, "%s.VQA", Movies[Scen->BriefMovie]);
	}

	if (Scen->StartingDropships > 0) {
		FUN_004B6C30();
	}

	if (briefing) {
		FUN_005BF260(Scen->ActionMovie, -1, true, true, true);
	}

	if (VisibleRect.Width != Options.ScreenWidth || VisibleRect.Height != Options.ScreenHeight) {
		DebugString("Toggle display mode to %d x %d\n", Options.ScreenWidth, Options.ScreenHeight);
		Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
	}

	FUN_0072DEF0();
	Options.Set();

	HiddenSurface->Fill(0);
	Update_Visible_Surface(true, HiddenSurface);

	Scen->ElapsedTimer.Start();

	if (Scen->TransitTheme != THEME_NONE) {
		Theme.Queue_Song(Scen->TransitTheme);
	} else {
		Theme.Stop(true);
	}

	FUN_007781A0();

	ScenarioStarted = true;
	TacticalActive = true;

	if (Session.Type == GAME_INTERNET) {
		FUN_0078ABF0(false, L"");
	}
	return(true);
}

// FUNCTION: 0x00683EB0
void Pause_Scenario(void)
{
	FUN_00406F00();
	DAT_0083D834 = FUN_004072B0(DAT_0087E758);
	FUN_00407150(DAT_0087E758, 0x4000);

	if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
		DebugString("Paused ElapsedTimer = %ld\n", Scen->ElapsedTimer.Value());
		Scen->ElapsedTimer.Stop();
	}

	FUN_005BF500(true);

	if (ToolTips != NULL) {
		ToolTips->Activate(false);
	}

	Map.Abort_Drag_Select();
	Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	Hide_Mouse();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	Map.Render();
	Show_Mouse();
}

// FUNCTION: 0x00683FB0
void Resume_Scenario(void)
{
	Map.Revert_Mouse_Shape();

	Scen->ElapsedTimer.Start();
	DebugString("Resume ElapsedTimer = %ld\n", Scen->ElapsedTimer.Value());

	if (ToolTips != NULL) {
		ToolTips->Activate(Options.ToolTips);
	}

	FUN_00407150(DAT_0087E758, DAT_0083D834);
	DAT_0083D834 = 0x4000;
	FUN_005BF500(false);
	FUN_00406F40();
}

// FUNCTION: 0x00684060
void __fastcall Hold_Scenario(int seconds)
{
	if (seconds) {
		Scen->PauseTimer = seconds * 60;
		if (Scen->IsGamePaused) {
			return;
		}
		Scen->IsGamePaused = true;
	}

	if (Scen->unknown_62C == 0) {
		FUN_00406F00();

		if (Session.Type == GAME_NORMAL || Session.Type == GAME_SKIRMISH) {
			Scen->ElapsedTimer.Stop();
		}

		bool locked = Scen->IsInputLocked;
		DAT_00B054CC = locked;
		if (!locked) {
			Map.Abort_Drag_Select();
			if (!Get_Mouse_State()) {
				Hide_Mouse();
			}
			Scen->IsInputLocked = true;
			UserInputLocked = true;
			InputManagerClass::Instance->DoSomething();
			DAT_00B07784 = 0;
		}
	}
	Scen->unknown_62C++;
}

// FUNCTION: 0x00684180
void Release_Scenario(void)
{
	if (Scen->unknown_62C == 0) {
		return;
	}
	Scen->unknown_62C--;
	if (Scen->unknown_62C != 0) {
		return;
	}

	if (!DAT_00B054CC) {
		if (Scen->IsInputLocked == true) {
			while (Get_Mouse_State()) {
				Show_Mouse();
			}
		}
		Scen->IsInputLocked = false;
		UserInputLocked = false;
		InputManagerClass::Instance->DoSomething();
		DAT_00B07784 = 0;
	}

	Scen->ElapsedTimer.Start();
	FUN_00406F40();
}

// FUNCTION: 0x00684240
void Lock_Scenario_Input(void)
{
	Map.Abort_Drag_Select();

	if (!Get_Mouse_State()) {
		Hide_Mouse();
	}

	Scen->IsInputLocked = true;
	UserInputLocked = true;
	InputManagerClass::Instance->DoSomething();
	DAT_00B07784 = 0;
}

// FUNCTION: 0x00684290
void Unlock_Scenario_Input(void)
{
	if (Scen->IsInputLocked == true) {
		while (Get_Mouse_State()) {
			Show_Mouse();
		}
	}

	Scen->IsInputLocked = false;
	UserInputLocked = false;
	InputManagerClass::Instance->DoSomething();
	DAT_00B07784 = 0;
}

// FUNCTION: 0x0068B8A0
int Adjust_To_CPU_Timing(int time)
{
	static const double P_SIX_TWEAK = .8;
	static const double TIME_SCALE = 200;

	int cpu_type;
	bool mmx;

	int speed = 0;

	Get_CPU_Type(cpu_type, mmx, NULL, 0);

	if (cpu_type > PROC_PENTIUM_PRO) {
		time = (int)(time * P_SIX_TWEAK);
	}

	double hiscale = ((__int64)2 << 31);

	unsigned int lo;
	unsigned int hi;
	lo = Get_CPU_Rate(hi);

	if (lo != 0 || hi != 0) {
		speed = (int)(((double)lo + ((double)hi * hiscale)) / (double)1000000);
	}

	time = (int)(time * TIME_SCALE / speed);
	return(time);
}

// FUNCTION: 0x0068BCB0
int ScenarioClass::Get_Unique_ID(void)
{
	UniqueID++;

	return(UniqueID);
}

bool ScenarioClass::Set_Global_To(int global, bool value)
{
	if ((unsigned)global < ARRAY_SIZE(Scen->GlobalFlags)) {
		bool previous = GlobalFlags[global].Value;
		if (previous != value) {
			GlobalFlags[global].Value = value;
			IsGlobalChanged = true;
			TagClass::All_Timer_Global_Reset(global);
		}
		return(previous);
	}
	return(false);
}

bool ScenarioClass::Set_Local_To(int index, bool value)
{
	if ((unsigned)index < ARRAY_SIZE(Scen->LocalFlags)) {
		bool previous = LocalFlags[index].Value;
		if (previous != value) {
			LocalFlags[index].Value = value;
			IsGlobalChanged = true;
			TagClass::All_Timer_Local_Reset(index);
		}
		return(previous);
	}
	return(false);
}
