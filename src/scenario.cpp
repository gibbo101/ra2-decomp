// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
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
#include "bomblist.h"
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
#include "dsurface.h"
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
#include "ipxmgr.h"
#include "ion.h"
#include "isotype.h"
#include "kamikaze.h"
#include "language\language.h"
#include "ldprgmgr.h"
#include "light.h"
#include "logic.h"
#include "mainopt.h"
#include "mapgen.h"
#include "mapsel.h"
#include "misc.h"
#include "mouse.h"
#include "movie.h"
#include "multiplayer.h"
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
#include "wspipx.h"
#include "wsproto.h"
#include "bench.hh"
#include <stddef.h>
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x00683070 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006830A0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006830C0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006830E0 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00683100 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00683120 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00683140 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00683180 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006831B0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006831E0 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00683210 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00683220 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00683240 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00683260 SYMBOL
// _$E49

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
		void FUN_0054F000(void);
		static InputManagerClass * Instance;
};

// FUNCTION: GAMEMD 0x00683280
// FUNCTION: GAMEMD 0x006832B0
CDTimerClass<SystemTimerClass> ScenUnusedTimer;

// FUNCTION: GAMEMD 0x006832C0
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

// FUNCTION: GAMEMD 0x00683560
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

// FUNCTION: GAMEMD 0x00683610
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

// FUNCTION: GAMEMD 0x00683AB0
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

// FUNCTION: GAMEMD 0x00683EB0
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

// FUNCTION: GAMEMD 0x00683FB0
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

// FUNCTION: GAMEMD 0x00684060
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

// FUNCTION: GAMEMD 0x00684180
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

// FUNCTION: GAMEMD 0x00684240
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

// FUNCTION: GAMEMD 0x00684290
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

// FUNCTION: GAMEMD 0x006842F0
void Toggle_Display_Mode(bool ingame)
{
	if (ingame) {
		if (VisibleRect.Width != Options.ScreenWidth || VisibleRect.Height != Options.ScreenHeight) {
			DebugString("Toggle display mode to %d x %d\n", Options.ScreenWidth, Options.ScreenHeight);
			Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
		}
	} else {
		if (VisibleRect.Width != Options.ShellWidth && VisibleRect.Height != Options.ShellHeight) {
			DebugString("Toggle display mode back to shell res\n");
			Change_Display_Mode(Options.ShellWidth, Options.ShellHeight);
		}
	}
}

void __fastcall FUN_0071DCA0(TheaterType theater);
void __fastcall FUN_00660C50(void);
void __fastcall FUN_00734210(void);
void __fastcall FUN_0053AB50(void);
void __fastcall FUN_0074D450(TheaterType theater);
void __fastcall FUN_006B5490(TheaterType theater);

namespace Game { extern bool IsActive; extern HWND hWnd; bool __cdecl CallBack(void); }
void __fastcall FUN_0055AF40(int value);
void __fastcall FUN_0055AF50(int value);

struct FileSystem { static char TEMPERAT_PAL[]; };
void __fastcall FUN_004A3C30(void * palette, int rate, bool (__cdecl * callback)(void));
bool Wait_For_Players_To_Load(void);
namespace Game { namespace Network { extern int Tournament; extern int WOLGameID; } }
struct Unknown_00B052D0 { bool FUN_00664610(int, int); };
extern Unknown_00B052D0 DAT_00B052D0;
int __fastcall FUN_0072B380(void);
void __fastcall Multiplayer_Last_Minute_Fixups(bool official);

// FUNCTION: GAMEMD 0x00684620
bool Read_Scenario(char const * fname)
{
	bool read_ok = true;
	char name[_MAX_PATH];

	strcpy(name, fname);

	Frame = 0;

	Scen->IsReadingScenario = true;
	ScenarioInit++;

	char * ext = name + strlen(name) - 4;

	if (stricmp(ext, ".SED") == 0) {
		Scen->IsRandom = true;
		DebugString("Scen->IsRandom = true\n");
	} else {
		Scen->IsRandom = false;
		DebugString("Scen->IsRandom = false\n");
	}

	LoadProgressManager * manager = NULL;

	if (!Debug_Map) {
		int players = 1;

		if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
			players = Session.Players.Count();
		}

		Progress.Initialize(100.0, players, NULL);

		wchar_t const * prog_msg = NULL;
		wchar_t prog_msg_buffer[129];

		if (Session.Type == GAME_INTERNET && Game::Network::Tournament) {
			swprintf(prog_msg_buffer, StringTable::LoadString("TXT_GAME_ID", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x4af), Game::Network::WOLGameID);
			prog_msg = prog_msg_buffer;
		}

		manager = LoadProgressManager::Create();
		manager->Compute_Rects();
		Progress.Set_Callback(manager);

		int side = 0;
		if (Session.Type == GAME_NORMAL) {
			if (Scen->Campaign != CAMPAIGN_NONE) {
				side = Campaigns[Scen->Campaign]->CDNumber;
			}
		} else {
			int house = Session.Players[0]->Player.House;
			if (house == -3) {
				house = 0;
			}
			side = HouseTypes[house]->Side;
		}
		Scen->PlayerSideIndex = side;
		Progress.Set_Side(side);

		LoadProgressManager::Create()->Load_Mixes();

		if (Session.Type == GAME_NORMAL) {
			Progress.Set_Graphic_Data("SPLDBR.SHP", NULL, FUN_0072B380());
		} else {
			Progress.Set_Graphic_Data("PROGBARM.SHP", NULL, 0);
		}

		bool single = (Session.Type == GAME_NORMAL);
		Progress.Set_Layout(manager->Text_Origin(), prog_msg, !single, !single);
		Progress.Set_Field78(manager->FUN_00552C90());

		if (PacketTransport != NULL && Session.Type == GAME_INTERNET && Session.Players.Count() > 1) {
			DebugString("Setting addresses for UDP broadcast\n");
			PacketTransport->Clear_Broadcast_Addresses();

			char address[32] = {"xxx.xxx.xxx.xxx"};
			unsigned char net[4];
			unsigned char node[6];

			for (int p = 1; p < Session.Players.Count(); p++) {
				Session.Players[p]->Address.Get_Address(net, node);
				sprintf(address, "%d.%d.%d.%d", node[0], node[1], node[2], node[3]);
				DebugString("Adding broadcast address %s\n", address);
				PacketTransport->Set_Broadcast_Address(address, Session.PlayerPorts[p]);
			}
		}
	}

	if (Scen->IsRandom) {
		read_ok = RandomMapGen.SeedData.Load(name);

		if (read_ok) {
			RandomMapGen.Generate_Random_Map(false, NULL);
			Multiplayer_Last_Minute_Fixups(true);
		}
		strcpy(Scen->ScenarioName, name);
	} else {
		read_ok = Read_Scenario_INI(name, true);
	}

	if (!Rule->Shroud) {
		Map.Reveal_The_Map(NULL);
	}

	if (!read_ok) {
		DebugString("Error - Unable to read scenario: %s\n", name);
		FUN_004A3C30(&FileSystem::TEMPERAT_PAL, 7, Game::CallBack);
		WWMessageBox().Process(StringTable::LoadString("TXT_UNABLE_READ_SCENARIO", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x50d), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x50d), NULL, NULL, false);

		ScenarioInit--;
		Scen->IsReadingScenario = false;
		if (manager != NULL) {
			Progress.Set_Callback(NULL);
			LoadProgressManager::Destroy();
		}
		return(false);
	}

	Session.Messages.Init(
		DSurface::ViewBounds.X + 3, DSurface::ViewBounds.Y,
		6,
		98,
		7 * 2,
		-1, -1,
		0,
		20,
		98,
		DSurface::ViewBounds.Width - 6);

	Fill_In_Data();

	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		Session.Update_Progress(99);
		if (Session.Type == GAME_INTERNET && Session.ViaPacketRouter) {
			DAT_00B052D0.FUN_00664610(15000, true);
		}
	}

	Session.Update_Progress(100 + (Scen->IsRandom ? 100 : 0));

	Session.SawGameCompletion = false;
	Session.OutOfSync = false;

	bool all_loaded = Wait_For_Players_To_Load();

	if (Session.Type == GAME_INTERNET) {
		Session.Init_Fixed_Alliances();
	}

	ScenarioInit--;

	if (!Debug_Map) {
		if (all_loaded) {
			Game::CallBack();
			double progress = Progress.Get_Current_Progress();

			while (progress < 1.0) {
				for (int i = 0; i < Session.Players.Count(); i++) {
					Progress.Set_Progress_Percent(i, 100.0);
				}
				progress = Progress.Get_Current_Progress();
			}
		}
	}

	Scen->IsReadingScenario = false;

	Progress.End();

	if (manager != NULL) {
		Progress.Set_Callback(NULL);
		manager->FUN_00554370();
	}

	for (int house = Houses.Count() - 1; house >= 0; house--) {
		Houses[house]->FUN_0050D610();
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00684C30
void Fill_In_Data(void)
{
	ScenarioInit++;
	int index;
	for (index = 0; index < Buildings.Count(); index++) {
		Buildings[index]->Update_Buildables();
	}

	Map.Flag_To_Redraw(GS_REDRAW_ALL);

	if (!Debug_Map) {
		Map.Activate(1);

		if (Session.Type == GAME_NORMAL) {
			bool flag = Environment.Globals[0];
			Cell view = Scen->Waypoint[flag ? Scen->AltHome : Scen->Home];

			Scen->Views[0] = Scen->Views[1] = Scen->Views[2] = Scen->Views[3] = view;
			Coord coord = Coord(view);
			coord.Z = Map.Get_Height_GL(coord);
			Map.Set_Tactical_Position(coord);
		}
	}

	for (index = 0; index < TagTypes.Count(); index++) {
		TagTypeClass * tp = TagTypes[index];

		if (tp->Attaches_To() & 4) {
			MapTags.Add(Find_Or_Make(tp));
		}

		if (tp->Attaches_To() & 0x10) {
			LogicTags.Add(Find_Or_Make(tp));
		}

		if (tp->Attaches_To() & 8) {
			TagClass * tt = Find_Or_Make(tp);
			HouseClass * house = HouseClass::FindByCountryIndex(tt->Class->FirstTrigger->House->House);
			if (house != NULL) {
				house->HouseTags.Add(tt);
			}
		}
	}

	ScenarioInit--;

	for (index = 0; index < TagTypes.Count(); index++) {
		TagTypeClass * tp = TagTypes[index];
		if (tp->Is_Allow_Win()) {
			HouseClass * owner = HouseClass::FindByCountryIndex(tp->FirstTrigger->House->HeapID);
			owner->Blockage++;
		}
	}

	if (Scen->IsMoneyTiberium) {
		for (int house = 0; house < Houses.Count(); house++) {
			HouseClass * hptr = Houses[house];

			if (hptr != NULL) {
				int money = hptr->Available_Money();
				TiberiumClass const * tib = Tiberiums[0];

				while (money > tib->CreditValue && hptr->Available_Storage() > 0) {
					hptr->Give_Tiberium(1.0f, 0);
					money -= tib->CreditValue;
				}
			}
		}
	}

	Map.FUN_004ADCD0(true, false);
	WaveClass::Init_Statics();

	Map.Reset_Iterator();
	CellClass * cellptr = Map.Iterate();

	while (cellptr) {
		cellptr->Recalc_Attributes();
		cellptr = Map.Iterate();
	}

	Map.Compute_Zone_Connections();
	Map.Update_Pathfinding_1();
	Map.Reset_All_Subzones();
	Map.Set_WasUnderBridge_Flags();

	if (GasSystem == NULL) {
		GasSystem = new ParticleSystemClass(ParticleSystemTypes[ParticleSystemTypeClass::From_Name("GasCloudSys")], Coord(Cell(10, 10)));
	}

	if (Rule->OreTwinkle != NULL) {
		Map.Reset_Iterator();
		CellClass * cell = Map.Iterate();
		while (cell != NULL) {
			if (cell->Tiberium_Value() != 0) {
				int chance = Rule->OreTwinkleChance - 1;
				if (Scen->RandomNumber(0, chance) == 0) {
					new AnimClass(Rule->OreTwinkle, cell->Center_Coord());
				}
			}
			cell = Map.Iterate();
		}
	}

	FUN_0055AF40(Rule->DetailMinFrameRateNormal);
	FUN_0055AF50(Rule->DetailBufferZoneWidth);
}

// FUNCTION: GAMEMD 0x00685120
void Post_Load_Game(void)
{
	BuildingTypeClass::Post_Load_Game();
	TiberiumClass::Post_Load_Game();
	FUN_0074D450(Scen->Theater);
	FUN_0071DCA0(Scen->Theater);
	FUN_006B5490(Scen->Theater);
	Map.Post_Load_Radar_Fixup();

	TechnoClass::ActionLineTimer.Stop();
	TechnoClass::ActionLineTimer.Start();

	Search.Update_Map_Dimensions(Map.PlayRect);

	FUN_0053AB50();
	AnimClass::Post_Load_Game();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	InvalidateRect(Game::hWnd, NULL, FALSE);
	FUN_00734210();
	((TechnoClass *)NULL)->ClearSidebarTabObject();
	FUN_00660C50();
}

void __fastcall FUN_00750FA0(void);
void __fastcall FUN_00539760(void);
void __fastcall FUN_006370B0(void);
void __fastcall FUN_0052CB90(void);
void __fastcall FUN_0053A090(void);

// FUNCTION: GAMEMD 0x006851F0
void Clear_Scenario(void)
{
	int index;

	Scen->UniqueID = 1000 * 1000;
	Scen->RandomNumber.unknown_00 = true;
	DAT_0083D834 = 0x4000;
	FUN_00407150(DAT_0087E758, 0x4000);
	FUN_00750FA0();

	PlayerPtr = NULL;

	Scen->Reset();

	FUN_0053A090();

	for (index = AbstractTypes.Count() - 1; index >= 0; index--) {
		AbstractTypeClass * abstype = AbstractTypes[index];
		if (abstype->What_Am_I() == RTTI_ISOTILETYPE) {
			AbstractTypes.Delete_Index(index);
		}
	}
	Delete_All_Objects();
	for (index = 0; index < IsometricTileTypes.Count(); index++) {
		AbstractTypes.Add(IsometricTileTypes[index]);
	}

	Map.Free_Cells();

	Map.ZoneConnections.Clear();

	TagClass::Init();

	for (index = 0; index < ARRAY_SIZE(Scen->GlobalFlags); index++) {
		Scen->Set_Global_To(index, false);
	}

	delete TacticalMap;
	TacticalMap = new Tactical;
	TacticalMap->Reset_Dirty_Rectangles();

	LightSourceClass::Recalc = false;
	while (Objects.Count()) {
		if (Objects[0]->Fetch_RTTI() == RTTI_BULLET) {
			Objects[0]->Release();
		} else {
			delete Objects[0];
		}
	}

	LightSourceClass::Recalc = true;

	while (TagTypes.Count()) {
		delete TagTypes[0];
	}

	MapTags.Clear();
	LogicTags.Clear();

	Scen->ParTimeEasy = 3600;
	Scen->ParTimeMedium = Scen->ParTimeEasy;
	Scen->ParTimeDifficult = Scen->ParTimeEasy;
	Scen->UnderParTitle[0] = '\0';
	Scen->UnderParMessage[0] = '\0';
	Scen->OverParTitle[0] = '\0';
	Scen->OverParMessage[0] = '\0';

	LightSourceClass::Reset();
	FUN_00539760();
	EMPulseClass::Reset();
	VeinholeMonsterClass::Reset();
	TiberiumClass::Deinit_Tiberium_Spread_System();
	TiberiumClass::Deinit_Tiberium_Growth_System();
	BombListClass::Instance.FUN_00439110();
	KamikazeInstance.Clear();
	AircraftTracker.Clear();
	FUN_006370B0();

	Map.PlayRect = Rect(0, 0, 0, 0);
	DebugString("Map.Init_Clear()\n");
	Map.Init_Clear();
	DebugString("Logic.Init()\n");
	Logic.Init();

	DebugString("CurrentObjects.Clear()\n");
	CurrentObject.Clear();

	DebugString("Scen->Clear_All_Waypoints()\n");
	Scen->Clear_All_Waypoints();

	DebugString("Init_Campaigns()\n");
	FUN_0052CB90();

	if (GasSystem) {
		GasSystem->Clear_System();
		delete GasSystem;
		GasSystem = NULL;
	}

	Scen->RandomNumber.unknown_00 = false;
	FUN_00734210();
	((TechnoClass *)NULL)->ClearSidebarTabObject();
	FUN_00660C50();
	Scen->UniqueID = 1000 * 1000;
}

void __fastcall FUN_0072DDB0(void);
void __fastcall FUN_005BF450(void);
void __fastcall FUN_00406E80(void);
void __fastcall FUN_007529A0(bool a);
void __fastcall FUN_00752CA0(void);
void __fastcall FUN_00407070(void * p);
void __fastcall FUN_004070B0(void * p);
bool __fastcall FUN_007529E0(void);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
bool Net2Init_Network(void);
namespace Game { namespace Network { extern int PreCalcFrameRate; } }
namespace CopyProtection { bool CheckProtectedData(void); }
void FUN_0055CF10(void);
class MPScoreScreen { public: MPScoreScreen(void); void Present(void); };
class STLScoreScreen { public: STLScoreScreen(void); void Present(void); };

// FUNCTION: GAMEMD 0x00685670
void Do_Win(void)
{
	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		if (!Session.Play) {
			Wait_For_End_Of_Queue();
			FUN_0055CF10();
		}
	}

	ScenarioStarted = false;
	TacticalActive = false;

	Scen->ElapsedTimer.Stop();
	FUN_005BF450();

	MouseCursor->Erase_Mouse(HiddenSurface);
	MouseCursor->Release_Mouse();
	Map.Set_Default_Mouse(MOUSE_NORMAL, false);
	MouseCursor->Hide_Mouse();

	FUN_00407070(DAT_0087E758);
	FUN_004070B0(DAT_0087E758);

	int timeout = TickCount + 300;

	while (FUN_007529E0() && timeout > TickCount) {
		Game::CallBack();
	}

	FUN_007529A0(true);
	FUN_00752CA0();
	FUN_00406E80();
	FUN_00750FA0();
	FUN_00407150(DAT_0087E758, 0x4000);
	Toggle_Display_Mode(false);

	if (Session.Type != GAME_NORMAL) {
		bool finished = false;

		if (!Session.Play) {
			Session.GamesPlayed++;

			if (Session.Type == GAME_INTERNET) {
				FUN_0078ABF0(true, StringTable::LoadString("TXT_AWAY_SCORESCREEN", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x7e8));
			}

			if (Session.MPGameMode != NULL) {
				if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode->vt_entry_08()) {
					FUN_0052FEC0(NULL, true);
				} else {
					Session.GameName[0] = 0;
					MPScoreScreen().Present();
				}
			} else {
				Session.GameName[0] = 0;
				MPScoreScreen().Present();
			}

			if (Session.MPGameMode != NULL) {
				finished = Session.MPGameMode->vt_entry_A8();
			}

			Session.CurGame++;
			if (Session.CurGame >= 4) {
				Session.CurGame = 3;
			}
		}

		if (!finished) {
			Session.GameName[0] = 0;
			if (Session.MPGameMode != NULL) {
				Session.MPGameMode->vt_entry_A4(true);
			}
			Game::IsActive = false;
			FUN_0078ABF0(false, L"");
			return;
		}

		Session.MPGameMode->vt_entry_AC();
		FUN_0078ABF0(false, L"");
	}

	FUN_005BF260(Scen->WinMovie, -1, true, true, true);

	if (!Session.Play) {
		InputManagerClass::Instance->DoSomething();

		if (!Scen->IsSkipScore) {
			STLScoreScreen().Present();
		}

		if (Scen->PostScoreMovie != VQ_NONE) {
			FUN_005BF260(Scen->PostScoreMovie, -1, true, true, true);
		}
		if (Scen->PreMapSelectMovie != VQ_NONE) {
			FUN_005BF260(Scen->PreMapSelectMovie, -1, true, true, true);
		}

		if (!CopyProtection::CheckProtectedData() || Scen->IsOneTimeOnly) {
			Game::IsActive = false;
			MouseCursor->Show_Mouse();
			return;
		}

		if (Scen->IsEndOfGame) {
			if (Scen->Campaign != CAMPAIGN_NONE) {
				FUN_005BF260(Campaigns[Scen->Campaign]->FinalMovie, -1, true, true, true);
			}
			Show_Who_Was_Responsible();
			Game::IsActive = false;
			MouseCursor->Show_Mouse();
			return;
		}

		bool skip = false;
		if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
			if (Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
				skip = true;
			}
		}

		if (!skip) {
			if (Scen->IsNoMapSel) {
				if (Scen->GlobalFlags[1].Value) {
					Map_Select_Advance(Scen, Scen->AltNextScenarioName);
				} else {
					Map_Select_Advance(Scen, Scen->NextScenarioName);
				}
			} else {
				MouseCursor->Show_Mouse();
				Map_Selection(Scen);
				MouseCursor->Hide_Mouse();
			}
		}

		InputManagerClass::Instance->DoSomething();
	}

	MouseCursor->Show_Mouse();

	Environment.Store();

	Scen->Scenario++;

	int saved_ai_players;
	int saved_difficulties[8];
	int saved_countries[8];
	int saved_colors[8];
	NodeNameType * saved_slots[8];

	if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
		if (Session.Type == GAME_IPX) {
			PacketTransport->Close_Socket();
			if (PacketTransport != NULL) {
				delete PacketTransport;
			}
			PacketTransport = NULL;
			PacketTransport = new IPXInterfaceClass;
		}

		PacketTransport->Init();

		saved_ai_players = Session.Options.AIPlayers;
		memcpy(saved_slots, Session.Slots, sizeof(saved_slots));
		memcpy(saved_colors, Session.Options.AISlots.Colors, sizeof(saved_colors));
		memcpy(saved_countries, Session.Options.AISlots.Countries, sizeof(saved_countries));
		memcpy(saved_difficulties, Session.Options.AISlots.Difficulties, sizeof(saved_difficulties));

		if (!Net2Init_Network()) {
			if (PacketTransport != NULL) {
				delete PacketTransport;
				PacketTransport = NULL;
			}
			Game::IsActive = false;
			return;
		}

		Session.Options.AIPlayers = saved_ai_players;
		for (int i = 0; i < 8; i++) {
			Session.Options.AISlots.Difficulties[i] = saved_difficulties[i];
			Session.Options.AISlots.Countries[i] = saved_countries[i];
			Session.Options.AISlots.Colors[i] = saved_colors[i];
			Session.Slots[i] = saved_slots[i];
		}
		InputManagerClass::Instance->DoSomething();
	}

	Game::IsActive = CopyProtection::CheckProtectedData();
	if (!Game::IsActive) {
		MouseCursor->Show_Mouse();
		return;
	}

	while (!Start_Scenario(Scen->ScenarioName, true, Scen->Campaign)) {
	}

	if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
		Session.Create_Connections();

		if (Session.Type == GAME_IPX) {
			Ipx.Set_Timing((Ipx.Global_Response_Time() + 2 < 15 ? 15 : Ipx.Global_Response_Time() + 2), -1, 600, true);
			Ipx.Set_External_Timing((Ipx.Global_Response_Time() + 2 < 60 ? 60 : Ipx.Global_Response_Time() + 2), -1, 600);
		} else if (Session.Type == GAME_INTERNET && Game::Network::PreCalcFrameRate == 0) {
			Ipx.Set_Timing((Ipx.Global_Response_Time() + 2 < 60 ? 60 : Ipx.Global_Response_Time() + 2), -1, 600, true);
		}
	}

	Environment.Restore();

	if (ToolTips != NULL) {
		ToolTips->Activate(Options.ToolTips);
	}

	Map.Render();
	FUN_004A3C30(&FileSystem::TEMPERAT_PAL, 7, Game::CallBack);
}

// FUNCTION: GAMEMD 0x00685DC0
void Do_Lose(void)
{
	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		if (!Session.Play) {
			Wait_For_End_Of_Queue();
			FUN_0055CF10();
		}
	}

	ScenarioStarted = false;
	TacticalActive = false;

	Scen->ElapsedTimer.Stop();
	FUN_005BF450();

	MouseCursor->Erase_Mouse(HiddenSurface);
	MouseCursor->Release_Mouse();
	Map.Set_Default_Mouse(MOUSE_NORMAL, false);
	MouseCursor->Hide_Mouse();
	Toggle_Display_Mode(false);

	FUN_00407070(DAT_0087E758);
	FUN_004070B0(DAT_0087E758);

	int timeout = TickCount + 300;

	while (FUN_007529E0() && timeout > TickCount) {
		Game::CallBack();
	}

	FUN_007529A0(true);
	FUN_00752CA0();
	FUN_00406E80();
	FUN_00750FA0();
	FUN_00407150(DAT_0087E758, 0x4000);

	bool finished = false;

	if (Session.Type != GAME_NORMAL) {
		if (!Session.Play) {
			Session.GamesPlayed++;

			if (Session.Type == GAME_INTERNET) {
				FUN_0078ABF0(true, StringTable::LoadString("TXT_AWAY_SCORESCREEN", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x9bc));
			}

			if (Session.MPGameMode != NULL) {
				if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode->vt_entry_08()) {
					FUN_0052FEC0(NULL, true);
				} else {
					Session.GameName[0] = 0;
					MPScoreScreen().Present();
				}
			} else {
				Session.GameName[0] = 0;
				MPScoreScreen().Present();
			}

			if (Session.MPGameMode != NULL) {
				finished = Session.MPGameMode->vt_entry_A8();
			}

			Session.CurGame++;
			if (Session.CurGame >= 4) {
				Session.CurGame = 3;
			}
		}

		if (!finished) {
			Session.GameName[0] = 0;
			if (Session.MPGameMode != NULL) {
				Session.MPGameMode->vt_entry_A4(false);
			}
			Game::IsActive = false;
			FUN_0078ABF0(false, L"");
			return;
		}

		Session.MPGameMode->vt_entry_AC();
		FUN_0078ABF0(false, L"");
	}

	FUN_005BF260(Scen->LoseMovie, -1, true, true, true);
	FUN_0052FEC0(NULL, true);

	if ((Session.Type == GAME_NORMAL && !Session.Play && WWMessageBox().Process(StringTable::LoadString("TXT_TO_REPLAY", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x9ff), StringTable::LoadString("TXT_YES", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x9ff), StringTable::LoadString("TXT_NO", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0x9ff), NULL, false) == 0) || finished) {
		if (Session.Type == GAME_INTERNET) {
			FUN_0078ABF0(true, StringTable::LoadString("TXT_AWAY_LOADING", NULL, "D:\\ra2mdpost\\Scenario.CPP", 0xa06));
		}

		int saved_ai_players;
		int saved_difficulties[8];
		int saved_countries[8];
		int saved_colors[8];
		NodeNameType * saved_slots[8];

		if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
			if (Session.Type == GAME_IPX) {
				PacketTransport->Close_Socket();
				if (PacketTransport != NULL) {
					delete PacketTransport;
				}
				PacketTransport = NULL;
				PacketTransport = new IPXInterfaceClass;
			}

			PacketTransport->Init();

			saved_ai_players = Session.Options.AIPlayers;
			memcpy(saved_slots, Session.Slots, sizeof(saved_slots));
			memcpy(saved_colors, Session.Options.AISlots.Colors, sizeof(saved_colors));
			memcpy(saved_countries, Session.Options.AISlots.Countries, sizeof(saved_countries));
			memcpy(saved_difficulties, Session.Options.AISlots.Difficulties, sizeof(saved_difficulties));

			if (!Net2Init_Network()) {
				if (PacketTransport != NULL) {
					delete PacketTransport;
					PacketTransport = NULL;
				}
				Game::IsActive = false;
				if (Session.Type == GAME_INTERNET) {
					FUN_0078ABF0(false, L"");
				}
				return;
			}

			Session.Options.AIPlayers = saved_ai_players;
			for (int i = 0; i < 8; i++) {
				Session.Options.AISlots.Difficulties[i] = saved_difficulties[i];
				Session.Options.AISlots.Countries[i] = saved_countries[i];
				Session.Options.AISlots.Colors[i] = saved_colors[i];
				Session.Slots[i] = saved_slots[i];
			}
			InputManagerClass::Instance->DoSomething();
		}

		Start_Scenario(Scen->ScenarioName, false, Scen->Campaign);

		if ((Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) && Session.MPGameMode != NULL && Session.MPGameMode->vt_entry_08()) {
			Session.Create_Connections();

			if (Session.Type == GAME_IPX) {
				Ipx.Set_Timing(Ipx.Global_Response_Time() + 2 < 15 ? 15 : Ipx.Global_Response_Time() + 2, -1, 600, true);
				Ipx.Set_External_Timing(Ipx.Global_Response_Time() + 2 < 60 ? 60 : Ipx.Global_Response_Time() + 2, -1, 600);
			} else if (Session.Type == GAME_INTERNET && Game::Network::PreCalcFrameRate == 0) {
				Ipx.Set_Timing(Ipx.Global_Response_Time() + 2 < 60 ? 60 : Ipx.Global_Response_Time() + 2, -1, 600, true);
			}
		}

		Environment.Restore();

		if (ToolTips != NULL) {
			ToolTips->Activate(Options.ToolTips);
		}

		Map.Render();

		if (Session.Type == GAME_INTERNET) {
			FUN_0078ABF0(false, L"");
		}
	} else {
		Game::IsActive = false;
	}
}

// FUNCTION: GAMEMD 0x006863E0
void Do_Restart(void)
{
	FUN_0072DDB0();
	Toggle_Display_Mode(false);
	ScenarioStarted = false;
	TacticalActive = false;

	if (ToolTips != NULL) {
		ToolTips->Activate(false);
	}

	Scen->ElapsedTimer.Stop();
	FUN_005BF450();

	CDTimerClass<SystemTimerClass> timer;
	timer = 60;
	Theme.Stop(true);

	FUN_00406E80();
	FUN_00750FA0();
	FUN_007529A0(true);
	FUN_00752CA0();
	Map.Set_Default_Mouse(MOUSE_NORMAL, false);
	InputManagerClass::Instance->DoSomething();

	Start_Scenario(Scen->ScenarioName, false, Scen->Campaign);
	Environment.Restore();

	while (timer > 0) {
		Game::CallBack();
	}
	InputManagerClass::Instance->DoSomething();

	if (ToolTips != NULL) {
		ToolTips->Activate(Options.ToolTips);
	}
	Map.Render();
}

// FUNCTION: GAMEMD 0x00686570
void Do_Abort(void)
{
	ScenarioStarted = false;
	TacticalActive = false;

	Scen->ElapsedTimer.Stop();
	Scen->Scenario = 1;

	FUN_005BF450();
	InputManagerClass::Instance->DoSomething();

	MouseCursor->Erase_Mouse(HiddenSurface);
	MouseCursor->Release_Mouse();
	Map.Set_Default_Mouse(MOUSE_NORMAL, false);
	Theme.Stop(true);

	FUN_00407070(DAT_0087E758);
	VoxClass::Play("EVA_BattleControlTerminated", 2, -1);
	FUN_004070B0(DAT_0087E758);

	int timeout = TickCount + 300;

	while (FUN_007529E0() && timeout > TickCount) {
		Game::CallBack();
	}

	FUN_007529A0(true);
	FUN_00752CA0();
	FUN_00406E80();
	FUN_00750FA0();
	FUN_00407150(DAT_0087E758, 0x4000);
	Toggle_Display_Mode(false);
	Game::IsActive = false;
}

// FUNCTION: GAMEMD 0x00686700
void ScenarioClass::Set_Scenario_Name(char const * name)
{
	if (name != NULL) {
		strncpy(ScenarioName, name, sizeof(ScenarioName));
		ScenarioName[ARRAY_SIZE(ScenarioName) - 1] = '\0';
	}
}

// FUNCTION: GAMEMD 0x00686730
bool Read_Scenario_INI(char const * fname, bool)
{
	if (CD::Force_Available() == false) {
		return(false);
	}

	Frame = 0;

	CCINIClass ini;
	CCFileClass file(fname);

	DebugString("Read_Scenario_INI - Filename is %s\n", fname);

	int result = ini.Load(file, true);

	if (result == 0) {
		DebugString("Scenario ini load failed!\n");
		return(false);
	}

	strcpy(Scen->ScenarioName, fname);

	bool ok = Read_Scenario_INI(ini);
	return(ok);
}

void Create_Units(bool official);

static void Remove_AI_Players(void)
{
	for (int i = 0; i < Houses.Count(); i++) {
		int aicount = 0;
		HouseClass * housep = Houses[i];
		if (housep->IsHuman == false && !housep->Class->IsMultiplayPassive) {
			aicount++;
			if (aicount > Session.Options.AIPlayers) {
				housep->Clobber_All();
			}
		}
	}
}

// FUNCTION: GAMEMD 0x00686890
void Multiplayer_Last_Minute_Fixups(bool official)
{
	if ((Session.Options.AIPlayers + Session.Players.Count() < Rule->MaxPlayers) && !Debug_Map) {
		Remove_AI_Players();
	}
	Game::CallBack();

	if (!Debug_Map) {
		int save_init = ScenarioInit;
		ScenarioInit = 0;
		if (Session.MPGameMode != NULL) {
			Session.MPGameMode->StartingPositionsToHouseBaseCells2(official);
			Session.MPGameMode->FUN_005D6D80();
		} else {
			Create_Units(official);
		}
		ScenarioInit = save_init;
	}
	Session.Update_Progress(93);
	Game::CallBack();

	if (Session.Options.Goodies) {
		int count = MAX(Rule->CrateMinimum, Session.NumPlayers);
		count = MIN(count, Rule->CrateMaximum);
		for (int index = 0; index < count; index++) {
			Map.Place_Random_Crate();
		}
	}
	Game::CallBack();

	Map.Compute_Start_Pos();

	HouseClass * special_hptr = House_From_HousesType(HouseTypeClass::From_Name("Special"));

	for (int i = 0; i < Houses.Count(); i++) {
		Houses[i]->Base.House = Houses[i];

		if (!Houses[i]->IsHuman && !Houses[i]->Class->IsMultiplayPassive) {
			Houses[i]->PickEnemyTimer = Rule->AIHateDelays[Houses[i]->Difficulty];

			int available_money = Houses[i]->Available_Money();
			Houses[i]->Refund_Money((int)((Rule->MultiplayerAICreditMultipliers[Houses[i]->Difficulty] / 100.0) * available_money));
		}

		if (Houses[i]->Class->IsMultiplayPassive) {
			Houses[i]->Class->IsWallOwner = false;
		} else {
			Houses[i]->Class->IsWallOwner = true;
		}

		if (Houses[i] != special_hptr) {
			Houses[i]->Make_Ally(special_hptr, false);
			special_hptr->Make_Ally(Houses[i], false);
		}
	}

	if (Session.MPGameMode != NULL) {
		Session.MPGameMode->AllyTeams();
		Session.MPGameMode->vt_entry_8C();
	}
	Scen->FUN_0068C050();
	memset(Session.PlayerChatEnabled, true, sizeof(Session.PlayerChatEnabled));
}

// FUNCTION: GAMEMD 0x00687CE0
void Write_Scenario_INI(char const * fname, bool mplayer)
{
	CCINIClass ini;

	if (Debug_Map) {
		if (CCFileClass(fname).Is_Available()) {
			CCFileClass cfile(fname);
			ini.Load(cfile, true);
		}
	}

	Scen->Write_INI(ini, mplayer);

	if (mplayer) {
		if (RandomMapGen.MapPreview != NULL && RandomMapGen.MapPreview->Get_Preview_Surface() != NULL) {
			RandomMapGen.MapPreview->Write_INI(ini);
		} else {
			MapPreviewClass map_preview;
			map_preview.Create_Preview();
			map_preview.Write_INI(ini);
		}
	} else {
		HouseClass::Write_All(ini);
	}
	Map.Write_INI(ini);
	TerrainClass::Write_INI(ini);
	UnitClass::Write_INI(ini);
	InfantryClass::Write_INI(ini);
	AircraftClass::Write_INI(ini);
	BuildingClass::Write_All(ini);
	OverlayClass::Write_INI(ini);
	SmudgeClass::Write_INI(ini);
	TubeClass::Write_INI(ini);

	if (!mplayer || Debug_Map) {
		ScriptTypeClass::Write_All(ini, SCOPE_LOCAL);
		TaskForceClass::Write_All(ini, SCOPE_LOCAL);
		TeamTypeClass::Write_All(ini, SCOPE_LOCAL);
		AITriggerTypeClass::Write_All(ini, SCOPE_LOCAL);
	}

	TriggerTypeClass::Write_All(ini);
	TagTypeClass::Write_All(ini);

	RawFileClass rawfile(fname);
	ini.Save(rawfile, true);
}

extern HouseClass * DAT_00AC1198;

// FUNCTION: GAMEMD 0x00687F10
void Assign_Houses(void)
{
	int i;
	int count = Session.Players.Count();

	for (i = 0; i < count; i++) {
		if (Session.Players[i]->Player.House == -3) {
			Session.Players[i]->Set_Country(0);
		}
	}

	bool assigned[MAX_PLAYERS];
	for (i = 0; i < MAX_PLAYERS; i++) {
		assigned[i] = false;
	}
	DAT_00AC1198 = NULL;

	for (i = 0; i < count; i++) {
		int index = -1;
		int lowest_color = -1;
		for (int j = 0; j < count; j++) {
			if (!assigned[j] && (lowest_color == -1 || Session.Players[j]->Player.Color < lowest_color)) {
				lowest_color = Session.Players[j]->Player.Color;
				index = j;
			}
		}
		assigned[index] = true;

		NodeNameType * player = Session.Players[index];

		HouseClass * housep = new HouseClass(HouseTypes[player->Player.House]);

		if (Session.Type == GAME_INTERNET) {
			Wstring name = FUN_007B7F90(player->Name);
			housep->IniName = TStringID<HOUSE_NAME_MAX>(name.get());
		} else {
			housep->IniName = TStringID<HOUSE_NAME_MAX>("<human player>");
		}
		wcsncpy(housep->UIName, player->Name, 21);
		housep->UIName[20] = 0;
		housep->IsHuman = true;
		housep->Init_Data(player->Player.Color, (HousesType)player->Player.House, Session.Options.Credits);

		housep->Scheme = Session.Color_Index_To_Scheme(player->Player.Color);
		housep->FUN_0050B840();
		housep->FUN_0050BA00();
		housep->StartingPoint = player->Get_Start();
		housep->StartingAllies = player->Slot.Team;

		if (index == 0) {
			PlayerPtr = housep;
			PlayerPtr->IsPlayerControl = true;
		}
		if (player->Slot.SpectatorFlag == -1) {
			DAT_00AC1198 = housep;
		}

		housep->Control.TechLevel = BuildLevel;
		housep->Assign_Handicap(DIFF_NORMAL);

		player->Player.ID = housep->HeapID;
	}

	int ai_count = 0;
	int ai_players = Session.Options.AIPlayers;
	for (int slot = 0; slot < 8; slot++) {
		if (ai_count < ai_players && Session.Options.AISlots.Countries[slot] != -1 && Session.Options.AISlots.Countries[slot] != -3) {
			int country = Session.Options.AISlots.Countries[slot];
			int color = Session.Options.AISlots.Colors[slot];
			ai_count++;

			HouseClass * housep = new HouseClass(HouseTypes[country]);

			housep->Control.TechLevel = BuildLevel;
			housep->IsHuman = false;
			housep->Init_Data(color, (HousesType)country, Session.Options.Credits);
			housep->Scheme = Session.Color_Index_To_Scheme(color);
			housep->FUN_0050B840();
			housep->FUN_0050BA00();
			housep->StartingPoint = Session.Options.AISlots.Starts[slot];
			housep->StartingAllies = Session.Options.AISlots.Allies[slot];
			if (housep->StartingAllies != -1) {
				Scen->TeamsPresent = true;
			}
			housep->IniName = TStringID<HOUSE_NAME_MAX>("Computer");
			wcscpy(housep->UIName, StringTable::LoadString("TXT_COMPUTER", NULL, "D:\\ra2mdpost\\Scenario.CPP", 4032));

			if (Session.Type != GAME_NORMAL) {
				housep->IQ = Rule->MaxIQ;
			}

			int difficulty = Session.Options.AISlots.Difficulties[slot];
			if (Session.Players.Count() > 1 && Rule->IsCompEasyBonus && difficulty > 0) {
				difficulty--;
			}
			housep->Assign_Handicap((DiffType)difficulty);
		}
	}

	HouseClass * neutral_house = new HouseClass(HouseTypes[HouseTypeClass::From_Name("Neutral")]);
	neutral_house->Scheme = Fetch_Scheme_Index_By_Name("LightGrey", 53);
	neutral_house->FUN_0050B840();

	HouseClass * special_house = new HouseClass(HouseTypes[HouseTypeClass::From_Name("Special")]);
	special_house->Scheme = Fetch_Scheme_Index_By_Name("LightGrey", 53);
	special_house->FUN_0050B840();
}

// FUNCTION: GAMEMD 0x00688380
DynamicVectorClass<Cell> Build_Start_Waypoint_List(void)
{
	DynamicVectorClass<Cell> waypts;

	int num_waypts = 0;
	for (int i = 0; i < 8; i++) {
		if (Scen->Is_Valid_Waypoint(i)) {
			num_waypts++;
		} else {
			break;
		}
	}

	int spectators = 0;
	for (int s = 0; s < Session.Players.Count(); s++) {
		if (Session.Players[s]->Slot.SpectatorFlag == -1) {
			spectators++;
		}
	}

	int look_for = MAX(num_waypts, Session.Players.Count() - spectators + Session.Options.AIPlayers);

	for (int waycount = 0; waycount < look_for; waycount++) {
		if (Scen->Is_Valid_Waypoint(waycount)) {
			waypts.Add(Scen->Get_Waypoint_Cell(waycount));
			DebugString("Multiplayer start waypoint found at cell %d,%d\n", Scen->Get_Waypoint_Cell(waycount).X, Scen->Get_Waypoint_Cell(waycount).Y);
		}
	}

	int deficiency = look_for - waypts.Count();
	if (deficiency > 0) {
		DebugString("Multiplayer start waypoint deficiency - looking for more start positions\n");

		while (waypts.Count() < look_for) {
			Cell trycell = Cell(Map.MapRect.X + Random_Pick(10, Map.MapRect.Width - 10), Map.MapRect.Y + 10 + Random_Pick(0, Map.MapRect.Height - 10));

			trycell = Map.Nearby_Location(trycell, SPEED_TRACK, -1, MZONE_NORMAL, false, Point2D(8, 8));
			if (trycell != CELL_NONE) {
				waypts.Add(trycell);
				DebugString("Random multiplayer start waypoint added at cell %d,%d\n", trycell.X, trycell.Y);
			}
		}
	}

	return(waypts);
}

DynamicVectorClass<Cell> __fastcall FUN_00688380(char arg);

static inline bool Is_Base_Unit(UnitTypeClass const * type)
{
	for (int i = 0; i < Rule->BaseUnit.Count(); i++) {
		if (Rule->BaseUnit[i] == type) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x006886B0
void Create_Units(bool official)
{
	Cell centroid;
	DynamicVectorClass<TechnoClass *> deployed_list;
	int unit_count = Session.Options.UnitCount;

	if (Session.Options.Bases) {
		unit_count--;
	}

	DebugString("Creating %d units - Random seed is %08x\n", unit_count, Scen->RandomNumber);
	DebugString("UniqueID is %08x\n", Scen->UniqueID);

	int total_cost = 0;
	int total_objs = 0;
	for (int u = 0; u < UnitTypes.Count(); u++) {
		UnitTypeClass * utype = UnitTypes[u];
		if (utype->IsAllowedToStartInMultiplayer) {
			if (!Is_Base_Unit(utype)) {
				total_cost += utype->Raw_Cost();
				total_objs++;
			}
		}
	}

	for (int inf = 0; inf < InfantryTypes.Count(); inf++) {
		InfantryTypeClass * itype = InfantryTypes[inf];
		if (itype->IsAllowedToStartInMultiplayer) {
			total_cost += itype->Raw_Cost();
			total_objs++;
		}
	}

	int average_cost = total_cost / total_objs;
	int max_value = unit_count * average_cost;

	DynamicVectorClass<Cell> waypts = FUN_00688380(official);
	bool taken[16];
	for (int index = 0; index < ARRAY_SIZE(taken); index++) {
		taken[index] = false;
	}

	int numtaken = 0;
	for (int house = 0; house < Houses.Count(); house++) {
		HouseClass * hptr = Houses[house];

		if (hptr->Class->IsMultiplayPassive) continue;

		DebugString("Generating units for house %d (%s)\n", house, (char const *)hptr->Class->IniName);

		DynamicVectorClass<UnitTypeClass *> units;
		DynamicVectorClass<InfantryTypeClass *> infantry;
		unsigned int mask = 1 << hptr->Class->HeapID;

		for (int unit = 0; unit < UnitTypes.Count(); unit++) {
			UnitTypeClass * utype = UnitTypes[unit];
			if (utype->IsAllowedToStartInMultiplayer) {
				if (utype->Level <= hptr->Control.TechLevel && (utype->Ownable & mask)) {
					if (!Is_Base_Unit(utype)) {
						units.Add(utype);
					}
				}
			}
		}

		for (int inf = 0; inf < InfantryTypes.Count(); inf++) {
			InfantryTypeClass * itype = InfantryTypes[inf];
			if (itype->IsAllowedToStartInMultiplayer) {
				if (itype->Level <= hptr->Control.TechLevel && (itype->Ownable & mask)) {
					infantry.Add(itype);
				}
			}
		}

		if (numtaken == 0) {
			int pick = Scen->RandomNumber(0, waypts.Count() - 1);
			centroid = waypts[pick];
			taken[pick] = true;
			numtaken++;
		} else {
			int score[26];
			memset(score, '\0', sizeof(score));

			for (int index = 0; index < waypts.Count(); index++) {
				if (!taken[index]) {
					for (int trypoint = 0; trypoint < waypts.Count(); trypoint++) {
						if (taken[trypoint]) {
							score[index] += Distance(waypts[index], waypts[trypoint]);
						}
					}
				}
			}

			int best = 0;
			int bestvalue = 0;
			for (int searchindex = 0; searchindex < waypts.Count(); searchindex++) {
				if (score[searchindex] > bestvalue || bestvalue == 0) {
					bestvalue = score[searchindex];
					best = searchindex;
				}
			}

			centroid = waypts[best];
			taken[best] = true;
			numtaken++;
		}

		hptr->Set_Base_Spawn_Cell(centroid);

		if (Session.Options.Bases) {
			TechnoClass * obj = new UnitClass(hptr->Get_First_Ownable(Rule->BaseUnit), hptr);
			if (!obj->Unlimbo(Coord(centroid), DIR_N)) {
				if (!Scan_Place_Object(obj, centroid, 1)) {
					delete obj;
					obj = NULL;
				}
			}
			if (obj != NULL) {
				hptr->FlagHome = CELL_NONE;
				hptr->FlagLocation = NULL;
				if (Scen->Special.IsCaptureTheFlag) {
					hptr->Flag_Attach((UnitClass *)obj, true);
				}
			}
		}

		int deployed_value = 0;
		deployed_list.Clear();

		while (deployed_value < max_value) {
			TechnoTypeClass const * tech = NULL;

			if (deployed_value < (max_value * 2) / 3 && units.Count() > 0) {
				tech = units[Scen->RandomNumber(0, units.Count() - 1)];
			} else if (infantry.Count() > 0) {
				tech = infantry[Scen->RandomNumber(0, infantry.Count() - 1)];
			}

			ObjectClass * obj = tech->Create_One_Of(hptr);
			TechnoClass * tobj = Dynamic_Cast<TechnoClass *>(obj);

			if (!Scan_Place_Object(obj, centroid, 4)) {
				delete obj;
			} else {
				DebugString("House %s deployed object %s\n", (char const *)hptr->Class->IniName, (char const *)tech->IniName);

				deployed_value += tech->Raw_Cost();
				deployed_list.Add(tobj);

				if (Scen->Special.IsInitialVeteran) {
					tobj->Veterancy.Set_Elite(true);
				}

				if (!hptr->FUN_0050B730()) {
					tobj->Set_Mission(MISSION_GUARD_AREA);
				} else {
					tobj->Set_Mission(MISSION_GUARD);
				}
			}
		}

		deployed_list.Clear();
	}
	DebugString("Finished unit generation. Random number is %d\n", Scen->RandomNumber(0, 65535));
}

// FUNCTION: GAMEMD 0x00689310
void ScenarioClass::Save(IStream * stream) const
{
	DebugString("Scenario Save: ElapsedTimer = %ld\n", ElapsedTimer);
	bool was_stopped = !ElapsedTimer.Is_Active();
	if (!was_stopped) {
		ElapsedTimer.Stop();
	}

	stream->Write(this, sizeof(*this), NULL);

	AllowableUnits.Save(stream);
	AllowableUnitMaximums.Save(stream);
	AllowableUnitCounts.Save(stream);

	if (!was_stopped) {
		ElapsedTimer.Start();
	}
}

// FUNCTION: GAMEMD 0x00689470
void ScenarioClass::Load(IStream * stream)
{
	ElapsedTimer.Stop();

	stream->Read(this, sizeof(*this), NULL);

	new (this) ScenarioClass(NoInitClass());

	AllowableUnits.Load(stream);
	AllowableUnitMaximums.Load(stream);
	AllowableUnitCounts.Load(stream);

	for (int i = 0; i < AllowableUnits.Count(); i++) {
		Swizzle_Pointer(&AllowableUnits[i]);
	}

	if (MissionTimerText[0]) {
		MissionTimerTextCSF = StringTable::LoadString(MissionTimerText, NULL, "D:\\ra2mdpost\\Scenario.CPP", 6608);
	} else {
		MissionTimerTextCSF = L"";
	}

	ElapsedTimer.Start();
	DebugString("Scenario Load: ElapsedTimer = %ld\n", ElapsedTimer);
}

// FUNCTION: GAMEMD 0x00689670
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

// FUNCTION: GAMEMD 0x006896C0
bool ScenarioClass::Set_Global_To(char const * variable_name, bool value)
{
	int index = Global_From_Name(variable_name);
	if (index != -1) {
		return(Set_Global_To(index, value));
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00689760
bool ScenarioClass::Fetch_Global_Value(int global, bool & value)
{
	if (global >= 0 && global < ARRAY_SIZE(GlobalFlags)) {
		value = GlobalFlags[global].Value;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00689790
bool ScenarioClass::Fetch_Global_Value(char const * variable_name, bool & value)
{
	int index = Global_From_Name(variable_name);
	if (index != -1) {
		return(Fetch_Global_Value(index, value));
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00689820
int ScenarioClass::Global_From_Name(char const * variable_name)
{
	for (int i = 0; i < ARRAY_SIZE(GlobalFlags); i++) {
		if (!strcmp(variable_name, GlobalFlags[i].VariableName)) {
			return(i);
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x00689880
bool ScenarioClass::Read_Global_INI(CCINIClass const & ini)
{
	char const * const SECTION = "VariableNames";
	int length = MIN(ini.Entry_Count(SECTION), ARRAY_SIZE(GlobalFlags));
	for (int i = 0; i < length; i++) {
		char const * entry = ini.Get_Entry(SECTION, i);
		int index = atoi(entry);

		ini.Get_String(SECTION, entry, NULL, GlobalFlags[index].VariableName, sizeof(GlobalFlags[index].VariableName));
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00689910
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

// FUNCTION: GAMEMD 0x00689960
bool ScenarioClass::Set_Local_To(char const * variable_name, bool value)
{
	int index = Local_From_Name(variable_name);
	if (index != -1) {
		return(Set_Local_To(index, value));
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00689A00
bool ScenarioClass::Fetch_Local_Value(int index, bool & value)
{
	if (index >= 0 && index < ARRAY_SIZE(LocalFlags)) {
		value = LocalFlags[index].Value;
		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00689A30
bool ScenarioClass::Fetch_Local_Value(char const * variable_name, bool & value)
{
	int index = Local_From_Name(variable_name);
	if (index != -1) {
		return(Fetch_Local_Value(index, value));
	}
	return(false);
}

// FUNCTION: GAMEMD 0x00689AC0
int ScenarioClass::Local_From_Name(char const * variable_name)
{
	for (int i = 0; i < ARRAY_SIZE(LocalFlags); i++) {
		if (strcmp(variable_name, LocalFlags[i].VariableName) == 0) {
			return(i);
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x00689B20
bool ScenarioClass::Read_Local_INI(CCINIClass const & ini)
{
	char buffer[128];
	int index;

	for (index = 0; index < ARRAY_SIZE(LocalFlags); index++) {
		LocalFlags[index].VariableName[0] = '\0';
	}

	char const * const SECTION = "VariableNames";
	int length = MIN(ini.Entry_Count(SECTION), ARRAY_SIZE(LocalFlags));
	for (index = 0; index < length; index++) {
		char const * entry = ini.Get_Entry(SECTION, index);
		int index = atoi(entry);

		ini.Get_String(SECTION, entry, NULL, buffer, sizeof(buffer));
		char * token = strtok(buffer, ",");
		strcpy(LocalFlags[index].VariableName, token);

		token = strtok(NULL, ",");
		if (token != NULL) {
			LocalFlags[index].Value = (atoi(token) == 0) ? false : true;
		}
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00689C60
bool ScenarioClass::Write_Local_INI(CCINIClass & ini) const
{
	static char const * const SECTION = "VariableNames";
	char index_buffer[10];
	char buffer[128];

	ini.Clear(SECTION);
	int length = ARRAY_SIZE(LocalFlags);
	for (int index = 0; index < length; index++) {
		if (LocalFlags[index].VariableName[0] != '\0') {
			wsprintf(index_buffer, "%d", index);
			wsprintf(buffer, "%s,%d", LocalFlags[index].VariableName, LocalFlags[index].Value ? 1 : 0);
			ini.Put_String(SECTION, index_buffer, buffer);
		}
	}
	return(true);
}

// FUNCTION: GAMEMD 0x00689CF0
int ScenarioClass::Used_Local_Count(void)
{
	int count = 0;
	for (int i = 0; i < ARRAY_SIZE(LocalFlags); i++) {
		if (LocalFlags[i].VariableName[0] != '\0') {
			count++;
		}
	}
	return(count);
}

// FUNCTION: GAMEMD 0x00689D10
int ScenarioClass::First_Unused_Local(void) const
{
	for (int index = 0; index < ARRAY_SIZE(LocalFlags); index++) {
		if (LocalFlags[index].VariableName[0] == '\0') {
			return(index);
		}
	}
	return(-1);
}

// FUNCTION: GAMEMD 0x00689D30
bool ScenarioClass::ReadStartPoints(CCINIClass const & ini)
{
	char const * const SECTION = "Header";
	char buffer[12];

	StartX = -1;
	StartY = -1;
	Width = -1;
	Height = -1;
	NumberStartingPoints = -1;
	int j;
	for (j = 0; j < ARRAY_SIZE(StartingPoints); j++) {
		StartingPoints[j] = Point2D(0, 0);
	}

	StartX = ini.Get_Int(SECTION, "StartX", StartX);
	StartY = ini.Get_Int(SECTION, "StartY", StartY);
	Width = ini.Get_Int(SECTION, "Width", Width);
	Height = ini.Get_Int(SECTION, "Height", Height);
	NumberStartingPoints = ini.Get_Int(SECTION, "NumberStartingPoints", NumberStartingPoints);
	NumCoopHumanStartSpots = ini.Get_Int(SECTION, "NumCoopHumanStartSpots", NumCoopHumanStartSpots);
	for (int i = 0; i < NumberStartingPoints; i++) {
		sprintf(buffer, "Waypoint%d", i + 1);
		StartingPoints[i] = ini.Get_Point(SECTION, buffer, StartingPoints[i]);
	}
	return(true);
}

extern char DAT_00889F64[];

// FUNCTION: GAMEMD 0x00689E90
bool ScenarioClass::Read_INI(CCINIClass & ini)
{
	char const * const BASIC = "Basic";

	int i;

	ini.Reset();
	Special.Read_INI(ini);

	char const * const HEADER = "Header";
	StartX = ini.Get_Int(HEADER, "StartX", StartX);
	StartY = ini.Get_Int(HEADER, "StartY", StartY);
	Width = ini.Get_Int(HEADER, "Width", Width);
	Height = ini.Get_Int(HEADER, "Height", Height);
	NumberStartingPoints = ini.Get_Int(HEADER, "NumberStartingPoints", NumberStartingPoints);
	NumCoopHumanStartSpots = ini.Get_Int(HEADER, "NumCoopHumanStartSpots", NumCoopHumanStartSpots);
	char name[12];
	for (i = 0; i < NumberStartingPoints; i++) {
		sprintf(name, "Waypoint%d", i + 1);
		StartingPoints[i] = ini.Get_Point(HEADER, name, StartingPoints[i]);
	}

	ini.Get_String(BASIC, "NextScenario", NextScenarioName, NextScenarioName, sizeof(NextScenarioName));
	ini.Get_String(BASIC, "AltNextScenario", AltNextScenarioName, AltNextScenarioName, sizeof(AltNextScenarioName));
	IntroMovie = ini.Get_VQType(BASIC, "Intro", IntroMovie);
	BriefMovie = ini.Get_VQType(BASIC, "Brief", BriefMovie);
	WinMovie = ini.Get_VQType(BASIC, "Win", WinMovie);
	LoseMovie = ini.Get_VQType(BASIC, "Lose", LoseMovie);
	ActionMovie = ini.Get_VQType(BASIC, "Action", ActionMovie);
	PostScoreMovie = ini.Get_VQType(BASIC, "PostScore", PostScoreMovie);
	PreMapSelectMovie = ini.Get_VQType(BASIC, "PreMapSelect", PreMapSelectMovie);

	IsMultiplayerOnly = ini.Get_Bool(BASIC, "MultiplayerOnly", IsMultiplayerOnly);
	IsInheritTimer = ini.Get_Bool(BASIC, "TimerInherit", IsInheritTimer);
	IsEndOfGame = ini.Get_Bool(BASIC, "EndOfGame", IsEndOfGame);
	TransitTheme = ini.Get_ThemeType(BASIC, "Theme", TransitTheme);
	NewINIFormat = ini.Get_Int(BASIC, "NewINIFormat", 0);
	CarryOverPercent = ini.Get_Float(BASIC, "CarryOverMoney", CarryOverPercent);
	CarryOverPercent = MIN(CarryOverPercent, 1.0);
	CarryOverCap = ini.Get_Int(BASIC, "CarryOverCap", CarryOverCap);
	IsSkipScore = ini.Get_Bool(BASIC, "SkipScore", IsSkipScore);
	IsOneTimeOnly = ini.Get_Bool(BASIC, "OneTimeOnly", IsOneTimeOnly);
	IsNoMapSel = ini.Get_Bool(BASIC, "SkipMapSelect", IsNoMapSel);
	IsTruckCrate = ini.Get_Bool(BASIC, "TruckCrate", IsTruckCrate);
	IsTrainCargo = ini.Get_Bool(BASIC, "TrainCrate", IsTrainCargo);
	IsMoneyTiberium = ini.Get_Bool(BASIC, "FillSilos", IsMoneyTiberium);
	IsIgnoreGlobalAITriggers = ini.Get_Bool(BASIC, "IgnoreGlobalAITriggers", IsIgnoreGlobalAITriggers);
	Percent = ini.Get_Int(BASIC, "Percent", Percent);
	StartingDropships = ini.Get_Int(BASIC, "StartingDropships", StartingDropships);
	AllowableUnits = ini.Get_TechnoType_List(BASIC, "AllowableUnits", AllowableUnits);
	AllowableUnitMaximums = ini.Get_IntList(BASIC, "AllowableUnitMaximums", AllowableUnitMaximums);
	IsTibGrowth = ini.Get_Bool(BASIC, "TiberiumGrowthEnabled", IsTibGrowth);
	IsVeinGrowth = ini.Get_Bool(BASIC, "VeinGrowthEnabled", IsVeinGrowth);
	IsIceGrowth = ini.Get_Bool(BASIC, "IceGrowthEnabled", IsIceGrowth);
	IsTiberiumDeathToVisceroid = ini.Get_Bool(BASIC, "TiberiumDeathToVisceroid", IsTiberiumDeathToVisceroid);
	IsFreeRadar = ini.Get_Bool(BASIC, "FreeRadar", IsFreeRadar);
	Home = ini.Get_Int(BASIC, "HomeCell", Home);
	AltHome = ini.Get_Int(BASIC, "AltHomeCell", AltHome);

	ini.Get_Bool(BASIC, "CivEvac", false);

	for (i = 0; i < AllowableUnits.Count() - AllowableUnitMaximums.Count(); i++) {
		AllowableUnitMaximums.Add(-1);
	}

	for (i = 0; i < AllowableUnitMaximums.Count(); i++) {
		AllowableUnitCounts.Add(0);
	}

	Read_Local_INI(ini);
	Game::CallBack();

	char const * const RANKING = "Ranking";
	ParTimeEasy = ini.ReadTime(RANKING, "ParTimeEasy", ParTimeEasy);
	ParTimeMedium = ini.ReadTime(RANKING, "ParTimeMedium", ParTimeMedium);
	ParTimeDifficult = ini.ReadTime(RANKING, "ParTimeHard", ParTimeDifficult);
	ini.Get_String(RANKING, "UnderParTitle", "GUI:NoUnderParTitle", UnderParTitle, sizeof(UnderParTitle));
	ini.Get_String(RANKING, "UnderParMessage", "GUI:NoUnderParMessage", UnderParMessage, sizeof(UnderParMessage));
	ini.Get_String(RANKING, "OverParTitle", "GUI:NoOverParTitle", OverParTitle, sizeof(OverParTitle));
	ini.Get_String(RANKING, "OverParMessage", "GUI:NoOverParMessage", OverParMessage, sizeof(OverParMessage));

	char const * const LIGHTING = "Lighting";
	AmbientLight = (int)(100.0 * ini.Get_Float(LIGHTING, "Ambient", AmbientLight / 100.0) + 0.01);
	CurrentAmbientLight = AmbientLight;
	DesiredAmbientLight = AmbientLight;
	NormalLighting.RedTint = (int)(100.0 * ini.Get_Float(LIGHTING, "Red", NormalLighting.RedTint / 100.0) + 0.01);
	NormalLighting.GreenTint = (int)(100.0 * ini.Get_Float(LIGHTING, "Green", NormalLighting.GreenTint / 100.0) + 0.01);
	NormalLighting.BlueTint = (int)(100.0 * ini.Get_Float(LIGHTING, "Blue", NormalLighting.BlueTint / 100.0) + 0.01);
	NormalLighting.GroundLight = (int)(1000.0 * ini.Get_Float(LIGHTING, "Ground", NormalLighting.GroundLight / 1000.0f) + 0.01);
	NormalLighting.LevelLight = (int)(1000.0 * ini.Get_Float(LIGHTING, "Level", NormalLighting.LevelLight / 1000.0f) + 0.01);
	IonAmbientLight = (int)(100.0 * ini.Get_Float(LIGHTING, "IonAmbient", IonAmbientLight / 100.0) + 0.01);
	IonLighting.RedTint = (int)(100.0 * ini.Get_Float(LIGHTING, "IonRed", IonLighting.RedTint / 100.0) + 0.01);
	IonLighting.GreenTint = (int)(100.0 * ini.Get_Float(LIGHTING, "IonGreen", IonLighting.GreenTint / 100.0) + 0.01);
	IonLighting.BlueTint = (int)(100.0 * ini.Get_Float(LIGHTING, "IonBlue", IonLighting.BlueTint / 100.0) + 0.01);
	IonLighting.GroundLight = (int)(1000.0 * ini.Get_Float(LIGHTING, "IonGround", IonLighting.GroundLight / 1000.0f) + 0.01);
	IonLighting.LevelLight = (int)(1000.0 * ini.Get_Float(LIGHTING, "IonLevel", IonLighting.LevelLight / 1000.0f) + 0.01);
	NukeAmbientChangeRate = (int)ini.Get_Float(LIGHTING, "NukeAmbientChangeRate", NukeAmbientChangeRate);
	DominatorAmbient = (int)(100.0 * ini.Get_Float(LIGHTING, "DominatorAmbient", DominatorAmbient / 100.0) + 0.01);
	DominatorLighting.RedTint = (int)(100.0 * ini.Get_Float(LIGHTING, "DominatorRed", DominatorLighting.RedTint / 100.0) + 0.01);
	DominatorLighting.GreenTint = (int)(100.0 * ini.Get_Float(LIGHTING, "DominatorGreen", DominatorLighting.GreenTint / 100.0) + 0.01);
	DominatorLighting.BlueTint = (int)(100.0 * ini.Get_Float(LIGHTING, "DominatorBlue", DominatorLighting.BlueTint / 100.0) + 0.01);
	DominatorLighting.GroundLight = (int)(1000.0 * ini.Get_Float(LIGHTING, "DominatorGround", DominatorLighting.GroundLight / 1000.0f) + 0.01);
	DominatorLighting.LevelLight = (int)(1000.0 * ini.Get_Float(LIGHTING, "DominatorLevel", DominatorLighting.LevelLight / 1000.0f) + 0.01);
	DominatorAmbientChangeRate = (int)(1000.0 * ini.Get_Float(LIGHTING, "DominatorAmbientChangeRate", DominatorAmbientChangeRate / 1000.0f) + 0.01);

	Session.Update_Progress(55);
	Game::CallBack();

	if (Session.Type == GAME_NORMAL) {
		char player[20];
		ini.Get_String(BASIC, "Player", DAT_00889F64, player, sizeof(player));
		int index = HouseClass::FindIndexByName(player);
		if (index == -1) {
			index = 0;
		}
		PlayerPtr = Houses[index];
		PlayerHouse = PlayerPtr->Class->House;
	} else {
		Assign_Houses();
	}

	PlayerPtr->IsHuman = true;
	PlayerPtr->IsPlayerControl = true;
	PlayerPtr->CurrentDropship = 0;

	Session.Update_Progress(58);
	Game::CallBack();

	if (Debug_Map) {
		IsometricTileTypeClass::Init_Drawers();
	}

	Session.Update_Progress(60);
	Game::CallBack();

	return(true);
}

static inline Point2D Header_Point(CellClass & cell)
{
	int px;
	int py;
	TacticalMap->Lepton_To_Map_Pixel(cell.Center_Coord().X, cell.Center_Coord().Y, px, py);
	return Point2D(px / 60, py / 30);
}

// FUNCTION: GAMEMD 0x0068AD70
bool ScenarioClass::Write_INI(CCINIClass & ini, bool mplayer) const
{
	static char const * const BASIC = "Basic";
	ini.Clear(BASIC);

	ini.Clear(BASIC, "CivEvac");

	if (strlen(Scen->BriefingCSF)) {
		ini.Put_String(BASIC, "Briefing", Scen->BriefingCSF);
	}
	if (strlen(Scen->UIName)) {
		ini.Put_String(BASIC, "UIName", Scen->UIName);
	}

	Map.Reset_Iterator();
	int minx, miny, maxx, maxy;
	minx = miny = 10000;
	maxy = maxx = 0;
	CellClass * cell = Map.Iterate();
	while (cell != NULL) {
		if (Map.In_Local_Radar(cell->CellID, false)) {
			Point2D p = Header_Point(*cell);
			int x = p.X;
			int y = p.Y;
			if (x < minx) minx = x;
			if (x > maxx) maxx = x;
			if (y < miny) miny = y;
			if (y > maxy) maxy = y;
		}
		cell = Map.Iterate();
	}

	char const * const HEADER = "Header";
	ini.Put_Int(HEADER, "StartX", minx);
	ini.Put_Int(HEADER, "StartY", miny);
	ini.Put_Int(HEADER, "Width", maxx - minx);
	ini.Put_Int(HEADER, "Height", maxy - miny);

	int count = 0;
	for (int w = 0; w < 8; w++) {
		if (Scen->Is_Valid_Waypoint(w)) {
			count++;
		}
	}
	ini.Put_Int(HEADER, "NumberStartingPoints", count);
	ini.Put_Int(HEADER, "NumCoopHumanStartSpots", NumCoopHumanStartSpots);

	char name[12];
	for (int i = 0; i < 8; i++) {
		sprintf(name, "Waypoint%d", i + 1);
		if (i < count) {
			Cell wcell = Scen->Waypoint[i];
			ini.Put_Point(HEADER, name, Header_Point(Map[wcell]));
		} else {
			ini.Put_Point(HEADER, name, Point2D(0, 0));
		}
	}

	Special.Write_INI(ini);

	ini.Put_String(BASIC, "NextScenario", NextScenarioName);
	ini.Put_String(BASIC, "AltNextScenario", AltNextScenarioName);
	ini.Put_UnicodeString(BASIC, "Name", Description);
	ini.Put_Int(BASIC, "NewINIFormat", 4);
	ini.Put_Int(BASIC, "CarryOverCap", CarryOverCap / 100);
	ini.Put_Bool(BASIC, "EndOfGame", IsEndOfGame);
	ini.Put_Bool(BASIC, "SkipScore", IsSkipScore);
	ini.Put_Bool(BASIC, "OneTimeOnly", IsOneTimeOnly);
	ini.Put_Bool(BASIC, "SkipMapSelect", IsNoMapSel);
	ini.Put_Bool(BASIC, "Official", true);
	ini.Put_Bool(BASIC, "IgnoreGlobalAITriggers", IsIgnoreGlobalAITriggers);
	ini.Put_Bool(BASIC, "TruckCrate", IsTruckCrate);
	ini.Put_Bool(BASIC, "TrainCrate", IsTrainCargo);
	ini.Put_Int(BASIC, "Percent", Percent);
	ini.Put_TechnoType_List(BASIC, "AllowableUnits", AllowableUnits);
	ini.Put_IntList(BASIC, "AllowableUnitMaximums", AllowableUnitMaximums);
	ini.Put_ThemeType(BASIC, "Theme", TransitTheme);

	if (!mplayer) {
		char player[32];
		strcpy(player, PlayerPtr->IniName);
		ini.Put_String(BASIC, "Player", player);
		ini.Put_VQType(BASIC, "Intro", IntroMovie);
		ini.Put_VQType(BASIC, "Brief", BriefMovie);
		ini.Put_VQType(BASIC, "Win", WinMovie);
		ini.Put_VQType(BASIC, "Lose", LoseMovie);
		ini.Put_VQType(BASIC, "Action", ActionMovie);
		ini.Put_VQType(BASIC, "PostScore", PostScoreMovie);
		ini.Put_VQType(BASIC, "PreMapSelect", PreMapSelectMovie);
		ini.Put_Float(BASIC, "CarryOverMoney", CarryOverPercent);
		ini.Put_Bool(BASIC, "TimerInherit", IsInheritTimer);
		ini.Put_Bool(BASIC, "FillSilos", IsMoneyTiberium);
		ini.Put_Int(BASIC, "StartingDropships", StartingDropships);
		ini.Put_Int(BASIC, "HomeCell", Home);
		ini.Put_Int(BASIC, "AltHomeCell", AltHome);
	}

	ini.Put_Int(BASIC, "MultiplayerOnly", IsMultiplayerOnly);
	ini.Put_Bool(BASIC, "TiberiumGrowthEnabled", IsTibGrowth);
	ini.Put_Bool(BASIC, "VeinGrowthEnabled", IsVeinGrowth);
	ini.Put_Bool(BASIC, "IceGrowthEnabled", IsIceGrowth);
	ini.Put_Bool(BASIC, "TiberiumDeathToVisceroid", IsTiberiumDeathToVisceroid);
	ini.Put_Bool(BASIC, "FreeRadar", IsFreeRadar);

	ini.Put_Int(BASIC, "InitTime", InitTime);

	Write_Local_INI(ini);

	char const * const RANKING = "Ranking";
	ini.WriteTime(RANKING, "ParTimeEasy", ParTimeEasy);
	ini.WriteTime(RANKING, "ParTimeMedium", ParTimeMedium);
	ini.WriteTime(RANKING, "ParTimeHard", ParTimeDifficult);
	ini.Put_String(RANKING, "UnderParTitle", UnderParTitle);
	ini.Put_String(RANKING, "UnderParMessage", UnderParMessage);
	ini.Put_String(RANKING, "OverParTitle", OverParTitle);
	ini.Put_String(RANKING, "OverParMessage", OverParMessage);

	char const * const LIGHTING = "Lighting";
	ini.Clear(LIGHTING);
	ini.Put_Float(LIGHTING, "Ambient", ((double)AmbientLight) / 100.0);
	ini.Put_Float(LIGHTING, "Red", ((double)NormalLighting.RedTint) / 100.0);
	ini.Put_Float(LIGHTING, "Green", ((double)NormalLighting.GreenTint) / 100.0);
	ini.Put_Float(LIGHTING, "Blue", ((double)NormalLighting.BlueTint) / 100.0);
	ini.Put_Float(LIGHTING, "Ground", ((double)NormalLighting.GroundLight) / 1000.0);
	ini.Put_Float(LIGHTING, "Level", ((double)NormalLighting.LevelLight) / 1000.0);
	ini.Put_Float(LIGHTING, "IonAmbient", ((double)IonAmbientLight) / 100.0);
	ini.Put_Float(LIGHTING, "IonRed", ((double)IonLighting.RedTint) / 100.0);
	ini.Put_Float(LIGHTING, "IonGreen", ((double)IonLighting.GreenTint) / 100.0);
	ini.Put_Float(LIGHTING, "IonBlue", ((double)IonLighting.BlueTint) / 100.0);
	ini.Put_Float(LIGHTING, "IonGround", ((double)IonLighting.GroundLight) / 1000.0);
	ini.Put_Float(LIGHTING, "IonLevel", ((double)IonLighting.LevelLight) / 1000.0);
	ini.Put_Float(LIGHTING, "DominatorAmbient", ((double)DominatorAmbient) / 100.0);
	ini.Put_Float(LIGHTING, "DominatorRed", ((double)DominatorLighting.RedTint) / 100.0);
	ini.Put_Float(LIGHTING, "DominatorGreen", ((double)DominatorLighting.GreenTint) / 100.0);
	ini.Put_Float(LIGHTING, "DominatorBlue", ((double)DominatorLighting.BlueTint) / 100.0);
	ini.Put_Float(LIGHTING, "DominatorGround", ((double)DominatorLighting.GroundLight) / 1000.0);
	ini.Put_Float(LIGHTING, "DominatorLevel", ((double)DominatorLighting.LevelLight) / 1000.0);
	ini.Put_Float(LIGHTING, "DominatorAmbientChangeRate", ((double)DominatorAmbientChangeRate) / 1000.0);
	return(true);
}

// FUNCTION: GAMEMD 0x0068B8A0
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

// FUNCTION: GAMEMD 0x0068B940
void ScenarioClass::Compute_CRC(CRCEngine & crc) const
{
	crc(UIName);
	crc(IntroMovie);
	crc(BriefMovie);
	crc(WinMovie);
	crc(LoseMovie);
	crc(ActionMovie);
	crc(PostScoreMovie);
	crc(PreMapSelectMovie);
	crc(PlayerHouse);
	crc(Percent);
	crc(TransitTheme);
	crc(CarryOverPercent);
	crc(IsMultiplayerOnly);
	crc(IsInheritTimer);
	crc(CarryOverCap / 100);
	crc(BridgeCount);
	crc(IsEndOfGame);
	crc(Campaign);
	crc(IsSkipScore);
	crc(IsOneTimeOnly);
	crc(IsNoMapSel);
	crc(IsMoneyTiberium);
	crc(IsIgnoreGlobalAITriggers);
	crc(IsTruckCrate);
	crc(Percent);
	crc(StartingDropships);
	crc((int)MissionTimer);
	crc((int)ShroudTimer);
	crc((int)FogTimer);
	crc((int)IceGrowthTimer);
	crc((int)VeinGrowthTimer);
	crc((int)ElapsedTimer);
	crc(Difficulty);
	crc(CDifficulty);
	crc(Theater);
	crc(NormalLighting.GroundLight);
	crc(NormalLighting.LevelLight);
	crc(IsFreeRadar);
	crc(IonAmbientLight);
	crc(IonLighting.RedTint);
	crc(IonLighting.GreenTint);
	crc(IonLighting.BlueTint);
	crc(IonLighting.GroundLight);
	crc(IonLighting.LevelLight);
	crc(AmbientLight);
	crc(CurrentAmbientLight);
	crc(DesiredAmbientLight);
}

// FUNCTION: GAMEMD 0x0068BCB0
int ScenarioClass::Get_Unique_ID(void)
{
	UniqueID++;

	return(UniqueID);
}

// FUNCTION: GAMEMD 0x0068BCC0
Cell ScenarioClass::Get_Waypoint_Cell(WAYPOINT waypoint) const
{
	return(Waypoint[waypoint]);
}

// FUNCTION: GAMEMD 0x0068BCE0
AbstractClass * ScenarioClass::Get_Waypoint_Target(WAYPOINT waypoint) const
{
	return(&Map[Waypoint[waypoint]]);
}

// FUNCTION: GAMEMD 0x0068BD00
Coord ScenarioClass::Get_Waypoint_Coord(WAYPOINT waypoint) const
{
	CellClass const * cell = &Map[Waypoint[waypoint]];
	Coord coord = cell->Center_Coord();

	if (cell->IsUnderBridge || cell->WasUnderBridge) {
		coord.Z += BRIDGE_LEPTON_HEIGHT;
	}
	return(coord);
}

// FUNCTION: GAMEMD 0x0068BD60
void ScenarioClass::Clear_All_Waypoints(void)
{
	for (int index = 0; index < ARRAY_SIZE(Waypoint); index++) {
		Waypoint[index] = CELL_NONE;
	}
}

// FUNCTION: GAMEMD 0x0068BD80
bool ScenarioClass::Is_Valid_Waypoint(WAYPOINT waypoint) const
{
	if (waypoint < ARRAY_SIZE(Waypoint) &&
		waypoint >= 0 &&
		Waypoint[waypoint] != CELL_NONE) {

		return(true);
	}
	return(false);
}
