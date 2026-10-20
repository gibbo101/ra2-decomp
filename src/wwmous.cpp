// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "wwmous.h"

#include "bsurface.h"
#include "dsurface.h"
#include "surface.h"
#include "win.h"

void __cdecl FUN_004068E0(char const * format, ...);

struct MouseThreadInfo {
	int Stop;
	int Count;
	int Done;
	int Pad;
	__int64 Delay;
	int Running;

	void Reset(void)
	{
		Stop = 0;
		Done = 0;
		Running = 0;
		Delay = 0x10;
		Count = 0;
	}
};

extern MouseThreadInfo DAT_00B78138;
extern WWMouseClass * DAT_00B78164;
extern HANDLE DAT_00B78168;
extern int DAT_00B7816C;
extern HANDLE DAT_00B78154;
extern DWORD DAT_00B78158;

#line 1274 "D:\\ra2mdpost\\wwmous.cpp"
static inline void Block_Mouse(void)
{
	if (WaitForSingleObject(DAT_00B78168, 10000) == WAIT_TIMEOUT) {
		FUN_004068E0("Warning: Probable deadlock occurred on MouseMutex. %s, line %d\n", __FILE__, __LINE__);
	}
}

static inline void Hide_System_Cursor(void)
{
	::ShowCursor(FALSE);
}

static inline void Unblock_Mouse(void)
{
	ReleaseMutex(DAT_00B78168);
}

// FUNCTION: 0x007B84D0
void __cdecl FUN_007B84D0(void)
{
	if (DAT_00B7816C == 0 && DAT_00B78164 != 0) {
		DAT_00B78164->FUN_007BA090();
	}
}


DWORD WINAPI FUN_007B8610(void * param);

// FUNCTION: 0x007B8730
WWMouseClass::WWMouseClass(DSurface * surface, HWND window) :
	Image(0),
	ImageFrameIndex(0),
	RefCount(-1),
	field_10(false),
	MouseX(0),
	MouseY(0),
	field_1C(0),
	field_20(0),
	Surface(surface),
	hWnd(window),
	Rect0(RECT_NONE),
	MouseXHot(0),
	MouseYHot(0),
	field_44(0),
	Rect1(RECT_NONE),
	field_58(0),
	Rect2(RECT_NONE),
	field_6C(0),
	Rect3(RECT_NONE),
	Rect4(RECT_NONE),
	field_90(-1),
	field_94(0)
{
	Block_Mouse();
	FUN_007B8960();
	MouseXHot = Rect0.X + Rect0.Width / 2;
	MouseYHot = Rect0.Y + Rect0.Height / 2;
	DAT_00B78164 = this;
	Unblock_Mouse();
	Hide_System_Cursor();
}

// FUNCTION: 0x007B88D0
WWMouseClass::~WWMouseClass(void)
{
	Block_Mouse();
	if (DAT_00B78164 == this) {
		DAT_00B78164 = 0;
	}
	Unblock_Mouse();
	if (field_44 != 0) {
		delete field_44;
		field_44 = 0;
	}
	if (field_58 != 0) {
		delete field_58;
		field_58 = 0;
	}
	if (field_6C != 0) {
		delete field_6C;
		field_6C = 0;
	}
}

// FUNCTION: 0x007B8960
void WWMouseClass::FUN_007B8960(void)
{
	RECT client;
	GetClientRect(hWnd, &client);
	POINT topleft;
	topleft.x = client.left;
	topleft.y = client.top;
	ClientToScreen(hWnd, &topleft);
	POINT bottomright;
	bottomright.x = client.right;
	bottomright.y = client.bottom;
	ClientToScreen(hWnd, &bottomright);
	Rect rect(topleft.x, topleft.y, bottomright.x - topleft.x, bottomright.y - topleft.y);
	Rect0 = rect;
	FUN_004068E0("Calc_Confining_Rect(%d,%d,%d,%d)\n", topleft.x, topleft.y, bottomright.x - topleft.x, bottomright.y - topleft.y);
}

// FUNCTION: 0x007B89F0
unsigned long WWMouseClass::GetRefCount(void)
{
	return RefCount;
}

// FUNCTION: 0x007B8C50
bool WWMouseClass::FUN_007B8C50(void)
{
	return Image ? true : false;
}

// FUNCTION: 0x007B8C60
bool WWMouseClass::FUN_007B8C60(void)
{
	if (Image != 0) {
		if (field_44 != 0) {
			if (Image->Get_Width() > field_44->Get_Width() || Image->Get_Height() > field_44->Get_Height()) {
				delete field_44;
				field_44 = 0;
			}
		}
		if (field_58 != 0) {
			if (Image->Get_Width() > field_58->Get_Width() || Image->Get_Height() > field_58->Get_Height()) {
				delete field_58;
				field_58 = 0;
			}
		}
		if (field_6C != 0) {
			if (Image->Get_Width() > field_6C->Get_Width() || Image->Get_Height() > field_6C->Get_Height()) {
				delete field_6C;
				field_6C = 0;
			}
		}
		if (Surface != 0) {
			if (field_44 == 0) {
				field_44 = new BSurface(Image->Get_Width(), Image->Get_Height(), Surface->Bytes_Per_Pixel());
			}
			if (field_58 == 0) {
				field_58 = new BSurface(Image->Get_Width(), Image->Get_Height(), Surface->Bytes_Per_Pixel());
			}
			if (field_6C == 0) {
				field_6C = new BSurface(Image->Get_Width(), Image->Get_Height(), Surface->Bytes_Per_Pixel());
			}
		}
		return(field_44 != 0 && field_58 != 0 && field_6C != 0);
	}
	return false;
}

// FUNCTION: 0x007B8E80
Rect WWMouseClass::FUN_007B8E80(void)
{
	Rect rect(0, 0, 0, 0);
	if (Image != 0) {
		Block_Mouse();
		rect = Image->GetFrameBounds(ImageFrameIndex);
		rect.X += MouseX - MouseXHot;
		rect.Y += MouseY - MouseYHot;
		Unblock_Mouse();
	}
	return rect;
}
