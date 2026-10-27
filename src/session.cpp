// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include "geometry.h"
#include "session.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "ccini.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "phone.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "special.h"
#include "wonline.h"
#include "stringtable.h"
#include "multiplayer.h"
#include "xstraw.h"
#include <ctime> // for station ID computation
#include <dos.h> // for station ID computation
#include <math.h>

MultiplayerGameMode * First_Game_Mode(void);
extern CCINIClass DAT_008870C0;
extern signed char PlayerColorToColorSchemeLUT[9];
extern char DAT_00889F64[];
void __fastcall FUN_00735060(wchar_t * dest, char const * source, int length);
char * __fastcall FUN_007350C0(wchar_t const * text);
void __cdecl FUN_004068E0(char const * format, ...);
void __fastcall FUN_00684240(void);
void __fastcall FUN_00407150(void * p, int value);
extern int DAT_00B73E44[0x800];
extern int DAT_00B76338[0x40];
extern void * DAT_0087E758;
extern int DAT_0083ED20;
extern int DAT_00B07784;
namespace Game { extern int PlayerColor; extern bool IsActive; }
namespace Unsorted { extern int CurrentFrame; }

// SYNTHETIC: GAMEMD 0x00696D00 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x00696D20 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x00696D50 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00696D70 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00696D90 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00696DB0 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00696DD0 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x00696DF0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x00696E30 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00696E60 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00696E90 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00696EC0 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00696ED0 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00696EF0 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x00696F10
int NodeNameType::Get_Country(void)
{
	return (Slot.InitialCountry == -2) ? -2 : Slot.Country;
}

// FUNCTION: GAMEMD 0x00696F20
int NodeNameType::Get_Color(void)
{
	return (Slot.InitialColor == -2) ? -2 : Slot.ColorIdx;
}

// FUNCTION: GAMEMD 0x00696F30
int NodeNameType::Get_Color_Or_Random(void)
{
	if (Slot.InitialColor != -2 || Slot.ColorIdx != -1) {
		return Slot.ColorIdx;
	}
	return -2;
}

// FUNCTION: GAMEMD 0x00696F50
int NodeNameType::Get_Start(void)
{
	if (Slot.InitialStart != -2 || Slot.Start != -1) {
		return Slot.Start;
	}
	return -2;
}

// FUNCTION: GAMEMD 0x00696F70
int NodeNameType::Get_Team(void)
{
	if (Slot.InitialTeam != -2 || Slot.Team != -1) {
		return Slot.Team;
	}
	return -2;
}

// FUNCTION: GAMEMD 0x00696F90
void NodeNameType::Set_Country(int value)
{
	if (value == -2) {
		Slot.Country = 0;
		Slot.InitialCountry = value;
	} else {
		Slot.Country = value;
		Slot.InitialCountry = -1;
	}
}

// FUNCTION: GAMEMD 0x00696FC0
void NodeNameType::Set_Color(int value)
{
	if (value == -2) {
		Slot.ColorIdx = -1;
		Slot.InitialColor = value;
	} else {
		Slot.ColorIdx = value;
		Slot.InitialColor = -1;
	}
}

// FUNCTION: GAMEMD 0x00696FF0
void NodeNameType::Set_Start(int value)
{
	if (value == -2) {
		Slot.Start = -1;
		Slot.InitialStart = value;
	} else {
		Slot.Start = value;
		Slot.InitialStart = -1;
	}
}

// FUNCTION: GAMEMD 0x00697020
void NodeNameType::Set_Team(int value)
{
	if (value == -2) {
		Slot.Team = -1;
		Slot.InitialTeam = value;
	} else {
		Slot.Team = value;
		Slot.InitialTeam = -1;
	}
}

// FUNCTION: GAMEMD 0x00697050
SessionOptionsClass::SessionOptionsClass(void)
{
	MPGameMode = 0;
	ScenIndex = 0;
	GameSpeed = 1;
	Credits = 10000;
	UnitCount = 10;
	ShortGame = true;
	SuperWeaponsAllowed = true;
	BuildOffAlly = true;
	MCVRepacks = true;
	CratesAppear = true;
	for (int i = 0; i < 8; i++) {
		SlotData[i][0] = 0;
		SlotData[i][1] = 0;
		SlotData[i][2] = 0;
	}
}

// FUNCTION: GAMEMD 0x006970A0
SessionClass::SessionClass(void) :
	MPGameMode(0)
{
	Type = GAME_NORMAL;
	CommProtocol = DEFAULT_COMM_PROTOCOL;

	Options.ScenarioIndex = 0;
	Options.Bases = true;
	Options.Credits = 0;
	Options.BridgeDestruction = false;
	Options.Goodies = false;
	Options.Ghosts = false;
	Options.UnitCount = 0;
	Options.ShortGame = false;
	Options.SWAllowed = false;
	Options.BuildOffAlly = false;
	Options.GameSpeed = 0;
	Options.CrapEngineers = false;
	Options.AlliesAllowed = false;
	Options.HarvTruce = false;
	Options.CTF = false;
	Options.FogOfWar = false;
	Options.MCVRedeploy = false;
	Options.AIDifficulty = DIFF_NORMAL;

	UniqueID = 0;
	Handle[0] = 0;
	House = 0;
	idxSide2 = -1;
	PrefColor = 2;
	ColorIdx = -1;
	Side = House;
	Side2 = idxSide2;
	Color = PrefColor;
	Color2 = ColorIdx;

	WOLTaunts = true;
	LANTaunts = true;
	WOLScrollText = true;
	LANScrollText = true;

	for (int p = 0; p < 8; p++) {
		PlayerPorts[p] = 0;
	}
	unknown_3078 = false;
	unknown_306C = false;
	unknown_306E = 0;
	unknown_3070 = 0;
	PortBase = 1250;
	ForcePortBase = 0;
	unknown_3018 = true;
	unknown_2F48 = false;
	unknown_301C = 0;
	memset(DAT_00B73E44, 0, sizeof(DAT_00B73E44));
	memset(DAT_00B76338, 0, sizeof(DAT_00B76338));

	MultiplayerObserver = 0;
	Unknown_304 = 0;
	LastNicknameSlot = -1;
	WOLLimitResolution = false;
	ViaPacketRouter = false;
	MasterPlayerID = -1;
	memset(MasterPlayerName, 0, sizeof(MasterPlayerName));
	unknown_308C = 0;
	unknown_3090 = true;
	unknown_3094 = -1;
	unknown_3098 = false;
	unknown_309C = 20;
	unknown_30A0 = 0;
	unknown_30A4 = 0;
	unknown_30A8 = -1;
	*(unsigned short *)unknown_2984 = 0;

	SquadAlliances = false;
	SawGameCompletion = false;
	OutOfSync = false;
	MaxPlayers = 8;
	NumPlayers = 0;
	FrameSendRate = DEFAULT_FRAME_SEND_RATE;
	MaxAhead = FrameSendRate * 3;
	MaxMaxAhead = MaxAhead;
	memset(ConnectionStats, 0, sizeof(ConnectionStats));
	PrecalcMaxAhead = 0;
	PrecalcDesiredFrameRate = 0;
	EnableMultiplayerDebug = false;
	DrawMPDebugStats = false;
	LoadGame = 0;
	EmergencySave = 0;
	LastMessage[0] = 0;
	WWChat = 0;

	RecordFile.Set_Name("RECORD.BIN");
	Record = 0;
	Play = 0;
	Attract = 0;

	IsBridge = 0;
	NetStealth = false;
	NetProtect = true;
	NetOpen = false;
	GameName[0] = 0;
	GProductID = 0;
	Suspended = 0;
	LatencyFudge = 1;
	MetaSize = MAX_IPX_PACKET_SIZE;

	memset(KickVoteCount, 0, sizeof(KickVoteCount));
	memset(KickVoteWho, -1, sizeof(KickVoteWho));

	ModemService = true;
	CurPhoneIdx = 0;
	SerialDefaults.Port = 0x2f8;
	SerialDefaults.IRQ = 3;
	SerialDefaults.Baud = 9600;
	SerialDefaults.DialMethod = DIAL_TOUCH_TONE;
	SerialDefaults.InitStringIndex = -1;
	SerialDefaults.CallWaitStringIndex = 0;
	strcpy(SerialDefaults.CallWaitString, DAT_00889F64);
	ModemType = MODEM_NULL_HOST;

	TrapFrame = 0x7fffffff;
	TrapObjType = RTTI_NONE;
	TrapObject.Ptr.All = NULL;
	TrapCoord = COORD_NONE;
	TrapTarget.Invalidate();
	TrapCell = NULL;
	TrapCheckHeap = 0;
	TrapPrintCRC = 0;

	unknown_2878 = false;
	Options.AIPlayers = 0;
	for (int i = 0; i < 8; i++) {
		Options.AISlots.Countries[i] = -1;
		Options.AISlots.Colors[i] = -1;
	}
	unknown_0C = 0;
	unknown_10 = 0;
	ScenarioFileName[0] = 0;
	ScenarioFileLength = 0;
	ScenarioIsOfficial = false;
	unknown_8D4 = 0;
}

// FUNCTION: GAMEMD 0x006977C0
void SessionClass::Load_Call_Wait_Strings(void)
{
	for (int i = 0; i < CALL_WAIT_STRINGS_NUM; i++) {
		CallWaitStrings.Add(_wcsdup(StringTable::LoadString(CallWaitStringIDs[i], NULL, "D:\\ra2mdpost\\Session.CPP", 593)));
	}
}

// FUNCTION: GAMEMD 0x00697840
SessionClass::~SessionClass(void)
{
	while (InitStrings.Count()) {
		delete[] InitStrings[0];
		InitStrings.Delete_Index(0);
	}

	while (CallWaitStrings.Count()) {
		free((void *)CallWaitStrings[0]);
		CallWaitStrings.Delete_Index(0);
	}
}

// FUNCTION: GAMEMD 0x00697AD0
void SessionClass::One_Time(void)
{
	Read_Scenario_Descriptions();

	UniqueID = Compute_Unique_ID();
	DebugString("Session one time init. UniqueID is %08x\n", UniqueID);
}

// FUNCTION: GAMEMD 0x00697B60
void SessionClass::Init(void)
{
}

struct Unknown_00B052D0 {
	void FUN_00663B40(void);
	bool FUN_00663DB0(int);
	bool FUN_006639D0(void);
};
extern Unknown_00B052D0 DAT_00B052D0;

// FUNCTION: GAMEMD 0x00697B70
int SessionClass::Create_Connections(void)
{
	int i;
	NetNumType net;
	NetNodeType node;

	DebugString("Entering Create_Connections\n");

	if (Session.Type != GAME_IPX && Session.Type != GAME_INTERNET) {
		return(0);
	}

	for (i = 0; i < Players.Count(); i++) {
		DebugString("Player %d, Name: %s, ID %d\n", i, Players[i]->Name, Players[i]->Player.ID);

		if (!wcsicmp(Players[i]->Name, Houses[Players[i]->Player.ID]->UIName)) {
			if (wcsicmp(Players[i]->Name, MasterPlayerName) == 0) {
				MasterPlayerID = Players[i]->Player.ID;
				Session.Players[i]->Address.Get_Address(net, node);
				Session.unknown_296C.Set_Address(net, node, 0);
				Ipx.FUN_00543050(&Session.unknown_296C);
			}

			Houses[Players[i]->Player.ID]->SquadID = Players[i]->Player.SquadID;

			Session.Players[i]->Address.Get_Address(net, node);

			unsigned int ip = 0;
			for (int k = 0; k < 4; k++) {
				ip <<= 8;
				ip |= node[k];
			}

			DebugString("House[%d] IP = %X  Clan=%d\n", Players[i]->Player.ID, ip, Players[i]->Player.SquadID);

			Houses[Players[i]->Player.ID]->IPAddress = ip;
			Houses[Players[i]->Player.ID]->LostConnection = false;

			if (i > 0) {
				Ipx.Create_Connection((int)Players[i]->Player.ID, Players[i]->Name, &(Players[i]->Address));
				Players[i]->Player.ProcessTime = -1;
			}
		} else {
			return(0);
		}
	}

	for (i = 0; i < 8; i++) {
		Session.unknown_304C[i] = TickCount;
	}

	if (Type == GAME_INTERNET && unknown_3090) {
		NetNumType zero_net;
		NetNodeType zero_node;
		zero_net[0] = 0;
		zero_net[1] = 0;
		zero_net[2] = 0;
		zero_net[3] = 0;
		zero_node[0] = 0;
		zero_node[1] = 0;
		zero_node[2] = 0;
		zero_node[3] = 0;
		zero_node[4] = 0;
		zero_node[5] = 0;
		Session.unknown_2978.Set_Address(zero_net, zero_node, 0);
		ViaPacketRouter = true;
		unknown_308C = 0;
		unknown_309C = 20;
		unknown_3094 = -1;
		Ipx.Create_Multicast_Connection();
		DAT_00B052D0.FUN_00663B40();
		if (Session.Am_I_Master(NULL)) {
			bool r = DAT_00B052D0.FUN_00663DB0(0);
			if (r) r = DAT_00B052D0.FUN_006639D0();
			if (!r) {
				Session.ViaPacketRouter = false;
			}
		}
	} else {
		Session.ViaPacketRouter = false;
	}

	DebugString("Session.ViaPacketRouter = %s\n", Session.ViaPacketRouter ? "true" : "false");
	DebugString("Leaving Create_Connections\n");
	return(1);
}

// FUNCTION: GAMEMD 0x00697E70
bool SessionClass::Am_I_Master(HouseClass * who)
{
	int i;
	HouseClass * hptr;

	if (who == NULL) {
		who = PlayerPtr;
	}

	if (Session.Type == GAME_INTERNET && who) {
		if (MasterPlayerID != -1) {
			return(who->HeapID == MasterPlayerID);
		}

		if (wcsicmp(who->UIName, MasterPlayerName) == 0) {
			return(true);
		}
	}

	for (i = 0; i < Houses.Count(); i++) {
		hptr = Houses[i];
		if (hptr->IsHuman && !hptr->IsDefeated) {
			if (who == hptr) {
				return(true);
			} else {
				return(false);
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x00697F10
void SessionOptionsClass::Read(CCINIClass & ini, char const * section, int default_slot, int default_first_slot)
{
	char entry[16];

	ini.Reset();
	int mode = First_Game_Mode()->MPModeIndex;
	MPGameMode = ini.Get_Int(section, "GameMode", mode);
	ScenIndex = ini.Get_Int(section, "ScenIndex", 0);
	GameSpeed = ini.Get_Int(section, "GameSpeed", Rule->GameSpeed);
	Credits = ini.Get_Int(section, "Credits", Rule->MPMoney);
	UnitCount = ini.Get_Int(section, "UnitCount", Rule->MPUnitCount);
	ShortGame = ini.Get_Bool(section, "ShortGame", Rule->ShortGame);
	SuperWeaponsAllowed = ini.Get_Bool(section, "SuperWeaponsAllowed", Rule->SuperWeaponsAllowed);
	BuildOffAlly = ini.Get_Bool(section, "BuildOffAlly", Rule->BuildOffAlly);
	MCVRepacks = ini.Get_Bool(section, "MCVRepacks", Rule->MCVRedeploys);
	CratesAppear = ini.Get_Bool(section, "CratesAppear", Rule->IsMPCrates);

	for (int i = 1; i < 8; i++) {
		sprintf(entry, "Slot%02d", i);
		int first = (i == 1) ? default_first_slot : default_slot;
		int second = -2;
		int third = -2;
		ini.Get_Int3(section, entry, &first, &second, &third);
		SlotData[i][0] = first;
		SlotData[i][1] = second;
		SlotData[i][2] = third;
	}
}

// FUNCTION: GAMEMD 0x00698CB0
bool SessionClass::Log_To_File(FILE * out)
{
	int i;
	int j;
	NetNumType net;
	NetNodeType node;

	if (Session.Options.ScenarioIndex < 0) {
		fprintf(out, "BOGUS scenario index!?!\n");
		Session.Options.ScenarioIndex = 0;
	}

	fprintf(out, "Scenario = %s\n", Session.Scenarios[Session.Options.ScenarioIndex]->Description());
	fprintf(out, "Handle = %s\n", Handle);

	for (i = 0; i < NumPlayers; i++) {
		Session.Players[i]->Address.Get_Address(net, node);
		fprintf(out, "Player Addr[%d] = ", i);
		for (j = 0; j < sizeof(NetNumType); j++) {
			fprintf(out, "%02X", net[j]);
		}
		fprintf(out, ":");
		for (j = 0; j < sizeof(NetNodeType); j++) {
			fprintf(out, "%02X", node[j]);
		}
		fprintf(out, "\n");
	}
	fprintf(out, "\n");

	Session.HostAddress.Get_Address(net, node);
	fprintf(out, "Address = ");
	for (i = 0; i < sizeof(NetNumType); i++) {
		fprintf(out, "%02X", net[i]);
	}

	fprintf(out, ":");
	for (i = 0; i < sizeof(NetNodeType); i++) {
		fprintf(out, "%02X", node[i]);
	}

	fprintf(out, "\n\n");
	fprintf(out, "MaxAhead = %d\n", MaxAhead);
	fprintf(out, "LoadGame = %d\n", LoadGame);
	fprintf(out, "PrefColor = %d\n", Color);
	fprintf(out, "ColorIdx = %d\n", PrefColor);
	fprintf(out, "House = %d\n", House);
	fprintf(out, "NumPlayers = %d\n", NumPlayers);
	fprintf(out, "Options.Bases = %d\n", Options.Bases);
	fprintf(out, "Options.Credits = %d\n", Options.Credits);
	fprintf(out, "Options.BridgeDestruction = %d\n", Options.BridgeDestruction);
	fprintf(out, "Options.Goodies = %d\n", Options.Goodies);
	fprintf(out, "Options.Ghosts = %d\n", Options.Ghosts);
	fprintf(out, "Options.UnitCount = %d\n", Options.UnitCount);
	fprintf(out, "Options.AIPlayers = %d\n", Options.AIPlayers);
	fprintf(out, "Options.AIDifficulty = %d\n", Options.AIDifficulty);
	fprintf(out, "ObiWan = %d\n", MultiplayerObserver);

	if (Session.Type == GAME_IPX) {
		fprintf(out, "Type = IPX\n");
	} else if (Session.Type == GAME_INTERNET) {
		fprintf(out, "Type = INTERNET\n");
	} else {
		fprintf(out, "Type = !!Unknown!!\n");
	}

	return(true);
}

// FUNCTION: GAMEMD 0x00698F90
void SessionOptionsClass::Write(CCINIClass & ini, char const * section)
{
	char entry[16];

	ini.Put_Int(section, "GameMode", MPGameMode);
	ini.Put_Int(section, "ScenIndex", ScenIndex);
	ini.Put_Int(section, "GameSpeed", GameSpeed);
	ini.Put_Int(section, "Credits", Credits);
	ini.Put_Int(section, "UnitCount", UnitCount);
	ini.Put_Bool(section, "ShortGame", ShortGame);
	ini.Put_Bool(section, "SuperWeaponsAllowed", SuperWeaponsAllowed);
	ini.Put_Bool(section, "BuildOffAlly", BuildOffAlly);
	ini.Put_Bool(section, "MCVRepacks", MCVRepacks);
	ini.Put_Bool(section, "CratesAppear", CratesAppear);

	for (int i = 1; i < 8; i++) {
		sprintf(entry, "Slot%02d", i);
		ini.Put_Int3(section, entry, SlotData[i][0], SlotData[i][1], SlotData[i][2]);
	}
}

// FUNCTION: GAMEMD 0x006990A0
void SessionClass::Write_MultiPlayer_Settings(void)
{
	RawFileClass file("RA2MD.INI");

	DAT_008870C0.Put_Int("MultiPlayer", "PhoneIndex", CurPhoneIdx);
	DAT_008870C0.Put_Bool("MultiPlayer", "WOLLimitResolution", WOLLimitResolution);
	DAT_008870C0.Put_Int("MultiPlayer", "LastNickSlot", LastNicknameSlot);
	DAT_008870C0.Put_UnicodeString("MultiPlayer", "Handle", FUN_00735120(Handle));
	DAT_008870C0.Put_Int("MultiPlayer", "Color", Color);
	DAT_008870C0.Put_Int("MultiPlayer", "ColorEx", Color2);
	DAT_008870C0.Put_HousesType("MultiPlayer", "Side", (HousesType)Side);
	DAT_008870C0.Put_Int("MultiPlayer", "SideEx", Side2);
	if (MPGameMode) {
		DAT_008870C0.Put_Int("MultiPlayer", "GameMode", MPGameMode->MPModeIndex);
	} else {
		DAT_008870C0.Put_Int("MultiPlayer", "GameMode", 0);
	}

	Skirmish.Write(DAT_008870C0, "Skirmish");
	LAN.Write(DAT_008870C0, "LAN");
	WOL.Write(DAT_008870C0, "WonlinePref");

	DAT_008870C0.Clear("SerialDefaults");
	DAT_008870C0.Put_String("SerialDefaults", "CallWaitString", SerialDefaults.CallWaitString);
	DAT_008870C0.Put_Int("SerialDefaults", "CallWaitStringIndex", SerialDefaults.CallWaitStringIndex);
	DAT_008870C0.Put_Int("SerialDefaults", "InitStringIndex", SerialDefaults.InitStringIndex);
	DAT_008870C0.Put_String("SerialDefaults", "DialMethod", DialMethodCheck[SerialDefaults.DialMethod]);
	DAT_008870C0.Put_Int("SerialDefaults", "Baud", SerialDefaults.Baud);
	DAT_008870C0.Put_Int("SerialDefaults", "IRQ", SerialDefaults.IRQ);
	DAT_008870C0.Put_Int("SerialDefaults", "Port", SerialDefaults.Port, 1);
	DAT_008870C0.Put_Int("SerialDefaults", "Compression", SerialDefaults.Compression);
	DAT_008870C0.Put_Int("SerialDefaults", "ErrorCorrection", SerialDefaults.ErrorCorrection);
	DAT_008870C0.Put_String("SerialDefaults", "ModemName", SerialDefaults.ModemName);

	DAT_008870C0.Clear("InitStrings");

	for (int index = 0; index < InitStrings.Count(); index++) {
		char buf[10];
		wsprintf(buf, "%03d", index);
		DAT_008870C0.Put_String("InitStrings", buf, InitStrings[index]);
	}

	DAT_008870C0.Clear("PhoneBook");

	for (int i = (PhoneBook.Count() - 1); i >= 0; i--) {
		char buf[128];
		char entrytext[10];
		wsprintf(buf, "%s|%s|%x|%d|%d|%d|%d|%s|%d|%d|%s",
			PhoneBook[i]->Name,
			PhoneBook[i]->Number,
			PhoneBook[i]->Settings.Port,
			PhoneBook[i]->Settings.IRQ,
			PhoneBook[i]->Settings.Baud,
			PhoneBook[i]->Settings.Compression,
			PhoneBook[i]->Settings.ErrorCorrection,
			DialMethodCheck[PhoneBook[i]->Settings.DialMethod],
			PhoneBook[i]->Settings.InitStringIndex,
			PhoneBook[i]->Settings.CallWaitStringIndex,
			PhoneBook[i]->Settings.CallWaitString);
		wsprintf(entrytext, "%03d", i);
		DAT_008870C0.Put_String("PhoneBook", entrytext, buf);
	}

	DAT_008870C0.Save(file, false);
}

// FUNCTION: GAMEMD 0x00699450
void SessionClass::Write_Chat_Settings(void)
{
	RawFileClass file("RA2MD.INI");

	DAT_008870C0.Put_Bool("MultiPlayer", "WOLTaunts", WOLTaunts);
	DAT_008870C0.Put_Bool("MultiPlayer", "LANTaunts", LANTaunts);
	DAT_008870C0.Put_Bool("MultiPlayer", "WOLScrollText", WOLScrollText);
	DAT_008870C0.Put_Bool("MultiPlayer", "LANScrollText", LANScrollText);
	DAT_008870C0.Save(file, false);
}

// FUNCTION: GAMEMD 0x0069A1F0
void SessionClass::Free_Scenario_Descriptions(void)
{
	int i;

	for (int index = 0; index < Scenarios.Count(); index++) {
		delete Scenarios[index];
	}
	Scenarios.Clear();

	for (i = 0; i < InitStrings.Count(); i++) {
		delete [] InitStrings[i];
	}
	InitStrings.Clear();

	for (i = 0; i < PhoneBook.Count(); i++) {
		if (PhoneBook[i] != NULL) {
			delete PhoneBook[i];
		}
	}
	PhoneBook.Clear();
}

// FUNCTION: GAMEMD 0x0069A300
void FUN_0069A300(void)
{
}

// FUNCTION: GAMEMD 0x0069A310
int SessionClass::Color_Index_To_Scheme(int id)
{
	if (id == -2) {
		id = 8;
	}
	if ((unsigned)id < 9) {
		return PlayerColorToColorSchemeLUT[id];
	}
	return id;
}

// FUNCTION: GAMEMD 0x0069A340
unsigned int SessionClass::Compute_Unique_ID(void)
{
	unsigned int id;
	char *path;
	unsigned i;

	id = timeGetTime();

	unsigned int space_available = Disk_Space_Available();
	Add_CRC(&id, space_available);

	path = getenv("PATH");
	if (path) {
		for (i = 0; i < strlen(path); i++) {
			Add_CRC(&id, (unsigned int)path[i]);
		}
	}

	return(id);
}

// FUNCTION: GAMEMD 0x0069A3B0
MultiMission::MultiMission(INIClass const & ini, char const * name)
{
	char buffer[128];
	char *token;

	ScenarioDescription[0] = L'\0';
	Filename[sizeof(Filename) - 1] = '\0';
	Set_Digest(NULL);
	IsOfficial = true;
	MinPlayers = 2;
	MaxPlayers = 4;
	Disks.Clear();

	if (name != NULL) {

		strcpy(Filename, name);
		strcat(Filename, ".MAP");

		if (ini.Is_Present(name, "DescriptionText")) {
			char text[ARRAY_SIZE(ScenarioDescription)];
			ini.Get_String(name, "DescriptionText", DAT_00889F64, text, sizeof(text));
			FUN_00735060(ScenarioDescription, text, -1);
		} else {
			ini.ReadStringtableEntry(name, "Description", ScenarioDescription, ARRAY_SIZE(ScenarioDescription));
		}

		MinPlayers = ini.Get_Int(name, "MinPlayers", MinPlayers);
		MaxPlayers = ini.Get_Int(name, "MaxPlayers", MaxPlayers);

		if (ini.Get_String(name, "CD", DAT_00889F64, buffer, sizeof(buffer))) {
			token = strtok(buffer, ",");

			while (token != NULL && *token != '\0') {
				Disks.Add((DiskID)atoi(token));
				token = strtok(NULL, ",");
			}
		}

		Wstring modes;
		if (ini.Get_String(name, "GameMode", DAT_00889F64, modes) > 0) {
			Wstring mode;
			int pos = 0;
			while ((pos = modes.getToken(pos, ",", mode)) != -1) {
				mode.trimLeading(" ");
				mode.trimTrailing(" ");
				GameModes.Add(mode);
			}
		}

		CCFileClass file(Filename);

		if (!file.Is_Available(0)) {
			DebugString("Unable to locate scenario %s - No digest info will be loaded\n", Filename);
		}else{
			if (!file.Open(FileClass::READ)) {
				return;
			}
			char digestbuf[128];

			file.Seek(-((int)sizeof(digestbuf) - 28), SEEK_END);
			memset(digestbuf, 0, sizeof(digestbuf));
			file.Read(digestbuf, sizeof(digestbuf) - 28);
			file.Close();

			char *digest = strstr(digestbuf, "[Digest]");

			if (digest != NULL) {
				CCINIClass mapini;
				BufferStraw straw(digestbuf, sizeof(digestbuf) - 28);
				mapini.Load(straw, false, false);
				mapini.Get_String("Digest", "1", NULL, digestbuf, sizeof(digestbuf));
				Set_Digest(digestbuf);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0069A980
MultiMission::MultiMission(char const * filename, wchar_t const * description, char const * digest, bool official, char const * game_modes, int min_players, int max_players)
{
	Set_Filename(filename);
	Set_Description(description);
	Set_Digest(digest);
	IsOfficial = official;
	MinPlayers = min_players;
	MaxPlayers = max_players;
	Disks.Clear();

	Disks.Add(DISK_LOCAL);

	if (game_modes != NULL) {
		Wstring modes(game_modes);
		if (modes.length() > 0) {
			Wstring mode;
			int pos = 0;
			while ((pos = modes.getToken(pos, ",", mode)) != -1) {
				mode.trimLeading(" ");
				mode.trimTrailing(" ");
				GameModes.Add(mode);
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0069AC30
bool MultiMission::Is_On_CD(DiskID cd) const
{
	if (cd == DISK_LOCAL) {
		return RawFileClass(Filename).Is_Available();
	}
	if (Disks.Is_In_List(DISK_ANY) == true) {
		return true;
	}
	return Disks.Is_In_List(cd);
}

// FUNCTION: GAMEMD 0x0069ACC0
DiskID MultiMission::On_Which_CD(void) const
{
	return Disks[0];
}

// FUNCTION: GAMEMD 0x0069ACD0
void MultiMission::Set_Description(wchar_t const * description)
{
	if (description != NULL) {
		wcsncpy(ScenarioDescription, description, ARRAY_SIZE(ScenarioDescription));
		ScenarioDescription[ARRAY_SIZE(ScenarioDescription) - 1] = L'\0';
	} else {
#line 2845 "D:\\ra2mdpost\\Session.CPP"
		wcscpy(ScenarioDescription, StringTable::LoadString("MSG:NoDescription", NULL, __FILE__, __LINE__));
	}
}

// FUNCTION: GAMEMD 0x0069AD20
void MultiMission::Set_Filename(char const * filename)
{
	if (filename != NULL) {
		strncpy(Filename, filename, ARRAY_SIZE(Filename));
		Filename[ARRAY_SIZE(Filename) - 1] = '\0';
	} else {
		strcpy(Filename, "No File Name");
	}
}

// FUNCTION: GAMEMD 0x0069AD80
void MultiMission::Set_Digest(char const * digest)
{
	if (digest != NULL) {
		strncpy(Digest, digest, ARRAY_SIZE(Digest));
		Digest[ARRAY_SIZE(Digest) - 1] = '\0';
	} else {
		strcpy(Digest, "No Digest");
	}
}

// FUNCTION: GAMEMD 0x0069ADE0
void MultiMission::Set_Official(bool official)
{
	IsOfficial = official;
}

// FUNCTION: GAMEMD 0x0069ADF0
bool MultiMission::Is_Random_Map(void) const
{
	return stricmp(Filename, "RandMap.Sed") == 0;
}

// FUNCTION: GAMEMD 0x0069AE10
bool MultiMission::Supports_Game_Mode(Wstring & mode) const
{
	if (GameModes.Count() == 0) {
		return mode == "standard";
	}
	for (int index = 0; index < GameModes.Count(); index++) {
		if (mode == GameModes[index]) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x0069AE70
bool SessionClass::Is_Random_Map(void) const
{
	return stricmp(ScenarioFileName, "RandMap.Sed") == 0;
}

extern char DAT_00B77A04[];
extern char DAT_00B76568[];

// FUNCTION: GAMEMD 0x0069AE90
void SessionClass::Update_Progress(int percent)
{
	if (Debug_Map) {
		return;
	}

	int new_percent = percent;
	if (Scen->IsRandom) {
		new_percent = new_percent / 2;
	}

	if (Progress.Get_Current_Progress(0) * 100.0 < new_percent) {
		Progress.Set_Progress_Percent(0, new_percent);
	}

	if (Players.Count() == 1) {
		return;
	}

	CDTimerClass<SystemTimerClass> timer = 240;

	switch (Type) {
		case GAME_IPX:
		case GAME_INTERNET: {
				GlobalPacketType prog_packet;
				memset((void *)&prog_packet, 0, sizeof(prog_packet));

				prog_packet.Command = NET_PROGRESS_REPORT;
				prog_packet.Progress.Percent = int(100.0 * Progress.Get_Current_Progress(0));

				strcpy(prog_packet.Name, FUN_007350C0(Session.Players[0]->Name));

				if (prog_packet.Progress.Percent < 99.95) {
					Ipx.Send_Global_Message(&prog_packet, 0x33, 0, NULL);
				} else {
					for (int p = 1; p < Session.Players.Count(); p++) {
						DebugString("Sending guarenteed progress message to all players\n");
						Ipx.Send_Global_Message_To_Name(&prog_packet, 0x33, 1, Session.Players[p]->Name);
					}
				}

				Call_Back();

				while (Ipx.Global_Num_Send() > 5 && timer > 0) {
					Sleep(20);
					Windows_Message_Handler();
					Call_Back();
				}
			}
			break;

		case GAME_MODEM:
		case GAME_NULL_MODEM: {
				SerialPacketType send_packet;
				memset((void *)&send_packet, 0, sizeof(send_packet));

				send_packet.Command = SERIAL_PROGRESS_REPORT;
				send_packet.Progress.Percent = int(100.0 * Progress.Get_Current_Progress(0));
				send_packet.ID = Session.ModemType;

				NullModem.Send_Message(&send_packet, sizeof(send_packet), 0);
				NullModem.Service();

				while (NullModem.Num_Send() > 5 && timer > 0) {
					Sleep(20);
					Keyboard->Check();
					NullModem.Service();
				}
			}
			break;

		default:
			break;
	}

	Call_Back();
}

// FUNCTION: GAMEMD 0x0069B170
void SessionClass::Init_Fixed_Alliances(void)
{
	HouseClass * hptr;
	int i;
	int j;

	SquadAlliances = false;

	if (Type != GAME_INTERNET) {
		return;
	}

	Scen->Special.IsAllianceFixed = false;

	char * names1 = DAT_00B77A04;
	char * names2 = DAT_00B76568;

	DebugString("Squad 1 = %s\n", names1);
	DebugString("Squad 2 = %s\n", names2);

	if (strlen(names1) == 0 || strlen(names2) == 0) {
		return;
	}

	DynamicVectorClass<HouseClass *> squad1;
	DynamicVectorClass<HouseClass *> squad2;

	char * buffer = new char [MAX(strlen(names1), strlen(names2)) + 32];

	strcpy(buffer, names1);
	char * token = strtok(buffer, ",");

	while (token != NULL && *token != '\0') {
		for (i = 0; i < Houses.Count(); i++) {
			hptr = Houses[i];
			if (hptr->IsHuman && (stricmp(hptr->IniName, token) == 0)) {
				squad1.Add(hptr);
			}
		}
		token = strtok(NULL, ",");
	}

	strcpy(buffer, names2);
	token = strtok(buffer, ",");

	while (token != NULL && *token != '\0') {
		for (i = 0; i < Houses.Count(); i++) {
			hptr = Houses[i];
			if (hptr->IsHuman && (stricmp(hptr->IniName, token) == 0)) {
				squad2.Add(hptr);
			}
		}
		token = strtok(NULL, ",");
	}

	if (squad1.Count() > 1) {
		for (i = 0; i < squad1.Count(); i++) {
			for (j = 0; j < squad1.Count(); j++) {
				if (squad1[i]->HeapID != squad1[j]->HeapID) {
					squad1[i]->Make_Ally(squad1[j], false);
				}
			}
		}
	}

	if (squad2.Count() > 1) {
		for (i = 0; i < squad2.Count(); i++) {
			for (j = 0; j < squad2.Count(); j++) {
				if (squad2[i]->HeapID != squad2[j]->HeapID) {
					squad2[i]->Make_Ally(squad2[j], false);
				}
			}
		}
	}

	delete [] buffer;

	SquadAlliances = true;
	Scen->Special.IsAllianceFixed = true;
}

// FUNCTION: GAMEMD 0x0069B540
void GameOptionsType::FUN_0069B540(void)
{
	AIPlayers = 0;
	for (int index = 0; index < 8; index++) {
		AISlots.Countries[index] = -1;
		AISlots.Colors[index] = -1;
	}
}

// FUNCTION: GAMEMD 0x0069B560
bool GameOptionsType::Save(IStream * stream) const
{
	if (stream != NULL) {
		unsigned int size = sizeof(*this);
		HRESULT res = stream->Write(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			res = stream->Write(this, size, NULL);
			if (SUCCEEDED(res)) {
				return(true);
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x0069B5B0
bool GameOptionsType::Load(IStream * stream)
{
	if (stream != NULL) {
		unsigned int size;
		HRESULT res = stream->Read(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			if (size == sizeof(*this)) {
				res = stream->Read(this, size, NULL);
				ScenarioIndex = -1;
				if (SUCCEEDED(res)) {
					return(true);
				}
			}
		}
	}

	return(false);
}

// FUNCTION: GAMEMD 0x0069B600
bool SessionClass::FUN_0069B600(int color) const
{
	if (color == -2) {
		return false;
	}
	for (int index = 0; index < Players.Count(); index++) {
		if (Players[index]->Get_Color_Or_Random() == color) {
			return true;
		}
	}
	for (int slot = 0; slot < 8; slot++) {
		if (Options.AISlots.Colors[slot] == color) {
			return true;
		}
	}
	return false;
}

// FUNCTION: GAMEMD 0x0069B670
int SessionClass::FUN_0069B670(void)
{
	return Scen->RandomNumber(0, 9);
}

// FUNCTION: GAMEMD 0x0069B690
int SessionClass::FUN_0069B690(void)
{
	int color;
	do {
		color = Scen->RandomNumber(0, 7);
	} while (FUN_0069B600(color));
	return color;
}

// FUNCTION: GAMEMD 0x0069B700
int SessionClass::FUN_0069B700(void) const
{
	return (idxSide2 == -2) ? -2 : House;
}

// FUNCTION: GAMEMD 0x0069B720
int SessionClass::FUN_0069B720(void) const
{
	return (ColorIdx == -2) ? -2 : PrefColor;
}

// FUNCTION: GAMEMD 0x0069B740
int SessionClass::FUN_0069B740(void) const
{
	return (unknown_168 == -2) ? -2 : unknown_164;
}

// FUNCTION: GAMEMD 0x0069B760
void SessionClass::FUN_0069B760(int side, bool random)
{
	if (side == -2) {
		if (random) {
			Side = Scen->RandomNumber(0, 9);
		} else {
			Side = 0;
		}
		Side2 = -2;
	} else {
		Side = side;
		Side2 = -1;
	}
	House = Side;
	idxSide2 = Side2;
}

// FUNCTION: GAMEMD 0x0069B7E0
void SessionClass::FUN_0069B7E0(int color, bool random)
{
	if (color == -2) {
		if (random) {
			int pick;
			do {
				pick = Scen->RandomNumber(0, 7);
			} while (FUN_0069B600(pick));
			Color = pick;
		} else {
			Color = 0;
		}
		Color2 = -2;
	} else {
		Color = color;
		Color2 = -1;
	}
	PrefColor = Color;
	ColorIdx = Color2;
}

// FUNCTION: GAMEMD 0x0069B8B0
void __stdcall FUN_0069B8B0(int, int)
{
}

// FUNCTION: GAMEMD 0x0069BAB0
void SessionClass::Resume(void)
{
	if (!CurrentlyInGame && Game::IsActive) {
		CurrentlyInGame = true;
		if (DAT_0083ED20 == -1) {
			DAT_0083ED20 = ::Options.GameSpeed;
		}
		if (Type == GAME_NORMAL) {
			::Options.GameSpeed = 2;
		}
		if (!Scen->IsInputLocked) {
			FUN_00684240();
			DAT_00B07784 = Unsorted::CurrentFrame + 3;
		}
		if (!MouseCursor->Is_Captured()) {
			MouseCursor->Capture_Mouse();
		}
		FUN_00407150(DAT_0087E758, 0x4000);
	}
}
