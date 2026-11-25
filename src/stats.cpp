// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).


#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "playerprofile.h"
#include "rcptr.h"
#include "stimer.h"
#include "timer.h"
#include "wolapi\wolapi.h"
#include "arraylist.h"
#include <string.h>
#include <new.h>

// SYNTHETIC: GAMEMD 0x006C68C0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x006C68F0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x006C6910 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x006C6930 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x006C6950 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x006C6970 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x006C6990 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x006C69D0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x006C6A00 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x006C6A30 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x006C6A60 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x006C6A70 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x006C6A90 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x006C6AB0 SYMBOL
// _$E41

bool GameTimerInUse;
int GameEndTime;

struct PacketEntry {
	int Kind;
	int Size;
	char Data[0x8000];

	PacketEntry(void) : Kind(0), Size(0) {}
	~PacketEntry(void) {}
	PacketEntry & operator=(PacketEntry const & rhs);
};

// FUNCTION: GAMEMD 0x006C6AD0
BasicTimerClass<SystemTimerClass> GameTimer;

// SYNTHETIC: GAMEMD 0x006C6AF0 SYMBOL
// _$E44

// FUNCTION: GAMEMD 0x006C6B00
PacketEntry & PacketEntry::operator=(PacketEntry const & rhs)
{
	Kind = rhs.Kind;
	Size = rhs.Size;
	if (Size > 0x8000) {
		Size = 0;
		return(*this);
	}
	memcpy(Data, rhs.Data, Size);
	return(*this);
}

// FUNCTION: GAMEMD 0x006C6B40
// FUNCTION: GAMEMD 0x006C6B60
static ArrayList<PacketEntry> Queue;

// FUNCTION: GAMEMD 0x006C6B80
void __fastcall Queue_Packet(void *buf, int size)
{
	if (size > 0x8000 || buf == NULL || size < 1) {
		return;
	}
	PacketEntry entry;
	entry.Size = size;
	memcpy(entry.Data, buf, size);
	Queue.addTail(entry);
}

// FUNCTION: GAMEMD 0x006C6D00
void Queue_Pop(void)
{
	Queue.remove(0);
}

extern struct INetUtil * g_pNetUtil;
extern char g_GameServerHost[];
extern int g_GameServerPort;

// FUNCTION: GAMEMD 0x006C6DE0
void Queue_Send(void)
{
	PacketEntry entry;
	if (Queue.length() == 0) {
		return;
	}
	Queue.getHead(entry);
	while (entry.Size < 1 || entry.Kind > 10) {
		Queue.remove(0);
		if (Queue.length() == 0) {
			return;
		}
		Queue.getHead(entry);
	}
	if (g_pNetUtil == NULL) {
		return;
	}
	g_pNetUtil->RequestGameresSend(g_GameServerHost, g_GameServerPort, (unsigned char *)entry.Data, entry.Size);
	entry.Kind++;
	Queue.replace(entry, 0);
}

#include "aircraft.h"
#include "airctype.h"
#include "building.h"
#include "builtype.h"
#include "dsurface.h"
#include "getcpu.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "infatype.h"
#include "mpu.h"
#include "multiplayer.h"
#include "packet.h"
#include "rawfile.h"
#include "session.h"
#include "unit.h"
#include "unittype.h"
#include "utracker.h"
#include "version.h"
#include "win.h"
#include <winsock.h>

extern HouseClass * DAT_00AC1198;
extern int WestwoodOnline_GameID;
extern int WestwoodOnline_GameSKU_TS;
extern int DAT_00B73814;
extern int WestwoodOnline_Tournament;
extern int g_PingsSent;
extern int g_PingsReceived;
extern int WestwoodOnline_StartTime;
extern char WestwoodOnline_UserName[];
extern char WestwoodOnline_LoginName[];
extern Wstring g_NickName;
extern unsigned char DAT_00A8F900;
extern char DAT_00B77CE8[];
extern int DAT_00B75F5C;
namespace DirectDrawWrap { extern LPDIRECTDRAW lpDD; }
void __cdecl FUN_004068E0(char const * format, ...);
rc_ptr<PlayerProfile> __fastcall FUN_00778A20(Wstring const * name);

enum {
	COMPLETION_DISCONNECT = 1,
	COMPLETION_NO = 3,
	COMPLETION_QUIT = 4,
	COMPLETION_WON = 8,
	COMPLETION_LOST = 9,
};

int Get_Unit_Count(int *buf, int count);

#define GET_UNIT_TOTALS(tracker) (void*) player->tracker.Get_All_Totals(), Get_Unit_Count(player->tracker.Get_All_Totals(), player->tracker.Get_Unit_Count())*4

// FUNCTION: GAMEMD 0x006C6F50
void __fastcall Send_Statistics_Packet(bool wdt)
{
	PacketClass		stats;
	unsigned int 	playercount = 0;
	unsigned int 	statsid = 0;

	HouseClass		*player;
	static int		packet_size;
	int				index;
	unsigned int	i;
	char			*packet;
	unsigned int	temp;

	static char	field_player_handle[5] 				= { "NAM?" };
	static char	field_player_team[5] 				= { "SID?" };
	static char	field_player_t_team[5] 				= { "TID?" };
	static char	field_player_color[5]				= { "COL?" };
	static char	field_player_country[5]				= { "CTY?" };
	static char field_player_credits[5]				= { "CRD?" };
	static char field_player_units_left[5]			= { "UNL?" };
	static char field_player_infantry_left[5]		= { "INL?" };
	static char field_player_planes_left[5]			= { "PLL?" };
	static char field_player_buildings_left[5]		= { "BLL?" };
	static char field_player_units_bought[5] 		= { "UNB?" };
	static char field_player_infantry_bought[5] 	= { "INB?" };
	static char field_player_planes_bought[5]		= { "PLB?" };
	static char field_player_buildings_bought[5]	= { "BLB?" };
	static char field_player_units_killed[5] 		= { "UNK?" };
	static char field_player_infantry_killed[5] 	= { "INK?" };
	static char field_player_planes_killed[5]		= { "PLK?" };
	static char field_player_buildings_killed[5]	= { "BLK?" };
	static char field_player_buildings_captured[5]	= { "BLC?" };
	static char field_player_crates_found[5]		= { "CRA?" };
	static char field_player_harvested[5]			= { "HRV?" };
	static char field_player_completion[5]			= { "CMP?" };
	static char field_player_ip[5]					= { "IPA?" };
	static char field_player_clan[5]				= { "CID?" };
	static char field_player_lost_connection[5]		= { "LCN?" };
	static char field_player_cpu_type[5]			= { "CPT?" };
	static char field_player_cpu_speed[5]			= { "CPS?" };
	static char field_player_memory[5]				= { "MEM?" };
	static char field_player_video_memory[5]		= { "VID?" };

	if (DAT_00AC1198 != NULL && PlayerPtr == DAT_00AC1198) {
		return;
	}

	FUN_004068E0("Sending game results.  SawCompletion=%d\n", Session.SawGameCompletion);

	stats.Add_Field("IDNO", (unsigned int)WestwoodOnline_GameID);

	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		stats.Add_Field("WDTT", Session.WDTTerritory);
		if (wdt) {
			stats.Add_Field("GSKU", (unsigned int)WestwoodOnline_GameSKU_TS);
		} else {
			stats.Add_Field("GSKU", (unsigned int)DAT_00B73814);
		}
	} else {
		stats.Add_Field("GSKU", (unsigned int)WestwoodOnline_GameSKU_TS);
	}

	stats.Add_Field("TRNY", (unsigned int)WestwoodOnline_Tournament);
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3 && wdt) {
		WestwoodOnline_Tournament = 0;
	}

	stats.Add_Field("OOSY", (unsigned char)(Session.OutOfSync == true));
	stats.Add_Field("FINI", (unsigned char)(Session.SawGameCompletion == true));
	stats.Add_Field("DURA", (unsigned int)GameEndTime/60);
	stats.Add_Field("CRED", (unsigned int)Session.Options.Credits);

	char shortgame = Session.Options.ShortGame != false;
	stats.Add_Field("SHRT", (unsigned int)shortgame & 0xFF);
	char superweapons = Session.Options.SWAllowed != false;
	stats.Add_Field("SUPR", (unsigned int)superweapons & 0xFF);

	stats.Add_Field("MODE", (unsigned int)Session.Options.MPModeIndex);
	stats.Add_Field("BAMR", (unsigned int)(((Session.Options.BuildOffAlly == true) << 1) + (Session.Options.MCVRedeploy == true)));
	stats.Add_Field("BASE", (unsigned char)(Session.Options.Bases != false));

	char crates = Session.Options.Goodies != false;
	stats.Add_Field("CRAT", (unsigned int)crates & 0xFF);

	stats.Add_Field("AIPL", (unsigned int)Session.Options.AIPlayers);
	stats.Add_Field("UNIT", (unsigned int)Session.Options.UnitCount);
	stats.Add_Field("SCEN", Session.ScenarioFileName);
	stats.Add_Field("PNGS", (unsigned int)g_PingsSent);
	stats.Add_Field("PNGR", (unsigned int)g_PingsReceived);

	HouseClass *houses[MAX_PLAYERS];
	for (int h = 0; h < Houses.Count(); h++) {
		HouseClass *ptr = Houses[h];
		if (ptr->IsHuman && ptr != DAT_00AC1198) {
			houses[playercount] = ptr;
			playercount++;
		}
	}

	for (i = 0; i < playercount; i++) {
		for (unsigned int j = 0; j < (playercount - i) - 1; j++) {
			HouseClass *ptr1 = houses[j+1];
			HouseClass *ptr2 = houses[j];
			if (strcmpi(ptr2->IniName, ptr1->IniName) > 0) {
				houses[j] = ptr1;
				houses[j+1] = ptr2;
			}
		}
	}

	for (i = 1; i < playercount; i++) {
		HouseClass *ptr = houses[i];
		if (strcmpi(ptr->IniName, WestwoodOnline_UserName) == 0) {
			HouseClass *tmp = houses[0];
			houses[0] = ptr;
			houses[i] = tmp;
		}
	}

	for (i = 0; i < playercount; i++) {
		HouseClass *ptr = houses[i];
		if (strcmpi(ptr->IniName, WestwoodOnline_LoginName) == 0) {
			statsid = i;
			break;
		}
	}

	stats.Add_Field("PLRS", (unsigned int)playercount);
	stats.Add_Field("SPID", (unsigned int)statsid);

	if (WestwoodOnline_Tournament == 1) {
		rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
		if (!wdt) {
			if (Session.SawGameCompletion && !PlayerPtr->LostConnection) {
				if (!PlayerPtr->IsDefeated && !PlayerPtr->IsGiverUpper && !PlayerPtr->IsResigner) {
					profile.Get()->AddWins(1);
				} else {
					profile.Get()->AddLosses(1);
				}
			} else {
				profile.Get()->AddDisconnects(1);
			}
			profile.Get()->Save(&g_NickName);
		}
	}

	int completion[MAX_PLAYERS];
	for (i = 0; i < playercount; i++) {
		if (!Session.SawGameCompletion) {
			completion[i] = (1 << COMPLETION_NO);
		} else {
			HouseClass * hptr = houses[i];
			if (hptr->LostConnection) {
				completion[i] = (1 << COMPLETION_DISCONNECT);
			} else if (hptr->IsGiverUpper || hptr->IsResigner) {
				completion[i] = (1 << COMPLETION_LOST) | (1 << COMPLETION_QUIT);
			} else if (hptr->IsDefeated) {
				completion[i] = (1 << COMPLETION_LOST);
			} else {
				completion[i] = (1 << COMPLETION_WON);
			}
		}
	}

	stats.Add_Field("TIME", (int)WestwoodOnline_StartTime);

	int divisor = GameEndTime / 60;
	if (divisor != 0) {
		stats.Add_Field("AFPS", (unsigned int)Frame / (GameEndTime/60));
	} else {
		stats.Add_Field("AFPS", 0);
	}

	stats.Add_Field("SPED", (unsigned int)Options.GameSpeed);

	char	version[128];
	wsprintf(version, "V%s", VerNum.Version_Name());
	stats.Add_Field("VERS", version);

	char path_to_exe[280];
	FILETIME write_time;

	GetModuleFileName(ProgramInstance, path_to_exe, sizeof(path_to_exe));
	RawFileClass file;
	file.Set_Name(path_to_exe);
	file.Open();
	HANDLE handle = file.Get_File_Handle();

	if (handle != INVALID_HANDLE_VALUE) {
		if (GetFileTime(handle, NULL, NULL, &write_time)) {
			write_time.dwLowDateTime = htonl(write_time.dwLowDateTime);
			write_time.dwHighDateTime = htonl(write_time.dwHighDateTime);
			stats.Add_Field("DATE", (void*)&write_time, sizeof(write_time));
		}
	}

	if (Session.WOLWelcomeChoice == 1) {
		stats.Add_Field("BROK", (unsigned int)2);
	} else if (Session.WOLWelcomeChoice == 2) {
		stats.Add_Field("BROK", (unsigned int)4);
	} else if (Session.WOLWelcomeChoice == 4) {
		stats.Add_Field("BROK", (unsigned int)1);
	}

	if (Session.MPGameMode != NULL) {
		stats.Add_Field("ACCO", (unsigned int)Session.MPGameMode->vt_entry_B8());
	}

	for (unsigned house = 0; house < playercount; house++) {
		player = houses[house];
		char digit = '0' + (char)house;

		field_player_handle[3] = digit;
		stats.Add_Field(field_player_handle, (char const *)player->IniName);

		temp = player->IPAddress;
		field_player_ip[3] = digit;
		stats.Add_Field(field_player_ip, (unsigned int)(temp));

		temp = player->SquadID;
		field_player_clan[3] = digit;
		stats.Add_Field(field_player_clan, (unsigned int)(temp));

		field_player_team[3] = digit;
		stats.Add_Field(field_player_team, (unsigned int)player->ActLike);

		field_player_t_team[3] = digit;
		temp = house;
		if (WestwoodOnline_Tournament == 2) {
			temp = player->SquadID;
		}
		stats.Add_Field(field_player_t_team, (unsigned int)(temp));

		field_player_completion[3] = digit;
		stats.Add_Field(field_player_completion, (unsigned int)(completion[house]));

		field_player_lost_connection[3] = digit;
		bool lcn = false;
		if (player->LostConnection && !player->IsResigner) {
			lcn = true;
		}
		stats.Add_Field(field_player_lost_connection, (unsigned char)(lcn));

		field_player_color[3] = digit;
		stats.Add_Field(field_player_color, (unsigned int)(player->Scheme));

		field_player_country[3] = digit;
		stats.Add_Field(field_player_country, (unsigned int)(player->Class->House));

		field_player_credits[3] = digit;
		stats.Add_Field(field_player_credits, (int)player->Available_Money());

		field_player_infantry_bought[3] = digit;
		field_player_units_bought[3] = digit;
		field_player_planes_bought[3] = digit;
		field_player_buildings_bought[3] = digit;

		player->InfantryTotals.To_Network_Format();
		player->UnitTotals.To_Network_Format();
		player->AircraftTotals.To_Network_Format();
		player->BuildingTotals.To_Network_Format();

		stats.Add_Field(field_player_infantry_bought, GET_UNIT_TOTALS(InfantryTotals));
		stats.Add_Field(field_player_units_bought, GET_UNIT_TOTALS(UnitTotals));
		stats.Add_Field(field_player_planes_bought, GET_UNIT_TOTALS(AircraftTotals));
		stats.Add_Field(field_player_buildings_bought, GET_UNIT_TOTALS(BuildingTotals));

		player->InfantryTotals.To_PC_Format();
		player->UnitTotals.To_PC_Format();
		player->AircraftTotals.To_PC_Format();
		player->BuildingTotals.To_PC_Format();

		player->InfantryTotals.Clear_Unit_Total();
		player->AircraftTotals.Clear_Unit_Total();
		player->UnitTotals.Clear_Unit_Total();
		player->BuildingTotals.Clear_Unit_Total();

		for (index = 0; index < Units.Count(); index++) {
			UnitClass const * unit = Units[index];
			if (player == unit->House) {
				player->UnitTotals.Increment_Unit_Total(unit->Class->HeapID);
			}
		}

		for (index = 0; index < Infantry.Count(); index++) {
			InfantryClass const * infantry = Infantry[index];
			if (player == infantry->House && !infantry->Class->IsCivilian) {
				player->InfantryTotals.Increment_Unit_Total(infantry->Class->HeapID);
			}
		}

		for (index = 0; index < Aircraft.Count(); index++) {
			AircraftClass const * aircraft = Aircraft[index];
			if (player == aircraft->House) {
				player->AircraftTotals.Increment_Unit_Total(aircraft->Class->HeapID);
			}
		}

		int count = player->Buildings.Count();
		for (index = 0; index < count; index++) {
			BuildingClass const * building = player->Buildings[index];
			player->BuildingTotals.Increment_Unit_Total(building->Class->HeapID);
		}

		player->InfantryTotals.To_Network_Format();
		player->UnitTotals.To_Network_Format();
		player->AircraftTotals.To_Network_Format();
		player->BuildingTotals.To_Network_Format();

		field_player_infantry_left[3] = digit;
		field_player_units_left[3] = digit;
		field_player_planes_left[3] = digit;
		field_player_buildings_left[3] = digit;

		stats.Add_Field(field_player_infantry_left, GET_UNIT_TOTALS(InfantryTotals));
		stats.Add_Field(field_player_units_left, GET_UNIT_TOTALS(UnitTotals));
		stats.Add_Field(field_player_planes_left, GET_UNIT_TOTALS(AircraftTotals));
		stats.Add_Field(field_player_buildings_left, GET_UNIT_TOTALS(BuildingTotals));

		player->DestroyedInfantry.To_Network_Format();
		player->DestroyedUnits.To_Network_Format();
		player->DestroyedAircraft.To_Network_Format();
		player->DestroyedBuildings.To_Network_Format();

		field_player_infantry_killed[3] = digit;
		field_player_units_killed[3] = digit;
		field_player_planes_killed[3] = digit;
		field_player_buildings_killed[3] = digit;

		stats.Add_Field(field_player_infantry_killed, GET_UNIT_TOTALS(DestroyedInfantry));
		stats.Add_Field(field_player_units_killed, GET_UNIT_TOTALS(DestroyedUnits));
		stats.Add_Field(field_player_planes_killed, GET_UNIT_TOTALS(DestroyedAircraft));
		stats.Add_Field(field_player_buildings_killed, GET_UNIT_TOTALS(DestroyedBuildings));

		field_player_buildings_captured[3] = digit;
		player->CapturedBuildings.To_Network_Format();
		stats.Add_Field(field_player_buildings_captured, GET_UNIT_TOTALS(CapturedBuildings));

		field_player_crates_found[3] = digit;
		player->TotalCrates.To_Network_Format();
		stats.Add_Field(field_player_crates_found, GET_UNIT_TOTALS(TotalCrates));

		field_player_harvested[3] = digit;
		stats.Add_Field(field_player_harvested, (unsigned int)player->HarvestedCredits);

		field_player_cpu_type[3] = digit;
		if (statsid == house) {
			stats.Add_Field(field_player_cpu_type, (unsigned int)CPUType);
		} else {
			stats.Add_Field(field_player_cpu_type, -1);
		}

		field_player_cpu_speed[3] = digit;
		if (statsid == house) {
			stats.Add_Field(field_player_cpu_speed, Get_RDTSC_CPU_Speed());
		} else {
			stats.Add_Field(field_player_cpu_speed, -1);
		}

		field_player_memory[3] = digit;
		if (statsid == house) {
			MEMORYSTATUS mem_info;
			mem_info.dwLength = sizeof(mem_info);
			GlobalMemoryStatus(&mem_info);
			stats.Add_Field(field_player_memory, (int)mem_info.dwTotalPhys);
		} else {
			stats.Add_Field(field_player_memory, -1);
		}

		field_player_video_memory[3] = digit;
		if (statsid == house && DirectDrawWrap::lpDD != NULL) {
			DDCAPS video_capabilities;
			video_capabilities.dwSize = sizeof(video_capabilities);
			if (DD_OK == DirectDrawWrap::lpDD->GetCaps(&video_capabilities, NULL)) {
				int video_memory = video_capabilities.dwVidMemTotal;
				video_memory += 1024*1024 - 1;
				video_memory &= 0xfff00000;
				stats.Add_Field(field_player_video_memory, (int)video_memory);
			}
		} else {
			stats.Add_Field(field_player_video_memory, -1);
		}
	}

	packet = stats.Create_Comms_Packet(packet_size);

	if (g_pNetUtil != NULL) {
		if (wdt) {
			g_pNetUtil->RequestGameresSend(DAT_00B77CE8, DAT_00B75F5C, (unsigned char*)packet, packet_size);
		} else {
			g_pNetUtil->RequestGameresSend(g_GameServerHost, g_GameServerPort, (unsigned char*)packet, packet_size);
		}
	}

	Queue_Packet(packet, packet_size);

	delete [] packet;

	DAT_00A8F900 = true;
}

#undef GET_UNIT_TOTALS


// FUNCTION: GAMEMD 0x006C87D0
int Get_Unit_Count(int *buf, int count)
{
	int n = 0;
	for (int i = 0; i < count; i++) {
		if (buf[i] != 0) {
			n = i + 1;
		}
	}
	return(n);
}

// FUNCTION: GAMEMD 0x006C87F0
void Register_Game_Start_Time(void)
{
	GameTimer = 0;
	GameTimerInUse = true;
}

// FUNCTION: GAMEMD 0x006C8820
void Register_Game_End_Time(void)
{
	GameEndTime = GameTimer;
	GameTimerInUse = false;
}

// SYNTHETIC: GAMEMD 0x006C8840
// ??_G?$rc_ptr@VPlayerProfile@@@@UAEPAXI@Z
template class rc_ptr<PlayerProfile>;

// TEMPLATE: GAMEMD 0x006C8870
// ?shrinkVector@?$ArrayList@UPacketEntry@@@@AAEDXZ
// TEMPLATE: GAMEMD 0x006C8900
// ?growVector@?$ArrayList@UPacketEntry@@@@AAEDXZ
