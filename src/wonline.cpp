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
#include "playerprofile.h"
#include "ownrdraw.h"
#include "rcptr.h"
#include "rules.h"
#include "netshare.h"
#include "ipxmgr.h"
#include "_rules.h"
#include "gadget.h"
#include "_xmouse.h"

#include <windows.h>
#undef LoadString
#include <ole2.h>
#include <ocidl.h>
#include <shellapi.h>
#include <winsock.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include "stringtable.h"

HWND WS_Find_Dialog(int id);
HWND FUN_00775B10(void);
extern long DAT_00B77EA8;
void __fastcall FUN_004E34A0(HWND dialog, int side, int flag);
void __fastcall FUN_005E3720(char const * text);
extern int DAT_00B76440[];
rc_ptr<PlayerProfile> __fastcall FUN_00778710(void);

unsigned int __fastcall Wstring_Hash(Wstring & string);
#define HASH_ARG (Wstring_Hash)

struct WolChannel {
	int Type;
	unsigned int MinUsers;
	unsigned int MaxUsers;
	int CurrentUsers;
	unsigned int Official;
	int Tournament;
	unsigned int InGame;
	unsigned int Squad;
	unsigned long Reserved;
	unsigned long IPAddr;
	int Ping;
	int Hidden;
	WolChannel * Next;
	char Name[17];
	char Topic[81];
	char Location[65];
	char Key[9];
	char Info[41];
};

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
Wstring DAT_00B73AE0;

struct Server {
	char Pad[0x18];
	char name[71];
	char connlabel[5];
	char conndata[128];
	char login[10];
	char password[10];
};

// FUNCTION: GAMEMD 0x0077B900
// FUNCTION: GAMEMD 0x0077B920
ArrayList<Server> DAT_00B778D8;

// FUNCTION: GAMEMD 0x0077B940
// FUNCTION: GAMEMD 0x0077B960
ArrayList<WolChannel> DAT_00B73E10;

// FUNCTION: GAMEMD 0x0077B980
// FUNCTION: GAMEMD 0x0077B9A0
ArrayList<WolChannel> DAT_00B76AE0;

struct User {
	unsigned int Flags;
	char Pad[0x10];
	unsigned long Squad;
	unsigned long IPAddr;
	char Pad2[4];
	User * Next;
	char Nick[10];
	char Login[0x57 - 0x2E];
	char Abbrev[10];
	char Rest[0x64 - 0x61];
	int Locale;
	int Team;
};

// FUNCTION: GAMEMD 0x0077B9C0
// FUNCTION: GAMEMD 0x0077B9E0
ArrayList<User> DAT_00B77900;

// FUNCTION: GAMEMD 0x0077BA00
// FUNCTION: GAMEMD 0x0077BA20
ArrayList<int> DAT_00B76520;

struct LadderA {
	char Pad[8];
	int Field08;
	int Field0C;
	int Field10;
	char Pad2[8];
	int Rung;
	int Field20;
	char Data[0x6C - 0x24];
};

struct LadderB {
	char Data[0x44];
};

enum WolEntryState {
	WOL_ENTRY_STATE_NONE
};

// FUNCTION: GAMEMD 0x0077BA40
// FUNCTION: GAMEMD 0x0077BAE0
Dictionary<Wstring, LadderA> DAT_00B768C0 HASH_ARG;

// FUNCTION: GAMEMD 0x0077BC10
// FUNCTION: GAMEMD 0x0077BCB0
Dictionary<Wstring, LadderA> DAT_00B73AF8 HASH_ARG;

// FUNCTION: GAMEMD 0x0077BDE0
// FUNCTION: GAMEMD 0x0077BE80
Dictionary<Wstring, LadderA> DAT_00B77D80 HASH_ARG;

// FUNCTION: GAMEMD 0x0077BFB0
// FUNCTION: GAMEMD 0x0077C050
Dictionary<Wstring, LadderB> DAT_00B76640 HASH_ARG;

// FUNCTION: GAMEMD 0x0077C180
// FUNCTION: GAMEMD 0x0077C220
Dictionary<Wstring, int> DAT_00B76680 HASH_ARG;

// FUNCTION: GAMEMD 0x0077C350
// FUNCTION: GAMEMD 0x0077C370
ArrayList<Wstring> DAT_00B777A0;

// FUNCTION: GAMEMD 0x0077C3B0
// FUNCTION: GAMEMD 0x0077C450
Dictionary<Wstring, int> DAT_00B76530 HASH_ARG;

// FUNCTION: GAMEMD 0x0077C580
// FUNCTION: GAMEMD 0x0077C5A0
ArrayList<Wstring> DAT_00B765F8;

// FUNCTION: GAMEMD 0x0077C5E0
// FUNCTION: GAMEMD 0x0077C600
ArrayList<Wstring> DAT_00B73BD8;

// FUNCTION: GAMEMD 0x0077C640
// FUNCTION: GAMEMD 0x0077C660
ArrayList<Wstring> DAT_00B736F8;

// FUNCTION: GAMEMD 0x0077C6A0
// FUNCTION: GAMEMD 0x0077C6C0
ArrayList<Wstring> DAT_00B76B00;

// FUNCTION: GAMEMD 0x0077C700
// FUNCTION: GAMEMD 0x0077C720
ArrayList<Wstring> DAT_00B76510;

// FUNCTION: GAMEMD 0x0077C760
// FUNCTION: GAMEMD 0x0077C780
ArrayList<Wstring> DAT_00B73AE8;

// FUNCTION: GAMEMD 0x0077C7C0
// FUNCTION: GAMEMD 0x0077C7E0
ArrayList<Wstring> DAT_00B77820;

// FUNCTION: GAMEMD 0x0077C820
// FUNCTION: GAMEMD 0x0077C840
ArrayList<Wstring> DAT_00B73708;

// FUNCTION: GAMEMD 0x0077C880
// FUNCTION: GAMEMD 0x0077C8A0
ArrayList<Wstring> DAT_00B76188;

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

typedef WolChannel LadderC;

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
int FUN_0077CF40(void)
{
	int t = Session.unknown_0C;
	if (t == 3 || t == 1 || t == 2)
		return 1;
	return 0;
}

// FUNCTION: GAMEMD 0x0077CF60
int FUN_0077CF60(void)
{
	return Session.unknown_0C == 2;
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
	bool FUN_006241C0(void);
	int FUN_006241D0(void);
};

class FUN_007F7624_Class : public FUN_007EF738_Class {
public:
	FUN_007F7624_Class(void);
	virtual ~FUN_007F7624_Class(void);
};

// FUNCTION: GAMEMD 0x0077D1D0
FUN_007F7624_Class::FUN_007F7624_Class(void)
{
}

// FUNCTION: GAMEMD 0x0077D1F0
FUN_007F7624_Class::~FUN_007F7624_Class(void)
{
}

bool FUN_0077F900(void);

// FUNCTION: GAMEMD 0x0077D200
void FUN_0077D200(void)
{
	FUN_0077F900();
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

// FUNCTION: GAMEMD 0x0077D2A0
bool __fastcall FUN_0077D2A0(User * user)
{
	if (!user)
		return false;
	if (user->Flags & 1)
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
	if (user->Flags & 1)
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
	return wcsnicmp(text, buffer, wcslen(buffer)) == 0;
}

// FUNCTION: GAMEMD 0x0077D3A0
int __fastcall FUN_0077D3A0(wchar_t const * text)
{
	wchar_t buffer[256];
	swprintf(buffer, StringTable::LoadString("TXT_CLAN_FILTER", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x36F));
	return wcsnicmp(text, buffer, wcslen(buffer)) == 0;
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
		if (strcmp(name.get(), user->Nick) == 0)
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
	if (strcmpi(a->Login, b->Login) != 0)
		return false;
	else
		return true;
}

extern bool DAT_00B76AF0;
extern bool DAT_00B73AD0;
extern Wstring DAT_00B73E40;

// FUNCTION: GAMEMD 0x0077D4C0
bool __fastcall FUN_0077D4C0(User * user)
{
	if (DAT_00B76AF0 || DAT_00B73AD0) {
		if (WS_Find_Dialog(0x113)) {
			int index = FUN_0077D240(&DAT_00B73E40);
			User * host = NULL;
			int count = DAT_00B77900.length();
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

// FUNCTION: GAMEMD 0x0077D710
void FUN_0077D710(void)
{
	FUN_0077F900();
}

struct HostEntry {
	char Pad[0x18];
	unsigned long Address;
};

unsigned long __stdcall FUN_007C896E(unsigned long value);

// FUNCTION: GAMEMD 0x0077D720
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

// FUNCTION: GAMEMD 0x0077D7A0
void FUN_0077D7A0(bool play)
{
					if (play != 0) {
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

extern WolChannel DAT_00B76B10;

// FUNCTION: GAMEMD 0x0077D910
bool __fastcall FUN_0077D910(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = false;
	return chan->Tournament || (r = (chan->Reserved >> 8) & 1) || r;
}

// FUNCTION: GAMEMD 0x0077D940
bool FUN_0077D940(void)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	bool r = false;
	if (DAT_00B76B10.Tournament || (r = (DAT_00B76B10.Reserved >> 8) & 1)) {
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
	bool r = (DAT_00B76B10.Reserved >> 8) & 1;
	return r;
}

// FUNCTION: GAMEMD 0x0077D990
bool __fastcall FUN_0077D990(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return (chan->Reserved >> 8) & 1;
}

// FUNCTION: GAMEMD 0x0077D9B0
bool __fastcall FUN_0077D9B0(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return (chan->Reserved >> 11) & 1;
}

// FUNCTION: GAMEMD 0x0077D9D0
bool __fastcall FUN_0077D9D0(WolChannel * chan)
{
	if (Session.Type != GAME_INTERNET) {
		return false;
	}
	return (chan->Reserved >> 12) & 1;
}

// FUNCTION: GAMEMD 0x0077D9F0
// FUNCTION: GAMEMD 0x0077DA10
Wstring DAT_00B73E40;

// FUNCTION: GAMEMD 0x0077DA20
// FUNCTION: GAMEMD 0x0077DA40
Wstring DAT_00B7778C;

extern int DAT_00B779C4;
char * __fastcall FUN_007350C0(wchar_t const * text);

// FUNCTION: GAMEMD 0x0077DA50
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

bool __fastcall FUN_005E93F0(NodeNameType * slot);
Wstring __fastcall FUN_005EDEF0(NodeNameType * slot);

struct ChatSink {
	virtual long __stdcall V00(IID const & iid, void ** object);
	virtual void V01(void);
	virtual unsigned long __stdcall V02(void);
	virtual long __stdcall V03(void);
	virtual long __stdcall V04(unsigned long sku, unsigned long version, char const * name, char const * password, int timeout);
	virtual long __stdcall V05(Server * server, int timeout, int plain);
	virtual long __stdcall V06(int type, int ping);
	virtual void V07(void);
	virtual long __stdcall V08(WolChannel * channel);
	virtual long __stdcall V09(void);
	virtual void V10(void);
	virtual long __stdcall V11(char const * text);
	virtual long __stdcall V12(User * users, char * text);
	virtual long __stdcall V13(void);
	virtual long __stdcall V14(User * user, char * text);
	virtual long __stdcall V15(char * text);
	virtual void V16(void);
	virtual void V17(void);
	virtual long __stdcall V18(User * users);
	virtual void V19(void);
	virtual long __stdcall V20(unsigned long * version);
	virtual long __stdcall V21(User * user);
	virtual void V22(void);
	virtual void V23(void);
	virtual long __stdcall V24(User * user);
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
	virtual long __stdcall V35(unsigned long sku);
	virtual long __stdcall V36(int index, char const ** nick, char const ** pass);
	virtual long __stdcall V37(int slot, char const * login, char const * password, int plain);
	virtual void V38(void);
	virtual void V39(void);
	virtual void V40(void);
	virtual long __stdcall V41(char const * name, char const * value);
	virtual long __stdcall V42(char * info);
	virtual void V43(void);
	virtual void V44(void);
	virtual void V45(void);
	virtual long __stdcall V46(int locale);
	virtual long __stdcall V47(User * users);
	virtual long __stdcall V48(User * users);
	virtual void V49(void);
	virtual long __stdcall V50(int slot, int locale);
	virtual void V51(void);
	virtual long __stdcall V52(int * count);
	virtual long __stdcall V53(int mask);
	virtual void V54(void);
	virtual long __stdcall V55(void);
	virtual long __stdcall V56(User * user);
	virtual long __stdcall V57(User * user);
	virtual long __stdcall V58(wchar_t * text);
	virtual long __stdcall V59(User * users, wchar_t * text);
	virtual long __stdcall V60(wchar_t * text);
	virtual long __stdcall V61(User * users, wchar_t * text);
	virtual long __stdcall V62(User * user, wchar_t * text);
};
extern ChatSink * DAT_00AC116C;

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
	DAT_00AC116C->V15(list);
}

int ODMessageBox(const wchar_t * text, int type, void (*callback)(void), bool large);
extern char DAT_00A8ED80;
extern HWND DAT_00B73550;
int __cdecl FUN_007CD455(const char * path);

// FUNCTION: GAMEMD 0x0077DC90
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

// FUNCTION: GAMEMD 0x0077DED0
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

// FUNCTION: GAMEMD 0x0077DFD0
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

// FUNCTION: GAMEMD 0x0077E110
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

// FUNCTION: GAMEMD 0x0077E1D0
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

// FUNCTION: GAMEMD 0x0077E300
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
Wstring __fastcall FUN_007B82E0(WideWstring const & string);

extern int DAT_00B77E00;
extern int DAT_00B77E04;
extern int DAT_00B77E08;

// FUNCTION: GAMEMD 0x0077E780
int FUN_0077E780(void)
{
	return DAT_00B77E00 ^ DAT_00B77E08 ^ DAT_00B77E04;
}

extern int DAT_00B76B1C;
extern int DAT_00B76B14;
struct ChannelUserInfo {
	int House;
	int Unknown;
	int Accepted;
	char Rest[0x88];
};
extern ChannelUserInfo DAT_00B76E58[];
int FUN_00777B50(void);
int FUN_005E94D0(void);
bool FUN_005E9560(void);
bool FUN_005E9580(void);

// FUNCTION: GAMEMD 0x0077E7A0
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

extern bool DAT_00B77DEC;
extern int DAT_00B77E4C;
extern int DAT_00B77E14;
extern int DAT_00B73A20;
extern int DAT_00B77D70;
extern int DAT_00B77A88;
HWND __fastcall FUN_00775940(int id);
void __fastcall FUN_005EDDE0(HWND window, char flag);
void __fastcall FUN_00783C80(int arg, bool flag);

WideWstring __fastcall FUN_007B7ED0(Wstring const & text);

// FUNCTION: GAMEMD 0x0077EBA0
int __fastcall FUN_0077EBA0(Wstring const & who)
{
	Wstring name(who.length() > 0 ? who : DAT_00B73E40);
	Wstring lower = ((Wstring const &)name).toLower();
	if (FUN_007B7ED0(lower) == ((WideWstring const &)WideWstring(Session.GameName)).toLower()) {
		return 1;
	}
	int found = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		Wstring slot_name = FUN_007B7F90(Session.Players[i]->Name);
		if (((Wstring const &)slot_name).toLower() == lower) {
			found = i;
		}
	}
	if (found != -1) {
		return Session.Players[found]->Slot.Unknown77;
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

// FUNCTION: GAMEMD 0x0077ED00
int __fastcall FUN_0077ED00(Wstring const & who, int country, int color, int start, int team)
{
	int index = -1;
	Wstring lower = who.toLower();
	for (int j = 0; j < 8; j++) {
		Wstring slot_name = FUN_007B7F90(Session.Slots[j]->Name);
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
		Game::PlayerColor = color;
	}
	FUN_00783C80(0, false);
	return result;
}

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x0077EF90
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
void __fastcall FUN_00792BE0(int id, WideWstring const & text, char flag);
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

// FUNCTION: GAMEMD 0x0077F120
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


// FUNCTION: GAMEMD 0x0077F320
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

// FUNCTION: GAMEMD 0x0077F520
bool __fastcall FUN_0077F520(bool wait)
{
	if (DAT_00B76520.length() != 0) {
		if (!FUN_0077F120()) {
			return false;
		}
	}
	for (int i = 0; i < 18; i++) {
		ResetEvent(DAT_00B73684[i]);
	}
	if (!FUN_0077EF90(0, 0)) {
		SetEvent(DAT_00B73684[0]);
		return false;
	}
	if (wait) {
		return FUN_0077F120();
	}
	return true;
}

// FUNCTION: GAMEMD 0x0077F710
bool __fastcall FUN_0077F710(unsigned char ping, bool wait)
{
	if (DAT_00B76520.length() != 0) {
		if (!FUN_0077F120()) {
			return false;
		}
	}
	for (int i = 0; i < 18; i++) {
		ResetEvent(DAT_00B73684[i]);
	}
	if (!FUN_0077EF90(0x29, ping)) {
		SetEvent(DAT_00B73684[0]);
		return false;
	}
	if (wait) {
		return FUN_0077F120();
	}
	return true;
}

extern int DAT_00B7646C;
extern unsigned int DAT_00B7617C;
extern unsigned int DAT_00B76180;
struct DownloadObject : IUnknown {
	virtual long __stdcall DownloadFile(char const * server, char const * login, char const * password, char const * remote, char const * local, char const * registry) = 0;
	virtual long __stdcall Abort(void) = 0;
	virtual long __stdcall Pump(void) = 0;
};

extern Dictionary<Wstring, LadderA> * DAT_00B77E0C;
extern int DAT_00B779C8;
extern int DAT_00B736F0;
extern int DAT_0084A234;
extern WolChannel DAT_00B73BE8;
extern char DAT_00B75E58[0xF8];
extern char DAT_00B769C8[0xF8];
extern int DAT_00B736CC[8];
extern char DAT_00B77A94[0x29];
extern int DAT_00B76468;
extern int DAT_00B779CC;
extern char DAT_00B77DC0;
extern char DAT_00B77DC1;
extern char DAT_00B77DC2;
extern char DAT_0084A1B8;
extern int DAT_0084A1BC;
extern int DAT_00B77DCC;
extern int DAT_00B77DB8;
extern int DAT_00B77DBC;
extern wchar_t const * DAT_00B73E1C[8];
struct NetUtilSink {
	virtual long __stdcall V00(IID const & iid, void ** object);
	virtual unsigned long __stdcall V01(void);
	virtual unsigned long __stdcall V02(void);
	virtual void V03(void);
	virtual void V04(void);
	virtual long __stdcall V05(char const * a, int b, char const * c, unsigned long d, int e, int f, int g);
	virtual long __stdcall V06(char const * address, int timeout, int * handle);
	virtual long __stdcall V07(void);
	virtual long __stdcall V08(unsigned long address, int * ping);
	virtual void V09(void);
	virtual void V10(void);
	virtual long __stdcall V11(char const * host, int port, int mode);
	virtual void V12(void);
	virtual void V13(void);
	virtual long __stdcall V14(char const * a, int b, char const * c, unsigned long d);
};
extern NetUtilSink * DAT_00AC1170;
extern IUnknown * DAT_00AC1174;
extern DWORD DAT_00B77E54;
extern DWORD DAT_00B77E58;
extern DWORD DAT_00B77E5C;
extern int DAT_00B77DDC;
extern int DAT_00B77DE0;
extern int DAT_00B77DE4;
extern int DAT_00B77DD0;
extern int DAT_00B76E38;
extern int DAT_00B77DD8;
extern int DAT_00B77E28;
extern int DAT_00B77E60;
extern int DAT_00B77E64;
extern int DAT_00B77E20;


extern int DAT_00AA0430;
extern int __fastcall FUN_0053E770(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max, unsigned int remove);
void Title_Screen_Restore(bool force);
void __cdecl FUN_005E2700(int color, wchar_t const * text, ...);
void __fastcall FUN_005E32D0(bool force, bool now);
int FUN_00775B20(void);
void FUN_00794BA0(void);
void Call_Back(void);
bool FUN_00796710(void);
bool FUN_00786270(void);
extern int DAT_00B77E6C;
extern int DAT_00B77E70;
extern char DAT_00B77E26;
extern char DAT_00B77E27;
extern HANDLE DAT_00B73698;

bool FUN_0077F900(void);

// FUNCTION: GAMEMD 0x00780110
void FUN_00780110(void)
{
	DAT_00B75E48 = FUN_00778710();
	DAT_00B73E10.clear();
	DAT_00B76AE0.clear();
	DAT_00B77900.clear();
	DAT_00B777A0.clear();
	DAT_00B76680.clear();
	DAT_00B765F8.clear();
	DAT_00B76530.clear();
	DAT_00B7646C = time(NULL) + 86400;
	DAT_00B7617C = 0;
	DAT_00B76180 = 0;
	DAT_00B768C0.clear();
	DAT_00B73AF8.clear();
	DAT_00B76608.clear();
	DAT_00B73AA8.clear();
	DAT_00B76980.clear();
	DAT_00B76990.clear();
	if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
		int sku = 0x2300;
		DAT_00B779C8 = sku;
		DAT_00B736F0 = sku;
		DAT_00B77E0C = &DAT_00B73AF8;
	} else {
		int sku = DAT_0084A234;
		DAT_00B779C8 = sku;
		DAT_00B736F0 = sku;
		DAT_00B77E0C = &DAT_00B77D80;
	}
	memset(&DAT_00B76B10, 0, 0x10C);
	memset(&DAT_00B73BE8, 0, 0x10C);
	memset(DAT_00B75E58, 0, 0xF8);
	memset(DAT_00B769C8, 0, 0xF8);
	DAT_00B77AD0[0] = 0;
	DAT_00B77E14 = 0;
	DAT_00B73A20 = 1;
	DAT_00B77D70 = -1;
	DAT_00B77A88 = time(NULL);
	DAT_00B76468 = 0;
	DAT_00B779CC = 0;
	DAT_00B77E4C = 0;
	DAT_00B77DC8 = 0;
	DAT_00B77DEC = 0;
	DAT_00B77DC0 = 0;
	DAT_0084A1B8 = 1;
	DAT_00B73AE0.clear();
	memset(DAT_00B73684, 0, sizeof(DAT_00B73684));
	DAT_00B77DC1 = 0;
	DAT_00B77DC2 = 0;
	DAT_0084A1BC = -1;
	DAT_00B77DCC = 0;
	DAT_00B77DB8 = -1;
	DAT_00B77DBC = -1;
	DAT_00B73E1C[0] = StringTable::LoadString("TXT_LOB_1", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC0);
	DAT_00B73E1C[1] = StringTable::LoadString("TXT_LOB_2", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC1);
	DAT_00B73E1C[2] = StringTable::LoadString("TXT_LOB_3", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC2);
	DAT_00B73E1C[3] = StringTable::LoadString("TXT_LOB_4", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC3);
	DAT_00B73E1C[4] = StringTable::LoadString("TXT_LOB_5", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC4);
	DAT_00B73E1C[5] = StringTable::LoadString("TXT_LOB_6", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC5);
	DAT_00B73E1C[6] = StringTable::LoadString("TXT_LOB_7", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC6);
	DAT_00B73E1C[7] = StringTable::LoadString("TXT_LOB_8", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xAC7);
	memset(DAT_00B736CC, -1, sizeof(DAT_00B736CC));
	DAT_00AC116C = NULL;
	DAT_00AC1170 = NULL;
	DAT_00AC1174 = NULL;
	DAT_00B77E54 = 0;
	DAT_00B77E58 = 0;
	DAT_00B77E5C = 0;
	DAT_00B77DDC = 0;
	DAT_00B77DE0 = 0;
	memset(DAT_00B77A94, 0, sizeof(DAT_00B77A94));
	DAT_00B77DE4 = 0;
	DAT_00B77DD0 = 0;
	DAT_00B76E38 = 1;
	DAT_00B77DD8 = 0;
	DAT_00B76520.clear();
	DAT_00B77E28 = 0;
	DAT_00B77E60 = 0;
	DAT_00B77E64 = 0;
	DAT_00B77E00 = RulesClass::Get_Rule_Unique_ID();
	RulesClass::Load_Art_INI();
	DAT_00B77E04 = RulesClass::Get_Art_Unique_ID();
	DAT_00B77E08 = RulesClass::Get_AI_Unique_ID();
	DAT_00B77E20 = 0;
}

int __fastcall FUN_00623F00(HWND list, int index, Wstring * text);
int __fastcall FUN_00623E00(HWND list);
extern int DAT_00B76C1C;
extern wchar_t DAT_00887734[];

// FUNCTION: GAMEMD 0x007806C0
void FUN_007806C0(void)
{
	HWND dialog = FUN_00775940(0x113);
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
	int count = DAT_00B73E10.length();
	for (int i = 0; i < count; i++) {
		WolChannel chan;
		DAT_00B73E10.get(chan, i);
		if (strncmp(chan.Name, "Lob_41_", strlen("Lob_41_")) == 0) {
			int number = atol(chan.Name + strlen("Lob_41_"));
			bool current = strcmp(chan.Name, DAT_00B76B10.Name) == 0;
			int copy = number / 8;
			int slot = number % 8;
			wchar_t info[80];
			if (copy == 0) {
				swprintf(info, L"%s  (%d)", DAT_00B73E1C[slot], chan.CurrentUsers);
			} else {
				swprintf(info, L"%s_%d  (%d)", DAT_00B73E1C[slot], copy + 1, chan.CurrentUsers);
			}
			int entries = SendMessage(list, LB_GETCOUNT, 0, 0);
			int pos;
			for (pos = 0; pos < entries; pos++) {
				int item = SendMessage(list, LB_GETITEMDATA, pos, 0);
				WolChannel other;
				DAT_00B73E10.get(other, item);
				if (atol(other.Name + strlen("Lob_41_")) > number) {
					break;
				}
			}
			int row = SendMessage(list, 0x4C7, pos, (LPARAM)chan.Name);
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
	Wstring name(user->Nick);
	WideWstring text = FUN_007B7ED0(name) + L" : ";
	name.toLower();
	LadderA ladder;
	memset(&ladder, 0, sizeof(ladder));
	if (DAT_00B77E0C && DAT_00B77E0C->getValue(name, ladder)) {
		text += WideWstring(StringTable::LoadString("TXT_RANK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xB79)) + WideWstring(L" ") + FUN_007B7DD0(ladder.Rung);
	} else {
		text += StringTable::LoadString("TXT_UNRANKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xB7B);
	}
	Wstring login(user->Login);
	if (login.length() == 0) {
		login = (char *)user + 0x57;
	}
	if (login.length() > 0) {
		text += FUN_007B8090(L": ", FUN_007B7ED0(login));
	}
	if (DAT_00B76530.contains(name)) {
		if (DAT_00AC116C) {
			int locale = 0;
			DAT_00B76530.getValue(name, locale);
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
	WolChannel chan;
	memset(&chan, 0, sizeof(chan));
	int item = SendMessage(list, LB_GETITEMDATA, index, 0);
	if (item >= 0 && item < DAT_00B76AE0.length()) {
		DAT_00B76AE0.get(chan, item);
		Wstring name(chan.Name);
		name.truncate((char)0x27);
		User user;
		memset(&user, 0, sizeof(user));
		strncpy(user.Nick, name.get(), 10);
		WideWstring text = FUN_00780A90(&user);
		text += WideWstring(L", ");
		if (chan.Ping != -1) {
			wchar_t ping[64];
			swprintf(ping, StringTable::LoadString("MSG:PingInfo", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBCD), chan.Ping);
			text += ping;
		} else {
			text += StringTable::LoadString("TXT_UNKNOWN_PING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBD0);
		}
		char details[0x84];
		strncpy(details, chan.Info, 0x80);
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
	if (item >= 0 && item < DAT_00B77900.length()) {
		User user;
		memset(&user, 0, sizeof(user));
		DAT_00B77900.get(user, item);
		return FUN_00780A90(&user);
	}
	return WideWstring(StringTable::LoadString("STT:LobbyListUsers", NULL, "D:\\ra2mdpost\\wonline.cpp", 0xBEF));
}

extern int DAT_00B77E44;

// FUNCTION: GAMEMD 0x00781130
bool __fastcall FUN_00781130(WolChannel * chan, int mode, int a, int b)
{
	int filter = DAT_00B77E44;
	if ((filter == 2 && mode == 1) || (filter == 3 && mode == 0)) {
		if (chan->Squad & 0x100) {
			return true;
		}
	}
	if ((filter == 2 && mode == 0) || (filter == 3 && mode == 1)) {
		if (!(chan->Squad & 0x100)) {
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
		if ((chan->Reserved >> 9) & 3) {
			return true;
		}
	}
	if ((filter == 6 && mode == 0) || (filter == 7 && mode == 1)) {
		if (!((chan->Reserved >> 9) & 3)) {
			return true;
		}
	}
	if ((filter == 8 && mode == 1) || (filter == 9 && mode == 0)) {
		if (chan->Tournament) {
			return true;
		}
	}
	if ((filter == 8 && mode == 0) || (filter == 9 && mode == 1)) {
		if (!chan->Tournament) {
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

struct UserSort {
	int Index;
	char * Name;
	int Flag;
};

struct PCX {
	static PCX Instance;
	int GetSurface(char const * name, void * palette);
};
namespace Game { extern int ObserverMode; }
extern int DAT_00B77A8C;
extern char DAT_00B77E74;
extern int DAT_007EEF28;
extern int DAT_00B778D0;
extern int DAT_00B778CC;
extern int DAT_00B76AF4;
extern int DAT_00B7643C;
extern int DAT_007EEF48;
extern wchar_t DAT_0084A724[];
extern wchar_t DAT_0084A6E0[];
int __fastcall FUN_00623F00(HWND list, int index, Wstring * text);
HWND FUN_00775B10(void);

void FUN_00781480(void);

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
	int count = DAT_00B77900.length();
	if (count == 0) {
		DAT_00B77900.addTail(*user);
		return;
	}
	int owners = 0;
	DAT_00B77900.get(temp, 0);
	while (temp.Flags & 1) {
		if (owners >= count) {
			break;
		}
		owners++;
		DAT_00B77900.get(temp, owners);
	}
	int pos;
	if (user->Flags & 1) {
		pos = 0;
		DAT_00B77900.get(temp, 0);
		if (ascending) {
			while (pos < owners) {
				if (strcmpi(temp.Nick, user->Nick) >= 0) {
					break;
				}
				pos++;
				DAT_00B77900.get(temp, pos);
			}
		} else {
			while (pos < owners) {
				if (strcmpi(temp.Nick, user->Nick) <= 0) {
					break;
				}
				pos++;
				DAT_00B77900.get(temp, pos);
			}
		}
		DAT_00B77900.add(*user, pos);
	} else {
		pos = owners;
		DAT_00B77900.get(temp, owners);
		if (ascending) {
			while (pos < count) {
				if (strcmpi(temp.Nick, user->Nick) >= 0) {
					break;
				}
				pos++;
				DAT_00B77900.get(temp, pos);
			}
		} else {
			while (pos < count) {
				if (strcmpi(temp.Nick, user->Nick) <= 0) {
					break;
				}
				pos++;
				DAT_00B77900.get(temp, pos);
			}
		}
		DAT_00B77900.add(*user, pos);
	}
}

// FUNCTION: GAMEMD 0x00781D20
void __fastcall FUN_00781D20(bool ascending)
{
	ArrayList<User> copy;
	int count = DAT_00B77900.length();
	UserSort * sorted = new UserSort[count];
	if (!sorted) {
		return;
	}
	User temp;
	memset(&temp, 0, sizeof(temp));
	for (int i = 0; i < count; i++) {
		DAT_00B77900.get(temp, i);
		sorted[i].Index = i;
		sorted[i].Name = new char[10];
		if (!sorted[i].Name) {
			return;
		}
		strcpy(sorted[i].Name, temp.Nick);
		sorted[i].Flag = temp.Flags & 1;
		copy.addTail(temp);
	}
	DAT_00B77900.clear();
	if (ascending) {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_00781CC0);
	} else {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_00781CF0);
	}
	int k;
	for (k = 0; k < count; k++) {
		copy.get(temp, sorted[k].Index);
		DAT_00B77900.addTail(temp);
	}
	copy.clear();
	for (k = 0; k < count; k++) {
		if (sorted[k].Name) {
			delete sorted[k].Name;
		}
	}
	delete sorted;
}

void __fastcall FUN_00782BB0(User * user, bool ascending);
extern int DAT_00B77E48;
void __fastcall FUN_00782320(User * user, bool ascending);

void __fastcall FUN_00782320(User * user, bool ascending);

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
	DAT_00B77900.get(first, a->Index);
	DAT_00B77900.get(second, b->Index);
	return strcmpi(first.Nick, second.Nick);
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
	DAT_00B77900.get(first, a->Index);
	DAT_00B77900.get(second, b->Index);
	return strcmpi(first.Nick, second.Nick);
}

// FUNCTION: GAMEMD 0x00783270
void __fastcall FUN_00783270(bool ascending)
{
	ArrayList<User> copy;
	int count = DAT_00B77900.length();
	UserRank * sorted = new UserRank[count];
	if (!sorted) {
		return;
	}
	User temp;
	memset(&temp, 0, sizeof(temp));
	for (int i = 0; i < count; i++) {
		DAT_00B77900.get(temp, i);
		sorted[i].Index = i;
		sorted[i].Key = -1;
		Wstring name;
		LadderA info;
		memset(&info, 0, sizeof(info));
		name.set(temp.Nick);
		name.toLower();
		if (DAT_00B77E0C && DAT_00B77E0C->getValue(name, info)) {
			sorted[i].Key = info.Rung;
		}
		sorted[i].Flag = temp.Flags & 1;
		copy.addTail(temp);
	}
	if (ascending) {
		qsort(sorted, count, sizeof(UserRank), (CompareFn)FUN_007830C0);
	} else {
		qsort(sorted, count, sizeof(UserRank), (CompareFn)FUN_007831A0);
	}
	DAT_00B77900.clear();
	for (int k = 0; k < count; k++) {
		copy.get(temp, sorted[k].Index);
		DAT_00B77900.addTail(temp);
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
	return lb > 0;
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
	return lb > 0;
}

int __cdecl FUN_007835C0(UserSort const * a, UserSort const * b);
int __cdecl FUN_007836E0(UserSort const * a, UserSort const * b);

// FUNCTION: GAMEMD 0x00783800
void __fastcall FUN_00783800(bool ascending)
{
	ArrayList<User> copy;
	int count = DAT_00B77900.length();
	UserSort * sorted = new UserSort[count];
	if (!sorted) {
		return;
	}
	User temp;
	memset(&temp, 0, sizeof(temp));
	for (int i = 0; i < count; i++) {
		DAT_00B77900.get(temp, i);
		sorted[i].Index = i;
		sorted[i].Name = new char[10];
		if (!sorted[i].Name) {
			return;
		}
		strcpy(sorted[i].Name, temp.Abbrev);
		sorted[i].Flag = temp.Flags & 1;
		copy.addTail(temp);
	}
	if (ascending) {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_007835C0);
	} else {
		qsort(sorted, count, sizeof(UserSort), (CompareFn)FUN_007836E0);
	}
	DAT_00B77900.clear();
	int k;
	for (k = 0; k < count; k++) {
		copy.get(temp, sorted[k].Index);
		DAT_00B77900.addTail(temp);
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
int __fastcall FUN_00783A90(int digit)
{
	switch (digit) {
		case 0:
			return PCX::Instance.GetSurface("number0.pcx", NULL);
		case 1:
			return PCX::Instance.GetSurface("number1.pcx", NULL);
		case 2:
			return PCX::Instance.GetSurface("number2.pcx", NULL);
		case 3:
			return PCX::Instance.GetSurface("number3.pcx", NULL);
		case 4:
			return PCX::Instance.GetSurface("number4.pcx", NULL);
		case 5:
			return PCX::Instance.GetSurface("number5.pcx", NULL);
		case 6:
			return PCX::Instance.GetSurface("number6.pcx", NULL);
		case 7:
			return PCX::Instance.GetSurface("number7.pcx", NULL);
		case 8:
			return PCX::Instance.GetSurface("number8.pcx", NULL);
		case 9:
			return PCX::Instance.GetSurface("number9.pcx", NULL);
	}
	return 0;
}

struct RankedUser {
	char Pad[0xC];
	unsigned int Points;
};

// FUNCTION: GAMEMD 0x00783B80
int __fastcall FUN_00783B80(RankedUser * user)
{
	if (!user) {
		return 0;
	}
	unsigned int points = user->Points;
	if (points > 2000) {
		return PCX::Instance.GetSurface("comchief.pcx", NULL);
	}
	if (points > 1500) {
		return PCX::Instance.GetSurface("stargen.pcx", NULL);
	}
	if (points > 1000) {
		return PCX::Instance.GetSurface("general.pcx", NULL);
	}
	if (points > 500) {
		return PCX::Instance.GetSurface("briggenr.pcx", NULL);
	}
	if (points > 250) {
		return PCX::Instance.GetSurface("colonel.pcx", NULL);
	}
	if (points > 150) {
		return PCX::Instance.GetSurface("major.pcx", NULL);
	}
	if (points > 80) {
		return PCX::Instance.GetSurface("lieutena.pcx", NULL);
	}
	if (points > 60) {
		return PCX::Instance.GetSurface("sergeant.pcx", NULL);
	}
	if (points > 31) {
		return PCX::Instance.GetSurface("corporal.pcx", NULL);
	}
	return PCX::Instance.GetSurface("private.pcx", NULL);
}

// FUNCTION: GAMEMD 0x00785360
void FUN_00785360(void)
{
	FUN_00783C80(0, false);
}

struct AtlObjectEntry;

struct AtlExitNode {
	void (__stdcall * Function)(void * argument);
	void * Argument;
	AtlExitNode * Next;
};

struct AtlModule {
	unsigned int cbSize;
	char Pad0[0xC];
	AtlObjectEntry * Entries;
	long LockCount;
	HANDLE Heap;
	CRITICAL_SECTION Lock0;
	CRITICAL_SECTION Lock1;
	CRITICAL_SECTION Lock2;
	unsigned int BuildVersion;
	char Pad2[4];
	bool OwnsHeaps;
	char Pad3[3];
	GUID const * VersionGuid;
	unsigned int HeapCount;
	HANDLE * Heaps;
	char Pad4[4];
	AtlExitNode * ExitList;

	long Init(void * objects, HINSTANCE instance, GUID const * libid = NULL);
	long Lock(void);
	long Unlock(void);
};

extern AtlModule DAT_00B76488;
long __stdcall FUN_007AC500(AtlModule * module);

struct AtlIntMapEntry {
	IID const * piid;
	DWORD dw;
	long (__stdcall * pFunc)(void * pThis, IID const & iid, void ** object, DWORD dw);
};

extern AtlIntMapEntry DAT_007F7698[];
extern AtlIntMapEntry DAT_007F7650[];
extern AtlIntMapEntry DAT_007F7638[];
extern AtlModule * DAT_00A8F7F0;
extern GUID const DAT_00825CD0;
long __stdcall FUN_007A2F30(AtlModule * module, void * objects, HINSTANCE instance);

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

inline long __stdcall FUN_007A2EA0(IUnknown * source, IUnknown * sink, IID const & iid, DWORD * cookie)
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

static inline bool Is_Unknown_Iid(IID const & iid)
{
	long const * words = (long const *)&iid;
	return words[0] == 0 && words[1] == 0 && words[2] == 0xC0 && words[3] == 0x46000000;
}

static inline bool Same_Iid(IID const & a, IID const & b)
{
	long const * x = (long const *)&a;
	long const * y = (long const *)&b;
	return x[0] == y[0] && x[1] == y[1] && x[2] == y[2] && x[3] == y[3];
}

static inline long Atl_Internal_Query_Interface(void * pThis, AtlIntMapEntry * entries, IID const & iid, void ** object)
{
	if (object == NULL) {
		return E_POINTER;
	}
	*object = NULL;
	if (Is_Unknown_Iid(iid)) {
		IUnknown * unknown = (IUnknown *)((int)pThis + entries->dw);
		unknown->AddRef();
		*object = unknown;
		return 0;
	}
	while (entries->pFunc != NULL) {
		BOOL blind = (entries->piid == NULL);
		if (blind || Same_Iid(*entries->piid, iid)) {
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
	VersionGuid = &DAT_00825CD0;
	DAT_00A8F7F0 = this;
	cbSize = 0x84;
	BuildVersion = 0x300;
	FUN_007A2F30(this, objects, instance);
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
		return Atl_Internal_Query_Interface(pThis, entries, iid, object);
	}

	long RefCount;
};

struct AtlSinkRoot : AtlRootBase {
	unsigned long InternalAddRef(void) { return AtlThreadModel::Increment(&RefCount); }
	unsigned long InternalRelease(void) { return AtlThreadModel::Decrement(&RefCount); }

	AtlCriticalSection Lock;
};

class CNetUtilEventSink : public AtlSinkRoot, public IUnknown
{
	public:
		CNetUtilEventSink(void);
		static AtlIntMapEntry * __stdcall _GetEntries(void) { return DAT_007F7650; }
		IUnknown * _GetRawUnknown(void) { return (IUnknown *)((int)this + _GetEntries()->dw); }
		IUnknown * GetUnknown(void) { return _GetRawUnknown(); }
		long _InternalQueryInterface(IID const & iid, void ** object) { return InternalQueryInterface(this, _GetEntries(), iid, object); }
};

class CChatEventSink : public AtlSinkRoot, public IUnknown
{
	public:
		CChatEventSink(void);
		static AtlIntMapEntry * __stdcall _GetEntries(void) { return DAT_007F7698; }
		IUnknown * _GetRawUnknown(void) { return (IUnknown *)((int)this + _GetEntries()->dw); }
		IUnknown * GetUnknown(void) { return _GetRawUnknown(); }
		long _InternalQueryInterface(IID const & iid, void ** object) { return InternalQueryInterface(this, _GetEntries(), iid, object); }
};

class CDownloadEventSink : public AtlSinkRoot, public IUnknown
{
	public:
		CDownloadEventSink(void) {}
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
		CComObject(void * = NULL) { InterlockedIncrement(&DAT_00B76488.LockCount); }
		~CComObject(void)
		{
			RefCount = 1;
			FinalRelease();
			DAT_00B76488.Unlock();
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
extern ATL::CComObject<CChatEventSink> * DAT_00B778F8;
extern ATL::CComObject<CNetUtilEventSink> * DAT_00B736EC;
extern ATL::CComObject<CDownloadEventSink> * DAT_00B75F68;

static inline HRESULT Unadvise_Sink(IUnknown * source, IID const & iid, DWORD cookie)
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
int __fastcall FUN_00785370(HINSTANCE instance)
{
	HRESULT result = 0;
	DAT_00B76488.Init(NULL, instance);
	DAT_00AC116C = NULL;
	DAT_00AC1170 = NULL;
	DAT_00AC1174 = NULL;
	DAT_00B77E54 = 0;
	DAT_00B77E58 = 0;
	DAT_00B77E5C = 0;
	CoCreateInstance(DAT_007F75E0, NULL, CLSCTX_INPROC_SERVER, DAT_007F7540, (void **)&DAT_00AC116C);
	if (DAT_00AC116C) {
		ATL::CComObject<CChatEventSink>::CreateInstance(&DAT_00B778F8);
		result = FUN_007A2EA0((IUnknown *)DAT_00AC116C, DAT_00B778F8->GetUnknown(), DAT_007F7550, &DAT_00B77E54);
	}
	if (DAT_00AC116C == NULL || result != S_OK) {
		ODMessageBox(StringTable::LoadString("TXT_APIMISSING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x23DC), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	CoCreateInstance(DAT_007F7600, NULL, CLSCTX_INPROC_SERVER, DAT_007F7580, (void **)&DAT_00AC1170);
	if (DAT_00AC1170) {
		ATL::CComObject<CNetUtilEventSink>::CreateInstance(&DAT_00B736EC);
		result = FUN_007A2EA0((IUnknown *)DAT_00AC1170, DAT_00B736EC->GetUnknown(), DAT_007F7590, &DAT_00B77E58);
	}
	if (DAT_00AC1170 == NULL || result != S_OK) {
		ODMessageBox(StringTable::LoadString("TXT_APIMISSING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x23EC), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	CoCreateInstance(DAT_007F75F0, NULL, CLSCTX_INPROC_SERVER, DAT_007F7560, (void **)&DAT_00AC1174);
	if (DAT_00AC1174) {
		ATL::CComObject<CDownloadEventSink>::CreateInstance(&DAT_00B75F68);
		result = FUN_007A2EA0(DAT_00AC1174, DAT_00B75F68->GetUnknown(), DAT_007F7570, &DAT_00B77E5C);
	}
	if (DAT_00AC1174 == NULL || result != S_OK) {
		ODMessageBox(StringTable::LoadString("TXT_APIMISSING", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x23FC), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	unsigned long version;
	DAT_00AC116C->V20(&version);
	if (version < 0x10010) {
		ODMessageBox(StringTable::LoadString("TXT_APIWRONGVERSION", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2408), 0, (bool (*)(void))NULL, 0);
		return -2;
	}
	DAT_00AC116C->V41("RegPath", APP_REG_KEY);
	HKEY key;
	char path[256];
	if (APP_REG_KEY != NULL && strlen(APP_REG_KEY)) {
		strcpy(path, APP_REG_KEY);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ, &key) == ERROR_SUCCESS) {
			DWORD type;
			DWORD value;
			DWORD size = sizeof(value);
			if (RegQueryValueEx(key, "SKU", NULL, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
				DAT_00AC116C->V35(value);
			}
			RegCloseKey(key);
		}
	}
	return 1;
}

static inline void Close_Logout_Wait(unsigned int event)
{
	HWND window = FUN_00775940(0x11E);
	if (window) {
		FUN_007757E0(window, 0);
	}
	window = FUN_00775940(0xC4);
	if (window) {
		DAT_00B77DC8 &= ~event;
		if (DAT_00B77DC8 == 0) {
			FUN_007757E0(window, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
}

namespace OwnerDraw {
	void __stdcall SetNeedsControlImage(HWND window, int value);
}
int __fastcall FUN_00788A30(bool flag);
void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);
void __cdecl FUN_005E2700(int color, wchar_t const * text, ...);
void __fastcall FUN_005E9340(char const * nick);
void FUN_005E9850(void);
void __fastcall FUN_005E32D0(bool force, bool now);
int FUN_00775B20(void);
void FUN_0072CF90(void);
extern char DAT_00B76E40[0x940];
extern int DAT_007EEF28;
int FUN_0077CF40(void);
void __fastcall FUN_007A0EA0(char const * name, int ping);
#include "multiplayer.h"

void __fastcall FUN_00785740(User * user);

// FUNCTION: GAMEMD 0x00785EA0
void FUN_00785EA0(void)
{
	DAT_00B77DC0 = 1;
	if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
		DAT_00B76680.clear();
	}
	if (DAT_00AC116C) {
		for (int i = 0; i < 18; i++) {
			ResetEvent(DAT_00B73684[i]);
		}
		FUN_0077F900();
		if (DAT_00AC116C->V13() == 0) {
			FUN_00792BE0(0x20, StringTable::LoadString("TXT_WOL_LOGGING_OUT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2537), 0);
			for (int tries = 0; tries < 5; tries++) {
				DWORD index;
				while ((index = WaitForMultipleObjects(18, DAT_00B73684, FALSE, 100)) == WAIT_TIMEOUT) {
					FUN_0077F900();
				}
				ResetEvent(DAT_00B73684[index]);
				if (index == 0 || index == 9) {
					break;
				}
			}
		}
		Close_Logout_Wait(0x20);
	}
	int budget = DAT_00B77900.length();
}

extern IID const DAT_007F7550;
extern IID const DAT_007F7590;
extern IID const DAT_007F7570;
extern char DAT_00B77E75;
void FUN_005E8A20(void);

// FUNCTION: GAMEMD 0x00786080
void FUN_00786080(void)
{
	if (MultiplayerMapPreview) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = NULL;
	}
	HWND window = FUN_00775940(0x11E);
	if (window) {
		FUN_007757E0(window, 0);
	}
	if (DAT_00B77E75) {
		FUN_005E8A20();
		if (DAT_00AC116C) {
			Unadvise_Sink((IUnknown *)DAT_00AC116C, DAT_007F7550, DAT_00B77E54);
			DAT_00AC116C->V02();
		}
		if (DAT_00AC1170) {
			Unadvise_Sink((IUnknown *)DAT_00AC1170, DAT_007F7590, DAT_00B77E58);
			DAT_00AC1170->V02();
		}
		if (DAT_00AC1174) {
			Unadvise_Sink(DAT_00AC1174, DAT_007F7570, DAT_00B77E5C);
			DAT_00AC1174->Release();
		}
		DAT_00AC116C = NULL;
		DAT_00AC1170 = NULL;
		DAT_00AC1174 = NULL;
		for (int i = 0; i < 18; i++) {
			CloseHandle(DAT_00B73684[i]);
		}
		FUN_007AC500(&DAT_00B76488);
		DAT_00B77E75 = 0;
	}
}

extern char DAT_00B73B34[];
extern int DAT_00B76B0C;
extern int DAT_00B77E50;

// FUNCTION: GAMEMD 0x00786270
bool FUN_00786270(void)
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
	DAT_00B77E50 = value & 0xFF;
	if (DAT_00AC116C != NULL) {
		DAT_00AC116C->V35(value);
	}
	if (DAT_00AC1170 != NULL) {
		FUN_004068E0("Get_WDT_State_Silent: WDTHost is %s, port is %d\n", DAT_00B73B34, DAT_00B76B0C);
		DAT_00AC1170->V11(DAT_00B73B34, DAT_00B76B0C, 3);
		return true;
	}
	return false;
}

extern char DAT_00B77E75;
void FUN_005E87A0(void);
void FUN_005E8B40(void);
void FUN_005E8B60(void);
namespace Game { extern HINSTANCE hInstance; }
int __fastcall FUN_00785370(HINSTANCE instance);

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
	DAT_00B77E50 = value;
	DAT_0084A234 = value + 0x2900;
	FUN_00780110();
	DAT_00B77DC0 = 1;
	for (int i = 0; i < 18; i++) {
		DAT_00B73684[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	return FUN_00785370(Game::hInstance);
}

class SimpleWonlineDialogControl : public OwnerDraw::SimpleDialogControl
{
	public:
		virtual ~SimpleWonlineDialogControl(void) {}
};

static inline void Focus_Chat_Input_After(unsigned int event)
{
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 &= ~event;
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

int __fastcall FUN_006241F0(int id, DLGPROC proc, OwnerDraw::DialogControl * control);
BOOL CALLBACK FUN_00796B90(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
rc_ptr<PlayerProfile> __fastcall FUN_00778A20(Wstring const * name);
extern char DAT_0084A238;
extern int DAT_00B77A90;
extern char DAT_00B761A8[];
extern char DAT_00B75E4C;
void __fastcall FUN_00779F10(Wstring const * value);
#include "wolopt.h"

static inline int Pick_Locale(void)
{
	FUN_007F7624_Class dialog;
	int result = FUN_006241F0(0xFE, FUN_00796B90, (OwnerDraw::DialogControl *)&dialog);
	if (result < 0) {
		return 0;
	}
	return result;
}

static inline Wstring Strip_Server_Prefix(Server * server)
{
	Wstring text(server->name);
	Wstring prefix;
	int pos = text.getToken(0, ":", prefix);
	if (pos >= 0) {
		text.remove(0, pos + 1);
	}
	return text;
}

static inline void Set_Locale_Entry(Wstring & key, int locale)
{
	int old;
	if (DAT_00B76530.remove(key, old)) {
		DAT_00B76530.add(key, locale);
	}
}

bool __fastcall FUN_007864C0(Server * server);

// FUNCTION: GAMEMD 0x00786D90
bool FUN_00786D90(void)
{
	if (DAT_00B76468 >= 1 && DAT_00B76468 < DAT_00B778D8.length()) {
		Server * server = NULL;
		DAT_00B778D8.getPointer(&server, DAT_00B76468);
		return FUN_007864C0(server);
	}
	return false;
}

extern char DAT_0084A238;

// FUNCTION: GAMEMD 0x00786DC0
bool __fastcall FUN_00786DC0(Wstring & name)
{
	DAT_00B7778C.clear();
	for (int i = 1; i <= 32; i++) {
		char const * nick;
		char const * pass;
		if (DAT_00AC116C->V36(i, &nick, &pass) == 0 && nick != NULL && strlen(nick) != 0 && strcmpi(name.text(), nick) == 0) {
			DAT_00B73E40 = nick;
			DAT_00B7778C = pass;
			DAT_0084A238 = 0;
			return true;
		}
	}
	DAT_00B73E40.clear();
	return false;
}

// FUNCTION: GAMEMD 0x00786E70
int __fastcall FUN_00786E70(Wstring & name)
{
	if (name.length()) {
		for (int i = 1; i < DAT_00B778D8.length(); i++) {
			Server * server = NULL;
			DAT_00B778D8.getPointer(&server, i);
			char * colon = strchr(server->name, ':');
			if (colon != NULL && *colon != 0 && name == colon + 1) {
				return i;
			}
		}
	}
	return -1;
}

extern long DAT_00B77DF8;
int __cdecl sprintf(char * buffer, const char * format, ...);

// FUNCTION: GAMEMD 0x00786EF0
bool FUN_00786EF0(void)
{
	bool result = false;
	long error = DAT_00B77DF8;
	if (error == 0x80040077 || error == 0x80040078 || error == 0x80040072 || error == 0x80040074 || error == 0x80040075 || error == 0x80040076) {
		result = true;
		if (ODMessageBox(DAT_00B77AD0, 4, FUN_0077D710, 0) == IDYES) {
			char url[260];
			int sku = DAT_00B77E50;
			switch (DAT_00B77DF8) {
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
			FUN_0077DC90(url, 1);
		}
	} else {
		ODMessageBox(DAT_00B77AD0, 0, FUN_0077D710, 0);
	}
	return result;
}

namespace UI {
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
}
void FUN_0072C600(void);
void FUN_0072C650(void);
void FUN_0072BCA0(void);
void FUN_0072BCF0(void);
BOOL CALLBACK FUN_00799EA0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
Wstring __fastcall FUN_00779B50(void);
Wstring __fastcall FUN_00779DD0(void);
bool FUN_00779730(void);
extern char DAT_00B77E68;
extern int DAT_00B76468;
extern long DAT_00B77DF8;
int __fastcall FUN_00789500(char const * login, char const * password, int plain, int locale);
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);


#include "_mixfile.h"
#include "_pk.h"
#include "ccfile.h"
#include "mixfile.h"
#include "worlddom.h"
#include "wdtnet.h"

extern wchar_t const * DAT_00B73E1C[8];
extern wchar_t * DAT_00B75E50;
extern int DAT_00B77DD4;
rc_ptr<WDTState> * __cdecl FUN_007B0210(void);
void __fastcall FUN_00735060(wchar_t * dest, char const * source, int limit);

static inline void Set_Lobby_Number(WolChannel & chan)
{
	DAT_00B77DD4 = atol(chan.Name + strlen("Lob_41_"));
}

// FUNCTION: GAMEMD 0x00788180
int __fastcall FUN_00788180(bool flag)
{
	int count = DAT_00B73E10.length();
	int user_cap = 25;
	WolChannel chan;
	memset(&chan, 0, sizeof(chan));
	char name[32];
	char buf[32];
	wchar_t wide_name[32];
	char def[32];
	char entry[32];
	int i;

	if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
		memset(buf, 0, sizeof(buf));
		memset(wide_name, 0, sizeof(buf));
		memset(entry, 0, sizeof(entry));
		memset(def, 0, sizeof(def));

		MixFileClass * mix = new MixFileClass("WDT.MIX", &FastKey);

		INIClass ini;
		CCFileClass file("WDTMAPMD.INI");
		ini.Load(file, false);
		sprintf(entry, "Territory%02d", Session.unknown_08);
		strcpy(def, entry);
		if (((WDTState *)FUN_007B0210()->Ptr)->MapID == 0) {
			ini.Get_String("NorthAmerica", entry, def, buf, sizeof(def));
		} else {
			ini.Get_String("Europe", entry, def, buf, sizeof(def));
		}
		ini.Get_String(buf, "Name", "A territory", name, sizeof(buf));
		FUN_00735060(wide_name, name, -1);
		DAT_00B75E50 = _wcsdup(wide_name);
		delete mix;
	}

	Wstring lobby = Wstring("Lob_41_") + Wstring_FromInt(DAT_00B77DD4, 1);

	for (i = 0; i < count; i++) {
		DAT_00B73E10.get(chan, i);
		if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
			if (wcscmp(DAT_00B73E1C[i], wide_name) == 0) {
				if (!flag || chan.CurrentUsers <= user_cap) {
					return i;
				}
			} else {
				continue;
			}
		}
		Wstring other(chan.Name);
		if (other == lobby) {
			if (!flag || chan.CurrentUsers <= user_cap) {
				return i;
			}
		}
	}

	int best = -1;
	unsigned int fewest = 10000;
	for (i = 0; i < count; i++) {
		DAT_00B73E10.get(chan, i);
		if (Session.Type == GAME_INTERNET && Session.unknown_0C == 3) {
			if (wcsncmp(DAT_00B73E1C[i], wide_name, wcslen(wide_name)) == 0) {
				if (!flag || chan.CurrentUsers <= user_cap) {
					Set_Lobby_Number(chan);
					return i;
				}
				if ((unsigned int)chan.CurrentUsers < fewest) {
					best = i;
					fewest = chan.CurrentUsers;
				}
			} else {
				continue;
			}
		}
		if (strncmp(chan.Name, "Lob_41_", strlen("Lob_41_")) == 0) {
			if (!flag || chan.CurrentUsers <= user_cap) {
				Set_Lobby_Number(chan);
				return i;
			}
			if ((unsigned int)chan.CurrentUsers < fewest) {
				best = i;
				fewest = chan.CurrentUsers;
			}
		}
	}

	if (best != -1) {
		DAT_00B73E10.get(chan, best);
		DAT_00B77DD4 = atol(chan.Name + strlen("Lob_41_"));
	}
	return best;
}
void __fastcall FUN_00779C90(Wstring const * value);
extern char DAT_00889F64[];
extern char DAT_0084A238;
int __fastcall FUN_00797640(int sku);
BOOL CALLBACK FUN_0079AE00(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
int FUN_00787010(void);

void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
int __fastcall FUN_006241F0(int id, DLGPROC proc, OwnerDraw::DialogControl * control);
BOOL CALLBACK FUN_0078E030(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
namespace Unsorted { extern int MuteSWLaunches; }
extern int DAT_007EEF28;
extern char DAT_00B77E30[];
bool __fastcall FUN_0069BBE0(SessionClass * session);
wchar_t * __fastcall FUN_00735120(char const * text);
void __cdecl FUN_005E2700(int color, wchar_t const * text, ...);
extern int DAT_007EEF2C;
void __cdecl FUN_005E2790(int color, wchar_t const * text, ...);
extern int DAT_00B76C20;
extern int DAT_00B77E90;
bool __fastcall FUN_0079BA60(WolChannel * chan, bool flag, int extra);

// FUNCTION: GAMEMD 0x00788710
bool __fastcall FUN_00788710(int id, int flag)
{
	if (DAT_00B77DD0) {
		FUN_004068E0("Leave_Channel, channel %s type %d\n", DAT_00B76B10.Name, DAT_00B76B10.Type);
		DAT_00AC116C->V09();
		FUN_00792BE0(id, StringTable::LoadString("WOL:LeavingChannel", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x29D0), flag);
		int tries = 0;
		while (DAT_00B77DD0 && tries < 150) {
			Sleep(100);
			FUN_0077F900();
			tries++;
		}
		Focus_Chat_Input_After(0x40);
	}
	return DAT_00B77DD0 == 0;
}

void __fastcall FUN_00792BE0(int id, WideWstring const & text, char flag);

// FUNCTION: GAMEMD 0x00788800
long __fastcall FUN_00788800(WolChannel * chan, Wstring & key, WideWstring text)
{
	if (strcmp(chan->Name, DAT_00B76B10.Name) == 0 || strcmp(chan->Name, DAT_00B73BE8.Name) == 0) {
		return 0;
	}
	if (!FUN_00788710(0x40, 1)) {
		return 1;
	}
	strcpy(chan->Key, key.text());
	DAT_00B73BE8 = *chan;
	FUN_00792BE0(2, text.text(), 0);
	FUN_004068E0("Requesting channel join %s\n", chan->Name);
	return DAT_00AC116C->V08(chan);
}

void FUN_00781480(void);

// FUNCTION: GAMEMD 0x00788A30
int __fastcall FUN_00788A30(bool flag)
{
	int lobby = FUN_00788180(flag);
	if (lobby != -1) {
		WolChannel chan;
		DAT_00B73E10.get(chan, lobby);
		long result;
		{
			Wstring key("zotclot9");
			result = FUN_00788800(&chan, key, WideWstring(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2A01)));
		}
		if (result == 0) {
			HWND dialog = FUN_00775940(0x113);
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

namespace Game { extern HWND hWnd; }
HWND WS_Create_Dialog(HINSTANCE instance, int id, HWND parent, DLGPROC proc, BOOL force_show);
namespace UI { void __fastcall RegisterWindow(HWND window, int id); }
namespace OwnerDraw {
	void __fastcall DrawCampaignMenuTransition(HWND window, bool reverse);
}
void FUN_0072C240(void);
void FUN_0072C290(void);
void FUN_0072C420(void);
void FUN_0072C470(void);
BOOL CALLBACK FUN_0078E230(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK FUN_0078E950(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
extern int DAT_00B73A1C;
extern int DAT_00B73AB8;
extern int DAT_00B7782C;
extern int DAT_00B76228;
extern long DAT_00B77DF0;
extern int DAT_00B77DF4;
extern char DAT_00889F64[];

struct NetUtilAccount {
	virtual void S00(void);
	virtual void S01(void);
	virtual void S02(void);
	virtual void S03(void);
	virtual void S04(void);
	virtual void S05(void);
	virtual void S06(void);
	virtual void S07(void);
	virtual void S08(void);
	virtual long __stdcall V09(char const * nick, char const * password, char const * email, char const * parent_email, int newsletter, int shareinfo);
	virtual long __stdcall V10(int month, int day, int year, char const * email);
};

static inline void Set_Item_Text(HWND dialog, int id, char const * text)
{
	HWND item = GetDlgItem(dialog, id);
	SendMessage(item, 0x4B4, 0, (LPARAM)text);
}

// FUNCTION: GAMEMD 0x00788B00
void __fastcall FUN_00788B00(HWND win)
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
		ResetEvent(DAT_00B73684[i]);
	}
	memset(nick, 0, sizeof(nick));
	memset(pass, 0, sizeof(pass));
	memset(verify, 0, sizeof(verify));
	memset(email, 0, sizeof(email));
	memset(parent_email, 0, sizeof(parent_email));
	FUN_00792BE0(0x80, StringTable::LoadString("WOL:NewAccountFetch", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2A4A), 0);
	FUN_004068E0("Newnick update check\n");
	DAT_00AC116C->V04(DAT_0084A234, 0x10001, "TibSun", "TibPass99", 0x23);
	do {
		wait = WaitForMultipleObjects(18, DAT_00B73684, FALSE, 100);
		while (wait == WAIT_TIMEOUT) {
			FUN_0077F900();
			wait = WaitForMultipleObjects(18, DAT_00B73684, FALSE, 100);
		}
		ResetEvent(DAT_00B73684[wait]);
	} while (wait != 0 && wait != 1);
	Focus_Chat_Input_After(0xFFFFFFFF);
	if (wait == 0) {
		ShowWindow(win, SW_SHOW);
		OwnerDraw::SetNeedsControlImage(win, 0);
		return;
	}
	if (wait == 8) {
		SetEvent(DAT_00B73684[8]);
		ShowWindow(win, SW_SHOW);
		OwnerDraw::SetNeedsControlImage(win, 0);
		return;
	}

first_nick_retry:
	FUN_0072C240();
	FUN_0052FEC0(NULL, true);
	dlg = WS_Create_Dialog(Game::hInstance, 0xE7, Game::hWnd, FUN_0078E230, FALSE);
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
	DAT_00B77E64 = 0;
	DAT_00B77E60 = -1;
	if (((NetUtilAccount *)DAT_00AC1170)->V10(month, day, year, email) != 0) {
		ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ABB), 0, FUN_0077D710, 0);
		goto pick_dialog;
	}
	FUN_00792BE0(0x100, StringTable::LoadString("WOL:RequestingNickname", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ABE), 1);
	if (DAT_00B77E60 < 0) {
		if (DAT_00B77DF4 == 0x80040100) {
			ODMessageBox(StringTable::LoadString("TXT_REQUIRED_FIELD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AC2), 0, FUN_0077D710, 0);
		} else {
			ODMessageBox(StringTable::LoadString("TXT_CANT_CONNECT", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2AC4), 0, FUN_0077D710, 0);
		}
		goto first_nick_retry;
	}
	if (DAT_00B77E60 < 13 && DAT_00B77E64 == 0) {
		if (ODMessageBox(StringTable::LoadString("TXT_CONSENT_REQUIRED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2ACD), 4, FUN_0077D710, 0) == IDYES) {
			char url[256];
			sprintf(url, "http://apiregister.westwood.com/consent_form/index_%d.html", DAT_00B77E50);
			FUN_0077DC90(url, 1);
		}
		goto first_nick_retry;
	}
pick_dialog:
	FUN_0072C420();
	if (DAT_00B77E60 < 13) {
		dialog_id = 0xE6;
	} else {
		dialog_id = (DAT_00B77E60 >= 18) + 0xF3;
	}
	dlg = WS_Create_Dialog(Game::hInstance, dialog_id, Game::hWnd, FUN_0078E950, FALSE);
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
	if (((NetUtilAccount *)DAT_00AC1170)->V09(nick, pass, email, parent_email, newsletter, shareinfo) != 0) {
		goto cant_connect;
	}
	FUN_00792BE0(0x100, StringTable::LoadString("TXT_REQ_NICK", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2B4D), 1);
	if (DAT_00B77DF0 >= 0) {
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
	OwnerDraw::SetNeedsControlImage(win, 0);
	FUN_0072C470();
	} else {
		ShowWindow(win, SW_SHOWNORMAL);
		ShowWindow(win, SW_SHOW);
		OwnerDraw::SetNeedsControlImage(win, 0);
		FUN_0072C290();
	}
}

// FUNCTION: GAMEMD 0x00789500
int __fastcall FUN_00789500(char const * login, char const * password, int plain, int locale)
{
	char const * nick;
	char const * pass;
	int nicknum = 0;
	int i;
	for (i = 1; i <= 32; i++) {
		nick = NULL;
		pass = NULL;
		if (DAT_00AC116C->V36(i, &nick, &pass) == 0 && nick != NULL && strcmp(nick, login) == 0) {
			nicknum = i;
			break;
		}
	}
	if (nicknum == 0) {
		for (i = 1; i <= 32; i++) {
			nick = NULL;
			pass = NULL;
			if (DAT_00AC116C->V36(i, &nick, &pass) != 0 || nick == NULL || strlen(nick) == 0) {
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
		DAT_00AC116C->V37(nicknum, login, password, (unsigned char)plain);
		DAT_00AC116C->V50(nicknum, locale);
	}
	return nicknum;
}

// FUNCTION: GAMEMD 0x00789630
Wstring __fastcall FUN_00789630(Server * server)
{
	Wstring name(server->name);
	Wstring head;
	int pos = name.getToken(0, ":", head);
	if (pos >= 0) {
		name.remove(0, pos + 1);
	}
	return name;
}

#include "wolopt.h"

rc_ptr<PlayerProfile> __fastcall FUN_00778A20(Wstring const * name);
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
	if (DAT_00B778D8.length() == 1) {
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
		profile = FUN_00778A20(&DAT_00B73E40);
	} else {
		profile = DAT_00B75E48;
	}
	if (profile.Get()) {
		preferred = profile->Name;
	}
	EnableWindow(win, TRUE);
	int selected = -1;
	for (int i = 1; i < DAT_00B778D8.length(); i++) {
		Server * server = NULL;
		DAT_00B778D8.getPointer(&server, i);
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
				DAT_00B76468 = i;
				DAT_00B779CC = i;
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
bool __fastcall FUN_00789960(WolChannel * chan, int extra)
{
	if (!FUN_0079BA60(chan, true, extra)) {
		return false;
	}
	unsigned int mask = chan->Reserved;
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
	if (chan->Squad & 0x100) {
		FUN_0052FEC0(NULL, true);
		WonlineStringDialogControl dialog;
		if (FUN_006241F0(0xE1, FUN_0078E030, &dialog) == IDCANCEL) {
			return false;
		}
		name = dialog.Text;
		name.replace(" ", "_");
	}
	return FUN_00788800(chan, name, WideWstring(StringTable::LoadString("TXT_JOINLOB", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2D1B))) == 0;
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
					strcpy(user.Nick, FUN_007350C0(name));
					DAT_00AC116C->V62(&user, cursor);
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
					strcpy(user.Nick, DAT_00B77E30);
					DAT_00AC116C->V62(&user, cursor);
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
void __cdecl FUN_005E2790(int color, wchar_t const * text, ...);
extern int DAT_00B77E8C;
extern int DAT_007EEF44;
extern wchar_t DAT_0084B94C[];
extern char DAT_0082BFAC[];

// FUNCTION: GAMEMD 0x0078D120
BOOL CALLBACK FUN_0078D120(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
						if (DAT_00AC116C) {
							User userL;
							memset(&userL, 0, sizeof(userL));
							strcpy(userL.Nick, name.get());
							DAT_00AC116C->V24(&userL);
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
						if (DAT_00AC116C) {
							HWND item = GetDlgItem(window, 0x5C1);
							if (item) {
								char name[10];
								SendMessage(item, 0x4B5, 10, (LPARAM)name);
								name[9] = 0;
								memset(&user, 0, sizeof(user));
								strcpy(user.Nick, name);
								DAT_00AC116C->V24(&user);
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
					if (!DAT_00AC116C) {
						return FALSE;
					}
					{
						Wstring name;
						FUN_00623E40(GetDlgItem(window, 0x5C1), name);
						name.truncate((unsigned int)9);
						User userL;
						strcpy(userL.Nick, name.get());
						if (text.length() > 0 && strlen(userL.Nick) != 0) {
							bool muted = false;
							if (Unsorted::MuteSWLaunches) {
								for (int i = 0; i < Session.Players.Count(); i++) {
									NodeNameType * who = Session.Players[i];
									if (stricmp(FUN_007350C0(who->Name), userL.Nick) == 0) {
										muted = true;
										break;
									}
								}
							}
							if (muted) {
								FUN_005E2790(-1, StringTable::LoadString("TXT_OBSERVER_CANT_PAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3746));
							} else {
								DAT_00AC116C->V62(&userL, text.get());
								wchar_t * body = text.get();
								FUN_005E2790(DAT_007EEF44, DAT_0084B94C, DAT_00B73E40.c_str(), body);
							}
						} else if (strlen(userL.Nick) == 0) {
							FUN_005E2790(-1, StringTable::LoadString("TXT_NAME_ERROR", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3752));
						} else {
							FUN_005E2790(-1, StringTable::LoadString("TXT_ENTER_MESSAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3754));
						}
					}
					return FALSE;
				}

				case 0x607:
					if (DAT_00AC116C) {
						if (DAT_00B77DE4 == 0) {
							FUN_005E2790(-1, StringTable::LoadString("TXT_NOSQUAD", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3760));
							return FALSE;
						}
						WideWstring text;
						HWND input = GetDlgItem(window, 0x540);
						FUN_00623EA0(input, &text);
						SendMessage(input, 0x4B2, 0, (LPARAM)DAT_00887734);
						SetFocus(input);
						strcpy(user.Nick, DAT_0082BFAC);
						if (text.length() > 0) {
							DAT_00AC116C->V62(&user, text.get());
						} else {
							FUN_005E2790(-1, StringTable::LoadString("TXT_ENTER_MESSAGE", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x3773));
						}
						return FALSE;
					}
					break;

				case 0x5C3:
					if (DAT_00AC116C) {
						char name[64];
						memset(name, 0, sizeof(name));
						SendDlgItemMessage(window, 0x5C1, 0x4B5, 60, (LPARAM)name);
						name[9] = 0;
						strcpy(user.Nick, name);
						if (strlen(user.Nick) != 0) {
							DAT_00AC116C->V24(&user);
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
				if (DAT_00AC116C) {
					char name[10];
					SendMessage((HWND)lparam, 0x4B5, 10, (LPARAM)name);
					name[9] = 0;
					memset(&user, 0, sizeof(user));
					strcpy(user.Nick, name);
					DAT_00AC116C->V24(&user);
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
BOOL CALLBACK FUN_0078DA30(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
					((DownloadObject *)DAT_00AC1174)->Abort();
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
			((DownloadObject *)DAT_00AC1174)->Abort();
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

void __fastcall FUN_00623E40(HWND window, Wstring & text);
extern char DAT_00889F64[];
extern int DAT_00B77DC4;

class WonlineGameControl : public SimpleWonlineDialogControl
{
	public:
		Wstring Text;
		bool Flag10;
		bool Flag11;
		bool Flag12;
		bool Flag13;
};

// FUNCTION: GAMEMD 0x0078DB70
BOOL CALLBACK FUN_0078DB70(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		SetWindowLong(window, 8, ((int *)lparam)[1]);
	}
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc) {
		return rc;
	}
	WonlineGameControl * control = (WonlineGameControl *)GetWindowLong(window, 8);
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
						DAT_00B77DC4 = 7;
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
				SetFocus(GetDlgItem(window, 0x72C));
				CheckDlgButton(window, 0x629, control->Flag10 != 0);
				CheckDlgButton(window, 0x66C, control->Flag11 != 0);
				if (!(Session.Type == GAME_INTERNET && Session.unknown_0C == 3) && DAT_00B77DE4 < 1) {
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
BOOL CALLBACK FUN_0078E030(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
				SetFocus(GetDlgItem(window, 0x72C));
			}
			break;

		case 0x4DA:
			PostMessage(GetDlgItem(window, 1), 0xF5, 0, 0);
			break;
	}
	return FALSE;
}

int __fastcall FUN_0060D320(HWND dialog);
bool __fastcall Focus_Next_Tab_Item(HWND window, bool previous);

extern int DAT_00B73A1C;
extern int DAT_00B73AB8;
extern int DAT_00B7782C;
extern int DAT_00B76228;

// FUNCTION: GAMEMD 0x0078E230
BOOL CALLBACK FUN_0078E230(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
					FUN_0077DC90("http://privacypolicy.westwood.com", 0);
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
BOOL CALLBACK FUN_0078E950(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
				SetFocus(GetDlgItem(window, 0x628));
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
namespace Game { extern bool IsActive; }
namespace OwnerDraw {
	void __fastcall DrawCampaignMenuTransition(HWND window, bool reverse);
	void __stdcall SetNeedsControlImage(HWND window, int value);
}
void __fastcall FUN_0052FEC0(Surface * surface, bool flag);
void FUN_0072B8E0(void);
void FUN_0072B930(void);
void __fastcall FUN_006B6230(void * unused);

// FUNCTION: GAMEMD 0x0078EBC0
BOOL CALLBACK FUN_0078EBC0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
			Game::IsActive = true;
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
					Game::IsActive = false;
					FUN_0072B8E0();
					FUN_006B6230(&lparam);
					ShowWindow(window, SW_SHOW);
					OwnerDraw::SetNeedsControlImage(window, 0);
					FUN_0072B930();
					return FALSE;
			}
			break;

		case 0x4ED:
			ShowWindow(window, SW_HIDE);
			OwnerDraw::SetNeedsControlImage(window, 1);
			return FALSE;

		case 0x497:
				if (DAT_00B76234) {
					SendDlgItemMessage(window, 0x5F5, 0xF1, 1, 0);
				}
				if (DAT_00B779F0) {
					SendDlgItemMessage(window, 0x5F7, 0xF1, 1, 0);
				}
				if (DAT_00B779BC) {
					SendDlgItemMessage(window, 0x5F6, 0xF1, 1, 0);
				}
				if (DAT_00B7690C) {
					SendDlgItemMessage(window, 0x5F8, 0xF1, 1, 0);
				}
				if (DAT_00B77784) {
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
extern int DAT_00B76440[];
extern wchar_t DAT_00B75F6C[];

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
	return id == 0x75B ? 7 : -1;
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
		if (DAT_00AC116C->V09() < 0) {
			FUN_007757E0(window, 2);
		}
	}
}

namespace Game { extern HWND hWnd; }
HWND WS_Create_Dialog(HINSTANCE instance, int id, HWND parent, DLGPROC proc, BOOL force_show);
void FUN_0072CBA0(void);
namespace UI { void __fastcall RegisterWindow(HWND window, int id); }
BOOL CALLBACK FUN_00792CF0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00792BE0
void __fastcall FUN_00792BE0(int id, WideWstring const & text, char flag)
{
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 |= id;
		if (text.length() > 0) {
			HWND list = GetDlgItem(dialog, 0x5B2);
			SendMessage(list, 0x4B2, 0, (LPARAM)text.text());
		}
	} else if (id) {
		HWND top = FUN_00775B10();
		HWND input = GetDlgItem(top, 0x540);
		FUN_0072CBA0();
		dialog = WS_Create_Dialog(Game::hInstance, 0xC4, Game::hWnd, FUN_00792CF0, FALSE);
		UI::RegisterWindow(dialog, 0xC4);
		HWND list = GetDlgItem(dialog, 0x5B2);
		SendMessage(list, 0x4B2, 0, (LPARAM)text.text());
		DAT_00B77DC8 = id;
		if (input) {
			SetFocus(input);
		}
	}
	if (flag && DAT_00B77DC8) {
		FUN_007759E0(dialog, (bool (*)(void))FUN_0077D710, 0, 1, 0);
	}
}

extern int DAT_0084A248;
void __fastcall FUN_00608440(HWND window);

// FUNCTION: GAMEMD 0x00792CF0
BOOL CALLBACK FUN_00792CF0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
					SetEvent(DAT_00B73684[0]);
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
void __fastcall FUN_00792E90(wchar_t const * text)
{
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		SendDlgItemMessage(dialog, 0x5B2, 0x4B2, 0, (LPARAM)text);
	}
}

// FUNCTION: GAMEMD 0x00792EC0
void __fastcall FUN_00792EC0(unsigned int mask)
{
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 &= ~mask;
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

#include "wsproto.h"

extern int DAT_00B7790C[];
extern WinsockInterfaceClass * PacketTransport;
extern User DAT_00B777B0;
NodeNameType * __fastcall FUN_005E8BF0(wchar_t const * name);
User * FUN_007931A0(void);

// FUNCTION: GAMEMD 0x00792F10
void __fastcall FUN_00792F10(User * unused)
{
	Session.NumPlayers = 1;
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		User user;
		DAT_00B77900.get(user, i);
		NodeNameType * node = FUN_005E8BF0(FUN_00735120(user.Nick));
		if (node && FUN_005E93F0(node)) {
			node->Player.SquadID = user.Squad;
		}
		if (strcmpi(user.Nick, DAT_00B73E40.get()) != 0) {
			if (user.Flags & 1) {
				NodeNameType * host = FUN_005E8BF0(FUN_00735120(user.Nick));
				NetNumType net;
				NetNodeType addr;
				host->Address.Get_Address(net, addr);
				Session.HostAddress.Set_Address(net, addr, 0);
			}
			int ping = 0;
			DAT_00AC1170->V08(user.IPAddr, &ping);
			if (ping < 0) {
				ping = 0;
			}
			DAT_00B7790C[Session.NumPlayers] = ping;
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
bool FUN_007930A0(void)
{
	User * host = FUN_007931A0();
	if (!host) {
		return false;
	}
	if (strcmpi(host->Nick, DAT_00B73E40.text()) != 0) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x00793120
bool __fastcall FUN_00793120(Wstring & name)
{
	User * host = FUN_007931A0();
	if (!host) {
		return false;
	}
	if (strcmpi(host->Nick, name.text()) != 0) {
		return false;
	}
	return true;
}

// FUNCTION: GAMEMD 0x007931A0
User * FUN_007931A0(void)
{
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		DAT_00B77900.get(DAT_00B777B0, i);
		if (DAT_00B777B0.Flags & 1) {
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
		HWND dialog = FUN_00775940(0x130);
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
	HWND dialog = WS_Create_Dialog(Game::hInstance, 0x130, Game::hWnd, FUN_00793280, FALSE);
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
	HWND dialog = FUN_00775940(0x130);
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

	User * host = FUN_007931A0();
	if (!host) {
		return;
	}
	int host_slot = -1;
	for (int i = 0; i < Session.Players.Count(); i++) {
		if (stricmp(FUN_007350C0(Session.Players[i]->Name), host->Nick) == 0) {
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
unsigned short __fastcall FUN_00793710(int send_port, int mangled_port, int unused, int port_spread)
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

struct ConnectionRanker {
	int FUN_005DA040(int flags);
	int FUN_005DA080(int flags);

	int Behavior;
};
extern ConnectionRanker DAT_00ABFEC8;

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
			int cur_value = Session.unknown_29F4[order[b]];
			int next_value = Session.unknown_29F4[order[b + 1]];
			if (DAT_00ABFEC8.FUN_005DA040(cur_value) < DAT_00ABFEC8.FUN_005DA040(next_value)) {
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
	for (int j = 0; j < Session.unknown_2E14; j++) {
		if (Session.unknown_2A14[j][0] == from_slot && Session.unknown_2A14[j][1] == to_slot) {
			Session.unknown_2C14[j][0] = state;
			return;
		}
		if (Session.unknown_2A14[j][0] == to_slot && Session.unknown_2A14[j][1] == from_slot) {
			Session.unknown_2C14[j][1] = state;
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
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		DAT_00B77900.getPointer(&user, i);
		if (stricmp(name, user->Nick) == 0) {
			text[0] = 'Z';
			text[1] = '4';
			sprintf(text + 2, "%d", port);
			FUN_004068E0("Sending string %s to user %s\n", text, name);
			DAT_00AC116C->V14(user, text);
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
	DAT_00AC116C->V15(text);
}

// FUNCTION: GAMEMD 0x00793CD0
void __fastcall FUN_00793CD0(int slot)
{
	Session.unknown_2F6C[Session.unknown_2FEC] = slot;
	Session.unknown_2F8C[Session.unknown_2FEC] = 1200;
	Session.unknown_2FEC++;
}

#include <snmp.h>

struct tConnInfoStruct {
	unsigned int State;
	unsigned long LocalIP;
	unsigned short LocalPort;
	unsigned long RemoteIP;
	unsigned short RemotePort;
};

extern BOOL (__stdcall * DAT_00B7792C)(DWORD uptime, HANDLE * trap_event, AsnObjectIdentifier * first_region);
extern BOOL (__stdcall * DAT_00B76678)(BYTE pdu_type, SnmpVarBindList * bind_list, AsnInteger32 * error_status, AsnInteger32 * error_index);
extern LPVOID (__stdcall * DAT_00B778F0)(DWORD bytes);
extern VOID (__stdcall * DAT_00B77AC0)(LPVOID mem);
hostent * __stdcall FUN_007C8980(char const * name);
int __stdcall FUN_007C897A(void);

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

	if (DAT_00B76468 < 1 || DAT_00B76468 >= DAT_00B778D8.length()) {
		return(false);
	}

	FUN_004068E0("Finding local address used to talk to the chat server\n");

	Server * server = NULL;
	DAT_00B778D8.getPointer(&server, DAT_00B76468);
	memset(server_name, 0, sizeof(server_name));
	strncpy(server_name, server->conndata + 4, sizeof(server_name));

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

	DAT_00B7792C = (BOOL (__stdcall *)(DWORD, HANDLE *, AsnObjectIdentifier *))GetProcAddress(mib_ii_dll, "SnmpExtensionInit");
	DAT_00B76678 = (BOOL (__stdcall *)(BYTE, SnmpVarBindList *, AsnInteger32 *, AsnInteger32 *))GetProcAddress(mib_ii_dll, "SnmpExtensionQuery");
	DAT_00B778F0 = (LPVOID (__stdcall *)(DWORD))GetProcAddress(snmpapi_dll, "SnmpUtilMemAlloc");
	DAT_00B77AC0 = (VOID (__stdcall *)(LPVOID))GetProcAddress(snmpapi_dll, "SnmpUtilMemFree");
	if (DAT_00B7792C == NULL || DAT_00B76678 == NULL || DAT_00B778F0 == NULL || DAT_00B77AC0 == NULL) {
		FUN_004068E0("Failed to get proc addresses for linked functions\n");
		FreeLibrary(snmpapi_dll);
		FreeLibrary(mib_ii_dll);
		return(false);
	}

	SnmpVarBindList * bind_list_ptr = (SnmpVarBindList *)DAT_00B778F0(sizeof(SnmpVarBindList));
	SnmpVarBind * bind_ptr = (SnmpVarBind *)DAT_00B778F0(sizeof(SnmpVarBind));

	FUN_004068E0("About to init INETMIB1.DLL\n");
	int ok = DAT_00B7792C(GetTickCount(), &trap_handle, &first_supported_region);

	if (!ok) {
		FUN_004068E0("Failed to init the .dll\n");
		DAT_00B77AC0(bind_list_ptr);
		DAT_00B77AC0(bind_ptr);
		FreeLibrary(snmpapi_dll);
		FreeLibrary(mib_ii_dll);
		return(false);
	}

	unsigned int mib_ii_name[] = {1,3,6,1,2,1,6,13,1,1};
	unsigned int * mib_ii_name_ptr = (unsigned int *)DAT_00B778F0(sizeof(mib_ii_name));
	memcpy(mib_ii_name_ptr, mib_ii_name, sizeof(mib_ii_name));

	conn_entry_type_index = ARRAY_SIZE(mib_ii_name) - 1;

	bind_ptr->name.idLength = ARRAY_SIZE(mib_ii_name);
	bind_ptr->name.ids = mib_ii_name;
	bind_list_ptr->list = bind_ptr;
	bind_list_ptr->len = 1;

	last_field = 1;
	index = 0;

	while (true) {

		if (!DAT_00B76678(SNMP_PDU_GETNEXT, bind_list_ptr, &error_status, &error_index)) {
			FUN_004068E0("SnmpExtensionQuery returned false\n");
			DAT_00B77AC0(bind_list_ptr);
			DAT_00B77AC0(bind_ptr);
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

	DAT_00B77AC0(bind_list_ptr);
	DAT_00B77AC0(bind_ptr);
	DAT_00B77AC0(mib_ii_name_ptr);

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

#include "conquer.h"
#include "globals.h"
#include "wspudp.h"

extern bool DAT_00B77E2C;

// FUNCTION: GAMEMD 0x00796710
bool FUN_00796710(void)
{
	GlobalPacketType packet;
	int packet_len;
	unsigned short product_id;
	IPXAddressClass sender_address;

	if (DAT_00B76B10.Type != 0 && DAT_00B77900.length() >= 2 && !FUN_007930A0() && !DAT_00B77E28 && Ipx.Global_Num_Receive()) {
		Call_Back();
		packet_len = sizeof(packet);
		if (Ipx.Global_Channel_Op(0, (int)&packet, (int)&packet_len, (int)&sender_address, (int)&product_id, 0) && packet.Command == NET_PREVIEW_MODE) {
			HWND dialog = FUN_00775940(0xC9);
			if (dialog) {
				ShowWindow(GetDlgItem(dialog, 0x468), SW_SHOWNORMAL);
			}
			User user;
			DAT_00B77900.get(user, 0);
			user.Next = NULL;
			FUN_00792F10(&user);
			if (Ipx.Get_Global_Message(&packet, &packet_len, &sender_address, &product_id) && packet.Command == NET_PREVIEW_MODE && sender_address.FUN_0053EF30(Session.HostAddress) && product_id == IPXGlobalConnClass::COMMAND_AND_CONQUER4) {
				User * host = FUN_007931A0();
				if (host && stricmp(packet.Name, host->Nick) == 0) {
					FUN_004068E0("Received NET_PREVIEW_MODE packet from host\n");
					Ipx.Set_Timing(50, -1, 5000);
					DAT_00B77E2C = true;
					Receive_Random_Map_Preview();
					DAT_00B77E2C = false;
					Ipx.Set_Timing(30, -1, 600);
					return true;
				}
			}
		}
	}
	return false;
}

namespace OwnerDraw {
char * __fastcall WideToCharString(char * dest, wchar_t const * src, int count);
}

static inline bool Is_Local_Host(void)
{
	return FUN_007930A0();
}

static inline int Current_Channel_Type(void)
{
	return DAT_00B76B10.Type;
}

// FUNCTION: GAMEMD 0x00796960
void FUN_00796960(void)
{
	if (Session.Type != GAME_INTERNET) {
		IPXAddressClass address;
		if (DAT_00B77900.length() >= 2 && !Is_Local_Host() && !DAT_00B77E28 && Current_Channel_Type() != 0) {
			User user;
			DAT_00B77900.get(user, 0);
			user.Next = NULL;
			FUN_00792F10(&user);
			Call_Back();
			Ipx.Set_Timing(50, -1, 5000);
			FUN_004068E0("Poke_The_Host - Host address is %s\n", Session.HostAddress.As_String(NULL));
			GlobalPacketType packet;
			memset(&packet, 0, sizeof(packet));
			packet.Command = NET_PING;
			OwnerDraw::WideToCharString(packet.Name, Session.Players[0]->Name, -1);
			User * host = FUN_007931A0();
			if (host) {
				Ipx.Send_Global_Message_To_Name(&packet, sizeof(packet), 1, FUN_00735120(host->Nick));
			}
			Call_Back();
		}
	}
}

// FUNCTION: GAMEMD 0x00796B20
void FUN_00796B20(void)
{
}

BOOL CALLBACK FUN_00796B90(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00796B30
int FUN_00796B30(void)
{
	SimpleWonlineDialogControl dialog;
	int result = FUN_006241F0(0xFE, FUN_00796B90, &dialog);
	if (result < 0) {
		return 0;
	}
	return result;
}

void __fastcall FUN_006223C0(HWND window);

// FUNCTION: GAMEMD 0x00796B90
BOOL CALLBACK FUN_00796B90(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		do SetWindowLong(window, 8, ((int *)lparam)[1]); while (0);
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
				DAT_00AC116C->V52(&count);
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
	DAT_00B7790C[Session.NumPlayers] = 100;
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
	DAT_00B7790C[Session.NumPlayers] = 100;
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
	DAT_00B7790C[Session.NumPlayers] = 100;
	Session.NumPlayers++;
	Session.HostAddress = who->Address;
}

namespace Game {
	namespace Network {
		extern int WOLGameID;
		extern int Tournament;
	}
	extern int Seed;
	extern int TechLevel;
}
extern int DAT_00B77D6C;
extern char DAT_00B77830[];
extern unsigned char DAT_00B77A04;
extern unsigned char DAT_00B76568;
extern int DAT_00841F34;
extern int DAT_00841F38;
extern int DAT_00841F3C;
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
	DAT_00B77D6C = DAT_0084A234;
	Game::Network::Tournament = 0;
	strcpy(DAT_00B77830, DAT_00B73E40.get());
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
	DAT_00B77E28 = 2;
	FUN_004068E0("Starting a %d player game.\n", Session.NumPlayers);
	FUN_00735060(Session.MasterPlayerName, "PotatoToo", -1);
	Session.MasterPlayerID = -1;
	Session.House = 0;
	Game::Seed = 1;
	Game::TechLevel = 10;
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
		if (DAT_00B7790C[q] > worst) {
			worst = DAT_00B7790C[q];
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
	DAT_00B77A04 = 0;
	DAT_00B76568 = 0;
	FUN_005E7460();
}

int __fastcall FUN_00797640(int sku);

// FUNCTION: GAMEMD 0x007975E0
void FUN_007975E0(void)
{
	{
		WideWstring text(StringTable::LoadString("TXT_FETCHING_SERVLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5EEC));
		FUN_00792BE0(0x10, text, 0);
	}
	int sku = 0x2300;
	if (Session.unknown_0C != 3) {
		sku = 0x2900;
	}
	FUN_00797640(sku);
}

extern int DAT_00B77DE8;

// FUNCTION: GAMEMD 0x00797640
int __fastcall FUN_00797640(int sku)
{
	char saved = DAT_00B77DC0;
	bool servers;
	bool done;
	DWORD index;
	int result;

	int version = DAT_00B77E50 | sku;
	DAT_00B77DC0 = 1;
	DAT_00B778D8.setEmpty();
	DAT_00B76468 = 0;
	DAT_00B779CC = 0;
	DAT_00AC116C->V04(version, 0x10001, "TibSun", "TibPass99", 0x1E);
	DAT_00AC116C->V35(DAT_0084A234);
	done = false;
	servers = false;
	while (true) {
		while ((index = WaitForMultipleObjects(18, DAT_00B73684, FALSE, 50)) == WAIT_TIMEOUT) {
			FUN_0077F900();
		}
		ResetEvent(DAT_00B73684[index]);
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
			switch (DAT_00B77DE8) {
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
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 = 0;
		FUN_007757E0(dialog, 0);
	}
	HWND top = FUN_00775B10();
	HWND item = GetDlgItem(top, 0x540);
	if (item) {
		SetFocus(item);
	}
	DAT_00B77DC0 = saved;
	return result;
}

extern char DAT_00B77E10;

// FUNCTION: GAMEMD 0x00797C20
void FUN_00797C20(void)
{
	if (DAT_00B77E10) {
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

	DAT_00B73BD8.clear();
	DAT_00B736F8.clear();
	DAT_00B76B00.clear();
	DAT_00B76510.clear();
	DAT_00B73AE8.clear();
	DAT_00B77820.clear();
	DAT_00B73708.clear();
	DAT_00B76188.clear();

	DAT_00B73BD8.addHead(_sound_names[0]);
	DAT_00B73BD8.addHead(_sound_names[1]);
	DAT_00B73BD8.addHead(_sound_names[2]);
	DAT_00B76B00.addHead(_sound_names[3]);
	DAT_00B76B00.addHead(_sound_names[4]);
	DAT_00B76B00.addHead(_sound_names[5]);
	DAT_00B736F8.addHead(_sound_names[6]);
	DAT_00B736F8.addHead(_sound_names[7]);
	DAT_00B736F8.addHead(_sound_names[8]);
	DAT_00B76510.addHead(_sound_names[9]);
	DAT_00B76510.addHead(_sound_names[10]);
	DAT_00B76510.addHead(_sound_names[11]);
	DAT_00B73AE8.addHead(_sound_names[12]);
	DAT_00B73708.addHead(_sound_names[13]);
	DAT_00B77820.addHead(_sound_names[14]);
	DAT_00B76188.addHead(_sound_names[15]);

	DAT_00B77E10 = 1;
}

// SYNTHETIC: GAMEMD 0x00798120 SYMBOL
// _$E366
// FUNCTION: GAMEMD 0x00798140
void FUN_00798140(void)
{
	DAT_00B73BD8.clear();
	DAT_00B736F8.clear();
	DAT_00B76B00.clear();
	DAT_00B76510.clear();
	DAT_00B73AE8.clear();
	DAT_00B77820.clear();
	DAT_00B73708.clear();
	DAT_00B76188.clear();
	DAT_00B77E10 = 0;
}

// FUNCTION: GAMEMD 0x007982A0
void FUN_007982A0(ArrayList<Wstring> list)
{
}

// FUNCTION: GAMEMD 0x007982D0
int __fastcall FUN_007982D0(int seconds, int count)
{
	int remaining = seconds * 1000;
	DWORD index;
	while ((index = WaitForMultipleObjects(count, DAT_00B73684, FALSE, 100)) == WAIT_TIMEOUT) {
		remaining -= 100;
		if (remaining <= 0) {
			return 18;
		}
		FUN_0077F900();
	}
	ResetEvent(DAT_00B73684[index]);
	return index;
}

// FUNCTION: GAMEMD 0x00798340
void FUN_00798340(void)
{
	for (int i = 0; i < 18; i++) {
		ResetEvent(DAT_00B73684[i]);
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
			FUN_0077DC90(value, 0);
		}
	}
}

void __fastcall FUN_00603D30(HWND item, int surface);

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
					Session.unknown_0C = 1;
					control->F1(window, 0x6E0);
					return FALSE;

				case 0x686:
					Session.unknown_0C = 0;
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
								FUN_0077DC90(value, 0);
								return FALSE;
							}
						}
					}
					break;

				case 0x6E1:
					Session.unknown_0C = 4;
					control->F1(window, 0x6E1);
					return FALSE;

				case 0x6E2:
					Session.unknown_0C = 5;
					Session.unknown_10 = 0;
					control->F1(window, 0x6E2);
					break;

				case 0x6E3:
					Session.unknown_0C = 3;
					control->F1(window, 0x6E3);
					return FALSE;

				case 0x771:
					Session.unknown_0C = 2;
					control->F1(window, 0x771);
					return FALSE;
			}
			break;

		case 0x497:
			{
				HWND item = GetDlgItem(window, 0x746);
				int surface = PCX::Instance.GetSurface("gt18.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x748);
				surface = PCX::Instance.GetSurface("wolclan.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x749);
				surface = PCX::Instance.GetSurface("wolreslk.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x74A);
				surface = PCX::Instance.GetSurface("wolob.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x752);
				surface = PCX::Instance.GetSurface("wolpriv.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75C);
				surface = PCX::Instance.GetSurface("pingg.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75D);
				surface = PCX::Instance.GetSurface("pingy.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75E);
				surface = PCX::Instance.GetSurface("pingr.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79E);
				surface = PCX::Instance.GetSurface("number1.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79F);
				surface = PCX::Instance.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A0);
				surface = PCX::Instance.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A1);
				surface = PCX::Instance.GetSurface("cooperat.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A2);
				surface = PCX::Instance.GetSurface("comchief.pcx", NULL);
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
					Session.unknown_0C = 1;
					control->F1(window, 0x6E0);
					return FALSE;

				case 0x686:
					Session.unknown_0C = 0;
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
								FUN_0077DC90(value, 0);
								return FALSE;
							}
						}
					}
					break;

				case 0x6E1:
					Session.unknown_0C = 4;
					control->F1(window, 0x6E1);
					return FALSE;

				case 0x6E2:
					Session.unknown_0C = 5;
					Session.unknown_10 = 0;
					control->F1(window, 0x6E2);
					break;

				case 0x6E4:
					Session.unknown_0C = 6;
					control->F1(window, 0x6E4);
					return FALSE;

				case 0x771:
					Session.unknown_0C = 2;
					control->F1(window, 0x771);
					return FALSE;
			}
			break;

		case 0x497:
			{
				if (DAT_00B73E40.length() > 0) {
					rc_ptr<PlayerProfile> profile = FUN_00778A20(&DAT_00B73E40);
					if (profile.Get()) {
						profile->FUN_0077A9F0(window, &DAT_00B73E40, true);
					}
				}
				HWND item = GetDlgItem(window, 0x746);
				int surface = PCX::Instance.GetSurface("gt18.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x748);
				surface = PCX::Instance.GetSurface("wolclan.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x749);
				surface = PCX::Instance.GetSurface("wolreslk.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x74A);
				surface = PCX::Instance.GetSurface("wolob.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x752);
				surface = PCX::Instance.GetSurface("wolpriv.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75C);
				surface = PCX::Instance.GetSurface("pingg.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75D);
				surface = PCX::Instance.GetSurface("pingy.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x75E);
				surface = PCX::Instance.GetSurface("pingr.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79E);
				surface = PCX::Instance.GetSurface("number1.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x79F);
				surface = PCX::Instance.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A0);
				surface = PCX::Instance.GetSurface("number0.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A1);
				surface = PCX::Instance.GetSurface("cooperat.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
				item = GetDlgItem(window, 0x7A2);
				surface = PCX::Instance.GetSurface("comchief.pcx", NULL);
				if (item && surface) {
					FUN_00603D30(item, surface);
				}
			}
			break;
	}
	return FALSE;
}

// FUNCTION: GAMEMD 0x00798F50
void __fastcall FUN_00798F50(HWND combo)
{
	int count;

	FUN_00623840(combo);
	count = 0;
	DAT_00AC116C->V52(&count);
	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) {
		profile = FUN_00778A20(&DAT_00B73E40);
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
			if (profile.Get() && i == profile->Field10) {
				SendMessage(combo, 0x14E, item, 0);
			}
		}
	}
}

extern char DAT_0084A210;
extern char DAT_00B761A8[];
extern int DAT_00B77A90;
void FUN_0072C9C0(void);
void FUN_0072CA10(void);
BOOL CALLBACK FUN_00799080(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x00799CD0
void FUN_00799CD0(void)
{
	DAT_00B73E10.setEmpty();
	memset(&DAT_00B76B10, 0, 0x10C);
	memset(&DAT_00B73BE8, 0, 0x10C);
	DAT_0084A210 = 1;
	DAT_00B76AE0.setEmpty();
	DAT_00B77900.setEmpty();
	DAT_00B77DD0 = 0;
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
		{
			WideWstring text(StringTable::LoadString("TXT_FETCHING_SERVLIST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x5EEC));
			FUN_00792BE0(0x10, text, 0);
		}
		int sku = 0x2300;
		if (Session.unknown_0C != 3) {
			sku = 0x2900;
		}
		result = FUN_00797640(sku);
		switch (result) {
			case 0:
				break;
			case 2:
				return -1;
			default:
				return -2;
		}
		if (DAT_00AC1170 && strlen(DAT_00B761A8) && DAT_00B73E40.length() > 0) {
			DAT_00AC1170->V05(DAT_00B761A8, DAT_00B77A90, DAT_00B73E40.get(), DAT_0084A234 & 0xFFFF00, -1, 0, 0);
			DAT_00AC1170->V14(DAT_00B761A8, DAT_00B77A90, DAT_00B73E40.get(), DAT_0084A234 & 0xFFFF00);
		}
		FUN_0072C9C0();
		FUN_007F7624_Class control;
		FUN_006241F0(0x114, FUN_00799080, (OwnerDraw::DialogControl *)&control);
		FUN_0072CA10();
		return Session.unknown_0C != 0;
	}
	return result;
}

void __fastcall FUN_00779830(bool value);
void __fastcall FUN_00779C90(Wstring const * value);

// FUNCTION: GAMEMD 0x0079AE00
BOOL CALLBACK FUN_0079AE00(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_INITDIALOG && lparam) {
		do SetWindowLong(window, 8, ((int *)lparam)[1]); while (0);
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
					FUN_00779C90(&DAT_00B73E40);
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

// FUNCTION: GAMEMD 0x0079BE70
void FUN_0079BE70(void)
{
	HWND dialog = WS_Create_Dialog(Game::hInstance, 0x11E, Game::hWnd, FUN_0079DB50, FALSE);
	UI::RegisterWindow(dialog, 0x11E);
	SendMessage(dialog, 0x4A9, 0, 1);
	ShowWindow(dialog, SW_SHOWNORMAL);
	FUN_007759E0(dialog, FUN_0077F900, 0, 1, 1);
}

// FUNCTION: GAMEMD 0x0079BED0
bool FUN_0079BED0(void)
{
	if (DAT_00B76E24.length() == 0) {
		return false;
	}
	WolEntryState * state;
	if (!DAT_00B76990.getPointer(DAT_00B76E24, &state)) {
		return false;
	}
	if (state == NULL) {
		goto done;
	}
	{
		int value = *state;
		if (value < 0 || value > 2) {
			return true;
		}
	}
done:
	return false;
}

extern int DAT_00B779D0;

static inline void Set_Text(HWND window, wchar_t const * text)
{
	SendMessage(window, 0x4B2, 0, (LPARAM)text);
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
	LadderA ladder;
	memset(&ladder, 0, sizeof(ladder));
	Wstring key = ((Wstring const &)DAT_00B76E24).toLower();
	if (DAT_00B77D80.getValue(key, ladder)) {
		Set_Text(rank_label, FUN_007B7DD0(ladder.Rung).text());
		Set_Text(field10_label, FUN_007B7DD0(ladder.Field10).text());
		Set_Text(field08_label, FUN_007B7DD0(ladder.Field08).text());
		Set_Text(field0C_label, FUN_007B7DD0(ladder.Field0C).text());
		Set_Text(field20_label, FUN_007B7DD0(ladder.Field20).text());
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
	strcpy(user.Nick, DAT_00B73ACC.get());
	DAT_00AC116C->V24(&user);
}

// FUNCTION: GAMEMD 0x0079C520
bool __fastcall FUN_0079C520(Wstring * name)
{
	Wstring lower(*name);
	lower.toLower();
	if (name->length() == 0) {
		return false;
	}
	if (((Wstring const &)lower).toLower() == ((Wstring const &)DAT_00B73E40).toLower()) {
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

void FUN_0079D110(void);

// FUNCTION: GAMEMD 0x0079C8D0
void __fastcall FUN_0079C8D0(Wstring * name)
{
	if (!FUN_0079C520(name)) {
		return;
	}
	User user;
	memset(&user, 0, sizeof(user));
	strncpy(user.Nick, name->text(), 9);
	DAT_00AC116C->V56(&user);
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
			strncpy(user.Nick, name->text(), 9);
			DAT_00AC116C->V57(&user);
			return;
		}
	}
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
	DAT_00AC1170->V05(DAT_00B761A8, DAT_00B77A90, keys.get(), DAT_0084A234 & 0xFFFF00, -1, 0, 0);
	DAT_00AC1170->V14(DAT_00B761A8, DAT_00B77A90, keys.get(), DAT_0084A234 & 0xFFFF00);
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
						strcpy(user.Nick, DAT_00B76E24.get());
						DAT_00AC116C->V62(&user, text.get());
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
						strcpy(user.Nick, name.get());
						DAT_00AC116C->V62(&user, text.get());
						wchar_t * message = text.get();
						FUN_005E2700(DAT_007EEF2C, L"[%hs] %s", DAT_00B73E40.c_str(), message);
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

// FUNCTION: GAMEMD 0x0079F0A0
bool __fastcall FUN_0079F0A0(Wstring * name, ArrayList<WolChannel> * channels, WolChannel * result)
{
	memset(result, 0, sizeof(WolChannel));
	for (int i = 0; i < channels->length(); i++) {
		channels->get(*result, i);
		if (stricmp(result->Name, name->text()) == 0) {
			return true;
		}
	}
	memset(result, 0, sizeof(WolChannel));
	return false;
}

// FUNCTION: GAMEMD 0x0079FC10
void __fastcall FUN_0079FC10(int a, int b, int c)
{
	DAT_00B77E14 = c;
	DAT_00B73A20 = a;
	DAT_00B77D70 = b;
	DAT_00B77A88 = time(NULL);
}

// FUNCTION: GAMEMD 0x007A0430
void FUN_007A0430(void)
{
	User user;
	memset(&user, 0, sizeof(user));
	FUN_004068E0("User list is %d long, adding to ping request queue\n", DAT_00B77900.length());
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		DAT_00B77900.get(user, i);
		if (user.Flags & 1) {
			FUN_004068E0("%s is channel operator, no ping request\n", user.Nick);
		} else if (stricmp(user.Nick, DAT_00B73E40.get()) == 0) {
			FUN_004068E0("%s is local user, no ping request\n", user.Nick);
		} else {
			Wstring name(user.Nick);
			FUN_004068E0("Adding %s to ping request queue\n", name.get());
			DAT_00B76AD0.addTail(name);
		}
	}
}

void FUN_0052F3F0(void);
int FUN_005355D0(void);
rc_ptr<WorldDominationTour::Campaign> * __cdecl FUN_007B0370(void);
extern int DAT_007EEF30;
extern int DAT_00B736F4;
void FUN_007A1060(void);

// FUNCTION: GAMEMD 0x007A0690
bool FUN_007A0690(void)
{
	DAT_00B76AC4.clear();
	HWND dialog = FUN_00775940(0x11F);
	if (dialog) {
		FUN_007757E0(dialog, 0);
	}
	DAT_00B77E28 = 0;
	FUN_0052F3F0();
	if (DAT_00B77900.length() < 2) {
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
	FUN_004068E0("User list is %d long, adding to ping request queue\n", DAT_00B77900.length());
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		DAT_00B77900.get(user, i);
		if (user.Flags & 1) {
			host_index = i;
		}
	}
	if (host_index == -1) {
		FUN_0079FC10(0, -1, 1);
		return false;
	}

	User host;
	memset(&host, 0, sizeof(host));
	DAT_00B77900.get(host, host_index);
	host.Next = NULL;

	WOLOptionsClass options;
	rc_ptr<PlayerProfile> profile;
	if (options.Is_Bit1_Clear()) {
		profile = FUN_00778A20(&DAT_00B73E40);
	} else {
		profile = DAT_00B75E48;
	}
	int country = profile->Field1C;
	Wstring game("Match ");
	Wstring wdt;

	if (Session.unknown_0C == 3) {
		game = "MatchWDT ";
		rc_ptr<WorldDominationTour::Campaign> campaign = *FUN_007B0370();
		if (campaign.Get() == NULL) {
			wdt = ", WDS=0";
		} else {
			switch (campaign->PlayerFaction) {
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
		wdt += Wstring_FromInt(Session.unknown_08);
	}

	FUN_005E2700(DAT_007EEF30, StringTable::LoadString("WOL:RequestingMatch", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6EEB));

	Wstring request(game);
	request += "COU=";
	request += Wstring_FromInt(country);
	request += ", COL=";
	request += Wstring_FromInt(profile->Field20);
	request += ", MBR=";
	if (profile->Field38) {
		request += "1";
	} else {
		request += "0";
	}

	Wstring nick(DAT_00B73E40);
	nick.toLower();
	if (DAT_00B76530.contains(nick)) {
		int locale = 0;
		DAT_00B76530.getValue(nick, locale);
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
	Session.unknown_2988.Get_Address(net, node, &port);
	int address = *(int *)node;
	request += ", ADR=";
	request += Wstring_FromInt(address);
	request += ", NAT=";
	request += Wstring_FromInt((unsigned short)DAT_00ABFEC8.Behavior);
	request += ", PRT=";
	request += Wstring_FromInt(Session.PortBase);
	request += wdt;

	DAT_00AC116C->V12(&host, request.get());
	FUN_0079FC10(4, 180, 0);
	FUN_007A1060();
	return true;
}

// FUNCTION: GAMEMD 0x007A0D10
void FUN_007A0D10(void)
{
	if (DAT_00B77E28 == 0) {
		User user;
		memset(&user, 0, sizeof(user));
		int count = DAT_00B77900.length();
		int handle = 0;
		for (int i = 0; i < count; i++) {
			DAT_00B77900.get(user, i);
			in_addr addr;
			addr.s_addr = user.IPAddr;
			FUN_004068E0("Requesting ping for %s at %s\n", user.Nick, inet_ntoa(addr));
			DAT_00AC1170->V06(inet_ntoa(addr), 1000, &handle);
		}
	}
}

// FUNCTION: GAMEMD 0x007A0DD0
void __fastcall FUN_007A0DD0(User * user)
{
	if (user) {
		for (int i = 0; i < 8; i++) {
			if (Session.Slots[i] && strcmp(FUN_007350C0(Session.Slots[i]->Name), user->Nick) == 0) {
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
	int count = DAT_00B77900.length();
	User user;
	memset(&user, 0, sizeof(user));
	char list[0x100];
	list[0] = 'P';
	list[1] = (char)(count + '0');
	list[2] = 0;
	if (DAT_00B77E28 == 0) {
		for (int i = 0; i < count; i++) {
			DAT_00B77900.get(user, i);
			int ping;
			DAT_00AC1170->V08(user.IPAddr, &ping);
			char entry[0x80];
			sprintf(entry, ",%s,%d", user.Nick, ping);
			strcat(list, entry);
			FUN_007A0EA0(user.Nick, ping);
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
	if (DAT_00B77E28 == 0) {
		FUN_004068E0("Requesting match pings, request list is %d long, pending list is %d long\n", DAT_00B76AD0.length(), DAT_00B76970.length());
		if (DAT_00B76AD0.length() != 0) {
			unsigned long ip = user.IPAddr;
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
						index = FUN_0077D240(&key);
					}
					if (index != -1) {
						DAT_00B77900.get(user, index);
						ip = user.IPAddr;
						if (ip != 0) {
							FUN_004068E0("Pinging %s\n", name.get());
							in_addr addr;
							addr.s_addr = ip;
							if (DAT_00AC1170->V06(inet_ntoa(addr), 1000, &handle) == 0 && handle >= 0) {
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

extern char DAT_00B73714[];

static inline bool Match_Failed(char const * message)
{
	if (strnicmp(message, "Fail", 4) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFail", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x6FFF));
		SetEvent(DAT_00B73684[17]);
		return true;
	}
	if (strnicmp(message, "PingFail", 8) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailPing", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7005));
		SetEvent(DAT_00B73684[17]);
		return true;
	}
	if (strnicmp(message, "RankFail", 8) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailRank", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x700B));
		SetEvent(DAT_00B73684[17]);
		return true;
	}
	if (strnicmp(message, "WDTSideFail", 11) == 0) {
		FUN_005E2700(0x40FFFF, StringTable::LoadString("WOL:MatchFailWDTSide", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7011));
		SetEvent(DAT_00B73684[17]);
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
		DAT_00AC116C->V09();
		ODMessageBox(StringTable::LoadString("WOL:MatchFailExit", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x701A), 0, FUN_0077D710, 0);
		SetEvent(DAT_00B73684[0]);
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
		SetEvent(DAT_00B73684[16]);
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
	LadderA * found = NULL;
	DAT_00B77D80.getPointer(lower, &found);
	int index;
	{
		Wstring key(DAT_00B76AC4.get());
		index = FUN_0077D240(&key);
	}
	if (index == -1) {
		FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPingUnknown", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7073));
	} else {
		User user;
		memset(&user, 0, sizeof(user));
		DAT_00B77900.get(user, index);
		if (user.IPAddr == 0) {
			FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPingUnknown", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x707A));
		} else {
			int ping = -1;
			DAT_00AC1170->V08(user.IPAddr, &ping);
			if (ping != -1) {
				FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPing", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7080), ping);
			} else {
				FUN_005E2700(0xC0FFFF, StringTable::LoadString("WOL:MatchOpponentPingUnknown", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x7082));
			}
		}
	}
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
					SetEvent(DAT_00B73684[0]);
					break;

				case 0x708:
					DAT_00AC116C->V11("start 4439,0,0,10000,10,1,1,1,3,Match,1,0,MP18S3.MAP,Match:mshag,4,1,xyzkerble,7,4");
					return FALSE;

				case 0x709:
					DAT_00AC116C->V11("start 4439,0,0,10000,10,1,1,1,3,Match,1,0,MP18S3.MAP,Match:xyzkerble,7,4,mshag,4,1,");
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
			Session.unknown_2994[i].Set_Address((unsigned char *)&net, node, 0);
			Session.unknown_29F4[i] = third;
			Session.unknown_2FF8[i] = third_value;
			return;
		}
	}
}


// FUNCTION: GAMEMD 0x007A2730
void FUN_007A2730(void)
{
	if (DAT_00AC116C) {
		DAT_00AC116C->V15("G");
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
		notHost = FUN_0077EBA0(empty) != 1;
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
	HWND dialog = FUN_00775940(0xC2);
	FUN_005EC880(dialog, count, full, ai);
}

// FUNCTION: GAMEMD 0x007A2830
void __fastcall FUN_007A2830(Wstring * name)
{
	User user;
	user.Flags = 0;
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		DAT_00B77900.get(user, i);
		if (stricmp(name->text(), user.Nick) == 0) {
			DAT_00AC116C->V21(&user);
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
		user.Flags = 0;
		for (int i = 0; i < DAT_00B77900.length(); i++) {
			DAT_00B77900.get(user, i);
			if (stricmp(name.text(), user.Nick) == 0) {
				DAT_00AC116C->V21(&user);
				break;
			}
		}
	}
}

BOOL CALLBACK FUN_007A29E0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x007A2970
void FUN_007A2970(void)
{
	char busy = DAT_00B77DC2;
	DAT_00B77DC1 = 0;
	if (!busy && FUN_00779940() && FUN_0079C520(&DAT_00B76AC4)) {
		FUN_007F7624_Class control;
		FUN_006241F0(0x122, FUN_007A29E0, (OwnerDraw::DialogControl *)&control);
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


extern int DAT_00B77DF4;
extern int DAT_00B77DCC;
extern HANDLE DAT_00B736A4;
extern long DAT_00B77DF0;
extern int DAT_00B77E64;
extern int DAT_00B77780;
extern int DAT_00B77CE0;
extern int DAT_00B736F4;
void FUN_007A1330(void);
void FUN_007A1060(void);
void __cdecl FUN_004068E0(char const * format, ...);
void FUN_006C6D00(void);
void FUN_006C6DE0(void);
rc_ptr<WDTState> __fastcall FUN_0076ABA0(void * data);
void __fastcall FUN_007B02A0(rc_ptr<WDTState> const & state);
void __fastcall FUN_007B0220(rc_ptr<WDTState> const & state);

// FUNCTION: GAMEMD 0x007A2EA0
// FUN_007A2EA0

// FUNCTION: GAMEMD 0x007A2FC0
CNetUtilEventSink::CNetUtilEventSink(void)
{
}

// FUNCTION: GAMEMD 0x007A2FE0
long __stdcall FUN_007A2FE0(void * sink, long result, unsigned char * data, int length)
{
	if (result < 0 || length <= 0) {
		SetEvent(DAT_00B73684[5]);
		return 0;
	}
	rc_ptr<WDTState> state = FUN_0076ABA0(data);
	if (state.Get() != NULL && state->Has_Owner_History()) {
		if (state->Is_Previous_Cycle()) {
			FUN_004068E0("OnWDTState: Previous cycle data received\n");
			FUN_007B02A0(state);
			SetEvent(DAT_00B73684[11]);
		} else {
			FUN_004068E0("OnWDTState: Current cycle data received\n");
			DAT_00B7646C = time(NULL) + state->TickTime;
			FUN_007B0220(state);
			SetEvent(DAT_00B73684[11]);
		}
	} else {
		FUN_004068E0("OnWDTState: NACK received from server\n");
		SetEvent(DAT_00B73684[11]);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A30D0
long __stdcall FUN_007A30D0(void * sink, long result)
{
	FUN_004068E0("OnGameresSent  %d\n", result);
	if (result == 0) {
		FUN_006C6D00();
	}
	FUN_006C6DE0();
	return 0;
}

static inline rc_ptr<PlayerProfile> Profile_For_Nick(char const * nick)
{
	Wstring name(nick);
	return FUN_00778A20(&name);
}

// FUNCTION: GAMEMD 0x007A3D90
long __stdcall FUN_007A3D90(void * sink, long result, char const * message, char const * nick, char const * pass)
{
	FUN_004068E0("OnNewNick\n");
	DAT_00B77DF0 = result;
	if (result == 0) {
		rc_ptr<PlayerProfile> profile = Profile_For_Nick(nick);
		int nicknum = 0;
		WOLOptionsClass options;
		if (options.Is_Bit1_Clear()) {
			int locale = profile->Field10;
			nicknum = FUN_00789500(nick, pass, 1, locale);
			Session.Write_MultiPlayer_Settings();
		}
		char login_mode = 1;
		char combo_mode = 0;
		HWND dialog = FUN_00775940(0x10F);
		if (dialog == NULL) {
			dialog = FUN_00775940(0x11D);
			combo_mode = dialog != NULL;
			if (dialog == NULL) {
				login_mode = 0;
				dialog = FUN_00775940(0x114);
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
				DAT_00B73E40 = nick;
				DAT_00B7778C = pass;
				HWND password = GetDlgItem(dialog, 0x72C);
				SendMessage(password, 0x4B4, 0, (LPARAM)DAT_00B7778C.text());
				DAT_0084A238 = 1;
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
	HWND chat = FUN_00775940(0xC4);
	if (chat) {
		DAT_00B77DC8 &= ~0x100;
		if (DAT_00B77DC8 == 0) {
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
long __stdcall FUN_007A4070(void * sink, long result, int age, int consent)
{
	FUN_004068E0("OnAgeCheck\n");
	DAT_00B77DF4 = result;
	if (result == 0) {
		DAT_00B77E60 = age;
		DAT_00B77E64 = consent;
	}
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 &= ~0x100;
		if (DAT_00B77DC8 == 0) {
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
long __stdcall FUN_007A40F0(void * sink, long result, int time, unsigned long address, int extra)
{
	if (DAT_00B77DC1 && DAT_00B77780) {
		if (result == 0 && time >= 0 && time < 1000) {
			DAT_00B77CE0++;
		}
		return 0;
	}
	if (Session.unknown_0C == 3) {
		goto counted;
	}
	if (Session.unknown_0C == 1) {
		goto counted;
	}
	if (Session.unknown_0C == 2) {
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
	if (DAT_00B76B10.Type) {
		FUN_00783C80(0, false);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A41A0
CChatEventSink::CChatEventSink(void)
{
}

struct UpdateEntry {
	char Pad[0xC];
	UpdateEntry * Next;
	char Server[0x41];
	char PatchPath[0x100];
	char PatchFile[0x21];
	char Login[0x21];
	char Password[0x21];
	char Pad2[0x1D4 - 0x1B4];
	char LocalPath[0x100];
};

// FUNCTION: GAMEMD 0x007A4C10
long __stdcall FUN_007A4C10(void * sink, long result, UpdateEntry * updates)
{
	int count = 0;
	char working_dir[257];
	char fullpath[257];
	char localfile[257];
	wchar_t status[200];
	if (result < 0 || updates == NULL) {
		SetEvent(DAT_00B73684[0]);
		return 0;
	}
	DAT_00B77DCC = 1;
	UpdateEntry * tmp = updates;
	while (tmp != NULL) {
		tmp = tmp->Next;
		count++;
	}
	HWND chat = FUN_00775940(0xC4);
	if (chat) {
		DAT_00B77DC8 &= ~0x10;
		if (DAT_00B77DC8 == 0) {
			FUN_007757E0(chat, 0);
		}
	}
	HWND top = FUN_00775B10();
	HWND input = GetDlgItem(top, 0x540);
	if (input) {
		SetFocus(input);
	}
	if (ODMessageBox(StringTable::LoadString("TXT_UPGRADEREQUIRED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1580), 4, FUN_0077D710, 0) == IDNO) {
		SetEvent(DAT_00B73684[0]);
		return 0;
	}
	GetCurrentDirectory(0x100, working_dir);
	int i = 0;
	while (updates != NULL) {
		sprintf(fullpath, "%s/%s", updates->PatchPath, updates->PatchFile);
		sprintf(localfile, "%s\\%s", updates->LocalPath, updates->PatchFile);
		CreateDirectory(updates->LocalPath, NULL);
		((DownloadObject *)DAT_00AC1174)->DownloadFile(updates->Server, updates->Login, updates->Password, fullpath, localfile, "SOFTWARE\\Westwood\\Yuri's Revenge");
		i++;
		swprintf(status, StringTable::LoadString("TXT_DOWNLOADING_X_OF_Y", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1596), i, count);
		WideWstring text(status);
		FUN_007F7624_Class control;
		HWND dialog = UI::BeginDialog(0xD3, FUN_0078DA30, 0);
		HWND title = GetDlgItem(dialog, 0x5C7);
		SendMessage(title, 0x4B2, 0, (LPARAM)text.text());
		if (FUN_007759E0(dialog, (bool (*)(void))FUN_0077D710, 0, 1, 1) == IDCANCEL) {
			SetEvent(DAT_00B73684[0]);
			SetCurrentDirectory(working_dir);
			return 0;
		}
		updates = updates->Next;
	}
	ODMessageBox(StringTable::LoadString("TXT_GAME_RESTART", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x15B6), 0, FUN_0077D710, 0);
	SetEvent(DAT_00B736A4);
	SetCurrentDirectory(working_dir);
	return 0;
}

struct SquadEntry {
	char Pad[0x168];
	char Name[0x29];
	char Abbreviation[0x29];
};

// FUNCTION: GAMEMD 0x007A4EC0
long __stdcall FUN_007A4EC0(void * sink, long result, unsigned long id, SquadEntry * squad)
{
	DAT_00B77DDC--;
	if (DAT_00B77DE0) {
		DAT_00B77DE0 = 0;
		DAT_00B77DE4 = id;
		if (result != 0) {
			return 0;
		}
		strcpy(DAT_00B77A94, squad->Abbreviation);
	} else if (result != 0) {
		return 0;
	}
	for (int i = 0; i < DAT_00B77900.length(); i++) {
		User * user = NULL;
		DAT_00B77900.getPointer(&user, i);
		if (user->Squad == id) {
			strcpy(user->Abbrev, squad->Abbreviation);
			strcpy(user->Login, squad->Name);
			User temp;
			memset(&temp, 0, sizeof(temp));
			DAT_00B77900.get(temp, i);
			User copy = temp;
			copy.Next = NULL;
			DAT_00B77900.remove(i);
			FUN_00783070(&copy);
		}
	}
	Wstring key(squad->Abbreviation);
	key.toLower();
	if (!DAT_00B768C0.contains(key)) {
		DAT_00AC1170->V05(DAT_00B761A8, DAT_00B77A90, squad->Abbreviation, 0x802100, -1, 0, 0);
	}
	if (DAT_00B77DDC == 0) {
		FUN_00783C80(0, false);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007A5220
long __stdcall FUN_007A5220(void * sink, long result, User * user)
{
	if (result >= 0) {
		FUN_00785740(user);
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

wchar_t * __fastcall FUN_00735120(char const * narrow);
extern char DAT_0084ACA0[];
extern int DAT_00B76E54;

static inline void Close_Wait_Window(unsigned int event)
{
	HWND dialog = FUN_00775940(0xC4);
	if (dialog) {
		DAT_00B77DC8 &= ~event;
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

// FUNCTION: GAMEMD 0x007A5690
long __stdcall FUN_007A5690(void * sink, long result, WolChannel * channel)
{
	if (result < 0) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_CHANCREATE_FAILED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x16C1));
		DAT_00B77E4C = 0;
		DAT_00B76B10.Name[0] = 0;
		Close_Wait_Window(4);
		return 0;
	}
	if (channel->Type == 0) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_CREATED_CHAN", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x16CB), FUN_00735120(channel->Name));
		memcpy(&DAT_00B76B10, channel, 0x10C);
	} else {
		memset(DAT_00B76E40, 0, 0x940);
		strcpy(DAT_00B76E40, DAT_00B73E40.get());
		DAT_00B76E58[0].House = Session.House;
		DAT_00B76E58[0].Unknown = 0;
		DAT_00B76E54 = Game::PlayerColor;
		DAT_00B76E58[0].Accepted = 1;
		memcpy(&DAT_00B76B10, channel, 0x10C);
	}
	DAT_00B77E4C = 1;
	DAT_00B77DD0++;
	FUN_004068E0(DAT_0084ACA0);
	if (DAT_00AC116C && channel->Type != 0) {
		char info[80];
		memset(info, 0, sizeof(info));
		FUN_0077E7A0(info);
		info[40] = 0;
		DAT_00AC116C->V42(info);
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
	FUN_00785740(user);
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
	strcpy(DAT_00B77E30, user->Nick);
	if (DAT_00B77DC1) {
		wchar_t message[256];
		swprintf(message, DAT_0084B020, user->Nick, wide.get());
		int scheme = GadgetClass::Get_Color_Scheme();
		Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Session.Messages.Add_Message(NULL, 0, StringTable::LoadString("TXT_HOW_TO_REPLY", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1F82), scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
		FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->Nick, wide.get());
		DAT_00B76478 = 300;
	} else {
		HWND top = FUN_00775B10();
		while (top) {
			if (GetDlgItem(top, 0x53F)) {
				FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->Nick, wide.get());
				break;
			}
			if (GetDlgItem(top, 0x5B5)) {
				FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->Nick, wide.get());
			}
			top = WS_Next_Lower_Dialog(top);
		}
		if (!top) {
			ReleaseCapture();
			FUN_0052FEC0(NULL, true);
			top = UI::BeginDialog(0x11E, (DLGPROC)FUN_0079DB50, 0);
			if (top) {
				SetFocus(top);
				FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->Nick, wide.get());
				ShowWindow(top, SW_SHOWNORMAL);
			}
		}
		Set_Item_Text(GetDlgItem(top, 0x718), FUN_0079DA90(GetDlgItem(top, 0x53F)));
	}
	if (DAT_00B77E40) {
		DAT_00AC116C->V59(user, DAT_00B76230.get());
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
		swprintf(message, DAT_0084B020, user->Nick, text);
		int scheme = GadgetClass::Get_Color_Scheme();
		Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
		Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
		FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->Nick, text);
	} else {
		HWND top = FUN_00775B10();
		while (top) {
			if (GetDlgItem(top, 0x53F)) {
				FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->Nick, text);
				break;
			}
			if (GetDlgItem(top, 0x5B5)) {
				FUN_005E2790(DAT_007EEF40, DAT_0084B020, user->Nick, text);
			}
			top = WS_Next_Lower_Dialog(top);
		}
		if (!top) {
			ReleaseCapture();
			FUN_0052FEC0(NULL, true);
			top = UI::BeginDialog(0x11E, (DLGPROC)FUN_0079DB50, 0);
			SetFocus(top);
			FUN_005E2700(DAT_007EEF40, DAT_0084B020, user->Nick, text);
			ShowWindow(top, SW_SHOWNORMAL);
		}
		Set_Item_Text(GetDlgItem(top, 0x718), FUN_0079DA90(GetDlgItem(top, 0x53F)));
	}
	if (DAT_00B77E40) {
		DAT_00AC116C->V59(user, DAT_00B76230.get());
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

// FUNCTION: GAMEMD 0x007AA6A0
long __stdcall FUN_007AA6A0(void * sink, long result, int value)
{
	if (!DAT_00B77DC1 && result >= 0) {
		FUN_005E2700(DAT_007EEF28, DAT_0084B128, value);
	}
	return 0;
}

static inline void Clear_Channel(WolChannel & channel)
{
	memset(&channel, 0, 0x10C);
}

// FUNCTION: GAMEMD 0x007AA6D0
long __stdcall FUN_007AA6D0(void * sink, long result)
{
	wchar_t message[256];
	if (DAT_00B77DC0) {
		SetEvent(DAT_00B73684[9]);
		DAT_00B77DE8 = result;
		return 0;
	}
	if (result == 0x4012F) {
		if (DAT_00B77DC1 && DAT_00B77DC2) {
		} else if (DAT_00B77DC1) {
			message[0] = 0;
			swprintf(message, StringTable::LoadString("TXT_WOL_CONNLOST", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x210C));
			int scheme = GadgetClass::Get_Color_Scheme();
			Session.Messages.Add_Message(NULL, 0, message, scheme, TextPrintType(0x16), int(Rule->MessageDelay * TICKS_PER_MINUTE), false);
			Map.Flag_To_Redraw(GS_REDRAW_DIRTY);
			DAT_00B77DC2 = 1;
			DAT_00B77DD0 = 0;
			Clear_Channel(DAT_00B76B10);
			Clear_Channel(DAT_00B73BE8);
			FUN_004068E0(DAT_0084B170);
		} else {
			if (FUN_00775940(0x113)) {
				ODMessageBox(StringTable::LoadString("TXT_YOURE_DISCON", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x211E), 0, FUN_0077D710, 0);
			}
			DAT_00B77DC0 = 1;
			DAT_00B77DC2 = 1;
			SetEvent(DAT_00B73684[0]);
			DAT_00B77DD0 = 0;
			Clear_Channel(DAT_00B76B10);
			Clear_Channel(DAT_00B73BE8);
			FUN_004068E0(DAT_0084B138);
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
			if (stricmp(Session.Handle, user->Nick) != 0) {
				FUN_005E2700(DAT_007EEF34, DAT_0084B224, user->Nick, wide.get());
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
			if (stricmp(Session.Handle, user->Nick) != 0) {
				FUN_005E2700(DAT_007EEF34, DAT_0084B224, user->Nick, text);
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
			if (stricmp(Session.Handle, user->Nick) != 0) {
				FUN_005E2700(DAT_007EEF38, DAT_0084B224, user->Nick, wide.get());
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
			if (stricmp(Session.Handle, user->Nick) != 0) {
				FUN_005E2700(DAT_007EEF38, DAT_0084B224, user->Nick, text);
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

void __fastcall FUN_005E2CD0(wchar_t const * text, int type, void (*callback)(void), bool large);

// FUNCTION: GAMEMD 0x007AB350
long __stdcall FUN_007AB350(void * sink, long result, time_t bannedTill)
{
	if (!DAT_00B77DC1 && result == 0) {
		wchar_t text[200];
		swprintf(text, StringTable::LoadString("TXT_BANNEDTILL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x2269), ctime(&bannedTill));
		FUN_005E2CD0(text, 0, FUN_0077D710, 0);
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
		for (int i = 0; i < DAT_00B77900.length(); i++) {
			User user;
			if (DAT_00B77900.get(user, i) && stricmp(user.Nick, name) == 0) {
				user.Flags = (user.Flags & mask) | flags;
				DAT_00B77900.remove(i);
				User copy = user;
				copy.Next = NULL;
				FUN_00783070(&copy);
			}
		}
		FUN_00783C80(0, false);
		return 0;
	}
	return 0;
}

extern char DAT_00B785C0[64];
bool FUN_005E95E0(void);
bool FUN_005E9510(void);
void _DrawMessage(int color, const wchar_t * message, HWND window);

// FUNCTION: GAMEMD 0x007AB670
long __stdcall FUN_007AB670(void * sink, long result, WolChannel * channel, User * kicked, User * kicker)
{
	if (DAT_00B77DC1) {
		return 0;
	}
	if (result < 0) {
		return 0;
	}
	memset(DAT_00B785C0, 0, sizeof(DAT_00B785C0));
	if (strnicmp(channel->Name, "Lob_41_", strlen("Lob_41_")) == 0) {
		int lobby = atol(channel->Name + strlen("Lob_41_"));
		int copy = lobby / 8;
		int slot = lobby % 8;
		if (copy == 0) {
			sprintf(DAT_00B785C0, "%s", DAT_00B73E1C[slot]);
		} else {
			sprintf(DAT_00B785C0, "%s %d", DAT_00B73E1C[slot], copy + 1);
		}
	} else {
		sprintf(DAT_00B785C0, "%s", channel->Name);
	}
	if (kicked->Flags & 0x8000) {
		WideWstring wide;
		if (DAT_00B76B10.Type && FUN_005E95E0()) {
			wide = StringTable::LoadString("TXT_CHANNEL_FULL", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x22D0);
		} else {
			wchar_t message[1024];
			memset(message, 0, sizeof(message));
			swprintf(message, StringTable::LoadString("TXT_YOURE_KICKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x22D4), FUN_00735120(channel->Name), FUN_00735120(kicker->Nick));
			wide = message;
		}
		if (!DAT_00B76B10.Type) {
			FUN_005E2CD0(wide.get(), 0, FUN_0077D710, 0);
			DAT_00B77DD4++;
		} else {
			HWND dialog = FUN_00775940(0x113);
			if (dialog) {
				_DrawMessage(DAT_007EEF28, wide.get(), GetDlgItem(dialog, 0x53F));
			}
		}
	} else if (!FUN_005E9510()) {
		FUN_005E2700(DAT_007EEF28, StringTable::LoadString("TXT_USER_KICKED", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x22EC), FUN_00735120(kicked->Nick), FUN_00735120(channel->Name), FUN_00735120(kicker->Nick));
	}
	FUN_00785740(kicked);
	FUN_00783C80(0, false);
	FUN_00781480();
	return 0;
}

static inline void Forget_Locale(Wstring & name)
{
	int old;
	DAT_00B76530.remove(name, old);
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
		name.set(user->Nick);
		name.toLower();
		Forget_Locale(name);
		int locale = user->Locale;
		DAT_00B76530.add(name, locale);
		if (stricmp(users->Nick, DAT_00B73E40.get()) == 0) {
			WOLOptionsClass options;
			rc_ptr<PlayerProfile> profile;
			if (options.Is_Bit1_Clear()) {
				profile = FUN_00778A20(&DAT_00B73E40);
			} else {
				profile = DAT_00B75E48;
			}
			profile->SetLocale(users->Locale, 2);
			if (options.Is_Bit1_Clear()) {
				profile->Save(&DAT_00B73E40);
			}
			FUN_004068E0("OnUserLocale callback - Locale is %d\n", users->Locale);
			found = true;
		}
		user = user->Next;
	}
	if (found) {
		SetEvent(DAT_00B73684[14]);
	}
	return 0;
}


static inline void Uncache_Locale(Wstring & name)
{
	DAT_00B76530.remove(name);
}

// FUNCTION: GAMEMD 0x007ABAF0
long __stdcall FUN_007ABAF0(void * sink, long result, int newlocale)
{
	Wstring name;
	char handle[64];
	strcpy(handle, Session.Handle);
	name.set(handle);
	name.toLower();
	if (DAT_00B76530.contains(name)) {
		Uncache_Locale(name);
	}
	DAT_00B76530.add(name, newlocale);
	return 0;
}

static inline void Forget_Team(Wstring & name)
{
	DAT_00B76680.remove(name);
}

// FUNCTION: GAMEMD 0x007ABD40
long __stdcall FUN_007ABD40(void * sink, long result, User * users)
{
	if (result >= 0) {
		User * user = users;
		Wstring name;
		while (user != NULL) {
			name.set(user->Nick);
			if (DAT_00B76680.contains(name)) {
				Forget_Team(name);
			}
			int team = user->Team;
			DAT_00B76680.add(name, team);
			user = user->Next;
		}
		FUN_00783C80(0, false);
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
		if (DAT_00B76680.contains(name)) {
			Forget_Team(name);
		}
		DAT_00B76680.add(name, newteam);
		return 0;
	}
	FUN_004068E0("OnSetTeam FAILED: %d\n", newteam);
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

// FUNCTION: GAMEMD 0x007AC4F0
void __stdcall FUN_007AC4F0(void)
{
}

// FUNCTION: GAMEMD 0x007AC500
long __stdcall FUN_007AC500(AtlModule * module)
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

// TEMPLATE: GAMEMD 0x007ACA00
// ?add@?$ArrayList@UUser@@@@QAEDAAUUser@@H@Z

// TEMPLATE: GAMEMD 0x007ACB90
// ?get@?$ArrayList@UUser@@@@QBEDAAUUser@@H@Z

// TEMPLATE: GAMEMD 0x007ACBD0
// ?clear@?$ArrayList@H@@QAEXXZ

// TEMPLATE: GAMEMD 0x007ACD30
// ?clear@?$ArrayList@VWstring@@@@QAEXXZ

// TEMPLATE: GAMEMD 0x007ACD60
// ?remove@?$ArrayList@VWstring@@@@QAEDH@Z

// TEMPLATE: GAMEMD 0x007ACE30
// ?get@?$ArrayList@VWstring@@@@QBEDAAVWstring@@H@Z

// TEMPLATE: GAMEMD 0x007ACE60
// ?clear@?$Dictionary@VWstring@@H@@QAEXXZ

// TEMPLATE: GAMEMD 0x007AD1B0
// ?ID@?$VectorClass@PAUtConnInfoStruct@@@@UAEHABQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD1F0
// ?ID@?$VectorClass@PAUtConnInfoStruct@@@@UAEHPBQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD210
// ?Resize@?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAE_NHPBQAUtConnInfoStruct@@@Z
// TEMPLATE: GAMEMD 0x007AD2C0
// ?ID@?$DynamicVectorClass@PAUtConnInfoStruct@@@@UAEHABQAUtConnInfoStruct@@@Z
// SYNTHETIC: GAMEMD 0x007AD2F0
// ??_G?$DNode@VWstring@@ULadderA@@@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD310
// ??_G?$DNode@VWstring@@H@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD330
// ??_GWstring@@QAEPAXI@Z
// SYNTHETIC: GAMEMD 0x007AD350
// ??_G?$DNode@VWstring@@UWolChannel@@@@QAEPAXI@Z
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
// ?add@?$ArrayList@UWolChannel@@@@QAEDAAUWolChannel@@H@Z

// TEMPLATE: GAMEMD 0x007AD930
// ?shrinkVector@?$ArrayList@UWolChannel@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x007AD9D0
// ?growVector@?$ArrayList@UUser@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x007ADA70
// ?shrinkVector@?$ArrayList@UUser@@@@AAEDXZ
bool operator==(WolChannel const &, WolChannel const &);
bool operator<=(WolChannel const &, WolChannel const &);
bool operator<(WolChannel const &, WolChannel const &);
template class ArrayList<WolChannel>;
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
// ?getPointer@?$Dictionary@VWstring@@ULadderA@@@@QAEDAAVWstring@@PAPAULadderA@@@Z

// TEMPLATE: GAMEMD 0x007ADCD0
// ?remove@?$Dictionary@VWstring@@ULadderA@@@@QAEDAAVWstring@@AAULadderA@@@Z

// TEMPLATE: GAMEMD 0x007ADF70
// ?shrink@?$Dictionary@VWstring@@ULadderA@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE020
// ?expand@?$Dictionary@VWstring@@ULadderA@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AE0C0
// ?keyHash@?$Dictionary@VWstring@@ULadderA@@@@AAEIAAVWstring@@@Z
template class Dictionary<Wstring, LadderA>;

// TEMPLATE: GAMEMD 0x007AE0E0
// ?expand@?$Dictionary@VWstring@@ULadderB@@@@AAEXXZ

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
template class Dictionary<Wstring, LadderB>;
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
// ?getPointer@?$Dictionary@VWstring@@UWolChannel@@@@QAEDAAVWstring@@PAPAUWolChannel@@@Z

// TEMPLATE: GAMEMD 0x007AE950
// ?remove@?$Dictionary@VWstring@@UWolChannel@@@@QAEDAAVWstring@@AAUWolChannel@@@Z

// TEMPLATE: GAMEMD 0x007AEC00
// ?shrink@?$Dictionary@VWstring@@UWolChannel@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AECB0
// ?keyHash@?$Dictionary@VWstring@@UWolChannel@@@@AAEIAAVWstring@@@Z
template class Dictionary<Wstring, WolChannel>;

// TEMPLATE: GAMEMD 0x007AECD0
// ??0?$VectorClass@PAUtConnInfoStruct@@@@QAE@HPBQAUtConnInfoStruct@@@Z

// TEMPLATE: GAMEMD 0x007AED40
// ?growVector@?$ArrayList@UServer@@@@AAEDXZ

// TEMPLATE: GAMEMD 0x007AEDD0
// ?remove@?$ArrayList@H@@QAEDH@Z

// TEMPLATE: GAMEMD 0x007AEE90
// ?get@?$ArrayList@H@@QBEDAAHH@Z

// TEMPLATE: GAMEMD 0x007AEEC0
// ?growVector@?$ArrayList@H@@AAEDXZ

// TEMPLATE: GAMEMD 0x007AEF50
// ?shrink@?$Dictionary@VWstring@@ULadderB@@@@AAEXXZ

// TEMPLATE: GAMEMD 0x007AF000
// ?growVector@?$ArrayList@VWstring@@@@AAEDXZ

// FUNCTION: GAMEMD 0x007AD440
long __stdcall FUN_007AD440(void * sink)
{
	SendMessage(FUN_00775B10(), 0x5F4, 0, 0);
	return 0;
}

// FUNCTION: GAMEMD 0x007AD460
long __stdcall FUN_007AD460(void * sink, LPARAM param)
{
	SendMessage(FUN_00775B10(), 0x5F5, 0, param);
	return 0;
}

// FUNCTION: GAMEMD 0x007AD480
long __stdcall FUN_007AD480(void * sink, unsigned int done, unsigned int total, long unused, long seconds)
{
	wchar_t buffer[200];

	buffer[0] = 0;
	SendDlgItemMessage(FUN_00775B10(), 0x5C6, 0x402, done * 100 / total, 0);
	if (seconds > 0) {
		swprintf(buffer, StringTable::LoadString("TXT_BYTES_W_TIME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x142F), done, total, seconds);
	} else {
		swprintf(buffer, StringTable::LoadString("TXT_BYTES_WO_TIME", NULL, "D:\\ra2mdpost\\wonline.cpp", 0x1434), done, total);
	}
	if (time(NULL) - DAT_00B77EA8 != 0) {
		HWND item = GetDlgItem(FUN_00775B10(), 0x5C8);
		SendMessage(item, 0x4B2, 0, (LPARAM)buffer);
		DAT_00B77EA8 = time(NULL);
	}
	return 0;
}

// FUNCTION: GAMEMD 0x007AD580
long __stdcall FUN_007AD580(void * sink, int state)
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
long __stdcall FUN_007AD610(void * sink)
{
	return 0;
}
