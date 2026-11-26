// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG

#include "always.h"
#include "arraylist.h"
#include "session.h"
#include "timer.h"
#include "wstring.h"
#include "widewstring.h"
#include "ipxaddr.h"
#include "ccini.h"
#include "rawfile.h"
#include "theme.h"
#include "playerprofile.h"
#include "ownrdraw.h"
#include "rcptr.h"
#include "rules.h"
#include "netshare.h"
#include "ipxmgr.h"
#include "_rules.h"
#include "gadget.h"
#include "_xmouse.h"
#include "coopcamp.h"
#include "windlg.h"
#include "tiprequest.h"
#include "wolopt.h"

#include <windows.h>
#undef LoadString
#include <ole2.h>
#include <ocidl.h>
#include "wolapi/wolapi.h"
#include <shellapi.h>
#include <winsock.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "srfcache.h"
#include "stringtable.h"

HWND WS_Find_Dialog(int id);
HWND FUN_00775B10(void);
rc_ptr<PlayerProfile> __fastcall FUN_00778710(void);

unsigned int __fastcall Wstring_Hash(Wstring & string);
#define HASH_ARG (Wstring_Hash)


class OLEInitializer
{
public:
	OLEInitializer(void) { OleInitialize(NULL); }
	~OLEInitializer(void) { OleUninitialize(); }
};

#include <math.h>
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0077B690 SYMBOL
// _$E44
// SYNTHETIC: GAMEMD 0x0077B6C0 SYMBOL
// _$E47
// SYNTHETIC: GAMEMD 0x0077B6E0 SYMBOL
// _$E50
// SYNTHETIC: GAMEMD 0x0077B700 SYMBOL
// _$E53
// SYNTHETIC: GAMEMD 0x0077B720 SYMBOL
// _$E56
// SYNTHETIC: GAMEMD 0x0077B740 SYMBOL
// _$E59
// SYNTHETIC: GAMEMD 0x0077B760 SYMBOL
// _$E62
// SYNTHETIC: GAMEMD 0x0077B7A0 SYMBOL
// _$E65
// SYNTHETIC: GAMEMD 0x0077B7D0 SYMBOL
// _$E68
// SYNTHETIC: GAMEMD 0x0077B800 SYMBOL
// _$E71
// SYNTHETIC: GAMEMD 0x0077B830 SYMBOL
// _$E74
// SYNTHETIC: GAMEMD 0x0077B840 SYMBOL
// _$E77
// SYNTHETIC: GAMEMD 0x0077B860 SYMBOL
// _$E80
// SYNTHETIC: GAMEMD 0x0077B880 SYMBOL
// _$E83
const Rect RECT_NONE_2(0, 0, 0, 0);

// FUNCTION: GAMEMD 0x0077B8A0
// FUNCTION: GAMEMD 0x0077B8C0
OLEInitializer g_OLEInitializer;

// FUNCTION: GAMEMD 0x0077B8D0
// FUNCTION: GAMEMD 0x0077B8F0
Wstring g_MessageOfTheDay;


// FUNCTION: GAMEMD 0x0077B900
// FUNCTION: GAMEMD 0x0077B920
ArrayList<Server> g_Servers;

// FUNCTION: GAMEMD 0x0077B940
// FUNCTION: GAMEMD 0x0077B960
ArrayList<Channel> g_UserChannels;

// FUNCTION: GAMEMD 0x0077B980
// FUNCTION: GAMEMD 0x0077B9A0
ArrayList<Channel> g_GameChannelList;


// FUNCTION: GAMEMD 0x0077B9C0
// FUNCTION: GAMEMD 0x0077B9E0
ArrayList<User> g_UserList;

// FUNCTION: GAMEMD 0x0077BA00
// FUNCTION: GAMEMD 0x0077BA20
ArrayList<int> g_ChanListQueue;



enum WolEntryState {
	WOL_ENTRY_STATE_NONE
};

// FUNCTION: GAMEMD 0x0077BA40
// FUNCTION: GAMEMD 0x0077BAE0
Dictionary<Wstring, Ladder> g_BattleClansLadderDict HASH_ARG;

// FUNCTION: GAMEMD 0x0077BC10
// FUNCTION: GAMEMD 0x0077BCB0
Dictionary<Wstring, Ladder> g_WDTLadderDict HASH_ARG;

// FUNCTION: GAMEMD 0x0077BDE0
// FUNCTION: GAMEMD 0x0077BE80
Dictionary<Wstring, Ladder> g_GeneralLadderDict HASH_ARG;

// FUNCTION: GAMEMD 0x0077BFB0
// FUNCTION: GAMEMD 0x0077C050
Dictionary<Wstring, Highscore> DAT_00B76640 HASH_ARG;

// FUNCTION: GAMEMD 0x0077C180
// FUNCTION: GAMEMD 0x0077C220
Dictionary<Wstring, int> g_WDTTeams HASH_ARG;

// FUNCTION: GAMEMD 0x0077C350
// FUNCTION: GAMEMD 0x0077C370
ArrayList<Wstring> g_WDTTeamLookupQueue;

// FUNCTION: GAMEMD 0x0077C3B0
// FUNCTION: GAMEMD 0x0077C450
Dictionary<Wstring, int> g_UserLocales HASH_ARG;

// FUNCTION: GAMEMD 0x0077C580
// FUNCTION: GAMEMD 0x0077C5A0
ArrayList<Wstring> g_UserLocaleLookupQueue;

// FUNCTION: GAMEMD 0x0077C5E0
// FUNCTION: GAMEMD 0x0077C600
ArrayList<Wstring> g_WDT_NodWinEmphasisSounds;

// FUNCTION: GAMEMD 0x0077C640
// FUNCTION: GAMEMD 0x0077C660
ArrayList<Wstring> g_WDT_GDIWinEmphasisSounds;

// FUNCTION: GAMEMD 0x0077C6A0
// FUNCTION: GAMEMD 0x0077C6C0
ArrayList<Wstring> g_WDT_NodLoseEmphasisSounds;

// FUNCTION: GAMEMD 0x0077C700
// FUNCTION: GAMEMD 0x0077C720
ArrayList<Wstring> g_WDT_GDILoseEmphasisSounds;

// FUNCTION: GAMEMD 0x0077C760
// FUNCTION: GAMEMD 0x0077C780
ArrayList<Wstring> g_WDT_NodTerritoryWinSounds;

// FUNCTION: GAMEMD 0x0077C7C0
// FUNCTION: GAMEMD 0x0077C7E0
ArrayList<Wstring> g_WDT_GDITerritoryWinSounds;

// FUNCTION: GAMEMD 0x0077C820
// FUNCTION: GAMEMD 0x0077C840
ArrayList<Wstring> g_WDT_NodTerritoryLoseSounds;

// FUNCTION: GAMEMD 0x0077C880
// FUNCTION: GAMEMD 0x0077C8A0
ArrayList<Wstring> g_WDT_GDITerritoryLoseSounds;

// FUNCTION: GAMEMD 0x0077C8E0
// FUNCTION: GAMEMD 0x0077C900
ArrayList<Wstring> DAT_00B73AA8;

// FUNCTION: GAMEMD 0x0077C940
// FUNCTION: GAMEMD 0x0077C960
ArrayList<Wstring> DAT_00B76980;

// FUNCTION: GAMEMD 0x0077C9A0
// FUNCTION: GAMEMD 0x0077C9C0
ArrayList<Wstring> DAT_00B779B0;

// FUNCTION: GAMEMD 0x0077CA00
// FUNCTION: GAMEMD 0x0077CA20
Wstring DAT_00B73ACC;

// FUNCTION: GAMEMD 0x0077CA30
// FUNCTION: GAMEMD 0x0077CA50
Wstring DAT_00B76E24;

// FUNCTION: GAMEMD 0x0077CA60
// FUNCTION: GAMEMD 0x0077CB00
Dictionary<Wstring, WolEntryState> DAT_00B76990 HASH_ARG;

typedef Channel LadderC;

// FUNCTION: GAMEMD 0x0077CC30
// FUNCTION: GAMEMD 0x0077CCD0
Dictionary<Wstring, LadderC> DAT_00B76608 HASH_ARG;

extern int DAT_0084A1C4;
extern int DAT_0084A1C8;
extern int DAT_0084A1D4;
extern int DAT_0084A1D8;

// FUNCTION: GAMEMD 0x0077CE10
int DAT_00B765F0 = DAT_0084A1C8 - DAT_0084A1C4 - 4;

// FUNCTION: GAMEMD 0x0077CE30
int DAT_00B73E3C = DAT_0084A1D8 - DAT_0084A1D4 - 4;

// FUNCTION: GAMEMD 0x0077CE50
// FUNCTION: GAMEMD 0x0077CE70
ArrayList<Wstring> DAT_00B76AD0;

// FUNCTION: GAMEMD 0x0077CEB0
// FUNCTION: GAMEMD 0x0077CED0
ArrayList<Wstring> DAT_00B76970;

// FUNCTION: GAMEMD 0x0077CF10
// FUNCTION: GAMEMD 0x0077CF30
Wstring DAT_00B76AC4;

// FUNCTION: GAMEMD 0x0077CF40
bool FUN_0077CF40(void)
{
	int t = Session.WOLWelcomeChoice;
	return(t == 3 || t == 1 || t == 2);
}

// FUNCTION: GAMEMD 0x0077CF60
int FUN_0077CF60(void)
{
	return Session.WOLWelcomeChoice == 2;
}

// FUNCTION: GAMEMD 0x0077CF70
// FUNCTION: GAMEMD 0x0077CF90
Wstring DAT_00B76E34;


// FUNCTION: GAMEMD 0x0077CFA0
// FUNCTION: GAMEMD 0x0077CFC0
Wstring DAT_00B73B30;

// FUNCTION: GAMEMD 0x0077CFD0
// FUNCTION: GAMEMD 0x0077CFF0
Wstring DAT_00B7622C;

// FUNCTION: GAMEMD 0x0077D000
// FUNCTION: GAMEMD 0x0077D020
Wstring DAT_00B76908;

// FUNCTION: GAMEMD 0x0077D030
// FUNCTION: GAMEMD 0x0077D050
Wstring DAT_00B77794;

// FUNCTION: GAMEMD 0x0077D060
// FUNCTION: GAMEMD 0x0077D080
Wstring DAT_00B778F4;

// FUNCTION: GAMEMD 0x0077D090
// FUNCTION: GAMEMD 0x0077D0B0
Wstring DAT_00B778FC;

// FUNCTION: GAMEMD 0x0077D0C0
// FUNCTION: GAMEMD 0x0077D0E0
Wstring DAT_00B765EC;

// FUNCTION: GAMEMD 0x0077D0F0
// FUNCTION: GAMEMD 0x0077D110
Wstring DAT_00B76900;

// FUNCTION: GAMEMD 0x0077D120
// FUNCTION: GAMEMD 0x0077D140
rc_ptr<PlayerProfile> DAT_00B75E48;

// FUNCTION: GAMEMD 0x0077D160
// FUNCTION: GAMEMD 0x0077D190
CDTimerClass<SystemTimerClass> DAT_00B76478;

// FUNCTION: GAMEMD 0x0077D1A0
// FUNCTION: GAMEMD 0x0077D1C0
WideWstring DAT_00B76230;

class SimpleWonlineDialogControl : public OwnerDraw::SimpleDialogControl
{
	public:
		SimpleWonlineDialogControl(void);
		virtual ~SimpleWonlineDialogControl(void);
		virtual bool F4(void);
};

// FUNCTION: GAMEMD 0x0077D1D0
SimpleWonlineDialogControl::SimpleWonlineDialogControl(void)
{
}

// FUNCTION: GAMEMD 0x0077D1F0
SimpleWonlineDialogControl::~SimpleWonlineDialogControl(void)
{
}

bool WOL_Wait_Callback(void);

// FUNCTION: GAMEMD 0x0077D200
bool SimpleWonlineDialogControl::F4(void)
{
	return WOL_Wait_Callback();
}

// FUNCTION: GAMEMD 0x0077D210
void __fastcall FUN_0077D210(char * text)
{
	char * p = strchr(text, ' ');
	while (p) {
		*p = '_';
		p = strchr(p, ' ');
	}
}

// FUNCTION: GAMEMD 0x0077D240
int __fastcall Player_Name_To_Index(Wstring const & name)
{
	User * user = NULL;
	for (int i = 0; i < g_UserList.length(); i++) {
		g_UserList.getPointer(&user, i);
		if (strcmpi(name.text(), (char *)user->name) == 0)
			return i;
	}
	return -1;
}

extern int DAT_00B779C0;
extern int DAT_00B779DC;

// FUNCTION: GAMEMD 0x0077D2A0
bool __fastcall FUN_0077D2A0(User * user)
{
	if (!user)
		return false;
	if (user->flags & 1)
		return true;
	if (WS_Find_Dialog(0xC2))
		return true;
	if (WS_Find_Dialog(0xC9))
		return true;
	return DAT_00B779C0 != 0;
}

unsigned char __fastcall FUN_007B7D90(WideWstring * text);

// FUNCTION: GAMEMD 0x0077D2E0
bool __fastcall FUN_0077D2E0(User * user, WideWstring * text)
{
	if (!user)
		return false;
	if (user->flags & 1)
		return true;
	if (WS_Find_Dialog(0xC2))
		return true;
	if (WS_Find_Dialog(0xC9))
		return true;
	if (FUN_007B7D90(text))
		return DAT_00B779C0 != 0;
	return DAT_00B779DC != 0;
}

// FUNCTION: GAMEMD 0x0077D350
int __fastcall FUN_0077D350(wchar_t const * text)
{
	wchar_t buffer[256];
	swprintf(buffer, StringTable::LoadString("TXT_BUDDY_FILTER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x369));
	return _wcsnicmp(text, buffer, wcslen(buffer)) == 0;
}

// FUNCTION: GAMEMD 0x0077D3A0
int __fastcall FUN_0077D3A0(wchar_t const * text)
{
	wchar_t buffer[256];
	swprintf(buffer, StringTable::LoadString("TXT_CLAN_FILTER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x36F));
	return _wcsnicmp(text, buffer, wcslen(buffer)) == 0;
}

// FUNCTION: GAMEMD 0x0077D3F0
bool __fastcall FUN_0077D3F0(User * user)
{
	if (!user)
		return false;
	int count = DAT_00B73AA8.length();
	Wstring name;
	for (int i = 0; i < count; i++) {
		DAT_00B73AA8.get(name, i);
		if (strcmp(name.get(), (char *)user->name) == 0)
			return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x0077D490
bool __fastcall FUN_0077D490(User * a, User * b)
{
	if (!a)
		return false;
	if (!b)
		return false;
	if (strcmpi((char *)a->squadname, (char *)b->squadname) != 0)
		return false;
	else
		return true;
}

extern bool DAT_00B76AF0;
extern bool DAT_00B73AD0;
extern Wstring g_NickName;

// FUNCTION: GAMEMD 0x0077D4C0
bool __fastcall FUN_0077D4C0(User * user)
{
	if (DAT_00B76AF0 || DAT_00B73AD0) {
		HWND dialog = WS_Find_Dialog(0x113);
		if (dialog) {
			int index = Player_Name_To_Index(g_NickName);
			User * host = NULL;
			if (index >= 0) {
				g_UserList.getPointer(&host, index);
				if (DAT_00B76AF0) {
					if (DAT_00B73AD0) {
						if (FUN_0077D490(host, user))
							return true;
						return FUN_0077D3F0(user);
					}
					return FUN_0077D3F0(user);
				} else if (DAT_00B73AD0) {
					if (FUN_0077D490(host, user))
						return true;
				}
				return false;
			}
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x0077D710
bool FUN_0077D710(void)
{
	return WOL_Wait_Callback();
}


unsigned long __stdcall FUN_007C896E(unsigned long value);

// FUNCTION: GAMEMD 0x0077D720
IPXAddressClass __fastcall FUN_0077D720(User * user)
{
	NetNumType netnum;
	NetNodeType net;
	memset(&netnum, 0, sizeof(netnum));
	memset(&net, 0, sizeof(net));
	unsigned int ip_addr = FUN_007C896E(user->ipaddr);
	net[3] = ip_addr & 0xff;
	net[2] = (ip_addr >> 8) & 0xff;
	net[1] = (ip_addr >> 16) & 0xff;
	net[0] = (ip_addr >> 24) & 0xff;
	IPXAddressClass address(netnum, net, 0);
	return address;
}

#include "theme.h"
#include "scenario.h"
#include "_map.h"
#include "_keyboar.h"


extern bool UserInputLocked;
HWND FUN_005E26C0(void);
int __fastcall FUN_007759E0(HWND window, bool (*callback)(void), int a, int b, int c);
BOOL CALLBACK WOL_Find_Page_Dialog_Proc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam);
bool FUN_0077D820(void);

namespace UI {
	HWND __fastcall BeginDialog(int id, DLGPROC proc, int extra);
	bool Updated(void);
}

// FUNCTION: GAMEMD 0x0077D7A0
void FUN_0077D7A0(bool play)
{
	if (play && Theme.Still_Playing() == false) {
		Theme.Stop();
		Theme.Queue_Song(THEME_PICK_ANOTHER);
	} else if (play) {
		if (Theme.What_Is_Playing() == Theme.From_Name("INTRO")) {
			Theme.Stop();
			Theme.Queue_Song(THEME_PICK_ANOTHER);
		}
	} else if (!play) {
		Theme.Fade_Out();
	}
}

// FUNCTION: GAMEMD 0x0077D820
bool FUN_0077D820(void)
{
	UI::Updated();
	HWND top = FUN_005E26C0();
	if (top) {
		InvalidateRect(top, NULL, TRUE);
	}
	return false;
}

// FUNCTION: GAMEMD 0x0077D840
void DoFindPage(void)
{
	HWND dialog = UI::BeginDialog(0xCF, (DLGPROC)WOL_Find_Page_Dialog_Proc, 0);
	UserInputLocked = true;
	Keyboard->Clear();
	ShowWindow(dialog, SW_SHOWNORMAL);
	if (FUN_005E26C0()) {
		FUN_007759E0(dialog, FUN_0077D820, 0, 1, 1);
	} else {
		FUN_007759E0(dialog, UI::Updated, 0, 1, 1);
	}
	Keyboard->Clear();
	UserInputLocked = Scen->IsInputLocked;
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}

void __fastcall FUN_00773E90(const char * name);

// FUNCTION: GAMEMD 0x0077D8C0
void FUN_0077D8C0(void)
{
	FUN_00773E90("Battleclans");
}

// FUNCTION: GAMEMD 0x0077D8D0
void FUN_0077D8D0(void)
{
	FUN_00773E90("Battleclans");
}

// FUNCTION: GAMEMD 0x0077D8E0
void FUN_0077D8E0(void)
{
	FUN_00773E90("Ladder");
}

// FUNCTION: GAMEMD 0x0077D8F0
void FUN_0077D8F0(void)
{
	FUN_00773E90("News");
}

// FUNCTION: GAMEMD 0x0077D900
void FUN_0077D900(void)
{
	FUN_00773E90("Manage");
}

extern Channel g_CurrentChannel;

// FUNCTION: GAMEMD 0x0077D910
bool __fastcall FUN_0077D910(Channel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = false;
	return chan->tournament || (r = (chan->reserved >> 8) & 1) || r;
}

// FUNCTION: GAMEMD 0x0077D940
bool FUN_0077D940(void)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = false;
	if (g_CurrentChannel.tournament || (r = (g_CurrentChannel.reserved >> 8) & 1)) {
		return true;
	}
	return r;
}

// FUNCTION: GAMEMD 0x0077D970
bool FUN_0077D970(void)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = (g_CurrentChannel.reserved >> 8) & 1;
	return r;
}

// FUNCTION: GAMEMD 0x0077D990
bool __fastcall FUN_0077D990(Channel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return (chan->reserved >> 8) & 1;
}

// FUNCTION: GAMEMD 0x0077D9B0
bool __fastcall FUN_0077D9B0(Channel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return (chan->reserved >> 11) & 1;
}

// FUNCTION: GAMEMD 0x0077D9D0
bool __fastcall FUN_0077D9D0(Channel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return (chan->reserved >> 12) & 1;
}

// FUNCTION: GAMEMD 0x0077D9F0
// FUNCTION: GAMEMD 0x0077DA10
Wstring g_NickName;

// FUNCTION: GAMEMD 0x0077DA20
// FUNCTION: GAMEMD 0x0077DA40
Wstring g_LoginPassword;

extern int DAT_00B779C4;

// FUNCTION: GAMEMD 0x0077DA50
Ladder * __fastcall FUN_0077DA50(wchar_t const * name)
{
	Wstring key(FUN_007350C0(name));
	key.toLower();
	Ladder * result = new Ladder;
	if (!result) {
		return NULL;
	}
	memset(result, 0, sizeof(Ladder));
	Ladder * found = NULL;
	if (DAT_00B779C4 != 2) {
		if (g_GeneralLadderDict.getPointer(key, &found) && found) {
			*result = *found;
		}
	} else {
		if (g_BattleClansLadderDict.getPointer(key, &found) && found) {
			*result = *found;
		}
	}
	return result;
}

bool __fastcall FUN_005E93F0(NodeNameType * slot);
Wstring __fastcall FUN_005EDEF0(NodeNameType * slot);


// FUNCTION: GAMEMD 0x0077DB90
void FUN_0077DB90(void)
{
	char list[256];
	strcpy(list, "L");
	for (int i = 0; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		Wstring name;
		if (FUN_005E93F0(slot)) {
			WideWstring wide((wchar_t *)slot);
			name = FUN_007B7F90(wide);
		} else {
			name = FUN_005EDEF0(slot);
		}
		strncat(list, name.get(), 0x100);
		strncat(list, ",", 0x100);
	}
	g_pChat->RequestPublicGameOptions(list);
}

int ODMessageBox(const wchar_t * text, int type, bool (*callback)(void), bool large);
int __cdecl FUN_007CD455(const char * path);

// FUNCTION: GAMEMD 0x0077DC90
void __fastcall ViewHTML(const char * name, int no_ask)
{
	char filename[160];
	tmpnam(filename);
	strcat(filename, ".html");

	FILE * file = fopen(filename, "w");
	fprintf(file, "<title>Hi there.</title>");
	fclose(file);

	char exeName[MAX_PATH + 1];
	int result = (int)FindExecutable(filename, NULL, exeName);

	FUN_007CD455(filename);

	if ((result > 32) && (no_ask || (ODMessageBox(StringTable::LoadString("TXT_LAUNCHBROWSER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x576), 4, FUN_0077D710, 0) == IDYES))) {
		char commandLine[MAX_PATH + 10];
		sprintf(commandLine, "[open] %s", name);

		STARTUPINFO startupInfo;
		memset(&startupInfo, 0, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		PROCESS_INFORMATION processInfo;

		BOOL createSuccess = CreateProcess(exeName, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);

		if (createSuccess) {
			HANDLE process = processInfo.hProcess;
			if (process) {
				int done_waiting = FALSE;

				WOL_Wait_Callback();

				WaitForInputIdle(process, 5000);

				while (!done_waiting) {
					WOL_Wait_Callback();
					Sleep(500);

					DWORD exitCode;
					GetExitCodeProcess(process, &exitCode);
					if (exitCode != STILL_ACTIVE) {
						done_waiting = TRUE;
					}

					if (GameInFocus || GetTopWindow(NULL) == MainWindow || GetForegroundWindow() == MainWindow) {
						done_waiting = TRUE;
					}
					WOL_Wait_Callback();
				}

				WOL_Wait_Callback();

				if (GetTopWindow(NULL) != MainWindow) {
					WOL_Wait_Callback();
					SetForegroundWindow(MainWindow);
					ShowWindow(MainWindow, SW_RESTORE);
					WOL_Wait_Callback();
				}

				int waits = 10;
				while (waits) {
					Sleep(100);
					WOL_Wait_Callback();
					waits--;
				};
			}
			return;
		}
	} else if (result <= 32) {
		ODMessageBox(StringTable::LoadString("TXT_NOBROWSER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5BA), 0, FUN_0077D710, 0);
	}
}

extern CCINIClass INI_RA2MD;
extern int g_AllowPage;
extern int g_AllowFind;
extern int g_LangFilter;
extern int g_LobbyMusic;
extern int g_ShowAllChannels;
void FUN_005EE190(void);
void FUN_005EE1B0(void);
void __fastcall FUN_005EE350(HWND dialog);

// FUNCTION: GAMEMD 0x0077DED0
void Read_WOL_Settings(void)
{
	g_AllowPage = INI_RA2MD.Get_Int("WOnline", "AllowPage", 1);
	g_AllowFind = INI_RA2MD.Get_Int("WOnline", "AllowFind", 1);
	g_LangFilter = INI_RA2MD.Get_Int("WOnline", "LangFilter", 1);
	g_LobbyMusic = INI_RA2MD.Get_Int("WOnline", "LobMusic", 1);
	g_ShowAllChannels = INI_RA2MD.Get_Int("WOnline", "ShowAll", 1);
	DAT_00B779DC = INI_RA2MD.Get_Int("WOnline", "DisplayAsian", 1);
	DAT_00B779C0 = INI_RA2MD.Get_Int("WOnline", "DisplayLatin", 1);
	DAT_00B76AF0 = INI_RA2MD.Get_Bool("WOnline", "DisplayBuddies", false);
	DAT_00B73AD0 = INI_RA2MD.Get_Bool("WOnline", "DisplayClan", false);
	FUN_005EE190();
}

// FUNCTION: GAMEMD 0x0077DFD0
void Write_WOL_Settings(void)
{
	RawFileClass file("RA2MD.INI");
	INI_RA2MD.Put_Int("WOnline", "AllowPage", g_AllowPage);
	INI_RA2MD.Put_Int("WOnline", "AllowFind", g_AllowFind);
	INI_RA2MD.Put_Int("WOnline", "LangFilter", g_LangFilter);
	INI_RA2MD.Put_Int("WOnline", "LobMusic", g_LobbyMusic);
	INI_RA2MD.Put_Int("WOnline", "ShowAll", g_ShowAllChannels);
	INI_RA2MD.Put_Int("WOnline", "DisplayAsian", DAT_00B779DC);
	INI_RA2MD.Put_Int("WOnline", "DisplayLatin", DAT_00B779C0);
	INI_RA2MD.Put_Bool("WOnline", "DisplayBuddies", DAT_00B76AF0);
	INI_RA2MD.Put_Bool("WOnline", "DisplayClan", DAT_00B73AD0);
	INI_RA2MD.Save(file, false);
	FUN_005EE1B0();
}

// FUNCTION: GAMEMD 0x0077E110
void Apply_WOL_Settings(void)
{
	if (g_pChat) {
		g_pChat->SetLangFilter(g_LangFilter);
		g_pChat->SetFindPage(g_AllowFind, g_AllowPage);
	}
	FUN_005EE350(0);
	bool music = g_LobbyMusic != 0;
	FUN_0077D7A0(music);
}

// FUNCTION: GAMEMD 0x0077E1D0
int __fastcall Send_Chat_Message(WideWstring const & msg)
{
	static int sel_user_indexes[128];
	HWND users = GetDlgItem(FUN_00775B10(), 0x53E);
	int count = SendMessage(users, LB_GETSELITEMS, 32, (LPARAM)sel_user_indexes);

	if (count == 0) {
		g_pChat->RequestPublicUnicodeMessage(msg.text());
		return 0;
	}
	User * last = NULL;
	User * head = NULL;
	User * item = NULL;
	User source;
	memset(&source, 0, sizeof(source));
	for (int i = 0; i < count; i++) {
		g_UserList.get(source, sel_user_indexes[i]);
		item = new User;
		*item = source;
		item->next = NULL;
		if (i == 0) head = item;
		if (last) last->next = item;
		last = item;
	}
	if (head) g_pChat->RequestPrivateUnicodeMessage(head, msg.text());
	while (head) {
		item = head;
		head = head->next;
		delete item;
	}
	return 1;
}

// FUNCTION: GAMEMD 0x0077E300
int __fastcall Send_Chat_Action(WideWstring const & msg)
{
	static int sel_user_indexes[128];
	HWND users = GetDlgItem(FUN_00775B10(), 0x53E);
	int count = SendMessage(users, LB_GETSELITEMS, 32, (LPARAM)sel_user_indexes);

	if (count == 0) {
		g_pChat->RequestPublicUnicodeAction(msg.text());
		return 0;
	}
	User * last = NULL;
	User * head = NULL;
	User * item = NULL;
	User source;
	memset(&source, 0, sizeof(source));
	for (int i = 0; i < count; i++) {
		g_UserList.get(source, sel_user_indexes[i]);
		item = new User;
		*item = source;
		item->next = NULL;
		if (i == 0) head = item;
		if (last) last->next = item;
		last = item;
	}
	if (head) g_pChat->RequestPrivateUnicodeAction(head, msg.text());
	while (head) {
		item = head;
		head = head->next;
		delete item;
	}
	return 1;
}

#include "goptions.h"
namespace Game { extern int Seed; }
extern int Seed;

extern int g_RuleCRC;
extern int g_ArtCRC;
extern int g_AICRC;

// FUNCTION: GAMEMD 0x0077E780
int GetINIHash(void)
{
	return g_RuleCRC ^ g_AICRC ^ g_ArtCRC;
}

extern int DAT_00B76B1C;
extern int DAT_00B76B14;
struct ChannelUserInfo {
	char Name[20];
	int Color;
	int House;
	int Unknown;
	int Accepted;
	char Rest[0x70];
};
extern ChannelUserInfo g_UserInfo[16];
int FUN_00777B50(void);
int FUN_005E94D0(void);
bool FUN_005E9560(void);
bool FUN_005E9580(void);

// FUNCTION: GAMEMD 0x0077E7A0
void __fastcall Encode_Channel_ExInfo(char * out)
{
	int i = 0;
	int house1_count = 0;
	int house0_count = 0;
	for (int j = 0; j < DAT_00B76B1C; j++) {
		int house = g_UserInfo[j].House;
		if (house == 0) {
			house0_count++;
		} else if (house == 1) {
			house1_count++;
		}
	}
	if (house1_count < DAT_00B76B14 / 2) {
		i = 2;
	}
	if (house0_count < DAT_00B76B14 / 2) {
		i++;
	}
	sprintf(out, "%d,%d,%d,%d,%d,%s", FUN_00777B50(), GetINIHash(), FUN_005E94D0(), FUN_005E9580(), !FUN_005E9560(), Session.ScenarioFileName);
}

// FUNCTION: GAMEMD 0x0077E820
void __fastcall FUN_0077E820(char * text, int * first, int * second, int * third, int * fourth, int * fifth, Wstring * rest)
{
	char * token = strtok(text, ",");
	*first = token ? atol(token) : 0;
	token = strtok(NULL, ",");
	*second = token ? atol(token) : 0;
	token = strtok(NULL, ",");
	*third = token ? atol(token) : 0;
	token = strtok(NULL, ",");
	*fourth = token ? atol(token) : 0;
	token = strtok(NULL, ",");
	*fifth = token ? atol(token) : 0;
	*rest = strtok(NULL, ",");
}

extern bool g_GameStartRequested;
extern int g_IsChannelCreator;
extern int DAT_00B77E14;
extern int DAT_00B73A20;
extern int DAT_00B77D70;
extern int DAT_00B77A88;

void __fastcall Draw_Player_List(int keep_selection, bool flag);

WideWstring __fastcall FUN_007B7ED0(Wstring const & text);

void __fastcall FUN_004E34A0(HWND dialog, int side, int flag);
extern int DAT_00B76440[];
void __cdecl FUN_005E2700(int color, wchar_t const * text, ...);
void __fastcall FUN_005EDDE0(HWND dialog, bool enable);
void FUN_005E9850(void);
void FUN_0072CF90(void);
void __fastcall FUN_005E9340(char const * name);

static inline Wstring Narrow_Text(wchar_t const * text)
{
	WideWstring wide(text);
	return FUN_007B7F90(wide);
}

// FUNCTION: GAMEMD 0x0077E900
void __fastcall SetPlayerAccepted(Wstring const & who, int status)
{
	Wstring name(who.length() > 0 ? who : g_NickName);
	if (name == g_NickName && status == 0) {
		HWND w = WS_Find_Dialog(0xC9);
		FUN_005EDDE0(w, true);
	}
	int index = Player_Name_To_Index(name);
	if (index != -1) {
		g_UserInfo[index].Accepted = status;
		Draw_Player_List(0, false);
	}
	bool pair = g_IsChannelCreator == 1;
	Wstring lower = ((Wstring const &)name).toLower();
	int i;
	for (i = 0; i < Session.Players.Count(); i++) {
		Wstring slot_name = Narrow_Text(Session.Players[i]->Name);
		if (((Wstring const &)slot_name).toLower() == lower) {
			break;
		}
	}
	if (i >= Session.Players.Count()) {
		return;
	}
	Session.Players[i]->Slot.Unknown77 = status;
	if (pair && (Session.WOLWelcomeChoice == 3 || Session.WOLWelcomeChoice == 1 || Session.WOLWelcomeChoice == 2) && !g_GameStartRequested && i == 1 && g_UserList.length() == 2) {
		User *temp, *head = NULL;
		User source;
		memset(&source, 0, sizeof(source));
		for (int k = 0; k < g_UserList.length(); k++) {
			g_UserList.get(source, k);
			temp = new User;
			*temp = source;
			temp->next = head;
			head = temp;
		}
		DAT_00B77E14 = 0;
		DAT_00B73A20 = 9;
		DAT_00B77D70 = 90;
		DAT_00B77A88 = time(0);
		g_pChat->RequestGameStart(head);
		g_GameStartRequested = true;
		temp = head;
		while (temp) {
			head = temp->next;
			delete temp;
			temp = head;
		}
	}
	Draw_Player_List(0, false);
}

// FUNCTION: GAMEMD 0x0077EBA0
int __fastcall GetPlayerAccepted(Wstring const & who)
{
	Wstring name(who.length() > 0 ? who : g_NickName);
	Wstring lower = ((Wstring const &)name).toLower();
	if (FUN_007B7ED0(lower) == ((WideWstring const &)WideWstring(Session.GameName)).toLower()) {
		return 1;
	}
	int found = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		Wstring slot_name = Narrow_Text(Session.Players[i]->Name);
		if (((Wstring const &)slot_name).toLower() == lower) {
			found = i;
		}
	}
	if (found != -1) {
		return Session.Players[found]->Slot.Unknown77;
	}
	return -1;
}

int __fastcall FUN_004E37D0(int side);
int __fastcall FUN_004E41D0(int side);
int __fastcall FUN_004E4170(HWND dialog, int id, int flag);
void __fastcall FUN_004E3A00(HWND dialog, int id);
void __fastcall FUN_004E3B00(HWND dialog, int id);
void __fastcall FUN_004E45A0(HWND dialog, int id);
void __fastcall FUN_004E4770(HWND dialog, int id);
void __fastcall FUN_004E49A0(HWND dialog, int id, int flag);
int __fastcall FUN_004E4E60(int slot);
int __fastcall FUN_004E5940(int id);
void __fastcall FUN_004E5260(HWND dialog, int id);
void __fastcall FUN_004E5CB0(HWND dialog, int id);
void __fastcall FUN_004E50C0(HWND dialog, int id);
void __fastcall FUN_004E5B60(HWND dialog, int id);
int __fastcall FUN_005E98F0(int kind);

// FUNCTION: GAMEMD 0x0077ED00
int __fastcall Assign_House_And_Color(Wstring const & who, int country, int color, int start, int team)
{
	int index = -1;
	Wstring lower = who.toLower();
	for (int j = 0; j < 8; j++) {
		Wstring slot_name = Narrow_Text(Session.Slots[j]->Name);
		if (lower == ((Wstring const &)slot_name).toLower()) {
			index = j;
			break;
		}
	}
	int result = 0;
	if (index == -1) {
		return 0;
	}
	HWND dialog = WS_Find_Dialog(0xC2);
	if (dialog == NULL) {
		dialog = WS_Find_Dialog(0xC9);
	}
	NodeNameType * slot = Session.Slots[index];
	if (slot->Get_Start() != start) {
		result = 1;
		slot->Set_Start(start);
	}
	if (Session.Slots[index]->Get_Team() != team) {
		result = 1;
		slot->Set_Team(team);
	}
	if (Session.Slots[index]->Get_Country() != country) {
		result = 1;
		slot->Set_Country(country);
		int combo = FUN_004E37D0(index);
		int selection = FUN_004E4170(dialog, combo, -1);
		if (country == -3) {
			if (selection != -3) {
				FUN_004E3B00(dialog, combo);
				FUN_004E5260(dialog, FUN_004E4E60(index));
				FUN_004E5CB0(dialog, FUN_004E5940(index));
				int item = FUN_005E98F0(index);
				SendMessage(GetDlgItem(dialog, item), 0x4F1, 0, 1);
				combo = FUN_004E41D0(index);
				FUN_004E4770(dialog, combo);
				FUN_004E49A0(dialog, combo, 8);
			}
		} else if (selection == -3) {
			FUN_004E3A00(dialog, combo);
			FUN_004E50C0(dialog, FUN_004E4E60(index));
			FUN_004E5B60(dialog, FUN_004E5940(index));
			int item = FUN_005E98F0(index);
			SendMessage(GetDlgItem(dialog, item), 0x4F1, 0, 0);
			combo = FUN_004E41D0(index);
			FUN_004E45A0(dialog, combo);
			FUN_004E49A0(dialog, combo, -2);
		}
		if (slot->Get_Country() == -3) {
			slot->Slot.SpectatorFlag = -1;
		}
	}
	if (Session.Slots[index]->Get_Color() != color) {
		slot->Set_Color(color);
	}
	if (Session.Slots[index] == Session.Players[0]) {
		Session.PrefColor = color;
	}
	Draw_Player_List(0, false);
	return result;
}

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x0077EF90
bool __fastcall FUN_0077EF90(int type, unsigned char ping)
{
	FUN_004068E0("Requesting channel list type = %d, ping = %d\n", type, ping);
	if (g_pChat->RequestChannelList(type, ping) != 0) {
		return false;
	}
	g_ChanListQueue.addTail(type);
	return true;
}

extern HANDLE g_WaitEventHandles[18];
extern unsigned int g_ActiveWaitFlags;
extern wchar_t g_LastErrorMessage[];
extern char DAT_0084A584[];
void __fastcall Show_Wait_Window(int id, WideWstring const & text, char flag);
bool __fastcall FUN_007757E0(HWND window, int flag);

static inline void Focus_Chat_Input(void)
{
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags &= ~1;
		if (g_ActiveWaitFlags == 0) {
			FUN_007757E0(dialog, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
}

// FUNCTION: GAMEMD 0x0077F120
bool FUN_0077F120(void)
{
	Show_Wait_Window(1, StringTable::LoadString("TXT_REQ_CHANLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x8FA), 0);
	int timeout = 0;
	DWORD index;
	bool ok;
	do {
		while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT) {
			timeout += 100;
			if (timeout > 30000) {
				swprintf(g_LastErrorMessage, StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x903));
				SetEvent(g_WaitEventHandles[0]);
				return false;
			}
			WOL_Wait_Callback();
		}
		if (index == (DWORD)-1) {
			Focus_Chat_Input();
			FUN_004068E0(DAT_0084A584);
			return false;
		}
		if (index & 0x80) {
			index -= 0x80;
		}
		if (index >= 18) {
			return false;
		}
		ResetEvent(g_WaitEventHandles[index]);
		if (index == 5 || index == 0) {
			ok = false;
			break;
		}
		if (index == 3) {
			ok = true;
			break;
		}
	} while (true);
	if (!ok) {
		Focus_Chat_Input();
		return false;
	}
	Focus_Chat_Input();
	return true;
}


void FUN_00798340(void);

// FUNCTION: GAMEMD 0x0077F320
bool __fastcall FUN_0077F320(int type, unsigned char ping, bool wait)
{
	if (g_ChanListQueue.length() != 0) {
		if (!FUN_0077F120()) {
			return false;
		}
	}
	FUN_00798340();
	if (!FUN_0077EF90(type, ping)) {
		SetEvent(g_WaitEventHandles[0]);
		return false;
	}
	if (wait) {
		return FUN_0077F120();
	}
	return true;
}

// FUNCTION: GAMEMD 0x0077F520
bool __fastcall FUN_0077F520(bool wait)
{
	return FUN_0077F320(0, 0, wait);
}

// FUNCTION: GAMEMD 0x0077F710
bool __fastcall FUN_0077F710(unsigned char ping, bool wait)
{
	return FUN_0077F320(0x29, ping, wait);
}

extern int g_NextWDTStatePollTime;
extern unsigned int g_LastChannelLocaleRequestTime;
extern unsigned int g_LastWDTUserTeamRequestTime;

extern Dictionary<Wstring, Ladder> * g_ActiveLadder;
extern int g_InstalledLadderSKU;
extern int g_SelectedLadderSKU;
extern int g_GameSKU;
extern Channel s_TempChannel;
extern Server g_GameServer;
extern Server g_WDTServer;
extern int WDT_TERRITORY_LOBBY_INDEXES[8];
extern char g_OwnSquadName[0x29];
extern int g_CurrentServerIndex;
extern int g_TargetServerIndex;
extern char g_IgnoreNetStatus;
extern char DAT_00B77DC1;
extern char g_ConnectionLost;
extern char g_ShowMOTD;
extern int DAT_0084A1BC;
extern int g_SuspendChatPump;
extern int DAT_00B77DB8;
extern int DAT_00B77DBC;
extern wchar_t const * g_Lobbies[8];
extern DWORD g_dwChatAdvise;
extern DWORD g_dwNetUtilAdvise;
extern DWORD g_dwDownloadAdvise;
extern int g_SquadInfoRequests;
extern int g_RequestingOwnSquadInfo;
extern int g_OwnSquadID;
extern int g_ChannelCount;
extern int g_LadderPos;
extern int g_JoinLobbyNow;
extern int g_GameStarted;
extern int g_UserAge;
extern int g_UserConsent;
extern int DAT_00B77E20;


void __fastcall PumpGameopts(bool force, bool now);
int FUN_00775B20(void);
void Call_Back(void);
bool Handle_Preview_Download(void);
bool Get_WDT_State_Silent(void);

int WS_Top_Window_ID(void);
void Title_Screen_Restore(bool force);
BOOL __fastcall Peek_Windows_Message(MSG * message, HWND window, UINT first, UINT last, UINT flags);
extern int MessageLoopCount;
extern int DAT_00B77E6C;
extern int DAT_00B77E70;
extern bool DAT_00B77E26;
extern bool DAT_00B77E27;
void __fastcall FUN_0079FC10(int state, int timeout, int result);
void Start_Game_Now(void);

// FUNCTION: GAMEMD 0x0077F900
bool WOL_Wait_Callback(void)
{
	Title_Screen_Restore(false);
	MSG msg;
	int counter = 0;
	MessageLoopCount++;
	while (Peek_Windows_Message(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		counter++;
		if (counter == 100) break;
	}
	if (FUN_0077CF40() && DAT_00B77D70 != -1 && DAT_00B73A20 != 1 && DAT_00B73A20 != 0 && DAT_00B73A20 != 11) {
		if ((unsigned)(time(NULL) - DAT_00B77A88) > (unsigned)DAT_00B77D70) {
			if (DAT_00B73A20 != 5) {
				if (DAT_00B73A20 >= 2 && DAT_00B73A20 <= 4) {
					FUN_0079FC10(0, -1, 2);
				} else {
					PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchTimeout", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x97A));
					FUN_0079FC10(2, 30, 0);
				}
				SetEvent(g_WaitEventHandles[5]);
			} else {
				FUN_0079FC10(5, 180, 0);
			}
		}
	}
	WS_Top_Window_ID();
	if (g_WDTTeamLookupQueue.length() > 200) {
		g_WDTTeamLookupQueue.clear();
	}
	if (g_LastWDTUserTeamRequestTime < (unsigned)time(0) - 2 && g_WDTTeamLookupQueue.length() > 65 ||
		g_LastWDTUserTeamRequestTime < (unsigned)time(0) - 3 && g_WDTTeamLookupQueue.length() > 10 ||
		g_LastWDTUserTeamRequestTime < (unsigned)time(0) - 15 && g_WDTTeamLookupQueue.length() > 0) {
		User * list = NULL;
		User * last_user = NULL;
		for (int i = 0; i < 10; i++) {
			if (g_WDTTeamLookupQueue.length() > 0) {
				User * new_user = new User;
				memset(new_user, 0, sizeof(User));
				Wstring name;
				g_WDTTeamLookupQueue.removeTail(name);
				char * rawName = name.get();
				if (strlen(rawName) > 9) rawName[9] = '\0';
				strcpy((char *)new_user->name, rawName);
				if (last_user == NULL) list = new_user;
				else last_user->next = new_user;
				last_user = new_user;
			} else break;
		}
		if (g_pChat != NULL) g_pChat->RequestUserTeam(list);
		User * d = list;
		while (d != NULL) {
			User * next = d->next;
			delete d;
			d = next;
		}
		g_LastWDTUserTeamRequestTime = time(0);
	}
	if (g_UserLocaleLookupQueue.length() > 200) {
		g_UserLocaleLookupQueue.clear();
	}
	if (g_LastChannelLocaleRequestTime < (unsigned)time(0) - 2 && g_UserLocaleLookupQueue.length() > 65 ||
		g_LastChannelLocaleRequestTime < (unsigned)time(0) - 3 && g_UserLocaleLookupQueue.length() > 10 ||
		g_LastChannelLocaleRequestTime < (unsigned)time(0) - 15 && g_UserLocaleLookupQueue.length() > 0) {
		User * list = NULL;
		User * last_user = NULL;
		for (int i = 0; i < 10; i++) {
			if (g_UserLocaleLookupQueue.length() > 0) {
				User * new_user = new User;
				memset(new_user, 0, sizeof(User));
				Wstring name;
				g_UserLocaleLookupQueue.removeTail(name);
				char * rawName = name.get();
				if (strlen(rawName) > 9) rawName[9] = '\0';
				strcpy((char *)new_user->name, rawName);
				if (last_user == NULL) {
					list = new_user;
					last_user = new_user;
				} else {
					last_user->next = new_user;
					last_user = new_user;
				}
			} else break;
		}
		if (g_pChat != NULL) g_pChat->RequestUserLocale(list);
		User * d = list;
		while (d != NULL) {
			User * next = d->next;
			delete d;
			d = next;
		}
		g_LastChannelLocaleRequestTime = time(0);
	}
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3 && time(0) > g_NextWDTStatePollTime) {
		g_NextWDTStatePollTime = time(0) + 30;
		Get_WDT_State_Silent();
	}
	if (g_pChat && g_SuspendChatPump == 0) g_pChat->PumpMessages();
	if (g_pNetUtil) g_pNetUtil->PumpMessages();
	if (g_pDownload) g_pDownload->PumpMessages();
	Call_Back();
	if (g_CurrentChannel.name[0] && g_CurrentChannel.type == 41 && g_IsChannelCreator) PumpGameopts(false, false);
	if (g_pChat != NULL && g_CurrentChannel.type != 0 && g_IsChannelCreator) {
		if (time(NULL) - DAT_00B77E6C > 10) {
			char info[80];
			memset(info, 0, sizeof(info));
			Encode_Channel_ExInfo(info);
			info[40] = 0;
			g_pChat->SetChannelExInfo(info);
			DAT_00B77E6C = time(NULL);
		}
	}
	if (g_GameStarted != 2) {
		if (g_pChat && g_CurrentChannel.type != 0 && time(NULL) - DAT_00B77E70 > 10) {
			Wstring self = ((Wstring const &)g_NickName).toLower();
			if (!FUN_0077CF40()) {
				User user;
				int handle;
				for (int i = 0; i < g_UserList.length(); i++) {
					g_UserList.get(user, i);
					Wstring name((char *)user.name);
					bool is_self = self == ((Wstring const &)name).toLower();
					if (is_self) continue;
					in_addr address;
					address.s_addr = user.ipaddr;
					if (user.ipaddr != 0) {
						g_pNetUtil->RequestPing(inet_ntoa(address), 1000, &handle);
					}
				}
			}
			DAT_00B77E70 = time(NULL);
		}
	}
	if (g_GameStarted == 2 && DAT_00B77E27 && !DAT_00B77E26) Start_Game_Now();
	Handle_Preview_Download();
	return false;
}

// FUNCTION: GAMEMD 0x00780110
void Reset_WOL_Globals(void)
{
	DAT_00B75E48 = FUN_00778710();
	g_UserChannels.clear();
	g_GameChannelList.clear();
	g_UserList.clear();
	g_WDTTeamLookupQueue.clear();
	g_WDTTeams.clear();
	g_UserLocaleLookupQueue.clear();
	g_UserLocales.clear();
	g_NextWDTStatePollTime = time(NULL) + 86400;
	g_LastChannelLocaleRequestTime = 0;
	g_LastWDTUserTeamRequestTime = 0;
	g_BattleClansLadderDict.clear();
	g_WDTLadderDict.clear();
	DAT_00B76608.clear();
	DAT_00B73AA8.clear();
	DAT_00B76980.clear();
	DAT_00B76990.clear();
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		int sku = 0x2300;
		g_InstalledLadderSKU = sku;
		g_SelectedLadderSKU = sku;
		g_ActiveLadder = &g_WDTLadderDict;
	} else {
		int sku = g_GameSKU;
		g_InstalledLadderSKU = sku;
		g_SelectedLadderSKU = sku;
		g_ActiveLadder = &g_GeneralLadderDict;
	}
	memset(&g_CurrentChannel, 0, 0x10C);
	memset(&s_TempChannel, 0, 0x10C);
	memset(&g_GameServer, 0, sizeof(g_GameServer));
	memset(&g_WDTServer, 0, sizeof(g_WDTServer));
	g_LastErrorMessage[0] = 0;
	DAT_00B77E14 = 0;
	DAT_00B73A20 = 1;
	DAT_00B77D70 = -1;
	DAT_00B77A88 = time(NULL);
	g_CurrentServerIndex = 0;
	g_TargetServerIndex = 0;
	g_IsChannelCreator = 0;
	g_ActiveWaitFlags = 0;
	g_GameStartRequested = 0;
	g_IgnoreNetStatus = 0;
	g_ShowMOTD = 1;
	g_MessageOfTheDay.clear();
	memset(g_WaitEventHandles, 0, sizeof(g_WaitEventHandles));
	DAT_00B77DC1 = 0;
	g_ConnectionLost = 0;
	DAT_0084A1BC = -1;
	g_SuspendChatPump = 0;
	DAT_00B77DB8 = -1;
	DAT_00B77DBC = -1;
	g_Lobbies[0] = StringTable::LoadString("TXT_LOB_1", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC0);
	g_Lobbies[1] = StringTable::LoadString("TXT_LOB_2", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC1);
	g_Lobbies[2] = StringTable::LoadString("TXT_LOB_3", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC2);
	g_Lobbies[3] = StringTable::LoadString("TXT_LOB_4", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC3);
	g_Lobbies[4] = StringTable::LoadString("TXT_LOB_5", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC4);
	g_Lobbies[5] = StringTable::LoadString("TXT_LOB_6", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC5);
	g_Lobbies[6] = StringTable::LoadString("TXT_LOB_7", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC6);
	g_Lobbies[7] = StringTable::LoadString("TXT_LOB_8", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC7);
	memset(WDT_TERRITORY_LOBBY_INDEXES, -1, sizeof(WDT_TERRITORY_LOBBY_INDEXES));
	g_pChat = NULL;
	g_pNetUtil = NULL;
	g_pDownload = NULL;
	g_dwChatAdvise = 0;
	g_dwNetUtilAdvise = 0;
	g_dwDownloadAdvise = 0;
	g_SquadInfoRequests = 0;
	g_RequestingOwnSquadInfo = 0;
	memset(g_OwnSquadName, 0, sizeof(g_OwnSquadName));
	g_OwnSquadID = 0;
	g_ChannelCount = 0;
	g_LadderPos = 1;
	g_JoinLobbyNow = 0;
	g_ChanListQueue.clear();
	g_GameStarted = 0;
	g_UserAge = 0;
	g_UserConsent = 0;
	g_RuleCRC = RulesClass::Get_Rule_Unique_ID();
	RulesClass::Load_Art_INI();
	g_ArtCRC = RulesClass::Get_Art_Unique_ID();
	g_AICRC = RulesClass::Get_AI_Unique_ID();
	DAT_00B77E20 = 0;
}

int __fastcall FUN_00623F00(HWND list, int index, Wstring * text);
int __fastcall FUN_00623E00(HWND list);
extern int DAT_00B76C1C;
extern wchar_t DAT_00887734[];

// FUNCTION: GAMEMD 0x007806C0
void FUN_007806C0(void)
{
	HWND dialog = WS_Find_Dialog(0x113);
	if (!dialog) {
		return;
	}
	HWND list = GetDlgItem(dialog, 0x53D);
	if (!list) {
		return;
	}
	Wstring top;
	int topIndex = SendMessage(list, LB_GETTOPINDEX, 0, 0);
	if (topIndex != -1) {
		FUN_00623F00(list, topIndex, &top);
	}
	SendMessage(list, 0x49D, 0, 1);
	FUN_00623E00(list);
	int count = g_UserChannels.length();
	for (int i = 0; i < count; i++) {
		Channel chan;
		g_UserChannels.get(chan, i);
		if (strncmp((char *)chan.name, "Lob_41_", strlen("Lob_41_")) == 0) {
			int number = atol((char *)chan.name + strlen("Lob_41_"));
			bool current = strcmp((char *)chan.name, (char *)g_CurrentChannel.name) == 0;
			int copy = number / 8;
			int slot = number % 8;
			wchar_t info[80];
			if (copy == 0) {
				swprintf(info, L"%s  (%d)", g_Lobbies[slot], chan.currentUsers);
			} else {
				swprintf(info, L"%s_%d  (%d)", g_Lobbies[slot], copy + 1, chan.currentUsers);
			}
			int entries = SendMessage(list, LB_GETCOUNT, 0, 0);
			int pos;
			for (pos = 0; pos < entries; pos++) {
				int item = SendMessage(list, LB_GETITEMDATA, pos, 0);
				Channel other;
				g_UserChannels.get(other, item);
				if (atol((char *)other.name + strlen("Lob_41_")) > number) {
					break;
				}
			}
			int row = SendMessage(list, 0x4C7, pos, (LPARAM)chan.name);
			SendMessage(list, LB_SETITEMDATA, row, i);
			OwnerDraw::CellData cell;
			cell.type = OwnerDraw::CellData::TEXT;
			cell.string.set(info);
			cell.hint.set(DAT_00887734);
			SendMessage(list, 0x4A8, MAKEWPARAM(DAT_00B76C1C, row), (LPARAM)&cell);
			if (current) {
				SendMessage(list, LB_SETCURSEL, row, 0);
			}
		}
	}
	int found = SendMessage(list, 0x4C5, 0, (LPARAM)top.text());
	if (found != -1) {
		SendMessage(list, LB_SETTOPINDEX, found, 0);
	}
	SendMessage(list, 0x49D, 0, 0);
	InvalidateRect(list, NULL, 0);
}

// FUNCTION: GAMEMD 0x00780A00
WideWstring __fastcall FUN_00780A00(int locale)
{
	Wstring key = "WOL:Locale" + Wstring_FromInt(locale, 2);
	WideWstring name(StringTable::LoadString(key.get(), NULL, "D:\\ra2mdpost\\wonline.cpp", 0xB5D));
	if (locale < 0x25) {
		return name;
	}
	return name;
}

WideWstring __fastcall FUN_007B8090(wchar_t const * prefix, WideWstring const & text);
WideWstring __fastcall FUN_007B7DD0(int value);

// FUNCTION: GAMEMD 0x00780A90
WideWstring __fastcall FUN_00780A90(User * user)
{
	Wstring name((char *)user->name);
	WideWstring text = FUN_007B7ED0(name) + L" : ";
	name.toLower();
	Ladder ladder;
	memset(&ladder, 0, sizeof(ladder));
	if (g_ActiveLadder && g_ActiveLadder->getValue(name, ladder)) {
		text += WideWstring(StringTable::LoadString("TXT_RANK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xB79)) + WideWstring(L" ") + FUN_007B7DD0(ladder.rung);
	} else {
		text += StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xB7B);
	}
	Wstring login((char *)user->squadname);
	if (login.length() == 0) {
		login = (char *)user->squadabbrev;
	}
	if (login.length() > 0) {
		text += FUN_007B8090(L": ", FUN_007B7ED0(login));
	}
	if (g_UserLocales.contains(name)) {
		if (g_pChat) {
			int locale = 0;
			g_UserLocales.getValue(name, locale);
			if (locale) {
				text += FUN_007B8090(L" : ", FUN_00780A00(locale));
			}
		}
	}
	return text;
}

// FUNCTION: GAMEMD 0x00780D90
WideWstring __fastcall FUN_00780D90(Wstring * file)
{
	if (strcmpi(file->text(), "RandMap.Sed") == 0) {
		return WideWstring(StringTable::LoadString("TXT_RANDOM_MAP_DESCRIPTION", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xB9A));
	}
	for (int i = 0; i < Session.Scenarios.Count(); i++) {
		MultiMission * map = Session.Scenarios[i];
		if (strcmpi(file->text(), map->Get_Filename()) == 0) {
			return WideWstring(map->Description());
		}
	}
	return WideWstring(DAT_00887734);
}

// FUNCTION: GAMEMD 0x00780E30
WideWstring __fastcall FUN_00780E30(HWND list, int index)
{
	if (index == -1) {
		return WideWstring(StringTable::LoadString("STT:LobbyListGames", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBAB));
	}
	Channel chan;
	memset(&chan, 0, sizeof(chan));
	int item = SendMessage(list, LB_GETITEMDATA, index, 0);
	if (item >= 0 && item < g_GameChannelList.length()) {
		g_GameChannelList.get(chan, item);
		Wstring name((char *)chan.name);
		name.truncate((char)0x27);
		User user;
		memset(&user, 0, sizeof(user));
		strncpy((char *)user.name, name.get(), 10);
		WideWstring text = FUN_00780A90(&user);
		text += WideWstring(L", ");
		if (chan.latency != -1) {
			wchar_t ping[64];
			swprintf(ping, StringTable::LoadString("MSG:PingInfo", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBCD), chan.latency);
			text += ping;
		} else {
			text += StringTable::LoadString("TXT_UNKNOWN_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBD0);
		}
		char details[0x84];
		strncpy(details, (char *)chan.exInfo, 0x80);
		int first = 0;
		int second = 0;
		int third = 0;
		int fourth = 0;
		int fifth = 0;
		Wstring file;
		FUN_0077E820(details, &first, &second, &third, &fourth, &fifth, &file);
		if (file.length() > 0) {
			WideWstring description = FUN_00780D90(&file);
			if (description.length() > 0) {
				text += L", ";
				text += description;
			}
		}
		return text;
	}
	return WideWstring(StringTable::LoadString("STT:LobbyListGames", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBB2));
}

// FUNCTION: GAMEMD 0x00781090
WideWstring __fastcall FUN_00781090(HWND list, int index)
{
	if (index == -1) {
		return WideWstring(StringTable::LoadString("STT:LobbyListUsers", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBEA));
	}
	int item = SendMessage(list, LB_GETITEMDATA, index, 0);
	if (item >= 0 && item < g_UserList.length()) {
		User user;
		memset(&user, 0, sizeof(user));
		g_UserList.get(user, item);
		return FUN_00780A90(&user);
	}
	return WideWstring(StringTable::LoadString("STT:LobbyListUsers", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBEF));
}

extern int DAT_00B77E44;

// FUNCTION: GAMEMD 0x00781130
bool __fastcall FUN_00781130(Channel * chan, int mode, int a, int b)
{
	int filter = DAT_00B77E44;
	if ((filter == 2 && mode == 1) || (filter == 3 && mode == 0)) {
		if (chan->flags & 0x100) {
			return true;
		}
	}
	if ((filter == 2 && mode == 0) || (filter == 3 && mode == 1)) {
		if (!(chan->flags & 0x100)) {
			return true;
		}
	}
	if ((filter == 4 && mode == 1) || (filter == 5 && mode == 0)) {
		if (a || b) {
			return true;
		}
	}
	if ((filter == 4 && mode == 0) || (filter == 5 && mode == 1)) {
		if (!a && !b) {
			return true;
		}
	}
	if ((filter == 6 && mode == 1) || (filter == 7 && mode == 0)) {
		if ((chan->reserved >> 9) & 3) {
			return true;
		}
	}
	if ((filter == 6 && mode == 0) || (filter == 7 && mode == 1)) {
		if (!((chan->reserved >> 9) & 3)) {
			return true;
		}
	}
	if ((filter == 8 && mode == 1) || (filter == 9 && mode == 0)) {
		if (chan->tournament) {
			return true;
		}
	}
	if ((filter == 8 && mode == 0) || (filter == 9 && mode == 1)) {
		if (!chan->tournament) {
			return true;
		}
	}
	if ((filter == 10 && mode == 1) || (filter == 11 && mode == 0)) {
		if (FUN_0077D990(chan)) {
			return true;
		}
	}
	if ((filter == 10 && mode == 0) || (filter == 11 && mode == 1)) {
		if (!FUN_0077D990(chan)) {
			return true;
		}
	}
	if ((filter == 12 && mode == 1) || (filter == 13 && mode == 0)) {
		if (!FUN_0077D910(chan)) {
			return true;
		}
	}
	if ((filter == 12 && mode == 0) || (filter == 13 && mode == 1)) {
		if (FUN_0077D910(chan)) {
			return true;
		}
	}
	return false;
}

struct SortEntry {
	int Pad;
	int Key;
};

// FUNCTION: GAMEMD 0x00781380
int __cdecl FUN_00781380(SortEntry const * a, SortEntry const * b)
{
	int x = a->Key;
	int y = b->Key;
	if (x < y) {
		return x == -1 ? 1 : -1;
	}
	if (x > y) {
		return y == -1 ? -1 : 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007813B0
int __cdecl FUN_007813B0(SortEntry const * a, SortEntry const * b)
{
	int x = a->Key;
	int y = b->Key;
	if (x < y) {
		return 1;
	}
	if (x > y) {
		return -1;
	}
	return 0;
}


typedef int (__cdecl * CompareFn)(const void *, const void *);

// FUNCTION: GAMEMD 0x007813E0
void __fastcall FUN_007813E0(SortEntry * entries, bool descending)
{
	Channel chan;
	memset(&chan, 0, sizeof(chan));
	int count = g_GameChannelList.length();
	for (int i = 0; i < count; i++) {
		g_GameChannelList.get(chan, i);
		entries[i].Pad = i;
		entries[i].Key = chan.latency;
	}
	if (descending) {
		qsort(entries, count, sizeof(SortEntry), (CompareFn)FUN_00781380);
	} else {
		qsort(entries, count, sizeof(SortEntry), (CompareFn)FUN_007813B0);
	}
}

struct UserSort {
	int Index;
	char * Name;
	int Flag;
};

namespace Game { extern int ObserverMode; }
extern int DAT_007EEF28;

void __fastcall FUN_0079AF10(HWND dialog);

extern int g_Col_ChanName;
extern char DAT_00B77E74;
extern int g_Col_ChanIcon;
extern int g_Col_ChanPrivate;
extern int DAT_00B76AF4;
extern int g_Col_PingTime;
extern int DAT_007EEF48;
extern wchar_t DAT_0084A724[];
extern wchar_t DAT_0084A6E0[];

// FUNCTION: GAMEMD 0x00781480
void FUN_00781480(void)
{
	HWND dialog = WS_Find_Dialog(0x113);
	if (dialog == NULL) {
		return;
	}
	HWND list = GetDlgItem(dialog, 0x6EE);
	if (list == NULL) {
		return;
	}
	Wstring selected;
	int cur = SendMessage(list, 0x188, 0, 0);
	if (cur != -1) {
		FUN_00623F00(list, cur, &selected);
	}
	Wstring top;
	int topIndex = SendMessage(list, 0x18E, 0, 0);
	if (topIndex != -1) {
		FUN_00623F00(list, topIndex, &top);
	}
	SendMessage(list, 0x49D, 0, 1);
	DAT_00B77E74 = 1;
	SendMessage(list, 0x184, 0, 0);
	int filter = DAT_00B77E44;
	int count = g_GameChannelList.length();
	int passes = 1;
	if (filter == 2 || filter == 3 || filter == 8 || filter == 9 || filter == 10 || filter == 11 || filter == 6 || filter == 7 || filter == 4 || filter == 5 || filter == 12 || filter == 13) {
		passes = 2;
	}
	SortEntry * entries = new SortEntry[count];
	Channel chan;
	if (entries == NULL) {
		DAT_00B77E44 = 0;
	}
	if (DAT_00B77E44 == 0xF) {
		int n = g_GameChannelList.length();
		memset(&chan, 0, sizeof(chan));
		for (int i = 0; i < n; i++) {
			g_GameChannelList.get(chan, i);
			int ping = chan.latency;
			entries[i].Pad = i;
			entries[i].Key = ping;
		}
		qsort(entries, n, 8, (CompareFn)FUN_007813B0);
	} else {
		int n = g_GameChannelList.length();
		memset(&chan, 0, sizeof(chan));
		for (int i = 0; i < n; i++) {
			g_GameChannelList.get(chan, i);
			int ping = chan.latency;
			entries[i].Pad = i;
			entries[i].Key = ping;
		}
		qsort(entries, n, 8, (CompareFn)FUN_00781380);
	}

	for (int pass = 0; pass < 2; pass++) {
		for (int k = 0; k < passes; k++) {
			for (int j = 0; j < count; j++) {
				memset(&chan, 0, sizeof(chan));
				int idx = j;
				if (DAT_00B77E44 != 1) {
					if (DAT_00B77E44 != 0) {
						int s = entries[j].Pad;
						if (s >= 0 && s < count) {
							idx = s;
						}
					}
				} else {
					idx = count - 1 - j;
				}
				g_GameChannelList.get(chan, idx);
				if (FUN_0077D9B0(&chan) || FUN_0077D9D0(&chan)) {
					continue;
				}
				char details[132];
				strncpy(details, (char *)chan.exInfo, 0x80);
				int first = 0;
				int second = 0;
				int third = 0;
				int fourth = 0;
				int fifth = 0;
				Wstring file;
				FUN_0077E820(details, &first, &second, &third, &fourth, &fifth, &file);
				int players = chan.currentUsers - fourth + third;
				int maxplayers = chan.maxUsers - ((fourth != 0 || fifth != 0) ? 1 : 0);
				if (Game::ObserverMode) {
					if (fifth != 0 && fourth == 0) {
						if (pass != 0) continue;
					} else {
						if (pass != 1) continue;
					}
				} else {
					if (players == maxplayers) {
						if (pass != 1) continue;
					} else {
						if (pass != 0) continue;
					}
				}
				if (FUN_00781130(&chan, k, fifth, fourth)) {
					continue;
				}
				wchar_t text[80];
				swprintf(text, DAT_0084A724, chan.name, players, maxplayers);
				int row = SendMessage(list, 0x4C7, -1, (LPARAM)chan.name);
				SendMessage(list, LB_SETITEMDATA, row, idx);
				OwnerDraw::CellData cell;
				cell.type = OwnerDraw::CellData::TEXT;
				cell.string.set(text);
				cell.hint.set(DAT_00887734);
				cell.color = DAT_007EEF28;
				if (Game::ObserverMode) {
					if (fifth == 0 || fourth != 0) {
						cell.color = DAT_007EEF48;
					}
				} else if (players == maxplayers) {
					cell.color = DAT_007EEF48;
				}
				SendMessage(list, 0x4A8, MAKEWPARAM(g_Col_ChanName, row), (LPARAM)&cell);

				cell.type = OwnerDraw::CellData::INVALID;
				cell.hint.set(DAT_00887734);
				cell.type = OwnerDraw::CellData::SURFACE;
				cell.hint.set(DAT_00887734);
				if (FUN_0077D990(&chan)) {
					cell.surf = SurfaceCache.GetSurface("wolclan.pcx", NULL);
					cell.hint.set(StringTable::LoadString("TXT_BATTLECLAN_GAME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xD17));
				} else if (FUN_0077D910(&chan)) {
					cell.surf = SurfaceCache.GetSurface("woltrny.pcx", NULL);
					cell.hint.set(StringTable::LoadString("TXT_TOURNAMENT_GAME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xD1C));
				} else {
					cell.surf = SurfaceCache.GetSurface("gt18.pcx", NULL);
					cell.hint.set(DAT_00887734);
				}
				SendMessage(list, 0x4A8, MAKEWPARAM(g_Col_ChanIcon, row), (LPARAM)&cell);

				cell.type = OwnerDraw::CellData::SURFACE;
				cell.hint.set(DAT_00887734);
				if (chan.flags & 0x100) {
					cell.surf = SurfaceCache.GetSurface("wolpriv.pcx", NULL);
					cell.hint.set(StringTable::LoadString("TXT_GAME_PASSWORD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xD2A));
				} else {
					cell.type = OwnerDraw::CellData::INVALID;
					cell.hint.set(DAT_00887734);
				}
				SendMessage(list, 0x4A8, MAKEWPARAM(g_Col_ChanPrivate, row), (LPARAM)&cell);

				cell.type = OwnerDraw::CellData::INVALID;
				cell.string.set(DAT_00887734);
				cell.hint.set(DAT_00887734);
				if ((chan.reserved >> 9) & 3) {
					if (fourth == 0 && fifth != 0) {
						cell.type = OwnerDraw::CellData::SURFACE;
						cell.surf = SurfaceCache.GetSurface("wolrslkob.pcx", NULL);
					} else {
						cell.type = OwnerDraw::CellData::SURFACE;
						cell.surf = SurfaceCache.GetSurface("wolreslk.pcx", NULL);
					}
				} else if (fourth == 0 && fifth != 0) {
					cell.type = OwnerDraw::CellData::SURFACE;
					cell.surf = SurfaceCache.GetSurface("wolob.pcx", NULL);
				}
				SendMessage(list, 0x4A8, MAKEWPARAM(DAT_00B76AF4, row), (LPARAM)&cell);

				if (chan.latency == -1) {
					cell.type = OwnerDraw::CellData::TEXT;
					cell.string.set(DAT_0084A6E0);
				} else {
					cell.pingtime = chan.latency;
					cell.type = OwnerDraw::CellData::PING;
				}
				swprintf(text, StringTable::LoadString("WOL:GamePing", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xD52), chan.latency);
				cell.hint.set(text);
				SendMessage(list, 0x4A8, MAKEWPARAM(g_Col_PingTime, row), (LPARAM)&cell);
			}
		}
	}
	if (entries != NULL) {
		delete entries;
	}
	int found = SendMessage(list, 0x4C5, 0, (LPARAM)selected.text());
	SendMessage(list, 0x186, found, 0);
	found = SendMessage(list, 0x4C5, 0, (LPARAM)top.text());
	if (found != -1) {
		SendMessage(list, 0x197, found, 0);
	}
	SendMessage(list, 0x49D, 0, 0);
	InvalidateRect(list, NULL, 0);
	if (dialog == FUN_00775B10()) {
		SetFocus(GetDlgItem(dialog, 0x540));
	}
}

// FUNCTION: GAMEMD 0x00781CC0
int __cdecl FUN_00781CC0(UserSort const * a, UserSort const * b)
{
	if (a->Flag != b->Flag) {
		return a->Flag ? -1 : 1;
	}
	return strcmpi(a->Name, b->Name);
}

// FUNCTION: GAMEMD 0x00781CF0
int __cdecl FUN_00781CF0(UserSort const * a, UserSort const * b)
{
	if (a->Flag != b->Flag) {
		return a->Flag ? -1 : 1;
	}
	return strcmpi(b->Name, a->Name);
}

// FUNCTION: GAMEMD 0x00781FB0
void __fastcall FUN_00781FB0(User * user, bool ascending)
{
	User temp;
	memset(&temp, 0, sizeof(temp));
	int count = g_UserList.length();
	if (count == 0) {
		g_UserList.addTail(*user);
		return;
	}
	int owners = 0;
	g_UserList.get(temp, 0);
	while (temp.flags & 1) {
		if (owners >= count) {
			break;
		}
		owners++;
		g_UserList.get(temp, owners);
	}
	int pos;
	if (user->flags & 1) {
		pos = 0;
		g_UserList.get(temp, 0);
		if (ascending) {
			while (pos < owners) {
				if (strcmpi((char *)temp.name, (char *)user->name) >= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		} else {
			while (pos < owners) {
				if (strcmpi((char *)temp.name, (char *)user->name) <= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		}
		g_UserList.add(*user, pos);
	} else {
		pos = owners;
		g_UserList.get(temp, owners);
		if (ascending) {
			while (pos < count) {
				if (strcmpi((char *)temp.name, (char *)user->name) >= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		} else {
			while (pos < count) {
				if (strcmpi((char *)temp.name, (char *)user->name) <= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		}
		g_UserList.add(*user, pos);
	}
}

// FUNCTION: GAMEMD 0x00781D20
void __fastcall FUN_00781D20(bool ascending)
{
	ArrayList<User> copy;
	int count = g_UserList.length();
	UserSort * sorted = new UserSort[count];
	if (!sorted) {
		return;
	}
	User temp;
	memset(&temp, 0, sizeof(temp));
	for (int i = 0; i < count; i++) {
		g_UserList.get(temp, i);
		sorted[i].Index = i;
		sorted[i].Name = new char[10];
		if (!sorted[i].Name) {
			return;
		}
		strcpy(sorted[i].Name, (char *)temp.name);
		sorted[i].Flag = temp.flags & 1;
		copy.addTail(temp);
	}
	g_UserList.clear();
	if (ascending) {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_00781CC0);
	} else {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_00781CF0);
	}
	int k;
	for (k = 0; k < count; k++) {
		copy.get(temp, sorted[k].Index);
		g_UserList.addTail(temp);
	}
	copy.clear();
	for (k = 0; k < count; k++) {
		if (sorted[k].Name) {
			delete sorted[k].Name;
		}
	}
	delete sorted;
}

int __cdecl FUN_00783520(const char * a, const char * b);
int __cdecl FUN_00783570(const char * a, const char * b);

// FUNCTION: GAMEMD 0x00782320
void __fastcall FUN_00782320(User * user, bool ascending)
{
	Wstring name;
	Ladder ladder;
	memset(&ladder, 0, sizeof(ladder));
	int rank = -1;
	name.set((char const *)user->name);
	name.toLower();
	if (g_ActiveLadder && g_ActiveLadder->getValue(name, ladder)) {
		rank = ladder.rung;
	}

	User temp;
	memset(&temp, 0, sizeof(temp));
	int count = g_UserList.length();
	if (count == 0) {
		g_UserList.addTail(*user);
		return;
	}

	int owners = 0;
	g_UserList.get(temp, 0);
	while ((temp.flags & 1) && owners < count) {
		owners++;
		g_UserList.get(temp, owners);
	}

	int pos;
	int other_rank;
	if (user->flags & 1) {
		pos = 0;
		g_UserList.get(temp, 0);
		name.set((char const *)temp.name);
		name.toLower();
		other_rank = -1;
		if (g_ActiveLadder && g_ActiveLadder->getValue(name, ladder)) {
			other_rank = ladder.rung;
		}

		if (ascending) {
			while (pos < owners) {
				if (!((other_rank < rank && other_rank > 0)
					  || (rank == -1 && other_rank > 0))) break;
				pos++;
				g_UserList.get(temp, pos);
				name.set((char const *)temp.name);
				name.toLower();
				other_rank = -1;
				if (g_ActiveLadder
					&& g_ActiveLadder->getValue(name, ladder)) {
					other_rank = ladder.rung;
				}
			}
		} else {
			while (pos < owners) {
				if (!((other_rank > rank && other_rank > 0)
					  || (rank == -1 && other_rank > 0))) break;
				pos++;
				g_UserList.get(temp, pos);
				name.set((char const *)temp.name);
				name.toLower();
				other_rank = -1;
				if (g_ActiveLadder
					&& g_ActiveLadder->getValue(name, ladder)) {
					other_rank = ladder.rung;
				}
			}
		}

		while (pos < owners) {
			if (rank != other_rank) break;
			if (strcmpi((char const *)temp.name,
						(char const *)user->name) >= 0) break;
			pos++;
			g_UserList.get(temp, pos);
			name.set((char const *)temp.name);
			name.toLower();
			other_rank = -1;
			if (g_ActiveLadder
				&& g_ActiveLadder->getValue(name, ladder)) {
				other_rank = ladder.rung;
			}
		}
	} else {
		pos = owners;
		g_UserList.get(temp, owners);
		name.set((char const *)temp.name);
		name.toLower();
		other_rank = -1;
		if (g_ActiveLadder && g_ActiveLadder->getValue(name, ladder)) {
			other_rank = ladder.rung;
		}

		if (ascending) {
			while (pos < count) {
				if (!((other_rank < rank && other_rank > 0)
					  || (rank == -1 && other_rank > 0))) break;
				pos++;
				g_UserList.get(temp, pos);
				name.set((char const *)temp.name);
				name.toLower();
				other_rank = -1;
				if (g_ActiveLadder
					&& g_ActiveLadder->getValue(name, ladder)) {
					other_rank = ladder.rung;
				}
			}
		} else {
			while (pos < count) {
				if (!((other_rank > rank && other_rank > 0)
					  || (rank == -1 && other_rank > 0))) break;
				pos++;
				g_UserList.get(temp, pos);
				name.set((char const *)temp.name);
				name.toLower();
				other_rank = -1;
				if (g_ActiveLadder
					&& g_ActiveLadder->getValue(name, ladder)) {
					other_rank = ladder.rung;
				}
			}
		}

		while (pos < count) {
			if (rank != other_rank) break;
			if (strcmpi((char const *)temp.name,
						(char const *)user->name) >= 0) break;
			pos++;
			g_UserList.get(temp, pos);
			name.set((char const *)temp.name);
			name.toLower();
			other_rank = -1;
			if (g_ActiveLadder
				&& g_ActiveLadder->getValue(name, ladder)) {
				other_rank = ladder.rung;
			}
		}
	}

	g_UserList.add(*user, pos);
}

// FUNCTION: GAMEMD 0x00782BB0
void __fastcall FUN_00782BB0(User * user, bool ascending)
{
	int count = g_UserList.length();
	User temp;
	memset(&temp, 0, sizeof(temp));
	if (count == 0) {
		g_UserList.addTail(*user);
		return;
	}
	int owners = 0;
	g_UserList.get(temp, 0);
	while (temp.flags & 1) {
		if (owners >= count) {
			break;
		}
		owners++;
		g_UserList.get(temp, owners);
	}
	int pos;
	if (user->flags & 1) {
		pos = 0;
		g_UserList.get(temp, 0);
		if (ascending) {
			while (pos < owners) {
				if (FUN_00783520((char const *)temp.squadabbrev, (char const *)user->squadabbrev) >= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		} else {
			while (pos < owners) {
				if (FUN_00783570((char const *)temp.squadabbrev, (char const *)user->squadabbrev) >= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		}
		while (pos < owners) {
			if (FUN_00783570((char const *)temp.squadabbrev, (char const *)user->squadabbrev) != 0) {
				break;
			}
			if (strcmpi((char const *)temp.name, (char const *)user->name) >= 0) {
				break;
			}
			pos++;
			g_UserList.get(temp, pos);
		}
	} else {
		pos = owners;
		g_UserList.get(temp, owners);
		if (ascending) {
			while (pos < count) {
				if (FUN_00783520((char const *)temp.squadabbrev, (char const *)user->squadabbrev) >= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		} else {
			while (pos < count) {
				if (FUN_00783570((char const *)temp.squadabbrev, (char const *)user->squadabbrev) >= 0) {
					break;
				}
				pos++;
				g_UserList.get(temp, pos);
			}
		}
		while (pos < count) {
			if (FUN_00783570((char const *)temp.squadabbrev, (char const *)user->squadabbrev) != 0) {
				break;
			}
			if (strcmpi((char const *)temp.name, (char const *)user->name) >= 0) {
				break;
			}
			pos++;
			g_UserList.get(temp, pos);
		}
	}
	g_UserList.add(*user, pos);
}

extern int DAT_00B77E48;

// FUNCTION: GAMEMD 0x00783070
void __fastcall FUN_00783070(User * user)
{
	int mode = DAT_00B77E48;
	if (mode == 1) {
		FUN_00781FB0(user, false);
	} else if (mode == 2) {
		FUN_00782320(user, true);
	} else if (mode == 3) {
		FUN_00782320(user, false);
	} else if (mode == 4) {
		FUN_00782BB0(user, true);
	} else if (mode == 5) {
		FUN_00782BB0(user, false);
	} else {
		FUN_00781FB0(user, true);
	}
}

struct UserRank {
	int Index;
	int Key;
	int Flag;
};

// FUNCTION: GAMEMD 0x007830C0
int __cdecl FUN_007830C0(UserRank const * a, UserRank const * b)
{
	if (a->Flag != b->Flag) {
		return a->Flag ? -1 : 1;
	}
	if (a->Key < b->Key) {
		return a->Key == -1 ? 1 : -1;
	}
	if (a->Key > b->Key) {
		return b->Key == -1 ? -1 : 1;
	}
	User first;
	User second;
	memset(&first, 0, sizeof(first));
	memset(&second, 0, sizeof(second));
	g_UserList.get(first, a->Index);
	g_UserList.get(second, b->Index);
	return strcmpi((char *)first.name, (char *)second.name);
}

// FUNCTION: GAMEMD 0x007831A0
int __cdecl FUN_007831A0(UserRank const * a, UserRank const * b)
{
	if (a->Flag != b->Flag) {
		return a->Flag ? -1 : 1;
	}
	if (a->Key > b->Key) {
		return -1;
	}
	if (a->Key < b->Key) {
		return 1;
	}
	User first;
	User second;
	memset(&first, 0, sizeof(first));
	memset(&second, 0, sizeof(second));
	g_UserList.get(first, a->Index);
	g_UserList.get(second, b->Index);
	return strcmpi((char *)first.name, (char *)second.name);
}

// FUNCTION: GAMEMD 0x00783270
void __fastcall FUN_00783270(bool ascending)
{
	ArrayList<User> copy;
	int count = g_UserList.length();
	UserRank * sorted = new UserRank[count];
	if (!sorted) {
		return;
	}
	User temp;
	memset(&temp, 0, sizeof(temp));
	for (int i = 0; i < count; i++) {
		g_UserList.get(temp, i);
		sorted[i].Index = i;
		sorted[i].Key = -1;
		Wstring name;
		Ladder info;
		memset(&info, 0, sizeof(info));
		name.set((char *)temp.name);
		name.toLower();
		if (g_ActiveLadder && g_ActiveLadder->getValue(name, info)) {
			sorted[i].Key = info.rung;
		}
		sorted[i].Flag = temp.flags & 1;
		copy.addTail(temp);
	}
	if (ascending) {
		qsort(sorted, count, sizeof(UserRank), (CompareFn)FUN_007830C0);
	} else {
		qsort(sorted, count, sizeof(UserRank), (CompareFn)FUN_007831A0);
	}
	g_UserList.clear();
	for (int k = 0; k < count; k++) {
		copy.get(temp, sorted[k].Index);
		g_UserList.addTail(temp);
	}
	copy.clear();
	delete sorted;
}

// FUNCTION: GAMEMD 0x00783520
int __cdecl FUN_00783520(const char * a, const char * b)
{
	int la = strlen(a);
	int lb = strlen(b);
	if (la && lb) {
		return strcmpi(a, b);
	}
	if (la > 0) {
		return -1;
	}
	if (lb > 0) {
		return 1;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00783570
int __cdecl FUN_00783570(const char * a, const char * b)
{
	int la = strlen(a);
	int lb = strlen(b);
	if (la && lb) {
		return strcmpi(b, a);
	}
	if (la > 0) {
		return -1;
	}
	if (lb > 0) {
		return 1;
	}
	return 0;
}

static inline int Compare_User_Names(UserSort const * a, UserSort const * b)
{
	char * bn = b->Name;
	char * an = a->Name;
	int la = strlen(an);
	int lb = strlen(bn);
	int r;
	if (la && lb) {
		r = strcmpi(an, bn);
		if (r != 0) {
			goto done;
		}
	} else {
		if (la > 0) {
			return -1;
		}
		if (lb > 0) {
			return 1;
		}
	}
	User first;
	User second;
	memset(&first, 0, sizeof(first));
	memset(&second, 0, sizeof(second));
	g_UserList.get(first, a->Index);
	g_UserList.get(second, b->Index);
	r = strcmpi((char *)first.name, (char *)second.name);
done:
	return r;
}

// FUNCTION: GAMEMD 0x007835C0
int __cdecl FUN_007835C0(UserSort const * a, UserSort const * b)
{
	if (a->Flag != b->Flag) {
		return a->Flag ? -1 : 1;
	}
	return Compare_User_Names(a, b);
}

static inline int Compare_User_Names_Reversed(UserSort const * a, UserSort const * b)
{
	char * bn = b->Name;
	char * an = a->Name;
	int la = strlen(an);
	int lb = strlen(bn);
	int r;
	if (la && lb) {
		r = strcmpi(bn, an);
		if (r != 0) {
			goto done;
		}
	} else {
		if (la > 0) {
			return -1;
		}
		if (lb > 0) {
			return 1;
		}
	}
	User first;
	User second;
	memset(&first, 0, sizeof(first));
	memset(&second, 0, sizeof(second));
	g_UserList.get(first, a->Index);
	g_UserList.get(second, b->Index);
	r = strcmpi((char *)first.name, (char *)second.name);
done:
	return r;
}

// FUNCTION: GAMEMD 0x007836E0
int __cdecl FUN_007836E0(UserSort const * a, UserSort const * b)
{
	if (a->Flag != b->Flag) {
		return a->Flag ? -1 : 1;
	}
	return Compare_User_Names_Reversed(a, b);
}

// FUNCTION: GAMEMD 0x00783800
void __fastcall FUN_00783800(bool ascending)
{
	ArrayList<User> copy;
	int count = g_UserList.length();
	UserSort * sorted = new UserSort[count];
	if (!sorted) {
		return;
	}
	User temp;
	memset(&temp, 0, sizeof(temp));
	for (int i = 0; i < count; i++) {
		g_UserList.get(temp, i);
		sorted[i].Index = i;
		sorted[i].Name = new char[10];
		if (!sorted[i].Name) {
			return;
		}
		strcpy(sorted[i].Name, (char *)temp.squadabbrev);
		sorted[i].Flag = temp.flags & 1;
		copy.addTail(temp);
	}
	if (ascending) {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_007835C0);
	} else {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_007836E0);
	}
	g_UserList.clear();
	int k;
	for (k = 0; k < count; k++) {
		copy.get(temp, sorted[k].Index);
		g_UserList.addTail(temp);
	}
	copy.clear();
	for (k = 0; k < count; k++) {
		if (sorted[k].Name) {
			delete sorted[k].Name;
		}
	}
	delete sorted;
}

// FUNCTION: GAMEMD 0x00783A90
Surface * __fastcall FUN_00783A90(int digit)
{
	switch (digit) {
		case 0:
			return SurfaceCache.GetSurface("number0.pcx", NULL);
		case 1:
			return SurfaceCache.GetSurface("number1.pcx", NULL);
		case 2:
			return SurfaceCache.GetSurface("number2.pcx", NULL);
		case 3:
			return SurfaceCache.GetSurface("number3.pcx", NULL);
		case 4:
			return SurfaceCache.GetSurface("number4.pcx", NULL);
		case 5:
			return SurfaceCache.GetSurface("number5.pcx", NULL);
		case 6:
			return SurfaceCache.GetSurface("number6.pcx", NULL);
		case 7:
			return SurfaceCache.GetSurface("number7.pcx", NULL);
		case 8:
			return SurfaceCache.GetSurface("number8.pcx", NULL);
		case 9:
			return SurfaceCache.GetSurface("number9.pcx", NULL);
	}
	return 0;
}

struct RankedUser {
	char Pad[0xC];
	unsigned int Points;
};

// FUNCTION: GAMEMD 0x00783B80
Surface * __fastcall FUN_00783B80(RankedUser * user)
{
	if (!user) {
		return 0;
	}
	unsigned int points = user->Points;
	if (points > 2000) {
		return SurfaceCache.GetSurface("comchief.pcx", NULL);
	}
	if (points > 1500) {
		return SurfaceCache.GetSurface("stargen.pcx", NULL);
	}
	if (points > 1000) {
		return SurfaceCache.GetSurface("general.pcx", NULL);
	}
	if (points > 500) {
		return SurfaceCache.GetSurface("briggenr.pcx", NULL);
	}
	if (points > 250) {
		return SurfaceCache.GetSurface("colonel.pcx", NULL);
	}
	if (points > 150) {
		return SurfaceCache.GetSurface("major.pcx", NULL);
	}
	if (points > 80) {
		return SurfaceCache.GetSurface("lieutena.pcx", NULL);
	}
	if (points > 60) {
		return SurfaceCache.GetSurface("sergeant.pcx", NULL);
	}
	if (points > 31) {
		return SurfaceCache.GetSurface("corporal.pcx", NULL);
	}
	return SurfaceCache.GetSurface("private.pcx", NULL);
}

#include "multiplayer.h"

void __fastcall FUN_005EA3E0(HWND dialog, bool flag);
void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);
extern int DAT_00B779F4;
extern int DAT_00B76604;
extern int DAT_00B76438;
extern int DAT_00B73704;
extern int DAT_00B7619C;
extern int DAT_00B76194;
extern int DAT_00B76198;
extern int DAT_00B766BC;
extern int DAT_00B76AC8;
extern int DAT_00B77D68;
extern int DAT_00B76470;
extern int DAT_00B768FC;
extern int DAT_00B73AE4;
extern int DAT_00B76474;
extern int DAT_00B766B8;
extern int DAT_00B76958;
extern int DAT_00B76910;
extern int DAT_00B77798;
extern int DAT_00B768F8;
extern int DAT_00B76AC0;
extern int DAT_00B73680;
extern int DAT_00B77790;

// FUNCTION: GAMEMD 0x00783C80
void __fastcall Draw_Player_List(int keep_selection, bool flag)
{
	int i;
	int slot_index;
	wchar_t info[80];
	User user;
	int house;
	int star;

	HWND win = NULL;
	win = GameoptWindow();
	if (win) {
		FUN_005EA3E0(win, flag);
		return;
	}
	if (WS_Find_Dialog(0x6B)) return;
	win = WS_Find_Dialog(0x116);
	if (win) {
		FUN_0079AF10(win);
	} else {
		win = WS_Find_Dialog(0x113);
	}

	HWND userwin = GetDlgItem(win, 0x53E);

	if ((win == NULL) || (userwin == NULL)) return;

	Dictionary<WideWstring, bool> lbdict(Wstring_Hash);

	LBSaveSelections(userwin, lbdict);

	int topindex = SendDlgItemMessage(win, 0x53E, LB_GETTOPINDEX, 0, 0);

	SendDlgItemMessage(win, 0x53E, 0x49D, 0, 1);

	SendDlgItemMessage(win, 0x53E, LB_RESETCONTENT, 0, 0);

	Wstring ladder_name;
	Ladder ladder;
	wchar_t rank_string[80];
	static wchar_t localestring[64];
	int average_ping;

	memset(&ladder, 0, sizeof(ladder));
	OwnerDraw::CellData thecell;
	Wstring nick = ((Wstring const &)g_NickName).toLower();

	int count = g_UserList.length();
	for (i = 0; i < count; i++) {
		g_UserList.get(user, i);
		Wstring name((char *)user.name);

		if (g_CurrentChannel.type == 0) {

			swprintf(info, L"%hs", user.name);
			int row = SendDlgItemMessage(win, 0x53E, 0x4CC, (WPARAM)-1, (LPARAM)info);

			thecell.type = OwnerDraw::CellData::TEXT;
			thecell.hint.set(StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x104A));
			FUN_00735060(info, (char *)user.name, -1);
			thecell.string.set(info);

			if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
				if ((user.flags & 1) == 0) {
					int color;
					Wstring team = FUN_007B7F90(thecell.string.get());
					if (g_WDTTeams.contains(team)) {
						g_WDTTeams.getValue(team, color);
						WideWstring wide_team(FUN_00735120(team.get()));
						thecell.string = wide_team;
						thecell.color = PlayerColorTable[color];
					} else {
						thecell.color = DAT_007EEF28;
					}
				} else {
					thecell.color = DAT_007EEF28;
				}
			} else {
				thecell.color = DAT_007EEF28;
			}

			ladder_name.set((char *)user.name);
			ladder_name.toLower();
			int locale = 0;
			memset(localestring, 0, sizeof(localestring));
			if (g_UserLocales.contains(ladder_name) && (g_pChat)) {
				g_UserLocales.getValue(ladder_name, locale);
				if (locale) {
					WideWstring locale_name = FUN_00780A00(locale);
					swprintf(localestring, L"(%hs)", locale_name.get());
				}
			}
			if (g_ActiveLadder->getValue(ladder_name, ladder)) {
				swprintf(rank_string, L"%s %d", StringTable::LoadString("TXT_RANK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1083), ladder.rung);
				thecell.hint.set(rank_string);
			}
			if (g_UserLocales.contains(ladder_name)) {
				if (locale) {
					thecell.hint.insert(L" ", 0);
				}
				thecell.hint.insert(localestring, 0);
			}
			SendMessage(GetDlgItem(win, 0x53E), LB_SETITEMDATA, row, i);
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B779F4, i), (LPARAM)&thecell);

			thecell.type = OwnerDraw::CellData::TEXT;
			ladder_name.set((char *)user.name);
			ladder_name.toLower();
			rank_string[0] = 0;

			if (strlen((char *)user.squadabbrev)) {
				wchar_t squadname[200];
				swprintf(rank_string, L"[%hs]", user.squadabbrev);
				FUN_00735060(squadname, (char *)user.squadname, -1);
				thecell.hint.set(squadname);
			}
			thecell.string.set(rank_string);
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B76604, i), (LPARAM)&thecell);

			rank_string[0] = 0;
			if (g_ActiveLadder->getValue(ladder_name, ladder)) {
				swprintf(rank_string, L"%d", ladder.rung);
				thecell.hint.set(DAT_00887734);
			}
			thecell.string.set(rank_string);
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B76438, i), (LPARAM)&thecell);

			thecell.type = OwnerDraw::CellData::TEXT;
			if (user.flags & 1) {
				thecell.type = OwnerDraw::CellData::SURFACE;
				thecell.surf = SurfaceCache.GetSurface("woloper.pcx", NULL);
				thecell.hint.set(StringTable::LoadString("TXT_OPER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x10B7));
			} else if (user.flags & 2) {
				thecell.type = OwnerDraw::CellData::SURFACE;
				thecell.surf = SurfaceCache.GetSurface("wolvoice.pcx", NULL);
				thecell.hint.set(StringTable::LoadString("TXT_VOICE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x10BD));
			} else {
				thecell.string.set(DAT_00887734);
				thecell.hint.set(DAT_00887734);
			}
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B73704, i), (LPARAM)&thecell);

			Highscore score;
			memset(&score, 0, sizeof(score));
			if (DAT_00B76640.getValue(ladder_name, score)) {
				thecell.type = OwnerDraw::CellData::TEXT;
				if (score.rank <= 999 && score.rank >= 100) {
					thecell.type = OwnerDraw::CellData::SURFACE;
					thecell.surf = FUN_00783A90((score.rank % 1000) / 100);
				} else {
					thecell.string.set(DAT_00887734);
				}
				thecell.hint.set(StringTable::LoadString("TXT_OPER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x10D4));
				SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B7619C, i), (LPARAM)&thecell);

				thecell.type = OwnerDraw::CellData::TEXT;
				if (score.rank <= 999 && score.rank >= 10) {
					thecell.type = OwnerDraw::CellData::SURFACE;
					thecell.surf = FUN_00783A90((score.rank % 100) / 10);
				} else {
					thecell.string.set(DAT_00887734);
				}
				thecell.hint.set(StringTable::LoadString("TXT_OPER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x10DF));
				SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B76194, i), (LPARAM)&thecell);

				thecell.type = OwnerDraw::CellData::TEXT;
				if (score.rank <= 999 && score.rank >= 1) {
					thecell.type = OwnerDraw::CellData::SURFACE;
					thecell.surf = FUN_00783A90(score.rank % 10);
				} else {
					thecell.string.set(DAT_00887734);
				}
				thecell.hint.set(StringTable::LoadString("TXT_OPER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x10EA));
				SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B76198, i), (LPARAM)&thecell);

				thecell.surf = FUN_00783B80((RankedUser *)&score);
				if (thecell.surf) {
					thecell.type = OwnerDraw::CellData::SURFACE;
				} else {
					thecell.type = OwnerDraw::CellData::TEXT;
					thecell.string.set(DAT_00887734);
				}
				thecell.hint.set(StringTable::LoadString("TXT_OPER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x10F5));
				SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B766BC, i), (LPARAM)&thecell);

				int campaigns = 0;
				DAT_0089F518.FUN_0049E4F0(&campaigns);
				int mask = (1 << campaigns) - 1;
				if ((score.accomplishments & mask) == mask && campaigns > 0) {
					thecell.type = OwnerDraw::CellData::SURFACE;
					thecell.surf = SurfaceCache.GetSurface("cooperat.pcx", NULL);
				} else {
					thecell.type = OwnerDraw::CellData::TEXT;
					thecell.string.set(DAT_00887734);
				}
				thecell.hint.set(StringTable::LoadString("TXT_OPER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1105));
				SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B76AC8, i), (LPARAM)&thecell);
			}
		} else {
			if (HouseTypes.Count() <= 1) return;

			slot_index = Player_Name_To_Index((char *)user.name);

			house = 0;
			star = 0;
			if (slot_index != -1) {
				house = g_UserInfo[slot_index].House;
				if (g_UserInfo[slot_index].Accepted) star = 1;

				if (slot_index == 0) {
					g_UserInfo[slot_index].Accepted = 1;
					star = 2;
				}
			}

			house = MAX(house, 0);

			ladder_name.set((char *)user.name);
			ladder_name.toLower();

			swprintf(info, L"%hs", user.name);
			for (int j = 0; j < (int)wcslen(info); j++) {
				info[j] = tolower(info[j]);
			}

			SendDlgItemMessage(win, 0x53E, 0x4CC, (WPARAM)-1, (LPARAM)info);

			thecell.type = OwnerDraw::CellData::PRIMARY;
			thecell.color = PlayerColorTable[g_UserInfo[slot_index].Color];
			thecell.hint.set(StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x114C));
			if (g_ActiveLadder->getValue(ladder_name, ladder)) {
				swprintf(rank_string, L"%s %d", StringTable::LoadString("TXT_RANK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x114F), ladder.rung);
				thecell.hint.set(rank_string);
			}
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B779F4, i), (LPARAM)&thecell);

			thecell.type = OwnerDraw::CellData::TEXT;
			if (strlen((char *)user.squadabbrev) != 0) {
				FUN_00735060(info, (char *)user.squadname, -1);
				thecell.hint.set(info);
				FUN_00735060(info, (char *)user.squadabbrev, -1);
			} else {
				if (g_ActiveLadder->getValue(ladder_name, ladder)) {
					swprintf(info, L"(%s %d)", StringTable::LoadString("TXT_RANK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1162), ladder.rung);
					thecell.hint.set(DAT_00887734);
				} else {
					swprintf(info, DAT_00887734);
					thecell.hint.set(DAT_00887734);
				}
			}
			thecell.string.set(info);
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B77D68, i), (LPARAM)&thecell);

			thecell.type = OwnerDraw::CellData::SURFACE;
			switch (house) {
				case 0:
					thecell.surf = SurfaceCache.GetSurface("usai.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_GDI", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1186));
					break;
				case 1:
					thecell.surf = SurfaceCache.GetSurface("japi.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_GDI", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x118A));
					break;
				case 2:
					thecell.surf = SurfaceCache.GetSurface("frai.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_GDI", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x118E));
					break;
				case 3:
					thecell.surf = SurfaceCache.GetSurface("geri.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_GDI", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1192));
					break;
				case 4:
					thecell.surf = SurfaceCache.GetSurface("gbri.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_GDI", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1196));
					break;
				case 5:
					thecell.surf = SurfaceCache.GetSurface("djbi.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_NOD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x119A));
					break;
				case 6:
					thecell.surf = SurfaceCache.GetSurface("arbi.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_NOD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x119E));
					break;
				case 7:
					thecell.surf = SurfaceCache.GetSurface("lati.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_NOD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x11A2));
					break;
				case 8:
					thecell.surf = SurfaceCache.GetSurface("rusi.pcx", NULL);
					swprintf(info, L"%s", StringTable::LoadString("TXT_NOD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x11A6));
					break;
			}
			thecell.hint.set(info);
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B76470, i), (LPARAM)&thecell);

			thecell.type = OwnerDraw::CellData::SURFACE;
			if (star) {
				if (star == 2) {
					thecell.surf = SurfaceCache.GetSurface("wolhost.pcx", NULL);
				} else {
					thecell.surf = SurfaceCache.GetSurface("wolacpt.pcx", NULL);
				}
				thecell.hint.set(StringTable::LoadString("TXT_ACCEPTED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x11B4));
			} else {
				thecell.type = OwnerDraw::CellData::INVALID;
				thecell.hint.set(DAT_00887734);
			}
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B768FC, i), (LPARAM)&thecell);

			if (Session.MPGameMode) {
				Session.MPGameMode->vt_entry_78(win);
			}

			average_ping = 0;
			g_pNetUtil->GetAvgPing(user.ipaddr, &average_ping);
			if (average_ping < 0) average_ping = 0;
			thecell.type = OwnerDraw::CellData::PING;
			thecell.pingtime = average_ping;
			wchar_t ping[200];
			swprintf(ping, StringTable::LoadString("MSG:PingInfo", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x11C8), average_ping);
			if (average_ping == 0) {
				swprintf(ping, StringTable::LoadString("TXT_UNKNOWN_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x11CA));
			}
			thecell.hint.set(ping);
			Wstring lower((char *)user.name);
			if (nick == ((Wstring const &)lower).toLower()) {
				thecell.type = OwnerDraw::CellData::INVALID;
			}
			SendDlgItemMessage(win, 0x53E, 0x4A8, MAKEWPARAM(DAT_00B73AE4, i), (LPARAM)&thecell);
		}
	}

	if (keep_selection == 0) LBRestoreSelections(userwin, lbdict);

	int sel_count = SendDlgItemMessage(win, 0x53E, LB_GETSELCOUNT, 0, 0);
	int user_count = g_UserList.length();
	char str[20];
	if (sel_count > 0) {
		sprintf(str, "%d  (%d)", user_count, sel_count);
	} else {
		sprintf(str, "%d", user_count);
	}

	SendDlgItemMessage(win, 0x60B, 0x4B4, 0, (LPARAM)str);

	SendDlgItemMessage(win, 0x53E, LB_SETTOPINDEX, (WPARAM)topindex, 0);

	SendDlgItemMessage(win, 0x53E, 0x49D, 0, 0);

	InvalidateRect(GetDlgItem(win, 0x53E), NULL, 0);
}

// FUNCTION: GAMEMD 0x00785360
void Display_Users(void)
{
	Draw_Player_List(0, false);
}

struct AtlObjectEntry;

struct AtlExitNode {
	void (__stdcall * Function)(void * argument);
	void * Argument;
	AtlExitNode * Next;
};

struct AtlModule {
	unsigned int cbSize;
	HINSTANCE Instance;
	HINSTANCE ResourceInstance;
	HINSTANCE TypeLibInstance;
	AtlObjectEntry * Entries;
	long LockCount;
	HANDLE Heap;
	CRITICAL_SECTION Lock0;
	CRITICAL_SECTION Lock1;
	CRITICAL_SECTION Lock2;
	unsigned int BuildVersion;
	void * CreateWndList;
	bool OwnsHeaps;
	char Pad3[3];
	GUID const * VersionGuid;
	unsigned int HeapCount;
	HANDLE * Heaps;
	int HeapIndex;
	AtlExitNode * ExitList;

	long Init(void * objects, HINSTANCE instance, GUID const * libid = NULL);
	long Lock(void);
	long Unlock(void);
};

extern AtlModule _Module;
long __stdcall AtlModuleTerm(AtlModule * module);

struct AtlIntMapEntry {
	IID const * piid;
	DWORD dw;
	long (__stdcall * pFunc)(void * pThis, IID const & iid, void ** object, DWORD dw);
};

extern AtlIntMapEntry DAT_007F7698[];
extern AtlIntMapEntry DAT_007F7650[];
extern AtlIntMapEntry DAT_007F7638[];
extern AtlModule * _pModule;
extern GUID const GUID_ATLVer30;
long __stdcall AtlModuleInit(AtlModule * module, void * objects, HINSTANCE instance);

namespace ATL {

template <class T>
class _NoAddRefReleaseOnCComPtr : public T
{
	private:
		virtual unsigned long __stdcall AddRef(void) = 0;
		virtual unsigned long __stdcall Release(void) = 0;
};

template <class T>
class CComPtr
{
	public:
		CComPtr(void) { p = NULL; }
		~CComPtr(void)
		{
			if (p) {
				p->Release();
			}
		}
		T ** operator&(void) { return &p; }
		_NoAddRefReleaseOnCComPtr<T> * operator->(void) const { return (_NoAddRefReleaseOnCComPtr<T> *)p; }

		T * p;
};

}

inline long __stdcall AtlAdvise(IUnknown * source, IUnknown * sink, IID const & iid, DWORD * cookie)
{
	ATL::CComPtr<IConnectionPointContainer> container;
	ATL::CComPtr<IConnectionPoint> point;
	HRESULT result = source->QueryInterface(IID_IConnectionPointContainer, (void **)&container);
	if (SUCCEEDED(result)) {
		result = container->FindConnectionPoint(iid, &point);
	}
	if (SUCCEEDED(result)) {
		result = point->Advise(sink, cookie);
	}
	return result;
}

static inline bool InlineIsEqualUnknown(IID const & iid)
{
	long const * words = (long const *)&iid;
	return words[0] == 0 && words[1] == 0 && words[2] == 0xC0 && words[3] == 0x46000000;
}

static inline bool InlineIsEqualGUID(IID const & a, IID const & b)
{
	long const * x = (long const *)&a;
	long const * y = (long const *)&b;
	return x[0] == y[0] && x[1] == y[1] && x[2] == y[2] && x[3] == y[3];
}

static inline long AtlInternalQueryInterface(void * pThis, AtlIntMapEntry * entries, IID const & iid, void ** object)
{
	if (object == NULL) {
		return E_POINTER;
	}
	*object = NULL;
	if (InlineIsEqualUnknown(iid)) {
		IUnknown * unknown = (IUnknown *)((int)pThis + entries->dw);
		unknown->AddRef();
		*object = unknown;
		return 0;
	}
	while (entries->pFunc != NULL) {
		BOOL blind = (entries->piid == NULL);
		if (blind || InlineIsEqualGUID(*entries->piid, iid)) {
			if (entries->pFunc == (void *)1) {
				IUnknown * unknown = (IUnknown *)((int)pThis + entries->dw);
				unknown->AddRef();
				*object = unknown;
				return 0;
			}
			long result = entries->pFunc(pThis, iid, object, entries->dw);
			if (result == 0 || (!blind && result < 0)) {
				return result;
			}
		}
		entries++;
	}
	return E_NOINTERFACE;
}

struct AtlCriticalSection {
	CRITICAL_SECTION Section;
	AtlCriticalSection(void) { InitializeCriticalSection(&Section); }
	~AtlCriticalSection(void) { DeleteCriticalSection(&Section); }
};

struct AtlThreadModel {
	static unsigned long __stdcall Increment(long * p) { return InterlockedIncrement(p); }
	static unsigned long __stdcall Decrement(long * p) { return InterlockedDecrement(p); }
};

inline long AtlModule::Init(void * objects, HINSTANCE instance, GUID const * libid)
{
	VersionGuid = &GUID_ATLVer30;
	_pModule = this;
	cbSize = 0x84;
	BuildVersion = 0x300;
	AtlModuleInit(this, objects, instance);
	return S_OK;
}

inline long AtlModule::Lock(void)
{
	return AtlThreadModel::Increment(&LockCount);
}

inline long AtlModule::Unlock(void)
{
	return AtlThreadModel::Decrement(&LockCount);
}

struct AtlRootBase {
	AtlRootBase(void) { RefCount = 0; }
	long FinalConstruct(void) { return S_OK; }
	void FinalRelease(void) {}
	void SetVoid(void *) {}
	void InternalFinalConstructAddRef(void) {}
	void InternalFinalConstructRelease(void) {}
	static long __stdcall InternalQueryInterface(void * pThis, AtlIntMapEntry * entries, IID const & iid, void ** object)
	{
		return AtlInternalQueryInterface(pThis, entries, iid, object);
	}

	long RefCount;
};

struct AtlSinkRoot : AtlRootBase {
	unsigned long InternalAddRef(void) { return AtlThreadModel::Increment(&RefCount); }
	unsigned long InternalRelease(void) { return AtlThreadModel::Decrement(&RefCount); }

	AtlCriticalSection Lock;
};

class CNetUtilEventSink : public AtlSinkRoot, public INetUtilEvent
{
	public:
		CNetUtilEventSink(void);
		HRESULT __stdcall OnPing(HRESULT res, int time, unsigned long ip, int handle);
		HRESULT __stdcall OnLadderList(HRESULT res, Ladder * list, int totalCount, long timeStamp, int keyRung);
		HRESULT __stdcall OnGameresSent(HRESULT res);
		HRESULT __stdcall OnNewNick(HRESULT res, LPCSTR message, LPCSTR nick, LPCSTR pass);
		HRESULT __stdcall OnAgeCheck(HRESULT res, int years, int consent);
		HRESULT __stdcall OnWDTState(HRESULT res, unsigned char * state, int length);
		HRESULT __stdcall OnHighscore(HRESULT res, Highscore * list, int totalCount, long timeStamp, int keyRung);
		static AtlIntMapEntry * __stdcall _GetEntries(void) { return DAT_007F7650; }
		IUnknown * _GetRawUnknown(void) { return (IUnknown *)((int)this + _GetEntries()->dw); }
		IUnknown * GetUnknown(void) { return _GetRawUnknown(); }
		long _InternalQueryInterface(IID const & iid, void ** object) { return InternalQueryInterface(this, _GetEntries(), iid, object); }
};

rc_ptr<PlayerProfile> __fastcall FUN_00778A20(Wstring const * name);
void __fastcall FUN_0079C040(HWND dialog);
void FUN_0079D110(void);
extern char DAT_00884CF4;
extern int g_Col_Rung;
extern int g_Col_Name;
extern int g_Col_Points;
extern int g_Col_Wins;
extern int g_Col_Losses;
extern int g_Col_Disconnects;

static inline char Remove_Ladder(Dictionary<Wstring, Ladder> & dict, Wstring & key, Ladder & value)
{
	return dict.remove(key, value);
}

// FUNCTION: GAMEMD 0x007A3100
inline HRESULT CNetUtilEventSink::OnLadderList(HRESULT result, Ladder * list, int, long, int key_rung)
{
	Ladder * temp = NULL;
	if (result != 0) {
		list = NULL;
	}
	if (DAT_00B77DC1) {
		DAT_00884CF4 = 1;
		return 0;
	}
	HWND rung_list = GetDlgItem(WS_Find_Dialog(0xC1), 0x57F);
	if (rung_list && key_rung == -1 && result == 0) {
		rung_list = NULL;
	}
	if (rung_list) {
		SendMessage(rung_list, LB_RESETCONTENT, 0, 0);
		if (list == NULL) {
			SendMessage(rung_list, 0x4CC, (WPARAM)-1, (LPARAM)StringTable::LoadString("TXT_NOT_IN_LADDER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x12B1));
			InvalidateRect(rung_list, NULL, FALSE);
			return 0;
		}
	} else if (list == NULL) {
		return 0;
	}

	temp = list;
	wchar_t list_string[128];
	int top_rung = list->rung;
	OwnerDraw::CellData thecell;

	if (list->sku == 0x802100) {
		if (g_BattleClansLadderDict.getEntries() > 2000) {
			g_BattleClansLadderDict.clear();
		}
	} else if (list->sku == 0x2300) {
		if (g_WDTLadderDict.getEntries() > 2000) {
			g_WDTLadderDict.clear();
		}
	} else {
		if (g_GeneralLadderDict.getEntries() > 2000) {
			g_GeneralLadderDict.clear();
		}
	}

	if (list && rung_list) {
		g_LadderPos = list->rung;
	}

	Wstring my_name = ((Wstring const &)g_NickName).toLower();
	int row = 0;
	while (temp) {
		if (temp->points == -1) {
			temp = temp->next;
			continue;
		}

		if (rung_list) {
			thecell.type = OwnerDraw::CellData::PRIMARY;
			swprintf(list_string, L"%d", temp->rung);
			SendMessage(rung_list, 0x4CC, (WPARAM)-1, (LPARAM)list_string);
			SendMessage(rung_list, 0x4A8, MAKEWPARAM(g_Col_Rung, row), (LPARAM)&thecell);

			thecell.type = OwnerDraw::CellData::TEXT;
			wchar_t wide_name[0x1400];
			FUN_00735060(wide_name, (char *)temp->login_name, -1);
			thecell.string.set(wide_name);
			SendMessage(rung_list, 0x4A8, MAKEWPARAM(g_Col_Name, row), (LPARAM)&thecell);

			swprintf(list_string, L"%d", temp->points);
			thecell.string.set(list_string);
			SendMessage(rung_list, 0x4A8, MAKEWPARAM(g_Col_Points, row), (LPARAM)&thecell);

			swprintf(list_string, L"%d", temp->wins);
			thecell.string.set(list_string);
			SendMessage(rung_list, 0x4A8, MAKEWPARAM(g_Col_Wins, row), (LPARAM)&thecell);

			swprintf(list_string, L"%d", temp->losses);
			thecell.string.set(list_string);
			SendMessage(rung_list, 0x4A8, MAKEWPARAM(g_Col_Losses, row), (LPARAM)&thecell);

			swprintf(list_string, L"%d", temp->disconnects);
			thecell.string.set(list_string);
			SendMessage(rung_list, 0x4A8, MAKEWPARAM(g_Col_Disconnects, row), (LPARAM)&thecell);

			row++;
		}

		Wstring ladder_name((char *)temp->login_name);
		ladder_name.toLower();

		if (list->sku == 0x802100) {
			Ladder old;
			Remove_Ladder(g_BattleClansLadderDict, ladder_name, old);
			g_BattleClansLadderDict.add(ladder_name, *temp);
		} else if (list->sku == 0x2300) {
			Ladder old;
			Remove_Ladder(g_WDTLadderDict, ladder_name, old);
			g_WDTLadderDict.add(ladder_name, *temp);
		} else {
			Ladder old;
			Remove_Ladder(g_GeneralLadderDict, ladder_name, old);
			g_GeneralLadderDict.add(ladder_name, *temp);
		}

		if (g_CurrentChannel.type == 0 && (DAT_00B77E48 == 2 || DAT_00B77E48 == 3)) {
			int count = g_UserList.length();
			User user;
			for (int i = 0; i < count; i++) {
				g_UserList.get(user, i);
				if (stricmp((char *)temp->login_name, (char *)user.name) == 0) {
					break;
				}
			}
			if (i < count) {
				g_UserList.remove(i);
				FUN_00783070(&user);
			}
		}

		if (my_name == ladder_name) {
			rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
			if (profile.Get() && profile.Get()->StatsDiffer(temp)) {
				profile.Get()->UpdateStats(temp);
				profile.Get()->Save(&ladder_name);
				HWND dialog = WS_Find_Dialog(0x10E);
				if (dialog == NULL) {
					dialog = WS_Find_Dialog(0x114);
				}
				if (dialog) {
					profile.Get()->FUN_0077A9F0(dialog, &g_NickName, false);
				}
			}
		}

		temp = temp->next;
	}

	if (rung_list) {
		SendMessage(rung_list, LB_SETCURSEL, (WPARAM)key_rung - top_rung, 0);
		InvalidateRect(rung_list, NULL, FALSE);
	} else {
		Draw_Player_List(0, false);
	}
	FUN_0079D110();
	HWND dialog = WS_Find_Dialog(0x116);
	if (dialog) {
		FUN_0079C040(dialog);
	}
	return 0;
}



class CChatEventSink : public AtlSinkRoot, public IUnknown
{
	public:
		CChatEventSink(void);
		HRESULT __stdcall OnFind(HRESULT result, Channel *channel);
		HRESULT __stdcall OnChannelJoin(HRESULT result, Channel *channel, User *user);
		static AtlIntMapEntry * __stdcall _GetEntries(void) { return DAT_007F7698; }
		IUnknown * _GetRawUnknown(void) { return (IUnknown *)((int)this + _GetEntries()->dw); }
		IUnknown * GetUnknown(void) { return _GetRawUnknown(); }
		long _InternalQueryInterface(IID const & iid, void ** object) { return InternalQueryInterface(this, _GetEntries(), iid, object); }
};

class CDownloadEventSink : public AtlSinkRoot, public IDownloadEvent
{
	public:
		CDownloadEventSink(void) {}
		HRESULT __stdcall OnEnd(void);
		HRESULT __stdcall OnError(int error);
		HRESULT __stdcall OnProgressUpdate(int bytesread, int totalsize, int timetaken, int timeleft);
		HRESULT __stdcall OnQueryResume(void);
		HRESULT __stdcall OnStatusUpdate(int status);
		static AtlIntMapEntry * __stdcall _GetEntries(void) { return DAT_007F7638; }
		IUnknown * _GetRawUnknown(void) { return (IUnknown *)((int)this + _GetEntries()->dw); }
		IUnknown * GetUnknown(void) { return _GetRawUnknown(); }
		long _InternalQueryInterface(IID const & iid, void ** object) { return InternalQueryInterface(this, _GetEntries(), iid, object); }
};

namespace ATL {

template <class Base>
class CComObject : public Base
{
	public:
		CComObject(void * = NULL) { InterlockedIncrement(&_Module.LockCount); }
		~CComObject(void)
		{
			RefCount = 1;
			FinalRelease();
			_Module.Unlock();
		}
		virtual unsigned long __stdcall AddRef(void) { return InternalAddRef(); }
		virtual unsigned long __stdcall Release(void)
		{
			unsigned long count = InternalRelease();
			if (count == 0) {
				delete this;
			}
			return count;
		}
		virtual long __stdcall QueryInterface(IID const & iid, void ** object) { return _InternalQueryInterface(iid, object); }
		static long __stdcall CreateInstance(CComObject<Base> ** pp);
};

template <class Base>
long __stdcall CComObject<Base>::CreateInstance(CComObject<Base> ** pp)
{
	long result = E_OUTOFMEMORY;
	CComObject<Base> * p = NULL;
	p = new CComObject<Base>();
	if (p != NULL) {
		p->SetVoid(NULL);
		p->InternalFinalConstructAddRef();
		result = p->FinalConstruct();
		p->InternalFinalConstructRelease();
		if (result != S_OK) {
			delete p;
			p = NULL;
		}
	}
	*pp = p;
	return result;
}

}

extern GUID const DAT_007F75E0;
extern GUID const DAT_007F7600;
extern GUID const DAT_007F75F0;
extern IID const DAT_007F7540;
extern IID const DAT_007F7580;
extern IID const DAT_007F7560;
extern IID const DAT_007F7550;
extern IID const DAT_007F7590;
extern IID const DAT_007F7570;
extern ATL::CComObject<CChatEventSink> * g_pChatSink;
extern ATL::CComObject<CNetUtilEventSink> * g_pNetUtilSink;
extern ATL::CComObject<CDownloadEventSink> * g_pDownloadSink;

static inline HRESULT AtlUnadvise(IUnknown * source, IID const & iid, DWORD cookie)
{
	ATL::CComPtr<IConnectionPointContainer> container;
	ATL::CComPtr<IConnectionPoint> point;
	HRESULT result = source->QueryInterface(IID_IConnectionPointContainer, (void **)&container.p);
	if (SUCCEEDED(result)) {
		result = container.p->FindConnectionPoint(iid, &point.p);
	}
	if (SUCCEEDED(result)) {
		result = point.p->Unadvise(cookie);
	}
	return result;
}

#define APP_REG_KEY "SOFTWARE\\Westwood\\Yuri's Revenge"

// FUNCTION: GAMEMD 0x00785370
int __fastcall Startup_Chat(HINSTANCE instance)
{
	HRESULT result = 0;
	_Module.Init(NULL, instance);
	g_pChat = NULL;
	g_pNetUtil = NULL;
	g_pDownload = NULL;
	g_dwChatAdvise = 0;
	g_dwNetUtilAdvise = 0;
	g_dwDownloadAdvise = 0;
	CoCreateInstance(DAT_007F75E0, NULL, CLSCTX_INPROC_SERVER, DAT_007F7540, (void **)&g_pChat);
	if (g_pChat) {
		ATL::CComObject<CChatEventSink>::CreateInstance(&g_pChatSink);
		result = AtlAdvise(g_pChat, g_pChatSink->GetUnknown(), DAT_007F7550, &g_dwChatAdvise);
	}
	if (g_pChat == NULL || result != S_OK) {
		ODMessageBox(StringTable::LoadString("TXT_APIMISSING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x23DC), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	CoCreateInstance(DAT_007F7600, NULL, CLSCTX_INPROC_SERVER, DAT_007F7580, (void **)&g_pNetUtil);
	if (g_pNetUtil) {
		ATL::CComObject<CNetUtilEventSink>::CreateInstance(&g_pNetUtilSink);
		result = AtlAdvise(g_pNetUtil, g_pNetUtilSink->GetUnknown(), DAT_007F7590, &g_dwNetUtilAdvise);
	}
	if (g_pNetUtil == NULL || result != S_OK) {
		ODMessageBox(StringTable::LoadString("TXT_APIMISSING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x23EC), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	CoCreateInstance(DAT_007F75F0, NULL, CLSCTX_INPROC_SERVER, DAT_007F7560, (void **)&g_pDownload);
	if (g_pDownload) {
		ATL::CComObject<CDownloadEventSink>::CreateInstance(&g_pDownloadSink);
		result = AtlAdvise(g_pDownload, g_pDownloadSink->GetUnknown(), DAT_007F7570, &g_dwDownloadAdvise);
	}
	if (g_pDownload == NULL || result != S_OK) {
		ODMessageBox(StringTable::LoadString("TXT_APIMISSING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x23FC), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	unsigned long version;
	g_pChat->GetVersion(&version);
	if (version < 0x10010) {
		ODMessageBox(StringTable::LoadString("TXT_APIWRONGVERSION", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2408), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	g_pChat->SetAttributeValue("RegPath", APP_REG_KEY);
	HKEY key;
	char path[256];
	if (APP_REG_KEY != NULL && strlen(APP_REG_KEY)) {
		strcpy(path, APP_REG_KEY);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &key) == ERROR_SUCCESS) {
			DWORD type;
			DWORD value;
			DWORD size = sizeof(value);
			if (RegQueryValueEx(key, "SKU", NULL, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
				g_pChat->SetProductSKU(value);
			}
			RegCloseKey(key);
		}
	}
	return 1;
}

static inline void Close_Page_Dialog(void)
{
	HWND window = WS_Find_Dialog(0x11E);
	if (window) {
		FUN_007757E0(window, 0);
	}
}

namespace OwnerDraw {
	BOOL CALLBACK SetUserData2(HWND window, LPARAM lparam);
}
int __fastcall Join_Lobby(bool flag);
bool FUN_0077CF40(void);
void __fastcall FUN_007A0EA0(char const * name, int ping);

void __fastcall FUN_00792EC0(unsigned int mask);

// FUNCTION: GAMEMD 0x00785740
void __fastcall Handle_User_Leave(User *user)
{
	stricmp(g_NickName.get(), (char *)user->name);
	if (g_CurrentChannel.type != 0) {
		VocClass::PlayGlobal(Rule->PlayerLeft, 1.0f, 0x2000, 0);
	}
	if (user->flags & 0x8000) {
		FUN_004068E0("Local user leaving channel\n");
		if (g_GameStarted != 0) {
			FUN_004068E0("Local user leaving channel after game start\n");
		}
		wchar_t name[64];
		FUN_00735060(name, (char *)g_CurrentChannel.name, -1);
		if (strncmp((char *)g_CurrentChannel.name, "Lob_41_", 7) == 0) {
			int number = atol((char *)g_CurrentChannel.name + strlen("Lob_41_"));
			int page = number / 8;
			int lobby = number % 8;
			if (page == 0) {
				swprintf(name, L"%s", g_Lobbies[lobby]);
			} else {
				swprintf(name, L"%s %d", g_Lobbies[lobby], page + 1);
			}
		}
		if (!FUN_0077CF40()) {
			FUN_005E2700(DAT_007EEF28,
						 StringTable::LoadString("TXT_YOULEFT", 0, "D:\\ra2mdpost\\wonline.cpp", 0x244c),
						 name);
		}
		g_ChannelCount = 0;
		FUN_004068E0("Channel Leave \n");
		memset(&g_CurrentChannel, 0, sizeof(g_CurrentChannel));
		memset(g_UserInfo, 0, sizeof(g_UserInfo));
		g_UserList.clear();
		g_IsChannelCreator = 0;
		FUN_00792EC0(0x40);
		Session.Options.ScenarioDescription[0] = 0;
		++Session.Suspended;
		Session.ScenarioFileLength = 0;
		Session.ScenarioDigest[0] = 0;
		HWND main = WS_Find_Dialog(0x113);
		if (main && DAT_00B77DC1) {
			ShowWindow(main, SW_HIDE);
			OwnerDraw::SetUserData2(main, 1);
		}
		HWND guest = WS_Find_Dialog(0xc9);
		if (guest) {
			FUN_007757E0(guest, 2);
			FUN_0072CF90();
			ShowWindow(guest, SW_HIDE);
			OwnerDraw::SetUserData2(guest, 1);
			if (Session.WOLWelcomeChoice == 4) {
				if (DAT_00B77DC1) {
					if (main) {
						ShowWindow(main, SW_HIDE);
						OwnerDraw::SetUserData2(main, 1);
					}
				} else {
					Join_Lobby(false);
					if (main) {
						PostMessageA(main, 0x593, 0, 0);
					}
				}
			} else if (Session.WOLWelcomeChoice == 5) {
				FUN_004068E0("Setting g_JoinLobbyNow, Guest\n");
				g_JoinLobbyNow = 1;
			}
		}
		HWND game = WS_Find_Dialog(0xc2);
		if (game) {
			FUN_007757E0(game, 2);
			FUN_0072CF90();
			ShowWindow(game, SW_HIDE);
			OwnerDraw::SetUserData2(game, 1);
			if (Session.WOLWelcomeChoice == 4) {
				if (DAT_00B77DC1) {
					if (main) {
						ShowWindow(main, SW_HIDE);
						OwnerDraw::SetUserData2(main, 1);
					}
				} else {
					Join_Lobby(false);
					if (main) {
						PostMessageA(main, 0x593, 0, 0);
					}
				}
			} else if (Session.WOLWelcomeChoice == 5) {
				FUN_004068E0("Setting g_JoinLobbyNow, Host\n");
			}
		}
		if (main) {
			if (!DAT_00B77DC1) {
				ShowWindow(main, SW_SHOW);
				OwnerDraw::SetUserData2(main, 0);
			}
			FUN_00781480();
		}
		--Session.Suspended;
		return;
	}
	if (g_GameStarted == 2) {
		FUN_004068E0("other player leaving channel after Game Started\n");
		return;
	}
	if (user) {
		for (int slot = 0; slot < 8; ++slot) {
			if (Session.Slots[slot] && !strcmp(FUN_007350C0(Session.Slots[slot]->Name), (char *)user->name)) {
				HWND lobby = FUN_005E26C0();
				if (lobby) {
					FUN_004E34A0(lobby, slot, -1);
				}
				break;
			}
		}
	}
	User other;
	for (int index = 0; index < g_UserList.length(); ++index) {
		g_UserList.get(other, index);
		if (!stricmp((char *)other.name, (char *)user->name)) {
			g_UserList.remove(index);
			if (g_CurrentChannel.type != 0) {
				memmove(&g_UserInfo[index],
						&g_UserInfo[index + 1],
						sizeof(ChannelUserInfo) * (15 - index));
				if (g_GameStarted != 2) {
					for (unsigned j = 1; j < 16; ++j) {
						g_UserInfo[j].Accepted = 0;
						EnableWindow(GetDlgItem(FUN_005E26C0(), 0x59f), 1);
					}
					if (!g_IsChannelCreator) {
						HWND dialog = WS_Find_Dialog(0xc9);
						FUN_005EDDE0(dialog, true);
					}
					FUN_005E9850();
				}
			}
			if (index == 0 && g_CurrentChannel.type != 0 && !g_IsChannelCreator) {
				g_pChat->RequestChannelLeave();
			}
			break;
		}
	}
	if (g_CurrentChannel.type != 0) {
		FUN_005E9340((char *)user->name);
		for (int player = 0; player < Session.Players.Count(); ++player) {
			if (!stricmp(FUN_007350C0(Session.Players[player]->Name), (char *)user->name)) {
				NodeNameType *entry = Session.Players[player];
				Session.Players.Delete_Index(player);
				Session.FUN_0069BBF0(player);
				delete entry;
				break;
			}
		}
		for (int slot = 0; slot < 8; ++slot) {
			if (!Session.Slots[slot] || !FUN_005E93F0(Session.Slots[slot])) {
				DAT_00B76440[slot] = -1;
			}
			char *name = FUN_007350C0(Session.Slots[slot]->Name);
			FUN_007A0EA0(name, DAT_00B76440[slot]);
		}
		if (g_IsChannelCreator) {
			FUN_005E9850();
			FUN_0077DB90();
			PumpGameopts(true, true);
			if (FUN_00775B20() == 0x6b) {
				HWND window = FUN_00775B10();
				int index = SendDlgItemMessageA(window, 0x6eb, 0x188, 0, 0);
				if (index != -1) {
					MultiplayerGameMode *mode =
						(MultiplayerGameMode *)SendDlgItemMessageA(window, 0x6eb, 0x199, index, 0);
					if (mode) {
						mode->vt_entry_90();
						Draw_Player_List(0, false);
						return;
					}
				}
			} else if (Session.MPGameMode) {
				Session.MPGameMode->vt_entry_90();
			}
		}
	}
	Draw_Player_List(0, false);
}

// FUNCTION: GAMEMD 0x00785EA0
void Logout_WOnline(void)
{
	g_IgnoreNetStatus = 1;
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		g_WDTTeams.clear();
	}
	if (g_pChat) {
		for (int i = 0; i < 18; i++) {
			ResetEvent(g_WaitEventHandles[i]);
		}
		WOL_Wait_Callback();
		if (g_pChat->RequestLogout() == 0) {
			Show_Wait_Window(0x20, StringTable::LoadString("TXT_WOL_LOGGING_OUT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2537), 0);
			for (int tries = 0; tries < 5; tries++) {
				DWORD index;
				while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT) {
					WOL_Wait_Callback();
				}
				ResetEvent(g_WaitEventHandles[index]);
				if (index == 0 || index == 9) {
					break;
				}
			}
		}
		Close_Page_Dialog();
		FUN_00792EC0(0x20);
	}
}

extern char DAT_00B77E75;
void FUN_005E8A20(void);

// FUNCTION: GAMEMD 0x00786080
void Shutdown_Chat(void)
{
	if (MultiplayerMapPreview) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = NULL;
	}
	Close_Page_Dialog();
	if (DAT_00B77E75) {
		FUN_005E8A20();
		if (g_pChat) {
			AtlUnadvise(g_pChat, DAT_007F7550, g_dwChatAdvise);
			g_pChat->Release();
		}
		if (g_pNetUtil) {
			AtlUnadvise(g_pNetUtil, DAT_007F7590, g_dwNetUtilAdvise);
			g_pNetUtil->Release();
		}
		if (g_pDownload) {
			AtlUnadvise(g_pDownload, DAT_007F7570, g_dwDownloadAdvise);
			g_pDownload->Release();
		}
		g_pChat = NULL;
		g_pNetUtil = NULL;
		g_pDownload = NULL;
		for (int i = 0; i < 18; i++) {
			CloseHandle(g_WaitEventHandles[i]);
		}
		AtlModuleTerm(&_Module);
		DAT_00B77E75 = 0;
	}
}

extern char g_WDTHost[];
extern int g_WDTPort;
extern int g_LanguageCode;

// FUNCTION: GAMEMD 0x00786270
bool Get_WDT_State_Silent(void)
{
	DWORD value = 0;
	HKEY key;
	char path[256];
	if (APP_REG_KEY != NULL && strlen(APP_REG_KEY)) {
		strcpy(path, APP_REG_KEY);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &key) == ERROR_SUCCESS) {
			DWORD type;
			DWORD size = sizeof(value);
			RegQueryValueEx(key, "SKU", NULL, &type, (LPBYTE)&value, &size);
			RegCloseKey(key);
		}
	}
	g_LanguageCode = value & 0xFF;
	if (g_pChat != NULL) {
		g_pChat->SetProductSKU(value);
	}
	if (g_pNetUtil != NULL) {
		FUN_004068E0("Get_WDT_State_Silent: WDTHost is %s, port is %d\n", g_WDTHost, g_WDTPort);
		g_pNetUtil->RequestWDTState(g_WDTHost, g_WDTPort, 3);
		return true;
	}
	return false;
}

void FUN_005E87A0(void);
void FUN_005E8B40(void);
void FUN_005E8B60(void);
int __fastcall Startup_Chat(HINSTANCE instance);

// FUNCTION: GAMEMD 0x00786390
int FUN_00786390(void)
{
	if (DAT_00B77E75) {
		return 1;
	}
	DAT_00B77E75 = 1;
	FUN_005E87A0();
	FUN_005E8B40();
	FUN_005E8B60();
	DWORD value = 0;
	HKEY key;
	char path[256];
	if (APP_REG_KEY != NULL && strlen(APP_REG_KEY)) {
		strcpy(path, APP_REG_KEY);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &key) == ERROR_SUCCESS) {
			DWORD type;
			DWORD size = sizeof(value);
			RegQueryValueEx(key, "SKU", NULL, &type, (LPBYTE)&value, &size);
			RegCloseKey(key);
		}
	}
	value &= 0xFF;
	g_LanguageCode = value;
	g_GameSKU = value + 0x2900;
	Reset_WOL_Globals();
	g_IgnoreNetStatus = 1;
	for (int i = 0; i < 18; i++) {
		g_WaitEventHandles[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	return Startup_Chat(ProgramInstance);
}

int __fastcall Run_Dialog_Control(int id, DLGPROC proc, OwnerDraw::DialogControl * control);
BOOL CALLBACK Select_WDT_Location_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern int g_LadderServerPort;
extern char g_LadderServerHost[];
void __fastcall FUN_00779F10(Wstring const & value);
bool g_PlaintextPassword = true;

static inline Wstring Strip_Server_Prefix(Server * server)
{
	Wstring text((char *)server->name);
	Wstring prefix;
	int pos = text.getToken(0, ":", prefix);
	if (pos >= 0) {
		text.remove(0, pos + 1);
	}
	return text;
}

static inline int Pick_Locale(void)
{
	SimpleWonlineDialogControl dialog;
	int result = Run_Dialog_Control(0xFE, Select_WDT_Location_Proc, &dialog);
	if (result < 0) {
		return 0;
	}
	return result;
}

static inline void Set_Locale_Entry(Wstring & key, int locale)
{
	int old;
	if (g_UserLocales.remove(key, old)) {
		g_UserLocales.add(key, locale);
	}
}

static inline void Apply_Locale(Wstring & key, int locale, User & user)
{
	g_pChat->RequestSetLocale((Locale)locale);
	g_pChat->RequestUserLocale(&user);
	Set_Locale_Entry(key, locale);
}

static inline void Check_Locale(Wstring & key, int & locale, rc_ptr<PlayerProfile> & profile, User & user)
{
	if (profile.Get() != NULL && profile.Get()->Locale != locale) {
		if (profile.Get()->Locale == 0) {
			profile.Get()->SetLocale(locale, 2);
			profile.Get()->Save(&g_NickName);
		}
		int value = profile.Get()->Locale;
		g_pChat->RequestSetLocale((Locale)value);
		g_pChat->RequestUserLocale(&user);
		locale = value;
		int old;
		if (g_UserLocales.remove(key, old)) {
			g_UserLocales.add(key, locale);
		}
	}
}

static inline int Wait_For_Event(void)
{
	int timeout = 30000;
	int wait;
	while ((wait = WaitForMultipleObjects(0x12, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT) {
		timeout -= 100;
		if (timeout <= 0) {
			return 0x12;
		}
		WOL_Wait_Callback();
	}
	ResetEvent(g_WaitEventHandles[wait]);
	return wait;
}

bool __fastcall FUN_007864C0(Server * server);

// FUNCTION: GAMEMD 0x007864C0
bool __fastcall FUN_007864C0(Server * server)
{
	strcpy((char *)server->login, g_NickName.get());
	strcpy((char *)server->password, g_LoginPassword.get());
	for (int i = 0; i < 18; i++) {
		ResetEvent(g_WaitEventHandles[i]);
	}
	{
		WideWstring text(StringTable::LoadString("TXT_CONNECTING_SERVER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x25E6));
		Show_Wait_Window(0x10, text, 0);
	}
	g_pChat->RequestConnection(server, 0x14, g_PlaintextPassword);
	int tries = 0x14;
	for (;;) {
		int wait = Wait_For_Event();
		if (wait == 0) {
			FUN_00792EC0(0xFFFFFFFF);
			return false;
		}
		if (wait == 5) {
			ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x25F3), 0, FUN_0077D710, 0);
			FUN_00792EC0(0xFFFFFFFF);
			return false;
		}
		if (wait == 0x12) {
			FUN_00792EC0(0xFFFFFFFF);
			return false;
		}
		if (tries-- < 0) {
			ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x25FA), 0, FUN_0077D710, 0);
			FUN_00792EC0(0xFFFFFFFF);
			return false;
		}
		if (wait == 2) {
			break;
		}
	}
	FUN_00792EC0(0xFFFFFFFF);
	g_pChat->SetClientVersion(0x10010);
	g_IgnoreNetStatus = 0;
	Session.Read_MultiPlayer_Settings();
	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) {
		profile = FUN_00778A20(&g_NickName);
	} else {
		profile = DAT_00B75E48;
	}
	if (Session.WOLWelcomeChoice == 4 || Session.WOLWelcomeChoice == 5) {
		{
			Wstring name = Strip_Server_Prefix(server);
			profile.Get()->SetName(&name);
		}
		if (options.Is_Bit1_Clear()) {
			profile.Get()->Save(&g_NickName);
		}
	}
	strcpy(Session.Handle, g_NickName.get());
	Session.FUN_0069B760(profile.Get()->Country, false);
	Session.FUN_0069B7E0(profile.Get()->Field20, false);
	Session.Write_MultiPlayer_Settings();
	FUN_00779F10(g_NickName);
	if (g_pNetUtil != NULL && strlen(g_LadderServerHost) != 0) {
		g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, g_NickName.get(), g_GameSKU & 0xFFFF00, -1, 0, 0);
		g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, g_NickName.get(), g_GameSKU & 0xFFFF00);
	}
	for (int j = 0; j < 18; j++) {
		ResetEvent(g_WaitEventHandles[j]);
	}
	User user;
	memset(&user, 0, sizeof(user));
	strcpy((char *)user.name, g_NickName.get());
	g_pChat->RequestUserLocale(&user);
	for (;;) {
		int wait = Wait_For_Event();
		if (wait == 0) {
			FUN_00792EC0(0xFFFFFFFF);
			break;
		}
		if (wait == 5) {
			ODMessageBox(StringTable::LoadString("TXT_WDT_NET_ERR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x264F), 0, FUN_0077D710, 0);
			FUN_00792EC0(0xFFFFFFFF);
			break;
		}
		if (wait == 0x12) {
			FUN_00792EC0(0xFFFFFFFF);
			break;
		}
		if (wait == 0xE) {
			break;
		}
	}
	Wstring key = ((Wstring const &)g_NickName).toLower();
	if (g_UserLocales.contains(key)) {
		int locale = 0;
		g_UserLocales.getValue(key, locale);
		if (locale != 0) {
			Check_Locale(key, locale, profile, user);
			return true;
		}
	}
	int pick = 0;
	if (profile.Get() != NULL) {
		pick = profile.Get()->Locale;
	}
	if (pick == 0) {
		pick = Pick_Locale();
		if (profile.Get() != NULL) {
			profile.Get()->SetLocale(pick, 3);
			profile.Get()->Save(&g_NickName);
		}
	}
	if (pick != 0) {
		Apply_Locale(key, pick, user);
	}
	return true;
}
// FUNCTION: GAMEMD 0x00786D90
bool FUN_00786D90(void)
{
	if (g_CurrentServerIndex >= 1 && g_CurrentServerIndex < g_Servers.length()) {
		Server * server = NULL;
		g_Servers.getPointer(&server, g_CurrentServerIndex);
		return FUN_007864C0(server);
	}
	return false;
}

// FUNCTION: GAMEMD 0x00786DC0
bool __fastcall FUN_00786DC0(Wstring & name)
{
	g_LoginPassword.clear();
	for (int i = 1; i <= 32; i++) {
		char const * nick;
		char const * pass;
		if (g_pChat->GetNick(i, &nick, &pass) == 0 && nick != NULL && strlen(nick) != 0 && strcmpi(name.text(), nick) == 0) {
			g_NickName = nick;
			g_LoginPassword = pass;
			g_PlaintextPassword = 0;
			return true;
		}
	}
	g_NickName.clear();
	return false;
}

// FUNCTION: GAMEMD 0x00786E70
int __fastcall FUN_00786E70(Wstring & name)
{
	if (name.length()) {
		for (int i = 1; i < g_Servers.length(); i++) {
			Server * server = NULL;
			g_Servers.getPointer(&server, i);
			char * colon = strchr((char *)server->name, ':');
			if (colon != NULL && *colon != 0 && name == colon + 1) {
				return i;
			}
		}
	}
	return -1;
}

extern long g_OnConnectionResult;
int __cdecl sprintf(char * buffer, const char * format, ...);

// FUNCTION: GAMEMD 0x00786EF0
bool FUN_00786EF0(void)
{
	bool result = false;
	long error = g_OnConnectionResult;
	if (error == 0x80040077 || error == 0x80040078 || error == 0x80040072 || error == 0x80040074 || error == 0x80040075 || error == 0x80040076) {
		result = true;
		if (ODMessageBox(g_LastErrorMessage, 4, FUN_0077D710, 0) == IDYES) {
			char url[260];
			int sku = g_LanguageCode;
			switch (g_OnConnectionResult) {
				case 0x80040077:
					sprintf(url, "http://apiregister.westwood.com/support/serialunknown_%d.html", sku);
					break;
				case 0x80040078:
					sprintf(url, "http://apiregister.westwood.com/support/skuserialmismatch_%d.html", sku);
					break;
				case 0x80040072:
					sprintf(url, "http://apiregister.westwood.com/support/banned_%d.html", sku);
					break;
				case 0x80040074:
					sprintf(url, "http://apiregister.westwood.com/support/disabled_%d.html", sku);
					break;
				case 0x80040075:
					sprintf(url, "http://apiregister.westwood.com/support/serialbanned_%d.html", sku);
					break;
				case 0x80040076:
					sprintf(url, "http://apiregister.westwood.com/support/serialdup_%d.html", sku);
					break;
			}
			ViewHTML(url, 1);
		}
	} else {
		ODMessageBox(g_LastErrorMessage, 0, FUN_0077D710, 0);
	}
	return result;
}

Wstring __fastcall FUN_00779B50(void);
Wstring __fastcall FUN_00779DD0(void);
void __fastcall FUN_00779C90(Wstring const & value);
bool FUN_00779730(void);
int __fastcall FUN_00797640(int sku);
int __fastcall StoreLogin(char const * login, char const * password, bool plain, int locale);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void Title_Screen_Restore(bool flag);
void FUN_0072C600(void);
void FUN_0072C650(void);
void FUN_0072BCA0(void);
void FUN_0072BCF0(void);
BOOL CALLBACK FUN_00799EA0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK FUN_0079AE00(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern bool DAT_00B77E68;
extern char DAT_00889F64[];
namespace UI { void __fastcall EndDialog(HWND window); }

static inline bool Is_Internal_Session(void)
{
	return Session.WOLWelcomeChoice == 3 || Session.WOLWelcomeChoice == 1 || Session.WOLWelcomeChoice == 2;
}

static void Focus_Dialog_Item(HWND window, int id)
{
	SetFocus(GetDlgItem(window, id));
}

static void Clear_Channel_Lists(void)
{
	memset(&g_CurrentChannel, 0, sizeof(g_CurrentChannel));
	memset(&s_TempChannel, 0, sizeof(s_TempChannel));
	g_UserChannels.setEmpty();
	g_GameChannelList.setEmpty();
	g_UserList.setEmpty();
	g_ChannelCount = 0;
}

static int Fetch_Server_List(void)
{
	{
		WideWstring text(StringTable::LoadString("TXT_FETCHING_SERVLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5EEC));
		Show_Wait_Window(0x10, text, false);
	}
	int sku = 0x2300;
	if (Session.WOLWelcomeChoice != 3) {
		sku = 0x2900;
	}
	return FUN_00797640(sku);
}

// FUNCTION: GAMEMD 0x00787010
int Login_WOL(void)
{
	bool failed = false;
	g_OnConnectionResult = 0;
	int result = FUN_00786390();
	if (result != 1) {
		return result;
	}
	Clear_Channel_Lists();
	result = 0;
	int error = Fetch_Server_List();
	switch (error) {
		case 0:
			break;
		case 2:
			failed = true;
			result = -1;
			break;
		default:
			failed = true;
			result = -2;
			break;
	}
	WOLOptionsClass options;
	if (!failed) {
		if (options.Is_Bit0_Clear()) {
			Wstring persona = FUN_00779B50();
			if (persona.length() > 0 && FUN_00786DC0(persona)) {
				int index = -1;
				if (Is_Internal_Session()) {
					rc_ptr<PlayerProfile> profile = FUN_00778A20(&persona);
					if (profile.Get()) {
						index = FUN_00786E70(profile->Name);
					}
					if (index == -1) {
						index = g_CurrentServerIndex;
					}
				} else {
					rc_ptr<PlayerProfile> profile = FUN_00778A20(&persona);
					if (profile.Get()) {
						if (profile->Name.length() == 0) {
							index = g_CurrentServerIndex;
						} else {
							index = FUN_00786E70(profile->Name);
						}
					}
				}
				if (index != -1) {
					g_CurrentServerIndex = index;
					if (FUN_00786D90()) {
						return 1;
					}
					failed = FUN_00786EF0();
				}
			}
		}
		if (g_NickName.length() == 0 && options.Is_Bit1_Clear()) {
			g_NickName = FUN_00779DD0();
		}
		{
			Wstring empty(DAT_00889F64);
			FUN_00779C90(empty);
		}
		if (failed) {
			return 0;
		}
		FUN_004068E0("Creating login dlg\n");
		SimpleWonlineDialogControl control;
		int dialog_id = options.Is_Bit1_Clear() ? 0x10F : 0x11D;
		FUN_0072C600();
		FUN_0052FEC0(NULL, true);
		HWND window = UI::BeginDialog(dialog_id, FUN_00799EA0, (int)&control);
		OwnerDraw::Display_Dialog(window);
		if (g_NickName.length() > 0 && g_LoginPassword.length() == 0) {
			Focus_Dialog_Item(window, 0x72C);
		} else {
			Focus_Dialog_Item(window, 0x628);
		}
		while (!control.F2() && !WOL_Wait_Callback() && !UI::Updated()) {
			Title_Screen_Restore(false);
		}
		UI::EndDialog(window);
		FUN_0072C650();
		if (control.F3() == 2) {
			failed = true;
		} else {
			WOL_Wait_Callback();
			g_IgnoreNetStatus = false;
			WOLOptionsClass saved;
			rc_ptr<PlayerProfile> profile;
			if (saved.Is_Bit1_Clear()) {
				profile = FUN_00778A20(&g_NickName);
			} else {
				profile = DAT_00B75E48;
			}
			int found = FUN_00786E70(profile->Name);
			if (found != -1) {
				g_CurrentServerIndex = found;
			}
			if (g_CurrentServerIndex >= g_Servers.length()) {
				g_CurrentServerIndex = -1;
			}
			if (g_CurrentServerIndex == -1 && g_Servers.length() > 1) {
				g_CurrentServerIndex = 1;
			}
			if (FUN_00786D90()) {
				if (DAT_00B77E68 && saved.Is_Bit1_Clear()) {
					int locale = profile->Locale;
					StoreLogin(g_NickName.get(), g_LoginPassword.get(), g_PlaintextPassword, locale);
					Session.Write_MultiPlayer_Settings();
				}
				if (!saved.Is_Bit0_Clear() || !FUN_00779730()) {
					return 1;
				}
				FUN_0052FEC0(NULL, true);
				SimpleWonlineDialogControl next;
				FUN_0072BCA0();
				Run_Dialog_Control(0x112, FUN_0079AE00, &next);
				FUN_0072BCF0();
				return 1;
			}
		}
	}
	FUN_00792EC0(0xFFFFFFFF);
	if (!failed) {
		FUN_00786EF0();
	}
	g_IgnoreNetStatus = true;
	return result;
}

extern char g_WDTSoundsInited;
extern char DAT_0084A210;
void Init_WDT_Sounds(void);
void Play_WDT_Sound(ArrayList<Wstring> list);
void FUN_0079D4E0(void);
bool FUN_0052F3F0(void);
void __fastcall Set_Wait_Dialog_Text(wchar_t const * text);
void __fastcall FUN_00792EC0(unsigned int mask);
void FUN_0072CD60(void);
void FUN_0072CDB0(void);
BOOL CALLBACK WOL_Main_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

static inline void Reset_Wait_Event_At(int index)
{
	ResetEvent(g_WaitEventHandles[index]);
}

static inline bool Is_WDT_Session(void)
{
	return Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3;
}

// FUNCTION: GAMEMD 0x00787770
int __fastcall Join_WOL_Lobby(HWND win, bool flag)
{
	int retval = 0;

	if (DAT_00B77DC1 && Is_WDT_Session() && Session.Winner != -1 && Session.Winner < 8) {
		Wstring name = Narrow_Text(Session.Score[Session.Winner].Name);
		if (strnicmp(name.get(), Session.Handle, sizeof(Session.Handle)) == 0) {
			if (Session.House == 0) {
				Play_WDT_Sound(g_WDT_GDIWinEmphasisSounds);
			} else if (Session.House == 1) {
				Play_WDT_Sound(g_WDT_NodWinEmphasisSounds);
			}
		} else if (Session.House == 0) {
			Play_WDT_Sound(g_WDT_GDILoseEmphasisSounds);
		}
	}

	DAT_00B77DC1 = 0;
	g_GameStarted = 0;
	if (Is_WDT_Session()) {
		g_pChat->RequestSetTeam(Session.House);
		if (!g_WDTSoundsInited) {
			Init_WDT_Sounds();
		}
	}
	FUN_0052F3F0();

	if (g_ConnectionLost) {
		int result = Login_WOL();
		if (result != 1) {
			return result;
		}
	}

	FUN_00798340();

	if (win == 0) {
		FUN_0052FEC0(NULL, true);
		FUN_0072CD60();
		win = UI::BeginDialog(0x113, WOL_Main_Dialog_Proc, 0);
		if (g_MessageOfTheDay.length() > 0) {
			char * motd = new char[g_MessageOfTheDay.length() + 1];
			if (motd) {
				strcpy(motd, g_MessageOfTheDay.get());
				char * token = strtok(motd, "\n");
				while (token) {
					WideWstring line = FUN_007B7ED0(token);
					PMessagePrintf(DAT_007EEF28, line.get());
					token = strtok(NULL, "\n");
				}
				PMessagePrintf(DAT_007EEF28, L"\n");
				g_MessageOfTheDay.clear();
				delete [] motd;
			}
		}
		Show_Wait_Window(0x10, StringTable::LoadString("GUI:LoginComplete", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2894), 0);
	}

	g_SquadInfoRequests++;
	g_RequestingOwnSquadInfo = 1;
	g_pChat->RequestSquadInfo(0);

	FUN_0079D4E0();

	DAT_0084A1BC = 0x29;
	Set_Wait_Dialog_Text(StringTable::LoadString("TXT_REQ_CHANLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x28A8));
	if (!FUN_0077F520(true)) {
		goto cleanup;
	}
	Set_Wait_Dialog_Text(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x28AF));
	{
		int lobby = Join_Lobby(DAT_0084A210 && !flag);
		DAT_0084A210 = 1;
		if (lobby == -1) {
			goto cleanup;
		}
	}

	DWORD idx;
	while ((idx = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT) {
		WOL_Wait_Callback();
	}
	Reset_Wait_Event_At(idx);
	if (idx == 0 || idx == 5) {
		goto logout;
	}

	g_JoinLobbyNow = 0;
	PostMessageA(win, 0x593, 0, 0);
	FUN_00792EC0(0xFFFFFFFF);
	SetFocus(MainWindow);
	ShowWindow(win, SW_SHOWNORMAL);
	SetFocus(win);
	SetFocus(GetDlgItem(win, 0x540));
	DAT_00B77DBC = 0;
	DAT_00B77DB8 = time(0);

	while (true) {
		while ((idx = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20)) == WAIT_TIMEOUT) {
			WOL_Wait_Callback();
			if (g_JoinLobbyNow) {
				EnableWindow(GetDlgItem(win, 0x53D), FALSE);
				EnableWindow(GetDlgItem(win, 0x6EE), FALSE);
				EnableWindow(GetDlgItem(win, 0x62D), FALSE);
				EnableWindow(GetDlgItem(win, 0x62B), FALSE);
				EnableWindow(GetDlgItem(win, 0x689), FALSE);
				EnableWindow(GetDlgItem(win, 0x688), FALSE);
				EnableWindow(GetDlgItem(win, 0x7A3), FALSE);
				EnableWindow(GetDlgItem(win, 0x7A6), FALSE);
				EnableWindow(GetDlgItem(win, 0x61F), FALSE);
				EnableWindow(GetDlgItem(win, 0x5C2), FALSE);
				EnableWindow(GetDlgItem(win, 0x686), FALSE);
				if (Join_Lobby(false) == -1) {
					EnableWindow(GetDlgItem(win, 0x686), TRUE);
					SetEvent(g_WaitEventHandles[5]);
				} else {
					PostMessageA(win, 0x593, 0, 0);
				}
				g_JoinLobbyNow = 0;
			}
		}
		Reset_Wait_Event_At(idx);
		if (idx == 0) {
			goto cleanup;
		}
		if (idx != 6) {
			if (idx == 15) {
				retval = 2;
				break;
			}
			if (idx == 5) {
				goto cleanup;
			}
			if (idx == 7) {
				goto started;
			}
		} else {
			break;
		}
	}
	g_LastErrorMessage[0] = 0;

logout:
	WS_Destroy_Dialog(win, 0);
	WOL_Wait_Callback();
	win = 0;
	if (g_pChat) {
		g_pChat->RequestChannelLeave();
	}

cleanup:
	FUN_00792EC0(0xFFFFFFFF);
	ODMessageBox(g_LastErrorMessage, 0, FUN_0077D710, 0);
	if (win) {
		WS_Destroy_Dialog(win, 0);
	}
	while (WS_Destroy_Dialog(NULL, 0)) {
	}
	FUN_0072CDB0();
	if (retval == 1) {
		DAT_00B77DC1 = 1;
	}
	if (Is_WDT_Session()) {
		g_WDTTeams.clear();
	}
	return retval;

started:
	retval = 1;
	g_pChat->RequestChannelLeave();
	g_GameStartRequested = 0;
	g_LastErrorMessage[0] = 0;
	goto cleanup;
}


#include "_mixfile.h"
#include "_pk.h"
#include "ccfile.h"
#include "mixfile.h"
#include "worlddom.h"
#include "wdtnet.h"

extern wchar_t * _tername;
extern int g_CurrentLobby;
rc_ptr<WDTState> * __cdecl WDT_Get_New_State(void);
static inline void Set_Lobby_Number(Channel & chan)
{
	g_CurrentLobby = atol((char *)chan.name + strlen("Lob_41_"));
}


// FUNCTION: GAMEMD 0x00788180
int __fastcall Pick_Lobby(bool flag)
{
	int count = g_UserChannels.length();
	int user_cap = 25;
	Channel chan;
	memset(&chan, 0, sizeof(chan));
	char name[32];
	char buf[32];
	wchar_t wide_name[32];
	char def[32];
	char entry[32];
	int i;

	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		memset(buf, 0, sizeof(buf));
		memset(wide_name, 0, sizeof(buf));
		memset(entry, 0, sizeof(entry));
		memset(def, 0, sizeof(def));

		MixFileClass * mix = new MixFileClass("WDT.MIX", &FastKey);

		INIClass ini;
		CCFileClass file("WDTMAPMD.INI");
		ini.Load(file, false);
		sprintf(entry, "Territory%02d", Session.WDTTerritory);
		strcpy(def, entry);
		if (((WDTState *)WDT_Get_New_State()->Ptr)->MapID == 0) {
			ini.Get_String("NorthAmerica", entry, def, buf, sizeof(def));
		} else {
			ini.Get_String("Europe", entry, def, buf, sizeof(def));
		}
		ini.Get_String(buf, "Name", "A territory", name, sizeof(buf));
		FUN_00735060(wide_name, name, -1);
		_tername = _wcsdup(wide_name);
		delete mix;
	}

	Wstring lobby = Wstring("Lob_41_") + Wstring_FromInt(g_CurrentLobby, 1);

	for (i = 0; i < count; i++) {
		g_UserChannels.get(chan, i);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			if (wcscmp(g_Lobbies[i], wide_name) == 0) {
				if (!flag || (int)chan.currentUsers <= user_cap) {
					return i;
				}
			} else {
				continue;
			}
		}
		Wstring other((char *)chan.name);
		if (other == lobby) {
			if (!flag || (int)chan.currentUsers <= user_cap) {
				return i;
			}
		}
	}

	int best = -1;
	unsigned int fewest = 10000;
	for (i = 0; i < count; i++) {
		g_UserChannels.get(chan, i);
		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			if (wcsncmp(g_Lobbies[i], wide_name, wcslen(wide_name)) == 0) {
				if (!flag || (int)chan.currentUsers <= user_cap) {
					Set_Lobby_Number(chan);
					return i;
				}
				if (chan.currentUsers < fewest) {
					best = i;
					fewest = chan.currentUsers;
				}
			} else {
				continue;
			}
		}
		if (strncmp((char *)chan.name, "Lob_41_", strlen("Lob_41_")) == 0) {
			if (!flag || (int)chan.currentUsers <= user_cap) {
				Set_Lobby_Number(chan);
				return i;
			}
			if (chan.currentUsers < fewest) {
				best = i;
				fewest = chan.currentUsers;
			}
		}
	}

	if (best != -1) {
		g_UserChannels.get(chan, best);
		g_CurrentLobby = atol((char *)chan.name + strlen("Lob_41_"));
	}
	return best;
}

bool FUN_0052F3F0(void);
void __fastcall FUN_0079FC10(int a, int b, int c);
void __fastcall FUN_005E7460(void);
extern char WestwoodOnline_Clan1_Players[];
extern char WestwoodOnline_Clan2_Players[];
extern int DAT_00841F34;
extern int DAT_00841F38;
extern int DAT_00841F3C;

extern long prev_stat;
void __fastcall FUN_005E3720(char const * text);
extern char DAT_00889F64[];
int __fastcall StoreLogin(char const * login, char const * password, bool plain, int locale);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void __fastcall FUN_00779C90(Wstring const & value);
extern char DAT_00889F64[];
int __fastcall FUN_00797640(int sku);
BOOL CALLBACK FUN_0079AE00(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
int __fastcall Run_Dialog_Control(int id, DLGPROC proc, OwnerDraw::DialogControl * control);
BOOL CALLBACK WOL_Password_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
namespace Unsorted { extern int MuteSWLaunches; }
extern int DAT_007EEF28;
extern char DAT_00B77E30[];
bool __fastcall FUN_0069BBE0(SessionClass * session);
extern int DAT_007EEF2C;
void __cdecl FUN_005E2790(int color, wchar_t const * text, ...);
extern int DAT_00B76C20;
extern int DAT_00B77E90;
bool __fastcall FUN_0079BA60(Channel * chan, bool flag, bool extra);

// FUNCTION: GAMEMD 0x00788710
bool __fastcall Leave_Channel(int id, int flag)
{
	if (g_ChannelCount) {
		FUN_004068E0("Leave_Channel, channel %s type %d\n", g_CurrentChannel.name, g_CurrentChannel.type);
		g_pChat->RequestChannelLeave();
		Show_Wait_Window(id, StringTable::LoadString("WOL:LeavingChannel", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x29D0), flag);
		int tries = 0;
		while (g_ChannelCount && tries < 150) {
			Sleep(100);
			WOL_Wait_Callback();
			tries++;
		}
		FUN_00792EC0(0x40);
	}
	return g_ChannelCount == 0;
}


// FUNCTION: GAMEMD 0x00788800
long __fastcall FUN_00788800(Channel * chan, Wstring & key, WideWstring text)
{
	if (strcmp((char *)chan->name, (char *)g_CurrentChannel.name) == 0 || strcmp((char *)chan->name, (char *)s_TempChannel.name) == 0) {
		return 0;
	}
	if (!Leave_Channel(0x40, 1)) {
		return 1;
	}
	strcpy((char *)chan->key, key.text());
	s_TempChannel = *chan;
	Show_Wait_Window(2, text.text(), 0);
	FUN_004068E0("Requesting channel join %s\n", chan->name);
	return g_pChat->RequestChannelJoin(chan);
}


// FUNCTION: GAMEMD 0x00788A30
int __fastcall Join_Lobby(bool flag)
{
	int lobby = Pick_Lobby(flag);
	if (lobby != -1) {
		Channel chan;
		g_UserChannels.get(chan, lobby);
		long result;
		{
			Wstring key("zotclot9");
			result = FUN_00788800(&chan, key, WideWstring(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2A01)));
		}
		if (result == 0) {
			HWND dialog = WS_Find_Dialog(0x113);
			if (dialog) {
				SetFocus(GetDlgItem(dialog, 0x540));
			}
			FUN_00781480();
			return lobby;
		}
	}
	FUN_00781480();
	return -1;
}

HWND WS_Create_Dialog(HINSTANCE instance, int id, HWND parent, DLGPROC proc, BOOL force_show);
namespace UI { void __fastcall RegisterWindow(HWND window, int id); }
namespace OwnerDraw {
	void __fastcall DrawCampaignMenuTransition(HWND window, bool reverse);
}
void FUN_0072C240(void);
void FUN_0072C290(void);
void FUN_0072C420(void);
void FUN_0072C470(void);
BOOL CALLBACK WOL_Begin_Nick_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK WOL_New_Nick_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern int DAT_00B73A1C;
extern int DAT_00B73AB8;
extern int DAT_00B7782C;
extern int DAT_00B76228;
extern long g_OnNewNickResult;
extern int g_AgeCheckResult;


static inline void Set_Item_Text(HWND dialog, int id, char const * text)
{
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4B4, 0, (LPARAM)text);
}

// FUNCTION: GAMEMD 0x00788B00
void __fastcall NewLogin(HWND win)
{
	char nick[32];
	char pass[32];
	char verify[32];
	char email[128];
	char parent_email[128];
	int newsletter;
	int shareinfo;
	int month;
	int day;
	int year;
	int dialog_id;
	HWND dlg;
	int wait;

	OwnerDraw::DrawCampaignMenuTransition(win, false);
	FUN_0052FEC0(NULL, true);
	newsletter = 1;
	month = 1;
	day = 0;
	year = 0;
	for (int i = 0; i < 18; i++) {
		Reset_Wait_Event_At(i);
	}
	memset(nick, 0, sizeof(nick));
	memset(pass, 0, sizeof(pass));
	memset(verify, 0, sizeof(verify));
	memset(email, 0, sizeof(email));
	memset(parent_email, 0, sizeof(parent_email));
	Show_Wait_Window(0x80, StringTable::LoadString("WOL:NewAccountFetch", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2A4A), 0);
	FUN_004068E0("Newnick update check\n");
	g_pChat->RequestServerList(g_GameSKU, 0x10001, "TibSun", "TibPass99", 0x23);
	do {
		wait = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100);
		while (wait == WAIT_TIMEOUT) {
			WOL_Wait_Callback();
			wait = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100);
		}
		Reset_Wait_Event_At(wait);
	} while (wait != 0 && wait != 1);
	FUN_00792EC0(0xFFFFFFFF);
	if (wait == 0) {
		ShowWindow(win, SW_SHOW);
		OwnerDraw::SetUserData2(win, 0);
		return;
	}
	if (wait == 8) {
		SetEvent(g_WaitEventHandles[8]);
		ShowWindow(win, SW_SHOW);
		OwnerDraw::SetUserData2(win, 0);
		return;
	}

first_nick_retry:
	FUN_0072C240();
	FUN_0052FEC0(NULL, true);
	dlg = WS_Create_Dialog(ProgramInstance, 0xE7, MainWindow, WOL_Begin_Nick_Dialog_Proc, FALSE);
	UI::RegisterWindow(dlg, 0xE7);
	SendDlgItemMessage(dlg, 0x62E, 0x14E, month - 1, 0);
	if (day > 0) {
		Wstring text = Wstring_FromInt(day);
		Set_Item_Text(dlg, 0x62F, text.get());
	} else {
		Set_Item_Text(dlg, 0x62F, DAT_00889F64);
	}
	if (year > 0) {
		Wstring text = Wstring_FromInt(year);
		Set_Item_Text(dlg, 0x630, text.get());
	} else {
		Set_Item_Text(dlg, 0x630, DAT_00889F64);
	}
	SendDlgItemMessage(dlg, 0x626, 0x4B4, 0, (LPARAM)email);
	SendDlgItemMessage(dlg, 0x631, 0xF1, newsletter, 0);
	ShowWindow(win, SW_HIDE);
	ShowWindow(dlg, SW_SHOWNORMAL);
	if (FUN_007759E0(dlg, (bool (*)(void))FUN_0077D710, 0, 1, 1) == 1) {
	email[0] = 0;
	strcpy(email, DAT_00B76E34.get());
	month = DAT_00B73A1C + 1;
	day = DAT_00B73AB8;
	year = DAT_00B7782C;
	newsletter = DAT_00B76228;
	if (month <= 0) {
		goto invalid_month;
	}
	if (day <= 0 || year <= 0 || strlen(email) < 6) {
		goto date_chain;
	}
	g_UserConsent = 0;
	g_UserAge = -1;
	if (g_pNetUtil->RequestAgeCheck(month, day, year, email) != 0) {
		ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ABB), 0, FUN_0077D710, 0);
		goto pick_dialog;
	}
	Show_Wait_Window(0x100, StringTable::LoadString("WOL:RequestingNickname", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ABE), 1);
	if (g_UserAge < 0) {
		if (g_AgeCheckResult == 0x80040100) {
			ODMessageBox(StringTable::LoadString("TXT_REQUIRED_FIELD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AC2), 0, FUN_0077D710, 0);
		} else {
			ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AC4), 0, FUN_0077D710, 0);
		}
		goto first_nick_retry;
	}
	if (g_UserAge < 13 && g_UserConsent == 0) {
		if (ODMessageBox(StringTable::LoadString("TXT_CONSENT_REQUIRED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ACD), 4, FUN_0077D710, 0) == IDYES) {
			char url[256];
			sprintf(url, "http://apiregister.westwood.com/consent_form/index_%d.html", g_LanguageCode);
			ViewHTML(url, 1);
		}
		goto first_nick_retry;
	}
pick_dialog:
	FUN_0072C420();
	if (g_UserAge < 13) {
		dialog_id = 0xE6;
	} else {
		dialog_id = (g_UserAge >= 18) + 0xF3;
	}
	dlg = WS_Create_Dialog(ProgramInstance, dialog_id, MainWindow, WOL_New_Nick_Dialog_Proc, FALSE);
	UI::RegisterWindow(dlg, dialog_id);
	if (DAT_00B77794.length() > 0) {
		Set_Item_Text(dlg, 0x628, DAT_00B77794.get());
	} else {
		Set_Item_Text(dlg, 0x628, DAT_00889F64);
	}
	if (DAT_00B765EC.length() > 0) {
		Set_Item_Text(dlg, 0x5E5, DAT_00B765EC.get());
	} else {
		Set_Item_Text(dlg, 0x5E5, DAT_00889F64);
	}
	if (DAT_00B778F4.length() > 0) {
		Set_Item_Text(dlg, 0x72C, DAT_00B778F4.get());
	} else {
		Set_Item_Text(dlg, 0x72C, DAT_00889F64);
	}
	if (DAT_00B778FC.length() > 0) {
		Set_Item_Text(dlg, 0x627, DAT_00B778FC.get());
	} else {
		Set_Item_Text(dlg, 0x627, DAT_00889F64);
	}
	ShowWindow(dlg, SW_SHOWNORMAL);
	SendMessage(dlg, 0x4A9, 0, 1);
	if (FUN_007759E0(dlg, (bool (*)(void))FUN_0077D710, 0, 1, 1) != 1) {
		goto first_nick_retry;
	}
	nick[0] = 0;
	strcpy(nick, DAT_00B77794.get());
	pass[0] = 0;
	strcpy(pass, DAT_00B778F4.get());
	verify[0] = 0;
	strcpy(verify, DAT_00B778FC.get());
	parent_email[0] = 0;
	if (dialog_id == 0xF3) {
		strcpy(parent_email, DAT_00B765EC.get());
	}
	shareinfo = 0;
	if (dialog_id == 0xF4) {
		shareinfo = DAT_00B76C20;
	}
	if (strcmp(pass, verify) != 0) {
		ODMessageBox(StringTable::LoadString("TXT_PASSWORD_VERIFY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2B41), 0, FUN_0077D710, 0);
		goto pick_dialog;
	}
	if (strlen(pass) != 8) {
		ODMessageBox(StringTable::LoadString("TXT_PASSWORD_TOO_SHORT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2B46), 0, FUN_0077D710, 0);
		goto pick_dialog;
	}
	if (g_pNetUtil->RequestNewNick(nick, pass, email, parent_email, newsletter, shareinfo) != 0) {
		goto cant_connect;
	}
	Show_Wait_Window(0x100, StringTable::LoadString("TXT_REQ_NICK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2B4D), 1);
	if (g_OnNewNickResult >= 0) {
		goto done;
	}
	goto pick_dialog;

date_chain:
	if (month < 12) {
		if (day > 0 && day <= 31) {
			if (year <= 0) {
				ODMessageBox(StringTable::LoadString("TXT_INVALID_YEAR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AAE), 0, FUN_0077D710, 0);
				goto first_nick_retry;
			}
			if (strlen(email) < 6) {
				ODMessageBox(StringTable::LoadString("TXT_INVALID_EMAIL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AB0), 0, FUN_0077D710, 0);
			} else {
				ODMessageBox(StringTable::LoadString("TXT_REQUIRED_FIELD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AB2), 0, FUN_0077D710, 0);
			}
		} else {
			ODMessageBox(StringTable::LoadString("TXT_INVALID_DAY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AAC), 0, FUN_0077D710, 0);
		}
		goto first_nick_retry;
	}
invalid_month:
	ODMessageBox(StringTable::LoadString("TXT_INVALID_MONTH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AAA), 0, FUN_0077D710, 0);
	goto first_nick_retry;
cant_connect:
	ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2B4B), 0, FUN_0077D710, 0);
done:
	ShowWindow(win, SW_SHOW);
	OwnerDraw::SetUserData2(win, 0);
	FUN_0072C470();
	} else {
		ShowWindow(win, SW_SHOWNORMAL);
		ShowWindow(win, SW_SHOW);
		OwnerDraw::SetUserData2(win, 0);
		FUN_0072C290();
	}
}

// FUNCTION: GAMEMD 0x00789500
int __fastcall StoreLogin(char const * login, char const * password, bool plain, int locale)
{
	char const * nick;
	char const * pass;
	int nicknum = 0;
	int i;
	for (i = 1; i <= 32; i++) {
		nick = NULL;
		pass = NULL;
		if (g_pChat->GetNick(i, &nick, &pass) == 0 && nick != NULL && strcmp(nick, login) == 0) {
			nicknum = i;
			break;
		}
	}
	if (nicknum == 0) {
		for (i = 1; i <= 32; i++) {
			nick = NULL;
			pass = NULL;
			if (g_pChat->GetNick(i, &nick, &pass) != 0 || nick == NULL || strlen(nick) == 0) {
				nicknum = i;
				break;
			}
		}
	}
	if (nicknum == 0) {
		int slot = Session.LastNicknameSlot;
		if (Session.LastNicknameSlot == -1) {
			slot = 1;
		}
		nicknum = slot++;
		Session.LastNicknameSlot = slot;
		if (slot > 32) {
			Session.LastNicknameSlot = 1;
		}
	}
	if (nicknum != 0) {
		g_pChat->SetNick(nicknum, login, password, plain);
		g_pChat->SetNickLocale(nicknum, (Locale)locale);
	}
	return nicknum;
}

// FUNCTION: GAMEMD 0x00789630
Wstring __fastcall FUN_00789630(Server * server)
{
	Wstring name((char *)server->name);
	Wstring head;
	int pos = name.getToken(0, ":", head);
	if (pos >= 0) {
		name.remove(0, pos + 1);
	}
	return name;
}


void __fastcall FUN_00623840(HWND hwnd);

static inline bool Same_Name(Wstring const & a, Wstring const & b)
{
	return a.toLower() == b.toLower();
}

// FUNCTION: GAMEMD 0x007896A0
bool __fastcall FUN_007896A0(HWND win, bool combo)
{
	if (combo) {
		FUN_00623840(win);
	} else {
		FUN_00623E00(win);
	}
	if (g_Servers.length() == 1) {
		if (combo) {
			SendMessage(win, 0x4BC, 0, (LPARAM)"GUI:NoServersAvailable");
		} else {
			SendMessage(win, 0x4C8, 0, (LPARAM)"GUI:NoServersAvailable");
		}
		EnableWindow(win, FALSE);
		return false;
	}
	Wstring preferred;
	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) {
		profile = FUN_00778A20(&g_NickName);
	} else {
		profile = DAT_00B75E48;
	}
	if (profile.Get()) {
		preferred = profile->Name;
	}
	EnableWindow(win, TRUE);
	int selected = -1;
	for (int i = 1; i < g_Servers.length(); i++) {
		Server * server = NULL;
		g_Servers.getPointer(&server, i);
		FUN_004068E0("Server found %s\n", server->name);
		Wstring entry = FUN_00789630(server);
		int index;
		if (combo) {
			index = SendMessage(win, 0x4BC, 0, (LPARAM)entry.text());
			SendMessage(win, 0x151, index, i);
		} else {
			index = SendMessage(win, 0x4C8, 0, (LPARAM)entry.text());
			SendMessage(win, 0x19A, index, i);
		}
		if (preferred.length() > 0) {
			if (Same_Name(preferred, entry)) {
				selected = index;
				g_CurrentServerIndex = i;
				g_TargetServerIndex = i;
			}
		}
	}
	if (selected == -1) {
		selected = 0;
	}
	if (combo) {
		SendMessage(win, 0x14E, selected, 0);
	} else {
		SendMessage(win, 0x186, selected, 0);
		SendMessage(win, 0x197, selected, 0);
	}
	return true;
}

class WonlineStringDialogControl : public SimpleWonlineDialogControl
{
	public:
		WonlineStringDialogControl(void);
		virtual ~WonlineStringDialogControl(void) {}

		Wstring Text;
};

// FUNCTION: GAMEMD 0x00789960
bool __fastcall FUN_00789960(Channel * chan, bool extra)
{
	if (!FUN_0079BA60(chan, true, extra)) {
		return false;
	}
	unsigned int mask = chan->reserved;
	int limit;
	if (mask & 0x400) {
		limit = 800;
	} else if (mask & 0x200) {
		limit = 640;
	} else {
		limit = 1024;
	}
	if (Options.ScreenWidth > limit) {
		{
			const wchar_t * text;
			if (mask & 0x400) {
				text = StringTable::LoadString("WOL:ResolutionLimited800", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2CE3);
			} else {
				text = StringTable::LoadString("WOL:ResolutionLimited640", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2CE4);
			}
			WideWstring message(text);
			if (ODMessageBox(message.get(), 4, FUN_0077D710, 1) == IDYES) {
				Options.ScreenWidth = (limit == 800) ? 800 : 640;
				Options.ScreenHeight = (Options.ScreenWidth == 800) ? 600 : 480;
				Options.Save_Settings();
			}
		}
		if (Options.ScreenWidth > limit) {
			return false;
		}
	}
	Wstring name;
	if (chan->flags & 0x100) {
		FUN_0052FEC0(NULL, true);
		WonlineStringDialogControl dialog;
		if (Run_Dialog_Control(0xE1, WOL_Password_Dialog_Proc, &dialog) == IDCANCEL) {
			return false;
		}
		name = dialog.Text;
		name.replace(" ", "_");
	}
	return FUN_00788800(chan, name, WideWstring(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2D1B))) == 0;
}

extern char DAT_00B76E28[];


class CreateGameDialogControl : public WonlineStringDialogControl
{
	public:
		CreateGameDialogControl(bool limit_resolution, bool observer)
			: Flag10(false), Flag11(false), Flag12(limit_resolution), Flag13(observer) {}
		virtual ~CreateGameDialogControl(void) {}

		bool Flag10;
		bool Flag11;
		bool Flag12;
		bool Flag13;
};

BOOL CALLBACK WOL_New_Game_Dialog_Proc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WOL_Game_Options_Proc(HWND, UINT, WPARAM, LPARAM);
MultiplayerGameMode * First_Game_Mode(void);
void __fastcall FUN_005C6120(DynamicVectorClass<NodeNameType *> * vec);
void __fastcall FUN_005E8DA0(int slot);
void FUN_0072CF40(void);
extern int g_MaxPlayers;
extern int DAT_008316A4;

// FUNCTION: GAMEMD 0x0078A470
int __fastcall FUN_0078A470(wchar_t * input)
{
	wchar_t name[16];
	wchar_t prefix[64];
	User user;
	wchar_t message[256];
	int result = 0;

	wcscpy(prefix, StringTable::LoadString("TXT_ONLINE_COMMAND", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E58));
	if (wcsncmp(input, prefix, wcslen(prefix)) == 0) {
		int length = 0;
		wchar_t * cursor = input + wcslen(prefix);
		while (wcsncmp(cursor, L" ", 1) == 0) {
			cursor++;
		}
		while (*cursor != 0 && wcsncmp(cursor, L" ", 1) != 0 && length < 10) {
			name[length++] = *cursor++;
		}
		name[length] = 0;
		if (wcslen(name) > 0) {
			strcpy((char *)user.name, FUN_007350C0(name));
			strcpy(DAT_00B76E28, FUN_007350C0(name));
			g_pChat->RequestFind(&user);
			result = 1;
		} else {
			swprintf(message, StringTable::LoadString("TXT_NO_NAME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E6C));
			HWND top = FUN_00775B10();
			if (top) {
				if (GetDlgItem(top, 0x53F)) {
					PMessagePrintf(ColorSystem, message);
				}
				if (GetDlgItem(top, 0x5B5)) {
					SMessagePrintf(ColorSystem, message);
				}
			}
			if (FUN_0069BBE0(&Session)) {
				int scheme = GadgetClass::Get_Color_Scheme();
				Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
			}
			result = 1;
		}
	}
	return result;
}

// FUNCTION: GAMEMD 0x0078A6B0
int __fastcall FUN_0078A6B0(wchar_t * input)
{
	struct { wchar_t prefix[64]; wchar_t message[64]; } buf;
	User user;
	wchar_t name[16];
	int result = 0;

	wcscpy(buf.prefix, StringTable::LoadString("TXT_PAGE_COMMAND", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E8E));
	if (wcsnicmp(input, buf.prefix, wcslen(buf.prefix)) == 0) {
		int length = 0;
		int size = wcslen(input) + 1;
		wchar_t * copy = (wchar_t *)operator new(size * 2);
		if (!copy) {
			return 0;
		}
		copy = wcsncpy(copy, input, size);
		if (!copy) {
			return 0;
		}
		wchar_t * cursor = copy + wcslen(buf.prefix);
		while (*cursor && wcsncmp(cursor, L" ", 1) == 0) {
			cursor++;
		}
		while (*cursor && wcsncmp(cursor, L" ", 1) != 0 && length <= 10) {
			name[length++] = *cursor++;
		}
		name[length] = 0;
		while (*cursor && wcsncmp(cursor, L" ", 1) == 0) {
			cursor++;
		}
		if (length > 10) {
			result = 1;
		} else if (length == 0) {
			result = 1;
		} else {
			bool muted = false;
			if (Unsorted::MuteSWLaunches != 0) {
				for (int i = 0; i < Session.Players.Count(); i++) {
					if (wcsicmp(Session.Players[i]->Name, name) == 0) {
						muted = true;
						break;
					}
				}
			}
			if (!muted) {
				if (wcslen(cursor) > 0) {
					strcpy((char *)user.name, FUN_007350C0(name));
					g_pChat->RequestUnicodePage(&user, cursor);
				}
				result = 1;
			} else {
				int scheme = GadgetClass::Get_Color_Scheme();
				Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_OBSERVER_CANT_PAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ECA), scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
				result = 2;
			}
		}
		delete copy;
	}
	return result;
}

// FUNCTION: GAMEMD 0x0078A8F0
int __fastcall FUN_0078A8F0(wchar_t * input)
{
	wchar_t message[128];
	wchar_t shortcmd[128];
	User user;
	wchar_t cmd[128];
	int result = 0;

	wcscpy(cmd, StringTable::LoadString("TXT_REPLY_COMMAND", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2EDE));
	wcscpy(shortcmd, StringTable::LoadString("TXT_REPLY_COMMAND_SHORT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2EDF));
	if (wcsnicmp(input, cmd, wcslen(cmd)) == 0 || wcsncmp(input, shortcmd, wcslen(shortcmd)) == 0) {
		int size = wcslen(input) + 1;
		wchar_t * copy = (wchar_t *)operator new(size * 2);
		if (!copy) {
			return 0;
		}
		copy = wcsncpy(copy, input, size);
		if (!copy) {
			return 0;
		}
		wchar_t * cursor;
		if (wcsncmp(input, cmd, wcslen(cmd)) == 0) {
			cursor = copy + wcslen(cmd);
		} else {
			cursor = copy + wcslen(shortcmd);
		}
		while (*cursor && wcsncmp(cursor, L" ", 1) == 0) {
			cursor++;
		}
		bool muted = false;
		if (Unsorted::MuteSWLaunches != 0) {
			for (int i = 0; i < Session.Players.Count(); i++) {
				if (wcsicmp(Session.Players[i]->Name, FUN_00735120(DAT_00B77E30)) == 0) {
					muted = true;
					break;
				}
			}
		}
		if (!muted) {
			if (strlen(DAT_00B77E30) != 0) {
				if (wcslen(cursor) > 0) {
					strcpy((char *)user.name, DAT_00B77E30);
					g_pChat->RequestUnicodePage(&user, cursor);
				}
				result = 1;
			} else {
				wcscpy(message, StringTable::LoadString("TXT_NO_ONE_WHOM_TO_REPLY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2F12));
				HWND top = FUN_00775B10();
				if (top) {
					if (GetDlgItem(top, 0x53F)) {
						FUN_005E2700(DAT_007EEF28, message);
					}
					if (GetDlgItem(top, 0x5B5)) {
						FUN_005E2790(DAT_007EEF28, message);
					}
				}
				if (FUN_0069BBE0(&Session)) {
					int scheme = GadgetClass::Get_Color_Scheme();
					Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
				}
				result = 2;
			}
		} else {
			int scheme = GadgetClass::Get_Color_Scheme();
			Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_OBSERVER_CANT_PAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2F2A), scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
			result = 2;
		}
		delete copy;
	}
	return result;
}

extern bool DAT_00B77E40;

// FUNCTION: GAMEMD 0x0078ABF0
void __fastcall FUN_0078ABF0(bool flag, wchar_t const * text)
{
	DAT_00B77E40 = flag;
	DAT_00B76230.set(text);
}

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
}
namespace OwnerDraw {
	void DrawItem(LPDRAWITEMSTRUCT item);
	void Paint(HWND window);
}
void __fastcall FUN_00607FD0(HWND window);


void __fastcall FUN_00623E40(HWND window, Wstring & text);
int __fastcall FUN_00623EA0(HWND window, WideWstring * text);
WideWstring __fastcall FUN_005EDD00(HWND edit);
bool __fastcall Focus_Next_Tab_Item(HWND window, bool previous);
extern int DAT_00B77E8C;
extern int DAT_007EEF44;
extern wchar_t DAT_0084B94C[];
extern char DAT_0082BFAC[];

extern int DAT_007EEF30;
extern int DAT_007EEF34;
extern int DAT_007EEF38;
extern char DAT_00B77E78[];
extern int DAT_0084A23C;
extern int DAT_0084A240;
void __fastcall FUN_00608440(HWND window);
void FUN_0072BAC0(void);
void FUN_0072BB10(void);
bool FUN_00789B60(void);
BOOL CALLBACK WOL_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

#include "tiprequest.h"

// FUNCTION: GAMEMD 0x0078AC10
BOOL CALLBACK WOL_Main_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_TIMER:
			switch (wparam) {
				case 0x55D:
					KillTimer(window, 0x55D);
					EnableWindow(GetDlgItem(window, 0x55D), TRUE);
					return FALSE;

				case 1:
					if (FUN_00775B10() == window && !FUN_0077F520(false)) {
						SetEvent(g_WaitEventHandles[5]);
					}
					break;

				case 0:
					if (FUN_00775B10() == window && !FUN_0077F710(1, false)) {
						SetEvent(g_WaitEventHandles[5]);
					}
					break;
			}
			break;

		case 0x593:
			EnableWindow(GetDlgItem(window, 0x53D), FALSE);
			if (!FUN_0077F520(true) || !FUN_0077F710(1, false)) {
				SetEvent(g_WaitEventHandles[5]);
			}
			EnableWindow(GetDlgItem(window, 0x53D), TRUE);
			break;

		case WM_DESTROY:
			KillTimer(window, 0);
			KillTimer(window, 0x55D);
			FUN_005EE1B0();
			return FALSE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_SHOWWINDOW:
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x53D: {
					int index = SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
					if (index == -1) {
						return FALSE;
					}
					int extra = SendMessage((HWND)lparam, LB_GETITEMDATA, index, 0);
					if (HIWORD(wparam) == LBN_SELCHANGE && extra != -1) {
						Channel chan;
						if (g_UserChannels.get(chan, extra)) {
							if (strcmp((char *)chan.name, (char *)g_CurrentChannel.name) != 0 && strcmp((char *)chan.name, (char *)s_TempChannel.name) != 0) {
								g_JoinLobbyNow = 1;
								g_CurrentLobby = atol((char *)chan.name + strlen("Lob_41_"));
								SetEvent(g_WaitEventHandles[9]);
								return FALSE;
							}
						}
					}
					return FALSE;
				}

				case 0x53E:
					return FALSE;

				case 0x3ED: {
					HWND input = GetDlgItem(window, 0x540);
					WideWstring text = FUN_005EDD00(input);
					SendMessage(input, 0x4B2, 0, (LPARAM)DAT_00887734);
					SetFocus(input);
					InvalidateRect(input, NULL, FALSE);
					int color = DAT_007EEF38;
					if (strlen((char *)g_CurrentChannel.name) == 0) {
						PMessagePrintf(-1, StringTable::LoadString("TXT_NOT_IN_CHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x324A));
					} else if (text.length() > 0) {
						if (Send_Chat_Action(text)) {
							color = DAT_007EEF34;
						}
						wchar_t * message_text = text.get();
						PMessagePrintf(color, L"%hs %s", g_NickName.c_str(), message_text);
					} else {
						PMessagePrintf(-1, StringTable::LoadString("TXT_ENTER_MESSAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3252));
					}
					return FALSE;
				}

				case 0x540: {
					wchar_t nickname[16];
					wchar_t primary_command[64];
					wchar_t alternate_command[64];
					HWND input = GetDlgItem(window, 0x540);
					WideWstring text = FUN_005EDD00(input);
					if (HIWORD(wparam) == 0x501) {
						SendMessage(input, 0x4B2, 0, (LPARAM)DAT_00887734);
						if (text.length() > 0) {
							int parsed = FUN_0078A6B0(text.get());
							if (parsed != 0) {
								if (parsed == 1) {
									int count = 0;
									unsigned length = text.length() + 1;
									wchar_t * copy = (wchar_t *)operator new(length * 2);
									wcscpy(primary_command, StringTable::LoadString("TXT_PAGE_COMMAND", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x31C3));
									if (!copy) {
										return FALSE;
									}
									copy = wcsncpy(copy, text.get(), length);
									if (!copy) {
										return FALSE;
									}
									wchar_t * body = copy + wcslen(primary_command);
									while (*body && wcsncmp(body, L" ", 1) == 0) {
										++body;
									}
									while (*body) {
										if (wcsncmp(body, L" ", 1) == 0 || count > 10) {
											break;
										}
										nickname[count++] = *body++;
									}
									nickname[count] = 0;
									while (*body && wcsncmp(body, L" ", 1) == 0) {
										++body;
									}
									if (count > 10) {
										PMessagePrintf(-1, StringTable::LoadString("TXT_NAME_TOO_LONG", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x31E1));
									} else if (count == 0) {
										PMessagePrintf(-1, StringTable::LoadString("TXT_NO_NAME_SPECIFIED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x31E3));
									} else if (wcslen(body) > 0) {
										PMessagePrintf(DAT_007EEF30, L"[%hs > %s] %s", g_NickName.c_str(), nickname, body);
									} else {
										PMessagePrintf(-1, StringTable::LoadString("TXT_CANT_SEND_BLANK_PAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x31E9));
									}
									operator delete(copy);
								}
							} else if (FUN_0078A8F0(text.get())) {
								unsigned length = text.length() + 1;
								wchar_t * copy = (wchar_t *)operator new(length * 2);
								wcscpy(primary_command, StringTable::LoadString("TXT_REPLY_COMMAND", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x31FB));
								wcscpy(alternate_command, StringTable::LoadString("TXT_REPLY_COMMAND_SHORT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x31FC));
								if (!copy) {
									return FALSE;
								}
								copy = wcsncpy(copy, text.get(), length);
								if (!copy) {
									return FALSE;
								}
								wchar_t * body = copy + (wcsncmp(copy, primary_command, wcslen(primary_command)) == 0
														  ? wcslen(primary_command)
														  : wcslen(alternate_command));
								while (*body && wcsncmp(body, L" ", 1) == 0) {
									++body;
								}
								wcscpy(nickname, FUN_00735120(DAT_00B77E30));
								if (wcslen(nickname) > 0 && wcslen(body) > 0) {
									PMessagePrintf(DAT_007EEF30, L"[%hs > %s] %s", g_NickName.c_str(), nickname, body);
								} else if (wcslen(nickname) > 0) {
									PMessagePrintf(-1, StringTable::LoadString("TXT_CANT_SEND_BLANK_PAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3219));
								}
								operator delete(copy);
							} else if (FUN_0077D350(text.get())) {
								DAT_00B76AF0 = !DAT_00B76AF0;
								if (DAT_00B76AF0) {
									PMessagePrintf(DAT_007EEF38, L"%s", StringTable::LoadString("TXT_BUDDY_FILTER_ON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3223));
								} else {
									PMessagePrintf(DAT_007EEF38, L"%s", StringTable::LoadString("TXT_BUDDY_FITER_OFF", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3225));
								}
							} else if (FUN_0077D3A0(text.get())) {
								DAT_00B73AD0 = !DAT_00B73AD0;
								if (DAT_00B73AD0) {
									PMessagePrintf(DAT_007EEF38, L"%s", StringTable::LoadString("TXT_CLAN_FILTER_ON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x322A));
								} else {
									PMessagePrintf(DAT_007EEF38, L"%s", StringTable::LoadString("TXT_CLAN_FILTER_OFF", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x322C));
								}
							} else if (!FUN_0078A470(text.get())) {
								if (strlen((char *)g_CurrentChannel.name) == 0) {
									PMessagePrintf(-1, StringTable::LoadString("TXT_NOT_IN_CHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3230));
								} else {
									int color = DAT_007EEF44;
									if (Send_Chat_Message(text) == 1) {
										color = DAT_007EEF30;
									}
									wchar_t * message_text = text.get();
									PMessagePrintf(color, L"[%hs] %s", g_NickName.c_str(), message_text);
								}
							}
						}
					}
					return FALSE;
				}

				case 0x55D:
					EnableWindow((HWND)lparam, FALSE);
					SetTimer(window, 0x55D, 3000, NULL);
					if (!FUN_0077F320(0x29, 1, false)) {
						SetEvent(g_WaitEventHandles[5]);
					}
					break;

				case 0x547:
				case 0x5C0:
				case 0x686:
					FUN_00608440(window);
					SetEvent(g_WaitEventHandles[6]);
					return FALSE;

				case 0x61F: {
					OwnerDraw::DrawCampaignMenuTransition(window, false);
					FUN_0052FEC0(NULL, true);
					FUN_0072BAC0();
					HWND dialog = UI::BeginDialog(0x2BC, (DLGPROC)WOL_Options_Dialog_Proc, 0);
					ShowWindow(dialog, SW_SHOWNORMAL);
					int result = FUN_007759E0(dialog, (bool (*)(void))FUN_0077D710, 0, 1, 1);
					InvalidateRect(window, NULL, FALSE);
					ShowWindow(window, SW_SHOW);
					OwnerDraw::SetUserData2(window, 0);
					FUN_0072BB10();
					if (result == 2) {
						Apply_WOL_Settings();
					} else {
						g_AllowPage = DAT_00B76474;
						g_AllowFind = DAT_00B766B8;
						g_LangFilter = DAT_00B76958;
						g_LobbyMusic = DAT_00B76910;
						g_ShowAllChannels = DAT_00B77798;
						DAT_00B779DC = DAT_00B768F8;
						DAT_00B779C0 = DAT_00B76AC0;
						DAT_00B76AF0 = DAT_00B73680 != 0;
						DAT_00B73AD0 = DAT_00B77790 != 0;
						Write_WOL_Settings();
						Apply_WOL_Settings();
					}
					SetFocus(GetDlgItem(window, 0x540));
					break;
				}

				case 0x5C2:
					DoFindPage();
					return FALSE;

				case 0x62D: {
					HWND list = GetDlgItem(window, 0x6EE);
					int index = SendMessage(list, LB_GETCURSEL, 0, 0);
					bool joined = false;
					int extra = -1;
					if (index != -1) {
						extra = SendMessage(list, LB_GETITEMDATA, index, 0);
						if (extra >= 0 && extra < g_GameChannelList.length()) {
							Channel chan;
							memset(&chan, 0, sizeof(chan));
							g_GameChannelList.get(chan, extra);
							joined = FUN_0079BA60(&chan, extra != DAT_0084A23C, Game::ObserverMode != 0);
							if (joined) {
								EnableWindow(GetDlgItem(window, 0x62B), FALSE);
								EnableWindow(GetDlgItem(window, 0x62D), FALSE);
								SendMessage(GetDlgItem(window, 0x6EE), LB_SETCURSEL, -1, 0);
								EnableWindow(GetDlgItem(window, 0x689), FALSE);
								EnableWindow(GetDlgItem(window, 0x688), FALSE);
								EnableWindow(GetDlgItem(window, 0x7A3), FALSE);
								EnableWindow(GetDlgItem(window, 0x7A6), FALSE);
								EnableWindow(GetDlgItem(window, 0x61F), FALSE);
								EnableWindow(GetDlgItem(window, 0x5C2), FALSE);
								EnableWindow(GetDlgItem(window, 0x686), FALSE);
								bool started = FUN_00789960(&chan, Game::ObserverMode == 1);
								EnableWindow(GetDlgItem(window, 0x62B), TRUE);
								EnableWindow(GetDlgItem(window, 0x689), TRUE);
								EnableWindow(GetDlgItem(window, 0x688), TRUE);
								EnableWindow(GetDlgItem(window, 0x7A3), TRUE);
								EnableWindow(GetDlgItem(window, 0x7A6), TRUE);
								EnableWindow(GetDlgItem(window, 0x61F), TRUE);
								EnableWindow(GetDlgItem(window, 0x5C2), TRUE);
								EnableWindow(GetDlgItem(window, 0x686), TRUE);
								if (started) {
									OwnerDraw::DrawCampaignMenuTransition(window, false);
									FUN_0052FEC0(NULL, true);
									ShowWindow(window, SW_HIDE);
									OwnerDraw::SetUserData2(window, 1);
								}
							}
						}
					}
					DAT_0084A23C = extra;
					EnableWindow((HWND)lparam, joined);
					SetFocus(GetDlgItem(window, 0x540));
					return FALSE;
				}

				case 0x62B: {
					EnableWindow((HWND)lparam, FALSE);
					EnableWindow(GetDlgItem(window, 0x62D), FALSE);
					SendMessage(GetDlgItem(window, 0x6EE), LB_SETCURSEL, -1, 0);
					EnableWindow(GetDlgItem(window, 0x689), FALSE);
					EnableWindow(GetDlgItem(window, 0x688), FALSE);
					EnableWindow(GetDlgItem(window, 0x7A3), FALSE);
					EnableWindow(GetDlgItem(window, 0x7A6), FALSE);
					EnableWindow(GetDlgItem(window, 0x61F), FALSE);
					EnableWindow(GetDlgItem(window, 0x5C2), FALSE);
					EnableWindow(GetDlgItem(window, 0x686), FALSE);
					bool created = FUN_00789B60();
					EnableWindow((HWND)lparam, TRUE);
					EnableWindow(GetDlgItem(window, 0x689), TRUE);
					EnableWindow(GetDlgItem(window, 0x688), TRUE);
					EnableWindow(GetDlgItem(window, 0x7A3), TRUE);
					EnableWindow(GetDlgItem(window, 0x7A6), TRUE);
					EnableWindow(GetDlgItem(window, 0x61F), TRUE);
					EnableWindow(GetDlgItem(window, 0x5C2), TRUE);
					EnableWindow(GetDlgItem(window, 0x686), TRUE);
					if (!created) {
						SetFocus(GetDlgItem(window, 0x540));
					}
					return FALSE;
				}

				case 0x608: {
					WideWstring text;
					HWND input = GetDlgItem(window, 0x540);
					FUN_00623EA0(input, &text);
					SendMessage(input, 0x4B2, 0, (LPARAM)DAT_00887734);
					SetFocus(input);
					if (g_OwnSquadID) {
						User user;
						memset(&user, 0, sizeof(user));
						strcpy((char *)user.name, DAT_0082BFAC);
						g_pChat->RequestPrivateUnicodeMessage(&user, text.get());
					} else {
						PMessagePrintf(-1, StringTable::LoadString("TXT_NOSQUAD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x326C));
					}
					return FALSE;
				}

				case 0x688:
					FUN_0077D8C0();
					SetFocus(GetDlgItem(window, 0x540));
					return FALSE;

				case 0x689:
					Session.WOLWelcomeChoice = 5;
					Session.unknown_10 = 4;
					FUN_00608440(window);
					SetEvent(g_WaitEventHandles[6]);
					return FALSE;

				case 0x765:
					DAT_00B77E44 = (DAT_00B77E44 == 0);
					FUN_00781480();
					return FALSE;

				case 0x766:
					DAT_00B77E44 = (DAT_00B77E44 == 14) ? 15 : 14;
					FUN_00781480();
					return FALSE;

				case 0x767:
					if (DAT_00B77E44 == 6) {
						DAT_00B77E44 = 7;
					} else if (DAT_00B77E44 == 7) {
						DAT_00B77E44 = 4;
					} else if (DAT_00B77E44 == 4) {
						DAT_00B77E44 = 5;
					} else {
						DAT_00B77E44 = 6;
					}
					FUN_00781480();
					return FALSE;

				case 0x6EE: {
					bool dblclk = false;
					switch (HIWORD(wparam)) {
						case LBN_SELCHANGE:
							break;

						case LBN_DBLCLK:
							dblclk = true;
							break;

						default:
							return FALSE;
					}
					int index = SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
					int extra = -1;
					bool joinable = false;
					if (index != -1) {
						extra = SendMessage((HWND)lparam, LB_GETITEMDATA, index, 0);
						if (extra >= 0 && extra < g_GameChannelList.length()) {
							Channel chan;
							memset(&chan, 0, sizeof(chan));
							g_GameChannelList.get(chan, extra);
							if (DAT_0084A240 == -1) {
								DAT_00B77E78[0] = 0;
							}
							int changed = strcmp(DAT_00B77E78, (char *)chan.name);
							joinable = FUN_0079BA60(&chan, dblclk || changed != 0, Game::ObserverMode != 0);
							strcpy(DAT_00B77E78, (char *)chan.name);
							if (joinable && dblclk) {
								EnableWindow(GetDlgItem(window, 0x62B), FALSE);
								EnableWindow(GetDlgItem(window, 0x62D), FALSE);
								SendMessage(GetDlgItem(window, 0x6EE), LB_SETCURSEL, -1, 0);
								EnableWindow(GetDlgItem(window, 0x689), FALSE);
								EnableWindow(GetDlgItem(window, 0x688), FALSE);
								EnableWindow(GetDlgItem(window, 0x7A3), FALSE);
								EnableWindow(GetDlgItem(window, 0x7A6), FALSE);
								EnableWindow(GetDlgItem(window, 0x61F), FALSE);
								EnableWindow(GetDlgItem(window, 0x5C2), FALSE);
								EnableWindow(GetDlgItem(window, 0x686), FALSE);
								if (FUN_00789960(&chan, Game::ObserverMode == 1)) {
									OwnerDraw::DrawCampaignMenuTransition(window, false);
									FUN_0052FEC0(NULL, true);
									ShowWindow(window, SW_HIDE);
									OwnerDraw::SetUserData2(window, 1);
								}
								EnableWindow(GetDlgItem(window, 0x62B), TRUE);
								EnableWindow(GetDlgItem(window, 0x689), TRUE);
								EnableWindow(GetDlgItem(window, 0x688), TRUE);
								EnableWindow(GetDlgItem(window, 0x7A3), TRUE);
								EnableWindow(GetDlgItem(window, 0x7A6), TRUE);
								EnableWindow(GetDlgItem(window, 0x61F), TRUE);
								EnableWindow(GetDlgItem(window, 0x5C2), TRUE);
								EnableWindow(GetDlgItem(window, 0x686), TRUE);
							}
						} else {
							DAT_00B77E78[0] = 0;
						}
					}
					int selected = SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0);
					if (!DAT_00B77E74 || selected != -1) {
						DAT_0084A240 = extra;
					}
					if (DAT_00B77E74 && selected == -1) {
						DAT_00B77E74 = 0;
					}
					HWND join_button = GetDlgItem(window, 0x62D);
					EnableWindow(join_button, joinable);
					return FALSE;
				}

				case 0x75F:
				case 0x760: {
					int checked = 0;
					HWND item = GetDlgItem(window, 0x760);
					if (item) {
						checked = SendMessage(item, BM_GETCHECK, 0, 0);
					}
					if (LOWORD(wparam) == 0x75F) {
						checked = !checked;
					}
					Game::ObserverMode = checked;
					FUN_005EE350(NULL);
					return FALSE;
				}

				case 0x768:
					DAT_00B77E44 = (DAT_00B77E44 == 2) ? 3 : 2;
					FUN_00781480();
					return FALSE;

				case 0x7A3:
					FUN_0077D8E0();
					SetFocus(GetDlgItem(window, 0x540));
					break;

				case 0x7A6:
					FUN_0077D8F0();
					SetFocus(GetDlgItem(window, 0x540));
					break;

				case 0x769:
					if (DAT_00B77E44 == 12) {
						DAT_00B77E44 = 8;
						FUN_00781480();
					} else if (DAT_00B77E44 == 13) {
						DAT_00B77E44 = 8;
						FUN_00781480();
					} else if (DAT_00B77E44 == 8) {
						DAT_00B77E44 = 10;
						FUN_00781480();
					} else if (DAT_00B77E44 == 9) {
						DAT_00B77E44 = 10;
						FUN_00781480();
					} else {
						DAT_00B77E44 = 12;
						FUN_00781480();
					}
					return FALSE;

				case 0x76A:
					if (DAT_00B77E48 == 0) {
						DAT_00B77E48 = 1;
						FUN_00781D20(false);
					} else {
						DAT_00B77E48 = 0;
						FUN_00781D20(true);
					}
					Draw_Player_List(0, true);
					return FALSE;

				case 0x76B:
					if (DAT_00B77E48 == 2) {
						DAT_00B77E48 = 3;
						FUN_00783270(false);
					} else {
						DAT_00B77E48 = 2;
						FUN_00783270(true);
					}
					Draw_Player_List(0, true);
					return FALSE;

				case 0x76C:
					if (DAT_00B77E48 == 4) {
						DAT_00B77E48 = 5;
						FUN_00783800(false);
					} else {
						DAT_00B77E48 = 4;
						FUN_00783800(true);
					}
					Draw_Player_List(0, true);
					return FALSE;

				default:
					return FALSE;
			}
			break;

		case WM_SETFOCUS:
			SetFocus(GetDlgItem(window, 0x540));
			return FALSE;

		case 0x497:
			FUN_005EE350(window);
			SendDlgItemMessage(window, 0x3ED, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wodact.pcx", NULL));
			SendDlgItemMessage(window, 0x3ED, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("wouact.pcx", NULL));
			SendDlgItemMessage(window, 0x55D, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wouref.pcx", NULL));
			SendDlgItemMessage(window, 0x55D, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("wodref.pcx", NULL));
			SendDlgItemMessage(window, 0x769, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("woluclan.pcx", NULL));
			SendDlgItemMessage(window, 0x769, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("woldclan.pcx", NULL));
			SendDlgItemMessage(window, 0x768, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wolupriv.pcx", NULL));
			SendDlgItemMessage(window, 0x768, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("woldpriv.pcx", NULL));
			SendDlgItemMessage(window, 0x767, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wolurslkob.pcx", NULL));
			SendDlgItemMessage(window, 0x767, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("woldrslkob.pcx", NULL));
			g_Col_ChanIcon = 2;
			g_Col_ChanPrivate = 19;
			DAT_00B76AF4 = 36;
			g_Col_ChanName = 53;
			g_Col_PingTime = 200;
			SendDlgItemMessage(window, 0x6EE, 0x4A6, g_Col_PingTime - g_Col_ChanName, g_Col_ChanName);
			SendDlgItemMessage(window, 0x6EE, 0x4A6, 0, g_Col_ChanIcon);
			SendDlgItemMessage(window, 0x6EE, 0x4A6, 0, g_Col_ChanPrivate);
			SendDlgItemMessage(window, 0x6EE, 0x4A6, 0, g_Col_PingTime);
			SendDlgItemMessage(window, 0x6EE, 0x4A6, 0, DAT_00B76AF4);
			DAT_00B76C1C = 2;
			SendDlgItemMessage(window, 0x53D, 0x4A6, 0, DAT_00B76C1C);
			DAT_00B73704 = 2;
			DAT_00B7619C = 18;
			DAT_00B76194 = 26;
			DAT_00B76198 = 34;
			DAT_00B76AC8 = 44;
			DAT_00B766BC = 61;
			DAT_00B779F4 = 78;
			DAT_00B76438 = 240;
			DAT_00B76604 = 170;
			SendDlgItemMessage(window, 0x53E, 0x4A6, DAT_00B76438 - DAT_00B779F4, DAT_00B779F4);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B73704);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B76438);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B7619C);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B76194);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B76198);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B76AC8);
			SendDlgItemMessage(window, 0x53E, 0x4A6, 0, DAT_00B766BC);
			SendDlgItemMessage(window, 0x53E, 0x4A6, DAT_00B76438 - DAT_00B76604, DAT_00B76604);
			FUN_007806C0();
			FUN_00781480();
			Draw_Player_List(0, false);
			SetFocus(GetDlgItem(window, 0x540));
			SetTimer(window, 0, 30000, NULL);
			SetTimer(window, 1, 53000, NULL);
			return FALSE;

		case 0x4E9: {
			TooltipRequest * request = (TooltipRequest *)lparam;
			if (request) {
				switch (GetDlgCtrlID(request->Window)) {
					case 0x6EE:
						request->Text = FUN_00780E30(request->Window, request->Index);
						break;

					case 0x53E:
						request->Text = FUN_00781090(request->Window, request->Index);
						break;
				}
			}
			return FALSE;
		}

		case 0x4ED:
			ShowWindow(window, SW_HIDE);
			OwnerDraw::SetUserData2(window, 1);
			return FALSE;

		default:
			return FALSE;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x0078D120
BOOL CALLBACK WOL_Find_Page_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	User user;
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x5C1: {
					Wstring name;
					FUN_00623E40((HWND)lparam, name);
					WideWstring text;
					FUN_00623EA0(GetDlgItem(window, 0x540), &text);
					EnableWindow(GetDlgItem(window, 0x5C2), name.length() > 0 && text.length() > 0);
					DAT_00B77E8C = 0x5C1;
					HWND item = GetDlgItem(window, 0x5C8);
					if (item) {
						SendMessage(item, 0x4B2, 0, (LPARAM)DAT_00887734);
					}
					return FALSE;
				}

				case 0x540: {
					WideWstring text = FUN_005EDD00((HWND)lparam);
					Wstring name;
					FUN_00623E40(GetDlgItem(window, 0x5C1), name);
					EnableWindow(GetDlgItem(window, 0x5C2), text.length() > 0 && name.length() > 0);
					if (DAT_00B77E8C == 0x5C1) {
						if (g_pChat) {
							User userL;
							memset(&userL, 0, sizeof(userL));
							strcpy((char *)userL.name, name.get());
							g_pChat->RequestFind(&userL);
						}
						HWND item = GetDlgItem(window, 0x5C8);
						if (item) {
							SendMessage(item, 0x4B2, 0, (LPARAM)DAT_00887734);
						}
					}
					DAT_00B77E8C = 0x540;
					return FALSE;
				}

				case IDCANCEL:
					FUN_007757E0(window, 2);
					return FALSE;

				case 0x5C2: {
					if (DAT_00B77E8C == 0x5C1) {
						if (g_pChat) {
							HWND item = GetDlgItem(window, 0x5C1);
							if (item) {
								char name[10];
								SendMessage(item, 0x4B5, 10, (LPARAM)name);
								name[9] = 0;
								memset(&user, 0, sizeof(user));
								strcpy((char *)user.name, name);
								g_pChat->RequestFind(&user);
							}
						}
						HWND item = GetDlgItem(window, 0x5C8);
						if (item) {
							SendMessage(item, 0x4B2, 0, (LPARAM)DAT_00887734);
						}
					}
					DAT_00B77E8C = 0x5C2;
					HWND input = GetDlgItem(window, 0x540);
					WideWstring text = FUN_005EDD00(input);
					SendMessage(input, 0x4B2, 0, (LPARAM)DAT_00887734);
					EnableWindow(GetDlgItem(window, 0x5C2), FALSE);
					SetFocus(input);
					if (!g_pChat) {
						return FALSE;
					}
					{
						Wstring name;
						FUN_00623E40(GetDlgItem(window, 0x5C1), name);
						name.truncate((unsigned int)9);
						User userL;
						strcpy((char *)userL.name, name.get());
						if (text.length() > 0 && strlen((char *)userL.name) != 0) {
							bool muted = false;
							if (Unsorted::MuteSWLaunches) {
								for (int i = 0; i < Session.Players.Count(); i++) {
									NodeNameType * who = Session.Players[i];
									if (stricmp(FUN_007350C0(who->Name), (char *)userL.name) == 0) {
										muted = true;
										break;
									}
								}
							}
							if (muted) {
								FUN_005E2790(-1, StringTable::LoadString("TXT_OBSERVER_CANT_PAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3746));
							} else {
								g_pChat->RequestUnicodePage(&userL, text.get());
								wchar_t * body = text.get();
								FUN_005E2790(DAT_007EEF44, DAT_0084B94C, g_NickName.c_str(), body);
							}
						} else if (strlen((char *)userL.name) == 0) {
							FUN_005E2790(-1, StringTable::LoadString("TXT_NAME_ERROR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3752));
						} else {
							FUN_005E2790(-1, StringTable::LoadString("TXT_ENTER_MESSAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3754));
						}
					}
					return FALSE;
				}

				case 0x607:
					if (g_pChat) {
						if (g_OwnSquadID == 0) {
							FUN_005E2790(-1, StringTable::LoadString("TXT_NOSQUAD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3760));
							return FALSE;
						}
						WideWstring text;
						HWND input = GetDlgItem(window, 0x540);
						FUN_00623EA0(input, &text);
						SendMessage(input, 0x4B2, 0, (LPARAM)DAT_00887734);
						SetFocus(input);
						strcpy((char *)user.name, DAT_0082BFAC);
						if (text.length() > 0) {
							g_pChat->RequestUnicodePage(&user, text.get());
						} else {
							FUN_005E2790(-1, StringTable::LoadString("TXT_ENTER_MESSAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3773));
						}
						return FALSE;
					}
					break;

				case 0x5C3:
					if (g_pChat) {
						char name[64];
						memset(name, 0, sizeof(name));
						SendDlgItemMessage(window, 0x5C1, 0x4B5, 60, (LPARAM)name);
						name[9] = 0;
						strcpy((char *)user.name, name);
						if (strlen((char *)user.name) != 0) {
							g_pChat->RequestFind(&user);
							return FALSE;
						}
						FUN_005E2790(-1, StringTable::LoadString("TXT_NAME_ERROR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3700));
						return FALSE;
					}
					break;
			}
			break;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case 0x497: {
			SendDlgItemMessage(window, 0x5C1, 0xC5, 9, 0);
			HWND item = GetDlgItem(window, 0x5C1);
			SendMessage(item, 0x4D1, 1, 0);
			item = GetDlgItem(window, 0x5C1);
			SendMessage(item, 0x4B4, 0, (LPARAM)DAT_00889F64);
			if (DAT_00B76478 > 0) {
				item = GetDlgItem(window, 0x5C1);
				SendMessage(item, 0x4B4, 0, (LPARAM)DAT_00B77E30);
				SetFocus(GetDlgItem(window, 0x540));
				DAT_00B76478 = 0;
			}
			return FALSE;
		}

		case 0x4DB:
			Focus_Next_Tab_Item((HWND)lparam, wparam != 0);
			if ((HWND)lparam == GetDlgItem(window, 0x5C1)) {
				if (g_pChat) {
					char name[10];
					SendMessage((HWND)lparam, 0x4B5, 10, (LPARAM)name);
					name[9] = 0;
					memset(&user, 0, sizeof(user));
					strcpy((char *)user.name, name);
					g_pChat->RequestFind(&user);
				}
				HWND item = GetDlgItem(window, 0x5C8);
				if (item) {
					SendMessage(item, 0x4B2, 0, (LPARAM)DAT_00887734);
				}
				DAT_00B77E8C = 0x540;
			}
			break;

		case 0x4DA:
			PostMessage(GetDlgItem(window, 0x5C2), 0xF5, 0, 0);
			break;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x0078DA30
BOOL CALLBACK WOL_Download_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDCANCEL:
					g_pDownload->Abort();
					FUN_007757E0(window, 2);
					break;

				default:
					return FALSE;
			}
			break;

		case WM_INITDIALOG:
			break;

		case 0x5F4:
			FUN_007757E0(window, 1);
			break;

		case 0x5F5:
			ODMessageBox(StringTable::LoadString("TXT_DOWNLOAD_FAILED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3959), 0, FUN_0077D710, 0);
			g_pDownload->Abort();
			FUN_007757E0(window, 2);
			break;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_ERASEBKGND:
			break;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		default:
			return FALSE;
	}
	return TRUE;
}

extern int g_MaxPlayers;



// FUNCTION: GAMEMD 0x0078DB70
BOOL CALLBACK WOL_New_Game_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	CreateGameDialogControl * control = (CreateGameDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDCANCEL:
					control->F1(window, 2);
					return FALSE;

				case IDOK:
					{
						g_MaxPlayers = 7;
						Wstring text;
						FUN_00623E40(GetDlgItem(window, 0x72C), text);
						control->Text = text;
						control->F1(window, 1);
					}
					return FALSE;

				case 0x629:
					if (SendDlgItemMessage(window, 0x629, 0xF0, 0, 0) == 1) {
						SendDlgItemMessage(window, 0x66C, 0xF1, 0, 0);
						control->Flag11 = false;
						control->Flag10 = true;
					} else {
						control->Flag10 = false;
					}
					return FALSE;

				case 0x66C:
					if (SendDlgItemMessage(window, 0x66C, 0xF0, 0, 0) == 1) {
						SendDlgItemMessage(window, 0x629, 0xF1, 0, 0);
						control->Flag11 = true;
						control->Flag10 = false;
					} else {
						control->Flag11 = false;
					}
					return FALSE;

				case 0x6DF:
					if (IsDlgButtonChecked(window, 0x6E0)) {
						CheckDlgButton(window, 0x6E0, 0);
					} else {
						CheckDlgButton(window, 0x6DF, 1);
					}
					return FALSE;

				case 0x72C:
					if (lparam) {
						Wstring text;
						FUN_00623E40((HWND)lparam, text);
						EnableWindow(GetDlgItem(window, 1), !text.containsAny(" ,;"));
					}
					return FALSE;
				case 0x6E0:
					if (IsDlgButtonChecked(window, 0x6DF)) {
						CheckDlgButton(window, 0x6DF, 0);
					} else {
						CheckDlgButton(window, 0x6E0, 1);
					}
					return FALSE;

				case 0x71E:
					control->Flag12 = SendMessage((HWND)lparam, 0xF0, 0, 0) == 1;
					return FALSE;

				case 0x71F:
					control->Flag13 = SendMessage((HWND)lparam, 0xF0, 0, 0) == 1;
					return FALSE;

			}
			break;

		case 0x497:
			{
				HWND edit = GetDlgItem(window, 0x72C);
				SendMessage(edit, 0x4D1, 1, 0);
				edit = GetDlgItem(window, 0x72C);
				SendMessage(edit, 0x4B4, 0, (LPARAM)DAT_00889F64);
				SendDlgItemMessage(window, 0x72C, 0xC5, 8, 0);
				Focus_Dialog_Item(window, 0x72C);
				CheckDlgButton(window, 0x629, control->Flag10 != 0);
				CheckDlgButton(window, 0x66C, control->Flag11 != 0);
				if (!(Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) && g_OwnSquadID < 1) {
					EnableWindow(GetDlgItem(window, 0x66C), FALSE);
				}
				HWND box = GetDlgItem(window, 0x71E);
				bool big = Options.ScreenWidth >= 0x400;
				EnableWindow(box, !big);
				SendMessage(box, 0xF1, (control->Flag12 && !big) ? 1 : 0, 0);
				HWND box2 = GetDlgItem(window, 0x71F);
				SendMessage(box2, 0xF1, control->Flag13 != 0, 0);
			}
			break;

		case 0x4DA:
			PostMessage(GetDlgItem(window, 1), 0xF5, 0, 0);
			break;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x0078E030
BOOL CALLBACK WOL_Password_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	WonlineStringDialogControl * control = (WonlineStringDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x72C:
					if (lparam) {
						Wstring text;
						FUN_00623E40((HWND)lparam, text);
						EnableWindow(GetDlgItem(window, 1), !text.containsAny(" ,;"));
					}
					return FALSE;

				case IDCANCEL:
					control->F1(window, 2);
					return FALSE;

				case IDOK:
					{
						Wstring text;
						FUN_00623E40(GetDlgItem(window, 0x72C), text);
						control->Text = text;
						control->F1(window, 1);
					}
					return FALSE;
			}
			break;

		case 0x497:
			{
				HWND edit = GetDlgItem(window, 0x72C);
				SendMessage(edit, 0x4D1, 1, 0);
				edit = GetDlgItem(window, 0x72C);
				SendMessage(edit, 0x4B4, 0, (LPARAM)DAT_00889F64);
				SendDlgItemMessage(window, 0x72C, 0xC5, 8, 0);
				Focus_Dialog_Item(window, 0x72C);
			}
			break;

		case 0x4DA:
			PostMessage(GetDlgItem(window, 1), 0xF5, 0, 0);
			break;
	}
	return FALSE;
}

int __fastcall FUN_0060D320(HWND dialog);

extern int DAT_00B73A1C;
extern int DAT_00B73AB8;
extern int DAT_00B7782C;
extern int DAT_00B76228;

// FUNCTION: GAMEMD 0x0078E230
BOOL CALLBACK WOL_Begin_Nick_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					{
						DAT_00B76E34.clear();
						HWND item = GetDlgItem(window, 0x626);
						if (item) {
							FUN_00623E40(item, DAT_00B76E34);
						}
						DAT_00B73B30.clear();
						item = GetDlgItem(window, 0x62E);
						if (item) {
							FUN_00623E40(item, DAT_00B73B30);
							if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_JANUARY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3ADF))) == 0) {
								DAT_00B73A1C = 0;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_FEBRUARY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AE1))) == 0) {
								DAT_00B73A1C = 1;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_MARCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AE3))) == 0) {
								DAT_00B73A1C = 2;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_APRIL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AE5))) == 0) {
								DAT_00B73A1C = 3;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_MAY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AE7))) == 0) {
								DAT_00B73A1C = 4;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_JUNE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AE9))) == 0) {
								DAT_00B73A1C = 5;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_JULY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AEB))) == 0) {
								DAT_00B73A1C = 6;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_AUGUST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AED))) == 0) {
								DAT_00B73A1C = 7;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_SEPTEMBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AEF))) == 0) {
								DAT_00B73A1C = 8;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_OCTOBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AF1))) == 0) {
								DAT_00B73A1C = 9;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_NOVEMBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AF3))) == 0) {
								DAT_00B73A1C = 10;
							} else if (stricmp(DAT_00B73B30.get(), FUN_007350C0(StringTable::LoadString("TXT_DECEMBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3AF5))) == 0) {
								DAT_00B73A1C = 11;
							}
						} else {
							DAT_00B73A1C = 0;
						}
						DAT_00B7622C.clear();
						item = GetDlgItem(window, 0x62F);
						if (item) {
							FUN_00623E40(item, DAT_00B7622C);
							DAT_00B73AB8 = atol(DAT_00B7622C.get());
						}
						DAT_00B76908.clear();
						item = GetDlgItem(window, 0x630);
						if (item) {
							FUN_00623E40(item, DAT_00B76908);
							DAT_00B7782C = atol(DAT_00B76908.get());
						}
						DAT_00B76228 = 0;
						item = GetDlgItem(window, 0x631);
						if (item) {
							DAT_00B76228 = SendMessage(item, 0xF0, 0, 0);
						}
						FUN_007757E0(window, 1);
					}
					return FALSE;

				case IDCANCEL:
					FUN_007757E0(window, 2);
					return FALSE;

				case 0x576:
					ViewHTML("http://privacypolicy.westwood.com", 0);
					return FALSE;
			}
			return FALSE;

		case 0x497:
			{
				HWND item = GetDlgItem(window, 0x626);
				SendMessage(item, 0x4D1, 1, 0);
				SendMessage(GetDlgItem(window, 0x62F), 0xC5, 2, 0);
				SendMessage(GetDlgItem(window, 0x630), 0xC5, 4, 0);
				SetFocus(GetDlgItem(window, 0x62F));
				HWND combo = GetDlgItem(window, 0x62E);
				FUN_00623840(combo);
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_JANUARY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B3A));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_FEBRUARY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B3B));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_MARCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B3C));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_APRIL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B3D));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_MAY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B3E));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_JUNE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B3F));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_JULY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B40));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_AUGUST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B41));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_SEPTEMBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B42));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_OCTOBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B43));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_NOVEMBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B44));
				SendMessage(combo, 0x4C2, 0, (LPARAM)StringTable::LoadString("TXT_DECEMBER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3B45));
				SendDlgItemMessage(window, 0x62E, 0x14E, 0, 0);
				SendMessage(window, 0x4A9, 0, 1);
			}
			return FALSE;

		case 0x4DB:
			Focus_Next_Tab_Item((HWND)lparam, wparam != 0);
			return FALSE;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x0078E950
BOOL CALLBACK WOL_New_Nick_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					{
						int mode = FUN_0060D320(window);
						DAT_00B77794.clear();
						HWND item = GetDlgItem(window, 0x628);
						if (item) {
							FUN_00623E40(item, DAT_00B77794);
						}
						DAT_00B778F4.clear();
						item = GetDlgItem(window, 0x72C);
						if (item) {
							FUN_00623E40(item, DAT_00B778F4);
						}
						DAT_00B778FC.clear();
						item = GetDlgItem(window, 0x627);
						if (item) {
							FUN_00623E40(item, DAT_00B778FC);
						}
						if (mode == 0xF3) {
							DAT_00B765EC.clear();
							item = GetDlgItem(window, 0x5E5);
							if (item) {
								FUN_00623E40(item, DAT_00B765EC);
								FUN_007757E0(window, 1);
								return FALSE;
							}
						} else if (mode == 0xF4) {
							DAT_00B76C20 = 0;
							item = GetDlgItem(window, 0x66B);
							if (item) {
								DAT_00B76C20 = SendMessage(item, 0xF0, 0, 0);
							}
						}
						FUN_007757E0(window, 1);
					}
					return FALSE;

				case IDCANCEL:
					FUN_007757E0(window, 2);
					return FALSE;
			}
			break;

		case 0x497:
			{
				HWND item = GetDlgItem(window, 0x628);
				SendMessage(item, 0x4D1, 1, 0);
				item = GetDlgItem(window, 0x72C);
				SendMessage(item, 0x4D1, 1, 0);
				item = GetDlgItem(window, 0x627);
				SendMessage(item, 0x4D1, 1, 0);
				item = GetDlgItem(window, 0x5E5);
				SendMessage(item, 0x4D1, 1, 0);
				SendMessage(GetDlgItem(window, 0x628), 0xC5, 9, 0);
				SendMessage(GetDlgItem(window, 0x72C), 0xC5, 8, 0);
				SendMessage(GetDlgItem(window, 0x627), 0xC5, 8, 0);
				Focus_Dialog_Item(window, 0x628);
			}
			break;

		case 0x4DB:
			DAT_00B77E90++;
			Focus_Next_Tab_Item((HWND)lparam, wparam != 0);
			return FALSE;
	}
	return FALSE;
}

extern int DAT_00B76474;
extern int DAT_00B766B8;
extern int DAT_00B76958;
extern int DAT_00B76910;
extern int DAT_00B77798;
extern int DAT_00B768F8;
extern int DAT_00B76AC0;
extern int DAT_00B73680;
extern int DAT_00B77790;
extern int * DAT_00B77E98;
namespace OwnerDraw {
	void __fastcall DrawCampaignMenuTransition(HWND window, bool reverse);
}
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void FUN_0072B8E0(void);
void FUN_0072B930(void);
void __fastcall FUN_006B6230(void * unused);

// FUNCTION: GAMEMD 0x0078EBC0
BOOL CALLBACK WOL_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	bool play;
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_DESTROY:
			GameActive = true;
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDCANCEL:
					delete DAT_00B77E98;
					DAT_00B77E98 = NULL;
					FUN_007757E0(window, 2);
					return FALSE;

				case IDOK:
					{
						HWND item;
						DAT_00B76474 = 0;
						item = GetDlgItem(window, 0x5F5);
						if (item) {
							DAT_00B76474 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B766B8 = 0;
						item = GetDlgItem(window, 0x5F7);
						if (item) {
							DAT_00B766B8 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B76958 = 0;
						item = GetDlgItem(window, 0x5F6);
						if (item) {
							DAT_00B76958 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B76910 = 0;
						item = GetDlgItem(window, 0x5F8);
						if (item) {
							DAT_00B76910 = SendMessage(item, 0xF0, 0, 0);
						}
						item = GetDlgItem(window, 0x5F9);
						if (item) {
							DAT_00B77798 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B768F8 = 0;
						item = GetDlgItem(window, 0x5FA);
						if (item) {
							DAT_00B768F8 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B76AC0 = 0;
						item = GetDlgItem(window, 0x5FB);
						if (item) {
							DAT_00B76AC0 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B73680 = 0;
						item = GetDlgItem(window, 0x774);
						if (item) {
							DAT_00B73680 = SendMessage(item, 0xF0, 0, 0);
						}
						DAT_00B77790 = 0;
						item = GetDlgItem(window, 0x775);
						if (item) {
							DAT_00B77790 = SendMessage(item, 0xF0, 0, 0);
						}
						delete DAT_00B77E98;
						DAT_00B77E98 = NULL;
						FUN_007757E0(window, 1);
					}
					return FALSE;

				case 0x5F8:
					if (HIWORD(wparam) != 1 && HIWORD(wparam) != 0) {
						return FALSE;
					}
					play = false;
					if (rc == 0) {
						play = SendMessage((HWND)lparam, 0xF0, 0, 0) == 1;
					}
					if (play) {
						if (!Theme.Still_Playing()) {
							Theme.Stop();
							Theme.Queue_Song(THEME_PICK_ANOTHER);
							return FALSE;
						}
						if (Theme.What_Is_Playing() == Theme.From_Name("INTRO")) {
							Theme.Stop();
							Theme.Queue_Song(THEME_PICK_ANOTHER);
							return FALSE;
						}
					} else {
						Theme.Queue_Song(THEME_QUIET);
						return FALSE;
					}
					return FALSE;

				case 0x63A:
					OwnerDraw::DrawCampaignMenuTransition(window, false);
					FUN_0052FEC0(NULL, true);
					GameActive = false;
					FUN_0072B8E0();
					FUN_006B6230(&lparam);
					ShowWindow(window, SW_SHOW);
					OwnerDraw::SetUserData2(window, 0);
					FUN_0072B930();
					return FALSE;
			}
			break;

		case 0x4ED:
			ShowWindow(window, SW_HIDE);
			OwnerDraw::SetUserData2(window, 1);
			return FALSE;

		case 0x497:
				if (g_AllowPage) {
					SendDlgItemMessage(window, 0x5F5, 0xF1, 1, 0);
				}
				if (g_AllowFind) {
					SendDlgItemMessage(window, 0x5F7, 0xF1, 1, 0);
				}
				if (g_LangFilter) {
					SendDlgItemMessage(window, 0x5F6, 0xF1, 1, 0);
				}
				if (g_LobbyMusic) {
					SendDlgItemMessage(window, 0x5F8, 0xF1, 1, 0);
				}
				if (g_ShowAllChannels) {
					SendDlgItemMessage(window, 0x5F9, 0xF1, 1, 0);
				}
				if (DAT_00B779DC) {
					SendDlgItemMessage(window, 0x5FA, 0xF1, 1, 0);
				}
				if (DAT_00B779C0) {
					SendDlgItemMessage(window, 0x5FB, 0xF1, 1, 0);
				}
				if (DAT_00B76AF0) {
					SendDlgItemMessage(window, 0x774, 0xF1, 1, 0);
				}
				if (DAT_00B73AD0) {
					SendDlgItemMessage(window, 0x775, 0xF1, 1, 0);
				}
			break;
	}
	return FALSE;
}

int __fastcall FUN_005E8D00(wchar_t const * name);
extern wchar_t DAT_00B75F6C[];

int __fastcall FUN_005E64C0(int index);
bool __fastcall FUN_005E9440(NodeNameType * slot);
extern NodeNameType * DAT_00AC119C;
extern NodeNameType * DAT_00AC11B4;
extern char DAT_00B76B44[];
extern char DAT_00822050[];

// FUNCTION: GAMEMD 0x0078F000
void FUN_0078F000(void)
{
	int count = FUN_005E64C0(Session.Options.ScenarioIndex);
	if (Game::ObserverMode == 1) {
		count++;
	}
	int players = 0;
	int observers = 0;
	int extra = 0;
	int i;
	for (i = 1; i < count; i++) {
		NodeNameType * slot = Session.Slots[i];
		if (slot == DAT_00AC119C || slot == DAT_00AC11B4) {
			observers++;
		} else if (FUN_005E93F0(slot)) {
			players++;
		} else if (FUN_005E9440(slot)) {
			extra++;
		}
	}
	for (; i < 8; i++) {
		NodeNameType * slot = Session.Slots[i];
		if (slot == DAT_00AC11B4) {
			observers++;
		} else if (FUN_005E93F0(slot)) {
			players++;
		}
	}
	if (g_pChat) {
		int total = observers + players + 1;
		Wstring message = Wstring("mode #") + Wstring(DAT_00B76B44) + Wstring(" +l ") + Wstring_FromInt(total) + Wstring(DAT_00822050);
		g_pChat->RequestRawMessage(message.get());
		char info[80];
		memset(info, 0, sizeof(info));
		Encode_Channel_ExInfo(info);
		info[40] = 0;
		g_pChat->SetChannelExInfo(info);
		g_pChat->RequestSetPlayerCount(1, total + extra);
	}
}

extern wchar_t DAT_00B76238[];

static inline wchar_t * Wide_Squad_Name(User const & user)
{
	return FUN_00735120((char const *)user.squadname);
}

// FUNCTION: GAMEMD 0x0078F200
wchar_t * __fastcall FUN_0078F200(wchar_t const * name)
{
	Wstring player_name;
	Ladder player_ladder;
	memset(&player_ladder, 0, sizeof(player_ladder));
	player_name.set(FUN_007350C0(name));
	player_name.toLower();
	bool player_found = g_ActiveLadder && g_ActiveLadder->getValue(player_name, player_ladder);
	Ladder squad_ladder;
	memset(&squad_ladder, 0, sizeof(squad_ladder));
	Wstring squad_abbreviation;
	User user;
	for (int index = 0; index < g_UserList.length(); ++index) {
		g_UserList.get(user, index);
		if (strcmp((char *)user.name, FUN_007350C0(name)) == 0) break;
	}
	bool has_squad = false;
	bool squad_found = false;
	if (strlen((char *)user.squadname) != 0) {
		has_squad = true;
		squad_abbreviation.set((char *)user.squadabbrev);
		squad_abbreviation.toLower();
		if (g_BattleClansLadderDict.getValue(squad_abbreviation, squad_ladder)) squad_found = true;
	}
	if (player_found) {
		if (has_squad) {
			if (squad_found) {
				wchar_t * squad_name = Wide_Squad_Name(user);
				swprintf(DAT_00B76238, StringTable::LoadString("TXT_PLAYER_TOOLTIP_R_C_CR", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D5C),
					StringTable::LoadString("TXT_RANK", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D5C), player_ladder.rung, squad_name, squad_ladder.rung);
			} else {
				wchar_t * squad_name = Wide_Squad_Name(user);
				swprintf(DAT_00B76238, StringTable::LoadString("TXT_PLAYER_TOOLTIP_R_C_CUNR", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D60),
					StringTable::LoadString("TXT_RANK", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D60), player_ladder.rung, squad_name);
			}
		} else {
			swprintf(DAT_00B76238, StringTable::LoadString("TXT_PLAYER_TOOLTIP_R_NOC", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D64),
				StringTable::LoadString("TXT_RANK", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D64), player_ladder.rung);
		}
	} else {
		if (has_squad) {
			if (squad_found) {
				wchar_t * squad_name = Wide_Squad_Name(user);
				swprintf(DAT_00B76238, StringTable::LoadString("TXT_PLAYER_TOOLTIP_UNR_C_CR", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D6A),
					StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D6A), squad_name, squad_ladder.rung);
			} else {
				wchar_t * squad_name = Wide_Squad_Name(user);
				swprintf(DAT_00B76238, StringTable::LoadString("TXT_PLAYER_TOOLTIP_UNR_C_CUNR", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D6E),
					StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D6E), squad_name);
			}
		} else {
			swprintf(DAT_00B76238, StringTable::LoadString("TXT_PLAYER_TOOLTIP_UNR_NOC", 0, "D:\\ra2mdpost\\wonline.cpp", 0x3D72),
				StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D72));
		}
	}
	return DAT_00B76238;
}

// FUNCTION: GAMEMD 0x0078F5C0
wchar_t * __fastcall FUN_0078F5C0(wchar_t const * name)
{
	int ping = DAT_00B76440[FUN_005E8D00(name)];
	if (ping < 0) {
		swprintf(DAT_00B75F6C, StringTable::LoadString("TXT_LATENCY_ICON_TOOLTIP_NOPING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D7D), name, StringTable::LoadString("TXT_UNKNOWN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D7D), StringTable::LoadString("TXT_UNKNOWN_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D7D));
	} else if (ping < 300) {
		swprintf(DAT_00B75F6C, StringTable::LoadString("TXT_LATENCY_ICON_TOOLTIP_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D7F), name, StringTable::LoadString("TXT_GOOD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D7F), ping);
	} else if (ping < 500) {
		swprintf(DAT_00B75F6C, StringTable::LoadString("TXT_LATENCY_ICON_TOOLTIP_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D81), name, StringTable::LoadString("TXT_FAIR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D81), ping);
	} else {
		swprintf(DAT_00B75F6C, StringTable::LoadString("TXT_LATENCY_ICON_TOOLTIP_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D83), name, StringTable::LoadString("TXT_POOR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3D83), ping);
	}
	return DAT_00B75F6C;
}

// FUNCTION: GAMEMD 0x0078F6B0
int __fastcall FUN_0078F6B0(int id)
{
	if (id == 0x754) {
		return 0;
	}
	if (id == 0x755) {
		return 1;
	}
	if (id == 0x756) {
		return 2;
	}
	if (id == 0x757) {
		return 3;
	}
	if (id == 0x758) {
		return 4;
	}
	if (id == 0x759) {
		return 5;
	}
	if (id == 0x75A) {
		return 6;
	}
	if (id == 0x75B) return 7;
	return -1;
}

// FUNCTION: GAMEMD 0x0078F720
void __fastcall FUN_0078F720(HWND window)
{
	if (FUN_0077D940()) {
		EnableWindow(GetDlgItem(window, 0x552), FALSE);
		EnableWindow(GetDlgItem(window, 0x553), FALSE);
		EnableWindow(GetDlgItem(window, 0x597), FALSE);
		Session.Options.AIPlayers = 0;
		Session.Options.AlliesAllowed = false;
		HWND item = GetDlgItem(window, 0x54E);
		SendMessage(item, 0xF1, 1, 0);
		EnableWindow(item, FALSE);
		item = GetDlgItem(window, 0x54B);
		SendMessage(item, 0xF1, 1, 0);
		EnableWindow(item, FALSE);
		Session.Options.ShortGame = true;
		Session.Options.Bases = true;
	}
}

namespace Game { extern int ObserverMode; }

// FUNCTION: GAMEMD 0x00791AA0
void __fastcall FUN_00791AA0(HWND window)
{
	if (window) {
		if (!Game::ObserverMode) {
			ODMessageBox(StringTable::LoadString("TXT_CHANNEL_FULL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x4351), 0, FUN_0077D710, 0);
		} else {
			ODMessageBox(StringTable::LoadString("TXT_OBSERVER_FULL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x4353), 0, FUN_0077D710, 0);
		}
		if (g_pChat->RequestChannelLeave() < 0) {
			FUN_007757E0(window, 2);
		}
	}
}

HWND WS_Create_Dialog(HINSTANCE instance, int id, HWND parent, DLGPROC proc, BOOL force_show);
void FUN_0072CBA0(void);
namespace UI { void __fastcall RegisterWindow(HWND window, int id); }
BOOL CALLBACK WOL_Waiting_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00792BE0
void __fastcall Show_Wait_Window(int id, WideWstring const & text, char flag)
{
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags |= id;
		if (text.length() > 0) {
			HWND list = GetDlgItem(dialog, 0x5B2);
			SendMessage(list, 0x4B2, 0, (LPARAM)text.text());
		}
	} else if (id) {
		HWND top = FUN_00775B10();
		HWND input = GetDlgItem(top, 0x540);
		FUN_0072CBA0();
		dialog = WS_Create_Dialog(ProgramInstance, 0xC4, MainWindow, WOL_Waiting_Dialog_Proc, FALSE);
		UI::RegisterWindow(dialog, 0xC4);
		HWND list = GetDlgItem(dialog, 0x5B2);
		SendMessage(list, 0x4B2, 0, (LPARAM)text.text());
		g_ActiveWaitFlags = id;
		if (input) {
			SetFocus(input);
		}
	}
	if (flag && g_ActiveWaitFlags) {
		FUN_007759E0(dialog, (bool (*)(void))FUN_0077D710, 0, 1, 0);
	}
}

extern int DAT_0084A248;
void __fastcall FUN_00608440(HWND window);

// FUNCTION: GAMEMD 0x00792CF0
BOOL CALLBACK WOL_Waiting_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_INITDIALOG:
			return FALSE;

		case WM_DESTROY:
			KillTimer(window, 0x45);
			return FALSE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x58E:
					FUN_00773E90("NetStatus");
					return FALSE;

				case 0x58D:
					SetEvent(g_WaitEventHandles[0]);
					FUN_007757E0(window, 1);
					break;
			}
			break;

		case WM_TIMER:
			if (wparam == 0x45) {
				ShowWindow(window, SW_SHOW);
				KillTimer(window, 0x45);
				return FALSE;
			}
			break;

		case 0x497:
			SendMessage(window, 0x4A9, 0, 1);
			SetTimer(window, 0x45, DAT_0084A248, NULL);
			FUN_00608440(window);
			HWND item = GetDlgItem(window, 0x7A9);
			PostMessage(item, 0x4D5, 0, 0);
			PostMessage(item, 0x4D3, 0, 0);
			return FALSE;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x00792E90
void __fastcall Set_Wait_Dialog_Text(wchar_t const * text)
{
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		SendDlgItemMessage(dialog, 0x5B2, 0x4B2, 0, (LPARAM)text);
	}
}

// FUNCTION: GAMEMD 0x00792EC0
void __fastcall FUN_00792EC0(unsigned int mask)
{
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags &= ~mask;
		if (g_ActiveWaitFlags == 0) {
			FUN_007757E0(dialog, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
}

#include "wsproto.h"

extern int WestwoodOnline_PingTimes[];
extern WinsockInterfaceClass * PacketTransport;
extern User DAT_00B777B0;
NodeNameType * __fastcall FUN_005E8BF0(wchar_t const * name);
User * Get_Channel_Host(void);

// FUNCTION: GAMEMD 0x00792F10
void __fastcall User_List_To_Session_Players(User * unused)
{
	Session.NumPlayers = 1;
	for (int i = 0; i < g_UserList.length(); i++) {
		User user;
		g_UserList.get(user, i);
		NodeNameType * node = FUN_005E8BF0(FUN_00735120((char *)user.name));
		if (node && FUN_005E93F0(node)) {
			node->Player.SquadID = user.squadID;
		}
		if (strcmpi((char *)user.name, g_NickName.get()) != 0) {
			if (user.flags & 1) {
				NodeNameType * host = FUN_005E8BF0(FUN_00735120((char *)user.name));
				NetNumType net;
				NetNodeType addr;
				host->Address.Get_Address(net, addr);
				Session.HostAddress.Set_Address(net, addr, 0);
			}
			int ping = 0;
			g_pNetUtil->GetAvgPing(user.ipaddr, &ping);
			if (ping < 0) {
				ping = 0;
			}
			WestwoodOnline_PingTimes[Session.NumPlayers] = ping;
			Session.NumPlayers++;
		}
	}
	PacketTransport->Clear_Broadcast_Addresses();
	for (int p = 1; p < Session.Players.Count(); p++) {
		FUN_004068E0("Adding broadcast address %s\n", Session.Players[p]->Address.As_String(NULL));
		PacketTransport->Set_Broadcast_Address((char const *)&Session.Players[p]->Address, 0);
	}
	FUN_004068E0("User_List_To_Session_Players: Session.HostAddress = %s\n", Session.HostAddress.As_String(NULL));
}

// FUNCTION: GAMEMD 0x007930A0
bool Is_Channel_Owner(void)
{
	User * host = Get_Channel_Host();
	if (!host) {
		return false;
	}
	if (strcmpi((char *)host->name, g_NickName.text()) != 0) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x00793120
bool __fastcall FUN_00793120(Wstring & name)
{
	User * host = Get_Channel_Host();
	if (!host) {
		return false;
	}
	if (strcmpi((char *)host->name, name.text()) != 0) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x007931A0
User * Get_Channel_Host(void)
{
	for (int i = 0; i < g_UserList.length(); i++) {
		g_UserList.get(DAT_00B777B0, i);
		if (DAT_00B777B0.flags & 1) {
			return &DAT_00B777B0;
		}
	}
	return NULL;
}

extern int DAT_0084A1F0[];
extern char DAT_00B75F54[];

// FUNCTION: GAMEMD 0x007931F0
void __fastcall FUN_007931F0(int index)
{
	if (index >= 0 && index < Session.Players.Count()) {
		HWND dialog = WS_Find_Dialog(0x130);
		if (dialog) {
			HWND item = GetDlgItem(dialog, DAT_0084A1F0[index]);
			DAT_00B75F54[index] = 1;
			SendMessage(item, 0xF1, 1, 0);
			PostMessage(dialog, 0xF, 0, 0);
		}
	}
}

BOOL CALLBACK FUN_00793280(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00793250
void FUN_00793250(void)
{
	HWND dialog = WS_Create_Dialog(ProgramInstance, 0x130, MainWindow, FUN_00793280, FALSE);
	if (dialog) {
		UI::RegisterWindow(dialog, 0x130);
	}
}

extern char DAT_00B757EB[];

// FUNCTION: GAMEMD 0x00793280
BOOL CALLBACK FUN_00793280(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_COMMAND:
			{
				int id = LOWORD(wparam);
				switch (id) {
					case 0x761:
					case 0x762:
					case 0x764:
					case 0x765:
					case 0x766:
					case 0x767:
					case 0x768:
					case 0x769:
						if (DAT_00B757EB[id]) {
							if (SendDlgItemMessage(window, id, 0xF0, 0, 0) == 0) {
								SendDlgItemMessage(window, id, 0xF1, 1, 0);
							}
						} else {
							if (SendDlgItemMessage(window, id, 0xF0, 0, 0) == 1) {
								SendDlgItemMessage(window, id, 0xF1, 0, 0);
							}
							return FALSE;
						}
						break;
				}
			}
			return FALSE;

		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
			return (BOOL)GetStockObject(BLACK_BRUSH);

		case 0x497:
			{
				for (int i = 0; i < 8; i++) {
					HWND item = GetDlgItem(window, DAT_0084A1F0[i]);
					memset(DAT_00B75F54, 0, 8);
					if (i > Session.Players.Count() - 1) {
						DestroyWindow(item);
					}
				}
				for (int j = 0; j < Session.Players.Count(); j++) {
					HWND item = GetDlgItem(window, DAT_0084A1F0[j]);
					SendMessage(item, 0x4B2, 0, (LPARAM)Session.Players[j]);
					SendMessage(item, 0xF1, 0, 0);
				}
				SendMessage(window, 0x4A9, 0, 1);
				FUN_00608440(window);
				ShowWindow(window, SW_SHOW);
			}
			return FALSE;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x00793440
void FUN_00793440(void)
{
	HWND dialog = WS_Find_Dialog(0x130);
	if (dialog) {
		FUN_007757E0(dialog, 0);
	}
}

extern int DAT_00B73BB4[];
extern int DAT_0084A1E0[];

// FUNCTION: GAMEMD 0x00793460
void __fastcall FUN_00793460(int index, int value)
{
	if (index < Session.Players.Count()) {
		if (DAT_00B73BB4[index] < DAT_0084A1E0[index]) {
			DAT_00B73BB4[index] = value;
		}
	}
}

// FUNCTION: GAMEMD 0x00793490
void FUN_00793490(void)
{
	wchar_t message[256];

	User * host = Get_Channel_Host();
	if (!host) {
		return;
	}
	int host_slot = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (stricmp(FUN_007350C0(Session.Players[i]->Name), (char *)host->name) == 0) {
			host_slot = i;
			break;
		}
	}
	bool reported = false;
	for (int slot = 0; slot < Session.Players.Count(); slot++) {
		switch (DAT_00B73BB4[slot]) {
			case 0:
				reported = true;
				FUN_004068E0("Player %s was unable to establish a connection\n", FUN_007350C0(Session.Players[slot]->Name));
				swprintf(message, StringTable::LoadString("TXT_TALK_REPORTED_PROBLEM", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x4998), Session.Players[slot]->Name);
				FUN_005E2700(DAT_007EEF28, message);
				break;

			case 1:
				reported = true;
				FUN_004068E0("Unable to connect to player %s\n", FUN_007350C0(Session.Players[slot]->Name));
				swprintf(message, StringTable::LoadString("TXT_TALK_LOCAL_PROBLEM", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x49A2), Session.Players[slot]->Name);
				FUN_005E2700(DAT_007EEF28, message);
				break;

			case 2:
				reported = true;
				if (slot == host_slot) {
					FUN_004068E0("Unable to talk to the pserver port\n");
					FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_TALK_FORWARDER_PROBLEM_GUES", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x49AD));
				} else {
					FUN_004068E0("Player %s was unable to connect to the pserver port\n", FUN_007350C0(Session.Players[slot]->Name));
					swprintf(message, StringTable::LoadString("TXT_TALK_FORWARDER_PROBLEM_HOST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x49B0), Session.Players[slot]->Name);
					FUN_005E2700(DAT_007EEF28, message);
				}
				break;

			case 3:
				reported = true;
				FUN_004068E0("The game host reported a connection problem between players\n");
				FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_TALK_HOST_ABORTED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x49BB));
				break;
		}
	}
	if (!reported) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_FAILED_TO_TALK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x49C8));
	}
}

// FUNCTION: GAMEMD 0x00793710
unsigned short __fastcall Get_Port_Blitz_Port_Number(int send_port, int mangled_port, int unused, int port_spread)
{
	int port = send_port + mangled_port;
	if (port_spread == 1) {
	} else if (port_spread == 2) {
		port = mangled_port + send_port * 2;
	} else if (port_spread > 1) {
		port = mangled_port + send_port / 2 * port_spread + send_port % 2;
	} else if (port_spread < 0) {
		if (port_spread == -100) {
			port = mangled_port + send_port * 100;
		} else {
			port = send_port % 2 - send_port / 2 * port_spread + mangled_port;
		}
	}
	FUN_004068E0("Get_Port_Blitz_Port_Number - send port = %d, mangled_port = %d, port_spread = %d, port = %d\n", send_port, mangled_port, port_spread, port);
	unsigned short result = port;
	if (result < 0x400) {
		result = 0x400;
	}
	return result;
}

#include "firewall.h"

// FUNCTION: GAMEMD 0x007937B0
int __fastcall FUN_007937B0(int (*pairs)[2])
{
	int order[8];
	memset(order, 0, sizeof(order));
	int i = 0;
	int * out = order;
	for (; i < 8; i++) {
		if (FUN_005E93F0(Session.Slots[i])) {
			*out++ = i;
		}
	}
	for (int a = 0; a < Session.Players.Count(); a++) {
		for (int b = 0; b < Session.Players.Count() - 1; b++) {
			int cur_value = Session.PlayerFirewall[order[b]];
			int next_value = Session.PlayerFirewall[order[b + 1]];
			if (FirewallHelper.Rank_A(cur_value) < FirewallHelper.Rank_A(next_value)) {
				int temp = order[b];
				order[b] = order[b + 1];
				order[b + 1] = temp;
			}
		}
	}
	FUN_004068E0("Connection order = %d, %d, %d, %d, %d, %d\n", order[0], order[1], order[2], order[3], order[4], order[5]);
	int n = 0;
	FUN_004068E0("Connection pairs....\n");
	for (int x = 0; x < Session.Players.Count(); x++) {
		for (int y = x + 1; y < Session.Players.Count(); y++) {
			pairs[n][0] = order[x];
			pairs[n][1] = order[y];
			FUN_004068E0("\t%d & %d\n", order[x], order[y]);
			n++;
		}
	}
	return n;
}



// FUNCTION: GAMEMD 0x00793930
void __fastcall FUN_00793930(char const * from, char const * to, int state)
{
	int from_slot = -1;
	int to_slot = -1;
	FUN_004068E0("Registering connection state from %s to %s. conn_state = %d\n", from, to, state);
	for (int i = 0; i < 8; i++) {
		if (stricmp(FUN_007350C0(Session.Slots[i]->Name), from) == 0) {
			from_slot = i;
		}
		if (stricmp(FUN_007350C0(Session.Slots[i]->Name), to) == 0) {
			to_slot = i;
		}
	}
	for (int j = 0; j < Session.NumConnectionPairs; j++) {
		if (Session.ConnectionPairs[j][0] == from_slot && Session.ConnectionPairs[j][1] == to_slot) {
			Session.ConnectionResults[j * 2] = state;
			return;
		}
		if (Session.ConnectionPairs[j][0] == to_slot && Session.ConnectionPairs[j][1] == from_slot) {
			Session.ConnectionResults[j * 2 + 1] = state;
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x00793A10
void __fastcall FUN_00793A10(char const * name, unsigned short port)
{
	FUN_004068E0("Player %s is telling us we should send to port %d\n", name, port);
	for (int i = 0; i < 8; i++) {
		if (stricmp(FUN_007350C0(Session.Slots[i]->Name), name) == 0) {
			Session.unknown_2E18[i] = port;
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x00793A70
void __fastcall FUN_00793A70(char const * name, unsigned short port)
{
	char text[256];
	User * user = NULL;
	for (int i = 0; i < g_UserList.length(); i++) {
		g_UserList.getPointer(&user, i);
		if (stricmp(name, (char *)user->name) == 0) {
			text[0] = 'Z';
			text[1] = '4';
			sprintf(text + 2, "%d", port);
			FUN_004068E0("Sending string %s to user %s\n", text, name);
			g_pChat->RequestPrivateGameOptions(user, text);
		}
	}
}

// FUNCTION: GAMEMD 0x00793B30
void __fastcall FUN_00793B30(int mode, char const * options)
{
	char text[256];
	text[0] = 'Z';
	text[1] = '1';
	text[2] = (char)(mode + '0');
	if (options) {
		strcpy(text + 3, options);
	} else {
		text[3] = 0;
	}
	FUN_004068E0("Sending public options string %s\n", text);
	g_pChat->RequestPublicGameOptions(text);
}

// FUNCTION: GAMEMD 0x00793BB0
void FUN_00793BB0(void)
{
	for (int i = 0; i < Session.KeepAliveCount; ++i) {
		if (Session.KeepAliveTimers[i] == 0) {
			GlobalPacketType packet;
			memset(&packet, 0, sizeof(packet));
			packet.Command = NET_PING;
			strcpy(packet.Name, g_NickName.get());
			char const *name = FUN_007350C0(Session.Slots[Session.KeepAliveSlots[i]]->Name);
			FUN_004068E0("Sending keepalive NET_PING packet to %s\n", name);
			Ipx.Send_Global_Message_To_Name(&packet, 0x2F, 0, Session.Slots[Session.KeepAliveSlots[i]]->Name);
			Session.KeepAliveTimers[i] = CDTimerClass<SystemTimerClass>(1200);
		}
	}
}

// FUNCTION: GAMEMD 0x00793CD0
void __fastcall FUN_00793CD0(int slot)
{
	Session.KeepAliveSlots[Session.KeepAliveCount] = slot;
	Session.KeepAliveTimers[Session.KeepAliveCount] = 1200;
	Session.KeepAliveCount++;
}

// FUNCTION: GAMEMD 0x00793D20
unsigned char __fastcall FUN_00793D20(int partner, int tag, int retries)
{
	CDTimerClass<SystemTimerClass> timeout;
	CDTimerClass<SystemTimerClass> resend_timer;
	bool mangling = true;
	int packet_len = 0;
	IPXAddressClass address;
	int peek_packet = 0;
	bool gotit = false;
	FUN_004068E0("Start_Talking_To player %d, retries = %d\n", partner, retries);

	int session_players_index = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		const wchar_t * slot_name = Session.Slots[partner]->Name;
		char * player_name = FUN_007350C0(Session.Players[i]->Name);
		if (stricmp(player_name, FUN_007350C0(slot_name)) == 0) {
			session_players_index = i;
		}
	}
	if (session_players_index == -1) {
		FUN_00793B30(0, NULL);
		FUN_004068E0("session_players_index == %d, returning false from Start_Talking_To.\n", session_players_index);
		return false;
	}

	for (int n = 0; n < MAX_PLAYERS && Session.unknown_2E28[n][0]; n++) {
		if (stricmp(Session.unknown_2E28[n], FUN_007350C0(Session.Slots[partner]->Name)) == 0) {
			FUN_004068E0("Behind the same NAT, mangling == false\n");
			mangling = false;
			break;
		}
	}

	int their_firewall = Session.PlayerFirewall[partner];
	if (mangling && !FirewallHelper.Is_NAT(their_firewall) && !FirewallHelper.Is_NAT(FirewallHelper.Get_Raw_Firewall_Behavior())) {
		FUN_004068E0("Both have crap firewalls, or modems, or somesuch. mangling = false\n");
		mangling = false;
	}

	int trying = 0;
	unsigned short last_send_port = 0;
	unsigned short my_mangled_port = 0;
	GlobalPacketType packet;
	GlobalPacketType receive_packet;
	unsigned short product_id;

	do {
		FUN_004068E0("Sending keep alive packets\n");
		FUN_00793BB0();

		if (!mangling) {
			unsigned short port = Session.PlayerPorts[session_players_index];
			FUN_004068E0("Easy mangling, Session.OtherPlayersMangledPorts[%d] = %d\n", partner, port);
			Session.unknown_2E18[partner] = port;
		} else {
			int behavior = FirewallHelper.Get_Raw_Firewall_Behavior();
			unsigned short mangled_port = 0;

			FUN_004068E0("Raw firewall behavior is %d\n", behavior);
			if ((behavior & 0x40) == 0) {
				FUN_004068E0("No destination port delta - looking for earlier connections at the same address\n");
				for (int r = 0; r < Session.KeepAliveCount; r++) {
					int slot = Session.KeepAliveSlots[r];
					IPXAddressClass old_address = Session.Slots[slot]->Address;
					if (old_address.FUN_0053EF30(Session.Slots[partner]->Address)) {
						mangled_port = Session.unknown_2F4C[slot];
						FUN_004068E0("Found old connection port number to use - port number %d\n", mangled_port);
					}
				}
			}

			if (mangled_port == 0) {
				FUN_004068E0("Work out what my port will be.\n");
				for (int p = 0; p < Session.Players.Count(); p++) {
					const wchar_t * player_name = Session.Players[p]->Name;
					char * name = FUN_007350C0(player_name);
					if (stricmp(name, g_NickName.get()) == 0) {
						unsigned short base_port = Session.PlayerPorts[p];
						FUN_004068E0("My default port = %d\n", base_port);
						mangled_port = FirewallHelper.Get_Next_Mangled_Source_Port(base_port);
						FUN_004068E0("My mangled port = %d\n", mangled_port);
						break;
					}
				}
			}

			FUN_00793A70(FUN_007350C0(Session.Slots[partner]->Name), mangled_port);
			my_mangled_port = mangled_port;
		}

		timeout = CDTimerClass<SystemTimerClass>(360);
		if (trying > 0) {
			timeout = CDTimerClass<SystemTimerClass>(240);
		}
		while (timeout.Value() != 0 && ((mangling == true && Session.unknown_2E18[partner] == last_send_port) || (mangling == false && Session.unknown_2E18[partner] == 0))) {
			WOL_Wait_Callback();
		}

		last_send_port = Session.unknown_2E18[partner];
		if (last_send_port != 0) {
			FUN_004068E0("Session.OtherPlayersMangledPorts[%d] != 0\n", partner);
			Session.PlayerPorts[session_players_index] = Session.unknown_2E18[partner];
			Session.Players[session_players_index]->Address.Set_Port(Session.unknown_2E18[partner]);
			Session.Slots[partner]->Address.Set_Port(Session.unknown_2E18[partner]);

			memset(&packet, 0, 0x2F);
			packet.Command = NET_PORT_PROBE;
			strcpy(packet.Name, g_NickName.get());

			if (FirewallHelper.Is_Netgear() && !FirewallHelper.Is_Netgear(Session.PlayerFirewall[partner])) {
				FUN_004068E0("Doing the netgear sleep thing\n");
				CDTimerClass<SystemTimerClass> netgear_timer(240);
				while (netgear_timer.Value() != 0) {
					WOL_Wait_Callback();
				}
			}

			const wchar_t * slot_name = Session.Slots[partner]->Name;
			char * name = FUN_007350C0(slot_name);
			FUN_004068E0("Sending NET_PORT_PROBE packet to %s at port %d\n", name, Session.PlayerPorts[session_players_index]);
			timeout = CDTimerClass<SystemTimerClass>(360);
			resend_timer = CDTimerClass<SystemTimerClass>(0);

			do {
				if (resend_timer == 0) {
					FUN_004068E0("Sending NET_PORT_PROBE packet to player %S\n", Session.Slots[partner]->Name);
					Ipx.Send_Global_Message_To_Name(&packet, 0x2F, 0, Session.Slots[partner]->Name);
					resend_timer = CDTimerClass<SystemTimerClass>(30);
				}
				WOL_Wait_Callback();

				if (!gotit) {
					if (peek_packet >= Ipx.Global_Num_Receive()) {
						peek_packet = 0;
					}
					if (Ipx.Global_Channel_Op(peek_packet, (int)&receive_packet, (int)&packet_len, (int)&address, (int)&product_id, 0) && product_id == 0xaa04) {
						if (receive_packet.Command == NET_PING || receive_packet.Command == NET_PORT_PROBE) {
							if (receive_packet.Command == NET_PORT_PROBE) {
								FUN_004068E0("Got NET_PORT_PROBE packet\n");
								if (Session.Players[session_players_index]->Address.FUN_0053EF30(address)) {
									if (stricmp(FUN_007350C0(Session.Slots[partner]->Name), receive_packet.Name) == 0) {
										FUN_004068E0("Got NET_PORT_PROBE packet from %s\n", receive_packet.Name);
										FUN_00793B30(11, receive_packet.Name);
										FUN_007931F0(session_players_index);
										FUN_00793CD0(partner);
										gotit = true;
										Session.unknown_2F4C[partner] = my_mangled_port;
									}
								}
							}
							Ipx.Global_Channel_Op_3C(peek_packet);
						}
					}
				}

				if (gotit && Session.ConnectionResults[tag] == 11) {
					FUN_004068E0("Got connection result - my packet must have got through\n");
					break;
				}

				int result = Session.ConnectionResults[tag];
				if (result >= 1 && result < 11) {
					int their_try = result - 1;
					if (their_try > trying) {
						FUN_004068E0("Other player is trying again - their_try = %d\n", their_try);
						trying = their_try - 1;
						break;
					}
				}
				peek_packet++;
			} while (timeout.Value() != 0);
		}

		if (gotit && Session.ConnectionResults[tag] == 11) {
			FUN_004068E0("gotit && Session.ConnectionResults[completion_index] == CONNRESULT_CONNECTED\n");
			break;
		}
		if (trying >= retries) {
			FUN_004068E0("trying >= retries\n");
			break;
		}
		trying++;
		FUN_00793B30(trying + 1, FUN_007350C0(Session.Slots[partner]->Name));
		FUN_004068E0("Trying again - trying = %d\n", trying);
	} while (true);

	return gotit;
}


#include "wspudp.h"

static inline bool Is_Local_Host(void)
{
	return Is_Channel_Owner();
}

#include <snmp.h>

struct tConnInfoStruct {
	unsigned int State;
	unsigned long LocalIP;
	unsigned short LocalPort;
	unsigned long RemoteIP;
	unsigned short RemotePort;
};

extern BOOL (__stdcall * SnmpExtensionInitPtr)(DWORD uptime, HANDLE * trap_event, AsnObjectIdentifier * first_region);
extern BOOL (__stdcall * SnmpExtensionQueryPtr)(BYTE pdu_type, SnmpVarBindList * bind_list, AsnInteger32 * error_status, AsnInteger32 * error_index);
extern LPVOID (__stdcall * SnmpUtilMemAllocPtr)(DWORD bytes);
extern VOID (__stdcall * SnmpUtilMemFreePtr)(LPVOID mem);
hostent * __stdcall FUN_007C8980(char const * name);
int __stdcall FUN_007C897A(void);

#include "ownertalk.h"

extern bool DAT_00B77E26;
extern bool DAT_00B77E27;
extern int Frame;
bool FUN_007947B0(void);
int __fastcall Negotiate_Scenario_Transfer(bool go);
void __fastcall FUN_0069BD60(SessionClass * session);
unsigned char __fastcall FUN_00793D20(int partner, int tag, int retries);
void FUN_00793BB0(void);

// FUNCTION: GAMEMD 0x007947B0
bool FUN_007947B0(void)
{
	CDTimerClass<SystemTimerClass> timer;
	int tag = -1;
	int partner;
	Session.KeepAliveCount = 0;
	memset(Session.unknown_2F4C, 0, sizeof(Session.unknown_2F4C));
	g_SuspendChatPump = 0;
	if (!Session.NumConnectionPairs) {
		return false;
	}
	FUN_0069BD60(&Session);
	FUN_007931F0(0);
	FUN_004068E0("Session.NumConnectionPairs = %d\n", Session.NumConnectionPairs);
	for (int i = 0; i < Session.NumConnectionPairs; i++) {
		int first = Session.ConnectionPairs[i][0];
		int second = Session.ConnectionPairs[i][1];
		partner = -1;
		NodeNameType * one = Session.Slots[first];
		NodeNameType * two = Session.Slots[second];
		char const * first_name = FUN_007350C0(one->Name);
		FUN_004068E0("Connecting pair %d, %s and %s\n", i, first_name, FUN_007350C0(two->Name));
		char const * name = FUN_007350C0(Session.Slots[first]->Name);
		if (stricmp(name, g_NickName.get()) == 0) {
			partner = second;
			tag = i * 2 + 1;
			Session.ConnectionResults[i * 2] = 11;
		}
		name = FUN_007350C0(Session.Slots[second]->Name);
		if (stricmp(name, g_NickName.get()) == 0) {
			partner = first;
			tag = i * 2;
			Session.ConnectionResults[i * 2 + 1] = 11;
		}
		if (partner != -1) {
			int retries;
			if (FirewallHelper.Rank_B(Session.PlayerFirewall[first]) > FirewallHelper.Rank_B(Session.PlayerFirewall[second])) {
				retries = FirewallHelper.Rank_B(Session.PlayerFirewall[first]);
			} else {
				retries = FirewallHelper.Rank_B(Session.PlayerFirewall[second]);
			}
			FUN_004068E0("About to enter Start_Talking_To, retries = %d", retries);
			if (!FUN_00793D20(partner, tag, retries)) {
				FUN_004068E0("Something went horribly wrong\n");
				return false;
			}
		} else {
			FUN_004068E0("Waiting for other guys to connect\n");
			timer = CDTimerClass<SystemTimerClass>(960);
			int best = 0;
			do {
				Sleep(5);
				WOL_Wait_Callback();
				FUN_00793BB0();
				int res1 = Session.ConnectionResults[i * 2];
				int res2 = Session.ConnectionResults[i * 2 + 1];
				if ((res1 == 11 || res1 == 0) && (res2 == 11 || res2 == 0)) {
					FUN_004068E0("Got both results - woohoo!\n");
					break;
				}
				if (res1 >= 1 && res1 < 11 && res1 - 1 > best) {
					FUN_004068E0("Connecting player 1 is trying again - try_now = %d\n", res1 - 1);
					best = res1 - 1;
					timer = CDTimerClass<SystemTimerClass>(840);
				}
				if (res2 >= 1 && res2 < 11 && res2 - 1 > best) {
					FUN_004068E0("Connecting player 2 is trying again - try_now = %d\n", res2 - 1);
					best = res2 - 1;
					timer = CDTimerClass<SystemTimerClass>(840);
				}
			} while (timer.Value() != 0);
		}
		if (Session.ConnectionResults[i * 2] != 11) {
			FUN_004068E0("res1 != CONNRESULT_CONNECTED\n");
			return false;
		}
		if (Session.ConnectionResults[i * 2 + 1] != 11) {
			FUN_004068E0("res2 != CONNRESULT_CONNECTED\n");
			return false;
		}
		FUN_004068E0("Connection good, next pair\n");
	}
	FUN_004068E0("Everyones happy\n");
	return true;
}

// FUNCTION: GAMEMD 0x00794BA0
void Start_Game_Now(void)
{
	FUN_004068E0("Start_Game_Now\n");
	DAT_00B77E26 = true;
	if (g_UserList.length() == 0) {
		g_GameStarted = 0;
		if (FUN_0077CF40()) {
			FUN_0079FC10(11, -1, 0);
		}
		g_GameStartRequested = 0;
		FUN_0052F3F0();
		DAT_00B77E27 = false;
		DAT_00B77E26 = false;
		return;
	}

	PacketTransport->Discard_In_Buffers();
	PacketTransport->Discard_Out_Buffers();
	memset(DAT_00B73BB4, -1, 8 * sizeof(int));
	FUN_00793250();
	for (int wait = 0; wait < 100; wait++) {
		Call_Back();
	}
	Ipx.Global_Channel_Op_44(300);

	bool verified = FUN_007947B0();
	if (!verified) {
		g_SuspendChatPump = 0;
		if (!Is_Local_Host()) {
			FUN_004068E0("Failed to verify connection paths\n");
		}
	}

	int result = Negotiate_Scenario_Transfer(verified);
	if (result) {
		FUN_004068E0("Negotiate_Scenario_Transfer failed\n");
		if (result == 2) {
			ODMessageBox(StringTable::LoadString("TXT_SCENARIOS_DO_NOT_MATCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x4F44), 0, FUN_0077D710, 0);
		}
		FUN_00793440();
		g_GameStarted = 0;
		if (FUN_0077CF40()) {
			FUN_0079FC10(11, -1, 0);
		}
		g_GameStartRequested = 0;
		FUN_00793490();
		FUN_0052F3F0();
		DAT_00B77E27 = false;
		DAT_00B77E26 = false;
		DAT_00B77DC1 = 1;
		FUN_004068E0("setting EV_EXIT\n");
		SetEvent(g_WaitEventHandles[6]);
		FUN_004068E0("Request channel leave in game start after scenario mismatch\n");
		g_pChat->RequestChannelLeave();
		if (Session.WOLWelcomeChoice == 1 || Session.WOLWelcomeChoice == 2 || Session.WOLWelcomeChoice == 3) {
			FUN_0078ABF0(true, StringTable::LoadString("TXT_AWAY_QUICKMATCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x4F69));
		} else {
			FUN_0078ABF0(false, DAT_00887734);
		}
		return;
	}

	FUN_004068E0("Negotiate_Scenario_Transfer succeeded\n");
	for (int p = 0; p < Session.Players.Count(); p++) {
		if (Session.PlayerPorts[p] == 0) {
			Session.PlayerPorts[p] = UDPInterfaceClass::UDPListenPort;
		}
	}
	User * host = Get_Channel_Host();
	if (host) {
		for (int h = 0; h < Session.Players.Count(); h++) {
			if (stricmp(FUN_007350C0(Session.Players[h]->Name), (char *)host->name) == 0) {
				Session.PacketRouterPort = Session.PlayerPorts[h];
				break;
			}
		}
	}

	FUN_004068E0("Closing Verify Connections Dialog\n");
	FUN_00793440();
	FUN_004068E0("Closed Verify Connections Dialog\n");

	if (!(Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3)) {
		Session.House = 0;
	}

	Session.FrameSendRate = 5;
	switch (Session.Options.GameSpeed) {
		case 0:
			Session.MaxAhead = 40;
			Session.PrecalcDesiredFrameRate = 60;
			Session.FrameSendRate = 10;
			break;
		case 1:
			Session.MaxAhead = 40;
			Session.PrecalcDesiredFrameRate = 45;
			Session.FrameSendRate = 10;
			break;
		case 2:
			Session.MaxAhead = 30;
			Session.PrecalcDesiredFrameRate = 30;
			break;
		case 3:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 20;
			break;
		case 4:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 15;
			break;
		case 5:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 12;
			break;
		default:
			Session.MaxAhead = 10;
			Session.PrecalcDesiredFrameRate = 10;
			break;
	}
	if (Session.UsePacketRouter) {
		Session.MaxAhead += 20;
	}
	Session.PrecalcMaxAhead = 0;
	if (!Is_Channel_Owner()) {
		Session.PrecalcDesiredFrameRate = 0;
	}

	int worst = 0;
	for (int q = 0; q < 8; q++) {
		if (WestwoodOnline_PingTimes[q] > worst) {
			worst = WestwoodOnline_PingTimes[q];
		}
	}
	if (Session.UsePacketRouter) {
		worst *= 2;
	}

	Session.LatencyFudge = 1;
	if (worst != 0 && worst != 1000) {
		FUN_004068E0("Max ping is %d ms\n", worst);
		if (worst > 1000) {
			Session.LatencyFudge = 2;
		} else if (worst < 600) {
			Session.LatencyFudge = 1;
		}
		int ticks = 60 * worst / 1000;
		ticks += 120 * ticks / 100;
		Ipx.Set_Timing(ticks, (unsigned int)-1, ticks * 8);
		if (Is_Channel_Owner()) {
			unsigned ahead;
			switch (Session.LatencyFudge) {
				case 0:
				case 1:
					ahead = Session.MaxAhead;
					break;
				case 2:
					ahead = (Session.MaxAhead / 2) + Session.MaxAhead;
					break;
				case 3:
					ahead = Session.MaxAhead * 2;
					break;
				default:
					ahead = Session.PrecalcMaxAhead;
					break;
			}
			ahead = 10 * (ahead / 10);
			Session.PrecalcMaxAhead = ahead;
		}
	} else {
		FUN_004068E0("Ping times not available - Using defaults\n");
		Session.PrecalcMaxAhead = 10 * (Session.MaxAhead / 10);
		if ((unsigned)Session.PrecalcMaxAhead <= 10) {
			Session.PrecalcMaxAhead = 10;
		}
	}

	FUN_004068E0("PrecalcMaxAhead is %d\n", Session.PrecalcMaxAhead);
	FUN_004068E0("PrecalcDesiredFrameRate is %d\n", Session.PrecalcDesiredFrameRate);
	FUN_004068E0("LatencyFudge is %d\n", Session.LatencyFudge);
	FUN_004068E0("FrameSendRate is %d\n", Session.FrameSendRate);
	Session.MaxMaxAhead = Session.MaxAhead;
	memset(Session.ConnectionStats, 0, sizeof(Session.ConnectionStats));
	UDPInterfaceClass::UDPListenPort = 1234;
	Options.GameSpeed = Session.Options.GameSpeed;
	DAT_00841F34 = 1235;
	DAT_00841F38 = 1236;
	DAT_00841F3C = 1237;
	Frame = 0;
	if (Session.UsePacketRouter) {
		OwnerTalk.Wait_For_Router_Shutdown(2000, true);
	}
	WestwoodOnline_Clan1_Players[0] = 0;
	WestwoodOnline_Clan2_Players[0] = 0;

	if (FUN_0077D970()) {
		User * user = NULL;
		int squad1 = 0;
		int squad2 = 0;
		for (int i = 0; i < g_UserList.length(); i++) {
			g_UserList.getPointer(&user, i);
			if (squad1 == 0 || squad1 == (int)user->squadID) {
				strcat(WestwoodOnline_Clan1_Players, (char *)user->name);
				strcat(WestwoodOnline_Clan1_Players, ",");
				squad1 = user->squadID;
			} else if (squad2 == 0 || squad2 == (int)user->squadID) {
				strcat(WestwoodOnline_Clan2_Players, (char *)user->name);
				strcat(WestwoodOnline_Clan2_Players, ",");
				squad2 = user->squadID;
			}
		}
	}
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		User * user = NULL;
		for (int i = 0; i < g_UserList.length(); i++) {
			g_UserList.getPointer(&user, i);
			if (g_UserInfo[i].House == 0) {
				strcat(WestwoodOnline_Clan1_Players, (char *)user->name);
				strcat(WestwoodOnline_Clan1_Players, ",");
			} else {
				strcat(WestwoodOnline_Clan2_Players, (char *)user->name);
				strcat(WestwoodOnline_Clan2_Players, ",");
			}
		}
	}

	FUN_004068E0("setting EV_STARTGAME\n");
	SetEvent(g_WaitEventHandles[7]);
	if (FUN_0077CF40()) {
		FUN_0079FC10(11, -1, 0);
	}
	DAT_00B77DC1 = 1;
	FUN_004068E0("Request channel leave in game start\n");
	g_pChat->RequestChannelLeave();
	FUN_005E7460();
	DAT_00B77E27 = false;
	DAT_00B77E26 = false;
	FUN_004068E0("Leaving Start_Game_Now\n");
}

// FUNCTION: GAMEMD 0x00795650
bool __fastcall FUN_00795650(IPXAddressClass * address)
{
	enum {
		ESTABLISHED = 5
	};

	enum {
		tcpConnState = 1,
		tcpConnLocalAddress,
		tcpConnLocalPort,
		tcpConnRemAddress,
		tcpConnRemPort
	};

	char server_name[128];
	unsigned char server_address[4];
	unsigned char remote_address[4];
	HANDLE trap_handle;
	AsnObjectIdentifier first_supported_region;
	DynamicVectorClass<tConnInfoStruct *> connection_list;
	int last_field;
	int index;
	AsnInteger error_status;
	AsnInteger error_index;
	int conn_entry_type_index;
	int conn_entry_type;
	bool found;
	unsigned int server_port = 0;
	NetNumType net;
	NetNodeType node;

	if (g_CurrentServerIndex < 1 || g_CurrentServerIndex >= g_Servers.length()) {
		return(false);
	}

	FUN_004068E0("Finding local address used to talk to the chat server\n");

	Server * server = NULL;
	g_Servers.getPointer(&server, g_CurrentServerIndex);
	memset(server_name, 0, sizeof(server_name));
	strncpy(server_name, (char *)server->conndata + 4, sizeof(server_name));

	char * semi_colon_ptr = strchr(server_name, ';');
	if (semi_colon_ptr) {
		*semi_colon_ptr = 0;
		sscanf(semi_colon_ptr + 1, "%d", &server_port);
	} else {
		FUN_004068E0("Failed to parse server name\n");
		return(false);
	}
	FUN_004068E0("Current chat server name is %s\n", server_name);
	FUN_004068E0("Chat server port is %d\n", server_port);

	FUN_004068E0("About to call gethostbyname\n");
	hostent * host_info = FUN_007C8980(server_name);

	if (!host_info) {
		FUN_004068E0("gethostbyname failed! Error code %d\n", FUN_007C897A());
		return(false);
	}

	memcpy(server_address, &host_info->h_addr_list[0][0], 4);
	unsigned long temp = *((unsigned long *)(&server_address[0]));
	temp = FUN_007C896E(temp);
	*((unsigned long *)(&server_address[0])) = temp;

	FUN_004068E0("Host address is %d.%d.%d.%d\n", server_address[3], server_address[2], server_address[1], server_address[0]);

	FUN_004068E0("About to load INETMIB1.DLL\n");

	HINSTANCE mib_ii_dll = LoadLibraryA("inetmib1.dll");
	if (mib_ii_dll == NULL) {
		FUN_004068E0("Failed to load INETMIB1.DLL\n");
		return(false);
	}

	FUN_004068E0("About to load SNMPAPI.DLL\n");

	HINSTANCE snmpapi_dll = LoadLibraryA("snmpapi.dll");
	if (snmpapi_dll == NULL) {
		FUN_004068E0("Failed to load SNMPAPI.DLL\n");
		FreeLibrary(mib_ii_dll);
		return(false);
	}

	SnmpExtensionInitPtr = (BOOL (__stdcall *)(DWORD, HANDLE *, AsnObjectIdentifier *))GetProcAddress(mib_ii_dll, "SnmpExtensionInit");
	SnmpExtensionQueryPtr = (BOOL (__stdcall *)(BYTE, SnmpVarBindList *, AsnInteger32 *, AsnInteger32 *))GetProcAddress(mib_ii_dll, "SnmpExtensionQuery");
	SnmpUtilMemAllocPtr = (LPVOID (__stdcall *)(DWORD))GetProcAddress(snmpapi_dll, "SnmpUtilMemAlloc");
	SnmpUtilMemFreePtr = (VOID (__stdcall *)(LPVOID))GetProcAddress(snmpapi_dll, "SnmpUtilMemFree");
	if (SnmpExtensionInitPtr == NULL || SnmpExtensionQueryPtr == NULL || SnmpUtilMemAllocPtr == NULL || SnmpUtilMemFreePtr == NULL) {
		FUN_004068E0("Failed to get proc addresses for linked functions\n");
		FreeLibrary(snmpapi_dll);
		FreeLibrary(mib_ii_dll);
		return(false);
	}

	SnmpVarBindList * bind_list_ptr = (SnmpVarBindList *)SnmpUtilMemAllocPtr(sizeof(SnmpVarBindList));
	SnmpVarBind * bind_ptr = (SnmpVarBind *)SnmpUtilMemAllocPtr(sizeof(SnmpVarBind));

	FUN_004068E0("About to init INETMIB1.DLL\n");
	int ok = SnmpExtensionInitPtr(GetTickCount(), &trap_handle, &first_supported_region);

	if (!ok) {
		FUN_004068E0("Failed to init the .dll\n");
		SnmpUtilMemFreePtr(bind_list_ptr);
		SnmpUtilMemFreePtr(bind_ptr);
		FreeLibrary(snmpapi_dll);
		FreeLibrary(mib_ii_dll);
		return(false);
	}

	unsigned int mib_ii_name[] = {1,3,6,1,2,1,6,13,1,1};
	unsigned int * mib_ii_name_ptr = (unsigned int *)SnmpUtilMemAllocPtr(sizeof(mib_ii_name));
	memcpy(mib_ii_name_ptr, mib_ii_name, sizeof(mib_ii_name));

	conn_entry_type_index = ARRAY_SIZE(mib_ii_name) - 1;

	bind_ptr->name.idLength = ARRAY_SIZE(mib_ii_name);
	bind_ptr->name.ids = mib_ii_name;
	bind_list_ptr->list = bind_ptr;
	bind_list_ptr->len = 1;

	last_field = 1;
	index = 0;

	while (true) {

		if (!SnmpExtensionQueryPtr(SNMP_PDU_GETNEXT, bind_list_ptr, &error_status, &error_index)) {
			FUN_004068E0("SnmpExtensionQuery returned false\n");
			SnmpUtilMemFreePtr(bind_list_ptr);
			SnmpUtilMemFreePtr(bind_ptr);
			FreeLibrary(snmpapi_dll);
			FreeLibrary(mib_ii_dll);
			return(false);
		}

		if (bind_ptr->name.idLength < ARRAY_SIZE(mib_ii_name)) {
			break;
		}

		conn_entry_type = bind_ptr->name.ids[conn_entry_type_index];

		if (last_field != conn_entry_type) {
			index = 0;
			last_field = conn_entry_type;
		}

		switch (conn_entry_type) {

			case tcpConnState:
			{
				tConnInfoStruct * new_conn = new tConnInfoStruct;
				new_conn->State = bind_ptr->value.asnValue.number;
				connection_list.Add(new_conn);
				break;
			}

			case tcpConnLocalAddress:
				connection_list[index]->LocalIP = *((unsigned long *)bind_ptr->value.asnValue.address.stream);
				index++;
				break;

			case tcpConnLocalPort:
				connection_list[index]->LocalPort = (unsigned short)bind_ptr->value.asnValue.number;
				index++;
				break;

			case tcpConnRemAddress:
				connection_list[index]->RemoteIP = *((unsigned long *)bind_ptr->value.asnValue.address.stream);
				index++;
				break;

			case tcpConnRemPort:
				connection_list[index]->RemotePort = (unsigned short)bind_ptr->value.asnValue.number;
				index++;
				break;
		}
	}

	SnmpUtilMemFreePtr(bind_list_ptr);
	SnmpUtilMemFreePtr(bind_ptr);
	SnmpUtilMemFreePtr(mib_ii_name_ptr);

	FUN_004068E0("Got %d connections in list, parsing...\n", connection_list.Count());

	found = false;
	while (connection_list.Count()) {
		tConnInfoStruct * connection = connection_list[0];

		temp = FUN_007C896E(connection->RemoteIP);
		memcpy(remote_address, (unsigned char *)&temp, 4);

		if (!found && memcmp(remote_address, server_address, 4) == 0) {
			FUN_004068E0("Found connection with same remote address as server\n");

			if (server_port == 0 || server_port == (unsigned int)connection->RemotePort) {

				FUN_004068E0("Connection has same port\n");
				if (connection->State == ESTABLISHED) {
					FUN_004068E0("Connection is ESTABLISHED\n");
					memcpy(node, &connection->LocalIP, 4);
					address->Set_Address(net, node, connection->LocalPort);
					found = true;
				} else {
					FUN_004068E0("Connection is not ESTABLISHED - skipping\n");
				}
			} else {
				FUN_004068E0("Connection has different port. Port is %d, looking for %d\n", connection->RemotePort, server_port);
			}
		}

		delete connection_list[0];
		connection_list.Delete_Index(0);
	}

	if (found) {
		FUN_004068E0("Using address %s to talk to chat server\n", address->As_String(NULL));
	}

	FreeLibrary(snmpapi_dll);
	FreeLibrary(mib_ii_dll);

	return(found);
}

extern bool g_RecievingPreview;

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

bool __fastcall FUN_006951F0(char const * name, unsigned int length, char const * digest, bool official, bool flag);
bool __fastcall FUN_00693B70(char * name, int type, bool flag);
bool __fastcall FUN_00694760(char * name, int type, bool a, bool b);
void FUN_0058BC20(void);

// FUNCTION: GAMEMD 0x00795CD0
int __fastcall Negotiate_Scenario_Transfer(bool go)
{
	GlobalPacketType packet;
	int responses[MAX_PLAYERS];
	bool no_scenario = false;
	bool dont_go = !go;
	int result = 0;

	Ipx.Global_Channel_Clear_FieldC8();
	Ipx.Set_Timing(50, (unsigned int)-1, 5000);

	if (Is_Local_Host()) {
		memset(&packet, 0, sizeof(packet));

		memset(responses, 0, sizeof(responses));
		int num_responses = 0;
		bool send_scenario = false;
		FUN_004068E0("About to wait for 'GO' response.\n");

		CDTimerClass<SystemTimerClass> response_timer;
		response_timer = 900;
		if (dont_go) {
			response_timer = 300;
		}

		do {
			Call_Back();
			if (Ipx.Get_Global_Message(&Session.GPacket, &Session.GPacketlen, &Session.GAddress, &Session.GProductID) && Session.GProductID == IPXGlobalConnClass::COMMAND_AND_CONQUER4) {
				FUN_004068E0("Packet is YR packet\n");

				for (int i = 1; i < Session.Players.Count(); i++) {
					if (Session.Players[i]->Address.FUN_0053EF30(Session.GAddress) && stricmp(FUN_007350C0(Session.Players[i]->Name), Session.GPacket.Name) == 0) {
						if (!responses[i]) {
							if (Session.GPacket.Command == NET_REQ_SCENARIO) {
								FUN_004068E0("Packet type is NET_REQ_SCENARIO\n");
								responses[i] = Session.GPacket.Command;
								send_scenario = true;
								num_responses++;
							}
							if (Session.GPacket.Command == NET_READY_TO_GO) {
								FUN_004068E0("Packet type is NET_READY_TO_GO\n");
								responses[i] = Session.GPacket.Command;
								num_responses++;
							}
							if (Session.GPacket.Command == 42) {
								FUN_004068E0("Packet type is NET_DONT_GO\n");
								responses[i] = Session.GPacket.Command;
								num_responses++;
								dont_go = true;
								FUN_00793460(i, 0);
							}
							if (Session.GPacket.Command == NET_NO_SCENARIO) {
								FUN_004068E0("Packet type is NET_NO_SCENARIO\n");
								responses[i] = Session.GPacket.Command;
								result = 2;
								dont_go = true;
								no_scenario = true;
								num_responses++;
							}
						}
					}
				}
			}
		} while (num_responses < Session.Players.Count() - 1 && response_timer);

		if (no_scenario) {
			result = 2;
		}

		if (!dont_go && !no_scenario && send_scenario && num_responses == Session.Players.Count() - 1) {
			while (FUN_007757E0(NULL, 0) == 1) {
			}
			memset(Session.ScenarioRequests, 0, sizeof(Session.ScenarioRequests));
			Session.RequestCount = 0;
			for (int i = 1; i < Session.Players.Count(); i++) {
				if (responses[i] == NET_REQ_SCENARIO) {
					Session.ScenarioRequests[Session.RequestCount++] = (char)i;
				}
			}
			if (!FUN_00694760(Scen->ScenarioName, GAME_INTERNET, false, true)) {
				result = 2;
				dont_go = false;
			}
			if (Session.Is_Random_Map()) {
				if (!FUN_00694760("RandMap.img", GAME_INTERNET, false, true)) {
					result = 2;
					dont_go = false;
				}
			}
		}

		memset(&packet, 0, sizeof(packet));
		if (!dont_go && num_responses == Session.Players.Count() - 1 && !no_scenario) {
			packet.Command = NET_GO;
		} else {
			packet.Command = (NetCommandType)42;
			dont_go = true;
			if (result == 0) {
				result = 1;
			}
		}
		OwnerDraw::WideToCharString(packet.Name, Session.Players[0]->Name, -1);

		for (int j = 1; j < Session.Players.Count(); j++) {
			CDTimerClass<SystemTimerClass> timer = 600;
			if (dont_go) {
				timer = 300;
			}
			do {
				Call_Back();
			} while (!Ipx.Send_Global_Message_To_Name(&packet, 0x2f, 1, Session.Players[j]->Name) && timer);
		}

	} else {

		if (FUN_006951F0(Session.ScenarioFileName, Session.ScenarioFileLength, Session.ScenarioDigest, Session.ScenarioIsOfficial, true)) {
			memset(&packet, 0, sizeof(packet));
			packet.Command = NET_READY_TO_GO;
			if (dont_go) {
				packet.Command = (NetCommandType)42;
			}
			OwnerDraw::WideToCharString(packet.Name, Session.Players[0]->Name, -1);
			User * host = Get_Channel_Host();
			if (host) {
				Ipx.Send_Global_Message_To_Name(&packet, 0x2f, 1, FUN_00735120((char *)host->name));
			}

			CDTimerClass<SystemTimerClass> timer;
			timer = 1200;
			if (dont_go) {
				timer = 300;
			}
			while (Ipx.Global_Num_Send() > 0 && timer) {
				Call_Back();
			}

		} else {
			if ((Session.WOLWelcomeChoice != 3 && (Session.WOLWelcomeChoice == 1 || Session.WOLWelcomeChoice == 2)) || (Session.ScenarioIsOfficial && !Session.Is_Random_Map())) {
				memset(&packet, 0, sizeof(packet));
				packet.Command = NET_NO_SCENARIO;
				User * host = Get_Channel_Host();
				if (host) {
					Ipx.Send_Global_Message_To_Name(&packet, 0x2f, 1, FUN_00735120((char *)host->name));
				}
				FUN_004068E0("Sent NET_NO_SCENARIO\n");

				CDTimerClass<SystemTimerClass> timer;
				timer = 1200;
				while (Ipx.Global_Num_Send() > 0 && timer) {
					Call_Back();
				}
				result = 2;
				dont_go = true;
			} else {
				while (FUN_007757E0(NULL, 0) == 1) {
				}
				FUN_0058BC20();
				if (!FUN_00693B70(Session.ScenarioFileName, GAME_INTERNET, true)) {
					result = 2;
					dont_go = true;
				} else if (!dont_go && Session.Is_Random_Map()) {
					char preview_name[16];
					if (!FUN_00693B70(preview_name, GAME_INTERNET, true)) {
						result = 2;
						dont_go = true;
					}
				}
			}
		}

		strcpy(Scen->ScenarioName, Session.ScenarioFileName);
		FUN_004068E0("About to wait for 'GO' from host.\n");

		CDTimerClass<SystemTimerClass> response_timer;
		response_timer = 1200;
		if (dont_go) {
			response_timer = 300;
		}

		do {
			Call_Back();
			if (Ipx.Get_Global_Message(&Session.GPacket, &Session.GPacketlen, &Session.GAddress, &Session.GProductID) && Session.GProductID == IPXGlobalConnClass::COMMAND_AND_CONQUER4) {
				FUN_004068E0("Packet is YR packet\n");
				User * host = Get_Channel_Host();
				if (host && stricmp(Session.GPacket.Name, (char *)host->name) == 0) {
					if (Session.GPacket.Command == NET_GO) {
						FUN_004068E0("Got NET_GO from host\n");
						goto done;
					}
					if (Session.GPacket.Command == 42) {
						FUN_004068E0("Got NET_DONT_GO from host\n");
						dont_go = true;
						int k;
						for (k = 0; k < Session.Players.Count(); k++) {
							if (stricmp(FUN_007350C0(Session.Players[k]->Name), (char *)host->name) == 0) {
								FUN_00793460(k, 3);
								break;
							}
						}
						break;
					}
				}
			}
		} while (response_timer);

		if (!response_timer) {
			dont_go = true;
		}
	}

done:
	Ipx.Set_Timing(Ipx.Global_Response_Time() + 2, (unsigned int)-1, MAX(2 * TIMER_SECOND, Ipx.Global_Response_Time() * 8));
	Ipx.Global_Channel_Set_FieldC8();

	if (result != 2 && dont_go) {
		result = 1;
	}
	return(result);
}

// FUNCTION: GAMEMD 0x00796710
bool Handle_Preview_Download(void)
{
	GlobalPacketType packet;
	int packet_len;
	unsigned short product_id;
	IPXAddressClass sender_address;

	if (g_CurrentChannel.type != 0 && g_UserList.length() >= 2 && !Is_Channel_Owner() && !g_GameStarted && Ipx.Global_Num_Receive()) {
		Call_Back();
		packet_len = sizeof(packet);
		if (Ipx.Global_Channel_Op(0, (int)&packet, (int)&packet_len, (int)&sender_address, (int)&product_id, 0) && packet.Command == NET_PREVIEW_MODE) {
			HWND dialog = WS_Find_Dialog(0xC9);
			if (dialog) {
				ShowWindow(GetDlgItem(dialog, 0x468), SW_SHOWNORMAL);
			}
			User user;
			g_UserList.get(user, 0);
			user.next = NULL;
			User_List_To_Session_Players(&user);
			if (Ipx.Get_Global_Message(&packet, &packet_len, &sender_address, &product_id) && packet.Command == NET_PREVIEW_MODE && sender_address.FUN_0053EF30(Session.HostAddress) && product_id == IPXGlobalConnClass::COMMAND_AND_CONQUER4) {
				User * host = Get_Channel_Host();
				if (host && stricmp(packet.Name, (char *)host->name) == 0) {
					FUN_004068E0("Received NET_PREVIEW_MODE packet from host\n");
					Ipx.Set_Timing(50, -1, 5000);
					g_RecievingPreview = true;
					Receive_Random_Map_Preview();
					g_RecievingPreview = false;
					Ipx.Set_Timing(30, -1, 600);
					return true;
				}
			}
		}
	}
	return false;
}

static inline int Current_Channel_Type(void)
{
	return g_CurrentChannel.type;
}

// FUNCTION: GAMEMD 0x00796960
void Poke_The_Host(void)
{
	if (Session.Type != GAME_INTERNET) {
		IPXAddressClass address;
		if (g_UserList.length() >= 2 && !Is_Local_Host() && !g_GameStarted && Current_Channel_Type() != 0) {
			User user;
			g_UserList.get(user, 0);
			user.next = NULL;
			User_List_To_Session_Players(&user);
			Call_Back();
			Ipx.Set_Timing(50, -1, 5000);
			FUN_004068E0("Poke_The_Host - Host address is %s\n", Session.HostAddress.As_String(NULL));
			GlobalPacketType packet;
			memset(&packet, 0, sizeof(packet));
			packet.Command = NET_PING;
			OwnerDraw::WideToCharString(packet.Name, Session.Players[0]->Name, -1);
			User * host = Get_Channel_Host();
			if (host) {
				Ipx.Send_Global_Message_To_Name(&packet, sizeof(packet), 1, FUN_00735120((char *)host->name));
			}
			Call_Back();
		}
	}
}

// FUNCTION: GAMEMD 0x00796B20
void FUN_00796B20(void)
{
}

BOOL CALLBACK Select_WDT_Location_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00796B30
int Select_WDT_Location(void)
{
	SimpleWonlineDialogControl dialog;
	int result = Run_Dialog_Control(0xFE, Select_WDT_Location_Proc, &dialog);
	if (result < 0) {
		return 0;
	}
	return result;
}

void __fastcall FUN_006223C0(HWND window);

// FUNCTION: GAMEMD 0x00796B90
BOOL CALLBACK Select_WDT_Location_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG) {
		if (lparam) {
			SetWindowLong(window, 8, ((int *)lparam)[1]);
		}
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	int count;
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return FALSE;

		case WM_NCDESTROY:
			FUN_006223C0(window);
			return FALSE;

		case 0x497:
			{
				SendDlgItemMessage(window, 0x3F0, LB_RESETCONTENT, 0, 0);
				count = 0;
				g_pChat->GetLocaleCount(&count);
				if (count > 0) {
					HWND list = GetDlgItem(window, 0x3F0);
					for (int i = count - 1; i >= 0; i--) {
						WideWstring name(FUN_00780A00(i));
						FUN_004068E0("Adding country: %hs\n", name.get());
						int item;
						if (i < 2) {
							item = SendMessage(list, 0x4CC, 0, (LPARAM)name.get());
						} else {
							item = SendMessage(list, 0x4CD, 0, (LPARAM)name.get());
						}
						SendMessage(list, LB_SETITEMDATA, item, i);
					}
					SendMessage(list, LB_SETCURSEL, -1, 0);
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					{
						HWND list = GetDlgItem(window, 0x3F0);
						int data = 0;
						int index = SendMessage(list, LB_GETCURSEL, 0, 0);
						if (index != -1) {
							data = SendMessage(list, LB_GETITEMDATA, index, 0);
						}
						control->F1(window, data);
					}
					return FALSE;

				case IDCANCEL:
					control->F1(window, 0);
					return FALSE;

				case 0x3F0:
					if (HIWORD(wparam) == LBN_SELCHANGE) {
						EnableWindow(GetDlgItem(window, LOWORD(wparam) - 0x3EF), SendMessage((HWND)lparam, LB_GETCURSEL, 0, 0) != -1);
					}
					return FALSE;
			}
			break;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x00796DE0
void FUN_00796DE0(void)
{
	NodeNameType * who = new NodeNameType;
	FUN_00735060(who->Name, "Ol Cruddy", -1);
	who->Player.SquadID = -1;
	who->Player.House = 0;
	who->Player.Color = 3;
	NetNumType netnum;
	NetNodeType net;
	memset(&netnum, 0, sizeof(netnum));
	net[5] = 0;
	net[4] = 0;
	net[3] = 0x52;
	net[2] = 0x51;
	net[1] = 2;
	net[0] = 10;
	who->Address.Set_Address(netnum, net, 0);
	Session.Players.Add(who);
	WestwoodOnline_PingTimes[Session.NumPlayers] = 100;
	Session.NumPlayers++;
}

// FUNCTION: GAMEMD 0x00796ED0
void FUN_00796ED0(void)
{
	NodeNameType * who = new NodeNameType;
	FUN_00735060(who->Name, "Potato", -1);
	who->Player.SquadID = -1;
	who->Player.House = 0;
	who->Player.Color = 0;
	NetNumType netnum;
	NetNodeType net;
	memset(&netnum, 0, sizeof(netnum));
	net[5] = 0;
	net[4] = 0;
	net[3] = 0x61;
	net[2] = 0x14;
	net[1] = 2;
	net[0] = 10;
	who->Address.Set_Address(netnum, net, 0);
	Session.Players.Add(who);
	WestwoodOnline_PingTimes[Session.NumPlayers] = 100;
	Session.NumPlayers++;
}

// FUNCTION: GAMEMD 0x00796FC0
void FUN_00796FC0(void)
{
	NodeNameType * who = new NodeNameType;
	FUN_00735060(who->Name, "PotatoToo", -1);
	who->Player.SquadID = -1;
	who->Player.House = 1;
	who->Player.Color = 2;
	NetNumType netnum;
	NetNodeType net;
	memset(&netnum, 0, sizeof(netnum));
	net[5] = 0;
	net[4] = 0;
	net[3] = 0xFA;
	net[2] = 0x50;
	net[1] = 2;
	net[0] = 10;
	who->Address.Set_Address(netnum, net, 0);
	Session.Players.Add(who);
	WestwoodOnline_PingTimes[Session.NumPlayers] = 100;
	Session.NumPlayers++;
	Session.HostAddress = who->Address;
}

namespace Game {
	namespace Network {
		extern int WOLGameID;
		extern int Tournament;
	}
}
extern int WestwoodOnline_GameSKU_TS;
extern char WestwoodOnline_LoginName[];
void __fastcall FUN_005C6120(DynamicVectorClass<NodeNameType *> * vec);
void __fastcall FUN_005E7BF0(int slot);
void __fastcall FUN_005E7460(void);

// FUNCTION: GAMEMD 0x007970D0
void FUN_007970D0(void)
{
	char computer[0x3F8];
	unsigned long size;
	bool flag;

	Game::Network::WOLGameID = 0;
	WestwoodOnline_GameSKU_TS = g_GameSKU;
	Game::Network::Tournament = 0;
	strcpy(WestwoodOnline_LoginName, g_NickName.get());
	FUN_005C6120(&Session.Players);
	size = 0x3F7;
	GetComputerName(computer, &size);
	flag = false;
	if (stricmp(computer, "STEVE-T3") == 0) {
		FUN_00796FC0();
		FUN_00796DE0();
		FUN_00796ED0();
		flag = true;
	} else if (stricmp(computer, "STEVE-T2") == 0) {
		FUN_00796ED0();
		FUN_00796DE0();
		FUN_00796FC0();
	} else {
		FUN_00796DE0();
		FUN_00796ED0();
		FUN_00796FC0();
	}
	PacketTransport->Clear_Broadcast_Addresses();
	char address[32] = {"xxx.xxx.xxx.xxx"};
	NetNumType dummy;
	NetNodeType ipaddr;
	for (int p = 1; p < Session.Players.Count(); p++) {
		Session.Players[p]->Address.Get_Address(dummy, ipaddr);
		PacketTransport->Set_Broadcast_Address(address, 0);
	}
	g_GameStarted = 2;
	FUN_004068E0("Starting a %d player game.\n", Session.NumPlayers);
	FUN_00735060(Session.MasterPlayerName, "PotatoToo", -1);
	Session.MasterPlayerID = -1;
	Session.House = 0;
	Seed = 1;
	BuildLevel = 10;
	Session.Options.UnitCount = 10;
	Session.Options.Credits = 10000;
	Session.Options.FogOfWar = 0;
	Session.Options.BridgeDestruction = 1;
	Session.Options.Goodies = 1;
	Session.Options.MCVRedeploy = 1;
	Session.Options.AlliesAllowed = 1;
	Session.Options.HarvTruce = 0;
	Session.Options.Bases = 1;
	Session.Options.CTF = 0;
	Session.Options.AIPlayers = 0;
	Session.Options.AIDifficulty = (DiffType)0;
	Session.Options.ShortGame = 1;
	Session.Options.SWAllowed = 1;
	Session.Options.BuildOffAlly = 0;
	Session.Options.CrapEngineers = 1;
	Session.Options.ScenarioIndex = 1;
	Session.Options.GameSpeed = 0;
	FUN_005E7BF0(Session.Options.ScenarioIndex);
	Session.Options.GameSpeed = 2;
	Options.GameSpeed = 2;
	Scen->Scenario = Session.Options.ScenarioIndex;
	Session.FrameSendRate = 5;
	switch (Session.Options.GameSpeed) {
		case 0:
			Session.MaxAhead = 40;
			Session.PrecalcDesiredFrameRate = 60;
			Session.FrameSendRate = 10;
			break;
		case 1:
			Session.MaxAhead = 40;
			Session.PrecalcDesiredFrameRate = 45;
			Session.FrameSendRate = 10;
			break;
		case 2:
			Session.MaxAhead = 30;
			Session.PrecalcDesiredFrameRate = 30;
			break;
		case 3:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 20;
			break;
		case 4:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 15;
			break;
		case 5:
			Session.MaxAhead = 20;
			Session.PrecalcDesiredFrameRate = 12;
			break;
		default:
			Session.MaxAhead = 10;
			Session.PrecalcDesiredFrameRate = 10;
			break;
	}
	Session.PrecalcMaxAhead = 0;
	if (!flag) {
		Session.PrecalcDesiredFrameRate = 0;
	}
	int worst = 0;
	for (int q = 0; q < Session.Players.Count(); q++) {
		if (WestwoodOnline_PingTimes[q] > worst) {
			worst = WestwoodOnline_PingTimes[q];
		}
	}
	Session.LatencyFudge = 1;
	if (worst != 0 && worst != 1000) {
		FUN_004068E0("Max ping is %d ms\n", worst);
		if (worst > 1000) {
			Session.LatencyFudge = 2;
		} else if (worst < 600) {
			Session.LatencyFudge = 1;
		}
		int ticks = 60 * worst / 1000;
		ticks += 120 * ticks / 100;
		Ipx.Set_Timing(ticks, (unsigned int)-1, ticks * 8);
		if (flag) {
			Ipx.Set_Lobby_Timing(5, (unsigned int)-1, 0x78);
		}
		if (flag) {
			unsigned ahead;
			switch (Session.LatencyFudge) {
				case 0:
				case 1:
					ahead = Session.MaxAhead;
					break;
				case 2:
					ahead = (Session.MaxAhead / 2) + Session.MaxAhead;
					break;
				case 3:
					ahead = Session.MaxAhead * 2;
					break;
				default:
					ahead = Session.PrecalcMaxAhead;
					break;
			}
			ahead = 10 * (ahead / 10);
			Session.PrecalcMaxAhead = ahead;
		}
	} else {
		Session.PrecalcDesiredFrameRate = 0;
		FUN_004068E0("Ping times not available\n");
	}
	FUN_004068E0("PrecalcMaxAhead is %d\n", Session.PrecalcMaxAhead);
	FUN_004068E0("PrecalcDesiredFrameRate is %d\n", Session.PrecalcDesiredFrameRate);
	FUN_004068E0("LatencyFudge is %d\n", Session.LatencyFudge);
	FUN_004068E0("FrameSendRate is %d\n", Session.FrameSendRate);
	Session.MaxMaxAhead = Session.MaxAhead;
	memset(Session.ConnectionStats, 0, sizeof(Session.ConnectionStats));
	UDPInterfaceClass::UDPListenPort = 1234;
	Options.GameSpeed = Session.Options.GameSpeed;
	DAT_00841F34 = 1235;
	DAT_00841F38 = 1236;
	DAT_00841F3C = 1237;
	Frame = 0;
	WestwoodOnline_Clan1_Players[0] = 0;
	WestwoodOnline_Clan2_Players[0] = 0;
	FUN_005E7460();
}

int __fastcall FUN_00797640(int sku);

// FUNCTION: GAMEMD 0x007975E0
void FUN_007975E0(void)
{
	{
		WideWstring text(StringTable::LoadString("TXT_FETCHING_SERVLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5EEC));
		Show_Wait_Window(0x10, text, 0);
	}
	int sku = 0x2300;
	if (Session.WOLWelcomeChoice != 3) {
		sku = 0x2900;
	}
	FUN_00797640(sku);
}

extern int g_LastIgnoredStatus;

// FUNCTION: GAMEMD 0x00797640
int __fastcall FUN_00797640(int sku)
{
	char saved = g_IgnoreNetStatus;
	bool servers;
	bool done;
	DWORD index;
	int result;

	int version = g_LanguageCode | sku;
	g_IgnoreNetStatus = 1;
	g_Servers.setEmpty();
	g_CurrentServerIndex = 0;
	g_TargetServerIndex = 0;
	g_pChat->RequestServerList(version, 0x10001, "TibSun", "TibPass99", 0x1E);
	g_pChat->SetProductSKU(g_GameSKU);
	done = false;
	servers = false;
	while (true) {
		while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 50)) == WAIT_TIMEOUT) {
			WOL_Wait_Callback();
		}
		Reset_Wait_Event_At(index);
		if (index == 0) {
			result = 1;
			break;
		}
		if (index == 8) {
			result = 2;
			break;
		}
		if (index == 1) {
			done = true;
			if (servers) {
				result = 0;
				break;
			}
		} else if (index == 9) {
			bool fail = false;
			bool ok = false;
			switch (g_LastIgnoredStatus) {
				case 0x80040067:
				case 0x80040068:
				case 0x80040069:
				case 0x8004006A:
				case 0x8004006C:
				case 0x8004006D:
					ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5F45), 0, FUN_0077D710, 0);
					fail = true;
					break;
				case 0x4012F:
					servers = true;
					if (done) {
						ok = true;
					}
					break;
			}
			if (fail) {
				result = 1;
				break;
			}
			if (ok) {
				result = 0;
				break;
			}
		}
	}
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags = 0;
		FUN_007757E0(dialog, 0);
	}
	HWND top = FUN_00775B10();
	HWND item = GetDlgItem(top, 0x540);
	if (item) {
		SetFocus(item);
	}
	g_IgnoreNetStatus = saved;
	return result;
}

extern char DAT_00B76A2C[];

static inline void Close_Wait_Dialog(void)
{
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags = 0;
		FUN_007757E0(dialog, 0);
	}
	HWND top = FUN_00775B10();
	HWND item = GetDlgItem(top, 0x540);
	if (item) {
		SetFocus(item);
	}
}

// FUNCTION: GAMEMD 0x00797800
bool Request_WDT_Cycle(void)
{
	if (!DAT_00B76A2C) {
		ODMessageBox(StringTable::LoadString("TXT_WDT_NET_ERR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5F73), 0, FUN_0077D710, 0);
		return false;
	}
	for (int i = 0; i < 18; i++) {
		Reset_Wait_Event_At(i);
	}
	long result = g_pNetUtil->RequestWDTState(g_WDTHost, g_WDTPort, 3);
	FUN_004068E0("WDTHost is %s, port is %d\n", g_WDTHost, g_WDTPort);
	FUN_004068E0("Requesting current WDT cycle\n");
	if (result != 0) {
		ODMessageBox(StringTable::LoadString("TXT_WDT_NET_ERR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5F83), 0, FUN_0077D710, 0);
		return false;
	}
	{
		WideWstring text(StringTable::LoadString("TXT_FETCHING_WDT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5F87));
		Show_Wait_Window(0x10, text, 0);
	}
	DWORD index;
	while (true) {
		int timeout = 30000;
		index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100);
		while (index == WAIT_TIMEOUT) {
			timeout -= 100;
			if (timeout <= 0) {
				goto failed;
			}
			WOL_Wait_Callback();
			index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100);
		}
		Reset_Wait_Event_At(index);
		if (index == 0) {
			Close_Wait_Dialog();
			return false;
		}
		if (index == 5) {
			ODMessageBox(StringTable::LoadString("TXT_WDT_NET_ERR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5F9A), 0, FUN_0077D710, 0);
			Close_Wait_Dialog();
			return false;
		}
		if (index == 0x12) {
failed:
			ODMessageBox(StringTable::LoadString("TXT_WDT_NET_ERR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5FA0), 0, FUN_0077D710, 0);
			Close_Wait_Dialog();
			return false;
		}
		if (index == 0xB) {
			break;
		}
	}
	result = g_pNetUtil->RequestWDTState(g_WDTHost, g_WDTPort, 5);
	FUN_004068E0("Requesting Previous Cycle\n");
	if (result != 0) {
		Close_Wait_Dialog();
		return false;
	}
	while (true) {
		index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100);
		while (index == WAIT_TIMEOUT) {
			WOL_Wait_Callback();
			index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100);
		}
		Reset_Wait_Event_At(index);
		if (index == 0) {
			Close_Wait_Dialog();
			return false;
		}
		if (index == 5) {
			ODMessageBox(StringTable::LoadString("TXT_WDT_NET_ERR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5FBB), 0, FUN_0077D710, 0);
			Close_Wait_Dialog();
			return false;
		}
		if (index == 0xB) {
			break;
		}
	}
	Close_Wait_Dialog();
	return true;
}

extern char g_WDTSoundsInited;

// FUNCTION: GAMEMD 0x00797C20
void Init_WDT_Sounds(void)
{
	if (g_WDTSoundsInited) {
		return;
	}
	static Wstring _sound_names[16];

	_sound_names[0].set("01-W024.V01");
	_sound_names[1].set("01-W026.V01");
	_sound_names[2].set("01-W028.V01");
	_sound_names[3].set("01-W018.V01");
	_sound_names[4].set("01-W020.V01");
	_sound_names[5].set("01-W022.V01");
	_sound_names[6].set("00-W022.V00");
	_sound_names[7].set("00-W024.V00");
	_sound_names[8].set("00-W026.V00");
	_sound_names[9].set("00-W017.V00");
	_sound_names[10].set("00-W018.V00");
	_sound_names[11].set("00-W020.V00");
	_sound_names[12].set("01-W032.V01");
	_sound_names[13].set("01-W030.V01");
	_sound_names[14].set("00-W031.V00");
	_sound_names[15].set("00-W029.V00");

	g_WDT_NodWinEmphasisSounds.clear();
	g_WDT_GDIWinEmphasisSounds.clear();
	g_WDT_NodLoseEmphasisSounds.clear();
	g_WDT_GDILoseEmphasisSounds.clear();
	g_WDT_NodTerritoryWinSounds.clear();
	g_WDT_GDITerritoryWinSounds.clear();
	g_WDT_NodTerritoryLoseSounds.clear();
	g_WDT_GDITerritoryLoseSounds.clear();

	g_WDT_NodWinEmphasisSounds.addHead(_sound_names[0]);
	g_WDT_NodWinEmphasisSounds.addHead(_sound_names[1]);
	g_WDT_NodWinEmphasisSounds.addHead(_sound_names[2]);
	g_WDT_NodLoseEmphasisSounds.addHead(_sound_names[3]);
	g_WDT_NodLoseEmphasisSounds.addHead(_sound_names[4]);
	g_WDT_NodLoseEmphasisSounds.addHead(_sound_names[5]);
	g_WDT_GDIWinEmphasisSounds.addHead(_sound_names[6]);
	g_WDT_GDIWinEmphasisSounds.addHead(_sound_names[7]);
	g_WDT_GDIWinEmphasisSounds.addHead(_sound_names[8]);
	g_WDT_GDILoseEmphasisSounds.addHead(_sound_names[9]);
	g_WDT_GDILoseEmphasisSounds.addHead(_sound_names[10]);
	g_WDT_GDILoseEmphasisSounds.addHead(_sound_names[11]);
	g_WDT_NodTerritoryWinSounds.addHead(_sound_names[12]);
	g_WDT_NodTerritoryLoseSounds.addHead(_sound_names[13]);
	g_WDT_GDITerritoryWinSounds.addHead(_sound_names[14]);
	g_WDT_GDITerritoryLoseSounds.addHead(_sound_names[15]);

	g_WDTSoundsInited = 1;
}

// SYNTHETIC: GAMEMD 0x00798120 SYMBOL
// _$E366
// FUNCTION: GAMEMD 0x00798140
void Deinit_WDT_Sounds(void)
{
	g_WDT_NodWinEmphasisSounds.clear();
	g_WDT_GDIWinEmphasisSounds.clear();
	g_WDT_NodLoseEmphasisSounds.clear();
	g_WDT_GDILoseEmphasisSounds.clear();
	g_WDT_NodTerritoryWinSounds.clear();
	g_WDT_GDITerritoryWinSounds.clear();
	g_WDT_NodTerritoryLoseSounds.clear();
	g_WDT_GDITerritoryLoseSounds.clear();
	g_WDTSoundsInited = 0;
}

// FUNCTION: GAMEMD 0x007982A0
void Play_WDT_Sound(ArrayList<Wstring> list)
{
}

// FUNCTION: GAMEMD 0x007982D0
int __fastcall FUN_007982D0(int seconds, int count)
{
	int remaining = seconds * 1000;
	DWORD index;
	while ((index = WaitForMultipleObjects(count, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT) {
		remaining -= 100;
		if (remaining <= 0) {
			return 18;
		}
		WOL_Wait_Callback();
	}
	Reset_Wait_Event_At(index);
	return index;
}

// FUNCTION: GAMEMD 0x00798340
void FUN_00798340(void)
{
	for (int i = 0; i < 18; i++) {
		Reset_Wait_Event_At(i);
	}
}

// FUNCTION: GAMEMD 0x00798360
void FUN_00798360(void)
{
	HKEY key;
	DWORD size;
	DWORD type;
	char value[0x200];

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Westwood\\Yuri's Revenge\\URL", 0, KEY_READ, &key) == ERROR_SUCCESS) {
		size = 0x200;
		LONG status = RegQueryValueEx(key, "Community", NULL, &type, (LPBYTE)value, &size);
		RegCloseKey(key);
		if (status == ERROR_SUCCESS) {
			ViewHTML(value, 0);
		}
	}
}

void __fastcall FUN_00603D30(HWND item, Surface * surface);

// FUNCTION: GAMEMD 0x007983E0
BOOL CALLBACK FUN_007983E0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x6E0:
					Session.WOLWelcomeChoice = 1;
					control->F1(window, 0x6E0);
					return FALSE;

				case 0x686:
					Session.WOLWelcomeChoice = 0;
					control->F1(window, 0x686);
					return FALSE;

				case 0x55F:
					{
						HKEY key;
						DWORD size;
						DWORD type;
						char value[0x200];
						if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Westwood\\Yuri's Revenge\\URL", 0, KEY_READ, &key) == ERROR_SUCCESS) {
							size = 0x200;
							LONG status = RegQueryValueEx(key, "Community", NULL, &type, (LPBYTE)value, &size);
							RegCloseKey(key);
							if (status == ERROR_SUCCESS) {
								ViewHTML(value, 0);
								return FALSE;
							}
						}
					}
					break;

				case 0x6E1:
					Session.WOLWelcomeChoice = 4;
					control->F1(window, 0x6E1);
					return FALSE;

				case 0x6E2:
					Session.WOLWelcomeChoice = 5;
					Session.unknown_10 = 0;
					control->F1(window, 0x6E2);
					break;

				case 0x6E3:
					Session.WOLWelcomeChoice = 3;
					control->F1(window, 0x6E3);
					return FALSE;

				case 0x771:
					Session.WOLWelcomeChoice = 2;
					control->F1(window, 0x771);
					return FALSE;
			}
			break;

		case 0x497:
			{
				HWND item = GetDlgItem(window, 0x746);
				Surface * surface = SurfaceCache.GetSurface("gt18.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x748);
				surface = SurfaceCache.GetSurface("wolclan.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x749);
				surface = SurfaceCache.GetSurface("wolreslk.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x74A);
				surface = SurfaceCache.GetSurface("wolob.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x752);
				surface = SurfaceCache.GetSurface("wolpriv.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75C);
				surface = SurfaceCache.GetSurface("pingg.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75D);
				surface = SurfaceCache.GetSurface("pingy.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75E);
				surface = SurfaceCache.GetSurface("pingr.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79E);
				surface = SurfaceCache.GetSurface("number1.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79F);
				surface = SurfaceCache.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A0);
				surface = SurfaceCache.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A1);
				surface = SurfaceCache.GetSurface("cooperat.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A2);
				surface = SurfaceCache.GetSurface("comchief.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
			}
			break;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x007988C0
BOOL CALLBACK FUN_007988C0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x6E0:
					Session.WOLWelcomeChoice = 1;
					control->F1(window, 0x6E0);
					return FALSE;

				case 0x686:
					Session.WOLWelcomeChoice = 0;
					control->F1(window, 0x686);
					return FALSE;

				case 0x55F:
					{
						HKEY key;
						DWORD size;
						DWORD type;
						char value[0x200];
						if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Westwood\\Yuri's Revenge\\URL", 0, KEY_READ, &key) == ERROR_SUCCESS) {
							size = 0x200;
							LONG status = RegQueryValueEx(key, "Community", NULL, &type, (LPBYTE)value, &size);
							RegCloseKey(key);
							if (status == ERROR_SUCCESS) {
								ViewHTML(value, 0);
								return FALSE;
							}
						}
					}
					break;

				case 0x6E1:
					Session.WOLWelcomeChoice = 4;
					control->F1(window, 0x6E1);
					return FALSE;

				case 0x6E2:
					Session.WOLWelcomeChoice = 5;
					Session.unknown_10 = 0;
					control->F1(window, 0x6E2);
					break;

				case 0x6E4:
					Session.WOLWelcomeChoice = 6;
					control->F1(window, 0x6E4);
					return FALSE;

				case 0x771:
					Session.WOLWelcomeChoice = 2;
					control->F1(window, 0x771);
					return FALSE;
			}
			break;

		case 0x497:
			{
				if (g_NickName.length() > 0) {
					rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
					if (profile.Get()) {
						profile->FUN_0077A9F0(window, &g_NickName, true);
					}
				}
				HWND item = GetDlgItem(window, 0x746);
				Surface * surface = SurfaceCache.GetSurface("gt18.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x748);
				surface = SurfaceCache.GetSurface("wolclan.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x749);
				surface = SurfaceCache.GetSurface("wolreslk.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x74A);
				surface = SurfaceCache.GetSurface("wolob.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x752);
				surface = SurfaceCache.GetSurface("wolpriv.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75C);
				surface = SurfaceCache.GetSurface("pingg.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75D);
				surface = SurfaceCache.GetSurface("pingy.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75E);
				surface = SurfaceCache.GetSurface("pingr.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79E);
				surface = SurfaceCache.GetSurface("number1.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79F);
				surface = SurfaceCache.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A0);
				surface = SurfaceCache.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A1);
				surface = SurfaceCache.GetSurface("cooperat.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A2);
				surface = SurfaceCache.GetSurface("comchief.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
			}
			break;
	}
	return FALSE;
}

void FUN_0072C7E0(void);
void FUN_0072C830(void);

// FUNCTION: GAMEMD 0x00798DE0
int FUN_00798DE0(void)
{
	WOLOptionsClass options;
	FUN_0052FEC0(NULL, true);
	if (g_pNetUtil && strlen(g_LadderServerHost) != 0 && g_NickName.length() > 0) {
		g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, g_NickName.get(), g_GameSKU & 0xFFFF00, -1, NULL, NULL);
		g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, g_NickName.get(), g_GameSKU & 0xFFFF00);
	}
	FUN_0072C7E0();
	if (options.Is_Bit1_Clear()) {
		SimpleWonlineDialogControl dialog;
		Run_Dialog_Control(0x10E, FUN_007988C0, &dialog);
		FUN_0072C830();
		int chosen = Session.WOLWelcomeChoice;
		return chosen != 0;
	}
	SimpleWonlineDialogControl dialog;
	Run_Dialog_Control(0x11C, FUN_007983E0, &dialog);
	FUN_0072C830();
	int chosen = Session.WOLWelcomeChoice;
	return chosen != 0;
}


// FUNCTION: GAMEMD 0x00798F50
void __fastcall FUN_00798F50(HWND combo)
{
	int count;

	FUN_00623840(combo);
	count = 0;
	g_pChat->GetLocaleCount(&count);
	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) {
		profile = FUN_00778A20(&g_NickName);
	} else {
		profile = DAT_00B75E48;
	}
	if (count > 0) {
		for (int i = count - 1; i >= 0; i--) {
			WideWstring name(FUN_00780A00(i));
			int item;
			if (i < 2) {
				item = SendMessage(combo, 0x4C1, 0, (LPARAM)name.get());
			} else {
				item = SendMessage(combo, 0x4C2, 0, (LPARAM)name.get());
			}
			SendMessage(combo, 0x151, item, i);
			if (profile.Get() && i == profile->Locale) {
				SendMessage(combo, 0x14E, item, 0);
			}
		}
	}
}

extern char DAT_0084A210;
extern char g_LadderServerHost[];
extern int g_LadderServerPort;
void FUN_0072C9C0(void);
void FUN_0072CA10(void);
BOOL CALLBACK FUN_00799080(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

Wstring __fastcall FUN_00779B50(void);
Wstring __fastcall FUN_00779DD0(void);
void __fastcall FUN_0077A8D0(HWND dialog, bool flag);
int __fastcall FUN_00624020(HWND combo, int index, WideWstring * out);
void FUN_004E43C0(void);

// FUNCTION: GAMEMD 0x00799080
BOOL CALLBACK FUN_00799080(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG) {
		if (lparam) {
			SetWindowLong(window, 8, ((int *)lparam)[1]);
		}
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x687:
					Session.WOLWelcomeChoice = 5;
					Session.unknown_10 = 6;
					control->F1(window, 0x687);
					return FALSE;

				case 0x688:
					FUN_00773E90("Battleclans");
					return FALSE;

				case 0x686:
					Session.WOLWelcomeChoice = 0;
					control->F1(window, 0x686);
					return FALSE;

				case 0x628:
					switch (HIWORD(wparam)) {
						case CBN_SELCHANGE: {
							HWND check = GetDlgItem(window, 0x6FF);
							SendMessage(check, 0x49D, 0, 1);
							ShowWindow(check, SW_HIDE);
							int item = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);
							int index = -1;
							if (item != -1) {
								index = SendMessage((HWND)lparam, CB_GETITEMDATA, item, 0);
							}
							g_NickName.clear();
							if (index != -1) {
								char const * nick = NULL;
								char const * pass = NULL;
								if (g_pChat->GetNick(index, &nick, &pass) == 0) {
									g_NickName = nick;
								}
							}
							rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
							Wstring autologin = FUN_00779B50();
							EnableWindow(check, index != -1);
							SendMessage(check, BM_SETCHECK, (autologin == g_NickName) ? BST_CHECKED : BST_UNCHECKED, 0);
							SendMessage(check, 0x49D, 0, 0);
							ShowWindow(check, SW_SHOW);
							InvalidateRect(check, NULL, FALSE);
							if (profile.Get()) {
								profile->FUN_0077A9F0(window, &g_NickName, false);
							}
							EnableWindow(GetDlgItem(window, 0x6EB), index != -1 && g_NickName.length() > 0);
							EnableWindow(GetDlgItem(window, 0x687), index != -1 && g_NickName.length() > 0);
							return FALSE;
						}
					}
					break;

				case 0x629:
					switch (HIWORD(wparam)) {
						case CBN_SELCHANGE: {
							Wstring text;
							FUN_00623E40((HWND)lparam, text);
							rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
							if (profile.Get()) {
								profile->SetName(&text);
								profile->Save(&g_NickName);
							}
							return FALSE;
						}
					}
					break;

				case 0x62A:
					switch (HIWORD(wparam)) {
						case CBN_SELCHANGE: {
							int locale = 0;
							int item = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);
							if (item != -1) {
								locale = SendMessage((HWND)lparam, CB_GETITEMDATA, item, 0);
							}
							rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
							if (profile.Get()) {
								profile->SetLocale(locale, 3);
								profile->Save(&g_NickName);
							}
							return FALSE;
						}
					}
					break;

				case 0x6A1:
					switch (HIWORD(wparam)) {
						case CBN_SELCHANGE: {
							int country = -2;
							int item = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);
							if (item != -1) {
								country = SendMessage((HWND)lparam, CB_GETITEMDATA, item, 0);
							}
							rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
							if (profile.Get()) {
								profile->SetCountry(country);
								profile->Save(&g_NickName);
							}
							return FALSE;
						}
					}
					break;

				case 0x6A2:
					switch (HIWORD(wparam)) {
						case CBN_SELCHANGE: {
							int value = -2;
							int item = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);
							if (item != -1) {
								value = SendMessage((HWND)lparam, CB_GETITEMDATA, item, 0);
							}
							rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
							if (profile.Get()) {
								profile->SetField20(value);
								profile->Save(&g_NickName);
							}
							return FALSE;
						}
					}
					break;

				case 0x7A3:
					FUN_00773E90("Ladder");
					return FALSE;

				case 0x6FF:
					if (SendMessage((HWND)lparam, BM_GETCHECK, 0, 0) == BST_CHECKED) {
						FUN_00779C90(g_NickName);
						return FALSE;
					} else {
						Wstring const autologin = FUN_00779B50();
						if (autologin.toLower() == ((Wstring const &)g_NickName).toLower()) {
							FUN_00779C90(DAT_00889F64);
						}
						return FALSE;
					}

				case 0x773:
					{
						rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
						if (profile.Get()) {
							profile->SetMatchByRes(SendMessage((HWND)lparam, BM_GETCHECK, 0, 0) == BST_CHECKED);
							profile->Save(&g_NickName);
						}
					}
					return FALSE;

				case 0x6EA:
					NewLogin(window);
					return FALSE;

				case 0x6EB:
					{
						HWND combo = GetDlgItem(window, 0x628);
						int item = SendMessage(combo, CB_GETCURSEL, 0, 0);
						if (item != -1) {
							WideWstring wide;
							FUN_00624020(combo, item, &wide);
							Wstring name = FUN_007B7F90(wide);
							name.toLower();
							Wstring const autologin = FUN_00779B50();
							if (name == autologin.toLower()) {
								FUN_00779C90(DAT_00889F64);
							}
							Wstring const lastvalid = FUN_00779DD0();
							if (name == lastvalid.toLower()) {
								FUN_00779F10(DAT_00889F64);
							}
							int slot = SendMessage(combo, CB_GETITEMDATA, item, 0);
							if (slot != -1) {
								g_pChat->SetNick(slot, DAT_00889F64, DAT_00889F64, 1);
								SendMessage(combo, CB_DELETESTRING, item, 0);
							}
							SendMessage(combo, CB_SETCURSEL, -1, 0);
						}
						FUN_0077A8D0(window, true);
						InvalidateRect(combo, NULL, FALSE);
						EnableWindow(GetDlgItem(window, 0x6EB), FALSE);
						SendMessage(combo, CB_SETCURSEL, 0, 0);
						rc_ptr<PlayerProfile> profile = FUN_00778710();
						if (profile.Get()) {
							profile->FUN_0077A9F0(window, &(Wstring const &)DAT_00889F64, false);
							HWND check = GetDlgItem(window, 0x6FF);
							SendMessage(check, 0x49D, 0, 1);
							EnableWindow(check, FALSE);
							SendMessage(check, BM_SETCHECK, 0, 0);
							SendMessage(check, 0x49D, 0, 0);
							InvalidateRect(check, NULL, FALSE);
						}
					}
					return FALSE;

				case 0x6EC:
					FUN_00773E90("Manage");
					return FALSE;
			}
			break;

		case 0x497:
			{
				HWND combo = GetDlgItem(window, 0x628);
				FUN_00623840(combo);
				SendMessage(combo, CB_LIMITTEXT, 9, 0);
				WideWstring text(StringTable::LoadString("GUI:Default", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x629F));
				int item = SendMessage(combo, 0x4C2, 0, (LPARAM)text.text());
				SendMessage(combo, CB_SETITEMDATA, item, -1);
				SendMessage(combo, CB_SETCURSEL, item, 0);
				for (int i = 1; i <= 0x20; i++) {
					char const * nick = NULL;
					char const * pass = NULL;
					if (g_pChat->GetNick(i, &nick, &pass) == 0) {
						if (nick != NULL) {
							if (strlen(nick) != 0) {
								WideWstring wide = FUN_007B7ED0(nick);
								int added = SendMessage(combo, 0x4C2, 0, (LPARAM)wide.text());
								SendMessage(combo, CB_SETITEMDATA, added, i);
							}
						}
					}
				}
				FUN_00798F50(GetDlgItem(window, 0x62A));
				FUN_004E3A00(window, 0x6A1);
				FUN_004E43C0();
				FUN_004E45A0(window, 0x6A2);
				FUN_007896A0(GetDlgItem(window, 0x629), true);
				rc_ptr<PlayerProfile> profile = FUN_00778A20(&g_NickName);
				if (profile.Get()) {
					profile->FUN_0077A9F0(window, &g_NickName, true);
				}
				int index = -1;
				int sel = SendMessage(combo, CB_GETCURSEL, 0, 0);
				if (sel != -1) {
					index = SendMessage(combo, CB_GETITEMDATA, sel, 0);
				}
				BOOL valid = index != -1;
				EnableWindow(GetDlgItem(window, 0x6EB), valid);
				EnableWindow(GetDlgItem(window, 0x687), valid);
				HWND check = GetDlgItem(window, 0x6FF);
				EnableWindow(check, valid);
				Wstring autologin = FUN_00779B50();
				SendMessage(GetDlgItem(window, 0x6FF), BM_SETCHECK, (autologin == g_NickName) ? BST_CHECKED : BST_UNCHECKED, 0);
			}
			break;

		case 0x4ED:
			ShowWindow(window, SW_HIDE);
			OwnerDraw::SetUserData2(window, 1);
			return FALSE;
	}
	return FALSE;
}


// FUNCTION: GAMEMD 0x00799CD0
void FUN_00799CD0(void)
{
	g_UserChannels.setEmpty();
	memset(&g_CurrentChannel, 0, 0x10C);
	memset(&s_TempChannel, 0, 0x10C);
	DAT_0084A210 = 1;
	g_GameChannelList.setEmpty();
	g_UserList.setEmpty();
	g_ChannelCount = 0;
}

// FUNCTION: GAMEMD 0x00799D10
void FUN_00799D10(void)
{
	HWND window = FUN_00775B10();
	if (window) {
		do {
			FUN_007757E0(window, 0);
			window = FUN_00775B10();
		} while (window);
	}
}

// FUNCTION: GAMEMD 0x00799D30
int FUN_00799D30(void)
{
	int result = FUN_00786390();
	if (result == 1) {
		result = Fetch_Server_List();
		switch (result) {
			case 0:
				break;
			case 2:
				return -1;
			default:
				return -2;
		}
		if (g_pNetUtil && strlen(g_LadderServerHost) && g_NickName.length() > 0) {
			g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, g_NickName.get(), g_GameSKU & 0xFFFF00, -1, 0, 0);
			g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, g_NickName.get(), g_GameSKU & 0xFFFF00);
		}
		FUN_0072C9C0();
		SimpleWonlineDialogControl control;
		Run_Dialog_Control(0x114, FUN_00799080, &control);
		FUN_0072CA10();
		return Session.WOLWelcomeChoice != 0;
	}
	return result;
}

Wstring __fastcall FUN_00779B50(void);
Wstring __fastcall FUN_00779DD0(void);
void FUN_004E43C0(void);
extern bool DAT_00B77EA0;
extern bool DAT_00B77E68;
int __fastcall FUN_00623FC0(HWND combo, int index, Wstring * out);

// FUNCTION: GAMEMD 0x00799EA0
BOOL CALLBACK FUN_00799EA0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x629:
					if (WS_Find_Dialog(0x11D) && DAT_00B75E48.Get()) {
						Wstring text;
						FUN_00623E40(GetDlgItem(window, 0x629), text);
						DAT_00B75E48.Get()->SetName(&text);
						return FALSE;
					}
					break;

				case 0x628:
					{
						bool open = WS_Find_Dialog(0x11D) != NULL;
						switch (HIWORD(wparam)) {
							case CBN_EDITCHANGE:
							case EN_CHANGE:
								if (HIWORD(wparam) == EN_CHANGE && !open) {
									break;
								}
								if (HIWORD(wparam) == CBN_EDITCHANGE && open) {
									break;
								}
								if (DAT_00B77EA0) {
									break;
								}
								{
									Wstring text;
									FUN_00623E40((HWND)lparam, text);
									bool found = FUN_00786DC0(text);
									bool enable = found || DAT_00B77E68;
									if (!found) {
										g_NickName = text;
									}
									DAT_00B77EA0 = true;
									HWND edit = GetDlgItem(window, 0x72C);
									SendMessage(edit, 0x4B4, 0, (LPARAM)g_LoginPassword.text());
									InvalidateRect(edit, NULL, FALSE);
									DAT_00B77EA0 = false;
									bool ready = g_NickName.length() > 0 && g_LoginPassword.length() == 8;
									HWND check = GetDlgItem(window, 0x72D);
									ShowWindow(check, SW_HIDE);
									SendMessage(check, 0x49D, 0, 1);
									SendMessage(check, BM_SETCHECK, enable, 0);
									EnableWindow(check, FALSE);
									SendMessage(check, 0x49D, 0, 0);
									ShowWindow(check, SW_SHOW);
									InvalidateRect(check, NULL, FALSE);
									EnableWindow(GetDlgItem(window, 0x6EB), g_NickName.length() > 0);
									EnableWindow(GetDlgItem(window, 0x539), ready);
								}
								return FALSE;

							case CBN_SELCHANGE:
								if (open) {
									break;
								}
								{
									DAT_00B77EA0 = true;
									g_NickName.clear();
									g_LoginPassword.clear();
									g_PlaintextPassword = true;
									HWND edit = GetDlgItem(window, 0x72C);
									SendMessage(edit, 0x4B4, 0, (LPARAM)g_LoginPassword.text());
									bool enable = DAT_00B77E68;
									int item = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);
									if (item != -1) {
										int slot = SendMessage((HWND)lparam, CB_GETITEMDATA, item, 0);
										if (slot > 0 && slot <= 0x20) {
											FUN_00623FC0((HWND)lparam, item, &g_NickName);
											char const * nick;
											char const * pass;
											g_pChat->GetNick(slot, &nick, &pass);
											g_LoginPassword = pass;
											edit = GetDlgItem(window, 0x72C);
											SendMessage(edit, 0x4B4, 0, (LPARAM)g_LoginPassword.text());
											g_PlaintextPassword = false;
											enable = true;
										}
									}
									bool ready = g_NickName.length() > 0 && g_LoginPassword.length() == 8;
									HWND check = GetDlgItem(window, 0x72D);
									SendMessage(check, BM_SETCHECK, enable, 0);
									EnableWindow(check, FALSE);
									InvalidateRect(check, NULL, FALSE);
									EnableWindow(GetDlgItem(window, 0x6EB), g_NickName.length() > 0);
									EnableWindow(GetDlgItem(window, 0x539), ready);
									DAT_00B77EA0 = false;
								}
								return FALSE;
						}
					}
					break;

				case 0x539:
					control->F1(window, 0x539);
					return FALSE;

				case 0x62A:
					if (WS_Find_Dialog(0x11D) && DAT_00B75E48.Get()) {
						HWND combo = GetDlgItem(window, 0x62A);
						int item = SendMessage(combo, CB_GETCURSEL, 0, 0);
						int locale = 0;
						if (item != -1) {
							locale = SendMessage(combo, CB_GETITEMDATA, item, 0);
						}
						DAT_00B75E48.Get()->SetLocale(locale, 3);
						return FALSE;
					}
					break;

				case 0x6A1:
					if (WS_Find_Dialog(0x11D) && DAT_00B75E48.Get()) {
						int country = -2;
						HWND combo = GetDlgItem(window, 0x6A1);
						int item = SendMessage(combo, CB_GETCURSEL, 0, 0);
						if (item != -1) {
							country = SendMessage(combo, CB_GETITEMDATA, item, 0);
						}
						DAT_00B75E48->SetCountry(country);
						return FALSE;
					}
					break;

				case 0x686:
					control->F1(window, 2);
					return FALSE;

				case 0x6A2:
					if (WS_Find_Dialog(0x11D) && DAT_00B75E48.Get()) {
						int value = -2;
						HWND combo = GetDlgItem(window, 0x6A2);
						int item = SendMessage(combo, CB_GETCURSEL, 0, 0);
						if (item != -1) {
							value = SendMessage(combo, CB_GETITEMDATA, item, 0);
						}
						DAT_00B75E48->SetField20(value);
						return FALSE;
					}
					break;

				case 0x72C:
					if (HIWORD(wparam) == EN_CHANGE && !DAT_00B77EA0) {
						g_PlaintextPassword = true;
						FUN_00623E40((HWND)lparam, g_LoginPassword);
						Wstring text;
						FUN_00623E40(GetDlgItem(window, 0x628), text);
						bool ready = text.length() > 0 && g_LoginPassword.length() == 8;
						HWND check = GetDlgItem(window, 0x72D);
						SendMessage(check, 0x49D, 0, 1);
						EnableWindow(check, ready);
						InvalidateRect(check, NULL, FALSE);
						SendMessage(check, 0x49D, 0, 0);
						EnableWindow(GetDlgItem(window, 0x6EB), FALSE);
						EnableWindow(GetDlgItem(window, 0x539), ready);
						return FALSE;
					}
					break;

				case 0x72D:
					DAT_00B77E68 = SendMessage((HWND)lparam, BM_GETCHECK, 0, 0) == BST_CHECKED;
					return FALSE;

				case 0x6EB:
					{
						DAT_00B77EA0 = true;
						HWND combo = GetDlgItem(window, 0x628);
						int item = SendMessage(combo, 0x4B9, -1, (LPARAM)g_NickName.text());
						if (item != -1) {
							Wstring name;
							FUN_00623FC0(combo, item, &name);
							name.toLower();
							Wstring const autologin = FUN_00779B50();
							if (name == autologin.toLower()) {
								FUN_00779C90(DAT_00889F64);
							}
							Wstring const lastvalid = FUN_00779DD0();
							if (name == lastvalid.toLower()) {
								FUN_00779F10(DAT_00889F64);
							}
							int slot = SendMessage(combo, CB_GETITEMDATA, item, 0);
							SendMessage(combo, CB_DELETESTRING, item, 0);
							g_pChat->SetNick(slot, DAT_00889F64, DAT_00889F64, 1);
						}
						SendMessage(combo, 0x4B4, 0, (LPARAM)DAT_00889F64);
						InvalidateRect(combo, NULL, FALSE);
						HWND edit = GetDlgItem(window, 0x72C);
						SendMessage(edit, 0x4B4, 0, (LPARAM)DAT_00889F64);
						g_NickName.clear();
						g_LoginPassword.clear();
						g_PlaintextPassword = true;
						HWND check = GetDlgItem(window, 0x72D);
						EnableWindow(check, FALSE);
						InvalidateRect(check, NULL, FALSE);
						EnableWindow(GetDlgItem(window, 0x6EB), FALSE);
						EnableWindow(GetDlgItem(window, 0x539), FALSE);
						DAT_00B77EA0 = false;
					}
					return FALSE;

				case 0x6EA:
					NewLogin(window);
					return FALSE;

				case 0x6EC:
					FUN_00773E90("Manage");
					return FALSE;
			}
			break;

		case 0x497:
			{
				DAT_00B77EA0 = true;
				HWND combo = GetDlgItem(window, 0x628);
				HWND edit = GetDlgItem(window, 0x72C);
				SendMessage(combo, 0x4D1, 1, 0);
				SendMessage(edit, 0x4D1, 1, 0);
				SendMessage(combo, EM_LIMITTEXT, 9, 0);
				SendMessage(edit, EM_LIMITTEXT, 8, 0);
				FUN_00623840(combo);
				SendMessage(combo, CB_LIMITTEXT, 9, 0);
				for (int i = 1; i < 0x20; i++) {
					char const * nick;
					char const * pass;
					if (g_pChat->GetNick(i, &nick, &pass) == 0 && nick != NULL && strlen(nick) != 0) {
						int added = SendMessage(combo, 0x4BC, 0, (LPARAM)nick);
						SendMessage(combo, CB_SETITEMDATA, added, i);
					}
				}
				Wstring saved(g_NickName);
				bool found = FUN_00786DC0(g_NickName);
				if (!found) {
					g_NickName = saved;
				}
				SendMessage(combo, 0x4B4, 0, (LPARAM)g_NickName.text());
				if (g_NickName.length() > 0) {
					SendMessage(edit, 0x4B4, 0, (LPARAM)g_LoginPassword.text());
					g_PlaintextPassword = false;
				}
				bool ready = g_NickName.length() > 0 && g_LoginPassword.length() == 8;
				HWND check = GetDlgItem(window, 0x72D);
				SendMessage(check, 0x49D, 0, 1);
				SendMessage(check, BM_SETCHECK, found, 0);
				EnableWindow(check, FALSE);
				InvalidateRect(check, NULL, FALSE);
				SendMessage(check, 0x49D, 0, 0);
				EnableWindow(GetDlgItem(window, 0x6EB), g_NickName.length() > 0);
				EnableWindow(GetDlgItem(window, 0x539), ready);
				WOLOptionsClass options;
				if (!options.Is_Bit1_Clear()) {
					FUN_00798F50(GetDlgItem(window, 0x62A));
					FUN_004E3A00(window, 0x6A1);
					HWND country = GetDlgItem(window, 0x6A1);
					int count = SendMessage(country, CB_GETCOUNT, 0, 0);
					for (int j = 0; j < count; j++) {
						int data = SendMessage(country, CB_GETITEMDATA, j, 0);
						if (data == DAT_00B75E48.Get()->Country) {
							SendMessage(country, CB_SETCURSEL, j, 0);
							break;
						}
					}
					FUN_004E43C0();
					FUN_004E45A0(window, 0x6A2);
					HWND side = GetDlgItem(window, 0x6A2);
					count = SendMessage(side, CB_GETCOUNT, 0, 0);
					for (int k = 0; k < count; k++) {
						int data = SendMessage(side, CB_GETITEMDATA, k, 0);
						if (data == DAT_00B75E48.Get()->Field20) {
							SendMessage(side, CB_SETCURSEL, k, 0);
							break;
						}
					}
					FUN_007896A0(GetDlgItem(window, 0x629), true);
				}
				DAT_00B77EA0 = false;
			}
			return FALSE;

		case 0x4ED:
			ShowWindow(window, SW_HIDE);
			OwnerDraw::SetUserData2(window, 1);
			return FALSE;

		case 0x4DB:
			{
				bool previous = wparam != 0;
				HWND child = (HWND)lparam;
				HWND parent = GetParent(child);
				while (parent && child && parent != window) {
					child = parent;
					parent = GetParent(child);
				}
				if (child) {
					Focus_Next_Tab_Item(child, previous);
					return FALSE;
				}
			}
			break;

		case 0x4DA:
			{
				FUN_00623E40(GetDlgItem(window, 0x72C), g_LoginPassword);
				Wstring text;
				FUN_00623E40(GetDlgItem(window, 0x628), text);
				if (text.length() > 0 && g_LoginPassword.length() == 8) {
					PostMessage(GetDlgItem(window, 0x539), BM_CLICK, 0, 0);
				}
			}
			break;
	}
	return FALSE;
}

void __fastcall FUN_00779830(bool value);
void __fastcall FUN_00779C90(Wstring const & value);

// FUNCTION: GAMEMD 0x0079AE00
BOOL CALLBACK FUN_0079AE00(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					FUN_00779C90(g_NickName);
					control->F1(window, 1);
					break;

				case IDCANCEL:
					bool checked = IsDlgButtonChecked(window, 0x6F9) == BST_CHECKED;
					FUN_00779830(!checked);
					control->F1(window, 2);
					break;
			}
			break;
	}
	return FALSE;
}

BOOL CALLBACK FUN_0079DB50(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

bool __fastcall FUN_0079F0A0(Wstring * name, ArrayList<Channel> * channels, Channel * result);

void __fastcall FUN_0079BF50(HWND dialog);
extern int DAT_0084A1CC;
extern int DAT_0084A1D0;

// FUNCTION: GAMEMD 0x0079AF10
void __fastcall FUN_0079AF10(HWND dialog)
{
	HWND list = GetDlgItem(dialog, 0x689);
	SendMessageA(list, 0x49d, 0, 1);

	for (int i = 0; i < DAT_00B73AA8.length(); ++i) {
		Wstring buddy;
		DAT_00B73AA8.get(buddy, i);
		WideWstring display = FUN_007B7ED0(buddy);
		WolEntryState state = WOL_ENTRY_STATE_NONE;
		DAT_00B76990.getValue(buddy, state);

		int row = SendMessageA(list, 0x4c5, 0, (LPARAM)buddy.text());
		if (row == -1) {
			row = SendMessageA(list, 0x4c8, 0, (LPARAM)buddy.text());
		}

		OwnerDraw::CellData name;
		name.type = OwnerDraw::CellData::TEXT;
		name.string.set(display.get());
		name.hint.set(DAT_00887734);

		OwnerDraw::CellData status;
		status.type = OwnerDraw::CellData::TEXT;
		status.hint.set(DAT_00887734);

		OwnerDraw::CellData mode;
		mode.type = OwnerDraw::CellData::INVALID;
		mode.string.set(DAT_00887734);
		mode.hint.set(DAT_00887734);

		OwnerDraw::CellData password;
		password.type = OwnerDraw::CellData::INVALID;
		password.string.set(DAT_00887734);
		password.hint.set(DAT_00887734);

		OwnerDraw::CellData gameType;
		gameType.type = OwnerDraw::CellData::INVALID;
		gameType.string.set(DAT_00887734);
		gameType.hint.set(DAT_00887734);

		OwnerDraw::CellData ping;
		ping.type = OwnerDraw::CellData::INVALID;
		ping.string.set(DAT_00887734);
		ping.hint.set(DAT_00887734);

		switch (state) {
		case 1:
			status.string.set(
			    StringTable::LoadString("WOL:BuddyDisabledFind", 0, "D:\\ra2mdpost\\wonline.cpp", 0x67BD));
			name.color = status.color = 0x808080;
			break;

		case 2:
			status.string.set(StringTable::LoadString("WOL:BuddyOffline", 0, "D:\\ra2mdpost\\wonline.cpp", 0x67C3));
			name.color = status.color = 0x808080;
			break;

		case 3:
			status.string.set(StringTable::LoadString("WOL:BuddyOnline", 0, "D:\\ra2mdpost\\wonline.cpp", 0x67C9));
			name.color = status.color = 0xf0f0;
			break;

		case 7:
		case 8:
			status.string.set(
			    StringTable::LoadString("WOL:BuddyQuickMatching", 0, "D:\\ra2mdpost\\wonline.cpp", 0x67D0));
			name.color = status.color = 0x80ff00;
			break;

		case 4: {
			Channel channel;
			memset(&channel, 0, sizeof(channel));

			if (!DAT_00B76608.getValue(buddy, channel)) {
				status.string.set(StringTable::LoadString("WOL:BuddyOnline", 0, "D:\\ra2mdpost\\wonline.cpp", 0x67E9));
				name.color = status.color = 0xf0f0;
			} else {
				if (channel.type == 0 && strncmp((char *)channel.name, "Lob_41_", strlen("Lob_41_")) == 0) {
					int lobby = atol((char *)channel.name + strlen("Lob_41_"));
					int page = lobby / 8;

					WideWstring location(g_Lobbies[lobby % 8]);

					if (page) {
						location += L" ";
						location += FUN_007B7DD0(page + 1);
					}
					status.string.set(location.get());
				} else {
					status.string.set(FUN_00735120((char *)channel.name));
				}
				name.color = status.color = 0xff4000;
			}
			break;
		}

		case 5: {
			Channel game;
			memset(&game, 0, sizeof(game));
			char found = DAT_00B76608.getValue(buddy, game);

			if (found) {
				Wstring game_name((char *)game.name);
				found = FUN_0079F0A0(&game_name, &g_GameChannelList, &game);
			}

			if (found) {
				char options[132];
				strncpy(options, (char *)game.exInfo, 128);
				int first = 0, second = 0, third = 0;
				int fourth = 0, fifth = 0;
				Wstring rest;

				FUN_0077E820(options, &first, &second, &third, &fourth, &fifth, &rest);

				unsigned int current = game.currentUsers - fourth + third;
				int reserved = (fourth != 0 || fifth != 0);
				unsigned int maximum = game.maxUsers - reserved;

				wchar_t details[80];
				swprintf(details, L"%hs %d/%d", game.name, current, maximum);
				status.string.set(details);
				name.color = status.color = 0xffff00;

				mode.type = OwnerDraw::CellData::SURFACE;

				if (game.tournament != 0) {
					mode.surf = SurfaceCache.GetSurface("woltrny.pcx", 0);
					mode.hint.set(
					    StringTable::LoadString("TXT_TOURNAMENT_GAME", 0, "D:\\ra2mdpost\\wonline.cpp", 0x680D));
				} else if (game.reserved & 0x100) {
					mode.surf = SurfaceCache.GetSurface("wolclan.pcx", 0);
					mode.hint.set(
					    StringTable::LoadString("TXT_BATTLECLAN_GAME", 0, "D:\\ra2mdpost\\wonline.cpp", 0x6810));
				} else {
					mode.surf = SurfaceCache.GetSurface("gt18.bmp", 0);
					mode.hint.set(DAT_00887734);
				}

				if (game.reserved & 0x600) {
					if (fourth == 0 && fifth != 0) {
						gameType.type = OwnerDraw::CellData::SURFACE;
						gameType.surf = SurfaceCache.GetSurface("wolrslkob.pcx", 0);
					} else {
						gameType.type = OwnerDraw::CellData::SURFACE;
						gameType.surf = SurfaceCache.GetSurface("wolreslk.pcx", 0);
					}
				} else if (fourth == 0 && fifth != 0) {
					gameType.type = OwnerDraw::CellData::SURFACE;
					gameType.surf = SurfaceCache.GetSurface("wolob.pcx", 0);
				}

				if (game.flags & 0x100) {
					password.type = OwnerDraw::CellData::SURFACE;
					password.surf = SurfaceCache.GetSurface("wolpriv.pcx", 0);
					password.hint.set(
					    StringTable::LoadString("TXT_GAME_PASSWORD", 0, "D:\\ra2mdpost\\wonline.cpp", 0x6830));
				}

				ping.type = OwnerDraw::CellData::PING;
				ping.pingtime = game.latency;
				swprintf(details, StringTable::LoadString("WOL:GamePing", 0, "D:\\ra2mdpost\\wonline.cpp", 0x6835),
				         game.latency);
				ping.hint.set(details);
			} else {
				status.string.set(StringTable::LoadString("WOL:BuddyOnline", 0, "D:\\ra2mdpost\\wonline.cpp", 0x6838));
				name.color = status.color = 0xf0f0;
			}
			break;
		}

		case 6:
			status.string.set(StringTable::LoadString("WOL:BuddyPlayingGame", 0, "D:\\ra2mdpost\\wonline.cpp", 0x683F));
			name.color = status.color = 0x3050ff;
			break;

		default:
			status.string.set(
			    StringTable::LoadString("WOL:BuddyUnknownStatus", 0, "D:\\ra2mdpost\\wonline.cpp", 0x6845));
			name.color = status.color = 0x808080;
			break;
		}

		SendMessageA(list, 0x4a8, ((unsigned short)row << 16) | ((unsigned short)DAT_0084A1C4), (LPARAM)&name);
		SendMessageA(list, 0x4a8, ((unsigned short)row << 16) | ((unsigned short)DAT_0084A1C8), (LPARAM)&mode);
		SendMessageA(list, 0x4a8, ((unsigned short)row << 16) | ((unsigned short)DAT_0084A1CC), (LPARAM)&password);
		SendMessageA(list, 0x4a8, ((unsigned short)row << 16) | ((unsigned short)DAT_0084A1D0), (LPARAM)&gameType);
		SendMessageA(list, 0x4a8, ((unsigned short)row << 16) | ((unsigned short)DAT_0084A1D4), (LPARAM)&status);
		SendMessageA(list, 0x4a8, ((unsigned short)row << 16) | ((unsigned short)DAT_0084A1D8), (LPARAM)&ping);
	}

	int wanted = -1;
	if (DAT_00B76E24.length() > 0) {
		wanted = SendMessageA(list, 0x4c5, 0, (LPARAM)DAT_00B76E24.text());
	}

	if (wanted != SendMessageA(list, 0x188, 0, 0)) {
		SendMessageA(list, 0x186, wanted, 0);
	}

	SendMessageA(list, 0x49d, 0, 0);
	InvalidateRect(list, 0, 0);
	FUN_0079BF50(dialog);
	FUN_0079C040(dialog);
}

// FUNCTION: GAMEMD 0x0079BA60
bool __fastcall FUN_0079BA60(Channel * chan, bool flag, bool extra)
{
	bool restricted = FUN_0077D9B0(chan) || FUN_0077D9D0(chan);
	if (restricted != FUN_0077CF40()) {
		return false;
	}
	if (FUN_0077D990(chan) && g_OwnSquadID < 1) {
		if (!flag) {
			return false;
		}
		PMessagePrintf(-1, StringTable::LoadString("TXT_NO_CLAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x686D));
		return false;
	}
	char text[0x84];
	strcpy(text, (char *)chan->exInfo);
	int first = 0;
	int second = 0;
	int third = 0;
	int fourth = 0;
	int fifth = 0;
	Wstring file;
	FUN_0077E820(text, &first, &second, &third, &fourth, &fifth, &file);
	int buildnum = FUN_00777B50();
	int crc = GetINIHash();
	if (buildnum == first && crc == second) {
		if (!extra) {
			int users = chan->currentUsers - fourth + third;
			int reserved = (fourth != 0 || fifth != 0) ? 1 : 0;
			if (users >= (int)chan->maxUsers - reserved) {
				if (flag) {
					PMessagePrintf(-1, StringTable::LoadString("WOL:GameFull", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x689C));
				}
			} else {
				return true;
			}
		} else {
			if (fifth == 0 || fourth != 0) {
				if (flag) {
					PMessagePrintf(-1, StringTable::LoadString("TXT_OBSERVER_FULL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x68A4));
				}
			} else {
				return true;
			}
		}
	} else if (flag) {
		PMessagePrintf(-1, StringTable::LoadString("TXT_MISMATCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6891));
	}
	return false;
}

// FUNCTION: GAMEMD 0x0079BC80
bool FUN_0079BC80(void)
{
	if (DAT_00B76E24.length() == 0) {
		return false;
	}
	WolEntryState state = WOL_ENTRY_STATE_NONE;
	DAT_00B76990.getValue(DAT_00B76E24, state);
	if (state == 4) {
		return true;
	}
	if (state != 5) {
		return false;
	}
	Channel channel;
	if (!DAT_00B76608.getValue(DAT_00B76E24, channel)) {
		return false;
	}
	Wstring game_name((char *)channel.name);
	Channel game;
	if (!FUN_0079F0A0(&game_name, &g_GameChannelList, &game)) {
		return false;
	}
	return(FUN_0079BA60(&game, false, Game::ObserverMode != 0));
}

// FUNCTION: GAMEMD 0x0079BE70
void FUN_0079BE70(void)
{
	HWND dialog = WS_Create_Dialog(ProgramInstance, 0x11E, MainWindow, FUN_0079DB50, FALSE);
	UI::RegisterWindow(dialog, 0x11E);
	SendMessage(dialog, 0x4A9, 0, 1);
	ShowWindow(dialog, SW_SHOWNORMAL);
	FUN_007759E0(dialog, WOL_Wait_Callback, 0, 1, 1);
}

// FUNCTION: GAMEMD 0x0079BED0
bool FUN_0079BED0(void)
{
	if (DAT_00B76E24.length() == 0) {
		return false;
	}

	WolEntryState state = WOL_ENTRY_STATE_NONE;
	DAT_00B76990.getValue(DAT_00B76E24, state);

	if ((int)state >= 0 && (int)state <= 2) {
		return false;
	}

	return true;
}

extern int DAT_00B779D0;

static inline void Set_Text(HWND window, wchar_t const * text)
{
	SendMessage(window, 0x4B2, 0, (LPARAM)text);
}

// FUNCTION: GAMEMD 0x0079BF50
void __fastcall FUN_0079BF50(HWND dialog)
{
	EnableWindow(GetDlgItem(dialog, 0x703), DAT_00B76E24.length() > 0);

	EnableWindow(GetDlgItem(dialog, 0x62D), FUN_0079BC80());

	EnableWindow(GetDlgItem(dialog, 0x5C2), FUN_0079BED0());
}

// FUNCTION: GAMEMD 0x0079C040
void __fastcall FUN_0079C040(HWND dialog)
{
	HWND name_label = GetDlgItem(dialog, 0x6EF);
	HWND rank_label = GetDlgItem(dialog, 0x6F4);
	HWND field10_label = GetDlgItem(dialog, 0x6F5);
	HWND field08_label = GetDlgItem(dialog, 0x6F0);
	HWND field0C_label = GetDlgItem(dialog, 0x6F1);
	HWND field20_label = GetDlgItem(dialog, 0x701);
	if (DAT_00B76E24.length() == 0) {
		Set_Text(name_label, DAT_00887734);
		Set_Text(rank_label, DAT_00887734);
		Set_Text(field10_label, DAT_00887734);
		Set_Text(field08_label, DAT_00887734);
		Set_Text(field0C_label, DAT_00887734);
		Set_Text(field20_label, DAT_00887734);
		return;
	}
	WideWstring format(StringTable::LoadString("GUI:UserProfileLabel", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6902));
	wchar_t * text = new wchar_t[format.length() + DAT_00B76E24.length() + 1];
	swprintf(text, format.get(), DAT_00B76E24.get());
	HWND name_item = GetDlgItem(dialog, 0x6EF);
	Set_Text(name_item, text);
	delete text;
	Ladder ladder;
	memset(&ladder, 0, sizeof(ladder));
	Wstring key = ((Wstring const &)DAT_00B76E24).toLower();
	if (g_GeneralLadderDict.getValue(key, ladder)) {
		Set_Text(rank_label, FUN_007B7DD0(ladder.rung).text());
		Set_Text(field10_label, FUN_007B7DD0(ladder.points).text());
		Set_Text(field08_label, FUN_007B7DD0(ladder.wins).text());
		Set_Text(field0C_label, FUN_007B7DD0(ladder.losses).text());
		Set_Text(field20_label, FUN_007B7DD0(ladder.disconnects).text());
	} else {
		WideWstring unknown(StringTable::LoadString("GUI:UnknownStats", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6913));
		Set_Text(rank_label, unknown.text());
		Set_Text(field10_label, unknown.text());
		Set_Text(field08_label, unknown.text());
		Set_Text(field0C_label, unknown.text());
		Set_Text(field20_label, unknown.text());
	}
}

// FUNCTION: GAMEMD 0x0079C3A0
void FUN_0079C3A0(void)
{
	if (DAT_00B73ACC.length() > 0) {
		if ((unsigned int)(time(NULL) - DAT_00B779D0) < 5) {
			return;
		}
		DAT_00B73ACC.clear();
	}
	if (DAT_00B76980.length() == 0) {
		return;
	}
	DAT_00B76980.removeHead(DAT_00B73ACC);
	DAT_00B779D0 = time(NULL);
	User user;
	memset(&user, 0, sizeof(user));
	strcpy((char *)user.name, DAT_00B73ACC.get());
	g_pChat->RequestFind(&user);
}

// FUNCTION: GAMEMD 0x0079C520
bool __fastcall FUN_0079C520(Wstring * name)
{
	Wstring lower(*name);
	lower.toLower();
	if (name->length() == 0) {
		return false;
	}
	if (((Wstring const &)lower).toLower() == ((Wstring const &)g_NickName).toLower()) {
		return false;
	}
	if (name->containsAny(" ,;")) {
		return false;
	}
	for (int i = 0; i < DAT_00B73AA8.length(); i++) {
		Wstring entry;
		DAT_00B73AA8.get(entry, i);
		entry.toLower();
		if (entry == lower) {
			return false;
		}
	}
	return true;
}

// FUNCTION: GAMEMD 0x0079C650
void __fastcall FUN_0079C650(Wstring * name)
{
	if (!FUN_0079C520(name)) {
		return;
	}
	Wstring entry(*name);
	DAT_00B73AA8.addTail(entry);
	DAT_00B76980.addTail(entry);
	DAT_00B779B0.addTail(entry);
	FUN_0079C3A0();
}


// FUNCTION: GAMEMD 0x0079C8D0
void __fastcall FUN_0079C8D0(Wstring * name)
{
	if (!FUN_0079C520(name)) {
		return;
	}
	User user;
	memset(&user, 0, sizeof(user));
	strncpy((char *)user.name, name->text(), 9);
	g_pChat->RequestBuddyAdd(&user);
	if (FUN_0079C520(name)) {
		Wstring entry(*name);
		DAT_00B73AA8.addTail(entry);
		DAT_00B76980.addTail(entry);
		DAT_00B779B0.addTail(entry);
		FUN_0079C3A0();
	}
	FUN_0079D110();
}

static inline char Remove_Channel(Wstring & key, LadderC & value)
{
	return DAT_00B76608.remove(key, value);
}

// FUNCTION: GAMEMD 0x0079CBA0
void __fastcall FUN_0079CBA0(Wstring * name)
{
	LadderC value;
	User user;
	for (int i = 0; i < DAT_00B73AA8.length(); i++) {
		Wstring entry;
		DAT_00B73AA8.get(entry, i);
		if (entry == *name) {
			DAT_00B73AA8.remove(i);
			Wstring key(*name);
			Remove_Channel(key, value);
			DAT_00B76990.remove(key);
			memset(&user, 0, sizeof(user));
			strncpy((char *)user.name, name->text(), 9);
			g_pChat->RequestBuddyDelete(&user);
			return;
		}
	}
}

extern int DAT_00B778C8;

// FUNCTION: GAMEMD 0x0079CE70
void FUN_0079CE70(void)
{
	DAT_00B778C8 = time(NULL);
	if (!FUN_0077F710(1, false)) {
		SetEvent(g_WaitEventHandles[5]);
		return;
	}
	DAT_00B76980.clear();
	for (int i = 0; i < DAT_00B73AA8.length(); i++) {
		Wstring node;
		DAT_00B73AA8.get(node, i);
		DAT_00B76980.addTail(node);
	}
	FUN_0079C3A0();
}

// FUNCTION: GAMEMD 0x0079D110
void FUN_0079D110(void)
{
	int count = DAT_00B779B0.length();
	if (count > 10) {
		count = 10;
	} else if (count == 0) {
		return;
	}
	Wstring keys;
	for (int i = 0; i < count; i++) {
		Wstring name;
		DAT_00B779B0.removeHead(name);
		if (keys.length() > 0) {
			keys += ":";
		}
		keys += name;
	}
	g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, keys.get(), g_GameSKU & 0xFFFF00, -1, 0, 0);
	g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, keys.get(), g_GameSKU & 0xFFFF00);
}

// FUNCTION: GAMEMD 0x0079D2F0
void FUN_0079D2F0(void)
{
	DAT_00B779B0.clear();
	for (int i = 0; i < DAT_00B73AA8.length(); i++) {
		Wstring node;
		DAT_00B73AA8.get(node, i);
		DAT_00B779B0.addTail(node);
	}
	FUN_0079D110();
}

void FUN_00623F60(HWND listbox, int index, WideWstring & text);
int __fastcall FUN_00623EA0(HWND window, WideWstring * text);

// FUNCTION: GAMEMD 0x0079D4E0
void FUN_0079D4E0(void)
{
	DAT_00B73AA8.clear();
	DAT_00B76980.clear();
	DAT_00B76990.clear();
	DAT_00B76608.clear();
	g_pChat->RequestBuddyList();
	if (Session.WOLWelcomeChoice != 3 && Session.WOLWelcomeChoice != 1 && Session.WOLWelcomeChoice != 2) {
		FUN_0079CE70();
	}
	FUN_0079D2F0();
}

// FUNCTION: GAMEMD 0x0079D720
BOOL CALLBACK FUN_0079D720(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_DESTROY:
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x540:
					switch (HIWORD(wparam)) {
						case EN_CHANGE: {
							WideWstring text;
							FUN_00623EA0((HWND)lparam, &text);
							EnableWindow(GetDlgItem(window, 0x5C2), text.length() > 0);
							return FALSE;
						}

						case EN_UPDATE: {
							WideWstring text;
							FUN_00623EA0((HWND)lparam, &text);
							text.trimTrailing(L"\n\r");
							return FALSE;
						}

						case EN_MAXTEXT:
							SendMessage(GetDlgItem(window, 0x5C2), BM_CLICK, 0, 0);
							return FALSE;

						default:
							return FALSE;
					}

				case 0x5C2: {
					WideWstring text;
					HWND edit = GetDlgItem(window, 0x540);
					FUN_00623EA0(edit, &text);
					SendMessage(edit, 0x4B2, 0, (LPARAM)DAT_00887734);
					if (text.length() > 0) {
						text.trimTrailing(L"\n\r");
						User user;
						memset(&user, 0, sizeof(user));
						strcpy((char *)user.name, DAT_00B76E24.get());
						g_pChat->RequestUnicodePage(&user, text.get());
					}
					InvalidateRect(edit, NULL, FALSE);
					FUN_007757E0(window, 0x5C2);
					return FALSE;
				}

				case 0x686:
					FUN_007757E0(window, 0x686);
					return FALSE;

				default:
					return FALSE;
			}

		case 0x497: {
			HWND edit = GetDlgItem(window, 0x540);
			SendMessage(edit, EM_LIMITTEXT, 0x78, 0);
			SendMessage(edit, 0x4B2, 0, (LPARAM)DAT_00887734);
			EnableWindow(GetDlgItem(window, 0x5C2), FALSE);
			HWND label = GetDlgItem(window, 0x712);
			SendMessage(label, 0x4B4, 0, (LPARAM)DAT_00B76E24.text());
			return FALSE;
		}

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		default:
			return FALSE;
	}
}

// FUNCTION: GAMEMD 0x0079DA90
Wstring __fastcall FUN_0079DA90(HWND list)
{
	int index = SendMessage(list, LB_GETCURSEL, 0, 0);
	if (index == -1) {
		index = 0;
	}
	while (index < SendMessage(list, LB_GETCOUNT, 0, 0)) {
		WideWstring text;
		FUN_00623F60(list, index, text);
		if (text.getChar(0) == L'(' && text.truncate(L')')) {
			text.removeChar(L'(');
			return FUN_007B7F90(text);
		}
		index++;
	}
	return DAT_00B76E24;
}

static inline void Show_Name(HWND window, Wstring const & name)
{
	HWND label = GetDlgItem(window, 0x718);
	SendMessage(label, 0x4B4, 0, (LPARAM)name.text());
}

// FUNCTION: GAMEMD 0x0079DB50
BOOL CALLBACK FUN_0079DB50(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_DESTROY:
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x53F:
					switch (HIWORD(wparam)) {
						case LBN_SELCHANGE: {
							Wstring name = FUN_0079DA90((HWND)lparam);
							HWND label = GetDlgItem(window, 0x718);
							SendMessage(label, 0x4B4, 0, (LPARAM)name.text());
							return FALSE;
						}

						default:
							return FALSE;
					}

				case 0x540:
					switch (HIWORD(wparam)) {
						case EN_CHANGE: {
							WideWstring text;
							FUN_00623EA0((HWND)lparam, &text);
							EnableWindow(GetDlgItem(window, 0x5C2), text.length() > 0);
							return FALSE;
						}

						case EN_UPDATE: {
							WideWstring text;
							FUN_00623EA0((HWND)lparam, &text);
							text.trimTrailing(L"\n\r");
							return FALSE;
						}

						case EN_MAXTEXT: {
							WideWstring text;
							FUN_00623EA0((HWND)lparam, &text);
							text.trimTrailing(L"\n\r");
							if (text.length() > 0) {
								SendMessage(GetDlgItem(window, 0x5C2), BM_CLICK, 0, 0);
							}
							return FALSE;
						}

						default:
							return FALSE;
					}

				case 0x5C2: {
					WideWstring text;
					HWND edit = GetDlgItem(window, 0x540);
					FUN_00623EA0(edit, &text);
					SendMessage(edit, 0x4B2, 0, (LPARAM)DAT_00887734);
					Wstring name = FUN_0079DA90(GetDlgItem(window, 0x53F));
					if (name.length() > 0 && text.length() > 0) {
						text.trimTrailing(L"\n\r");
						User user;
						memset(&user, 0, sizeof(user));
						strcpy((char *)user.name, name.get());
						g_pChat->RequestUnicodePage(&user, text.get());
						wchar_t * message = text.get();
						FUN_005E2700(DAT_007EEF2C, L"[%hs] %s", g_NickName.c_str(), message);
					}
					EnableWindow((HWND)lparam, FALSE);
					InvalidateRect(edit, NULL, FALSE);
					return FALSE;
				}

				case 0x686:
					FUN_007757E0(window, 0x5C2);
					return FALSE;

				default:
					return FALSE;
			}

		case 0x497: {
			HWND edit = GetDlgItem(window, 0x540);
			SendMessage(edit, EM_LIMITTEXT, 0x78, 0);
			SendMessage(edit, 0x4B2, 0, (LPARAM)DAT_00887734);
			EnableWindow(GetDlgItem(window, 0x5C2), FALSE);
			Show_Name(window, FUN_0079DA90(GetDlgItem(window, 0x53F)));
			if (MouseCursor) {
				MouseCursor->Release_Mouse();
			}
			return FALSE;
		}

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		default:
			return FALSE;
	}
}

// FUNCTION: GAMEMD 0x0079DFE0
BOOL CALLBACK FUN_0079DFE0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x540:
					if (HIWORD(wparam) != 0) {
						Wstring name;
						FUN_00623E40((HWND)lparam, name);
						EnableWindow(GetDlgItem(window, 0x702), FUN_0079C520(&name));
					}
					return FALSE;

				case 0x686:
					control->F1(window, 0x686);
					return FALSE;

				case 0x702: {
					Wstring name;
					HWND edit = GetDlgItem(window, 0x540);
					FUN_00623E40(edit, name);
					SendMessage(edit, 0x4B4, 0, (LPARAM)DAT_00889F64);
					if (FUN_0079C520(&name)) {
						FUN_0079C8D0(&name);
					}
					InvalidateRect(edit, NULL, FALSE);
					control->F1(window, 0x702);
					return FALSE;
				}

				default:
					return FALSE;
			}

		case 0x497: {
			HWND edit = GetDlgItem(window, 0x540);
			SendMessage(edit, EM_LIMITTEXT, 9, 0);
			SendMessage(edit, 0x4D1, 1, 0);
			SendMessage(edit, 0x4B4, 0, (LPARAM)DAT_00889F64);
			EnableWindow(GetDlgItem(window, 0x702), FALSE);
			return FALSE;
		}

		case 0x4DA:
			PostMessage(GetDlgItem(window, 0x702), BM_CLICK, 0, 0);
			return FALSE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);

		case WM_ERASEBKGND:
			return TRUE;

		default:
			return FALSE;
	}
}

// FUNCTION: GAMEMD 0x0079E520
BOOL CALLBACK FUN_0079E520(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DESTROY:
			KillTimer(window, 0);
			KillTimer(window, 0x704);
			FUN_005EE1B0();
			return FALSE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case 0x497:
			{
				FUN_005EE350(window);
				HWND item = GetDlgItem(window, 0x689);
				SendMessage(item, 0x4A6, DAT_00B765F0, DAT_0084A1C4);
				SendMessage(item, 0x4A6, 0, DAT_0084A1C8);
				SendMessage(item, 0x4A6, 0, DAT_0084A1CC);
				SendMessage(item, 0x4A6, 0, DAT_0084A1D0);
				SendMessage(item, 0x4A6, DAT_00B73E3C, DAT_0084A1D4);
				SendMessage(item, 0x4A6, 0, DAT_0084A1D8);
				item = GetDlgItem(window, 0x704);
				SendMessage(item, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wouref.pcx", NULL));
				SendMessage(item, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("wodref.pcx", NULL));
				FUN_0079D4E0();
				SetTimer(window, 0, 1000, NULL);
			}
			return FALSE;

		case WM_COMMAND:
			if (control->F2()) {
				return FALSE;
			}
			switch (LOWORD(wparam)) {
				case 0x5C2:
					FUN_0079BE70();
					return FALSE;

				case 0x540:
					switch (HIWORD(wparam)) {
						case EN_CHANGE: {
							Wstring text;
							FUN_00623E40((HWND)lparam, text);
							EnableWindow(GetDlgItem(window, 0x702), text.length() != 0);
							return FALSE;
						}

						default:
							return FALSE;
					}

				case 0x62B:
					control->F1(window, 0x62B);
					return FALSE;

				case 0x686:
					control->F1(window, 0x686);
					return FALSE;

				case 0x62D:
					control->F1(window, 0x62D);
					return FALSE;

				case 0x75F:
				case 0x760:
					{
						int checked = 0;
						HWND item = GetDlgItem(window, 0x760);
						if (item) {
							checked = SendMessage(item, BM_GETCHECK, 0, 0);
						}
						if (LOWORD(wparam) == 0x75F) {
							checked = checked == 0;
						}
						Game::ObserverMode = checked;
						FUN_005EE350(NULL);
					}
					return FALSE;

				case 0x702:
					{
						SimpleWonlineDialogControl dialog;
						Run_Dialog_Control(0x115, FUN_0079DFE0, &dialog);
						FUN_0079AF10(window);
					}
					return FALSE;

				case 0x703:
					{
						HWND list = GetDlgItem(window, 0x689);
						int sel = SendMessage(list, LB_GETCURSEL, 0, 0);
						if (sel == -1) {
							return FALSE;
						}
						Wstring name;
						FUN_00623F00(list, sel, &name);
						FUN_0079CBA0(&name);
						SendMessage(list, LB_DELETESTRING, sel, 0);
						DAT_00B76E24.clear();
						FUN_0079AF10(window);
					}
					return FALSE;

				case 0x704:
					EnableWindow((HWND)lparam, FALSE);
					FUN_0079CE70();
					SetTimer(window, 0x704, 3000, NULL);
					return FALSE;

				case 0x689:
					switch (HIWORD(wparam)) {
						case LBN_SELCHANGE:
							{
								HWND list = (HWND)lparam;
								int sel = SendMessage(list, LB_GETCURSEL, 0, 0);
								if (sel == -1) {
									DAT_00B76E24.clear();
								} else {
									FUN_00623F00(list, sel, &DAT_00B76E24);
								}
								FUN_0079BF50(window);
								FUN_0079C040(window);
							}
							return FALSE;

						case LBN_DBLCLK:
							if (FUN_0079BC80()) {
								SendMessage(GetDlgItem(window, 0x62D), BM_CLICK, 0, 0);
								return FALSE;
							}
							if (FUN_0079BED0()) {
								SendMessage(GetDlgItem(window, 0x5C2), BM_CLICK, 0, 0);
								return FALSE;
							}
							return FALSE;

						default:
							return FALSE;
					}

				default:
					return FALSE;
			}

		case WM_TIMER:
			switch (wparam) {
				case 0:
					if ((unsigned int)(time(NULL) - DAT_00B778C8) > 30) {
						FUN_0079CE70();
					}
					return FALSE;

				case 0x704:
					KillTimer(window, wparam);
					EnableWindow(GetDlgItem(window, wparam), TRUE);
					return FALSE;

				default:
					return FALSE;
			}

		default:
			return FALSE;
	}
}
// FUNCTION: GAMEMD 0x0079F0A0
bool __fastcall FUN_0079F0A0(Wstring * name, ArrayList<Channel> * channels, Channel * result)
{
	memset(result, 0, sizeof(Channel));
	for (int i = 0; i < channels->length(); i++) {
		channels->get(*result, i);
		char * text = name->text();
		if (stricmp((char *)result->name, text) == 0) {
			return true;
		}
	}
	memset(result, 0, sizeof(Channel));
	return false;
}

BOOL CALLBACK FUN_0079E520(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
bool __fastcall FUN_00789B60(void);
void FUN_0072C060(void);
void FUN_0072C0B0(void);

// FUNCTION: GAMEMD 0x0079F140
int FUN_0079F140(bool first_time)
{
	DAT_00B77DC1 = 0;
	g_GameStarted = 0;
	FUN_0052F3F0();
	if (!FUN_0077F520(true)) {
		Session.unknown_10 = 0;
		return 0;
	}
	Leave_Channel(0x40, 1);
	DAT_00B76608.clear();

	while (true) {
		FUN_0072C060();
		SimpleWonlineDialogControl control;
		FUN_00798340();
		HWND dialog = UI::BeginDialog(0x116, FUN_0079E520, (int)&control);
		OwnerDraw::Display_Dialog(dialog);
		while (!control.F2()) {
			DWORD index;
			while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 100)) == WAIT_TIMEOUT && !control.F2()) {
				WOL_Wait_Callback();
			}
			ResetEvent(g_WaitEventHandles[index]);
			if (index == 0) {
				FUN_00792EC0(0xFFFFFFFF);
				UI::EndDialog(dialog);
				Session.unknown_10 = 0;
				ODMessageBox(StringTable::LoadString("TXT_YOURE_DISCON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6C7B), 0, FUN_0077D710, 0);
				FUN_0072C0B0();
				return 0;
			}
			if (index == 5) {
				FUN_00792EC0(0xFFFFFFFF);
				UI::EndDialog(dialog);
				Session.unknown_10 = 0;
				ODMessageBox(StringTable::LoadString("TXT_YOURE_DISCON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6C84), 0, FUN_0077D710, 0);
				FUN_0072C0B0();
				return 0;
			}
			Title_Screen_Restore(false);
		}
		UI::EndDialog(dialog);
		FUN_0072C0B0();

		switch (control.F3()) {
			case 0x62B:
				FUN_00798340();
				if (!FUN_00789B60()) {
					FUN_004068E0("Buddy List Host Create Game failed\n");
				} else {
					g_JoinLobbyNow = 0;
					do {
						DWORD index;
						do {
							index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20);
							if (index == WAIT_TIMEOUT) {
								WOL_Wait_Callback();
							}
						} while (index == WAIT_TIMEOUT && !g_JoinLobbyNow);
						ResetEvent(g_WaitEventHandles[index]);
						if (index == 0) {
							FUN_004068E0("EV_ABORT from Buddy List Host\n");
							g_JoinLobbyNow = 1;
						} else if (index == 6) {
							FUN_004068E0("EV_EXIT from Buddy List Host\n");
							g_JoinLobbyNow = 1;
						} else if (index == 15) {
							FUN_004068E0("Skirmish from Buddy List\n");
							g_GameStartRequested = false;
							g_LastErrorMessage[0] = 0;
							DAT_00B77DC1 = 0;
							return 2;
						} else if (index == 7) {
							FUN_004068E0("Start Game from Buddy List Host\n");
							g_GameStartRequested = false;
							g_pChat->RequestChannelLeave();
							g_LastErrorMessage[0] = 0;
							DAT_00B77DC1 = 1;
							g_JoinLobbyNow = 0;
							return 1;
						}
					} while (!g_JoinLobbyNow);
					FUN_004068E0("Stop Buddy Host\n");
					g_JoinLobbyNow = 0;
				}
				FUN_00792EC0(0xFFFFFFFF);
				FUN_00798340();
				if (g_LastErrorMessage[0]) {
					ODMessageBox(g_LastErrorMessage, 0, FUN_0077D710, 0);
				}
				while (FUN_007757E0(NULL, 0)) {
				}
				DAT_00B77DC1 = 0;
				break;

			case 0x62D: {
				Channel chan;
				memset(&chan, 0, sizeof(chan));
				if (DAT_00B76E24.length() > 0) {
					bool known = DAT_00B76608.getValue(DAT_00B76E24, chan);
					if (!known) {
						break;
					}
					WolEntryState state = WOL_ENTRY_STATE_NONE;
					if (!DAT_00B76990.getValue(DAT_00B76E24, state)) {
						break;
					}
					switch (state) {
						case 4: {
							Channel other;
							memset(&other, 0, sizeof(other));
							Session.unknown_10 = 4;
							for (int j = 0; j < g_UserChannels.length(); j++) {
								g_UserChannels.get(other, j);
								if (stricmp((char *)other.name, (char *)chan.name) == 0) {
									Set_Lobby_Number(chan);
									DAT_0084A210 = 0;
									break;
								}
							}
							return 0;
						}

						case 5: {
							Wstring name((char *)chan.name);
							Channel other;
							bool found = FUN_0079F0A0(&name, &g_GameChannelList, &other);
							if (!found) {
								bool refreshed = FUN_0077F710(0, true);
								if (!refreshed) {
									FUN_004068E0("Buddy List Guest Get Channel List failed\n");
									SetEvent(g_WaitEventHandles[5]);
								}
								found = refreshed ? FUN_0079F0A0(&name, &g_GameChannelList, &other) : false;
							}
							if (!found) {
								FUN_004068E0("Buddy List Guest Find Channel by name failed\n");
							} else if (!FUN_00789960(&other, Game::ObserverMode != 0)) {
								FUN_004068E0("Buddy List Guest Join_Game failed\n");
							} else {
								g_JoinLobbyNow = 0;
								int timeout = 500;
								while (true) {
									DWORD index;
									while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20)) == WAIT_TIMEOUT && !g_JoinLobbyNow) {
										WOL_Wait_Callback();
									}
									ResetEvent(g_WaitEventHandles[index]);
									if (index == 0) {
										FUN_004068E0("EV_ABORT from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									else if (index == 6) {
										FUN_004068E0("EV_EXIT from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									else if (index == 5) {
										FUN_004068E0("EV_ERROR from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									else if (index == 10) {
										FUN_004068E0("EV_CHAN_JOIN_FAIL from Buddy List Guest\n");
										g_JoinLobbyNow = 1;
									}
									if (index == 7) {
										FUN_004068E0("EV_STARTGAME from Buddy List Guest\n");
										g_GameStartRequested = false;
										g_pChat->RequestChannelLeave();
										g_LastErrorMessage[0] = 0;
										DAT_00B77DC1 = 1;
										g_JoinLobbyNow = 0;
										return 1;
									}
									if (timeout-- < 0) {
										FUN_004068E0("Timeout from Buddy List Guest\n");
										break;
									}
									if (g_JoinLobbyNow) {
										break;
									}
								}
								g_JoinLobbyNow = 0;
								FUN_004068E0("Exit from BuddyList Guest\n");
							}
							break;
						}
					}
				}
				break;
			}

			case 0x5C0:
			case 0x686:
				if (Session.unknown_10 == 4) {
					DAT_0084A210 = 0;
					return 0;
				}
				return 0;
		}
	}
}

// FUNCTION: GAMEMD 0x0079FC10
void __fastcall FUN_0079FC10(int a, int b, int c)
{
	DAT_00B77E14 = c;
	DAT_00B73A20 = a;
	DAT_00B77D70 = b;
	DAT_00B77A88 = time(NULL);
}

bool FUN_0052F3F0(void);
void FUN_0072BE80(void);
void FUN_0072BED0(void);
void FUN_0079CE70(void);
void FUN_007A0430(void);
bool FUN_007A0690(void);
void FUN_007A1060(void);
bool FUN_007A1BB0(void);
BOOL CALLBACK FUN_007A2110(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK FUN_007A2600(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x0079FC40
int FUN_0079FC40(void)
{
	if (Game::ObserverMode) {
		Game::ObserverMode = 0;
		FUN_005EE350(0);
	}
	DAT_00B77DC1 = 0;
	FUN_0079FC10(1, -1, 0);
	FUN_0078ABF0(true, StringTable::LoadString("TXT_AWAY_QUICKMATCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6D63));
	g_GameStarted = 0;
	FUN_0052F3F0();
	FUN_0079FC10(2, 30, 0);
	g_UserChannels.clear();
	DAT_00B76AC4.clear();
	if (!FUN_0077F520(true)) {
		FUN_0078ABF0(false, L"");
		return 0;
	}
	if (g_UserChannels.length() == 0) {
		ODMessageBox(StringTable::LoadString("WOL:NoMatchingAgentFound", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6D8D), 0, FUN_0077D710, 0);
		FUN_0078ABF0(false, L"");
		return 0;
	}

	Channel chan;
	memset(&chan, 0, sizeof(chan));
	char const * prefix = "Lob_39_";
	if (Session.WOLWelcomeChoice != 2) {
		prefix = "Lob_40_";
	}
	int i;
	for (i = 0; i < g_UserChannels.length(); i++) {
		g_UserChannels.get(chan, i);
		if (strnicmp((char *)chan.name, prefix, strlen(prefix)) == 0) {
			break;
		}
	}
	if (i >= g_UserChannels.length()) {
		ODMessageBox(StringTable::LoadString("WOL:MatchingServerUnavailable", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6DA5), 0, FUN_0077D710, 0);
		return 0;
	}

	FUN_0072BE80();
	HWND dialog = WS_Create_Dialog(ProgramInstance, 0x117, MainWindow, FUN_007A2110, FALSE);
	UI::RegisterWindow(dialog, 0x117);
	SendMessage(dialog, 0x4A9, 0, 1);
	ShowWindow(dialog, SW_SHOWNORMAL);
	FUN_00798340();
	int result = 0;
	FUN_0079D4E0();

	while (true) {
		if (DAT_00B73A20 == 2) {
			long joined;
			{
				Wstring key("zotclot9");
				joined = FUN_00788800(&chan, key, WideWstring(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6DBF)));
			}
			if (joined != 0) {
				FUN_0079FC10(0, -1, 1);
				ODMessageBox(StringTable::LoadString("WOL:ChannelJoinFailure", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6DC2), 0, FUN_0077D710, 0);
				goto cleanup;
			}
			FUN_0079FC10(3, 20, 0);
		}
		if (DAT_00B73A20 == 3) {
			if (strcmp((char *)chan.name, (char *)g_CurrentChannel.name) == 0) {
				FUN_007A0690();
			}
		}
		DWORD index;
		while ((index = WaitForMultipleObjects(18, g_WaitEventHandles, FALSE, 20)) == WAIT_TIMEOUT) {
			WOL_Wait_Callback();
		}
		Reset_Wait_Event_At(index);
		if (index == 0 || index == 6) {
			goto cleanup;
		}
		if (index == 0x11) {
			FUN_007A0430();
			FUN_007A1060();
			continue;
		}
		if (index == 5) {
			if (DAT_00B73A20 == 0) {
				Leave_Channel(0x40, 0);
				switch (DAT_00B77E14) {
					case 1:
						ODMessageBox(StringTable::LoadString("WOL:NoMatchingAgentFound", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6E0E), 0, FUN_0077D710, 0);
						break;
					case 2:
						ODMessageBox(StringTable::LoadString("WOL:MatchTimeout", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6E14), 0, FUN_0077D710, 0);
						break;
					default:
						ODMessageBox(StringTable::LoadString("WOL:MatchFailExit", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6E19), 0, FUN_0077D710, 0);
						break;
				}
cleanup:
				FUN_0078ABF0(false, L"");
				goto done;
			}
			continue;
		}
		if (index == 0x10) {
			if (FUN_007A1BB0() && Session.WOLWelcomeChoice == 3) {
				HWND match = UI::BeginDialog(0x11F, FUN_007A2600, 0);
				FUN_0079FC10(10, 180, 0);
				SendMessage(match, 0x4A9, 0, 1);
				ShowWindow(match, SW_SHOWNORMAL);
			}
			continue;
		}
		if (index == 7) {
			break;
		}
	}

	FUN_005E2700(DAT_007EEF28, StringTable::LoadString("WOL:MatchGameStarting", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6E33));
	g_GameStartRequested = 0;
	result = 1;

done:
	FUN_007757E0(dialog, 0);
	FUN_0072BED0();
	HWND match = WS_Find_Dialog(0x11F);
	if (match) {
		FUN_007757E0(match, 0);
	}
	if (DAT_00B73A20 != 11) {
		FUN_0079FC10(1, -1, 0);
	}
	Leave_Channel(0x40, 1);
	if (MultiplayerMapPreview) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = NULL;
	}
	return result;
}

// FUNCTION: GAMEMD 0x007A0430
void FUN_007A0430(void)
{
	User user;
	memset(&user, 0, sizeof(user));
	FUN_004068E0("User list is %d long, adding to ping request queue\n", g_UserList.length());
	for (int i = 0; i < g_UserList.length(); i++) {
		g_UserList.get(user, i);
		if (user.flags & 1) {
			FUN_004068E0("%s is channel operator, no ping request\n", user.name);
		} else if (stricmp((char *)user.name, g_NickName.get()) == 0) {
			FUN_004068E0("%s is local user, no ping request\n", user.name);
		} else {
			Wstring name((char *)user.name);
			FUN_004068E0("Adding %s to ping request queue\n", name.get());
			DAT_00B76AD0.addTail(name);
		}
	}
}

int FUN_005355D0(void);
rc_ptr<WorldDominationTour::Campaign> * __cdecl WDT_Get_New_Campaign(void);
extern int DAT_007EEF30;
extern int DAT_00B736F4;

// FUNCTION: GAMEMD 0x007A0690
bool FUN_007A0690(void)
{
	DAT_00B76AC4.clear();
	HWND dialog = WS_Find_Dialog(0x11F);
	if (dialog) {
		FUN_007757E0(dialog, 0);
	}
	g_GameStarted = 0;
	FUN_0052F3F0();
	if (g_UserList.length() < 2) {
		FUN_0079FC10(0, -1, 1);
		return false;
	}
	DAT_00B76AD0.clear();
	DAT_00B76970.clear();
	DAT_00B736F4 = 0;
	FUN_007A0430();

	User user;
	memset(&user, 0, sizeof(user));
	int host_index = -1;
	FUN_004068E0("User list is %d long, adding to ping request queue\n", g_UserList.length());
	for (int i = 0; i < g_UserList.length(); i++) {
		g_UserList.get(user, i);
		if (user.flags & 1) {
			host_index = i;
		}
	}
	if (host_index == -1) {
		FUN_0079FC10(0, -1, 1);
		return false;
	}

	User host;
	memset(&host, 0, sizeof(host));
	g_UserList.get(host, host_index);
	host.next = NULL;

	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) {
		profile = FUN_00778A20(&g_NickName);
	} else {
		profile = DAT_00B75E48;
	}
	int country = profile.Get()->Country;
	Wstring game("Match ");
	Wstring wdt;

	if (Session.WOLWelcomeChoice == 3) {
		game = "MatchWDT ";
		rc_ptr<WorldDominationTour::Campaign> campaign = *WDT_Get_New_Campaign();
		if (campaign.Get() == NULL) {
			wdt = ", WDS=0";
		} else {
			switch (campaign.Get()->PlayerFaction) {
				case 2:
					wdt = ", WDS=0";
					if (country > 4) {
						country = -1;
					}
					break;

				case 3:
					wdt = ", WDS=1";
					if (country >= 0 && country <= 4) {
						country = -1;
					}
					break;

				default:
					FUN_0079FC10(0, -1, 0);
					return false;
			}
		}
		wdt += ", WDT=";
		wdt += Wstring_FromInt(Session.WDTTerritory);
	}

	FUN_005E2700(DAT_007EEF30, StringTable::LoadString("WOL:RequestingMatch", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6EEB));

	Wstring request(game);
	request += "COU=";
	request += Wstring_FromInt(country);
	request += ", COL=";
	request += Wstring_FromInt(profile.Get()->Field20);
	request += ", MBR=";
	if (profile.Get()->MatchByRes) {
		request += "1";
	} else {
		request += "0";
	}

	Wstring nick(g_NickName);
	nick.toLower();
	if (g_UserLocales.contains(nick)) {
		int locale = 0;
		g_UserLocales.getValue(nick, locale);
		request += ", LOC=";
		request += Wstring_FromInt(locale);
	}
	request += ", RAT=";
	request += Wstring_FromInt(FUN_005355D0());
	request += ", RES=";
	request += Wstring_FromInt(Options.ScreenWidth);

	NetNumType net;
	NetNodeType node;
	unsigned short port;
	Session.LocalAddress.Get_Address(net, node, &port);
	int address = *(int *)node;
	request += ", ADR=";
	request += Wstring_FromInt(address);
	request += ", NAT=";
	request += Wstring_FromInt((unsigned short)FirewallHelper.Detected);
	request += ", PRT=";
	request += Wstring_FromInt(Session.PortBase);
	request += wdt;

	g_pChat->RequestPrivateMessage(&host, request.get());
	FUN_0079FC10(4, 180, 0);
	FUN_007A1060();
	return true;
}

// FUNCTION: GAMEMD 0x007A0D10
void FUN_007A0D10(void)
{
	if (g_GameStarted == 0) {
		User user;
		memset(&user, 0, sizeof(user));
		int count = g_UserList.length();
		int handle = 0;
		for (int i = 0; i < count; i++) {
			g_UserList.get(user, i);
			in_addr addr;
			addr.s_addr = user.ipaddr;
			FUN_004068E0("Requesting ping for %s at %s\n", user.name, inet_ntoa(addr));
			g_pNetUtil->RequestPing(inet_ntoa(addr), 1000, &handle);
		}
	}
}

// FUNCTION: GAMEMD 0x007A0DD0
void __fastcall FUN_007A0DD0(User * user)
{
	if (user) {
		for (int i = 0; i < 8; i++) {
			if (Session.Slots[i] && strcmp(FUN_007350C0(Session.Slots[i]->Name), (char *)user->name) == 0) {
				HWND window = FUN_005E26C0();
				if (window) {
					FUN_004E34A0(window, i, -1);
				}
				return;
			}
		}
	}
}

void __fastcall FUN_007A0EA0(char const * name, int ping);

// FUNCTION: GAMEMD 0x007A0E50
void FUN_007A0E50(void)
{
	for (int i = 0; i < 8; i++) {
		if (!Session.Slots[i] || !FUN_005E93F0(Session.Slots[i])) {
			DAT_00B76440[i] = -1;
		}
		char const * name = FUN_007350C0(Session.Slots[i]->Name);
		FUN_007A0EA0(name, DAT_00B76440[i]);
	}
}

// FUNCTION: GAMEMD 0x007A0EA0
void __fastcall FUN_007A0EA0(char const * name, int ping)
{
	FUN_004068E0("Got ping from gameoptions.  name = %s, ping = %d\n", name, ping);
	for (int i = 0; i < 8; i++) {
		if (Session.Slots[i] && strcmp(FUN_007350C0(Session.Slots[i]->Name), name) == 0) {
			DAT_00B76440[i] = ping;
			HWND window = FUN_005E26C0();
			if (window) {
				if (ping < 0) {
					FUN_004E34A0(window, i, -1);
				} else if (ping < 300) {
					FUN_004E34A0(window, i, 0);
				} else if (ping < 500) {
					FUN_004E34A0(window, i, 1);
				} else {
					FUN_004E34A0(window, i, 2);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007A0F40
void FUN_007A0F40(void)
{
	int count = g_UserList.length();
	User user;
	memset(&user, 0, sizeof(user));
	char list[0x100];
	list[0] = 'P';
	list[1] = (char)(count + '0');
	list[2] = 0;
	if (g_GameStarted == 0) {
		for (int i = 0; i < count; i++) {
			g_UserList.get(user, i);
			int ping;
			g_pNetUtil->GetAvgPing(user.ipaddr, &ping);
			char entry[0x80];
			sprintf(entry, ",%s,%d", user.name, ping);
			strcat(list, entry);
			FUN_007A0EA0((char *)user.name, ping);
		}
		FUN_004068E0("Sending pings for users: %s\n", list);
		FUN_005E3720(list);
	}
}

// FUNCTION: GAMEMD 0x007A1060
void FUN_007A1060(void)
{
	bool failed = false;
	User user;
	memset(&user, 0, sizeof(user));
	if (g_GameStarted == 0) {
		FUN_004068E0("Requesting match pings, request list is %d long, pending list is %d long\n", DAT_00B76AD0.length(), DAT_00B76970.length());
		if (DAT_00B76AD0.length() != 0) {
			unsigned long ip = user.ipaddr;
			while (DAT_00B76AD0.length() != 0) {
				if (DAT_00B76970.length() >= 10 || failed) {
					break;
				}
				{
					Wstring name;
					DAT_00B76AD0.getHead(name);
					FUN_004068E0("Got %s from request list\n", name.get());
					int handle;
					int index;
					{
						Wstring key(name.get());
						index = Player_Name_To_Index(key);
					}
					if (index != -1) {
						g_UserList.get(user, index);
						ip = user.ipaddr;
						if (ip != 0) {
							FUN_004068E0("Pinging %s\n", name.get());
							in_addr addr;
							addr.s_addr = ip;
							if (g_pNetUtil->RequestPing(inet_ntoa(addr), 1000, &handle) == 0 && handle >= 0) {
								DAT_00B76970.addTail(name);
								DAT_00B76AD0.removeHead(name);
								FUN_004068E0("Removing %s from request list\n", name.get());
							} else {
								failed = true;
							}
						} else {
							DAT_00B76AD0.removeHead(name);
							FUN_004068E0("%s ipaddr is zero\n", name.get());
						}
					} else {
						if (DAT_00B76AD0.getHead(name)) {
							DAT_00B76AD0.remove(0);
						}
						FUN_004068E0("%s not found in info\n", name.get());
					}
				}
				if (DAT_00B76AD0.length() == 0) {
					break;
				}
			}
		}
	}
}

extern int DAT_00B736F4;

// FUNCTION: GAMEMD 0x007A1330
void FUN_007A1330(void)
{
	int count = 0;
	User user;
	memset(&user, 0, sizeof(user));
	if (g_GameStarted == 0) {
		Wstring pings("Pings ");
		FUN_004068E0("Sending ping list, pending list is %d long\n", DAT_00B76970.length());
		if (DAT_00B76970.length() != 0) {
			unsigned long ip = user.ipaddr;
			do {
				Wstring name;
				DAT_00B76970.remove(name, DAT_00B76970.length() - 1);
				FUN_004068E0("Removed %s from ping pending list\n", name.get());
				int index;
				{
					Wstring key(name.get());
					index = Player_Name_To_Index(key);
				}
				if (index == -1) {
					FUN_004068E0("%s not found in info\n", name.get());
				} else {
					g_UserList.get(user, index);
					ip = user.ipaddr;
					if (ip == 0) {
						FUN_004068E0("%s ipaddr is zero\n", name.get());
					} else {
						int ping = -1;
						g_pNetUtil->GetAvgPing(ip, &ping);
						if (ping != -1) {
							FUN_004068E0("%s avg ping is %d\n", name.get(), ping);
							pings += name + Wstring(",") + Wstring_FromInt(ping) + Wstring(";");
							count++;
						} else {
							FUN_004068E0("Error measuring %s/'s ping\n", name.get());
						}
					}
				}
			} while (DAT_00B76970.length() != 0);
		}
		DAT_00B736F4 = 0;
		if (count > 0) {
			User * host = Get_Channel_Host();
			if (host) {
				g_pChat->RequestPrivateMessage(host, pings.get());
				FUN_004068E0("Sending ping list : %s\n", pings.get());
			}
		}
	}
}

extern char DAT_00B73714[];

static inline bool Match_Failed(char const * message)
{
	if (strnicmp(message, "Fail", 4) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFail", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6FFF));
		SetEvent(g_WaitEventHandles[17]);
		return true;
	}
	if (strnicmp(message, "PingFail", 8) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailPing", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7005));
		SetEvent(g_WaitEventHandles[17]);
		return true;
	}
	if (strnicmp(message, "RankFail", 8) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailRank", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x700B));
		SetEvent(g_WaitEventHandles[17]);
		return true;
	}
	if (strnicmp(message, "WDTSideFail", 11) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailWDTSide", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7011));
		SetEvent(g_WaitEventHandles[17]);
		return true;
	}
	return false;
}

// FUNCTION: GAMEMD 0x007A16B0
void __fastcall FUN_007A16B0(char const * message, bool strict)
{
	if (strict && Match_Failed(message)) {
		return;
	}
	if (strnicmp(message, "Exit", 4) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailExit", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7017));
		g_pChat->RequestChannelLeave();
		ODMessageBox(StringTable::LoadString("WOL:MatchFailExit", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x701A), 0, FUN_0077D710, 0);
		SetEvent(g_WaitEventHandles[0]);
		return;
	}
	if (strnicmp(message, "Working", 7) == 0) {
		if (DAT_00B73A20 == 4) {
			FUN_0079FC10(5, 0xB4, 0);
		}
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("GUI:MatchingProgress2", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7026));
		return;
	}
	if (strnicmp(message, "Info ", 5) == 0) {
		FUN_005E2700(0xFF8080, FUN_00735120(message + 5));
		return;
	}
	if (strnicmp(message, "Pool ", 5) == 0) {
		message += 5;
		FUN_005E2700(0xFF8080, StringTable::LoadString("WOL:MatchingPlayerPool", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7030), message);
		return;
	}
	if (strict && strnicmp(message, "Start ", 6) == 0) {
		message += 6;
		strncpy(DAT_00B73714, message, 0xFF);
		FUN_004068E0("%s\n", DAT_00B73714);
		SetEvent(g_WaitEventHandles[16]);
		return;
	}
	if (stricmp(message, "ack") == 0 || stricmp(message, "nack") == 0) {
		return;
	}
	if (strnicmp(message, "need ", 5) == 0) {
		FUN_004068E0("%s\n", message);
		return;
	}
	if (strict) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchUnknownServerCommand", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7051));
	}
}

// FUNCTION: GAMEMD 0x007A19E0
void FUN_007A19E0(void)
{
	FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchFoundWith", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7066), DAT_00B76AC4.c_str());
	Wstring lower = ((Wstring const &)DAT_00B76AC4).toLower();
	Ladder * found = NULL;
	g_GeneralLadderDict.getPointer(lower, &found);
	int index;
	{
		Wstring key(DAT_00B76AC4.get());
		index = Player_Name_To_Index(key);
	}
	if (index == -1) {
		FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPingUnknown", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7073));
	} else {
		User user;
		memset(&user, 0, sizeof(user));
		g_UserList.get(user, index);
		if (user.ipaddr == 0) {
			FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPingUnknown", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x707A));
		} else {
			int ping = -1;
			g_pNetUtil->GetAvgPing(user.ipaddr, &ping);
			if (ping != -1) {
				FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPing", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7080), ping);
			} else {
				FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPingUnknown", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7082));
			}
		}
	}
}

bool __fastcall FUN_00789B60(void);

// FUNCTION: GAMEMD 0x007A1BB0
bool FUN_007A1BB0(void)
{
	g_GameStarted = 0;
	FUN_0052F3F0();
	char buffer[256];
	strncpy(buffer, DAT_00B73714, sizeof(buffer));
	strtok(buffer, ":");
	char * first = strtok(NULL, ",");
	strtok(NULL, ",");
	strtok(NULL, ",");
	strtok(NULL, ",");
	strtok(NULL, ",");
	strtok(NULL, ",");
	strtok(NULL, ",");
	strtok(NULL, ",");
	char * second = strtok(NULL, ",");
	if (first && second) {
		if (stricmp(first, g_NickName.get()) == 0 || stricmp(second, g_NickName.get()) == 0) {
			if (stricmp(first, g_NickName.get()) == 0) {
				DAT_00B76AC4 = second;
				FUN_007A19E0();
				PMessagePrintf(DAT_007EEF28, StringTable::LoadString("WOL:MatchCreatingGame", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70C3));
				g_GameStartRequested = false;
				FUN_0079FC10(6, 30, 0);
				if (FUN_00789B60()) {
					PMessagePrintf(DAT_007EEF28, StringTable::LoadString("WOL:MatchWaitingForJoin", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70C7), second);
					return true;
				}
				PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchErrorCreatingGame", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70CE));
				FUN_0079FC10(2, 30, 0);
			} else {
				DAT_00B76AC4 = first;
				FUN_007A19E0();
				Wstring game_name = Wstring(first) + Wstring("'s_game");
				PMessagePrintf(DAT_007EEF28, StringTable::LoadString("WOL:MatchAttemptingToFindGame", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70D5), first);
				FUN_0079FC10(7, 30, 0);
				int attempts = 1;
				for (;;) {
					if (!FUN_0077F710(0, false) || !FUN_0077F120()) {
						PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchGameChannelNotFound", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70D9), first);
						FUN_0079FC10(3, 20, 0);
						return false;
					}
					Channel channel;
					memset(&channel, 0, sizeof(channel));
					for (int i = 0; i < g_GameChannelList.length(); i++) {
						g_GameChannelList.get(channel, i);
						if (stricmp((char *)channel.name, game_name.get()) == 0) {
							if (FUN_00789960(&channel, false)) {
								PMessagePrintf(DAT_007EEF28, StringTable::LoadString("WOL:MatchJoinedGame", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70E3), first);
								FUN_0079FC10(9, 90, 0);
								return true;
							}
							PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchErrorJoiningGame", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70EB));
							return false;
						}
					}
					WOL_Wait_Callback();
					Sleep(100);
					if (attempts++ >= 30) {
						PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchGameChannelNotFound", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70F3), first);
						FUN_0079FC10(2, 30, 0);
						break;
					}
				}
			}
		} else {
			PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchBadParameters", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70B9));
			FUN_0079FC10(3, 20, 0);
		}
	} else {
		PMessagePrintf(0x20C0FF, StringTable::LoadString("WOL:MatchBadParameters", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x70AF));
		FUN_0079FC10(3, 20, 0);
	}
	return false;
}


void __fastcall FUN_005EC880(HWND dialog, int count, bool full, bool ai);
void FUN_005E2AC0(void);
bool FUN_00779940(void);

// FUNCTION: GAMEMD 0x007A2110
BOOL CALLBACK FUN_007A2110(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DESTROY:
			KillTimer(window, 0);
			return FALSE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case 0x5C0:
					if (DAT_00B73A20 == 0xB) {
						FUN_007757E0(window, 0);
						return FALSE;
					}
					SetEvent(g_WaitEventHandles[0]);
					break;

				case 0x708:
					g_pChat->RequestPublicMessage("start 4439,0,0,10000,10,1,1,1,3,Match,1,0,MP18S3.MAP,Match:mshag,4,1,xyzkerble,7,4");
					return FALSE;

				case 0x709:
					g_pChat->RequestPublicMessage("start 4439,0,0,10000,10,1,1,1,3,Match,1,0,MP18S3.MAP,Match:xyzkerble,7,4,mshag,4,1,");
					return FALSE;
			}
			break;

		case WM_TIMER:
			if (wparam == 0) {
				FUN_005E2AC0();
				return FALSE;
			}
			break;

		case 0x497:
			SetTimer(window, 0, 2000, NULL);
			return FALSE;
	}
	return FALSE;
}


// FUNCTION: GAMEMD 0x007A2230
void __fastcall FUN_007A2230(char * name, char * ip, unsigned long second, unsigned long third)
{
	if (!name || !ip || !second || !third) {
		return;
	}
	char * third_text = (char *)third;
	char * second_text = (char *)second;
	unsigned long third_value;
	second = 0;
	third = 0;
	third_value = 0;
	sscanf(ip, "%X", &second);
	sscanf(second_text, "%X", &third);
	sscanf(third_text, "%X", &third_value);
	for (int i = 0; i < 8; i++) {
		if (stricmp(name, FUN_007350C0(Session.Slots[i]->Name)) == 0) {
			unsigned long net = 0;
			unsigned char node[6];
			memset(node, 0, 6);
			*(unsigned long *)node = second;
			Session.PlayerLocalAddress[i].Set_Address((unsigned char *)&net, node, 0);
			Session.PlayerFirewall[i] = third;
			Session.PlayerPortBase[i] = third_value;
			return;
		}
	}
}


#include "preview.h"
#include "progress.h"
#include "mapgen.h"
void __fastcall FUN_00607FD0(HWND window);

// FUNCTION: GAMEMD 0x007A2600
BOOL CALLBACK FUN_007A2600(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	BOOL rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	switch (message) {
		case WM_COMMAND:
			if (LOWORD(wparam) == 0x5C0) {
				SetEvent(g_WaitEventHandles[0]);
				FUN_007757E0(window, 0x5C0);
			}
			break;

		case WM_DESTROY:
			if (MultiplayerMapPreview) {
				delete MultiplayerMapPreview;
				MultiplayerMapPreview = NULL;
			}
			return FALSE;

		case WM_PAINT:
			{
				OwnerDraw::Paint(window);
				FUN_00607FD0(window);
				if (MultiplayerMapPreview != NULL) {
					MultiplayerMapPreview->DrawStartPoints(window);
				} else if (Session.Options.ScenarioIndex == -1 && RandomMapGen.MapPreview != NULL) {
					RandomMapGen.MapPreview->DrawStartPoints(window);
				}
				if (Progress.Has_Dialog()) {
					Progress.Display_Progress();
				}
				ValidateRect(window, NULL);
			}
			return TRUE;

		case WM_ERASEBKGND:
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case 0x5C0:
			return FALSE;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x007A2730
void FUN_007A2730(void)
{
	if (g_pChat) {
		g_pChat->RequestPublicGameOptions("G");
	}
}

// FUNCTION: GAMEMD 0x007A2750
void FUN_007A2750(void)
{
	HWND dialog = FUN_00775B10();
	HWND list = GetDlgItem(dialog, 0x59F);
	bool notHost;
	{
		Wstring empty((char *)NULL);
		notHost = GetPlayerAccepted(empty) != 1;
	}
	if (notHost) {
		SendMessage(list, 0x4DC, 0, 1);
		FUN_005E2700(-1, StringTable::LoadString("GUI:HostGameStartJoiner", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7212));
	}
}

// FUNCTION: GAMEMD 0x007A27D0
void FUN_007A27D0(void)
{
	HWND dialog = FUN_00775B10();
	HWND list = GetDlgItem(dialog, 0x59F);
	SendMessage(list, 0x4DC, 0, 0);
}

// FUNCTION: GAMEMD 0x007A2800
void __fastcall FUN_007A2800(int count, bool full, bool ai)
{
	HWND dialog = WS_Find_Dialog(0xC2);
	FUN_005EC880(dialog, count, full, ai);
}

// FUNCTION: GAMEMD 0x007A2830
void __fastcall FUN_007A2830(Wstring * name)
{
	User user;
	user.flags = 0;
	for (int i = 0; i < g_UserList.length(); i++) {
		g_UserList.get(user, i);
		if (stricmp(name->text(), (char *)user.name) == 0) {
			g_pChat->RequestUserKick(&user);
			return;
		}
	}
}

// FUNCTION: GAMEMD 0x007A28B0
void __fastcall FUN_007A28B0(wchar_t const * wide)
{
	if (wide) {
		User user;
		Wstring name = FUN_007B7F90(wide);
		user.flags = 0;
		for (int i = 0; i < g_UserList.length(); i++) {
			g_UserList.get(user, i);
			if (stricmp(name.text(), (char *)user.name) == 0) {
				g_pChat->RequestUserKick(&user);
				break;
			}
		}
	}
}

BOOL CALLBACK FUN_007A29E0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x007A2970
void FUN_007A2970(void)
{
	char busy = g_ConnectionLost;
	DAT_00B77DC1 = 0;
	if (!busy && FUN_00779940() && FUN_0079C520(&DAT_00B76AC4)) {
		SimpleWonlineDialogControl control;
		Run_Dialog_Control(0x122, FUN_007A29E0, &control);
	}
}


void __fastcall FUN_00779A40(bool value);

// FUNCTION: GAMEMD 0x007A29E0
BOOL CALLBACK FUN_007A29E0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	SimpleWonlineDialogControl * control = (SimpleWonlineDialogControl *)GetWindowLong(window, 8);
	wchar_t buffer[128];
	switch (message) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT:
			OwnerDraw::Paint(window);
			FUN_00607FD0(window);
			ValidateRect(window, NULL);
			return TRUE;

		case WM_DRAWITEM:
			OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDOK:
					{
						bool checked = IsDlgButtonChecked(window, 0x6F9) == 1;
						FUN_00779A40(!checked);
					}
					FUN_0079C8D0(&DAT_00B76AC4);
					control->F1(window, 1);
					return FALSE;

				case IDCANCEL:
					{
						bool checked = IsDlgButtonChecked(window, 0x6F9) == 1;
						FUN_00779A40(!checked);
					}
					control->F1(window, 2);
					return FALSE;
			}
			break;

		case 0x497:
			{
				swprintf(buffer, StringTable::LoadString("WOL:AddBuddyQuery", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x726B), DAT_00B76AC4.get());
				HWND item = GetDlgItem(window, 0x71B);
				SendMessage(item, 0x4B2, 0, (LPARAM)buffer);
			}
			return FALSE;
	}
	return FALSE;
}

// SYNTHETIC: GAMEMD 0x007A2E70
// ??_GSimpleWonlineDialogControl@@UAEPAXI@Z

struct AtlObjectEntry {
	GUID const * Clsid;
	void * UpdateRegistry;
	void * GetClassObject;
	void * CreateInstance;
	IUnknown * Factory;
	DWORD Register;
	void * GetObjectDescription;
	void * GetCategoryMap;
	void (__stdcall * ObjectMain)(BOOL start);
};


extern int g_AgeCheckResult;
extern int g_SuspendChatPump;
extern HANDLE DAT_00B736A4;
extern long g_OnNewNickResult;
extern int g_UserConsent;
extern int g_PingsSent;
extern int g_PingsReceived;
extern int DAT_00B736F4;
void FUN_007A1330(void);
void FUN_007A1060(void);
void __cdecl FUN_004068E0(char const * format, ...);
void FUN_006C6D00(void);
void FUN_006C6DE0(void);
rc_ptr<WDTState> __fastcall FUN_0076ABA0(void * data);
void __fastcall WDT_Resume_Campaign(rc_ptr<WDTState> const & state);
void __fastcall WDT_Start_New_Campaign(rc_ptr<WDTState> const & state);

// FUNCTION: GAMEMD 0x007A2EA0
// AtlAdvise

static inline AtlObjectEntry * _NextObjectMapEntry(AtlModule * module, AtlObjectEntry * entry)
{
	if (module->cbSize == 0x64) {
		return (AtlObjectEntry *)((char *)entry + 0x1C);
	}
	return entry + 1;
}

// FUNCTION: GAMEMD 0x007A2F30
long __stdcall AtlModuleInit(AtlModule * module, void * objects, HINSTANCE instance)
{
	if (module == NULL) {
		return E_INVALIDARG;
	}
	module->Entries = (AtlObjectEntry *)objects;
	module->Instance = module->TypeLibInstance = module->ResourceInstance = instance;
	module->LockCount = 0;
	module->Heap = NULL;
	InitializeCriticalSection(&module->Lock0);
	InitializeCriticalSection(&module->Lock1);
	InitializeCriticalSection(&module->Lock2);
	module->CreateWndList = NULL;
	module->OwnsHeaps = true;
	module->HeapCount = 0;
	module->HeapIndex = 0;
	module->Heaps = NULL;
	module->ExitList = NULL;
	if (module->Entries != NULL) {
		AtlObjectEntry * entry = module->Entries;
		while (entry->Clsid != NULL) {
			entry->ObjectMain(TRUE);
			entry = _NextObjectMapEntry(module, entry);
		}
	}
	return S_OK;
}

// FUNCTION: GAMEMD 0x007A2FC0
CNetUtilEventSink::CNetUtilEventSink(void)
{
}

// FUNCTION: GAMEMD 0x007A2FE0
HRESULT CNetUtilEventSink::OnWDTState(HRESULT result, unsigned char * data, int length)
{
	if (result < 0 || length <= 0) {
		SetEvent(g_WaitEventHandles[5]);
		return 0;
	}
	rc_ptr<WDTState> state = FUN_0076ABA0(data);
	if (state.Get() != NULL && state->Has_Owner_History()) {
		if (state->Is_Previous_Cycle()) {
			FUN_004068E0("OnWDTState: Previous cycle data received\n");
			WDT_Resume_Campaign(state);
			SetEvent(g_WaitEventHandles[11]);
		} else {
			FUN_004068E0("OnWDTState: Current cycle data received\n");
			g_NextWDTStatePollTime = time(NULL) + state->TickTime;
			WDT_Start_New_Campaign(state);
			SetEvent(g_WaitEventHandles[11]);
		}
	} else {
		FUN_004068E0("OnWDTState: NACK received from server\n");
		SetEvent(g_WaitEventHandles[11]);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A30D0
HRESULT CNetUtilEventSink::OnGameresSent(HRESULT result)
{
	FUN_004068E0("OnGameresSent  %d\n", result);
	if (result == 0) {
		FUN_006C6D00();
	}
	FUN_006C6DE0();
	return 0;
}

static inline void Remove_Highscore(Wstring & name)
{
	DAT_00B76640.remove(name);
}

// FUNCTION: GAMEMD 0x007A3B30
HRESULT CNetUtilEventSink::OnHighscore(HRESULT result, Highscore * list, int, long, int)
{
	if (result == 0) {
		Highscore * temp = list;
		Wstring ladder_name("");
		while (temp != NULL) {
			ladder_name.set((char const *)temp->login_name);
			ladder_name.toLower();
			FUN_004068E0("Hey, I just got %s's high score, which happens to be %d. Guess what, his all-time highest rank is %d\n", ladder_name.get(), temp->points, temp->rank);
			Remove_Highscore(ladder_name);
			DAT_00B76640.add(ladder_name, *temp);
			temp = temp->next;
		}
	} else {
		FUN_004068E0("There was a problem with getting a high score.\n");
	}
	return 0;
}

static inline rc_ptr<PlayerProfile> Profile_For_Nick(char const * nick)
{
	Wstring name(nick);
	return FUN_00778A20(&name);
}

// FUNCTION: GAMEMD 0x007A3D90
HRESULT CNetUtilEventSink::OnNewNick(HRESULT result, LPCSTR message, LPCSTR nick, LPCSTR pass)
{
	FUN_004068E0("OnNewNick\n");
	g_OnNewNickResult = result;
	if (result == 0) {
		rc_ptr<PlayerProfile> profile = Profile_For_Nick(nick);
		int nicknum = 0;
		WOLOptionsClass options;
		if (options.Is_Bit1_Clear()) {
			int locale = profile->Locale;
			nicknum = StoreLogin(nick, pass, true, locale);
			Session.Write_MultiPlayer_Settings();
		}
		char login_mode = 1;
		char combo_mode = 0;
		HWND dialog = WS_Find_Dialog(0x10F);
		if (dialog == NULL) {
			dialog = WS_Find_Dialog(0x11D);
			combo_mode = dialog != NULL;
			if (dialog == NULL) {
				login_mode = 0;
				dialog = WS_Find_Dialog(0x114);
			}
		}
		if (dialog != NULL) {
			HWND item = GetDlgItem(dialog, 0x628);
			if (combo_mode) {
				SendMessage(item, 0x4B4, 0, (LPARAM)nick);
			} else {
				int index = SendMessage(item, 0x4BC, 0, (LPARAM)nick);
				SendMessage(item, 0x151, index, nicknum);
				SendMessage(item, 0x14E, index, 0);
			}
			if (login_mode) {
				g_NickName = nick;
				g_LoginPassword = pass;
				HWND password = GetDlgItem(dialog, 0x72C);
				SendMessage(password, 0x4B4, 0, (LPARAM)g_LoginPassword.text());
				g_PlaintextPassword = 1;
				EnableWindow(GetDlgItem(dialog, 0x539), TRUE);
				if (!combo_mode) {
					EnableWindow(GetDlgItem(dialog, 0x72D), FALSE);
					SendMessage(GetDlgItem(dialog, 0x72D), 0xF1, 1, 0);
				}
			} else {
				SendMessage(GetDlgItem(dialog, 0x6FF), 0xF1, 0, 0);
				EnableWindow(GetDlgItem(dialog, 0x6FF), TRUE);
			}
			if (!combo_mode) {
				EnableWindow(GetDlgItem(dialog, 0x6EB), TRUE);
				Wstring name(nick);
				profile->FUN_0077A9F0(dialog, &name, true);
			}
		}
	}
	if (message) {
		wchar_t text[0x1400];
		FUN_00735060(text, message, -1);
		ODMessageBox(text, 0, FUN_0077D710, 0);
	} else {
		ODMessageBox(L"Unknown Status", 0, FUN_0077D710, 0);
	}
	HWND chat = WS_Find_Dialog(0xC4);
	if (chat) {
		g_ActiveWaitFlags &= ~0x100;
		if (g_ActiveWaitFlags == 0) {
			FUN_007757E0(chat, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A4070
HRESULT CNetUtilEventSink::OnAgeCheck(HRESULT result, int age, int consent)
{
	FUN_004068E0("OnAgeCheck\n");
	g_AgeCheckResult = result;
	if (result == 0) {
		g_UserAge = age;
		g_UserConsent = consent;
	}
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags &= ~0x100;
		if (g_ActiveWaitFlags == 0) {
			FUN_007757E0(dialog, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A40F0
HRESULT CNetUtilEventSink::OnPing(HRESULT result, int time, unsigned long address, int extra)
{
	if (DAT_00B77DC1 && g_PingsSent) {
		if (result == 0 && time >= 0 && time < 1000) {
			g_PingsReceived++;
		}
		return 0;
	}
	if (Session.WOLWelcomeChoice == 3) {
		goto counted;
	}
	if (Session.WOLWelcomeChoice == 1) {
		goto counted;
	}
	if (Session.WOLWelcomeChoice == 2) {
counted:
		FUN_004068E0("Received Ping\n");
		DAT_00B736F4++;
		if (DAT_00B736F4 >= DAT_00B76970.length()) {
			if (DAT_00B73A20 == 4 || DAT_00B73A20 == 5) {
				FUN_007A1330();
			}
			FUN_007A1060();
		}
		return 0;
	}
	if (g_CurrentChannel.type) {
		Draw_Player_List(0, false);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A41A0
CChatEventSink::CChatEventSink(void)
{
}

extern char g_GameServerHost[];
extern int g_GameServerPort;
extern char DAT_00B77CE8[];
extern int DAT_00B75F5C;
extern char DAT_00B73E44[64][128];
extern int DAT_00B77E1C;
extern int DAT_00B76338[64];
#include "ccrand.h"

// FUNCTION: GAMEMD 0x007A41C0
long __stdcall FUN_007A41C0(void * sink, long result, Server * servers)
{
	Server * tmp = servers;
	Server * server = NULL;
	int chat_server_index = 0;
	int skumatchcount = 0;
	int game_srv_found = 0;
	char namebuf[128];
	char conndata[128];
	char conndata2[128];
	char conndata3[128];
	char token[128];
	char manglerdata[128];

	srand(time(0));

	if (result < 0) {
		SetEvent(g_WaitEventHandles[0]);
		swprintf(g_LastErrorMessage, StringTable::LoadString("TXT_NO_SERV_LIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x149B));
		return 0;
	}

	bool mangler_found = false;
	while (tmp != NULL) {
		if (strcmp((char *)tmp->connlabel, "IRC") == 0) {
			if (chat_server_index == 0) {
				g_Servers.addTail(*tmp);
				g_CurrentServerIndex = 0;
				chat_server_index = 1;
			} else {
				g_Servers.addTail(*tmp);
				char * nameToken;
				strcpy(namebuf, (char *)tmp->name);
				int sku = -1;
				nameToken = strtok(namebuf, ",");
				while (nameToken != NULL) {
					sku = atol(nameToken);
					if (sku == (unsigned char)g_GameSKU) {
						break;
					}
					if (strchr(nameToken, ':') != NULL) {
						break;
					}
					nameToken = strtok(NULL, ",");
				}
				if (sku == (unsigned char)g_GameSKU) {
					double pick_roll = rand();
					pick_roll /= 32767.0;
					double pick_chance = 1.0 / (double)++skumatchcount;
					if (pick_roll <= pick_chance) g_CurrentServerIndex = chat_server_index;
				}
				g_TargetServerIndex = g_CurrentServerIndex;
				chat_server_index++;
			}
		} else if (strcmp((char *)tmp->connlabel, "LAD") == 0) {
			char * host;
			char * port;
			strcpy(token, (char *)tmp->conndata);
			strtok(token, ";");
			host = strtok(NULL, ";");
			if (host != NULL) {
				strcpy(g_LadderServerHost, host);
			}
			port = strtok(NULL, ";");
			if (port != NULL) {
				g_LadderServerPort = atol(port);
			}
		} else if (strcmp((char *)tmp->connlabel, "GAM") == 0 && game_srv_found == 0) {
			char * host;
			char * port;
			memcpy(&g_GameServer, tmp, sizeof(g_GameServer));
			strcpy(conndata, (char *)g_GameServer.conndata);
			strtok(conndata, ";");
			host = strtok(NULL, ";");
			if (host != NULL) {
				strcpy(g_GameServerHost, host);
			}
			port = strtok(NULL, ";");
			if (port != NULL) {
				g_GameServerPort = atol(port);
			}
			DAT_00B77CE8[0] = 0;
			DAT_00B75F5C = 0;
			game_srv_found = 1;
		} else if (strcmp((char *)tmp->connlabel, "GAM") == 0 && game_srv_found == 1) {
			char * host;
			char * port;
			memcpy(&g_GameServer, tmp, sizeof(g_GameServer));
			strcpy(conndata3, (char *)g_GameServer.conndata);
			strtok(conndata3, ";");
			host = strtok(NULL, ";");
			if (host != NULL) {
				strcpy(DAT_00B77CE8, host);
			}
			port = strtok(NULL, ";");
			if (port != NULL) {
				DAT_00B75F5C = atol(port);
			}
			game_srv_found = 2;
		} else if (strcmp((char *)tmp->connlabel, "WDT") == 0) {
			char * host;
			char * port;
			memcpy(&g_WDTServer, tmp, sizeof(g_WDTServer));
			strcpy(conndata2, (char *)g_WDTServer.conndata);
			strtok(conndata2, ";");
			host = strtok(NULL, ";");
			if (host != NULL) {
				strcpy(g_WDTHost, host);
			}
			port = strtok(NULL, ";");
			if (port != NULL) {
				g_WDTPort = atol(port);
			}
		} else if (stricmp((char *)tmp->connlabel, "MGL") == 0) {
			FUN_004068E0("MGL entry in server list\n");
			if (!mangler_found) {
				memset(DAT_00B73E44, 0, sizeof(DAT_00B73E44));
				memset(DAT_00B76338, 0, sizeof(DAT_00B76338));
				DAT_00B77E1C = 0;
				mangler_found = true;
			}
			Server * mangler = new Server;
			memcpy(mangler, tmp, sizeof(Server));
			strcpy(manglerdata, (char *)mangler->conndata);
			strtok(manglerdata, ";");
			char * host = strtok(NULL, ";");
			if (host != NULL) {
				strcpy(DAT_00B73E44[DAT_00B77E1C], host);
				char * port = strtok(NULL, ";");
				if (port != NULL) {
					DAT_00B76338[DAT_00B77E1C] = atol(port);
				}
				FUN_004068E0("Got mangler server from servserv - address is %s ; %d\n", DAT_00B73E44[DAT_00B77E1C], DAT_00B76338[DAT_00B77E1C]);
				DAT_00B77E1C++;
			}
			delete mangler;
		}
		tmp = tmp->next;
	}

	server = NULL;
	for (int i = 0; i < g_Servers.length(); i++) {
		g_Servers.getPointer(&server, i);
		strcpy((char *)server->login, g_NickName.get());
		strcpy((char *)server->password, g_LoginPassword.get());
	}

	FUN_004068E0("Got %d mangler servers\n", DAT_00B77E1C);
	DAT_00B77E20 = (DAT_00B77E1C > 1) ? Sim_Random_Pick(0, DAT_00B77E1C - 1) : 0;
	FUN_004068E0("CurrentManglerServer = %d\n", DAT_00B77E20);

	Set_Wait_Dialog_Text(StringTable::LoadString("TXT_DETECTING_FIREWALL_SETTINGS", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1552));
	FirewallHelper.Save_Detected();
	SetEvent(g_WaitEventHandles[1]);
	return 0;
}


// FUNCTION: GAMEMD 0x007A4C10
long __stdcall FUN_007A4C10(void * sink, long result, Update * updates)
{
	int count = 0;
	char working_dir[257];
	char fullpath[257];
	char localfile[257];
	wchar_t status[200];
	if (result < 0 || updates == NULL) {
		SetEvent(g_WaitEventHandles[0]);
		return 0;
	}
	g_SuspendChatPump = 1;
	Update * tmp = updates;
	while (tmp != NULL) {
		tmp = tmp->next;
		count++;
	}
	HWND chat = WS_Find_Dialog(0xC4);
	if (chat) {
		g_ActiveWaitFlags &= ~0x10;
		if (g_ActiveWaitFlags == 0) {
			FUN_007757E0(chat, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
	if (ODMessageBox(StringTable::LoadString("TXT_UPGRADEREQUIRED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1580), 4, FUN_0077D710, 0) == IDNO) {
		SetEvent(g_WaitEventHandles[0]);
		return 0;
	}
	GetCurrentDirectory(0x100, working_dir);
	int i = 0;
	while (updates != NULL) {
		sprintf(fullpath, "%s/%s", updates->patchpath, updates->patchfile);
		sprintf(localfile, "%s\\%s", updates->localpath, updates->patchfile);
		CreateDirectory((char *)updates->localpath, NULL);
		g_pDownload->DownloadFile((char *)updates->server, (char *)updates->login, (char *)updates->password, fullpath, localfile, "SOFTWARE\\Westwood\\Yuri's Revenge");
		i++;
		swprintf(status, StringTable::LoadString("TXT_DOWNLOADING_X_OF_Y", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1596), i, count);
		WideWstring text(status);
		SimpleWonlineDialogControl control;
		HWND dialog = UI::BeginDialog(0xD3, WOL_Download_Dialog_Proc, 0);
		HWND title = GetDlgItem(dialog, 0x5C7);
		SendMessage(title, 0x4B2, 0, (LPARAM)text.text());
		if (FUN_007759E0(dialog, (bool (*)(void))FUN_0077D710, 0, 1, 1) == IDCANCEL) {
			SetEvent(g_WaitEventHandles[0]);
			SetCurrentDirectory(working_dir);
			return 0;
		}
		updates = updates->next;
	}
	ODMessageBox(StringTable::LoadString("TXT_GAME_RESTART", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x15B6), 0, FUN_0077D710, 0);
	SetEvent(DAT_00B736A4);
	SetCurrentDirectory(working_dir);
	return 0;
}

// FUNCTION: GAMEMD 0x007A4EC0
long __stdcall FUN_007A4EC0(void * sink, long result, unsigned long id, Squad * squad)
{
	g_SquadInfoRequests--;
	if (g_RequestingOwnSquadInfo) {
		g_RequestingOwnSquadInfo = 0;
		g_OwnSquadID = id;
		if (result != 0) {
			return 0;
		}
		strcpy(g_OwnSquadName, (char *)squad->abbreviation);
	} else if (result != 0) {
		return 0;
	}
	for (int i = 0; i < g_UserList.length(); i++) {
		User * user = NULL;
		g_UserList.getPointer(&user, i);
		if (user->squadID == id) {
			strcpy((char *)user->squadabbrev, (char *)squad->abbreviation);
			strcpy((char *)user->squadname, (char *)squad->name);
			User temp;
			memset(&temp, 0, sizeof(temp));
			g_UserList.get(temp, i);
			User copy = temp;
			copy.next = NULL;
			g_UserList.remove(i);
			FUN_00783070(&copy);
		}
	}
	Wstring key((char *)squad->abbreviation);
	key.toLower();
	if (!g_BattleClansLadderDict.contains(key)) {
		g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, (char *)squad->abbreviation, 0x802100, -1, 0, 0);
	}
	if (g_SquadInfoRequests == 0) {
		Draw_Player_List(0, false);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A5220
long __stdcall FUN_007A5220(void * sink, long result, User * user)
{
	if (result >= 0) {
		Handle_User_Leave(user);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A5240
long __stdcall FUN_007A5240(void * sink, long result, void * unused)
{
	wchar_t text[512];
	if (!DAT_00B77DC1) {
		swprintf(text, L"%s", StringTable::LoadString("TXT_SERVER_ERROR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1617));
		FUN_005E2700(-1, text);
	}
	return 0;
}


extern char DAT_0083F328[];
extern wchar_t DAT_00830174[];

extern int icons_loaded;
extern int g_ListIconHeight;

// FUNCTION: GAMEMD 0x007A5290
long __stdcall FUN_007A5290(void * sink, long result, char const * motd)
{
	g_OnConnectionResult = result;
	if (result < 0) {
		g_IgnoreNetStatus = 1;
	}
	switch (result) {
	case (long)0x80040064:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_LOGIN_USED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1669));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040065:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_BADPASS", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x166E));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040072:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_BANNED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1673));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040074:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_DISABLED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1679));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040075:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_BANNED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x167E));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040076:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_SERIALDUP", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1683));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040077:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_SERIALUNKNOWN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1688));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case (long)0x80040078:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_SKUSERIALMISMATCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x168D));
		SetEvent(g_WaitEventHandles[0]);
		break;
	default:
		wcscpy(g_LastErrorMessage, StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1693));
		SetEvent(g_WaitEventHandles[0]);
		break;
	case 0:
		if (icons_loaded == 0) {
			char const * gametypes;
			g_pChat->GetGametypeList(&gametypes);
			char * buffer = new char[strlen(gametypes) + 32];
			strcpy(buffer, gametypes);
			strcat(buffer, ",-1,0,18");
			char * token = strtok(buffer, ",");
			while (token) {
				long type = atol(token);
				unsigned char * bitmap;
				int bytes;
				char const * name;
				char const * url;
				g_pChat->GetGametypeInfo(type, g_ListIconHeight, &bitmap, &bytes, &name, &url);
				char bitmap_name[64];
				sprintf(bitmap_name, "gt%d.bmp", type);
				SurfaceCache.CacheBMP(bitmap_name, bitmap, bytes, 2);
				token = strtok(NULL, ",");
			}
			delete[] buffer;
			icons_loaded = 1;
		}
		if (g_ShowMOTD) {
			g_MessageOfTheDay = motd;
		} else {
			g_MessageOfTheDay.clear();
		}
		g_ConnectionLost = 0;
		g_ChannelCount = 0;
		memset(&g_CurrentChannel, 0, 0x10C);
		memset(&s_TempChannel, 0, 0x10C);
		SetEvent(g_WaitEventHandles[2]);
		FUN_00795650(&Session.LocalAddress);
		break;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A55F0
long __stdcall FUN_007A55F0(void * sink, long result, char const * motd)
{
	if (result < 0) {
		return 0;
	}
	char * buffer = strdup(motd);
	char * line = strtok(buffer, DAT_0083F328);
	while (line) {
		wchar_t wide[1024];
		FUN_00735060(wide, line, -1);
		FUN_005E2700(DAT_007EEF28, wide);
		line = strtok(NULL, DAT_0083F328);
	}
	free(buffer);
	FUN_005E2700(DAT_007EEF28, DAT_00830174);
	return 0;
}

extern char DAT_0084ACA0[];

static inline void Close_Wait_Window(unsigned int event)
{
	HWND dialog = WS_Find_Dialog(0xC4);
	if (dialog) {
		g_ActiveWaitFlags &= ~event;
		if (g_ActiveWaitFlags == 0) {
			FUN_007757E0(dialog, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
}

// FUNCTION: GAMEMD 0x007A5690
long __stdcall FUN_007A5690(void * sink, long result, Channel * channel)
{
	if (result < 0) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_CHANCREATE_FAILED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x16C1));
		g_IsChannelCreator = 0;
		g_CurrentChannel.name[0] = 0;
		Close_Wait_Window(4);
		return 0;
	}
	if (channel->type == 0) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_CREATED_CHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x16CB), FUN_00735120((char *)channel->name));
		memcpy(&g_CurrentChannel, channel, 0x10C);
	} else {
		memset(g_UserInfo, 0, sizeof(g_UserInfo));
		strcpy(g_UserInfo[0].Name, g_NickName.get());
		g_UserInfo[0].House = Session.House;
		g_UserInfo[0].Unknown = 0;
		g_UserInfo[0].Color = Session.PrefColor;
		g_UserInfo[0].Accepted = 1;
		memcpy(&g_CurrentChannel, channel, 0x10C);
	}
	g_IsChannelCreator = 1;
	g_ChannelCount++;
	FUN_004068E0(DAT_0084ACA0);
	if (g_pChat && channel->type != 0) {
		char info[80];
		memset(info, 0, sizeof(info));
		Encode_Channel_ExInfo(info);
		info[40] = 0;
		g_pChat->SetChannelExInfo(info);
	}
	Close_Wait_Window(4);
	return 0;
}

extern char DAT_0084AD84[];

// FUNCTION: GAMEMD 0x007A70E0
long __stdcall FUN_007A70E0(void * sink, long result, void * unused, User * user)
{
	if (result < 0) {
		FUN_004068E0(DAT_0084AD84);
		return 0;
	}
	Handle_User_Leave(user);
	return 0;
}

extern int DAT_007EEF40;
extern int DAT_007EEF34;
extern int DAT_007EEF38;
extern wchar_t DAT_0084B020[];
extern wchar_t DAT_0084B224[];
HWND WS_Next_Lower_Dialog(HWND window);

static inline void Set_Item_Text(HWND item, Wstring const & text)
{
	SendMessage(item, 0x4B4, 0, (LPARAM)text.text());
}

static inline WideWstring Widen(char const * text)
{
	Wstring narrow(text);
	return FUN_007B7ED0(narrow);
}

extern int DAT_007EEF3C;
extern char DAT_0084AFDC[];



extern int DAT_00AC1178;

static inline void Set_Initial_Player_Options(NodeNameType *node)
{
	node->Set_Country(-2);
	node->Set_Color(-2);
	node->Set_Start(-2);
	node->Set_Team(-2);
}

// FUNCTION: GAMEMD 0x007A7110
long __stdcall FUN_007A7110(void * sink, long result, void * channel, User * users)
{
	if (DAT_00B77DC1) {
		return 0;
	}

	static char keys[512];
	memset(keys, 0, sizeof(keys));
	Wstring ladder_name;

	if (result < 0) {
		return 0;
	}

	SendDlgItemMessage(FUN_00775B10(), 0x53E, LB_RESETCONTENT, 0, 0);
	g_UserList.clear();

	unsigned int flags;
	int batch_count = 0;
	int user_count = 0;
	bool saw_owner = false;

	while (users != NULL) {
		user_count++;

		if (users->squadID != 0 && strlen((char *)users->squadabbrev) == 0) {
			g_SquadInfoRequests++;
			g_pChat->RequestSquadInfo(users->squadID);
		} else if (g_pNetUtil && users->squadID != 0 && strlen((char *)users->squadabbrev) != 0) {
			Wstring abbrev((char *)users->squadabbrev);
			abbrev.toLower();
			if (!g_BattleClansLadderDict.contains(abbrev)) {
				g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, (char *)users->squadabbrev, 0x802100, -1, 0, 0);
			}
		}

		char name[128];
		flags = users->flags;
		strcpy(name, (char *)users->name);
		User copy = *users;
		copy.next = NULL;
		if (g_CurrentChannel.type == 0) {
			FUN_00783070(&copy);
		} else {
			g_UserList.addTail(copy);
		}

		if (flags & 1) {
			saw_owner = true;
		}

		ladder_name.set((char *)users->name);
		ladder_name.toLower();

		if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
			if (!g_WDTTeams.contains(ladder_name)) {
				g_WDTTeamLookupQueue.addHead(ladder_name);
			}
		}

		if (!g_UserLocales.contains(ladder_name)) {
			g_UserLocaleLookupQueue.addHead(ladder_name);
		}

		if (!g_ActiveLadder->contains(ladder_name) && user_count < 100) {
			ladder_name.set((char *)users->name);
			ladder_name.toLower();
			strcat(keys, ladder_name.get());
			strcat(keys, ":");
			batch_count++;
		}

		if (batch_count > 20) {
			g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00, -1, 0, 0);
			g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00);
			batch_count = 0;
			memset(keys, 0, sizeof(keys));
		}

		if (g_CurrentChannel.type != 0 && !g_IsChannelCreator) {
			Wstring nick((char *)users->name);
			int index = Player_Name_To_Index(nick);
			memset(&g_UserInfo[index], 0, sizeof(g_UserInfo[index]));
			strcpy(g_UserInfo[index].Name, (char *)users->name);
			if (stricmp((char *)users->name, g_NickName.get()) != 0) {
				if (Session.Players.Count() == 1) {
					wcscpy(Session.GameName, FUN_00735120((char *)users->name));
				}
				NodeNameType * node = new NodeNameType;
				memset(node, 0, sizeof(NodeNameType));
				WideWstring wide = Widen((char *)users->name);
				wcscpy(node->Name, wide.get());
				node->Address = FUN_0077D720(users);
				Set_Initial_Player_Options(node);
				Session.Players.Add(node);
			}
		}

		users = users->next;
	}

	if (strlen(keys)) {
		g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00, -1, 0, 0);
		g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00);
	}

	if (g_CurrentChannel.type != 0) {
		if (!saw_owner) {
			User user;
			for (int i = 0; i < g_UserList.length(); i++) {
				if (g_UserList.get(user, i) && stricmp((char *)user.name, g_NickName.get()) == 0) {
					Handle_User_Leave(&user);
					break;
				}
			}
		}

		if (g_CurrentChannel.type != 0) {
			if (!g_IsChannelCreator && g_UserList.length() == 1 && stricmp((char *)users->name, g_NickName.get()) == 0) {
				Handle_User_Leave(users);
			}

			if (g_CurrentChannel.type != 0) {
				DAT_00AC1178 = 1;
				if (!g_IsChannelCreator) {
					HWND dialog = WS_Find_Dialog(0xC9);
					FUN_005EDDE0(dialog, true);
				}
				FUN_005E9850();

				WOLOptionsClass options;
				rc_ptr<PlayerProfile> profile;
				if (options.Is_Bit1_Clear()) {
					profile = FUN_00778A20(&g_NickName);
				} else {
					profile = DAT_00B75E48;
				}
				Session.FUN_0069B760(profile.Get()->Country, false);
				Session.FUN_0069B7E0(profile.Get()->Field20, false);

				if (Game::ObserverMode && !Is_Internal_Session()) {
					Session.FUN_0069B760(-3, false);
					Session.FUN_0069B7E0(-2, false);
				}
				if (!Game::ObserverMode && !Is_Internal_Session() && Session.FUN_0069B700() == -3) {
					if (Session.Players.Count()) {
						Session.FUN_0069B760(Session.Players[0]->Get_Country(), false);
						Session.FUN_0069B7E0(Session.Players[0]->Get_Color(), false);
					} else {
						Session.FUN_0069B760(-2, false);
						Session.FUN_0069B7E0(-2, false);
					}
				}

				char buffer[128];
				{
					NetNumType net;
					NetNodeType node;
					unsigned short port;
					Session.LocalAddress.Get_Address(net, node, &port);
					sprintf(buffer, "R%d,%d,%d,%d,%x,%x,%x", Session.FUN_0069B700(), Session.FUN_0069B720(), -2, -2, *(int *)node, (unsigned short)FirewallHelper.Detected, Session.PortBase);
				}
				FUN_004068E0("Sending private game options (%s)\n", buffer);
				User host;
				if (g_UserList.getHead(host)) {
					g_pChat->RequestPrivateGameOptions(&host, buffer);
				}
			}
		}
	}

	if (Is_Internal_Session() && g_CurrentChannel.type == 0 && !FUN_007A0690()) {
		SetEvent(g_WaitEventHandles[5]);
	}
	Draw_Player_List(0, false);
	return 0;
}

namespace Game
{
namespace Network
{
extern int GameStockKeepingUnit;
extern long PlanetWestwoodStartTime;
}
}
extern char WestwoodOnline_UserName[];

// FUNCTION: GAMEMD 0x007A7B60
long __stdcall FUN_007A7B60(void *sink, long result, Channel *channel, User *users, int game_id)
{
	bool all_found = true;
	int count = g_UserList.length();
	int listed = 0;
	DAT_00B76478 = CDTimerClass<SystemTimerClass>(0);
	if (DAT_00B77E27)
		return S_OK;
	g_SuspendChatPump = 1;
	User *user = users;
	while (user) {
		++listed;
		Wstring name((char *)user->name);
		name.toLower();
		int i;
		for (i = 0; i < count; ++i) {
			User entry;
			g_UserList.get(entry, i);
			Wstring other((char *)entry.name);
			other.toLower();
			if (name == other)
				break;
		}
		if (i >= count) {
			all_found = false;
			break;
		}
		user = user->next;
	}
	bool ok = listed == count && all_found;
	if (!ok) {
		g_GameStartRequested = false;
		g_GameStarted = 0;
	} else {
		if (FUN_0077CF40()) {
			HWND dialog = WS_Find_Dialog(0x117);
			FUN_007757E0(dialog, 0);
		}
		HWND dialog = WS_Find_Dialog(0xCF);
		FUN_007757E0(dialog, 0);
		Game::Network::WOLGameID = game_id;
		WestwoodOnline_GameSKU_TS = g_GameSKU;
		Game::Network::GameStockKeepingUnit = 0x2300;
		Game::Network::Tournament = g_CurrentChannel.tournament;
		if (FUN_0077D970())
			Game::Network::Tournament = 2;
		strcpy(WestwoodOnline_LoginName, g_NickName.get());
		User *first = NULL;
		g_UserList.getPointer(&first, 0);
		if (first)
			strcpy(WestwoodOnline_UserName, (char *)first->name);
		Game::Network::PlanetWestwoodStartTime = time(0);
		FUN_004068E0("OnGameStart\n");
		if (channel)
			g_CurrentLobby = channel->reserved & 0xFF;
		DAT_00B77E26 = false;
		DAT_00B77E27 = true;
		User_List_To_Session_Players(users);
		g_GameStarted = 2;
		FUN_004068E0("Starting a %d player game.\n", Session.NumPlayers);
		User *host = Get_Channel_Host();
		if (host)
			FUN_00735060(Session.MasterPlayerName, (char *)host->name, -1);
		Session.MasterPlayerID = -1;
		Session.NumConnectionPairs = FUN_007937B0(Session.ConnectionPairs);
	}
	return S_OK;
}

bool __fastcall FUN_005E3D10(char * options, char * name);
extern char DAT_0084AE74[];
extern char DAT_0084AE4C[];
extern char DAT_0084AE08[];

// FUNCTION: GAMEMD 0x007A7EA0
long __stdcall FUN_007A7EA0(void * sink, long result, Channel * channel, User * user, char const * options)
{
	static BasicTimerClass<SystemTimerClass> timer;
	if (result < 0) return S_OK;
	char * buffer = strdup(options);
	bool scenario_changed = FUN_005E3D10(buffer, (char *)user->name);
	free(buffer);
	if (DAT_00B77DC1 || g_GameStarted || options[0] == 'Z' || options[0] == 'W') return S_OK;
	bool isnew = false;
	if (Is_Local_Host()) return S_OK;
	if (Is_Internal_Session()) {
		WideWstring nickname = FUN_007B7ED0(g_NickName);
		NodeNameType * local = FUN_005E8BF0(nickname.get());
		if (!local) return S_OK;
		if (local->Get_Country() < 0 || local->Get_Color() < 0) return S_OK;
		if (options[0] < '0' || options[0] > '9') return S_OK;
		if (g_UserList.length() == 2) g_pChat->RequestPublicGameOptions("A1");
		return S_OK;
	}
	HWND dialog = FUN_00775B10();
	if (!MultiplayerMapPreview) {
		if (scenario_changed) {
			isnew = true;
			timer = 0;
			FUN_004068E0(DAT_0084AE74);
		}
		if (timer > 900 && (!MultiplayerMapPreview || !MultiplayerMapPreview->Get_Preview_Surface()) && !g_RecievingPreview) {
			if (Session.ScenarioFileName[0] && !FUN_006951F0(Session.ScenarioFileName, Session.ScenarioFileLength, Session.ScenarioDigest, Session.ScenarioIsOfficial, false)) {
				isnew = true;
				FUN_004068E0(DAT_0084AE4C);
			} else {
				timer = 0;
			}
		}
		if (!Session.Is_Random_Map()) {
			bool exists = false;
			if (FUN_006951F0(Session.ScenarioFileName, Session.ScenarioFileLength, Session.ScenarioDigest, Session.ScenarioIsOfficial, false)) {
				{
					RawFileClass file(Session.ScenarioFileName);
					if (file.Is_Available()) exists = true;
				}
				if (!exists) {
					isnew = false;
					goto update_dialog;
				}
			}
			if (FUN_0077D940() && Session.Players[0]->Slot.Unknown77) {
				Session.Players[0]->Slot.Unknown77 = 0;
				FUN_005E3720("A0");
				EnableWindow(GetDlgItem(dialog, 0x59F), false);
			}
update_dialog:
			if (FUN_00775B20() == 0xC9) InvalidateRect(FUN_00775B10(), NULL, false);
		}
	} else {
		HWND item = GetDlgItem(dialog, 0x71F);
		if (item) ShowWindow(item, SW_HIDE);
		ShowWindow(GetDlgItem(dialog, 0x468), SW_SHOWNORMAL);
	}
	if (isnew) {
		char options_str[64];
		sprintf(options_str, "P%s", Session.ScenarioFileName);
		User host;
		g_UserList.getHead(host);
		g_pChat->RequestPrivateGameOptions(&host, options_str);
		FUN_004068E0(DAT_0084AE08);
		Poke_The_Host();
		return S_OK;
	}
	return S_OK;
}

// FUNCTION: GAMEMD 0x007A8DA0
long __stdcall FUN_007A8DA0(void * sink, long result, void * channel, User * user, char const * text)
{
	if (DAT_00B77DC1)
		return 0;
	if (result < 0)
		return 0;
	if (Is_Internal_Session()) {
		if (user->flags & 1) {
			FUN_007A16B0(text, false);
			return 0;
		}
		return 0;
	}
	bool allowed;
	{
		Wstring narrow(text);
		allowed = FUN_0077D2A0(user);
	}
	if (!allowed)
		return 0;
	if (!FUN_0077D4C0(user))
		return 0;
	WideWstring wide = Widen(text);
	int color = DAT_007EEF2C;
	if (user->flags & 1) {
		color = DAT_007EEF3C;
	}
	if (stricmp(Session.Handle, (char *)user->name) != 0) {
		FUN_005E2700(color, L"[%hs] : %s", user->name, wide.get());
	}
	return 0;
}

extern char DAT_0084AF80[], DAT_0084AF4C[], DAT_0084AED8[], DAT_0084AEA0[];
extern bool DAT_008316D4;
extern int DAT_008316A4;
void __fastcall FUN_005E32D0(bool, bool);
bool FUN_005E9510(void);
int __fastcall FUN_005E9240(NodeNameType * node);
void __fastcall FUN_005EC9F0(HWND dialog, int id, NodeNameType * node, int mode);
bool __fastcall FUN_005E9720(int id);
void __fastcall Send_Preview_To_Guests(MapPreviewClass * preview);

// FUNCTION: GAMEMD 0x007A8260
long __stdcall FUN_007A8260(void * sink, long result, User * user, char const * options)
{
	if (DAT_00B77DC1) return S_OK;
	if (g_GameStarted && options[0] != 'Z') return S_OK;
	if (Is_Internal_Session() && options[0] != 'Z') return S_OK;
	int existing_user;
	{
		Wstring name((char *)user->name);
		existing_user = Player_Name_To_Index(name);
	}
	if (existing_user == -1) return S_OK;
	switch (options[0]) {
	case 'Z':
		if (options[1] == '4') {
			FUN_004068E0(DAT_0084AF80, (char *)user->name);
			unsigned int port = atol(options + 2);
			FUN_004068E0(DAT_0084AF4C, (char *)user->name, port & 0xFFFF);
			for (int i = 0; i < 8; ++i) {
				if (!stricmp(FUN_007350C0(Session.Slots[i]->Name), (char *)user->name)) {
					Session.unknown_2E18[i] = (unsigned short)port;
					break;
				}
			}
		}
		break;
	case 'R': {
		char * buffer = strdup(options + 1);
		int index = -1;
		int row = 0;
		for (; row < 8; ++row) {
			Wstring name = FUN_007B7F90(Session.Slots[row]->Name);
			if (!stricmp(name.get(), (char *)user->name)) { index = row; break; }
		}
		if (index != -1) {
			char * token = strtok(buffer, ",");
			int country = 0;
			if (token) country = atol(token);
			token = strtok(NULL, ",");
			int color = 0;
			if (token) color = atol(token);
			token = strtok(NULL, ",");
			int start = 0;
			if (token) start = atol(token);
			token = strtok(NULL, ",");
			int team = 0;
			if (token) team = atol(token);
			token = strtok(NULL, ",");
			unsigned int ip = 0;
			sscanf(token, "%X", &ip);
			IPXAddressClass address;
			if (ip) {
				NetNumType net;
				NetNodeType node;
				memset(net, 0, sizeof(net));
				memset(node, 0, sizeof(node));
				memcpy(node, &ip, sizeof(ip));
				address.Set_Address(net, node, 0);
				Session.PlayerLocalAddress[row] = address;
			}
			token = strtok(NULL, ",");
			unsigned int port = 0;
			if (token) sscanf(token, "%X", &port);
			Session.PlayerFirewall[row] = port;
			token = strtok(NULL, ",");
			unsigned int value = 0;
			if (token) sscanf(token, "%X", &value);
			Session.PlayerPortBase[row] = value;
			FUN_004068E0(DAT_0084AED8, (char *)user->name, country, color, start, team, address.As_String(NULL), Session.PlayerFirewall[row], value);
			HWND window = WS_Find_Dialog(0xC2);
			if (!window) window = WS_Find_Dialog(0xC9);
			int old_country = Session.Slots[row]->Get_Country();
			NodeNameType * player = Session.Slots[row];
			bool become_observer = false, become_player = false, changed = false;
			if (old_country == -3) {
				if (country != old_country) become_player = true;
			} else if (country == -3) become_observer = true;
			User * host = Get_Channel_Host();
			if (host && !stricmp((char *)host->name, g_NickName.text()) && row == 0 && become_observer && Session.MPGameMode && !Session.MPGameMode->vt_entry_BC()) {
				become_observer = false;
				changed = true;
			}
			if (become_player) {
				host = Get_Channel_Host();
				if (host && !stricmp((char *)host->name, g_NickName.text())) {
					if (FUN_005E9510()) {
						FUN_005EC9F0(window, FUN_005E98F0(row), DAT_00AC11B4, 0);
						changed = true;
						FUN_0077DB90();
						FUN_005E32D0(true, false);
						Draw_Player_List(0, false);
					} else {
						player->Set_Country(-2);
						player->Slot.SpectatorFlag = 0;
						FUN_005EC9F0(window, FUN_005E98F0(DAT_008316A4), DAT_00AC11B4, 0);
						index = FUN_005E9240(player);
						FUN_005EC9F0(window, FUN_005E98F0(index), player, 0);
						FUN_0077DB90();
						Session.PlayerFirewall[index] = Session.PlayerFirewall[row];
						Session.PlayerLocalAddress[index] = Session.PlayerLocalAddress[row];
						Session.PlayerPortBase[index] = Session.PlayerPortBase[row];
					}
				}
			}
			if (become_observer) {
				host = Get_Channel_Host();
				if (host && !stricmp((char *)host->name, g_NickName.text())) {
					if (FUN_005E9560()) {
						FUN_005EC9F0(window, FUN_005E98F0(index), DAT_00AC119C, 0);
						FUN_0077DB90();
					} else {
						player->Set_Country(-3);
						player->Slot.SpectatorFlag = -1;
						FUN_005EC9F0(window, FUN_005E98F0(index), DAT_00AC119C, 0);
						FUN_005EC9F0(window, FUN_005E98F0(DAT_008316A4), player, 0);
						FUN_0077DB90();
						FUN_004E3B00(window, FUN_004E37D0(DAT_008316A4));
						int observer_control = FUN_005E98F0(DAT_008316A4);
						SendMessage(GetDlgItem(window, observer_control), 0x4F1, 0, 1);
						int control = FUN_004E41D0(DAT_008316A4);
						FUN_004E4770(window, control);
						FUN_004E49A0(window, control, 8);
						index = DAT_008316A4;
						FUN_005E32D0(true, false);
						Draw_Player_List(0, false);
						Session.PlayerFirewall[index] = Session.PlayerFirewall[row];
						Session.PlayerLocalAddress[index] = Session.PlayerLocalAddress[row];
						Session.PlayerPortBase[index] = Session.PlayerPortBase[row];
						goto request_done;
					}
					goto refresh_options;
				}
			}
			if (changed) goto request_done;
			bool country_changed = old_country != country;
			if (Session.MPGameMode && !Session.MPGameMode->vt_entry_1C(country)) {
				country = Session.MPGameMode->vt_entry_1C(old_country) ? old_country : Session.MPGameMode->vt_entry_28();
				country_changed = true;
			}
			if (country_changed) Session.Slots[index]->Set_Country(country);
			bool color_changed = Session.Slots[index]->Get_Color() != color;
			if (color_changed) {
				if (Session.FUN_0069B600(color)) color = -2;
				Session.Slots[index]->Set_Color(color);
			}
			bool start_changed = Session.Slots[index]->Get_Start() != start;
			if (start_changed) {
				if (FUN_005E9720(start)) start = -2;
				Session.Slots[index]->Set_Start(start);
			}
			bool team_changed = Session.Slots[index]->Get_Team() != team;
			if (team_changed) {
				if (Session.MPGameMode && !Session.MPGameMode->CanAllyWith(team)) {
					if (Session.MPGameMode) {
						Session.Slots[index]->Set_Team(Session.MPGameMode->vt_entry_2C());
					} else {
						Session.Slots[index]->Set_Team(-2);
					}
				} else {
					Session.Slots[index]->Set_Team(team);
				}
			}
			if (!country_changed && !color_changed && !start_changed && !team_changed) goto request_done;
		} else goto request_done;
refresh_options:
		FUN_005E32D0(true, false);
		Draw_Player_List(0, false);
request_done:
		free(buffer);
		break;
	}
	case 'P': {
		if (g_IsChannelCreator != 1) break;
		FUN_004068E0(DAT_0084AEA0);
		if (g_UserList.length() >= 2) {
			User temp;
			User * host = Get_Channel_Host();
			if (host && !stricmp((char *)host->name, g_NickName.text()) && !g_GameStarted && g_CurrentChannel.type && MultiplayerMapPreview && MultiplayerMapPreview->Get_Preview_Surface() && DAT_008316D4) {
				int found = Player_Name_To_Index((char *)user->name);
				if (found != -1) {
					g_UserList.get(temp, found);
					temp.next = NULL;
					User_List_To_Session_Players(&temp);
					int old = g_SuspendChatPump;
					g_SuspendChatPump = 1;
					Send_Preview_To_Guests(MultiplayerMapPreview);
					g_SuspendChatPump = old;
				}
			}
		}
		break;
	}
	}
	return S_OK;
}

// FUNCTION: GAMEMD 0x007A8ED0
long __stdcall FUN_007A8ED0(void * sink, long result, void * channel, User * user, wchar_t const * text)
{
	if (DAT_00B77DC1) {
		return 0;
	}
	if (result < 0) {
		return 0;
	}
	WideWstring wide(text);
	if (!FUN_0077D2E0(user, &wide)) {
		return 0;
	}
	if (!FUN_0077D4C0(user)) {
		return 0;
	}
	int color = DAT_007EEF2C;
	if (user->flags & 1) {
		color = DAT_007EEF3C;
	}
	if (stricmp(Session.Handle, (char *)user->name) != 0) {
		FUN_005E2700(color, L"[%hs] : %s", user->name, wide.get());
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A8FC0
long __stdcall FUN_007A8FC0(void * sink, long result, User * user, char const * text)
{
	if (DAT_00B77DC1)
		return 0;
	if (result < 0)
		return 0;
	if (Is_Internal_Session()) {
		if (user->flags & 1) {
			FUN_007A16B0(text, true);
			return 0;
		}
		FUN_004068E0(DAT_0084AFDC, user->name, text);
		return 0;
	}
	bool allowed;
	{
		Wstring narrow(text);
		allowed = FUN_0077D2A0(user);
	}
	if (!allowed)
		return 0;
	if (!FUN_0077D4C0(user))
		return 0;
	WideWstring wide = Widen(text);
	int color = DAT_007EEF30;
	if (user->flags & 1) {
		color = DAT_007EEF3C;
	}
	if (stricmp(Session.Handle, (char *)user->name) != 0) {
		FUN_005E2700(color, L"%hs : %s", user->name, wide.get());
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A9110
long __stdcall FUN_007A9110(void * sink, long result, User * user, wchar_t const * text)
{
	if (DAT_00B77DC1) {
		return 0;
	}
	if (result < 0) {
		return 0;
	}
	WideWstring wide(text);
	if (!FUN_0077D2E0(user, &wide)) {
		return 0;
	}
	if (!FUN_0077D4C0(user)) {
		return 0;
	}
	int color = DAT_007EEF30;
	if (user->flags & 1) {
		color = DAT_007EEF3C;
	}
	if (stricmp(Session.Handle, (char *)user->name) != 0) {
		FUN_005E2700(color, L"%hs : %s", user->name, wide.get());
	}
	return 0;
}


// FUNCTION: GAMEMD 0x007A9200
long __stdcall FUN_007A9200(void * sink, long result, User * user, char const * text)
{
	if (result < 0) {
		return 0;
	}
	WideWstring wide = Widen(text);
	if (!FUN_0077D2E0(user, &wide)) {
		return 0;
	}
	if (!FUN_0077D4C0(user)) {
		return 0;
	}
	strcpy(DAT_00B77E30, (char *)user->name);
	if (DAT_00B77DC1) {
		wchar_t message[256];
		swprintf(message, DAT_0084B020, user->name, wide.get());
		int scheme = GadgetClass::Get_Color_Scheme();
		Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_HOW_TO_REPLY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1F82), scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
		FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->name, wide.get());
		DAT_00B76478 = 300;
	} else {
		HWND top = FUN_00775B10();
		while (top) {
			if (GetDlgItem(top, 0x53F)) {
				FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->name, wide.get());
				break;
			}
			if (GetDlgItem(top, 0x5B5)) {
				FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->name, wide.get());
			}
			top = WS_Next_Lower_Dialog(top);
		}
		if (!top) {
			ReleaseCapture();
			FUN_0052FEC0(NULL, true);
			top = UI::BeginDialog(0x11E, (DLGPROC)FUN_0079DB50, 0);
			if (top) {
				SetFocus(top);
				FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->name, wide.get());
				ShowWindow(top, SW_SHOWNORMAL);
			}
		}
		Set_Item_Text(GetDlgItem(top, 0x718), FUN_0079DA90(GetDlgItem(top, 0x53F)));
	}
	if (DAT_00B77E40) {
		g_pChat->RequestPrivateUnicodeMessage(user, DAT_00B76230.get());
	}
	VocClass::PlayGlobal(Rule->IncomingMessage, 1.0f, 0x2000);
	return 0;
}

// FUNCTION: GAMEMD 0x007A9550
long __stdcall FUN_007A9550(void * sink, long result, User * user, wchar_t const * text)
{
	if (result < 0) {
		return 0;
	}
	bool allowed;
	{
		WideWstring wide(text);
		allowed = FUN_0077D2E0(user, &wide);
	}
	if (!allowed) {
		return 0;
	}
	if (!FUN_0077D4C0(user)) {
		return 0;
	}
	if (DAT_00B77DC1) {
		wchar_t message[256];
		swprintf(message, DAT_0084B020, user->name, text);
		int scheme = GadgetClass::Get_Color_Scheme();
		Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
		FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->name, text);
	} else {
		HWND top = FUN_00775B10();
		while (top) {
			if (GetDlgItem(top, 0x53F)) {
				FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->name, text);
				break;
			}
			if (GetDlgItem(top, 0x5B5)) {
				FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->name, text);
			}
			top = WS_Next_Lower_Dialog(top);
		}
		if (!top) {
			ReleaseCapture();
			FUN_0052FEC0(NULL, true);
			top = UI::BeginDialog(0x11E, (DLGPROC)FUN_0079DB50, 0);
			SetFocus(top);
			FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->name, text);
			ShowWindow(top, SW_SHOWNORMAL);
		}
		Set_Item_Text(GetDlgItem(top, 0x718), FUN_0079DA90(GetDlgItem(top, 0x53F)));
	}
	if (DAT_00B77E40) {
		g_pChat->RequestPrivateUnicodeMessage(user, DAT_00B76230.get());
	}
	VocClass::PlayGlobal(Rule->IncomingMessage, 1.0f, 0x2000);
	return 0;
}

// FUNCTION: GAMEMD 0x007A9800
long __stdcall FUN_007A9800(void * sink, long result)
{
	wchar_t message[128];
	if (result == 0) {
		FUN_005E2790(DAT_007EEF28, StringTable::LoadString("TXT_PAGESENT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1FFD));
		return 0;
	}
	if (result == 0x40134) {
		swprintf(message, StringTable::LoadString("TXT_PAGEDISABLED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2001));
	} else if (result == 0x40133) {
		swprintf(message, StringTable::LoadString("TXT_PAGEUSERGONE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2003));
	} else {
		swprintf(message, StringTable::LoadString("TXT_PAGEFAILED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2005));
	}
	HWND top = FUN_00775B10();
	if (top) {
		if (GetDlgItem(top, 0x53F)) {
			FUN_005E2700(DAT_007EEF28, message);
			return 0;
		}
		if (GetDlgItem(top, 0x5B5)) {
			FUN_005E2790(DAT_007EEF28, message);
			return 0;
		}
	}
	if (FUN_0069BBE0(&Session)) {
		int scheme = GadgetClass::Get_Color_Scheme();
		Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
	}
	return 0;
}



extern wchar_t DAT_0084B128[];
extern char DAT_0084B170[];
extern char DAT_0084B138[];

// FUNCTION: GAMEMD 0x007A9970
HRESULT __stdcall CChatEventSink::OnFind(HRESULT result, Channel *channel)
{
	HWND find_dialog = WS_Find_Dialog(0xCF);
	WolEntryState status = WOL_ENTRY_STATE_NONE;

	if (DAT_00B73ACC.length() != 0) {
		WolEntryState *previous = 0;
		DAT_00B76990.getPointer(DAT_00B73ACC, &previous);
		if (previous) status = *previous;

		char name[64];
		name[0] = 0;
		User user;

		if (channel) {
			if (channel->type == 0) {
				if (!_strnicmp((char *)channel->name, "Lob_40_", strlen("Lob_40_")))
					status = (WolEntryState)7;
				else if (!_strnicmp((char *)channel->name, "Lob_35_", strlen("Lob_35_")))
					status = (WolEntryState)8;
				else
					status = (WolEntryState)4;
			} else {
				status = (WolEntryState)5;
				Wstring lookup_name((char *)channel->name);
				Channel game;
				if (FUN_0079F0A0(&lookup_name, &g_GameChannelList, &game)) {
					if (FUN_0077D9D0(&game)) status = (WolEntryState)8;
					else if (FUN_0077D9B0(&game)) status = (WolEntryState)7;
				}
			}

			if (DAT_00B73ACC.length() > 0)
				DAT_00B76608.add(DAT_00B73ACC, *channel);

			strcpy(name, (char *)channel->name);
			if (!strncmp(name, "Lob_41_", 7)) {
				int value = atol(name + strlen("Lob_41_"));
				int division = value / 8;
				int remainder = value % 8;
				if (division == 0)
					sprintf(name, "%s", g_Lobbies[remainder]);
				else
					sprintf(name, "%s %d", g_Lobbies[remainder], division + 1);
			}
		} else {
			if (DAT_00B73ACC.length() > 0)
				DAT_00B76608.remove(DAT_00B73ACC);

			if (result == 0x40131) {
				status = (WolEntryState)3;
			} else if (result == 0x40130) {
				status = (WolEntryState)2;
			} else if (result == 0x40132) {
				status = (WolEntryState)1;
			}
		}

		if (DAT_00B73ACC.length() > 0) {
			DAT_00B76990.add(DAT_00B73ACC, status);
			DAT_00B73ACC.clear();
			HWND dialog = WS_Find_Dialog(0x116);
			if (dialog) FUN_0079AF10(dialog);
		}
		if (DAT_00B73ACC.length() > 0) {
			if ((unsigned long)(time(0) - DAT_00B779D0) < 5) return 0;
			DAT_00B73ACC.clear();
		}
		if (DAT_00B76980.length() != 0) {
			if (DAT_00B76980.length() > 0) {
				DAT_00B76980.get(DAT_00B73ACC, 0);
				DAT_00B76980.remove(0);
			}
			DAT_00B779D0 = time(0);
			memset(&user, 0, sizeof(user));
			strcpy((char *)user.name, DAT_00B73ACC.get());
			g_pChat->RequestFind(&user);
		}
		return 0;
	} else {
		if (find_dialog) {
			if (FUN_00775B10() != find_dialog) return 0;

			WideWstring location;
			HWND label = GetDlgItem(find_dialog, 0x5C8);

			if (channel && label) {
				if (channel->type == 0 &&
					!strncmp((char *)channel->name, "Lob_41_", strlen("Lob_41_"))) {
					int value = atol((char *)channel->name + strlen("Lob_41_"));
					int division = value / 8;
					location.set(g_Lobbies[value % 8]);
					if (division != 0) {
						location += L" ";
						location += FUN_007B7DD0(division + 1);
					}
				} else {
					location.set(FUN_00735120((char *)channel->name));
				}
			} else if (result == 0x40130) {
				location.set(StringTable::LoadString(
					"TXT_FINDPAGE_NOT_ONLINE", 0, "D:\\ra2mdpost\\wonline.cpp", 0x2045));
			} else if (result == 0x40131) {
				location.set(StringTable::LoadString(
					"TXT_FINDPAGE_NOT_IN_CHANNEL", 0, "D:\\ra2mdpost\\wonline.cpp", 0x2047));
			} else if (result == 0x40132) {
				location.set(StringTable::LoadString(
					"TXT_FINDPAGE_FIND_DISABLED", 0, "D:\\ra2mdpost\\wonline.cpp", 0x2049));
			} else {
				location.set(StringTable::LoadString(
					"TXT_FINDPAGE_NOT_ONLINE", 0, "D:\\ra2mdpost\\wonline.cpp", 0x204B));
			}
			SendMessageA(label, 0x4B2, 0, (LPARAM)location.text());
			return 0;
		}

		wchar_t text[128];

		if (channel) {
			if (channel->type == 0 &&
				!strncmp((char *)channel->name, "Lob_41_", strlen("Lob_41_"))) {
				int value = atol((char *)channel->name + strlen("Lob_41_"));
				int division = value / 8;
				int remainder = value % 8;
				WideWstring lobby(g_Lobbies[remainder]);
				if (division != 0) {
					lobby += L" ";
					lobby += FUN_007B7DD0(division + 1);
				}
				wchar_t const *user_name = FUN_00735120(DAT_00B76E28);
				wchar_t const *lobby_name = lobby.get();
				swprintf(text, StringTable::LoadString(
					"TXT_USER_FOUND", 0, "D:\\ra2mdpost\\wonline.cpp", 0x205E),
					user_name, lobby_name);
			} else {
				wchar_t const *user_name = FUN_00735120(DAT_00B76E28);
				wchar_t const *channel_name = FUN_00735120((char *)channel->name);
				swprintf(text, StringTable::LoadString(
					"TXT_USER_FOUND", 0, "D:\\ra2mdpost\\wonline.cpp", 0x2060),
					user_name, channel_name);
			}
		} else if (result == 0x40130) {
			swprintf(text, StringTable::LoadString(
				"TXT_USER_NOT_ONLINE", 0, "D:\\ra2mdpost\\wonline.cpp", 0x2067),
				FUN_00735120(DAT_00B76E28));
		} else if (result == 0x40131) {
			swprintf(text, StringTable::LoadString(
				"TXT_USER_NOT_IN_CHANNEL", 0, "D:\\ra2mdpost\\wonline.cpp", 0x2069),
				FUN_00735120(DAT_00B76E28));
		} else if (result == 0x40132) {
			swprintf(text, StringTable::LoadString(
				"TXT_USER_FIND_DISABLED", 0, "D:\\ra2mdpost\\wonline.cpp", 0x206B),
				FUN_00735120(DAT_00B76E28));
		} else {
			swprintf(text, StringTable::LoadString(
				"TXT_USER_NOT_ONLINE", 0, "D:\\ra2mdpost\\wonline.cpp", 0x206D),
				FUN_00735120(DAT_00B76E28));
		}

		HWND dialog = FUN_00775B10();
		if (dialog) {
			if (GetDlgItem(dialog, 0x53F)) {
				PMessagePrintf(DAT_007EEF28, text);
				return 0;
			}
			if (GetDlgItem(dialog, 0x5B5)) {
				FUN_005E2790(DAT_007EEF28, text);
				return 0;
			}
		}
		if (FUN_0069BBE0(&Session)) {
			int color = GadgetClass::Get_Color_Scheme();
			Session.Messages.Add_Message(
				0, 0, text, color, TextPrintType(0x16),
				(int)(Rule->MessageDelay * 900.0), false);
		}
		DAT_00B76E28[0] = 0;
		return 0;
	}
}

// FUNCTION: GAMEMD 0x007AA6A0
long __stdcall FUN_007AA6A0(void * sink, long result, int value)
{
	if (!DAT_00B77DC1 && result >= 0) {
		FUN_005E2700(DAT_007EEF28, DAT_0084B128, value);
	}
	return 0;
}

static inline void Clear_Channel(Channel & channel)
{
	memset(&channel, 0, 0x10C);
}

// FUNCTION: GAMEMD 0x007AA6D0
long __stdcall FUN_007AA6D0(void * sink, long result)
{
	wchar_t message[256];
	if (g_IgnoreNetStatus) {
		SetEvent(g_WaitEventHandles[9]);
		g_LastIgnoredStatus = result;
		return 0;
	}
	if (result == 0x4012F) {
		if (DAT_00B77DC1 && g_ConnectionLost) {
		} else if (DAT_00B77DC1) {
			message[0] = 0;
			swprintf(message, StringTable::LoadString("TXT_WOL_CONNLOST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x210C));
			int scheme = GadgetClass::Get_Color_Scheme();
			Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
			Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
			g_ConnectionLost = 1;
			g_ChannelCount = 0;
			Clear_Channel(g_CurrentChannel);
			Clear_Channel(s_TempChannel);
			FUN_004068E0(DAT_0084B170);
		} else {
			if (WS_Find_Dialog(0x113)) {
				ODMessageBox(StringTable::LoadString("TXT_YOURE_DISCON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x211E), 0, FUN_0077D710, 0);
			}
			g_IgnoreNetStatus = 1;
			g_ConnectionLost = 1;
			SetEvent(g_WaitEventHandles[0]);
			g_ChannelCount = 0;
			Clear_Channel(g_CurrentChannel);
			Clear_Channel(s_TempChannel);
			FUN_004068E0(DAT_0084B138);
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AA870
long __stdcall FUN_007AA870(void * sink, long result, Channel * channels)
{
	Wstring name;
	char keys[512 + 1];
	memset(keys, 0, sizeof(keys));
	int counter = 0;
	ArrayList<Channel> pinged;

	if (DAT_00B77DC1) {
		return 0;
	}

	int type = 0;
	if (result == 0x4013B) {
		type = 0x29;
	} else {
		g_ChanListQueue.removeHead(type);
		if (result < 0) {
			return 0;
		}
		if (type == 0) {
			g_UserChannels.clear();
		} else {
			FUN_004068E0("clearing game list\n");
			Channel chan;
			memset(&chan, 0, sizeof(chan));
			for (int i = 0; i < g_GameChannelList.length(); i++) {
				g_GameChannelList.get(chan, i);
				if (chan.latency != -1) {
					pinged.addTail(chan);
				}
			}
			g_GameChannelList.clear();
		}
	}

	while (channels != NULL) {
		if (channels->type == 0) {
			g_UserChannels.addTail(*channels);
		} else if (result == 0x4013B) {
			if (channels->latency != -1) {
				Channel * game = NULL;
				for (int j = 0; j < g_GameChannelList.length(); j++) {
					g_GameChannelList.getPointer(&game, j);
					if (game && strcmp((char *)game->name, (char *)channels->name) == 0) {
						FUN_004068E0("Setting ping %d for %s\n", channels->latency, channels->name);
						game->latency = channels->latency;
						break;
					}
				}
			}
		} else {
			g_GameChannelList.addTail(*channels);
			for (int k = 0; k < pinged.length(); k++) {
				Channel old;
				pinged.get(old, k);
				if (strcmp((char *)old.name, (char *)channels->name) == 0) {
					FUN_004068E0("restoring ping %d for channel %s\n", old.latency, channels->name);
					channels->latency = old.latency;
					break;
				}
			}

			name.set((char *)channels->name);
			name.truncate('\'');
			name.toLower();
			if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
				Wstring temp;
				bool found = false;
				for (int m = 0; m < g_WDTTeamLookupQueue.length(); m++) {
					g_WDTTeamLookupQueue.get(temp, m);
					if (temp == name) {
						found = true;
						break;
					}
				}
				if (!found) {
					g_WDTTeamLookupQueue.addHead(name);
				}
			}

			if (!g_UserLocales.contains(name)) {
				Wstring temp;
				bool found = false;
				for (int n = 0; n < g_UserLocaleLookupQueue.length(); n++) {
					g_UserLocaleLookupQueue.get(temp, n);
					if (temp == name) {
						found = true;
						break;
					}
				}
				if (!found) {
					g_UserLocaleLookupQueue.addHead(name);
				}
			}

			if (!g_ActiveLadder->contains(name)) {
				strcat(keys, name.get());
				strcat(keys, ":");
				counter++;
			}
		}

		if (counter > 20) {
			g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00, -1, 0, 0);
			g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00);
			counter = 0;
			memset(keys, 0, sizeof(keys) - 1);
		}
		channels = channels->next;
	}

	if (counter != 0) {
		g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00, -1, 0, 0);
		g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, keys, g_GameSKU & 0xFFFF00);
		memset(keys, 0, sizeof(keys) - 1);
	}

	SetEvent(g_WaitEventHandles[3]);

	if (FUN_0077CF40()) {
		FUN_004068E0("Received channel list, user list is %d long\n", g_UserList.length());
	} else {
		HWND games = WS_Find_Dialog(0x116);
		if (games) {
			FUN_0079AF10(games);
		}
		if (type == 0) {
			FUN_007806C0();
			HWND lobby = WS_Find_Dialog(0x113);
			if (lobby) {
				KillTimer(lobby, 0x55D);
				EnableWindow(GetDlgItem(lobby, 0x55D), TRUE);
			}
		} else {
			FUN_00781480();
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AAFB0
long __stdcall FUN_007AAFB0(void * sink, long result, User * user, char const * text)
{
	if (!DAT_00B77DC1 && result >= 0) {
		bool allowed;
		{
			Wstring narrow(text);
			allowed = FUN_0077D2A0(user);
		}
		if (allowed && FUN_0077D4C0(user)) {
			WideWstring wide = Widen(text);
			if (stricmp(Session.Handle, (char *)user->name) != 0) {
				FUN_005E2700(DAT_007EEF34, DAT_0084B224, user->name, wide.get());
			}
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AB0A0
long __stdcall FUN_007AB0A0(void * sink, long result, User * user, wchar_t const * text)
{
	if (!DAT_00B77DC1 && result >= 0) {
		bool allowed;
		{
			WideWstring wide(text);
			allowed = FUN_0077D2E0(user, &wide);
		}
		if (allowed && FUN_0077D4C0(user)) {
			if (stricmp(Session.Handle, (char *)user->name) != 0) {
				FUN_005E2700(DAT_007EEF34, DAT_0084B224, user->name, text);
			}
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AB170
long __stdcall FUN_007AB170(void * sink, long result, void * channel, User * user, char const * text)
{
	if (!DAT_00B77DC1 && result >= 0) {
		bool allowed;
		{
			Wstring narrow(text);
			allowed = FUN_0077D2A0(user);
		}
		if (allowed && FUN_0077D4C0(user)) {
			WideWstring wide = Widen(text);
			if (stricmp(Session.Handle, (char *)user->name) != 0) {
				FUN_005E2700(DAT_007EEF38, DAT_0084B224, user->name, wide.get());
			}
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AB260
long __stdcall FUN_007AB260(void * sink, long result, void * channel, User * user, wchar_t const * text)
{
	if (!DAT_00B77DC1 && result >= 0) {
		bool allowed;
		{
			WideWstring wide(text);
			allowed = FUN_0077D2E0(user, &wide);
		}
		if (allowed && FUN_0077D4C0(user)) {
			if (stricmp(Session.Handle, (char *)user->name) != 0) {
				FUN_005E2700(DAT_007EEF38, DAT_0084B224, user->name, text);
			}
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AB330
long __stdcall FUN_007AB330(void * sink, long result, int first, int second)
{
	return 0;
}

// FUNCTION: GAMEMD 0x007AB340
long __stdcall FUN_007AB340(void * sink, long result, void * arg)
{
	return 0;
}



// FUNCTION: GAMEMD 0x007AB350
long __stdcall FUN_007AB350(void * sink, long result, time_t bannedTill)
{
	if (!DAT_00B77DC1 && result == 0) {
		wchar_t text[200];
		swprintf(text, StringTable::LoadString("TXT_BANNEDTILL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2269), ctime(&bannedTill));
		ODMessageBox(text, 0, FUN_0077D710, 0);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AB3C0
long __stdcall FUN_007AB3C0(void * sink, long result, char const * name, int banned)
{
	if (!DAT_00B77DC1) {
		if (banned) {
			FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_CHAN_BAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2276), FUN_00735120(name));
		} else {
			FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_CHAN_UNBAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2279), FUN_00735120(name));
		}
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AB440
long __stdcall FUN_007AB440(void * sink, long result, char const * name, unsigned int flags, unsigned int mask)
{
	if (result == 0) {
		for (int i = 0; i < g_UserList.length(); i++) {
			User user;
			if (g_UserList.get(user, i) && stricmp((char *)user.name, name) == 0) {
				user.flags = (user.flags & mask) | flags;
				g_UserList.remove(i);
				User copy = user;
				copy.next = NULL;
				FUN_00783070(&copy);
			}
		}
		Draw_Player_List(0, false);
		return 0;
	}
	return 0;
}

extern char _kickmessage[64];
bool FUN_005E95E0(void);
bool FUN_005E9510(void);
void _DrawMessage(int color, const wchar_t * message, HWND window);

// FUNCTION: GAMEMD 0x007AB670
long __stdcall FUN_007AB670(void * sink, long result, Channel * channel, User * kicked, User * kicker)
{
	if (DAT_00B77DC1) {
		return 0;
	}
	if (result < 0) {
		return 0;
	}
	memset(_kickmessage, 0, sizeof(_kickmessage));
	if (strnicmp((char *)channel->name, "Lob_41_", strlen("Lob_41_")) == 0) {
		int lobby = atol((char *)channel->name + strlen("Lob_41_"));
		int copy = lobby / 8;
		int slot = lobby % 8;
		if (copy == 0) {
			sprintf(_kickmessage, "%s", g_Lobbies[slot]);
		} else {
			sprintf(_kickmessage, "%s %d", g_Lobbies[slot], copy + 1);
		}
	} else {
		sprintf(_kickmessage, "%s", channel->name);
	}
	if (kicked->flags & 0x8000) {
		WideWstring wide;
		if (g_CurrentChannel.type && FUN_005E95E0()) {
			wide = StringTable::LoadString("TXT_CHANNEL_FULL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x22D0);
		} else {
			wchar_t message[1024];
			memset(message, 0, sizeof(message));
			swprintf(message, StringTable::LoadString("TXT_YOURE_KICKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x22D4), FUN_00735120((char *)channel->name), FUN_00735120((char *)kicker->name));
			wide = message;
		}
		if (!g_CurrentChannel.type) {
			ODMessageBox(wide.get(), 0, FUN_0077D710, 0);
			g_CurrentLobby++;
		} else {
			HWND dialog = WS_Find_Dialog(0x113);
			if (dialog) {
				_DrawMessage(DAT_007EEF28, wide.get(), GetDlgItem(dialog, 0x53F));
			}
		}
	} else if (!FUN_005E9510()) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_USER_KICKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x22EC), FUN_00735120((char *)kicked->name), FUN_00735120((char *)channel->name), FUN_00735120((char *)kicker->name));
	}
	Handle_User_Leave(kicked);
	Draw_Player_List(0, false);
	FUN_00781480();
	return 0;
}

static inline void Forget_Locale(Wstring & name)
{
	int old;
	g_UserLocales.remove(name, old);
}

// FUNCTION: GAMEMD 0x007AB8E0
long __stdcall FUN_007AB8E0(void * sink, long result, User * users)
{
	bool found = false;
	if (result < 0) {
		FUN_004068E0("OnUserLocale callback failed!\n");
		return 0;
	}
	User * user = users;
	while (user != NULL) {
		Wstring name;
		name.set((char *)user->name);
		name.toLower();
		Forget_Locale(name);
		int locale = user->locale;
		g_UserLocales.add(name, locale);
		if (stricmp((char *)users->name, g_NickName.get()) == 0) {
			WOLOptionsClass options;
			rc_ptr<PlayerProfile> profile;
			if (options.Is_Bit1_Clear()) {
				profile = FUN_00778A20(&g_NickName);
			} else {
				profile = DAT_00B75E48;
			}
			profile->SetLocale(users->locale, 2);
			if (options.Is_Bit1_Clear()) {
				profile->Save(&g_NickName);
			}
			FUN_004068E0("OnUserLocale callback - Locale is %d\n", users->locale);
			found = true;
		}
		user = user->next;
	}
	if (found) {
		SetEvent(g_WaitEventHandles[14]);
	}
	return 0;
}


static inline void Uncache_Locale(Wstring & name)
{
	g_UserLocales.remove(name);
}

// FUNCTION: GAMEMD 0x007ABAF0
long __stdcall FUN_007ABAF0(void * sink, long result, int newlocale)
{
	Wstring name;
	char handle[64];
	strcpy(handle, Session.Handle);
	name.set(handle);
	name.toLower();
	if (g_UserLocales.contains(name)) {
		Uncache_Locale(name);
	}
	g_UserLocales.add(name, newlocale);
	return 0;
}

static inline void Forget_Team(Wstring & name)
{
	g_WDTTeams.remove(name);
}

// FUNCTION: GAMEMD 0x007ABD40
long __stdcall FUN_007ABD40(void * sink, long result, User * users)
{
	if (result >= 0) {
		User * user = users;
		Wstring name;
		while (user != NULL) {
			name.set((char *)user->name);
			if (g_WDTTeams.contains(name)) {
				Forget_Team(name);
			}
			int team = user->team;
			g_WDTTeams.add(name, team);
			user = user->next;
		}
		Draw_Player_List(0, false);
		return 0;
	}
	FUN_004068E0("OnUserTeam FAILED!\n");
	return 0;
}

// FUNCTION: GAMEMD 0x007ABFA0
long __stdcall FUN_007ABFA0(void * sink, long result, int newteam)
{
	if (result == 0) {
		Session.House = newteam;
		Wstring name;
		char handle[64];
		strcpy(handle, Session.Handle);
		name.set(handle);
		name.toLower();
		if (g_WDTTeams.contains(name)) {
			Forget_Team(name);
		}
		g_WDTTeams.add(name, newteam);
		return 0;
	}
	FUN_004068E0("OnSetTeam FAILED: %d\n", newteam);
	return 0;
}

// FUNCTION: GAMEMD 0x007AC220
long __stdcall FUN_007AC220(void * sink, long result, User * users)
{
	if (result >= 0) {
		for (User * user = users; user != NULL; user = user->next) {
			Wstring name((char *)user->name);
			FUN_0079C650(&name);
		}
		FUN_0079D110();
		Close_Wait_Window(0x200);
		HWND list = WS_Find_Dialog(0x116);
		if (list) {
			FUN_0079AF10(list);
			KillTimer(list, 0x704);
			EnableWindow(GetDlgItem(list, 0x704), TRUE);
		}
	}
	return 0;
}


// FUNCTION: GAMEMD 0x007AC4A0
long __stdcall FUN_007AC4A0(void * sink, long result, void * arg)
{
	return 0;
}

// FUNCTION: GAMEMD 0x007AC4B0
long __stdcall FUN_007AC4B0(void * sink, long result, void * arg)
{
	return 0;
}

// SYNTHETIC: GAMEMD 0x007AC5D0
// ??_GWonlineStringDialogControl@@UAEPAXI@Z

// FUNCTION: GAMEMD 0x007AC610
WonlineStringDialogControl::WonlineStringDialogControl(void)
{
}

// FUNCTION: GAMEMD 0x007AC4C0
long __stdcall FUN_007AC4C0(void * sink, long result, void * arg)
{
	return 0;
}

// FUNCTION: GAMEMD 0x007AC4D0
long __stdcall FUN_007AC4D0(void * sink, long result, void * arg)
{
	return 0;
}

// FUNCTION: GAMEMD 0x007AC4E0
long __stdcall FUN_007AC4E0(void * sink, long result, void * arg)
{
	return 0;
}

// SYNTHETIC: GAMEMD 0x007AC4F0 SYMBOL
// _$E372

// FUNCTION: GAMEMD 0x007AC500
long __stdcall AtlModuleTerm(AtlModule * module)
{
	if (module == NULL) {
		return E_INVALIDARG;
	}
	AtlObjectEntry * entry = module->Entries;
	if (entry != NULL) {
		while (entry->Clsid != NULL) {
			if (entry->Factory != NULL) {
				entry->Factory->Release();
			}
			entry->Factory = NULL;
			entry->ObjectMain(FALSE);
			if (module->cbSize == 0x64) {
				entry = (AtlObjectEntry *)((char *)entry + 0x1C);
			} else {
				entry = (AtlObjectEntry *)((char *)entry + 0x24);
			}
		}
	}
	DeleteCriticalSection(&module->Lock0);
	DeleteCriticalSection(&module->Lock1);
	DeleteCriticalSection(&module->Lock2);
	AtlExitNode * node = module->ExitList;
	while (node != NULL) {
		node->Function(node->Argument);
		AtlExitNode * next = node->Next;
		delete node;
		node = next;
	}
	if (module->Heap != NULL && module->OwnsHeaps) {
		if (module->Heaps != NULL) {
			unsigned int i = 0;
			do {
				HeapDestroy(module->Heaps[i]);
				i++;
			} while (i <= module->HeapCount);
		}
		HeapDestroy(module->Heap);
	}
	return 0;
}

// TEMPLATE: GAMEMD 0x007AC670
// ??1?$CComPtr@UIConnectionPointContainer@@@ATL@@QAE@XZ

// TEMPLATE: GAMEMD 0x007AC680
// ??1?$CComPtr@UIConnectionPoint@@@ATL@@QAE@XZ

// TEMPLATE: GAMEMD 0x007AC690
// ?AddRef@?$CComObject@VCChatEventSink@@@ATL@@UAGKXZ

// TEMPLATE: GAMEMD 0x007AC6B0
// ?Release@?$CComObject@VCChatEventSink@@@ATL@@UAGKXZ

// TEMPLATE: GAMEMD 0x007AC700
// ?QueryInterface@?$CComObject@VCChatEventSink@@@ATL@@UAGJABU_GUID@@PAPAX@Z

// TEMPLATE: GAMEMD 0x007AC800
// ?AddRef@?$CComObject@VCNetUtilEventSink@@@ATL@@UAGKXZ

// TEMPLATE: GAMEMD 0x007AC820
// ?Release@?$CComObject@VCNetUtilEventSink@@@ATL@@UAGKXZ

// TEMPLATE: GAMEMD 0x007AC870
// ?QueryInterface@?$CComObject@VCNetUtilEventSink@@@ATL@@UAGJABU_GUID@@PAPAX@Z

// TEMPLATE: GAMEMD 0x007AC970
// ?Clear@?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x007AC9A0
// ?ID@?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAEHPBQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AC9C0
// ?GetItem@?$VectorClass@PAUtConnInfoStruct@@@@UBEPAUtConnInfoStruct@@H@Z
struct tConnInfoStruct;
template class DynamicVectorClass<tConnInfoStruct *>;

// TEMPLATE: GAMEMD 0x007AD090
// ??8?$VectorClass@PAUtConnInfoStruct@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x007AD0D0
// ?Resize@?$VectorClass@PAUtConnInfoStruct@@@@UAE_NHPBQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD180
// ?Clear@?$VectorClass@PAUtConnInfoStruct@@@@UAEXXZ
template class VectorClass<tConnInfoStruct *>;

// TEMPLATE: GAMEMD 0x007AC9D0
// ?getPointer@?$ArrayList@UServer@@@@QBEDPAPAUServer@@H@Z

// TEMPLATE: GAMEMD 0x007ACA00
// ?add@?$ArrayList@UUser@@@@QAEDAAUUser@@H@Z

// TEMPLATE: GAMEMD 0x007ACB90
// ?get@?$ArrayList@UUser@@@@QBEDAAUUser@@H@Z

// TEMPLATE: GAMEMD 0x007ACBD0
// ?clear@?$ArrayList@H@@QAEXXZ

// TEMPLATE: GAMEMD 0x007ACBE0
// ?add@?$Dictionary@VWstring@@H@@QAEDAAVWstring@@AAH@Z

// TEMPLATE: GAMEMD 0x007ACD30
// ?clear@?$ArrayList@VWstring@@@@QAEXXZ

// TEMPLATE: GAMEMD 0x007ACD60
// ?remove@?$ArrayList@VWstring@@@@QAEDH@Z

// TEMPLATE: GAMEMD 0x007ACE30
// ?get@?$ArrayList@VWstring@@@@QBEDAAVWstring@@H@Z

// TEMPLATE: GAMEMD 0x007ACE60
// ?clear@?$Dictionary@VWstring@@W4WolEntryState@@@@QAEXXZ

// TEMPLATE: GAMEMD 0x007ACF70
// ?clear@?$Dictionary@VWstring@@UChannel@@@@QAEXXZ

// TEMPLATE: GAMEMD 0x007AD1B0
// ?ID@?$VectorClass@PAUtConnInfoStruct@@@@UAEHABQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD1F0
// ?ID@?$VectorClass@PAUtConnInfoStruct@@@@UAEHPBQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD210
// ?Resize@?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAE_NHPBQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD2C0
// ?ID@?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAEHABQAUtConnInfoStruct@@@Z
// SYNTHETIC: GAMEMD 0x007AD2F0
// ??_G?$DNode@VWstring@@ULadder@@@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD310
// ??_G?$DNode@VWstring@@H@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD330
// ??_GWstring@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD350
// ??_G?$DNode@VWstring@@UChannel@@@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD370
// ??_G?$VectorClass@PAUtConnInfoStruct@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD3C0
// ??_G?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x007AD410
// ??0?$CComObject@VCDownloadEventSink@@@ATL@@QAE@PAX@Z

// TEMPLATE: GAMEMD 0x007AD620
// ?AddRef@?$CComObject@VCDownloadEventSink@@@ATL@@UAGKXZ

// TEMPLATE: GAMEMD 0x007AD640
// ?Release@?$CComObject@VCDownloadEventSink@@@ATL@@UAGKXZ

// TEMPLATE: GAMEMD 0x007AD690
// ?QueryInterface@?$CComObject@VCDownloadEventSink@@@ATL@@UAGJABU_GUID@@PAPAX@Z

// TEMPLATE: GAMEMD 0x007AD790
// ?add@?$ArrayList@UChannel@@@@QAEDAAUChannel@@H@Z

// TEMPLATE: GAMEMD 0x007AD930
// ?shrinkVector@?$ArrayList@UChannel@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x007AD9D0
// ?growVector@?$ArrayList@UUser@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x007ADA70
// ?shrinkVector@?$ArrayList@UUser@@@@AAEDXZ
bool operator==(Channel const &, Channel const &);
bool operator<=(Channel const &, Channel const &);
bool operator<(Channel const &, Channel const &);
template class ArrayList<Channel>;
bool operator==(Server const &, Server const &);
bool operator<=(Server const &, Server const &);
bool operator<(Server const &, Server const &);
template class ArrayList<Server>;
bool operator==(User const &, User const &);
bool operator<=(User const &, User const &);
bool operator<(User const &, User const &);
template class ArrayList<User>;

// TEMPLATE: GAMEMD 0x007ADB00
// ?add@?$ArrayList@H@@QAEDAAHH@Z
template class ArrayList<int>;

// TEMPLATE: GAMEMD 0x007ADC60
// ?getPointer@?$Dictionary@VWstring@@ULadder@@@@QAEDAAVWstring@@PAPAULadder@@@Z

// TEMPLATE: GAMEMD 0x007ADCD0
// ?remove@?$Dictionary@VWstring@@ULadder@@@@QAEDAAVWstring@@AAULadder@@@Z

// TEMPLATE: GAMEMD 0x007ADF70
// ?shrink@?$Dictionary@VWstring@@ULadder@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE020
// ?expand@?$Dictionary@VWstring@@ULadder@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE0C0
// ?keyHash@?$Dictionary@VWstring@@ULadder@@@@AAEIAAVWstring@@@Z
template class Dictionary<Wstring, Ladder>;

// TEMPLATE: GAMEMD 0x007AE0E0
// ?expand@?$Dictionary@VWstring@@UHighscore@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE180
// ?getPointer@?$Dictionary@VWstring@@H@@QAEDAAVWstring@@PAPAH@Z

// TEMPLATE: GAMEMD 0x007AE1F0
// ?remove@?$Dictionary@VWstring@@H@@QAEDAAVWstring@@AAH@Z

// TEMPLATE: GAMEMD 0x007AE460
// ?shrink@?$Dictionary@VWstring@@H@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE510
// ?expand@?$Dictionary@VWstring@@H@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE5B0
// ?keyHash@?$Dictionary@VWstring@@H@@AAEIAAVWstring@@@Z
template class Dictionary<Wstring, Highscore>;
template class Dictionary<Wstring, int>;

// TEMPLATE: GAMEMD 0x007AE5D0
// ?add@?$ArrayList@VWstring@@@@QAEDAAVWstring@@H@Z

// TEMPLATE: GAMEMD 0x007AE730
// ?shrinkVector@?$ArrayList@VWstring@@@@AAEDXZ
bool operator<=(Wstring const &, Wstring const &);
bool operator<(Wstring const &, Wstring const &);
template class ArrayList<Wstring>;

// TEMPLATE: GAMEMD 0x007AE7C0
// ?getPointer@?$Dictionary@VWstring@@W4WolEntryState@@@@QAEDAAVWstring@@PAPAW4WolEntryState@@@Z

// TEMPLATE: GAMEMD 0x007AE830
// ?shrink@?$Dictionary@VWstring@@W4WolEntryState@@@@AAEXXZ
template class Dictionary<Wstring, WolEntryState>;

// TEMPLATE: GAMEMD 0x007AE8E0
// ?getPointer@?$Dictionary@VWstring@@UChannel@@@@QAEDAAVWstring@@PAPAUChannel@@@Z

// TEMPLATE: GAMEMD 0x007AE950
// ?remove@?$Dictionary@VWstring@@UChannel@@@@QAEDAAVWstring@@AAUChannel@@@Z

// TEMPLATE: GAMEMD 0x007AEC00
// ?shrink@?$Dictionary@VWstring@@UChannel@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AECB0
// ?keyHash@?$Dictionary@VWstring@@UChannel@@@@AAEIAAVWstring@@@Z
template class Dictionary<Wstring, Channel>;

// TEMPLATE: GAMEMD 0x007AECD0
// ??0?$VectorClass@PAUtConnInfoStruct@@@@QAE@HPBQAUtConnInfoStruct@@@Z

// SYNTHETIC: GAMEMD 0x007AED20
// ??_G?$DNode@VWstring@@UHighscore@@@@QAEPAXI@Z

// TEMPLATE: GAMEMD 0x007AED40
// ?growVector@?$ArrayList@UServer@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x007AEDD0
// ?remove@?$ArrayList@H@@QAEDH@Z

// TEMPLATE: GAMEMD 0x007AEE90
// ?get@?$ArrayList@H@@QBEDAAHH@Z

// TEMPLATE: GAMEMD 0x007AEEC0
// ?growVector@?$ArrayList@H@@AAEDXZ

// TEMPLATE: GAMEMD 0x007AEF50
// ?shrink@?$Dictionary@VWstring@@UHighscore@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AF000
// ?growVector@?$ArrayList@VWstring@@@@AAEDXZ

// FUNCTION: GAMEMD 0x007AD440
HRESULT CDownloadEventSink::OnEnd(void)
{
	SendMessage(FUN_00775B10(), 0x5F4, 0, 0);
	return 0;
}

// FUNCTION: GAMEMD 0x007AD460
HRESULT CDownloadEventSink::OnError(int error)
{
	SendMessage(FUN_00775B10(), 0x5F5, 0, error);
	return 0;
}

// FUNCTION: GAMEMD 0x007AD480
HRESULT CDownloadEventSink::OnProgressUpdate(int bytesread, int totalsize, int timetaken, int timeleft)
{
	wchar_t buffer[200];

	buffer[0] = 0;
	SendDlgItemMessage(FUN_00775B10(), 0x5C6, 0x402, bytesread * 100 / (unsigned)totalsize, 0);
	if (timeleft > 0) {
		swprintf(buffer, StringTable::LoadString("TXT_BYTES_W_TIME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x142F), bytesread, totalsize, timeleft);
	} else {
		swprintf(buffer, StringTable::LoadString("TXT_BYTES_WO_TIME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1434), bytesread, totalsize);
	}
	if (time(NULL) - prev_stat != 0) {
		HWND item = GetDlgItem(FUN_00775B10(), 0x5C8);
		SendMessage(item, 0x4B2, 0, (LPARAM)buffer);
		prev_stat = time(NULL);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AD580
HRESULT CDownloadEventSink::OnStatusUpdate(int state)
{
	wchar_t buffer[200];

	buffer[0] = 0;
	switch (state) {
		case 2:
			swprintf(buffer, StringTable::LoadString("TXT_CONNECTING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1447));
			break;

		case 4:
			swprintf(buffer, StringTable::LoadString("TXT_FINDING_PATCH", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x144B));
			break;
	}
	HWND item = GetDlgItem(FUN_00775B10(), 0x5C8);
	SendMessage(item, 0x4B2, 0, (LPARAM)buffer);
	return 0;
}

// FUNCTION: GAMEMD 0x007AD610
HRESULT CDownloadEventSink::OnQueryResume(void)
{
	return 0;
}


#include "multiplayer.h"
#include "progress.h"
#include "firewall.h"
extern bool DAT_00AC11C2;
extern int DAT_007EEF30, DAT_007EEF34, DAT_007EEF38, DAT_007EEF44;
extern wchar_t DAT_0084B224[], DAT_0084B94C[], DAT_0084B9B0[];
extern char DAT_0084AD30[];
extern char DAT_0084AD00[];
extern char DAT_0084ACE0[];
void __fastcall FUN_005E74E0(HWND window);
wchar_t *__fastcall FUN_0078F200(wchar_t const *name);
int __fastcall FUN_004E3830(int id);
wchar_t const *__fastcall FUN_004E38A0(int side);
void __fastcall FUN_004E3B90(HWND dialog);
int __fastcall FUN_004E4230(int id);
wchar_t const *__fastcall FUN_004E42A0(int color);
void FUN_004E43C0(void);
void __fastcall FUN_004E4820(HWND dialog);
int __fastcall FUN_004E4E20(HWND dialog, int id, int index);
int __fastcall FUN_004E4EC0(int id);
void FUN_004E4F50(void);
void __fastcall FUN_004E4FC0(int count);
void __fastcall FUN_004E5310(HWND dialog);
int __fastcall FUN_004E5900(HWND dialog, int id, int index);
int __fastcall FUN_004E59A0(int id);
void FUN_004E5AC0(void);
void __fastcall FUN_004E5D60(HWND dialog);
int __fastcall FUN_004E6030(HWND dialog, int id, int index);
MultiplayerGameMode *First_Game_Mode(void);
void FUN_005E2EF0(HWND dialog, MultiplayerGameMode *mode);
void __fastcall FUN_005E2F60(HWND dialog);
int __fastcall FUN_005E9950(int id);
wchar_t const *__fastcall FUN_005E99C0(NodeNameType *slot);
int __fastcall FUN_005E9B20(HWND dialog, int id, int index);
void __fastcall FUN_005E9B60(HWND dialog, int combo);
void __fastcall FUN_005E9C00(HWND dialog, int combo);
void __fastcall FUN_005E9CE0(HWND dialog, int combo);
void __fastcall FUN_005E9DB0(HWND dialog, int combo);
void __fastcall FUN_005EE510(int arg);
WideWstring FUN_00603F60(HWND window, HWND control);
WideWstring FUN_00604060(HWND window, HWND control);

#include "version.h"

extern int DAT_008316A4;
extern bool DAT_008316D4;
bool FUN_005E8760(void);
bool __fastcall FUN_005E9470(NodeNameType *slot);
void __fastcall FUN_005ED400(HWND dialog, SessionOptionsClass *options);
void __fastcall FUN_005ED5A0(HWND dialog, SessionOptionsClass *options);
void __fastcall FUN_005EC9F0(HWND dialog, int control, NodeNameType *slot, int flag);
void __fastcall FUN_005E9E00(HWND dialog);
void FUN_005E3630(void);
void __fastcall FUN_005EC030(HWND dialog);
bool __fastcall FUN_005EB660(HWND dialog);
void __fastcall FUN_005EC3A0(HWND dialog, int old_scenario, int scenario);
void __fastcall FUN_005ED370(HWND dialog);
void FUN_005EE3A0(void);
void __fastcall FUN_005EE6A0(HWND dialog);
void __fastcall FUN_004E3CE0(HWND dialog, int slot);
void __fastcall FUN_004E48E0(HWND dialog, int slot);
void __fastcall FUN_004E53D0(HWND dialog, int slot);
void __fastcall FUN_005E9E70(HWND dialog, int slot, bool enabled);
void __fastcall FUN_005EA2A0(HWND dialog);

// FUNCTION: GAMEMD 0x0078F7F0
BOOL CALLBACK WOL_Game_Options_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int i;
	int handled = UI::StandardWndProc(window, message, wparam, lparam);
	if (handled)
		return handled;
	if (!g_GameStarted) {
		switch (message) {
		case 0x497:
			FUN_0078F720(window);
			DisplayGameopts(window, 1);
			break;
		case WM_HSCROLL:
			if ((short)wparam == 8) {
				if (GetDlgItem(window, 0x54f))
					Session.Options.UnitCount = SendDlgItemMessageA(window, 0x54f, 0x400, 0, 0);
				if (GetDlgItem(window, 0x402))
					Session.Options.Credits = SendDlgItemMessageA(window, 0x402, 0x400, 0, 0);
				if (GetDlgItem(window, 0x529))
					Session.Options.GameSpeed = 6 - SendDlgItemMessageA(window, 0x529, 0x400, 0, 0);
				for (int accepted = 1; accepted < 8; ++accepted)
					g_UserInfo[accepted].Accepted = 0;
				FUN_005E9850();
				Draw_Player_List(0, false);
			}
			break;
		case WM_COMMAND: {
			bool changed = false;
			switch ((int)(wparam & 0xffff)) {
			case 0x3e8:
				Session.Options.MCVRedeploy = false;
				if (SendDlgItemMessageA(window, 0x3e8, 0xf0, 0, 0) == 1)
					Session.Options.MCVRedeploy = true;
				changed = true;
				break;
			case 0x54d:
				Session.Options.Goodies = false;
				if (SendDlgItemMessageA(window, 0x54d, 0xf0, 0, 0) == 1)
					Session.Options.Goodies = true;
				changed = true;
				break;
			case 0x551:
				Session.Options.CrapEngineers = false;
				if (SendDlgItemMessageA(window, 0x551, 0xf0, 0, 0) == 1)
					Session.Options.CrapEngineers = true;
				changed = true;
				break;
			case 0x596:
				Session.Options.HarvTruce = false;
				if (SendDlgItemMessageA(window, 0x596, 0xf0, 0, 0) == 1)
					Session.Options.HarvTruce = true;
				changed = true;
				break;
			case 0x597:
				Session.Options.AlliesAllowed = false;
				if (SendDlgItemMessageA(window, 0x597, 0xf0, 0, 0) == 1)
					Session.Options.AlliesAllowed = true;
				changed = true;
				break;
			case 0x599:
				Session.Options.FogOfWar = false;
				if (SendDlgItemMessageA(window, 0x599, 0xf0, 0, 0) == 1)
					Session.Options.FogOfWar = true;
				changed = true;
				break;
			case 0x624:
				Session.Options.BridgeDestruction = false;
				if (SendDlgItemMessageA(window, 0x624, 0xf0, 0, 0) == 1)
					Session.Options.BridgeDestruction = true;
				changed = true;
				break;
			case 0x69a:
				Session.Options.SWAllowed = false;
				if (SendDlgItemMessageA(window, 0x69a, 0xf0, 0, 0) == 1)
					Session.Options.SWAllowed = true;
				break;
			case 0x54b:
				Session.Options.Bases = false;
				if (SendDlgItemMessageA(window, 0x54b, 0xf0, 0, 0) == 1)
					Session.Options.Bases = true;
				else if (!Session.Options.Bases) {
					Session.Options.ShortGame = 0;
					SendDlgItemMessageA(window, 0x54e, 0xf1, 0, 0);
				}
				changed = true;
				break;
			case 0x54e:
				Session.Options.ShortGame = 0;
				if (SendDlgItemMessageA(window, 0x54e, 0xf0, 0, 0) == 1) {
					Session.Options.ShortGame = 1;
					if (!Session.Options.Bases) {
						Session.Options.Bases = true;
						SendDlgItemMessageA(window, 0x54b, 0xf1, 1, 0);
					}
				}
				changed = true;
				break;
			case 0x69d:
				Session.Options.BuildOffAlly = false;
				if (SendDlgItemMessageA(window, 0x69d, 0xf0, 0, 0) == 1)
					Session.Options.BuildOffAlly = true;
				break;
			}
			if (changed) {
				for (int accepted = 1; accepted < 8; ++accepted)
					g_UserInfo[accepted].Accepted = 0;
				FUN_005E9850();
				Draw_Player_List(0, false);
			}
			break;
		}
		}
	}
	switch (message) {
	case WM_NCHITTEST: {
		HWND tooltip = GetDlgItem(window, 0x695);
		if (!tooltip)
			goto host_unhandled;
		POINT point;
		point.x = lparam & 0xffff;
		point.y = (unsigned)lparam >> 16;
		RECT bounds;
		GetWindowRect(window, &bounds);
		point.x -= bounds.left;
		point.y -= bounds.top;
		WideWstring text;
		HWND child = ChildWindowFromPointEx(window, point, 1);
		if (child) {
			text = FUN_00603F60(window, child);
			if (wcscmp(text.get(), DAT_00887734) == 0)
				text = FUN_00604060(window, child);
		}
		SendMessageA(tooltip, 0x4b2, 0, (LPARAM)text.text());
		return 0;
	}
	case WM_DRAWITEM:
		OwnerDraw::Draw_Item((LPDRAWITEMSTRUCT)lparam);
		return 1;
	case WM_DESTROY:
		if (MultiplayerMapPreview) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = 0;
		}
		if (!Progress.Has_Dialog())
			goto host_unhandled;
		Progress.Clear_Dialog();
		return 0;
	case WM_PAINT:
		OwnerDraw::Paint(window);
		FUN_00607FD0(window);
		if (MultiplayerMapPreview)
			MultiplayerMapPreview->DrawStartPoints(window);
		else if (Session.Options.ScenarioIndex == -1 && RandomMapGen.MapPreview)
			RandomMapGen.MapPreview->DrawStartPoints(window);
		if (Progress.Has_Dialog())
			Progress.Display_Progress();
		ValidateRect(window, 0);
		return 0;
	case WM_COMMAND: {
		int command = wparam & 0xffff;
		switch (command) {
		case WM_DESTROY:
			if (g_GameStarted)
				goto host_unhandled;
			FUN_005ED400(window, &Session.WOL);
			Session.Write_MultiPlayer_Settings();
			if (MultiplayerMapPreview) {
				delete MultiplayerMapPreview;
				MultiplayerMapPreview = 0;
			}
			FUN_007757E0(window, 2);
			FUN_0072CF90();
			ShowWindow(window, 0);
			OwnerDraw::SetUserData2(window, 1);
			g_pChat->RequestChannelLeave();
			g_JoinLobbyNow = 1;
			return 0;
		case 0x3ed: {
			HWND input = GetDlgItem(window, 0x540);
			WideWstring text = FUN_005EDD00(input);
			SendMessageA(input, 0x4b2, 0, (LPARAM)DAT_00887734);
			SetFocus(input);
			InvalidateRect(input, 0, 0);
			int color = DAT_007EEF38;
			if (strlen((char *)g_CurrentChannel.name) == 0)
				PMessagePrintf(
					-1, StringTable::LoadString("TXT_NOT_IN_CHAN", 0, "D:\\ra2mdpost\\wonline.cpp", 0x41fe));
			else if (text.length() > 0) {
				if (Send_Chat_Action(text))
					color = DAT_007EEF34;
				wchar_t *message_text = text.get();
				PMessagePrintf(color, DAT_0084B224, g_NickName.c_str(), message_text);
			} else
				PMessagePrintf(
					-1, StringTable::LoadString("TXT_ENTER_MESSAGE", 0, "D:\\ra2mdpost\\wonline.cpp", 0x4205));
			return 0;
		}
		case 0x540: {
			wchar_t nickname[16], primary_command[64], alternate_command[64];
			WideWstring text = FUN_005EDD00((HWND)lparam);
			if ((unsigned short)(wparam >> 16) == 0x501) {
				SendMessageA((HWND)lparam, 0x4b2, 0, (LPARAM)DAT_00887734);
				if (text.length() > 0) {
					int parsed = FUN_0078A6B0(text.get());
					if (parsed != 0) {
						if (parsed == 1) {
							int count = 0;
							unsigned length = text.length() + 1;
							wchar_t *copy = (wchar_t *)operator new(length * 2);
							wcscpy(primary_command, StringTable::LoadString("TXT_PAGE_COMMAND", 0,
																			"D:\\ra2mdpost\\wonline.cpp", 0x418a));
							if (!copy)
								return 0;
							copy = wcsncpy(copy, text.get(), length);
							if (!copy)
								return 0;
							wchar_t *body = copy + wcslen(primary_command);
							while (*body && wcsncmp(body, L" ", 1) == 0)
								++body;
							while (*body) {
								if (wcsncmp(body, L" ", 1) == 0 || count > 10)
									break;
								nickname[count++] = *body++;
							}
							nickname[count] = 0;
							while (*body && wcsncmp(body, L" ", 1) == 0)
								++body;
							if (count > 10)
								PMessagePrintf(-1, StringTable::LoadString("TXT_NAME_TOO_LONG", 0,
																		   "D:\\ra2mdpost\\wonline.cpp", 0x41a8));
							else if (count == 0)
								PMessagePrintf(-1, StringTable::LoadString("TXT_NO_NAME_SPECIFIED", 0,
																		   "D:\\ra2mdpost\\wonline.cpp", 0x41aa));
							else if (wcslen(body) > 0)
								PMessagePrintf(DAT_007EEF30, DAT_0084B9B0, g_NickName.c_str(), nickname, body);
							else
								PMessagePrintf(-1, StringTable::LoadString("TXT_CANT_SEND_BLANK_PAGE", 0,
																		   "D:\\ra2mdpost\\wonline.cpp", 0x41b0));
							operator delete(copy);
						}
					} else if (FUN_0078A8F0(text.get())) {
						unsigned length = text.length() + 1;
						wchar_t *copy = (wchar_t *)operator new(length * 2);
						wcscpy(primary_command, StringTable::LoadString("TXT_REPLY_COMMAND", 0,
																		"D:\\ra2mdpost\\wonline.cpp", 0x41c2));
						wcscpy(alternate_command, StringTable::LoadString("TXT_REPLY_COMMAND_SHORT", 0,
																		  "D:\\ra2mdpost\\wonline.cpp", 0x41c3));
						if (!copy)
							return 0;
						copy = wcsncpy(copy, text.get(), length);
						if (!copy)
							return 0;
						wchar_t *body = copy + (wcsncmp(copy, primary_command, wcslen(primary_command)) == 0
													? wcslen(primary_command)
													: wcslen(alternate_command));
						while (*body && wcsncmp(body, L" ", 1) == 0)
							++body;
						wcscpy(nickname, FUN_00735120(DAT_00B77E30));
						if (wcslen(body) > 0 && wcslen(nickname) > 0)
							PMessagePrintf(DAT_007EEF30, DAT_0084B9B0, g_NickName.c_str(), nickname, body);
						else if (wcslen(nickname) > 0)
							PMessagePrintf(-1, StringTable::LoadString("TXT_CANT_SEND_BLANK_PAGE", 0,
																	   "D:\\ra2mdpost\\wonline.cpp", 0x41e0));
						operator delete(copy);
					} else if (!FUN_0078A470(text.get())) {
						int color = DAT_007EEF44;
						if (Send_Chat_Message(text) == 1)
							color = DAT_007EEF30;
						wchar_t *message_text = text.get();
						PMessagePrintf(color, DAT_0084B94C, g_NickName.c_str(), message_text);
					}
				}
			}
			return 0;
		}
		case 0x411:
		case 0x412:
		case 0x413:
		case 0x414:
		case 0x415:
		case 0x416:
		case 0x417:
		case 0x418:
			if ((unsigned short)(wparam >> 16) != 1 || g_GameStarted)
				goto host_unhandled;
			if (!FUN_005E8760())
				goto host_unhandled;
			{
				NodeNameType *entry = (NodeNameType *)FUN_005E9B20(window, command, -1);
				FUN_005EC9F0(window, command, entry, 2);
			}
			return 0;
		case 0x510:
		case 0x513:
		case 0x514:
		case 0x51e:
		case 0x51f:
		case 0x520:
		case 0x521:
		case 0x6a1:
			if ((unsigned short)(wparam >> 16) == 1 && !g_GameStarted) {
				FUN_005E9B60(window, command);
				PumpGameopts(true, false);
				FUN_005EA3E0(window, true);
			}
			break;
		case 0x522:
		case 0x523:
		case 0x524:
		case 0x525:
		case 0x526:
		case 0x527:
		case 0x528:
		case 0x6a2:
			if ((unsigned short)(wparam >> 16) == 1 && !g_GameStarted) {
				FUN_005E9C00(window, command);
				PumpGameopts(true, false);
				FUN_005EA3E0(window, true);
			}
			break;
		case 0x588: {
			SetFocus(GetDlgItem(window, 0x540));
			if (g_GameStartRequested == 1)
				goto host_unhandled;
			FUN_005ED400(window, &Session.WOL);
			Session.Write_MultiPlayer_Settings();
			FUN_005EA3E0(window, true);
			int observers = 0;
			for (i = 0; i < Session.Players.Count(); ++i)
				if (Session.Players[i]->Slot.SpectatorFlag == -1)
					++observers;
			if (g_UserList.length() - observers < 2) {
				PMessagePrintf(-1,
							   StringTable::LoadString("TXT_ONLY_ONE", 0, "D:\\ra2mdpost\\wonline.cpp", 0x406c));
				return 0;
			}
			if (observers && Session.Options.AIPlayers) {
				PMessagePrintf(
					-1, StringTable::LoadString("TXT_NO_AI_OBSERVATION", 0, "D:\\ra2mdpost\\wonline.cpp", 0x4078));
				return 0;
			}
			bool missing = false;
			for (i = 1; i < Session.Players.Count(); ++i)
				if (!Session.Players[i]->Slot.Unknown7F) {
					PMessagePrintf(
						-1, StringTable::LoadString("GUI:HostNoMap", 0, "D:\\ra2mdpost\\wonline.cpp", 0x4084),
						Session.Players[i], Session.Options.ScenarioDescription);
					missing = true;
				}
			if (missing)
				return 0;
			FUN_005E9E00(window);
			int team = -1;
			bool all_allied = true;
			for (i = 0; i < 8; ++i) {
				NodeNameType *slot = Session.Slots[i];
				if (FUN_005E9470(slot)) {
					if (FUN_005E93F0(slot) && slot->Slot.SpectatorFlag != -1) {
						if (i == 0)
							team = slot->Slot.Team;
						else if (team != slot->Slot.Team) {
							all_allied = false;
							break;
						}
					} else if (FUN_005E9440(slot) && slot->Slot.SpectatorFlag != -1) {
						if (team != Session.Options.AISlots.Allies[i]) {
							all_allied = false;
							break;
						}
					}
				}
			}
			if (all_allied && team >= 0 && !Session.MPGameMode->vt_entry_04()) {
				PMessagePrintf(
					-1, StringTable::LoadString("TXT_CANNOT_ALLY", 0, "D:\\ra2mdpost\\wonline.cpp", 0x40af));
				return 0;
			}
			for (i = 0; i < g_UserList.length(); ++i)
				if (!g_UserInfo[i].Accepted) {
					FUN_005E3630();
					PMessagePrintf(-1, StringTable::LoadString("GUI:HostGameStartHost", 0,
															   "D:\\ra2mdpost\\wonline.cpp", 0x40bf));
					return 0;
				}
			if (Session.Type == 4 && Session.WOLWelcomeChoice == 3) {
				if (g_UserList.length() < (int)g_CurrentChannel.maxUsers) {
					PMessagePrintf(
						-1,
						StringTable::LoadString("TXT_NEED_MORE_PLAYERS", 0, "D:\\ra2mdpost\\wonline.cpp", 0x40c9),
						g_CurrentChannel.maxUsers);
					return 0;
				}
				int team1 = 0, team2 = 0;
				for (i = 0; i < g_UserList.length(); ++i) {
					if (g_UserInfo[i].House == 0)
						++team2;
					else
						++team1;
				}
				if (team1 != team2) {
					PMessagePrintf(-1, StringTable::LoadString("TXT_NEED_EQUAL_TEAMS", 0,
															   "D:\\ra2mdpost\\wonline.cpp", 0x40d8));
					return 0;
				}
			}
			int limit = FUN_005E64C0(Session.Options.ScenarioIndex);
			int const requested = SendDlgItemMessageA(window, 0x552, 0x400, 0, 0);
			int const players = g_UserList.length();
			if (limit < (players - observers) + requested) {
				PMessagePrintf(
					-1, StringTable::LoadString("TXT_SCENARIO_TOO_SMALL", 0, "D:\\ra2mdpost\\wonline.cpp", 0x40e3),
					limit);
				return 0;
			}
			if (players > 6) {
				PMessagePrintf(
					-1, StringTable::LoadString("TXT_TOO_MANY_PLAYERS", 0, "D:\\ra2mdpost\\wonline.cpp", 0x40eb));
				return 0;
			}
			MultiplayerGameMode *mode = Session.MPGameMode;
			WideWstring error;
			if (!mode->vt_entry_14(&error)) {
				PMessagePrintf(-1, error.get());
				EnableWindow(GetDlgItem(FUN_00775B10(), 0x588), 1);
				return 0;
			}
			if (FUN_0077D990(&g_CurrentChannel)) {
				User *user = 0;
				unsigned squad1 = 0, squad2 = 0;
				int count1 = 0, count2 = 0;
				char observer[10] = "";
				if (DAT_008316A4 >= 0) {
					NodeNameType *node = Session.Slots[DAT_008316A4];
					if (FUN_005E9470(node) && node->Slot.SpectatorFlag == -1) {
						char *name = FUN_007350C0(node->Name);
						strncpy(observer, name, 10);
						observer[9] = 0;
					}
				}
				for (i = 0; i < g_UserList.length(); ++i) {
					g_UserList.getPointer(&user, i);
					if (stricmp(observer, (char *)user->name)) {
						unsigned squad = user->squadID;
						if (squad < 1) {
							PMessagePrintf(-1, StringTable::LoadString("TXT_CLAN_NOSTART", 0,
																	   "D:\\ra2mdpost\\wonline.cpp", 0x4115));
							return 0;
						}
						if (!squad1 || squad1 == squad) {
							squad1 = squad;
							++count1;
						} else {
							if (squad2 && squad2 != squad) {
								PMessagePrintf(-1, StringTable::LoadString("TXT_CLAN_NOSTART", 0,
																		   "D:\\ra2mdpost\\wonline.cpp", 0x4124));
								return 0;
							}
							squad2 = squad;
							++count2;
						}
					}
				}
				if (count1 != count2) {
					PMessagePrintf(
						-1, StringTable::LoadString("TXT_CLAN_NOSTART", 0, "D:\\ra2mdpost\\wonline.cpp", 0x412a));
					return 0;
				}
			}
			mode->vt_entry_68();
			Session.FUN_0069B8C0();
			FUN_0077DB90();
			PumpGameopts(true, true);
			mode->vt_entry_64();
			mode->vt_entry_24();
			User *temp, *head = NULL;
			User slot_user;
			memset(&slot_user, 0, sizeof(slot_user));
			for (i = 0; i < g_UserList.length(); ++i) {
				g_UserList.get(slot_user, i);
				temp = new User;
				*temp = slot_user;
				temp->next = head;
				head = temp;
			}
			g_pChat->RequestGameStart(head);
			g_GameStarted = 1;
			g_GameStartRequested = true;
			temp = head;
			while (temp) {
				head = temp->next;
				delete temp;
				temp = head;
			}
			return 0;
		}
		case 0x5c2:
			DoFindPage();
			return 0;
		case 0x6a3:
		case 0x6a4:
		case 0x6a5:
		case 0x6a6:
		case 0x6a7:
		case 0x6a8:
		case 0x6aa:
		case 0x6ab:
			if ((unsigned short)(wparam >> 16) == 1 && !g_GameStarted) {
				FUN_005E9CE0(window, command);
				PumpGameopts(true, false);
				FUN_005EA3E0(window, true);
			}
			break;
		case 0x76d:
		case 0x76e:
		case 0x76f:
		case 0x770:
		case 0x771:
		case 0x772:
		case 0x773:
		case 0x774:
			if ((unsigned short)(wparam >> 16) == 1 && !g_GameStarted) {
				FUN_005E9DB0(window, command);
				PumpGameopts(true, false);
				FUN_005EA3E0(window, true);
			}
			break;
		case 0x5aa: {
			if (g_GameStarted)
				goto host_unhandled;
			int old = Session.Options.ScenarioIndex;
			bool ai = !Session.MPGameMode || Session.MPGameMode->vt_entry_40();
			bool observers = !Session.MPGameMode || Session.MPGameMode->vt_entry_BC();
			char filename[132];
			wchar_t description[130];
			strcpy(filename, Session.ScenarioFileName);
			wcscpy(description, Session.Options.ScenarioDescription);
			DAT_008316D4 = false;
			OwnerDraw::DrawCampaignMenuTransition(window, false);
			FUN_0052FEC0(0, true);
			bool cancelled = Scenario_Dialog(MainWindow) == 2;
			ShowWindow(window, 5);
			OwnerDraw::SetUserData2(window, 0);
			SetFocus(GetDlgItem(window, 0x540));
			if (cancelled) {
				Session.Options.ScenarioIndex = old;
				Set_Scenario_Info_From_Index(old);
				FUN_005E74E0(window);
				DAT_008316D4 = true;
				ShowWindow(window, 5);
				OwnerDraw::SetUserData2(window, 0);
				FUN_005ED370(window);
				PumpGameopts(true, false);
				InvalidateRect(window, 0, 0);
				return 0;
			}
			if (!Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex))
				Session.Options.ScenarioIndex = old;
			else {
				bool new_ai = !Session.MPGameMode || Session.MPGameMode->vt_entry_40();
				bool new_observers = !Session.MPGameMode || Session.MPGameMode->vt_entry_BC();
				if (observers && !new_observers)
					FUN_005EC030(window);
				if (ai && !new_ai) {
					if (FUN_005EB660(window)) {
						Session.Options.AIPlayers = 0;
						FUN_0077DB90();
						Draw_Player_List(0, false);
						PumpGameopts(true, false);
					} else
						Draw_Player_List(0, true);
				}
				FUN_005EE3A0();
				FUN_005EC3A0(window, old, Session.Options.ScenarioIndex);
			}
			FUN_0078F000();
			DAT_008316D4 = true;
			ShowWindow(window, 5);
			OwnerDraw::SetUserData2(window, 0);
			FUN_005E2F60(window);
			if (strcmp(filename, Session.ScenarioFileName) ||
				wcscmp(description, Session.Options.ScenarioDescription)) {
				for (int accepted = 1; accepted < 8; ++accepted)
					g_UserInfo[accepted].Accepted = 0;
			}
			FUN_005E9850();
			FUN_005ED370(window);
			int start_count = RandomMapWaypointCount(Session.Options.ScenarioIndex);
			if (Session.MPGameMode && Session.MPGameMode->vt_entry_04() &&
				Scen->NumCoopHumanStartSpots <= start_count)
				FUN_004E4FC0(Scen->NumCoopHumanStartSpots);
			else
				FUN_004E4FC0(start_count);
			FUN_004E5310(window);
			FUN_005EE6A0(window);
			FUN_004E5D60(window);
			FUN_005EE510(-1);
			FUN_005EA3E0(window, true);
			PumpGameopts(true, false);
			InvalidateRect(window, 0, 0);
			return 0;
		}
		}
		goto host_unhandled;
	}
	case 0x497: {
		SendDlgItemMessageA(window, 0x3ed, 0x49c, 0, (LPARAM)SurfaceCache.GetSurface("wodact.pcx", 0));
		SendDlgItemMessageA(window, 0x3ed, 0x4aa, 0, (LPARAM)SurfaceCache.GetSurface("wouact.pcx", 0));
		MultiplayerGameMode *mode = First_Game_Mode();
		Session.Options.MPModeIndex = mode->MPModeIndex;
		Session.MPGameMode = mode;
		mode->vt_entry_20();
		FUN_005E2EF0(window, mode);
		Session.Options.ScenarioIndex = mode->FirstValidMapIndex();
		Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex);
		FUN_004E3B90(window);
		FUN_004E43C0();
		FUN_004E4820(window);
		FUN_004E4F50();
		FUN_004E5AC0();
		FUN_004E5D60(window);
		FUN_005EE510(-1);
		for (int slot = 0; slot < 8; ++slot) {
			Session.Options.AISlots.Starts[slot] = -2;
			Session.Options.AISlots.Allies[slot] = -2;
		}
		FUN_004E3CE0(window, Session.LocalSlot);
		FUN_004E48E0(window, Session.LocalSlot);
		FUN_004E53D0(window, Session.LocalSlot);
		FUN_005ED5A0(window, &Session.WOL);
		FUN_0078F720(window);
		if (Session.MPGameMode && !Session.MPGameMode->vt_entry_BC()) {
			DAT_008316A4 = -1;
			Game::ObserverMode = 0;
			FUN_005EE350(0);
			Session.Players[0]->Slot.SpectatorFlag = 0;
			Session.Slots[0]->Set_Country(Session.MPGameMode->vt_entry_28());
			Session.Slots[0]->Set_Color(-2);
			Session.Slots[0]->Set_Start(-2);
			Session.Slots[0]->Set_Team(Session.MPGameMode->vt_entry_2C());
			FUN_004E4820(window);
		}
		FUN_005E9E70(window, 0, true);
		FUN_005EC3A0(window, -1, Session.Options.ScenarioIndex);
		FUN_0078F000();
		FUN_005EA2A0(window);
		FUN_005E2EF0(window, Session.MPGameMode);
		FUN_005E2F60(window);
		FUN_005ED370(window);
		int start_count = RandomMapWaypointCount(Session.Options.ScenarioIndex);
		if (Session.MPGameMode && Session.MPGameMode->vt_entry_04() && Scen->NumCoopHumanStartSpots <= start_count)
			FUN_004E4FC0(Scen->NumCoopHumanStartSpots);
		else
			FUN_004E4FC0(start_count);
		FUN_004E5310(window);
		FUN_004E5D60(window);
		FUN_005EE510(-1);
		Session.PlayingAgainstVersion = VerNum.Version_Number();
		SetTimer(window, 0x14af8, 5000, 0);
		DisplayGameopts(window, 1);
		FUN_005EA3E0(window, true);
		SetFocus(GetDlgItem(window, 0x540));
		if (g_GameStarted)
			goto host_unhandled;
		User user;
		memset(&user, 0, sizeof(user));
		int count = g_UserList.length();
		int handle = 0;
		int i = 0;
		if (count > 0) {
			do {
				g_UserList.get(user, i);
				in_addr address;
				address.s_addr = user.ipaddr;
				FUN_004068E0("Requesting ping for %s at %s\n", user.name, inet_ntoa(address));
				g_pNetUtil->RequestPing(inet_ntoa(address), 1000, &handle);
				++i;
			} while (i < count);
			return 0;
		}
		goto host_unhandled;
	}
	case WM_HSCROLL:
	case 0x115:
		if ((HWND)lparam == GetDlgItem(window, 0x732)) {
			if (g_GameStarted)
				goto host_unhandled;
			Options.CampDifficulty = SendDlgItemMessageA(window, 0x732, 0x400, 0, 0);
			return 0;
		}
		if (!g_GameStarted) {
			Session.Options.UnitCount = SendDlgItemMessageA(window, 0x54f, 0x400, 0, 0);
			Session.Options.Credits = SendDlgItemMessageA(window, 0x402, 0x400, 0, 0);
			Session.Options.GameSpeed = 6 - SendDlgItemMessageA(window, 0x529, 0x400, 0, 0);
			return 0;
		}
		goto host_unhandled;
	case WM_TIMER: {
		if (wparam != 0x14af8 || !FUN_005E8760())
			goto host_unhandled;
		FUN_007A0F40();
		if (g_GameStarted)
			goto host_unhandled;
		User user;
		memset(&user, 0, sizeof(user));
		int count = g_UserList.length();
		int handle = 0;
		int i = 0;
		if (count <= 0)
			goto host_unhandled;
		do {
			g_UserList.get(user, i);
			in_addr address;
			address.s_addr = user.ipaddr;
			FUN_004068E0("Requesting ping for %s at %s\n", user.name, inet_ntoa(address));
			g_pNetUtil->RequestPing(inet_ntoa(address), 1000, &handle);
			++i;
		} while (i < count);
		return 0;
	}
	case 0x49b: {
		HWND control = GetDlgItem(window, wparam);
		SendMessageA(control, 0x4b3, 0x7f, lparam);
		return 0;
	}
	case 0x4e9: {
		TooltipRequest *request = (TooltipRequest *)lparam;
		int id;
		{
			WideWstring &text = request->Text;
			text = (wchar_t const *)0;
			if (!request->Window)
				goto tooltip_done;
			id = GetDlgCtrlID((HWND)request->Window);
			if (request->Index != -1) {
				if (FUN_005E9950(id) == -1) {
					if (FUN_004E3830(id) == -1) {
						if (FUN_004E4230(id) != -1) {
							text = FUN_004E42A0(FUN_004E4E20(window, id, request->Index));
							return 1;
						}
					} else {
						int country = FUN_004E4170(window, id, request->Index);
						if (country != -1) {
							text = FUN_004E38A0(country);
							return 1;
						}
					}
				} else {
					NodeNameType *entry = (NodeNameType *)FUN_005E9B20(window, id, request->Index);
					text = FUN_005E99C0(entry);
					return 1;
				}
				goto tooltip_done;
			}
			if (FUN_005E9950(id) != -1) {
				NodeNameType *entry = (NodeNameType *)FUN_005E9B20(window, id, -1);
				if (entry) {
					WideWstring label = FUN_00604060(window, request->Window);
					if ((wcscmp(label.get(), StringTable::LoadString("STT:GuestComboPlayer", 0,
																	 "D:\\ra2mdpost\\wonline.cpp", 0x3e6b)) == 0 ||
						 wcscmp(label.get(), StringTable::LoadString("STT:HostComboPlayer", 0,
																	 "D:\\ra2mdpost\\wonline.cpp", 0x3e6b)) ==
							 0) &&
						FUN_005E93F0(entry))
						label = FUN_0078F200(entry->Name);
					text = label.get();
				}
				goto tooltip_done;
			}
		}
		{
			int player;
			if (id == 0x754)
				player = 0;
			else if (id == 0x755)
				player = 1;
			else if (id == 0x756)
				player = 2;
			else if (id == 0x757)
				player = 3;
			else if (id == 0x758)
				player = 4;
			else if (id == 0x759)
				player = 5;
			else if (id == 0x75a)
				player = 6;
			else if (id == 0x75b)
				player = 7;
			else
				goto tooltip_done;
			WideWstring ping;
			int slot = FUN_005E98F0(player);
			if (slot != -1) {
				NodeNameType *entry = (NodeNameType *)FUN_005E9B20(window, slot, -1);
				if (entry && FUN_005E93F0(entry)) {
					ping = FUN_0078F5C0(entry->Name);
					request->Text = ping;
				}
			}
		}
	tooltip_done:
		return 1;
	}
	case 0x4ed:
		ShowWindow(window, 0);
		OwnerDraw::SetUserData2(window, 1);
	host_unhandled:
		return 0;
	case WM_MOVE:
	case 0x18:
		goto host_unhandled;
	case WM_ERASEBKGND:
		return 1;
	default:
		return 0;
	}
	return 0;
}

// FUNCTION: GAMEMD 0x00791B10
BOOL CALLBACK WOL_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int result = UI::StandardWndProc(window, message, wparam, lparam);
	if (result) return result;
	switch (message) {
	case WM_NCHITTEST: {
		HWND label = GetDlgItem(window, 0x695);
		if (!label) return FALSE;
		RECT rect;
		POINT point = {LOWORD(lparam), HIWORD(lparam)};
		GetWindowRect(window, &rect);
		point.x -= rect.left;
		point.y -= rect.top;
		WideWstring text;
		HWND control = ChildWindowFromPointEx(window, point, CWP_SKIPINVISIBLE);
		if (control) {
			text = FUN_00603F60(window, control);
			if (wcscmp(text.get(), L"") == 0) text = FUN_00604060(window, control);
		}
		SendMessage(label, 0x4B2, 0, (LPARAM)text.text());
		return FALSE;
	}

	case WM_DESTROY:
		if (Progress.Has_Dialog()) Progress.Clear_Dialog();
		if (MultiplayerMapPreview) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = NULL;
		}
		return FALSE;
	case WM_PAINT:
		OwnerDraw::Paint(window);
		FUN_00607FD0(window);
		Draw_Preview_Start_Points(window);
		if (Progress.Has_Dialog()) Progress.Display_Progress();
		ValidateRect(window, NULL);
		return FALSE;
	case WM_DRAWITEM:
		OwnerDraw::DrawItem((LPDRAWITEMSTRUCT)lparam);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case 0x3ED: {
			HWND edit = GetDlgItem(window, 0x540);
			WideWstring text = FUN_005EDD00(edit);
			SendMessage(edit, 0x4B2, 0, (LPARAM)DAT_00887734);
			SetFocus(edit);
			InvalidateRect(edit, NULL, FALSE);
			int color = DAT_007EEF38;
			if (strlen((char *)g_CurrentChannel.name) == 0) {
				PMessagePrintf(-1, StringTable::LoadString("TXT_NOT_IN_CHAN", NULL,
														   "D:\\ra2mdpost\\wonline.cpp", 17692));
			} else {
				if (text.length() > 0) {
					if (Send_Chat_Action(text)) color = DAT_007EEF34;
					wchar_t *message_text = text.get();
					PMessagePrintf(color, DAT_0084B224, g_NickName.c_str(), message_text);
				} else {
					PMessagePrintf(-1, StringTable::LoadString("TXT_ENTER_MESSAGE", NULL,
															   "D:\\ra2mdpost\\wonline.cpp", 17700));
				}
			}
			return FALSE;
		}
		case 0x540: {
			HWND edit = (HWND)lparam;
			WideWstring text = FUN_005EDD00(edit);
			if (HIWORD(wparam) == 0x501) {
				SendMessage(edit, 0x4B2, 0, (LPARAM)DAT_00887734);
				if (text.length() > 0) {
					int page = FUN_0078A6B0(text.get());
					if (page) {
						if (page == 1) {
							int count = 0;
							int length = text.length() + 1;
							wchar_t *copy = new wchar_t[length];
							wchar_t command[64];
							wcscpy(command, StringTable::LoadString("TXT_PAGE_COMMAND", NULL,
																	"D:\\ra2mdpost\\wonline.cpp", 17577));
							if (!copy) return FALSE;
							{
								wchar_t *start = wcsncpy(copy, text.get(), length);
								if (!start) return FALSE;
								{
									wchar_t *message_text = start + wcslen(command);
									while (*message_text && wcsncmp(message_text, L" ", 1) == 0)
										++message_text;
									wchar_t name[16];
									while (*message_text && wcsncmp(message_text, L" ", 1) != 0 &&
										   count <= 10) {
										name[count++] = *message_text++;
									}
									name[count] = 0;
									while (*message_text && wcsncmp(message_text, L" ", 1) == 0)
										++message_text;
									if (count > 10) {
										PMessagePrintf(
											-1, StringTable::LoadString("TXT_NAME_TOO_LONG", NULL,
																		"D:\\ra2mdpost\\wonline.cpp", 17607));
									} else if (!count) {
										PMessagePrintf(
											-1, StringTable::LoadString("TXT_NO_NAME_SPECIFIED", NULL,
																		"D:\\ra2mdpost\\wonline.cpp", 17609));
									} else if (wcslen(message_text) > 0) {
										PMessagePrintf(DAT_007EEF30, DAT_0084B9B0, g_NickName.c_str(), name,
													   message_text);
									} else {
										PMessagePrintf(
											-1, StringTable::LoadString("TXT_CANT_SEND_BLANK_PAGE", NULL,
																		"D:\\ra2mdpost\\wonline.cpp", 17615));
									}
									delete start;
								}
							}
						}
					} else if (FUN_0078A8F0(text.get())) {
						int length = text.length() + 1;
						wchar_t *copy = new wchar_t[length];
						wchar_t command[64], short_command[64];
						wcscpy(command, StringTable::LoadString("TXT_REPLY_COMMAND", NULL,
																"D:\\ra2mdpost\\wonline.cpp", 17633));
						wcscpy(short_command, StringTable::LoadString("TXT_REPLY_COMMAND_SHORT", NULL,
																	  "D:\\ra2mdpost\\wonline.cpp", 17634));
						if (!copy) return FALSE;
						{
							wchar_t *start = wcsncpy(copy, text.get(), length);
							if (!start) return FALSE;
							{
								int command_length;
								if (wcsncmp(start, command, wcslen(command)) == 0)
									command_length = wcslen(command);
								else
									command_length = wcslen(short_command);
								wchar_t *message_text = start + command_length;
								while (*message_text && wcsncmp(message_text, L" ", 1) == 0)
									++message_text;
								wchar_t name[16];
								wcscpy(name, FUN_00735120(DAT_00B77E30));
								if (wcslen(name) > 0 && wcslen(message_text) > 0) {
									PMessagePrintf(DAT_007EEF30, DAT_0084B9B0, g_NickName.c_str(), name,
												   message_text);
								} else if (wcslen(name) > 0) {
									PMessagePrintf(
										-1, StringTable::LoadString("TXT_CANT_SEND_BLANK_PAGE", NULL,
																	"D:\\ra2mdpost\\wonline.cpp", 17663));
								}
								delete start;
							}
						}
					} else if (!FUN_0078A470(text.get())) {
						int color = DAT_007EEF44;
						if (Send_Chat_Message(text) == 1) color = DAT_007EEF30;
						wchar_t *message_text = text.get();
						PMessagePrintf(color, DAT_0084B94C, g_NickName.c_str(), message_text);
					}
				}
			}
			return FALSE;
		}

		case IDCANCEL:
			if (!g_GameStarted && g_pChat->RequestChannelLeave() < 0) {
				FUN_007757E0(window, 2);
				FUN_0072CF90();
			}
			return FALSE;
		case 0x5C2:
			DoFindPage();
			return FALSE;
		case 0x59F: {
			char options[80];
			Session.Players[0]->Slot.Unknown77 = 1;
			sprintf(options, "A1");
			SendPublicGameopts(options);
			FUN_005EDDE0(window, false);
			HWND accept = GetDlgItem(window, 0x59F);
			EnableWindow(accept, FALSE);
			InvalidateRect(accept, NULL, FALSE);
			SendMessage(accept, 0x4DC, 0, 0);
			Draw_Player_List(0, false);
			SetFocus(GetDlgItem(window, 0x540));
			return FALSE;
		}
		case 0x510:
		case 0x513:
		case 0x514:
		case 0x51E:
		case 0x51F:
		case 0x520:
		case 0x521:
		case 0x522:
		case 0x523:
		case 0x524:
		case 0x525:
		case 0x526:
		case 0x527:
		case 0x528:
		case 0x6A1:
		case 0x6A2:
		case 0x6A3:
		case 0x6A4:
		case 0x6A5:
		case 0x6A6:
		case 0x6A7:
		case 0x6A8:
		case 0x6AA:
		case 0x6AB:
		case 0x76D:
		case 0x76E:
		case 0x76F:
		case 0x770:
		case 0x771:
		case 0x772:
		case 0x773:
		case 0x774:
			if (HIWORD(wparam) == CBN_SELCHANGE) {
				int country_slot = FUN_004E3830(LOWORD(wparam));
				int color_slot = FUN_004E4230(LOWORD(wparam));
				int start_slot = FUN_004E4EC0(LOWORD(wparam));
				int team_slot = FUN_004E59A0(LOWORD(wparam));
				int slot = country_slot > color_slot ? country_slot : color_slot;
				slot = slot > start_slot ? slot : start_slot;
				slot = slot > team_slot ? slot : team_slot;
				int country_id = FUN_004E37D0(slot);
				int color_id = FUN_004E41D0(slot);
				int start_id = FUN_004E4E60(slot);
				int team_id = FUN_004E5940(slot);
				if (slot == country_slot)
					FUN_005E9B60(window, country_id);
				else if (slot == color_slot)
					FUN_005E9C00(window, color_id);
				else if (slot == start_slot)
					FUN_005E9CE0(window, start_id);
				else
					FUN_005E9DB0(window, team_id);
				int country = FUN_004E4170(window, country_id, -1);
				int color = FUN_004E4E20(window, color_id, -1);
				int start = FUN_004E5900(window, start_id, -1);
				int team = FUN_004E6030(window, team_id, -1);
				NetNumType net;
				NetNodeType node;
				unsigned short port;
				Session.LocalAddress.Get_Address(net, node, &port);
				char options[128];
				sprintf(options, "R%d,%d,%d,%d,%X,%x,%x", country, color, start, team, *(int *)node,
						(unsigned short)FirewallHelper.Detected, Session.PortBase);
				FUN_004068E0("Sending private game options (%s)\n", options);
				Wstring host = FUN_007B7F90(Session.GameName);
				SendPrivateGameopts(host.get(), options);
			}
			return FALSE;
		}
		return FALSE;
	case 0x4ED:
		ShowWindow(window, SW_HIDE);
		OwnerDraw::SetUserData2(window, 1);
		return FALSE;
	case 0x4E9: {
		TooltipRequest *request = (TooltipRequest *)lparam;
		request->Text = (wchar_t const *)NULL;
		if (request->Window) {
			int control = GetDlgCtrlID(request->Window);
			if (request->Index != -1) {
				if (FUN_005E9950(control) == -1) {
					if (FUN_004E3830(control) == -1) {
						if (FUN_004E4230(control) != -1)
							request->Text = FUN_004E42A0(FUN_004E4E20(window, control, request->Index));
					} else {
						int country = FUN_004E4170(window, control, request->Index);
						if (country != -1) request->Text = FUN_004E38A0(country);
					}
				} else {
					NodeNameType *slot = (NodeNameType *)FUN_005E9B20(window, control, request->Index);
					request->Text = FUN_005E99C0(slot);
				}
				goto tooltip_done;
			}
			if (FUN_005E9950(control) != -1) {
				NodeNameType *slot = (NodeNameType *)FUN_005E9B20(window, control, -1);
				if (slot) {
					WideWstring text = FUN_00604060(window, request->Window);
					if (wcscmp(text.get(), StringTable::LoadString("STT:GuestComboPlayer", NULL,
																   "D:\\ra2mdpost\\wonline.cpp", 17364)) ==
							0 ||
						wcscmp(text.get(), StringTable::LoadString("STT:HostComboPlayer", NULL,
																   "D:\\ra2mdpost\\wonline.cpp", 17364)) ==
							0) {
						if (FUN_005E93F0(slot)) text = FUN_0078F200(slot->Name);
					}
					request->Text = text.get();
				}
			}
			{
				int slot_index = FUN_0078F6B0(control);
				if (slot_index != -1) {
					WideWstring text;
					int combo = FUN_005E98F0(slot_index);
					if (combo != -1) {
						NodeNameType *slot = (NodeNameType *)FUN_005E9B20(window, combo, -1);
						if (slot && FUN_005E93F0(slot)) {
							text = FUN_0078F5C0(slot->Name);
							request->Text = text;
						}
					}
				}
			}
		}

	tooltip_done:
		return TRUE;
	}
	case 0x497: {
		SetFocus(GetDlgItem(window, 0x540));
		SendDlgItemMessage(window, 0x3ED, 0x49C, 0, (LPARAM)SurfaceCache.GetSurface("wodact.pcx", NULL));
		SendDlgItemMessage(window, 0x3ED, 0x4AA, 0, (LPARAM)SurfaceCache.GetSurface("wouact.pcx", NULL));
		MultiplayerGameMode *mode = First_Game_Mode();
		Session.Options.MPModeIndex = mode->MPModeIndex;
		FUN_005E2EF0(window, mode);
		int scenario = mode->FirstValidMapIndex();
		Session.Options.ScenarioIndex = scenario;
		Set_Scenario_Info_From_Index(scenario);
		FUN_004E3B90(window);
		FUN_004E43C0();
		FUN_004E4820(window);
		FUN_004E4F50();
		FUN_004E5AC0();
		FUN_004E5D60(window);
		FUN_005EE510(-1);
		Set_Scenario_Info_From_Index(-1);
		DAT_00AC11C2 = false;
		Session.ScenarioMaxPlayers = 8;
		FUN_005E2F60(window);
		FUN_005E74E0(window);
		if (Session.MPGameMode && Session.MPGameMode->vt_entry_04() &&
			Scen->NumCoopHumanStartSpots <= RandomMapWaypointCount(scenario))
			FUN_004E4FC0(Scen->NumCoopHumanStartSpots);
		else
			FUN_004E4FC0(RandomMapWaypointCount(scenario));
		FUN_004E5310(window);
		FUN_004E5D60(window);
		FUN_005EE510(-1);
		DisplayGameopts(window, TRUE);
		Draw_Player_List(0, false);
		return FALSE;
	}
	}
	return FALSE;
}

void FUN_0072CF40(void);
int __fastcall FUN_005E9240(NodeNameType *node);
void FUN_005E2FA0(void);
void __fastcall FUN_007A2330(char *options);

static int Compare_Users(User *u1, User *u2)
{
	User *users[2];
	int value[2];
	users[0] = u1;
	users[1] = u2;
	value[0] = 0;
	value[1] = 0;
	for (int i = 0; i < 2; i++) {
		if (users[i]->flags & 1)
			value[i] += 4;
		if (users[i]->flags & 2)
			value[i] += 2;
	}
	if (value[0] < value[1]) {
		return 1;
	} else if (value[0] > value[1]) {
		return -1;
	}
	return stricmp((char *)u1->name, (char *)u2->name);
}

// FUNCTION: GAMEMD 0x007A5890
HRESULT __stdcall CChatEventSink::OnChannelJoin(HRESULT result, Channel *channel, User *user)
{
	if (result == 0x8004006C) {
		return 0;
	}
	int i;
	int players;
	wchar_t joinname[64];
	joinname[0] = 0;
	if (channel) {
		FUN_00735060(joinname, (char *)channel->name, -1);
		if (strncmp((char *)channel->name, "Lob_41_", 7) == 0) {
			int lobby = atol((char *)channel->name + strlen("Lob_41_"));
			int d = lobby / 8;
			int m = lobby % 8;
			if (d == 0) {
				swprintf(joinname, L"%s", g_Lobbies[m]);
			} else {
				swprintf(joinname, L"%s %d", g_Lobbies[m], d + 1);
			}
		}
	}
	if (result < 0) {
		if (result == 0x80040070) {
			Channel *chan = NULL;
			for (i = 0; i < g_UserChannels.length(); i++) {
				g_UserChannels.getPointer(&chan, i);
				if (strcmp((char *)s_TempChannel.name, (char *)chan->name) == 0) {
					g_UserChannels.remove(i);
					break;
				}
			}
			for (i = 0; i < g_GameChannelList.length(); i++) {
				g_GameChannelList.getPointer(&chan, i);
				if (strcmp((char *)s_TempChannel.name, (char *)chan->name) == 0) {
					g_GameChannelList.remove(i);
					break;
				}
			}
			if (Is_Internal_Session()) {
				PMessagePrintf(DAT_007EEF28, StringTable::LoadString("TXT_GAME_CLOSED", NULL,
																	 "D:\\ra2mdpost\\wonline.cpp", 0x1739));
				SetEvent(g_WaitEventHandles[5]);
			} else {
				ODMessageBox(
					StringTable::LoadString("TXT_GAME_CLOSED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x173C), 0,
					FUN_0077D710, false);
			}
		} else if (result == 0x80040071) {
			if (Is_Internal_Session()) {
				PMessagePrintf(DAT_007EEF28,
							   StringTable::LoadString("TXT_BADPASS", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1743));
				SetEvent(g_WaitEventHandles[5]);
			} else {
				ODMessageBox(StringTable::LoadString("TXT_BADPASS", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1746), 0,
							 FUN_0077D710, false);
			}
		} else if (result == 0x8004006E) {
			if (Is_Internal_Session()) {
				PMessagePrintf(DAT_007EEF28, StringTable::LoadString("TXT_CHANNEL_FULL", NULL,
																	 "D:\\ra2mdpost\\wonline.cpp", 0x174D));
				SetEvent(g_WaitEventHandles[5]);
			} else {
				ODMessageBox(
					StringTable::LoadString("TXT_CHANNEL_FULL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1750), 0,
					FUN_0077D710, false);
				if (s_TempChannel.type == 0) {
					SetEvent(g_WaitEventHandles[5]);
				}
			}
		} else if (result == 0x80040072) {
			ODMessageBox(StringTable::LoadString("TXT_JOINBAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1759), 0,
						 FUN_0077D710, false);
			SetEvent(g_WaitEventHandles[5]);
			g_CurrentLobby++;
		} else {
			PMessagePrintf(
				DAT_007EEF28,
				StringTable::LoadString("TXT_CANT_JOINCHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1760), result);
			SetEvent(g_WaitEventHandles[5]);
		}
		g_JoinLobbyNow = (s_TempChannel.type != 0);
		g_IsChannelCreator = 0;
		g_CurrentChannel.name[0] = 0;
		memset(&s_TempChannel, 0, sizeof(Channel));
		Close_Wait_Window(2);
		s_TempChannel.name[0] = 0;
		FUN_00781480();
		Draw_Player_List(0, false);
		SetEvent(g_WaitEventHandles[10]);
		HWND main = WS_Find_Dialog(0x113);
		if (main) {
			ShowWindow(main, SW_SHOW);
			OwnerDraw::SetUserData2(main, 0);
		}
		return 0;
	}

	s_TempChannel.name[0] = 0;
	if (user->flags & 0x8000) {
		memcpy(&g_CurrentChannel, channel, sizeof(Channel));
		memset(&s_TempChannel, 0, sizeof(Channel));
		Close_Wait_Window(2);
		g_ChannelCount++;
		FUN_004068E0(DAT_0084AD30);
		if (!Is_Internal_Session()) {
			PMessagePrintf(DAT_007EEF28,
						   StringTable::LoadString("TXT_JOINED_S", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1790),
						   joinname);
		}
		if (channel->type == 0) {
			g_CurrentLobby = atol((char *)channel->name + strlen("Lob_41_"));
			HWND main = WS_Find_Dialog(0x113);
			if (main) {
				EnableWindow(GetDlgItem(main, 0x53D), TRUE);
				bool join = false;
				bool observe = false;
				HWND list = GetDlgItem(main, 0x6EE);
				int index = SendMessage(list, LB_GETCURSEL, 0, 0);
				if (index != LB_ERR) {
					index = SendMessage(list, LB_GETITEMDATA, index, 0);
					if (index >= 0 && index < g_GameChannelList.length()) {
						Channel choice;
						memset(&choice, 0, sizeof(choice));
						g_GameChannelList.get(choice, index);
						join = FUN_0079BA60(&choice, false, false);
						observe = FUN_0079BA60(&choice, false, true);
					}
				}
				EnableWindow(GetDlgItem(main, 0x6EE), TRUE);
				EnableWindow(GetDlgItem(main, 0x62D), join || observe);
				EnableWindow(GetDlgItem(main, 0x62B), TRUE);
				EnableWindow(GetDlgItem(main, 0x689), TRUE);
				EnableWindow(GetDlgItem(main, 0x688), TRUE);
				EnableWindow(GetDlgItem(main, 0x7A3), TRUE);
				EnableWindow(GetDlgItem(main, 0x7A6), TRUE);
				EnableWindow(GetDlgItem(main, 0x61F), TRUE);
				EnableWindow(GetDlgItem(main, 0x5C2), TRUE);
				EnableWindow(GetDlgItem(main, 0x686), TRUE);
				SetFocus(GetDlgItem(main, 0x540));
			}
		}
		if (channel->type == 41) {
			Session.MPGameMode = First_Game_Mode();
			Session.ScenarioMaxPlayers = 8;
			DAT_008316A4 = 7;
			if (Session.MPGameMode) {
				Session.MPGameMode->vt_entry_24();
			}
			FUN_005C6120(&Session.Players);
			FUN_005E8B40();
			FUN_005E8B60();
			NodeNameType *node = new NodeNameType;
			memset(node, 0, sizeof(NodeNameType));
			wcscpy(node->Name, FUN_00735120((char *)user->name));
			node->Address = FUN_0077D720(user);
			Set_Initial_Player_Options(node);
			if (!Is_Internal_Session() && Game::ObserverMode) {
				node->Set_Country(-3);
				node->Slot.SpectatorFlag = -1;
			}
			Session.Players.Add(node);
			Session.GameName[0] = 0;
			if (!Is_Internal_Session()) {
				FUN_0052FEC0(NULL, true);
				FUN_0072CF40();
				HWND dlg = WS_Create_Dialog(ProgramInstance, 0xC9, MainWindow, WOL_Guest_Dialog_Proc, FALSE);
				UI::RegisterWindow(dlg, 0xC9);
				HWND main = WS_Find_Dialog(0x113);
				if (main) {
					ShowWindow(main, SW_HIDE);
					OwnerDraw::SetUserData2(main, 0);
				}
				SendMessage(dlg, 0x4A9, 0, 1);
				ShowWindow(dlg, SW_NORMAL);
				memset(DAT_00B76440, -1, 8 * sizeof(int));
				if (user->squadID != 0 && strlen((char *)user->squadabbrev) == 0) {
					g_SquadInfoRequests++;
					g_pChat->RequestSquadInfo(user->squadID);
				}
			}
		}
	} else {
		if (g_CurrentChannel.type == 41) {
			VocClass::PlayGlobal(Rule->PlayerJoined, 1.0f, 0x2000, 0);
		}
		if (channel->type != 0 && (g_GameStartRequested || g_GameStarted != 0 || DAT_00B77E27)) {
			g_pChat->RequestUserKick(user);
			return 0;
		}
		if (user->squadID != 0 && strlen((char *)user->squadabbrev) == 0) {
			g_SquadInfoRequests++;
			g_pChat->RequestSquadInfo(user->squadID);
		}
		{
			Wstring ladder_name;
			ladder_name.set((char *)user->name);
			ladder_name.toLower();
			if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
				Wstring name;
				bool found = false;
				for (i = 0; i < g_WDTTeamLookupQueue.length(); i++) {
					g_WDTTeamLookupQueue.get(name, i);
					if (name == ladder_name) {
						found = true;
						break;
					}
				}
				if (!found) {
					g_WDTTeamLookupQueue.addHead(ladder_name);
				}
			}
			if (!g_UserLocales.contains(ladder_name)) {
				Wstring name;
				bool found = false;
				for (i = 0; i < g_UserLocaleLookupQueue.length(); i++) {
					g_UserLocaleLookupQueue.get(name, i);
					if (name == ladder_name) {
						found = true;
						break;
					}
				}
				if (!found) {
					g_UserLocaleLookupQueue.addHead(ladder_name);
				}
			}
			if (!g_ActiveLadder->contains(ladder_name)) {
				g_pNetUtil->RequestLadderList(g_LadderServerHost, g_LadderServerPort, ladder_name.get(),
											  g_GameSKU & 0xFFFF00, -1, 0, 0);
			}
			if (!DAT_00B76640.contains(ladder_name)) {
				g_pNetUtil->RequestHighscore(g_LadderServerHost, g_LadderServerPort, ladder_name.get(),
											 g_GameSKU & 0xFFFF00);
			}
		}
		if (channel->type != 0) {
			for (i = 1; i < sizeof(g_UserInfo) / sizeof(ChannelUserInfo); i++) {
				g_UserInfo[i].Accepted = 0;
				EnableWindow(GetDlgItem(FUN_005E26C0(), 0x59F), TRUE);
			}
			if (!g_IsChannelCreator) {
				HWND dialog = WS_Find_Dialog(0xC9);
				FUN_005EDDE0(dialog, true);
			}
			FUN_005E9850();
		}
	}

	User newuser = *user;
	newuser.next = NULL;
	if (channel->type == 0) {
		FUN_00783070(&newuser);
	} else {
		int index = g_UserList.length();
		User temp_user;
		for (i = 0; i < g_UserList.length(); i++) {
			g_UserList.get(temp_user, i);
			if (Compare_Users(user, &temp_user) < 0) {
				index = i;
				break;
			}
		}
		g_UserList.add(newuser, index);
	}
	if (!(user->flags & 0x8000) && channel->type == 0 && Is_Internal_Session()) {
		Wstring name((char *)user->name);
		DAT_00B76AD0.addTail(name);
		if (DAT_00B76970.length() == 0) {
			FUN_007A1060();
		}
	}
	players = g_UserList.length();
	int userinfoindex = 0;
	if (channel->type != 0) {
		{
			Wstring name((char *)user->name);
			userinfoindex = Player_Name_To_Index(name);
		}
		memmove(&g_UserInfo[userinfoindex + 1], g_UserInfo + userinfoindex,
				sizeof(ChannelUserInfo) * (15 - userinfoindex));
		memset(&g_UserInfo[userinfoindex], 0, sizeof(ChannelUserInfo));
		strcpy(g_UserInfo[userinfoindex].Name, (char *)user->name);
		if (g_IsChannelCreator == 1) {
			if (!Is_Internal_Session()) {
				FUN_0078F000();
			}
			if (FUN_005E9510() && FUN_005E9560()) {
				Wstring name((char *)user->name);
				FUN_007A2830(&name);
			} else {
				int color = 0;
				int taken = 0;
				while (true) {
					g_UserInfo[userinfoindex].Color = color;
					taken = 0;
					for (i = 0; i < players; i++) {
						if (i != userinfoindex && g_UserInfo[i].Color == color) {
							color++;
							taken = 1;
						}
					}
					if (taken == 0) {
						break;
					}
				}
				NodeNameType *node = new NodeNameType;
				memset(node, 0, sizeof(NodeNameType));
				wcscpy(node->Name, FUN_00735120((char *)user->name));
				node->Address = FUN_0077D720(user);
				if (Session.MPGameMode) {
					node->Set_Country(Session.MPGameMode->vt_entry_28());
				} else {
					node->Set_Country(-2);
				}
				node->Set_Color(-2);
				node->Set_Start(-2);
				node->Set_Team(-2);
				if (!Is_Internal_Session() && FUN_005E9510()) {
					node->Set_Country(-3);
					node->Set_Color(-2);
					node->Slot.SpectatorFlag = -1;
				}
				if ((user->flags & 0x8000) && !Is_Internal_Session() && Game::ObserverMode) {
					node->Set_Country(-3);
					node->Set_Color(-2);
					node->Slot.SpectatorFlag = -1;
				}
				Session.Players.Add(node);
				FUN_005E9240(node);
				if (Is_Internal_Session() && channel->type != 0) {
					if (Session.WOLWelcomeChoice != 3) {
						DecodePubGameopt(DAT_00B73714, NULL);
					} else {
						FUN_007A2330(strchr(DAT_00B73714, ':'));
					}
					for (i = 0; i < Session.Scenarios.Count(); i++) {
						char const *filename = Session.Scenarios[i]->Get_Filename();
						if (stricmp(Session.ScenarioFileName, filename) == 0) {
							Set_Scenario_Info_From_Index(i);
							break;
						}
					}
					if (i >= Session.Scenarios.Count()) {
						Set_Scenario_Info_From_Index(0);
					}
				}
				FUN_005E9850();
				FUN_005E2FA0();
				char options[3];
				options[0] = 'O';
				options[1] = (DAT_008316A4 < 0 ? 8 : DAT_008316A4) + '0';
				options[2] = 0;
				FUN_004068E0(DAT_0084AD00, options[1]);
				g_pChat->RequestPublicGameOptions(options);
				FUN_0077DB90();
				PumpGameopts(true, false);
				if (FUN_005E8760()) {
					FUN_007A0F40();
					FUN_007A0D10();
				}
			}
		} else if (!(user->flags & 0x8000)) {
			NodeNameType *node = new NodeNameType;
			memset(node, 0, sizeof(NodeNameType));
			wcscpy(node->Name, FUN_00735120((char *)user->name));
			node->Address = FUN_0077D720(user);
			Set_Initial_Player_Options(node);
			Session.Players.Add(node);
			HWND dialog = WS_Find_Dialog(0xC9);
			FUN_005EDDE0(dialog, true);
			FUN_005E9850();
		}
	}
	SetEvent(g_WaitEventHandles[4]);
	Draw_Player_List(0, false);
	return 0;
}

// SYNTHETIC: GAMEMD 0x007AC630
// ??_GCreateGameDialogControl@@UAEPAXI@Z

bool __fastcall FUN_00789B60(void)
{
	Wstring password;
	if (Options.ScreenWidth >= 1024) Session.WOLLimitResolution = false;
	CreateGameDialogControl control(Session.WOLLimitResolution, Game::ObserverMode == 1);
	g_GameStartRequested = false;
	if (Is_Internal_Session()) {
		g_MaxPlayers = 2;
	} else {
		FUN_0052FEC0(NULL, true);
		if (Run_Dialog_Control(0xC3, WOL_New_Game_Dialog_Proc, &control) == IDCANCEL) return false;
		password = control.Text;
		password.replace(" ", "_");
	}
	g_pChat->RequestChannelLeave();
	Channel channel;
	memset(&channel, 0, sizeof(channel));
	sprintf((char *)channel.name, "%s's_game", g_NickName.c_str());
	channel.type = 41;
	channel.minUsers = 1;
	if (g_MaxPlayers < 2) g_MaxPlayers = 6;
	strcpy((char *)channel.key, password.get());
	channel.maxUsers = g_MaxPlayers;
	char user_options[80];
	memset(user_options, 0, sizeof(user_options));
	Encode_Channel_ExInfo(user_options);
	user_options[40] = 0;
	strcpy((char *)channel.exInfo, user_options);
	if (Session.Type == GAME_INTERNET && Session.WOLWelcomeChoice == 3) {
		channel.tournament = 1;
		channel.reserved |= 0x1000;
	} else if (Is_Internal_Session()) {
		if (Session.WOLWelcomeChoice == 2) channel.tournament = 0;
		else channel.tournament = 1;
		channel.reserved |= 0x800;
	} else {
		channel.tournament = control.Flag10;
		if (control.Flag11) channel.reserved |= 0x100;
		Session.WOLLimitResolution = control.Flag12;
		if (control.Flag12) channel.reserved |= Options.ScreenWidth == 800 ? 0x400 : 0x200;
	}
	channel.reserved += g_CurrentLobby;
	Leave_Channel(0xC0, false);
	MultiplayerGameMode * mode = First_Game_Mode();
	Session.MPGameMode = mode;
	Session.Options.MPModeIndex = mode->MPModeIndex;
	mode->vt_entry_20();
	Set_Scenario_Info_From_Index(0);
	FUN_005C6120(&Session.Players);
	FUN_005E8B40();
	FUN_005E8B60();
	NodeNameType * who = new NodeNameType;
	memset(who, 0, sizeof(NodeNameType));
	WideWstring name = FUN_007B7ED0(g_NickName);
	wcscpy(who->Name, name.get());
	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) profile = FUN_00778A20(&g_NickName);
	else profile = DAT_00B75E48;
	if (profile.Get()) {
		Session.FUN_0069B760(profile.Get()->Country, false);
		Session.FUN_0069B7E0(profile.Get()->Field20, false);
	}
	Session.Start = -1;
	Session.InitialStart = -2;
	who->Set_Country(Session.FUN_0069B700());
	who->Set_Color(Session.FUN_0069B720());
	who->Set_Start(-2);
	who->Set_Team(-2);
	who->Slot.Unknown77 = 1;
	who->Slot.SpectatorFlag = 0;
	DAT_008316A4 = 7;
	Game::ObserverMode = control.Flag13;
	FUN_005EE350(0);
	if (Game::ObserverMode) {
		DAT_008316A4 = 0;
		who->Slot.SpectatorFlag = -1;
		who->Set_Country(-3);
	}
	Session.Players.Add(who);
	FUN_005E8DA0(0);
	wcscpy(Session.GameName, name.get());
	HRESULT result = g_pChat->RequestChannelCreate(&channel);
	if (result == 0x800401F8) {
		ODMessageBox(StringTable::LoadString("TXT_BAD_CHANKEY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E0D), 0, FUN_0077D710, false);
		FUN_00792EC0(0x80);
		return false;
	}
	if (result != 0) {
		ODMessageBox(StringTable::LoadString("TXT_CANTCREATE_CHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E13), 0, FUN_0077D710, false);
		FUN_00792EC0(0x80);
		return false;
	}
	WideWstring title(StringTable::LoadString("GUI:CreatingChannel", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2E1B));
	Show_Wait_Window(4, title, false);
	FUN_00792EC0(0x80);
	Show_Wait_Window(4, title, true);
	int index = Player_Name_To_Index(g_NickName);
	if (index < 0) return false;
	User * user = NULL;
	g_UserList.getPointer(&user, index);
	{
		NetNumType net;
		NetNodeType node;
		memset(net, 0, sizeof(net));
		memset(node, 0, sizeof(node));
		unsigned int ip_addr = ntohl(user->ipaddr);
		node[3] = ip_addr & 0xFF;
		node[2] = (ip_addr >> 8) & 0xFF;
		node[1] = (ip_addr >> 16) & 0xFF;
		node[0] = (ip_addr >> 24) & 0xFF;
		IPXAddressClass address(net, node, 0);
		who->Address = address;
	}
	if (!Is_Internal_Session()) {
		HWND window = WS_Top_Window();
		if (window) {
			OwnerDraw::DrawCampaignMenuTransition(window, false);
			FUN_0052FEC0(NULL, true);
		}
		FUN_0052FEC0(NULL, true);
		FUN_0072CF40();
		HWND dialog = UI::BeginDialog(0xC2, WOL_Game_Options_Proc, 0);
		Session.MPGameMode->vt_entry_9C();
		Session.MPGameMode->vt_entry_20();
		Session.MPGameMode->vt_entry_18();
		Session.MPGameMode->UnfixAlliances();
		ShowWindow(dialog, SW_SHOWNORMAL);
		if (window) ShowWindow(window, SW_HIDE);
	}
	return true;
}

Wstring __fastcall FUN_007B82E0(WideWstring const & text);

// FUNCTION: GAMEMD 0x0077E430
void Encode_Game_Options(char * out)
{
	static char player_options[512];
	wchar_t const * description = Session.Options.ScenarioDescription;
	Wstring name = description ? FUN_007B82E0(description) : "";
	int value = Session.ScenarioMaxPlayers;
	sprintf(out, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%d,%d,%d,%s,%s:",
		Game::Seed, Options.GameSpeed, Session.Options.GameSpeed, Session.Options.Credits,
		Session.Options.UnitCount, Session.Options.ShortGame, Session.Options.SWAllowed,
		Session.Options.BuildOffAlly, Session.Options.MCVRedeploy, Session.Options.Goodies,
		Session.Options.MPModeIndex, Options.CampDifficulty, name.get(), value,
		Session.ScenarioIsOfficial, Session.ScenarioFileLength, Session.ScenarioFileName, Session.ScenarioDigest);
	memset(player_options, 0, sizeof(player_options));
	for (int i = 0, count = Session.Players.Count(); i < count; i++) {
		NodeNameType * entry = Session.Players[i];
		int country = entry->Get_Country();
		int color = entry->Get_Color();
		int start = entry->Get_Start();
		int team = entry->Get_Team();
		int address = 0;
		int firewall = 0;
		int port_base = 0;
		for (int j = 0; j < 8; j++) {
			const wchar_t * slot_name = Session.Slots[j]->Name;
			char * player_name = FUN_007350C0(Session.Players[i]->Name);
			if (stricmp(player_name, FUN_007350C0(slot_name)) == 0) {
				NetNumType net;
				NetNodeType node;
				unsigned short socket;
				Session.PlayerLocalAddress[j].Get_Address(net, node, &socket);
				memcpy(&address, node, sizeof(address));
				firewall = Session.PlayerFirewall[j];
				port_base = Session.PlayerPortBase[j];
				break;
			}
		}
		Wstring text(FUN_007B7F90(Session.Players[i]->Name));
		sprintf(player_options + strlen(player_options), "%s,%d,%d,%d,%d,%x,%x,%x,",
			text.get(), country, color, start, team, address, firewall, port_base);
	}
	player_options[strlen(player_options) - 1] = ':';
	strcat(player_options, "@:");
	for (int k = 0; k < 8; k++) {
		sprintf(player_options + strlen(player_options), "%d,%d,%d,%d,%d,",
			Session.Options.AISlots.Difficulties[k], Session.Options.AISlots.Countries[k],
			Session.Options.AISlots.Colors[k], Session.Options.AISlots.Starts[k], Session.Options.AISlots.Allies[k]);
	}
	strcat(out, player_options);
}

// FUNCTION: GAMEMD 0x007A2330
void __fastcall FUN_007A2330(char * options) {
	char buffer[256];
	strcpy(buffer, options);
	char * name = strtok(buffer, ":,");
	char * country_text = strtok(NULL, ",");
	int country = atol(country_text);
	char * color_text = strtok(NULL, ",");
	int color = atol(color_text);
	char * ip = strtok(NULL, ",");
	char * second = strtok(NULL, ",");
	char * third = strtok(NULL, ",");
	FUN_007A2230(name, ip, (unsigned long)second, (unsigned long)third);
	Assign_House_And_Color(name, country, color, -2, -2);
	char * name2 = strtok(NULL, ",");
	char * country2_text = strtok(NULL, ",");
	int country2 = atol(country2_text);
	char * color2_text = strtok(NULL, ",");
	int color2 = atol(color2_text);
	char * ip2 = strtok(NULL, ",");
	char * second2 = strtok(NULL, ",");
	char * third2 = strtok(NULL, ",");
	FUN_007A2230(name2, ip2, (unsigned long)second2, (unsigned long)third2);
	Assign_House_And_Color(name2, country2, color2, -2, -2);
	RandomMapGen.SeedData.NumPlayers = 4;
	strcpy(Session.ScenarioFileName, "RandMap.Sed");
	Session.Options.ScenarioIndex = -1;
	HWND dialog = WS_Find_Dialog(0x11F);
	Do_Random_Map(dialog, FUN_0077D710);
	RandomMapGen.SeedData.Save("RandMap.Sed");
	bool found = false;
	int index;
	for (index = 0; index < Session.Scenarios.Count(); index++) {
		if (Session.Scenarios[index]->Is_Random_Map()) {
			found = true;
			break;
		}
	}
	char * digest = CalcRandomMapDigest();
	if (!found) {
		MultiMission * map = new MultiMission("RandMap.Sed", RandomMapGen.SeedData.MapDescription, digest, true, NULL, 2, 4);
		Session.Scenarios.Add(map);
	} else {
		Session.Scenarios[index]->Set_Digest(digest);
		Session.Scenarios[index]->Set_Description(RandomMapGen.SeedData.MapDescription);
	}
	delete digest;
	if (Set_Scenario_Info_From_Index(index) == true) {
		Session.Options.ScenarioIndex = index;
	}
}
