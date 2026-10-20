// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG

#include "always.h"
#include "arraylist.h"
#include "dict.h"
#include "session.h"
#include "timer.h"
#include "wstring.h"
#include "widewstring.h"
#include "ipxaddr.h"
#include "ccini.h"
#include "rawfile.h"
#include "theme.h"

#include <windows.h>
#undef LoadString
#include <ole2.h>
#include <shellapi.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "stringtable.h"

HWND WS_Find_Dialog(int id);

unsigned int __fastcall Wstring_Hash(Wstring & string);
#define HASH_ARG (Wstring_Hash)

class OLEInitializer
{
public:
	OLEInitializer(void) { OleInitialize(NULL); }
	~OLEInitializer(void) { OleUninitialize(); }
};

// FUNCTION: 0x0077B8A0
// FUNCTION: 0x0077B8C0
OLEInitializer g_OLEInitializer;

// FUNCTION: 0x0077B8D0
// FUNCTION: 0x0077B8F0
Wstring DAT_00B73AE0;

// FUNCTION: 0x0077B900
// FUNCTION: 0x0077B920
ArrayList<int> DAT_00B778D8;

// FUNCTION: 0x0077B940
// FUNCTION: 0x0077B960
ArrayList<int> DAT_00B73E10;

// FUNCTION: 0x0077B980
// FUNCTION: 0x0077B9A0
ArrayList<int> DAT_00B76AE0;

struct User {
	char Pad[0x20];
	User * Next;
	char Nick[10];
	char Login[0x6C - 0x2E];
};

// FUNCTION: 0x0077B9C0
// FUNCTION: 0x0077B9E0
ArrayList<User> DAT_00B77900;

// FUNCTION: 0x0077BA00
// FUNCTION: 0x0077BA20
ArrayList<int> DAT_00B76520;

struct LadderA {
	char Data[0x6C];
};

struct LadderB {
	char Data[0x44];
};

// FUNCTION: 0x0077BA40
// FUNCTION: 0x0077BAE0
Dictionary<Wstring, LadderA> DAT_00B768C0 HASH_ARG;

// FUNCTION: 0x0077BC10
// FUNCTION: 0x0077BCB0
Dictionary<Wstring, LadderA> DAT_00B73AF8 HASH_ARG;

// FUNCTION: 0x0077BDE0
// FUNCTION: 0x0077BE80
Dictionary<Wstring, LadderA> DAT_00B77D80 HASH_ARG;

// FUNCTION: 0x0077BFB0
// FUNCTION: 0x0077C050
Dictionary<Wstring, LadderB> DAT_00B76640 HASH_ARG;

// FUNCTION: 0x0077C180
// FUNCTION: 0x0077C220
Dictionary<Wstring, int> DAT_00B76680 HASH_ARG;

// FUNCTION: 0x0077C350
// FUNCTION: 0x0077C370
ArrayList<Wstring> DAT_00B777A0;

// FUNCTION: 0x0077C3B0
// FUNCTION: 0x0077C450
Dictionary<Wstring, int> DAT_00B76530 HASH_ARG;

// FUNCTION: 0x0077C580
// FUNCTION: 0x0077C5A0
ArrayList<Wstring> DAT_00B765F8;

// FUNCTION: 0x0077C5E0
// FUNCTION: 0x0077C600
ArrayList<Wstring> DAT_00B73BD8;

// FUNCTION: 0x0077C640
// FUNCTION: 0x0077C660
ArrayList<Wstring> DAT_00B736F8;

// FUNCTION: 0x0077C6A0
// FUNCTION: 0x0077C6C0
ArrayList<Wstring> DAT_00B76B00;

// FUNCTION: 0x0077C700
// FUNCTION: 0x0077C720
ArrayList<Wstring> DAT_00B76510;

// FUNCTION: 0x0077C760
// FUNCTION: 0x0077C780
ArrayList<Wstring> DAT_00B73AE8;

// FUNCTION: 0x0077C7C0
// FUNCTION: 0x0077C7E0
ArrayList<Wstring> DAT_00B77820;

// FUNCTION: 0x0077C820
// FUNCTION: 0x0077C840
ArrayList<Wstring> DAT_00B73708;

// FUNCTION: 0x0077C880
// FUNCTION: 0x0077C8A0
ArrayList<Wstring> DAT_00B76188;

// FUNCTION: 0x0077C8E0
// FUNCTION: 0x0077C900
ArrayList<Wstring> DAT_00B73AA8;

// FUNCTION: 0x0077C940
// FUNCTION: 0x0077C960
ArrayList<Wstring> DAT_00B76980;

// FUNCTION: 0x0077C9A0
// FUNCTION: 0x0077C9C0
ArrayList<Wstring> DAT_00B779B0;

// FUNCTION: 0x0077CA00
// FUNCTION: 0x0077CA20
Wstring DAT_00B73ACC;

// FUNCTION: 0x0077CA30
// FUNCTION: 0x0077CA50
Wstring DAT_00B76E24;

// FUNCTION: 0x0077CA60
// FUNCTION: 0x0077CB00
Dictionary<Wstring, int> DAT_00B76990 HASH_ARG;

struct LadderC {
	char Data[0x10C];
};

// FUNCTION: 0x0077CC30
// FUNCTION: 0x0077CCD0
Dictionary<Wstring, LadderC> DAT_00B76608 HASH_ARG;

extern int DAT_0084A1C4;
extern int DAT_0084A1C8;
extern int DAT_0084A1D4;
extern int DAT_0084A1D8;

// FUNCTION: 0x0077CE10
int DAT_00B765F0 = DAT_0084A1C8 - DAT_0084A1C4 - 4;

// FUNCTION: 0x0077CE30
int DAT_00B73E3C = DAT_0084A1D8 - DAT_0084A1D4 - 4;

// FUNCTION: 0x0077CE50
// FUNCTION: 0x0077CE70
ArrayList<Wstring> DAT_00B76AD0;

// FUNCTION: 0x0077CEB0
// FUNCTION: 0x0077CED0
ArrayList<Wstring> DAT_00B76970;

// FUNCTION: 0x0077CF10
// FUNCTION: 0x0077CF30
Wstring DAT_00B76AC4;

// FUNCTION: 0x0077CF40
int FUN_0077CF40(void)
{
	int t = Session.unknown_0C;
	if (t == 3 || t == 1 || t == 2)
		return 1;
	return 0;
}

// FUNCTION: 0x0077CF60
int FUN_0077CF60(void)
{
	return Session.unknown_0C == 2;
}

// FUNCTION: 0x0077CF70
// FUNCTION: 0x0077CF90
Wstring DAT_00B76E34;


// FUNCTION: 0x0077CFA0
// FUNCTION: 0x0077CFC0
Wstring DAT_00B73B30;

// FUNCTION: 0x0077CFD0
// FUNCTION: 0x0077CFF0
Wstring DAT_00B7622C;

// FUNCTION: 0x0077D000
// FUNCTION: 0x0077D020
Wstring DAT_00B76908;

// FUNCTION: 0x0077D030
// FUNCTION: 0x0077D050
Wstring DAT_00B77794;

// FUNCTION: 0x0077D060
// FUNCTION: 0x0077D080
Wstring DAT_00B778F4;

// FUNCTION: 0x0077D090
// FUNCTION: 0x0077D0B0
Wstring DAT_00B778FC;

// FUNCTION: 0x0077D0C0
// FUNCTION: 0x0077D0E0
Wstring DAT_00B765EC;

// FUNCTION: 0x0077D0F0
// FUNCTION: 0x0077D110
Wstring DAT_00B76900;

class FUN_007F094C_Class {
public:
	FUN_007F094C_Class(void);
	virtual ~FUN_007F094C_Class(void);
	void * Ptr;
};

class FUN_007F3F44_Class : public FUN_007F094C_Class {
public:
	FUN_007F3F44_Class(void) {}
	virtual ~FUN_007F3F44_Class(void) {}
};

// FUNCTION: 0x0077D120
// FUNCTION: 0x0077D140
FUN_007F3F44_Class DAT_00B75E48;

// FUNCTION: 0x0077D160
// FUNCTION: 0x0077D190
CDTimerClass<SystemTimerClass> DAT_00B76478;

// FUNCTION: 0x0077D1A0
// FUNCTION: 0x0077D1C0
WideWstring DAT_00B76230;

class FUN_007EF720_Class {
public:
	virtual ~FUN_007EF720_Class(void) {}
};

class FUN_007EF738_Class : public FUN_007EF720_Class {
public:
	FUN_007EF738_Class(void);
	virtual ~FUN_007EF738_Class(void);
	int A;
	char B;
};

class FUN_007F7624_Class : public FUN_007EF738_Class {
public:
	FUN_007F7624_Class(void);
	virtual ~FUN_007F7624_Class(void);
};

// FUNCTION: 0x0077D1D0
FUN_007F7624_Class::FUN_007F7624_Class(void)
{
}

// FUNCTION: 0x0077D1F0
FUN_007F7624_Class::~FUN_007F7624_Class(void)
{
}

void FUN_0077F900(void);

// FUNCTION: 0x0077D200
void FUN_0077D200(void)
{
	FUN_0077F900();
}

// FUNCTION: 0x0077D210
void __fastcall FUN_0077D210(char * text)
{
	char * p = strchr(text, ' ');
	while (p) {
		*p = '_';
		p = strchr(p, ' ');
	}
}

// FUNCTION: 0x0077D240
int __fastcall FUN_0077D240(Wstring * name)
{
	User * user = NULL;
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		DAT_00B77900.getPointer(&user, i);
		if (strcmpi(name->text(), user->Nick) == 0)
			return i;
	}
	return -1;
}

extern int DAT_00B779C0;
extern int DAT_00B779DC;

// FUNCTION: 0x0077D2A0
bool __fastcall FUN_0077D2A0(User * user)
{
	if (!user)
		return false;
	if (user->Pad[0] & 1)
		return true;
	if (WS_Find_Dialog(0xC2))
		return true;
	if (WS_Find_Dialog(0xC9))
		return true;
	return DAT_00B779C0 != 0;
}

unsigned char __fastcall FUN_007B7D90(Wstring * text);

// FUNCTION: 0x0077D2E0
bool __fastcall FUN_0077D2E0(User * user, Wstring * text)
{
	if (!user)
		return false;
	if (user->Pad[0] & 1)
		return true;
	if (WS_Find_Dialog(0xC2))
		return true;
	if (WS_Find_Dialog(0xC9))
		return true;
	if (FUN_007B7D90(text))
		return DAT_00B779C0 != 0;
	return DAT_00B779DC != 0;
}

// FUNCTION: 0x0077D350
int __fastcall FUN_0077D350(wchar_t const * text)
{
	wchar_t buffer[256];
	swprintf(buffer, StringTable::LoadString("TXT_BUDDY_FILTER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x369));
	return wcsnicmp(text, buffer, wcslen(buffer)) == 0;
}

// FUNCTION: 0x0077D3A0
int __fastcall FUN_0077D3A0(wchar_t const * text)
{
	wchar_t buffer[256];
	swprintf(buffer, StringTable::LoadString("TXT_CLAN_FILTER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x36F));
	return wcsnicmp(text, buffer, wcslen(buffer)) == 0;
}

// FUNCTION: 0x0077D3F0
bool __fastcall FUN_0077D3F0(User * user)
{
	if (!user)
		return false;
	int count = DAT_00B73AA8.length();
	Wstring name;
	for (int i = 0; i < count; i++) {
		DAT_00B73AA8.get(name, i);
		if (strcmp(name.get(), user->Nick) == 0)
			return true;
	}
	return false;
}

// FUNCTION: 0x0077D490
bool __fastcall FUN_0077D490(User * a, User * b)
{
	if (!a)
		return false;
	if (!b)
		return false;
	if (strcmpi(a->Login, b->Login) != 0)
		return false;
	else
		return true;
}

extern bool DAT_00B76AF0;
extern bool DAT_00B73AD0;
extern Wstring DAT_00B73E40;

// FUNCTION: 0x0077D4C0
bool __fastcall FUN_0077D4C0(User * user)
{
	if (DAT_00B76AF0 || DAT_00B73AD0) {
		if (WS_Find_Dialog(0x113)) {
			int index = FUN_0077D240(&DAT_00B73E40);
			User * host = NULL;
			if (index >= 0) {
				DAT_00B77900.getPointer(&host, index);
				if (DAT_00B76AF0) {
					if (DAT_00B73AD0) {
						if (FUN_0077D490(host, user))
							return true;
						return FUN_0077D3F0(user);
					}
					return FUN_0077D3F0(user);
				}
				if (DAT_00B73AD0) {
					if (FUN_0077D490(host, user))
						return true;
				}
				return false;
			}
		}
	}
	return true;
}

// FUNCTION: 0x0077D710
void FUN_0077D710(void)
{
	FUN_0077F900();
}

struct HostEntry {
	char Pad[0x18];
	unsigned long Address;
};

unsigned long __stdcall FUN_007C896E(unsigned long value);

// FUNCTION: 0x0077D720
IPXAddressClass __fastcall FUN_0077D720(HostEntry * host)
{
	NetNumType netnum;
	NetNodeType net;
	memset(&netnum, 0, sizeof(netnum));
	memset(&net, 0, sizeof(net));
	unsigned int ip_addr = FUN_007C896E(host->Address);
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

extern bool UserInputLocked;
extern void * InputManagerInstance;
void __fastcall FUN_0054F720(void * input);
HWND FUN_005E26C0(void);
int __fastcall FUN_007759E0(HWND window, bool (*callback)(void), int a, int b, int c);
BOOL CALLBACK FUN_0078D120(HWND win, UINT msg, WPARAM wparam, LPARAM lparam);
bool FUN_0077D820(void);

namespace UI {
	HWND __fastcall BeginDialog(int id, DLGPROC proc, int extra);
	bool Updated(void);
}

// FUNCTION: 0x0077D7A0
void FUN_0077D7A0(bool play)
{
	if (play) {
		if (!Theme.Still_Playing()) {
			Theme.Stop();
			Theme.Queue_Song(THEME_PICK_ANOTHER);
			return;
		}
		if (Theme.What_Is_Playing() == Theme.From_Name("INTRO")) {
			Theme.Stop();
			Theme.Queue_Song(THEME_PICK_ANOTHER);
			return;
		}
	} else {
		Theme.Queue_Song(THEME_QUIET);
	}
}

// FUNCTION: 0x0077D820
bool FUN_0077D820(void)
{
	UI::Updated();
	HWND top = FUN_005E26C0();
	if (top) {
		InvalidateRect(top, NULL, TRUE);
	}
	return false;
}

// FUNCTION: 0x0077D840
void FUN_0077D840(void)
{
	HWND dialog = UI::BeginDialog(0xCF, (DLGPROC)FUN_0078D120, 0);
	UserInputLocked = true;
	FUN_0054F720(InputManagerInstance);
	ShowWindow(dialog, SW_SHOWNORMAL);
	if (FUN_005E26C0()) {
		FUN_007759E0(dialog, FUN_0077D820, 0, 1, 1);
	} else {
		FUN_007759E0(dialog, UI::Updated, 0, 1, 1);
	}
	FUN_0054F720(InputManagerInstance);
	UserInputLocked = Scen->IsInputLocked;
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
}

void __fastcall FUN_00773E90(const char * name);

// FUNCTION: 0x0077D8C0
void FUN_0077D8C0(void)
{
	FUN_00773E90("Battleclans");
}

// FUNCTION: 0x0077D8D0
void FUN_0077D8D0(void)
{
	FUN_00773E90("Battleclans");
}

// FUNCTION: 0x0077D8E0
void FUN_0077D8E0(void)
{
	FUN_00773E90("Ladder");
}

// FUNCTION: 0x0077D8F0
void FUN_0077D8F0(void)
{
	FUN_00773E90("News");
}

// FUNCTION: 0x0077D900
void FUN_0077D900(void)
{
	FUN_00773E90("Manage");
}

struct WolChannel {
	char Pad[0x14];
	int Tournament;
	char Pad2[8];
	unsigned int Reserved : 8;
	unsigned int Clan : 1;
	unsigned int Pad3 : 2;
	unsigned int FlagB : 1;
	unsigned int FlagC : 1;
	unsigned int Rest : 20;
};

extern WolChannel DAT_00B76B10;

// FUNCTION: 0x0077D910
bool __fastcall FUN_0077D910(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = false;
	return chan->Tournament || (r = chan->Clan) || r;
}

// FUNCTION: 0x0077D940
bool FUN_0077D940(void)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = false;
	if (DAT_00B76B10.Tournament || (r = DAT_00B76B10.Clan)) {
		return true;
	}
	return r;
}

// FUNCTION: 0x0077D970
bool FUN_0077D970(void)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = DAT_00B76B10.Clan;
	return r;
}

// FUNCTION: 0x0077D990
bool __fastcall FUN_0077D990(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return chan->Clan;
}

// FUNCTION: 0x0077D9B0
bool __fastcall FUN_0077D9B0(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return chan->FlagB;
}

// FUNCTION: 0x0077D9D0
bool __fastcall FUN_0077D9D0(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return chan->FlagC;
}

// FUNCTION: 0x0077D9F0
// FUNCTION: 0x0077DA10
Wstring DAT_00B73E40;

// FUNCTION: 0x0077DA20
// FUNCTION: 0x0077DA40
Wstring DAT_00B7778C;

extern int DAT_00B779C4;
char * __fastcall FUN_007350C0(wchar_t const * text);

// FUNCTION: 0x0077DA50
LadderA * __fastcall FUN_0077DA50(wchar_t const * name)
{
	Wstring key(FUN_007350C0(name));
	key.toLower();
	LadderA * result = new LadderA;
	if (!result) {
		return NULL;
	}
	memset(result, 0, sizeof(LadderA));
	LadderA * found = NULL;
	if (DAT_00B779C4 != 2) {
		if (DAT_00B77D80.getPointer(key, &found) && found) {
			*result = *found;
		}
	} else {
		if (DAT_00B768C0.getPointer(key, &found) && found) {
			*result = *found;
		}
	}
	return result;
}

typedef NodeNameType NSSlotName;
extern NSSlotName * DAT_00A8DA90[8];
bool __fastcall FUN_005E93F0(NSSlotName * slot);
Wstring __fastcall FUN_005EDEF0(NSSlotName * slot);

struct ChatSink {
	virtual void V00(void);
	virtual void V01(void);
	virtual void V02(void);
	virtual void V03(void);
	virtual void V04(void);
	virtual void V05(void);
	virtual long __stdcall V06(int type, int ping);
	virtual void V07(void);
	virtual void V08(void);
	virtual void V09(void);
	virtual void V10(void);
	virtual void V11(void);
	virtual void V12(void);
	virtual void V13(void);
	virtual void V14(void);
	virtual long __stdcall V15(char * text);
	virtual void V16(void);
	virtual void V17(void);
	virtual long __stdcall V18(User * users);
	virtual void V19(void);
	virtual void V20(void);
	virtual void V21(void);
	virtual void V22(void);
	virtual void V23(void);
	virtual void V24(void);
	virtual void V25(void);
	virtual long __stdcall V26(int find, int page);
	virtual void V27(void);
	virtual void V28(void);
	virtual void V29(void);
	virtual void V30(void);
	virtual long __stdcall V31(int filter);
	virtual void V32(void);
	virtual void V33(void);
	virtual void V34(void);
	virtual void V35(void);
	virtual void V36(void);
	virtual void V37(void);
	virtual void V38(void);
	virtual void V39(void);
	virtual void V40(void);
	virtual void V41(void);
	virtual void V42(void);
	virtual void V43(void);
	virtual void V44(void);
	virtual void V45(void);
	virtual void V46(void);
	virtual void V47(void);
	virtual void V48(void);
	virtual void V49(void);
	virtual void V50(void);
	virtual void V51(void);
	virtual void V52(void);
	virtual void V53(void);
	virtual void V54(void);
	virtual void V55(void);
	virtual void V56(void);
	virtual void V57(void);
	virtual long __stdcall V58(wchar_t * text);
	virtual long __stdcall V59(User * users, wchar_t * text);
	virtual long __stdcall V60(wchar_t * text);
	virtual long __stdcall V61(User * users, wchar_t * text);
};
extern ChatSink * DAT_00AC116C;

// FUNCTION: 0x0077DB90
void FUN_0077DB90(void)
{
	char list[256];
	strcpy(list, "L");
	for (int i = 0; i < 8; i++) {
		NSSlotName * slot = DAT_00A8DA90[i];
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
	DAT_00AC116C->V15(list);
}

int ODMessageBox(const wchar_t * text, int type, void (*callback)(void), bool large);
extern char DAT_00A8ED80;
extern HWND DAT_00B73550;
int __cdecl FUN_007CD455(const char * path);

// FUNCTION: 0x0077DC90
void __fastcall FUN_0077DC90(const char * name, int no_ask)
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

				FUN_0077F900();

				WaitForInputIdle(process, 5000);

				while (!done_waiting) {
					FUN_0077F900();
					Sleep(500);

					DWORD exitCode;
					GetExitCodeProcess(process, &exitCode);
					if (exitCode != STILL_ACTIVE) {
						done_waiting = TRUE;
					}

					if (DAT_00A8ED80 || GetTopWindow(NULL) == DAT_00B73550 || GetForegroundWindow() == DAT_00B73550) {
						done_waiting = TRUE;
					}
					FUN_0077F900();
				}

				FUN_0077F900();

				if (GetTopWindow(NULL) != DAT_00B73550) {
					FUN_0077F900();
					SetForegroundWindow(DAT_00B73550);
					ShowWindow(DAT_00B73550, SW_RESTORE);
					FUN_0077F900();
				}

				int waits = 10;
				while (waits) {
					Sleep(100);
					FUN_0077F900();
					waits--;
				};
			}
			return;
		}
	} else if (result <= 32) {
		ODMessageBox(StringTable::LoadString("TXT_NOBROWSER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5BA), 0, FUN_0077D710, 0);
	}
}

extern CCINIClass DAT_008870C0;
extern int DAT_00B76234;
extern int DAT_00B779F0;
extern int DAT_00B779BC;
extern int DAT_00B7690C;
extern int DAT_00B77784;
extern int DAT_00B779DC;
extern int DAT_00B779C0;
void FUN_005EE190(void);
void FUN_005EE1B0(void);
void __fastcall FUN_005EE350(int arg);

// FUNCTION: 0x0077DED0
void FUN_0077DED0(void)
{
	DAT_00B76234 = DAT_008870C0.Get_Int("WOnline", "AllowPage", 1);
	DAT_00B779F0 = DAT_008870C0.Get_Int("WOnline", "AllowFind", 1);
	DAT_00B779BC = DAT_008870C0.Get_Int("WOnline", "LangFilter", 1);
	DAT_00B7690C = DAT_008870C0.Get_Int("WOnline", "LobMusic", 1);
	DAT_00B77784 = DAT_008870C0.Get_Int("WOnline", "ShowAll", 1);
	DAT_00B779DC = DAT_008870C0.Get_Int("WOnline", "DisplayAsian", 1);
	DAT_00B779C0 = DAT_008870C0.Get_Int("WOnline", "DisplayLatin", 1);
	DAT_00B76AF0 = DAT_008870C0.Get_Bool("WOnline", "DisplayBuddies", false);
	DAT_00B73AD0 = DAT_008870C0.Get_Bool("WOnline", "DisplayClan", false);
	FUN_005EE190();
}

// FUNCTION: 0x0077DFD0
void FUN_0077DFD0(void)
{
	RawFileClass file("RA2MD.INI");
	DAT_008870C0.Put_Int("WOnline", "AllowPage", DAT_00B76234);
	DAT_008870C0.Put_Int("WOnline", "AllowFind", DAT_00B779F0);
	DAT_008870C0.Put_Int("WOnline", "LangFilter", DAT_00B779BC);
	DAT_008870C0.Put_Int("WOnline", "LobMusic", DAT_00B7690C);
	DAT_008870C0.Put_Int("WOnline", "ShowAll", DAT_00B77784);
	DAT_008870C0.Put_Int("WOnline", "DisplayAsian", DAT_00B779DC);
	DAT_008870C0.Put_Int("WOnline", "DisplayLatin", DAT_00B779C0);
	DAT_008870C0.Put_Bool("WOnline", "DisplayBuddies", DAT_00B76AF0);
	DAT_008870C0.Put_Bool("WOnline", "DisplayClan", DAT_00B73AD0);
	DAT_008870C0.Save(file, false);
	FUN_005EE1B0();
}

// FUNCTION: 0x0077E110
void FUN_0077E110(void)
{
	if (DAT_00AC116C) {
		DAT_00AC116C->V31(DAT_00B779BC);
		DAT_00AC116C->V26(DAT_00B779F0, DAT_00B76234);
	}
	FUN_005EE350(0);
	bool music = DAT_00B7690C != 0;
	if (music && Theme.Still_Playing() == false) {
		Theme.Stop();
		Theme.Queue_Song(THEME_PICK_ANOTHER);
	} else if (music) {
		if (Theme.What_Is_Playing() == Theme.From_Name("INTRO")) {
			Theme.Stop();
			Theme.Queue_Song(THEME_PICK_ANOTHER);
		}
	} else {
		Theme.Fade_Out();
	}
}

HWND FUN_00775B10(void);

// FUNCTION: 0x0077E1D0
int __fastcall FUN_0077E1D0(WideWstring const & msg)
{
	static int sel_user_indexes[128];
	HWND users = GetDlgItem(FUN_00775B10(), 0x53E);
	int count = SendMessage(users, LB_GETSELITEMS, 32, (LPARAM)sel_user_indexes);

	if (count == 0) {
		DAT_00AC116C->V58(msg.text());
		return 0;
	}
	User * last = NULL;
	User * head = NULL;
	User * item = NULL;
	User source;
	memset(&source, 0, sizeof(source));
	for (int i = 0; i < count; i++) {
		DAT_00B77900.get(source, sel_user_indexes[i]);
		item = new User;
		*item = source;
		item->Next = NULL;
		if (i == 0) head = item;
		if (last) last->Next = item;
		last = item;
	}
	if (head) DAT_00AC116C->V59(head, msg.text());
	while (head) {
		item = head;
		head = head->Next;
		delete item;
	}
	return 1;
}

// FUNCTION: 0x0077E300
int __fastcall FUN_0077E300(WideWstring const & msg)
{
	static int sel_user_indexes[128];
	HWND users = GetDlgItem(FUN_00775B10(), 0x53E);
	int count = SendMessage(users, LB_GETSELITEMS, 32, (LPARAM)sel_user_indexes);

	if (count == 0) {
		DAT_00AC116C->V60(msg.text());
		return 0;
	}
	User * last = NULL;
	User * head = NULL;
	User * item = NULL;
	User source;
	memset(&source, 0, sizeof(source));
	for (int i = 0; i < count; i++) {
		DAT_00B77900.get(source, sel_user_indexes[i]);
		item = new User;
		*item = source;
		item->Next = NULL;
		if (i == 0) head = item;
		if (last) last->Next = item;
		last = item;
	}
	if (head) DAT_00AC116C->V61(head, msg.text());
	while (head) {
		item = head;
		head = head->Next;
		delete item;
	}
	return 1;
}

#include "goptions.h"
namespace Game { extern int Seed; }
extern char DAT_00889F64[];
extern int DAT_00A8DA84;
extern NodeNameType ** DAT_00A8DA78;
extern IPXAddressClass DAT_00A8DBCC[8];
extern int DAT_00A8DC2C[8];
extern int DAT_00A8E230[8];
Wstring __fastcall FUN_007B82E0(WideWstring const & string);

extern int DAT_00B77E00;
extern int DAT_00B77E04;
extern int DAT_00B77E08;

// FUNCTION: 0x0077E780
int FUN_0077E780(void)
{
	return DAT_00B77E00 ^ DAT_00B77E08 ^ DAT_00B77E04;
}

extern int DAT_00B76B1C;
extern int DAT_00B76B14;
struct ChannelUserInfo {
	int House;
	char Pad[4];
	int Accepted;
	char Rest[0x88];
};
extern ChannelUserInfo DAT_00B76E58[];
int FUN_00777B50(void);
int FUN_005E94D0(void);
bool FUN_005E9560(void);
bool FUN_005E9580(void);

// FUNCTION: 0x0077E7A0
void __fastcall FUN_0077E7A0(char * out)
{
	int i = 0;
	int house1_count = 0;
	int house0_count = 0;
	for (int j = 0; j < DAT_00B76B1C; j++) {
		int house = DAT_00B76E58[j].House;
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
	sprintf(out, "%d,%d,%d,%d,%d,%s", FUN_00777B50(), FUN_0077E780(), FUN_005E94D0(), FUN_005E9580(), !FUN_005E9560(), Session.ScenarioFileName);
}

// FUNCTION: 0x0077E820
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

extern bool DAT_00B77DEC;
extern int DAT_00B77E4C;
extern int DAT_00B77E14;
extern int DAT_00B73A20;
extern int DAT_00B77D70;
extern int DAT_00B77A88;
HWND __fastcall FUN_00775940(int id);
void __fastcall FUN_005EDDE0(HWND window, char flag);
void __fastcall FUN_00783C80(int arg, bool flag);

extern wchar_t DAT_00A8D610[];
WideWstring __fastcall FUN_007B7ED0(Wstring const & text);

// FUNCTION: 0x0077EBA0
int __fastcall FUN_0077EBA0(Wstring const & who)
{
	Wstring name(who.length() > 0 ? who : DAT_00B73E40);
	Wstring lower = ((Wstring const &)name).toLower();
	if (FUN_007B7ED0(lower) == ((WideWstring const &)WideWstring(DAT_00A8D610)).toLower()) {
		return 1;
	}
	int found = -1;
	for (int i = 0; i < DAT_00A8DA84; i++) {
		Wstring slot_name = FUN_007B7F90(DAT_00A8DA78[i]->Name);
		if (((Wstring const &)slot_name).toLower() == lower) {
			found = i;
		}
	}
	if (found != -1) {
		return DAT_00A8DA78[found]->Slot.Unknown77;
	}
	return -1;
}

namespace Game { extern int PlayerColor; }
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

// FUNCTION: 0x0077ED00
int __fastcall FUN_0077ED00(Wstring const & who, int country, int color, int start, int team)
{
	int index = -1;
	Wstring lower = who.toLower();
	for (int j = 0; j < 8; j++) {
		Wstring slot_name = FUN_007B7F90(DAT_00A8DA90[j]->Name);
		if (lower == ((Wstring const &)slot_name).toLower()) {
			index = j;
			break;
		}
	}
	int result = 0;
	if (index == -1) {
		return 0;
	}
	HWND dialog = FUN_00775940(0xC2);
	if (dialog == NULL) {
		dialog = FUN_00775940(0xC9);
	}
	NodeNameType * slot = DAT_00A8DA90[index];
	if (slot->Get_Start() != start) {
		result = 1;
		slot->Set_Start(start);
	}
	if (DAT_00A8DA90[index]->Get_Team() != team) {
		result = 1;
		slot->Set_Team(team);
	}
	if (DAT_00A8DA90[index]->Get_Country() != country) {
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
	if (DAT_00A8DA90[index]->Get_Color() != color) {
		slot->Set_Color(color);
	}
	if (DAT_00A8DA90[index] == DAT_00A8DA78[0]) {
		Game::PlayerColor = color;
	}
	FUN_00783C80(0, false);
	return result;
}

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: 0x0077EF90
bool __fastcall FUN_0077EF90(int type, unsigned char ping)
{
	FUN_004068E0("Requesting channel list type = %d, ping = %d\n", type, ping);
	if (DAT_00AC116C->V06(type, ping) != 0) {
		return false;
	}
	DAT_00B76520.addTail(type);
	return true;
}

extern HANDLE DAT_00B73684[18];
extern unsigned int DAT_00B77DC8;
extern wchar_t DAT_00B77AD0[];
extern char DAT_0084A584[];
void __fastcall FUN_00792BE0(int id, WideWstring const & text, int flag);
void __fastcall FUN_007757E0(HWND window, int flag);
HWND FUN_00775B10(void);

static inline void Focus_Chat_Input(void)
{
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 &= ~1;
		if (DAT_00B77DC8 == 0) {
			FUN_007757E0(dialog, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
}

// FUNCTION: 0x0077F120
bool FUN_0077F120(void)
{
	FUN_00792BE0(1, StringTable::LoadString("TXT_REQ_CHANLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x8FA), 0);
	int timeout = 0;
	DWORD index;
	bool ok;
	do {
		while ((index = WaitForMultipleObjects(18, DAT_00B73684, FALSE, 100)) == WAIT_TIMEOUT) {
			timeout += 100;
			if (timeout > 30000) {
				swprintf(DAT_00B77AD0, StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x903));
				SetEvent(DAT_00B73684[0]);
				return false;
			}
			FUN_0077F900();
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
		ResetEvent(DAT_00B73684[index]);
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


// FUNCTION: 0x0077F320
bool __fastcall FUN_0077F320(int type, unsigned char ping, bool wait)
{
	if (DAT_00B76520.length() != 0) {
		if (!FUN_0077F120()) {
			return false;
		}
	}
	for (int i = 0; i < 18; i++) {
		ResetEvent(DAT_00B73684[i]);
	}
	if (!FUN_0077EF90(type, ping)) {
		SetEvent(DAT_00B73684[0]);
		return false;
	}
	if (wait) {
		return FUN_0077F120();
	}
	return true;
}

