// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "conquer.h"
#include "_keyboar.h"
#include "_map.h"
#include "_palette.h"
#include "_rules.h"
#include "_source.h"
#include "_surface.h"
#include "_tactica.h"
#include "_theater.h"
#include "_tooltip.h"
#include "_wsproto.h"
#include "beacon.h"
#include "cctooltip.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "except.h"
#include "gamedlg.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "init.h"
#include "internet.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language\language.h"
#include "logic.h"
#include "mainloop.h"
#include "movie.h"
#include "mplayer.h"
#include "msgloop.h"
#include "netdlg.h"
#include "netdlg2.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "setpal.h"
#include "sidebar.h"
#include "sounddlg.h"
#include "stats.h"
#include "surface.h"
#include "tactical.h"
#include "theme.h"
#include "voc.h"
#include "vox.h"
#include "wolapi\wolapi.h"
#include "wonline.h"
#include "wsproto.h"
#include "special.hh"
#include "goptions.h"
#undef LoadString
#include "stringtable.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <direct.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E51E0;
extern const double DAT_007E51E8;

void __fastcall FUN_00731D00(int mode);
extern BeaconManagerClass BeaconManager;

void __fastcall FUN_00406F70(void);
struct CallBackObject { virtual void V0(); virtual void V1(); virtual void V2(); };
extern CallBackObject ** DAT_00B45B5C;
extern int DAT_00B45B68;
extern bool DAT_00B77DC2;
extern int g_SuspendChatPump;
extern time_t DAT_0089E9B0;



struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0048C6B0 SYMBOL
// _$E87
double DAT_0089E948 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2.0);

// SYNTHETIC: GAMEMD 0x0048C6E0 SYMBOL
// _$E90
double DAT_0089E9A0 = DAT_0089E948 / DAT_007E51E0;

// SYNTHETIC: GAMEMD 0x0048C700 SYMBOL
// _$E93
double DAT_0089E988 = DAT_007E1718 / DAT_0089E9A0;

// SYNTHETIC: GAMEMD 0x0048C720 SYMBOL
// _$E96
double DAT_0089E980 = DAT_007E1720 * DAT_007E51E8;

// SYNTHETIC: GAMEMD 0x0048C740 SYMBOL
// _$E99
double DAT_0089E958 = DAT_007E1728 * DAT_007E51E8;

// SYNTHETIC: GAMEMD 0x0048C760 SYMBOL
// _$E102
double DAT_0089E918 = DAT_007E1730 * DAT_007E51E8;

// SYNTHETIC: GAMEMD 0x0048C780 SYMBOL
// _$E105
int DAT_0089E9A8 = (int)(FUN_004CAD50(DAT_0089E918 - DAT_0089E958) * DAT_007E1738 * DAT_0089E948);

// SYNTHETIC: GAMEMD 0x0048C7C0 SYMBOL
// _$E108
double DAT_0089E960 = FUN_004CADE0(DAT_0089E9A8 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0048C7F0 SYMBOL
// _$E111
double DAT_0089E978 = FUN_004CADE0((DAT_0089E9A8 * 2) / DAT_0089E948);

// SYNTHETIC: GAMEMD 0x0048C820 SYMBOL
// _$E114
int DAT_0089E99C = (int)(DAT_0089E9A8 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0048C850 SYMBOL
// _$E117
ShortPair DAT_0089E940;

// SYNTHETIC: GAMEMD 0x0048C860 SYMBOL
// _$E120
IntTriple DAT_0089E968;

int DAT_0089E998;
int DAT_0089E990;
int DAT_0089E994;

// FUNCTION: GAMEMD 0x0048C880
void Reset_Conquer_Globals(void)
{
	DAT_0089E998 = 0;
	DAT_0089E990 = 128;
	DAT_0089E994 = 128;
}

// FUNCTION: GAMEMD 0x0048C8A0
void Conquer_Nop(void)
{
}

namespace Game { extern int SpecialDialog; extern int ObserverMode; extern HWND hWnd; extern HINSTANCE hInstance; }
void __fastcall FUN_0065F520(ScenarioClass * scen);
struct FileSystem
{
	static char TEMPERAT_PAL[];
};
namespace Unsorted { extern int ScenarioInit; extern bool ScenarioStarted; extern bool TacticalActive; }
namespace FPSCounter { extern unsigned TotalTimeElapsed; extern unsigned TotalFramesElapsed; }
extern bool UserInputLocked;
extern int DAT_00ABCDFC;
extern int DAT_00ABCE00;
extern unsigned char DAT_00A8F900;
extern int DAT_00B0BDB8;
extern bool ConnectionLost;
extern unsigned char DAT_008175B0;
extern PaletteClass DAT_00884E80;
bool __fastcall FUN_0055CFD0(void);
void __fastcall FUN_0072DFB0(void);
void Print_MP_Stats(void);

// FUNCTION: GAMEMD 0x0048C8B0
void Ingame_Menu_Dialog(void)
{
	if (Game::SpecialDialog != SDLG_NONE) {
		if (Session.Type != GAME_NORMAL) {
			if (PlayerPtr->IsToLose || PlayerPtr->IsToWin || PlayerPtr->IsToDie) {
				Game::SpecialDialog = SDLG_NONE;
				return;
			}
			if (!_special_dialog_flag) {
				if (PlayerPtr->IsDefeated) {
					_special_dialog_flag = true;
				} else {
					Game::SpecialDialog = SDLG_NONE;
					return;
				}
			}
		}

		Pause_Scenario();
		MouseCursor->Release_Mouse();
		while (MouseCursor->Get_Mouse_State()) {
			MouseCursor->Show_Mouse();
		}

		while (Game::SpecialDialog != SDLG_NONE) {
			switch (Game::SpecialDialog) {
				case 9:
					FUN_0065F520(Scen);
					if (Game::SpecialDialog == 9) {
						Game::SpecialDialog = SDLG_NONE;
					}
					break;

				case 8:
					Map.FUN_006586D0();
					if (Game::SpecialDialog == 8) {
						Game::SpecialDialog = SDLG_NONE;
					}
					break;

				case 1:
					Game_Options_Dialog();
					if (Game::SpecialDialog != 1) {
						break;
					}
					Game::SpecialDialog = SDLG_NONE;
					break;

				case 5:
					GameControlsClass().Dialog();
					if (Game::SpecialDialog != 5) {
						break;
					}
					Game::SpecialDialog = 1;
					break;

				case 6:
					SoundControlsClass().Dialog();
					Game::SpecialDialog = 5;
					break;

				case 4:
					Options.Hotkey_Dialog();
					Game::SpecialDialog = 5;
					break;

				case 3:
					switch (Abort_Dialog()) {
						case 2:
							Queue_Exit();
							break;

						case 3:
							OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::DESTRUCT));
							_special_dialog_flag = false;
							Queue_Exit();
							break;

						case 4:
							OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::DESTRUCT));
							_special_dialog_flag = false;
							break;

						case 5:
							PlayerRestarts = true;
							break;
					}
					Game::SpecialDialog = SDLG_NONE;
					break;

				case 2:
					if (Game::ObserverMode) {
						Queue_Exit();
					} else if (!PlayerPtr->IsDefeated && !PlayerPtr->IsToWin && !PlayerPtr->IsToLose && !PlayerPtr->IsToDie && Surrender_Dialog(StringTable::LoadString("TXT_SURRENDER", NULL, "D:\\ra2mdpost\\Conquer.CPP", 0x1e7))) {
						OutList.Add(EventClass(PlayerPtr->HeapID, (unsigned char)EventClass::DESTRUCT));
						Queue_Exit();
						_special_dialog_flag = false;
					}
					Game::SpecialDialog = SDLG_NONE;
					break;

				case 7:
					DoFindPage();
					Game::SpecialDialog = SDLG_NONE;
					break;

				default:
					break;
			}
		}

		UpdateWindow(Game::hWnd);
		if (Scen->IsInputLocked) {
			MouseCursor->Hide_Mouse();
		}
		MouseCursor->Capture_Mouse();
		Session.Resume();
		Resume_Scenario();
	}
}

// FUNCTION: GAMEMD 0x0048CCC0
void Main_Game(int argc, char * argv[])
{
	static bool fade = true;

	int ret = Init_Game(argc, argv);
	if (ret) {
		if (ret < 0) {
			char text[0x1400];
			char caption[0x800];
			MSGBOXPARAMS params;
			WideCharToMultiByte(0, 0, StringTable::LoadString("TXT_INITGAME_FAILED", NULL, "D:\\ra2mdpost\\Conquer.CPP", 0x236), -1, text, sizeof(text), 0, 0);
			WideCharToMultiByte(0, 0, StringTable::LoadString("TXT_SHORT_TITLE", NULL, "D:\\ra2mdpost\\Conquer.CPP", 0x237), -1, caption, sizeof(caption), 0, 0);
			params.cbSize = sizeof(MSGBOXPARAMS);
			params.hwndOwner = Game::hWnd;
			params.hInstance = Game::hInstance;
			params.lpszText = text;
			params.lpszCaption = caption;
			params.dwStyle = (MB_OK | MB_ICONSTOP | MB_SETFOREGROUND | MB_TOPMOST);
			params.lpszIcon = NULL;
			params.dwContextHelpId = NULL;
			params.lpfnMsgBoxCallback = NULL;
			params.dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
			MessageBoxIndirect(&params);
		}
		return;
	}

	__asm {
		mov ExceptionReturnStack, esp
		mov ExceptionReturnFrame, ebp
		lea eax, ret_addr
		mov ExceptionReturnAddress, eax
	}

ret_addr:
	DebugString("ExceptionReturnStack = %08X\n", ExceptionReturnStack);
	DebugString("ExceptionReturnAddress = %08X\n", ExceptionReturnAddress);

	while (Select_Game(fade)) {
		Unsorted::ScenarioInit = 0;
		fade = true;

		Set_Palette(*(PaletteClass const *)FileSystem::TEMPERAT_PAL, 15, NULL);
		Keyboard->Clear();
		if (Session.Play) {
			MouseCursor->Hide_Mouse();
			DAT_00ABCDFC = 0;
			DAT_00ABCE00 = 0;
		}

		if (Session.Type == GAME_INTERNET) {
			Register_Game_Start_Time();
			DAT_00A8F900 = 0;
			DAT_00B0BDB8 = 0;
			ConnectionLost = false;
		}

		if (ToolTips != NULL) {
			ToolTips->Activate(Options.ToolTips);
		}

		UserInputLocked = Scen->IsInputLocked;
		Game::SpecialDialog = SDLG_NONE;
		_special_dialog_flag = true;
		Session.Resume();

		for (;;) {
			if (Main_Loop()) {
				if (FUN_0055CFD0()) {
					break;
				}
			}
			Ingame_Menu_Dialog();
			if (FUN_0055CFD0()) {
				break;
			}
		}
		FUN_0072DFB0();
		Session.FUN_0069BB40();
		Unsorted::ScenarioStarted = false;
		Unsorted::TacticalActive = false;

		DebugString("Game loop finished. Average FPS = %d\n", FPSCounter::TotalTimeElapsed ? FPSCounter::TotalFramesElapsed / FPSCounter::TotalTimeElapsed : 0);

		Print_MP_Stats();

		if (!DAT_008175B0) {
			Set_Palette(DAT_00884E80, 30, NULL);
		}
		Title_Screen_Restore(true);

		if (Session.Record || Session.Play) {
			Session.RecordFile.Close();
		}

		if (Session.Type == GAME_NULL_MODEM || Session.Type == GAME_MODEM) {
			if (!Session.Play) {
				Modem_Signoff();
			}
		} else {
			if (Session.Type == GAME_IPX) {
				if (!Session.Play) {
					Shutdown_Network();
				}
			}
		}

		if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET || Session.Type == GAME_NULL_MODEM || Session.Type == GAME_MODEM) {
			if (!Session.Play) {
				Session.Write_Chat_Settings();
			}
		}

		if (Session.Play) {
			Session.Type = GAME_NORMAL;
			Session.Play = 0;
			MouseCursor->Show_Mouse();
		}
	}

	if (MouseCursor != NULL) {
		MouseCursor->Release_Mouse();
	}
}

// FUNCTION: GAMEMD 0x0048CFD0
void Serial_Call_Back(void)
{
	int packetlen;
	SerialPacketType packet;

	if (Session.Type == GAME_NULL_MODEM || Session.ModemService) {
		NullModem.Service();
		if (Scen->IsReadingScenario) {
			while (NullModem.Get_Message(&packet, &packetlen) > 0) {
				if (packet.Command == SERIAL_PROGRESS_REPORT && packet.ID != Session.ModemType) {
					Progress.Set_Progress_Percent(1, packet.Progress.Percent);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0048D080
void Call_Back(void)
{
	FUN_00406F70();
	if (DAT_00B45B68 > 0 && DAT_00B45B5C[0] != NULL) {
		DAT_00B45B5C[0]->V2();
	}

	if (Session.Type == GAME_IPX || Session.Type == GAME_INTERNET) {
		IPX_Call_Back();
	}

	if (Session.Type == GAME_MODEM || Session.Type == GAME_NULL_MODEM) {
		Serial_Call_Back();
	}

	if (Session.Type == GAME_INTERNET) {
		if (!DAT_00B77DC2 && time(0) != DAT_0089E9B0) {
			if (g_pChat != NULL && !g_SuspendChatPump) {
				g_pChat->PumpMessages();
			}
			if (g_pNetUtil != NULL) {
				g_pNetUtil->PumpMessages();
			}
			DAT_0089E9B0 = time(0);
		}
	}
}

// FUNCTION: GAMEMD 0x0048D1D0
bool MapGen_Call_Back(void)
{
	Call_Back();
	return(false);
}

// FUNCTION: GAMEMD 0x0048DB70
void Conquer_Nop2(void)
{
}

// FUNCTION: GAMEMD 0x0048DB80
SourceType Source_From_Name(char const * name)
{
	if (name) {
		for (SourceType source = SOURCE_FIRST; source < SOURCE_COUNT; source++) {
			if (stricmp(SourceName[source], name) == 0) {
				return(source);
			}
		}
	}
	return(SOURCE_NONE);
}

// FUNCTION: GAMEMD 0x0048DBC0
char const * Name_From_Source(SourceType source)
{
	if ((unsigned)source < SOURCE_COUNT) {
		return(SourceName[source]);
	}
	return("None");
}

// FUNCTION: GAMEMD 0x0048DBE0
TheaterType Theater_From_Name(char const * name)
{
	for (TheaterType index = THEATER_FIRST; index < THEATER_COUNT; index++) {
		if (stricmp(name, Theaters[index].Name) == 0) {
			return(index);
		}
	}
	return(THEATER_NONE);
}

// FUNCTION: GAMEMD 0x0048DC20
FacingType KN_To_Facing(int input)
{
	input &= ~(KN_ALT_BIT|KN_SHIFT_BIT|KN_CTRL_BIT);
	switch (input) {
		case KN_LEFT:
			return(FACING_W);

		case KN_RIGHT:
			return(FACING_E);

		case KN_UP:
			return(FACING_N);

		case KN_DOWN:
			return(FACING_S);

		case KN_UPLEFT:
			return(FACING_NW);

		case KN_UPRIGHT:
			return(FACING_NE);

		case KN_DOWNLEFT:
			return(FACING_SW);

		case KN_DOWNRIGHT:
			return(FACING_SE);

		default:
			break;
	}
	return(FACING_NONE);
}

// FUNCTION: GAMEMD 0x0048DC90
void Unselect_All(void)
{
	while (CurrentObject.Count()) {
		CurrentObject[0]->Unselect();
	}
	FUN_00731D00(0);
	BeaconManager.ClearSelection();
}

// FUNCTION: GAMEMD 0x0048DCD0
TechnoTypeClass const * Fetch_Techno_Type(RTTIType type, int id)
{
	switch (type) {
		case RTTI_UNITTYPE:
		case RTTI_UNIT:
			return(TechnoTypeClass *)(UnitTypes[id]);

		case RTTI_BUILDINGTYPE:
		case RTTI_BUILDING:
			return(TechnoTypeClass *)(BuildingTypes[id]);

		case RTTI_INFANTRYTYPE:
		case RTTI_INFANTRY:
			return(TechnoTypeClass *)(InfantryTypes[id]);

		case RTTI_AIRCRAFTTYPE:
		case RTTI_AIRCRAFT:
			return(TechnoTypeClass *)(AircraftTypes[id]);

		default:
			break;
	}
	return(NULL);
}

// FUNCTION: GAMEMD 0x0048DD50
unsigned int Disk_Space_Available(void)
{
	ULARGE_INTEGER freebytecount;
	ULARGE_INTEGER totalbytecount;
	ULARGE_INTEGER totalfreebytecount;
	unsigned int diskspace;

	bool (__stdcall *getfreediskspaceex) (LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	DebugString("Checking available disk space\n");

	HINSTANCE kernel = GetModuleHandle("KERNEL32.DLL");
	if (kernel != NULL) {
		getfreediskspaceex = (bool (_stdcall*) (LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER)) GetProcAddress (kernel, "GetDiskFreeSpaceExA");
		if (getfreediskspaceex != NULL) {
			DebugString("Using GetDiskFreeSpaceEx\n");

			if (!getfreediskspaceex(NULL, &freebytecount, &totalbytecount, &totalfreebytecount)) {
				DebugString("GetDiskFreeSpaceEx failed with error code %d - %s\n", GetLastError(), Last_Error_Text());
			} else {
				diskspace = int(((int)freebytecount.LowPart + ((int)freebytecount.HighPart * (double)((__int64)UINT_MAX + 1))) / (double)1024);
				DebugString("Free disk space is %d Mb\n", diskspace / 1024);
				return(diskspace);
			}
		} else {
			DebugString("GetProcAddress failed with error code %d - %s\n", GetLastError(), Last_Error_Text());
		}
	} else {
		DebugString("Failed to get module handle for KERNEL32.DLL\n");
	}

	DWORD sectorspercluster, bytespersector, freeclustercount, totalclustercount;

	if (GetDiskFreeSpace(NULL, &sectorspercluster, &bytespersector, &freeclustercount, &totalclustercount)) {
		diskspace = ((sectorspercluster * bytespersector) / 1024) * freeclustercount;
		DebugString("Free disk space is %d Mb\n", diskspace / 1024);
		return(diskspace);
	}

	return(0);
}

// FUNCTION: GAMEMD 0x0048DE70
CrateType Crate_From_Name(char const * name)
{
	if (name != NULL) {
		for (CrateType crate = CRATE_FIRST; crate < CRATE_COUNT; crate++) {
			if (stricmp(name, CrateNames[crate]) == 0) return(crate);
		}
	}
	return(CRATE_MONEY);
}

// FUNCTION: GAMEMD 0x0048DEB0
int Owner_From_Name(char const * text)
{
	int ownable = 0;
	HousesType h = HouseTypeClass::From_Name(text);
	if (h != HOUSE_NONE) {
		ownable |= (1 << h);
	}
	return(ownable);
}

// FUNCTION: GAMEMD 0x0048DED0
void Conquer_Nop3(void)
{
}

// FUNCTION: GAMEMD 0x0048DEE0
void List_Copy(Cell const * source, int len, Cell * dest)
{
	if (source == NULL || dest == NULL) {
		return;
	}

	while (len > 0) {
		*dest = *source;
		if (*dest == REFRESH_EOL) return;
		dest++;
		source++;
		len--;
	}

	*(dest-1) = REFRESH_EOL;
}

// FUNCTION: GAMEMD 0x0048DF30
VQType VQ_From_Name(char const * name)
{
	if (name != NULL && strcmpi("<none>", name)) {
		for (int movie = 0; movie < Movies.Count(); movie++) {
			if (stricmp(name, Movies[movie]) == 0) return(VQType(movie));
		}
	}
	return(VQ_NONE);
}

// FUNCTION: GAMEMD 0x0048DF80
LandType Land_From_Name(char const * name)
{
	if (strcmpi("<none>", name) && name != NULL) {
		for (LandType land = LAND_FIRST; land < LAND_COUNT; land++) {
			if (stricmp(LandName[land], name) == 0) return(land);
		}
	}
	return(LAND_NONE);
}

// FUNCTION: GAMEMD 0x0048DFD0
char const * Name_From_Land(LandType land)
{
	if ((unsigned)land < LAND_COUNT) {
		return(LandName[land]);
	}
	return("<none>");
}

// FUNCTION: GAMEMD 0x0048DFF0
SpeedType Speed_From_Name(char const * name)
{
	if (name != NULL) {
		for (SpeedType speed = SPEED_FIRST; speed < SPEED_COUNT; speed++) {
			if (stricmp(SpeedName[speed], name) == 0) return(speed);
		}
	}
	return(SPEED_NONE);
}

// FUNCTION: GAMEMD 0x0048E030
char const * Name_From_Speed(SpeedType speed)
{
	if ((unsigned)speed < SPEED_COUNT) {
		return(SpeedName[speed]);
	}
	return("<none>");
}

extern char const * LayerName[LAYER_COUNT];

// FUNCTION: GAMEMD 0x0048E050
LayerType Layer_From_Name(char const * name)
{
	if (name != NULL) {
		for (LayerType layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
			if (stricmp(LayerName[layer], name) == 0) return(layer);
		}
	}
	return(LAYER_NONE);
}

// FUNCTION: GAMEMD 0x0048E090
char const * Name_From_Layer(LayerType layer)
{
	if ((unsigned)layer < LAYER_COUNT) {
		return(LayerName[layer]);
	}
	return("<none>");
}

// FUNCTION: GAMEMD 0x0048E0B0
void Print_MP_Stats(void)
{
	if (Session.Type == GAME_INTERNET) {
		FILE *file = fopen("mpstats.txt", "wt");
		if (file != NULL) {
			fprintf(file, "Frames: %d\n", Frame);

			fprintf(file, "Average FPS: %d\n", FPSCounter::TotalTimeElapsed != 0 ? FPSCounter::TotalFramesElapsed / FPSCounter::TotalTimeElapsed : 0);
			fprintf(file, "Max MaxAhead: %d\n", Session.MaxMaxAhead);
			fprintf(file, "Latency setting: %d\n", Session.LatencyFudge);
			fprintf(file, "Game speed setting: %d\n", Options.GameSpeed);

			if (PacketTransport != NULL) {
				for (int i = 0; i < PacketTransport->Get_Num_Local_Addresses(); i++) {
					unsigned char *addr = PacketTransport->Get_Local_Address(i);
					if (addr != NULL) {
						unsigned int a = *(unsigned int *)addr;
						fprintf(file, "Local address: %d.%d.%d.%d\n", (a) & 0xFF, (a >> 8) & 0xFF, (a >> 16) & 0xFF, (a >> 24) & 0xFF);
					}
				}
			}

			for (int i = 0; i < MAX_PLAYERS; i++) {
				MPStatsType &stats = Session.ConnectionStats[i];
				if (stats.Name[0] == '\0') {
					continue;
				}

				fprintf(file, "\nName: %s\n", stats.Name);
				fprintf(file, "Address: %s\n", stats.Address.As_String(0));
				fprintf(file, "Max avg round trip: %d\n", stats.MaxAvgRoundTrip);
				fprintf(file, "Max round trip: %d\n", stats.MaxRoundTrip);
				fprintf(file, "Resends: %d\n", stats.Resends);
				fprintf(file, "Frame sync stalls: %d\n", stats.FrameSyncStalls);
				fprintf(file, "Command cound stalls: %d\n", stats.CommandCountStalls);
				fprintf(file, "Lost: %d\n", stats.Lost);
				fprintf(file, "Percent lost: %d\n", stats.PercentLost);
			}

			fclose(file);
		}
	}
}

extern int DAT_0089E9C0, DAT_0089E9C4, DAT_0089E9C8, DAT_0089E9CC;

// FUNCTION: GAMEMD 0x0048E270
void Clear_Conquer_Counters(void)
{
	DAT_0089E9C0 = 0;
	DAT_0089E9C4 = 0;
	DAT_0089E9C8 = 0;
	DAT_0089E9CC = 0;
}
