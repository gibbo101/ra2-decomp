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

#include <windows.h>
#undef LoadString
#include <ole2.h>
#include <stddef.h>
#include <string.h>
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
	char Pad[0x24];
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

extern char DAT_00B76AF0;
extern char DAT_00B73AD0;
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
