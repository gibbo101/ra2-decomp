// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include "wwmous.h"

#include "bsurface.h"
#include "convert.h"
#include "draw.h"
#include "globals.h"
#include "goptions.h"
#include "dsurface.h"
#include "surface.h"
#include "win.h"

// SYNTHETIC: GAMEMD 0x007B84B0 SYMBOL
// _$E2

void __cdecl FUN_004068E0(char const * format, ...);

struct MouseThreadInfo {
	volatile int Stop;
	volatile int Count;
	volatile int Done;
	int Pad;
	volatile __int64 Delay;
	volatile int Running;

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
extern DSurface * DAT_00887300;
extern int DAT_00886FB8;
extern ConvertClass * FileSystem_MOUSE_PAL;

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

// FUNCTION: GAMEMD 0x007B84D0
void __cdecl FUN_007B84D0(void)
{
	if (DAT_00B7816C == 0 && DAT_00B78164 != 0) {
		DAT_00B78164->FUN_007BA090();
	}
}


DWORD WINAPI FUN_007B8610(void * param);

// FUNCTION: GAMEMD 0x007B84F0
void __cdecl FUN_007B84F0(void)
{
	char message[1024];
	if (DAT_00B78168 == 0) {
		DAT_00B78168 = CreateMutexA(0, 0, 0);
	}
	if (DAT_00B7816C == 0 && DAT_00B78168 != 0) {
		DAT_00B78138.Reset();
		DAT_00B78154 = CreateThread(0, 0x1000, FUN_007B8610, &DAT_00B78138, 0, &DAT_00B78158);
		if (DAT_00B78154 != 0) {
			DAT_00B7816C = 1;
			if (SetThreadPriority(DAT_00B78154, 15) == 0) {
				FormatMessageA(0x1000, 0, GetLastError(), 0, message, 0x400, 0);
				FUN_004068E0("Unable to change the priority of the mouse thread - %s\n", message);
				while (DAT_00B78138.Done == 0) {
					Sleep(0);
				}
				WaitForSingleObject(DAT_00B78154, 5000);
				CloseHandle(DAT_00B78154);
				DAT_00B7816C = 0;
			}
		}
	}
}

// FUNCTION: GAMEMD 0x007B8610
DWORD WINAPI FUN_007B8610(void * param)
{
	MouseThreadInfo * info = (MouseThreadInfo *)param;
	info->Running = 1;
	info->Done = 0;
	if (info->Stop == 0) {
		do {
			Block_Mouse();
			if (DAT_00B78164 != 0) {
				DAT_00B78164->FUN_007BA090();
			}
			Unblock_Mouse();
			Sleep((DWORD)info->Delay);
			info->Count++;
		} while (info->Stop == 0);
	}
	info->Done = 1;
	return 0;
}

// FUNCTION: GAMEMD 0x007B86B0
void __cdecl FUN_007B86B0(void)
{
	if (DAT_00B7816C != 0) {
		DAT_00B78138.Stop = 1;
		while (DAT_00B78138.Done == 0) {
			Sleep(0);
		}
		WaitForSingleObject(DAT_00B78154, 5000);
		CloseHandle(DAT_00B78154);
		DAT_00B7816C = 0;
	}
	if (DAT_00B78168 != 0) {
		CloseHandle(DAT_00B78168);
		DAT_00B78168 = 0;
	}
}

// FUNCTION: GAMEMD 0x007B8730
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

// FUNCTION: GAMEMD 0x007B88D0
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

// FUNCTION: GAMEMD 0x007B8960
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

// FUNCTION: GAMEMD 0x007B89F0
int WWMouseClass::Get_Mouse_State(void) const
{
	return RefCount;
}

// FUNCTION: GAMEMD 0x007B8C50
bool WWMouseClass::FUN_007B8C50(void)
{
	return Image ? true : false;
}

// FUNCTION: GAMEMD 0x007B8C60
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

// FUNCTION: GAMEMD 0x007B8E80
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

// FUNCTION: GAMEMD 0x007B8F30
void WWMouseClass::Save_Background(void)
{
	if (FUN_007B8C60() && Surface != 0) {
		Rect1 = FUN_007B8E80();
		Rect rect = Rect1;
		rect.X += Rect0.X;
		rect.Y += Rect0.Y;
		field_44->Blit_From(Rect(0, 0, rect.Width, rect.Height), *Surface, rect, false, true);
	}
}

// FUNCTION: GAMEMD 0x007B9040
void WWMouseClass::Restore_Background(void)
{
	if (Rect1.Is_Valid() && Surface != 0 && field_44 != 0) {
		Rect rect = Rect1;
		rect.X += Rect0.X;
		rect.Y += Rect0.Y;
		Surface->Blit_From(rect, *field_44, Rect(0, 0, rect.Width, rect.Height), false, true);
	}
}

// FUNCTION: GAMEMD 0x007B90C0
void WWMouseClass::Draw_Mouse(::Surface * surface, bool issidebarsurface)
{
	BSurface * savesurface;
	Rect * savedregion;
	int xoffset;

	if (!Is_Hidden() && Is_Captured() && surface != 0 && surface != Surface) {
		if (issidebarsurface) {
			if (Options.IsSidebarOnRight) {
				xoffset = -DSurface::ViewBounds.Width;
			} else {
				xoffset = 0;
			}
			savesurface = field_6C;
			savedregion = &Rect3;
		} else {
			if (!Options.IsSidebarOnRight) {
				if (-DAT_00887300->Get_Width() > surface->Get_Width() - DAT_00886FB8) {
					xoffset = -DAT_00887300->Get_Width();
				} else {
					xoffset = surface->Get_Width() - DAT_00886FB8;
				}
			} else {
				xoffset = 0;
			}
			savesurface = field_58;
			savedregion = &Rect2;
		}

		if (savesurface != 0 && field_44 != 0) {
			Block_Mouse();
			*savedregion = Rect1;
			savedregion->X += xoffset;
			savesurface->Blit_From(Rect(0, 0, savedregion->Width, savedregion->Height), *surface, *savedregion, false, true);
			if (issidebarsurface && savedregion->X < 0 && -savedregion->X < field_44->Get_Width()) {
				field_44->Blit_From(Rect(-savedregion->X, 0, savedregion->Width, savedregion->Height), *surface, Rect(0, savedregion->Y, savedregion->Width, savedregion->Height), false, true);
			} else {
				field_44->Blit_From(Rect(0, 0, savedregion->Width, savedregion->Height), *surface, *savedregion, false, true);
			}
			Raw_Draw_Mouse((DSurface *)surface, xoffset, 0);
			Unblock_Mouse();
		} else {
			savedregion->Width = 0;
		}
	}
}

// FUNCTION: GAMEMD 0x007B92D0
void WWMouseClass::Erase_Mouse(::Surface * surface, bool issidebarsurface)
{
	if (!Is_Hidden() && Is_Captured() && surface != 0 && surface != Surface && field_58 != 0 && field_6C != 0) {
		BSurface * savesurface;
		Rect savedregion;

		if (issidebarsurface) {
			savesurface = field_6C;
			savedregion = Rect3;
		} else {
			savesurface = field_58;
			savedregion = Rect2;
		}

		if (savesurface != 0) {
			Block_Mouse();
			surface->Blit_From(savedregion, *savesurface, Rect(0, 0, savedregion.Width, savedregion.Height), false, true);
			Unblock_Mouse();
		}
	}
}

// FUNCTION: GAMEMD 0x007B93F0
void WWMouseClass::Raw_Draw_Mouse(DSurface * surface, int xoffset, int yoffset)
{
	if (Image != 0 && surface != 0 && FileSystem_MOUSE_PAL != 0) {
		Rect rect = Rect1;
		rect.X += xoffset;
		rect.Y += yoffset;
		BSurface mdata(rect.Width, rect.Height, 1, Image->GetPixels(ImageFrameIndex));
		if (surface == Surface) {
			Blit_Block(*surface, *FileSystem_MOUSE_PAL, mdata, Rect(0, 0, rect.Width, rect.Height), Point2D(rect.X, rect.Y), Rect0);
		} else {
			Blit_Block(*surface, *FileSystem_MOUSE_PAL, mdata, Rect(0, 0, rect.Width, rect.Height), Point2D(rect.X, rect.Y), surface->Get_Rect());
		}
	}
}

// FUNCTION: GAMEMD 0x007B9530
void WWMouseClass::Low_Show_Mouse(void)
{
	Block_Mouse();
	Save_Background();
	Raw_Draw_Mouse(Surface, 0, 0);
	Unblock_Mouse();
}

// FUNCTION: GAMEMD 0x007B9690
void WWMouseClass::Low_Hide_Mouse(void)
{
	Block_Mouse();
	Restore_Background();
	Unblock_Mouse();
}

// FUNCTION: GAMEMD 0x007B9750
void WWMouseClass::Show_Mouse(void)
{
	if (RefCount != 0) {
		Block_Mouse();
		RefCount++;
		if (RefCount == 0) {
			if (Is_Captured()) {
				Low_Show_Mouse();
			} else {
				while (::ShowCursor(TRUE) < 0) {}
			}
		}
		if (RefCount > 0) {
			RefCount = 0;
		}
		Unblock_Mouse();
	}
}

// FUNCTION: GAMEMD 0x007B9930
void WWMouseClass::Hide_Mouse(void)
{
	Block_Mouse();
	if (RefCount == 0) {
		if (Is_Captured()) {
			Low_Hide_Mouse();
		} else {
			while (::ShowCursor(FALSE) >= 0) {}
		}
	}
	RefCount--;
	Unblock_Mouse();
}

// FUNCTION: GAMEMD 0x007B9A60
void WWMouseClass::Capture_Mouse(void)
{
	if (this != 0 && !Is_Captured()) {
		FUN_004068E0("Capture_Mouse()\n");
		Block_Mouse();
		field_10 = true;
		if (!Is_Hidden()) {
			Low_Show_Mouse();
		}
		::ShowCursor(FALSE);
		Unblock_Mouse();
	}
}

// FUNCTION: GAMEMD 0x007B9C30
void WWMouseClass::Release_Mouse(void)
{
	if (this != 0 && Is_Captured()) {
		FUN_004068E0("Release_Mouse()\n");
		Block_Mouse();
		field_10 = false;
		if (!Is_Hidden()) {
			Low_Hide_Mouse();
			while (::ShowCursor(TRUE) < 0) {}
		}
		Unblock_Mouse();
	}
}

// FUNCTION: GAMEMD 0x007B9D70
void WWMouseClass::Conditional_Hide_Mouse(Rect region)
{
	Hide_Mouse();
}

extern bool AllowNegativeY;
extern bool DAT_00B78170;
int __fastcall FUN_0053E720(void);

// FUNCTION: GAMEMD 0x007B9D80
// WWMouseClass::Conditional_Show_Mouse
void WWMouseClass::Conditional_Show_Mouse(void)
{
	Show_Mouse();
}

// FUNCTION: GAMEMD 0x007B9D90
void WWMouseClass::Convert_Coordinate(int & x, int & y) const
{
	x -= Rect0.X;
	y -= Rect0.Y;
	if (x < 0) x = 0;
	if (y < 0 && !AllowNegativeY) y = 0;
	if (x >= Rect0.Width) x = Rect0.Width - 1;
	if (y >= Rect0.Height) y = Rect0.Height - 1;
}

// FUNCTION: GAMEMD 0x007B9DF0
void WWMouseClass::Get_Bounded_Position(int & x, int & y) const
{
	POINT pt;
	GetCursorPos(&pt);
	x = pt.x;
	y = pt.y;
	Convert_Coordinate(x, y);
}

// FUNCTION: GAMEMD 0x007B9E30
void WWMouseClass::Update_Mouse_Position(int x, int y, bool forced)
{
	Block_Mouse();
	if (x != MouseX || y != MouseY || forced) {
		if (Is_Captured() && !Is_Hidden() && GameInFocus) {
			Low_Hide_Mouse();
		}
		MouseX = x;
		MouseY = y;
		if (Is_Captured() && !Is_Hidden() && GameInFocus) {
			Low_Show_Mouse();
		}
	}
	Unblock_Mouse();
}

// FUNCTION: GAMEMD 0x007BA090
void WWMouseClass::FUN_007BA090(void)
{
	if (Surface != 0) {
		Block_Mouse();
		int x;
		int y;
		if (FUN_0053E720()) {
			x = field_1C;
			y = field_20;
		} else {
			Get_Bounded_Position(x, y);
			field_1C = x;
			field_20 = y;
		}
		if (!Surface->entry_64() && !DAT_00B78170) {
			MouseX = x;
			MouseY = y;
			DAT_00B78170 = true;
		} else {
			Update_Mouse_Position(x, y, DAT_00B78170);
			DAT_00B78170 = false;
		}
		Unblock_Mouse();
	}
}

// FUNCTION: GAMEMD 0x007BA2C0
void __cdecl FUN_007BA2C0(void)
{
	Block_Mouse();
}

// FUNCTION: GAMEMD 0x007BA2F0
void __cdecl FUN_007BA2F0(void)
{
	Unblock_Mouse();
}
