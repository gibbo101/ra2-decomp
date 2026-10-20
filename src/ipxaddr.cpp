// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "ipxaddr.h"
#include "wsproto.h"
#include "_xmouse.h"
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
extern CRITICAL_SECTION DAT_00AA0400;
extern int DAT_00AA0434;
extern int DAT_00AA0438;
extern short DAT_00AA0168[256];
extern int DAT_00828140;
extern int DAT_00AA0430;
extern int DAT_00AA043C;
extern int DAT_00AA0440;
extern int DAT_00AA0478;
extern int DAT_00AA0368;
extern unsigned int DAT_00AA036C;
extern int DAT_00AA0370;
namespace Unsorted { extern int CurrentFrame; }
extern void __fastcall FUN_00408EE0(StreamRing * ring, void * buffer, int count);

struct MouseRecord {
	int Frame;
	int Counter;
	int Type;
	int X;
	int Y;
};

struct MessageRecord {
	int Frame;
	int Counter;
	int Type;
	MSG Message;
};

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

int __fastcall FUN_0053E770(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max, unsigned int remove);

// FUNCTION: 0x0053E730
int __fastcall FUN_0053E730(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max)
{
	int result;
	do {
		result = FUN_0053E770(msg, window, filter_min, filter_max, 1);
	} while (!result);
	if (msg->message == WM_QUIT) result = 0;
	return result;
}

// FUNCTION: 0x0053E770
int __fastcall FUN_0053E770(MSG * msg, HWND window, unsigned int filter_min, unsigned int filter_max, unsigned int remove)
{
	int result = 0;
	union {
		MouseRecord mouse;
		MessageRecord message;
	} rec;
	POINT pt;

	if (DAT_00AA0444 == 1 && MouseCursor) {
		GetCursorPos(&pt);
		DAT_00AA0434 = pt.x;
		DAT_00AA0438 = pt.y;
		MouseCursor->Convert_Coordinate(DAT_00AA0434, DAT_00AA0438);
		if (DAT_00AA0434 != DAT_00AA043C || DAT_00AA0438 != DAT_00AA0440) {
			rec.mouse.Type = 1;
			rec.mouse.Counter = DAT_00AA0430;
			rec.mouse.Frame = Unsorted::CurrentFrame;
			EnterCriticalSection(&DAT_00AA0400);
			MouseCursor->Set_Mouse_Point(DAT_00AA0434, DAT_00AA0438);
			rec.mouse.X = DAT_00AA0434;
			rec.mouse.Y = DAT_00AA0438;
			DAT_00AA043C = DAT_00AA0434;
			DAT_00AA0440 = DAT_00AA0438;
			LeaveCriticalSection(&DAT_00AA0400);
			FUN_00408EE0(DAT_00AA0460, &rec, 0x14);
		}
	}

top:
	if (DAT_00AA0444 == 2) {
		while (PeekMessageA(msg, 0, 0, 0, 1)) {
			if (msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE) {
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
				goto top;
			}
			if (msg->message == WM_PAINT) DispatchMessageA(msg);
		}
		for (;;) {
			if (!DAT_00AA0478) {
				unsigned int avail = DAT_00AA0464->FUN_00409320();
				if (avail == 0) {
					if (DAT_00AA046C) {
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
						goto top;
					}
					continue;
				}
				if (avail < 0xc) continue;
				FUN_00408EE0(DAT_00AA0464, &DAT_00AA0368, 0xc);
				DAT_00AA0478 = 1;
			}
			if (DAT_00AA036C > (unsigned int)DAT_00AA0430) return 0;
			DAT_00AA0478 = 0;
			if (DAT_00AA0370 == 1) {
				FUN_00408EE0(DAT_00AA0464, &pt, 8);
				EnterCriticalSection(&DAT_00AA0400);
				DAT_00AA0434 = pt.x;
				DAT_00AA0438 = pt.y;
				if (MouseCursor) MouseCursor->Set_Mouse_Point(pt.x, pt.y);
				LeaveCriticalSection(&DAT_00AA0400);
			} else {
				FUN_00408EE0(DAT_00AA0464, msg, 0x1c);
				msg->hwnd = (HWND)FUN_0053E4C0((int)msg->hwnd);
				if (msg->hwnd) {
					result = 1;
					break;
				}
			}
		}
	} else {
		result = PeekMessageA(msg, window, filter_min, filter_max, remove);
	}

	if (DAT_00AA0444 == 1 && result) {
		int index = FUN_0053E470((int)msg->hwnd);
		if (index < 0) {
			if (!remove) PeekMessageA(msg, window, filter_min, filter_max, 1);
			goto top;
		}
		rec.message.Type = 0;
		rec.message.Counter = DAT_00AA0430;
		rec.message.Frame = Unsorted::CurrentFrame;
		rec.message.Message = *msg;
		rec.message.Message.hwnd = (HWND)index;
		FUN_00408EE0(DAT_00AA0460, &rec, 0x28);
	}
	return result;
}

// FUNCTION: 0x0053EC00
void __fastcall FUN_0053EC00(int * x, int * y)
{
	if (DAT_00AA0448 && DAT_00AA0444) {
		EnterCriticalSection(&DAT_00AA0400);
		*x = DAT_00AA0434;
		*y = DAT_00AA0438;
		LeaveCriticalSection(&DAT_00AA0400);
	}
}

// FUNCTION: 0x0053EC40
void __fastcall FUN_0053EC40(int key, bool up)
{
	if (DAT_00AA0444) {
		DAT_00AA0168[key & 0xff] = up ? 0 : -0x8000;
	}
}

// FUNCTION: 0x0053EC70
short __fastcall FUN_0053EC70(int key)
{
	if (!DAT_00AA0444) return GetKeyState(key);
	return DAT_00AA0168[key & 0xff];
}

// FUNCTION: 0x0053EC90
short __fastcall FUN_0053EC90(int key)
{
	if (!DAT_00AA0444) return GetAsyncKeyState(key);
	return DAT_00AA0168[key & 0xff];
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

// FUNCTION: 0x0053ECE0
IPXAddressClass::IPXAddressClass(NetNumType net, NetNodeType node, unsigned short port)
{
	memcpy(NodeAddress, node, 6);
	if (DAT_00828140 == 1) {
		memcpy(NetworkNumber, net, 4);
	} else {
		*(unsigned long *)NetworkNumber = 0;
		*(unsigned short *)NetworkNumber = port;
	}
}

// FUNCTION: 0x0053ED20
void IPXAddressClass::Set_Address(NetNumType net, NetNodeType node, unsigned short port)
{
	memcpy(NodeAddress, node, 6);
	if (DAT_00828140 == 1) {
		memcpy(NetworkNumber, net, 4);
	} else {
		*(unsigned long *)NetworkNumber = 0;
		*(unsigned short *)NetworkNumber = port;
	}
}

// FUNCTION: 0x0053ED60
void IPXAddressClass::Get_Address(NetNumType net, NetNodeType node, unsigned short * port)
{
	memcpy(node, NodeAddress, 6);
	if (DAT_00828140 == 1) {
		memcpy(net, NetworkNumber, 4);
	} else {
		*port = *(unsigned short *)NetworkNumber;
	}
}

// FUNCTION: 0x0053EDA0
void IPXAddressClass::Get_Address(NetNumType net, NetNodeType node)
{
	memcpy(node, NodeAddress, 6);
	memcpy(net, NetworkNumber, 4);
}

// FUNCTION: 0x0053EDC0
void IPXAddressClass::Get_Node(NetNodeType node)
{
	memcpy(node, NodeAddress, 6);
}

// FUNCTION: 0x0053EDE0
void IPXAddressClass::Set_Node(NetNodeType node)
{
	memcpy(NodeAddress, node, 6);
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
