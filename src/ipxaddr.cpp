// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxaddr.h"
#include "wsproto.h"
#include <cstdio>
#include <cstring>

#include <windows.h>
#include <stdio.h>

struct StreamRing {
	unsigned int FUN_00409320(void);
};

extern int __fastcall FUN_00409030(StreamRing * ring, FILE * file, int count, int * written);
void __fastcall FUN_0053E5F0(void);
void __fastcall FUN_0053E6B0(void);

extern int DAT_00AA0374;
extern CRITICAL_SECTION DAT_00AA0418;
extern int DAT_00AA0444;
extern int DAT_00AA0448;
extern int * DAT_00AA044C;
extern int DAT_00AA0450;
extern void * DAT_00AA045C;
extern StreamRing * DAT_00AA0460;
extern StreamRing * DAT_00AA0464;
extern FILE * DAT_00AA0468;
extern int DAT_00AA046C;
extern int DAT_00AA0470;
extern int DAT_00AA0474;

// FUNCTION: 0x0053E3C0
void __fastcall FUN_0053E3C0(HWND window)
{
	if (DAT_00AA0448 && window) {
		int * slot = DAT_00AA044C;
		int * empty = 0;
		for (int i = 1; i < DAT_00AA0374; i++, slot++) {
			if (!*slot) {
				if (!empty) empty = slot;
			} else if (*slot == (int)window) {
				return;
			}
		}
		if (empty) {
			*empty = (int)window;
			DAT_00AA0450++;
		}
	}
}

// FUNCTION: 0x0053E420
void __fastcall FUN_0053E420(HWND window)
{
	if (DAT_00AA0448 && window) {
		int count = DAT_00AA0374;
		int * slot = DAT_00AA044C;
		for (int i = 1; i < count; i++, slot++) {
			if (*slot == (int)window) {
				*slot = 0;
				DAT_00AA0450--;
				return;
			}
		}
	}
}

// FUNCTION: 0x0053E470
int __fastcall FUN_0053E470(int value)
{
	if (DAT_00AA0448) {
		int cached = DAT_00AA0470;
		int * table = DAT_00AA044C;
		if (value == cached) return DAT_00AA0474;
		int count = DAT_00AA0374;
		for (int i = 0; i < count; i++) {
			if (table[i] == value) {
				DAT_00AA0470 = value;
				DAT_00AA0474 = i;
				return i;
			}
		}
	}
	return -1;
}

// FUNCTION: 0x0053E4C0
int __fastcall FUN_0053E4C0(int index)
{
	if (DAT_00AA0448 && index >= 0 && index < DAT_00AA0374) return DAT_00AA044C[index];
	return 0;
}

// FUNCTION: 0x0053E4F0
int __fastcall FUN_0053E4F0(char const * name)
{
	if (DAT_00AA0448 && !DAT_00AA0468) {
		EnterCriticalSection(&DAT_00AA0418);
		DAT_00AA0468 = fopen(name, "wb");
		if (DAT_00AA0468) DAT_00AA0444 = 1;
		LeaveCriticalSection(&DAT_00AA0418);
		return DAT_00AA0468 != 0;
	}
	return 0;
}

// FUNCTION: 0x0053E550
int __fastcall FUN_0053E550(char const * name)
{
	if (DAT_00AA0448 && !DAT_00AA0468) {
		EnterCriticalSection(&DAT_00AA0418);
		DAT_00AA0468 = fopen(name, "rb");
		DAT_00AA046C = 0;
		if (DAT_00AA0468) {
			DAT_00AA0444 = 2;
			if (FUN_00409030(DAT_00AA0460, DAT_00AA0468, DAT_00AA0460->FUN_00409320(), 0) != 1) {
				DAT_00AA046C = 1;
			}
		}
		LeaveCriticalSection(&DAT_00AA0418);
		return DAT_00AA0468 != 0;
	}
	return 0;
}

// FUNCTION: 0x0053E5F0
void __fastcall FUN_0053E5F0(void)
{
	if (DAT_00AA0448) {
		EnterCriticalSection(&DAT_00AA0418);
		FUN_0053E6B0();
		DAT_00AA0444 = 0;
		if (DAT_00AA0468) {
			fclose(DAT_00AA0468);
			DAT_00AA0468 = 0;
		}
		LeaveCriticalSection(&DAT_00AA0418);
	}
}

// FUNCTION: 0x0053E6B0
void __fastcall FUN_0053E6B0(void)
{
	if (DAT_00AA0448) {
		EnterCriticalSection(&DAT_00AA0418);
		if (DAT_00AA0468 && DAT_00AA045C && DAT_00AA0444 == 1 && DAT_00AA0464) {
			FUN_00409030(DAT_00AA0464, DAT_00AA0468, DAT_00AA0464->FUN_00409320(), 0);
			fflush(DAT_00AA0468);
		}
		LeaveCriticalSection(&DAT_00AA0418);
	}
}

// FUNCTION: 0x0053E720
int __fastcall FUN_0053E720(void)
{
	return DAT_00AA0444;
}

// FUNCTION: 0x0053ECB0
IPXAddressClass::IPXAddressClass(void)
{
	NetworkNumber[0] = 0xff;
	NetworkNumber[1] = 0xff;
	NetworkNumber[2] = 0xff;
	NetworkNumber[3] = 0xff;
	NodeAddress[0] = 0xff;
	NodeAddress[1] = 0xff;
	NodeAddress[2] = 0xff;
	NodeAddress[3] = 0xff;
	NodeAddress[4] = 0xff;
	NodeAddress[5] = 0xff;
}

// FUNCTION: 0x0053F020
bool IPXAddressClass::operator > (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) > 0);
}

// FUNCTION: 0x0053F050
bool IPXAddressClass::operator < (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) < 0);
}

// FUNCTION: 0x0053F080
bool IPXAddressClass::operator >= (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) >= 0);
}

// FUNCTION: 0x0053F0B0
bool IPXAddressClass::operator <= (IPXAddressClass & addr)
{
	return(memcmp(this, &addr, 10) <= 0);
}
