// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "winstub.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tooltip.h"
#include "ccfile.h"
#include "cctooltip.h"
#include "cdcntrl.h"
#include "convert.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "globals.h"
#include "init.h"
#include "misc.h"
#include "movie.h"
#include "movies.h"
#include "pcx.h"
#include "resource.h"
#include "theme.h"
#include "win.h"
#include "windlg.h"
#include "winfix.h"
#include "wsproto.h"
#include "wwmouse.h"
#include "mainopt.h"
#include "conquer.h"
#include <commctrl.h>
#include <math.h>

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1718;
extern const double DAT_007F74B0;
extern const double DAT_007F74B8;

// FUNCTION: 0x007771D0 $E67
double DAT_00B730D0 = FUN_004CAC40(pow(256.0, 2.0) * 2);

// FUNCTION: 0x00777200 $E70
double DAT_00B73538 = DAT_00B730D0 / DAT_007F74B0;

// FUNCTION: 0x00777220 $E73
double DAT_00B73520 = DAT_007E1718 / DAT_00B73538;

// FUNCTION: 0x00777240 $E76
double DAT_00B73310 = DAT_007F74B8 * 45.0;

// FUNCTION: 0x00777260 $E79
double DAT_00B730D8 = DAT_007F74B8 * 60.0;

// FUNCTION: 0x00777280 $E82
double DAT_00B730B0 = DAT_007F74B8 * 90.0;

// FUNCTION: 0x007772A0 $E85
int DAT_00B73540 = (int)(FUN_004CAD50(DAT_00B730B0 - DAT_00B730D8) * DAT_00B730D0 * 0.5);

// FUNCTION: 0x007772E0 $E88
double DAT_00B730E0 = FUN_004CADE0(DAT_00B73540 * 0.00390625);

// FUNCTION: 0x00777310 $E91
double DAT_00B73308 = FUN_004CADE0((DAT_00B73540 * 2) / DAT_00B730D0);

// FUNCTION: 0x00777340 $E94
int DAT_00B73534 = (int)(DAT_00B73540 * 4 + 0.5);

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x00777370 $E97
ShortPair DAT_00B730C8;

// FUNCTION: 0x00777380 $E100
IntTriple DAT_00B732F8;

namespace Game { extern HWND hWnd; extern HINSTANCE hInstance; extern void * hIMC; extern bool IsFocused; extern int SpecialDialog; }
namespace Unsorted { extern bool ScenarioStarted; extern int CurrentFrame; extern bool ArmageddonMode; }

void __cdecl FUN_004068E0(char const * format, ...);
void __fastcall FUN_005BF500(bool state);
void FUN_00407020(void);
void FUN_00407040(void);

class MouseSlots {
	public:
		virtual void Slot0(void);
		virtual void Slot1(void);
		virtual void Slot2(void);
		virtual void Slot3(void);
		virtual void Slot4(void);
		virtual void Release_Mouse(void);
		virtual void Capture_Mouse(void);
		virtual bool Is_Captured(void);
		void FUN_007B8960(void);
};
extern MouseSlots * DAT_00887640;
extern bool DAT_00B73560;
extern int DAT_00B73530;
extern int DAT_00B73528;
extern int DAT_00B7352C;

// FUNCTION: 0x007773A0
void FUN_007773A0(void)
{
	DAT_00B73530 = 0;
	DAT_00B73528 = 0x80;
	DAT_00B7352C = 0x80;
}

// FUNCTION: 0x007773C0
void Focus_Loss(void)
{
	FUN_004068E0("Focus_Loss()\n");
	FUN_005BF500(true);
	FUN_00407020();
	if (DAT_00887640) {
		DAT_00B73560 = DAT_00887640->Is_Captured();
		FUN_004068E0("Focus_Loss(): _MouseCaptured = %s\n", DAT_00B73560 ? "true" : "false");
		DAT_00887640->Release_Mouse();
	}
}

HWND FUN_00775B10(void);
extern bool DAT_00884B8F;

// FUNCTION: 0x00777420
void Focus_Restore(void)
{
	FUN_004068E0("Focus_Restore()\n");
	FUN_00407040();
	FUN_004068E0("Focus_Restore(): _MouseCaptured = %s\n", DAT_00B73560 ? "true" : "false");
	if (DAT_00887640 && DAT_00B73560 == true && !Unsorted::ArmageddonMode) {
		DAT_00887640->Capture_Mouse();
	}
	if (!WindowedMode) {
		if (AlternateSurface != NULL) {
			((DSurface *)AlternateSurface)->Restore_Check();
		}
		if (HiddenSurface != NULL) {
			((DSurface *)HiddenSurface)->Restore_Check();
		}
		if (CompositeSurface != NULL && ((DSurface *)CompositeSurface)->Restore_Check()) {
			if (!Unsorted::ScenarioStarted || Unsorted::CurrentFrame > 16) {
				CompositeSurface->Fill(0);
			}
		}
		if (TileSurface != NULL && ((DSurface *)TileSurface)->Restore_Check()) {
			if (!Unsorted::ScenarioStarted || Unsorted::CurrentFrame > 16) {
				TileSurface->Fill(0);
			}
		}
		if (SidebarSurface != NULL && ((DSurface *)SidebarSurface)->Restore_Check()) {
			DAT_00884B8F = true;
			if (!Unsorted::ScenarioStarted || Unsorted::CurrentFrame > 16) {
				SidebarSurface->Fill(0);
			}
		}
		if (VisibleSurface != NULL && ((DSurface *)VisibleSurface)->Restore_Check()) {
			if (!Unsorted::ScenarioStarted || Unsorted::CurrentFrame > 16) {
				VisibleSurface->Fill_Rect(VisibleRect, 0);
			}
		}
	}
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	InvalidateRect(Game::hWnd, 0, 0);
	FUN_005BF500(false);
	if (FUN_00775B10()) {
		SetActiveWindow(FUN_00775B10());
		SetFocus(FUN_00775B10());
	}
}

namespace Game { bool __fastcall IsMoviePlaying(void); void __fastcall BlitMovie(void); }
bool FUN_00774070(void);
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)
#endif
#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif
extern int DAT_00822D1C;
extern bool DAT_00B73561;
extern bool DAT_00884D34;
extern bool DAT_008A00A0;
extern bool DAT_00A8ED81;
extern int DAT_00A8EDD8;
extern int DAT_00B73570;
extern wchar_t DAT_00B730EC[];
extern MouseSlots * DAT_00887640;

namespace OwnerDraw { int __fastcall ServiceIMEMessage(HWND window, UINT message, WPARAM wparam, LPARAM lparam); }

// FUNCTION: 0x007775C0
LRESULT CALLBACK Windows_Procedure(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	if (PacketTransport) {
		if (message == (UINT) PacketTransport->Protocol_Event_Message(INVALID_SOCKET) || message == 0x475) {
			if (PacketTransport->Message_Handler(hwnd, message, wParam, lParam)) {
				return(DefWindowProc(hwnd, message, wParam, lParam));
			} else {
				return(0);
			}
		}
	}

	Map.Message_Handler(hwnd, message, wParam, lParam);

	if (Game::hWnd) {
		GetMenu(Game::hWnd);
	}

	if (OwnerDraw::ServiceIMEMessage(hwnd, message, wParam, lParam)) {
		FUN_004068E0("Default\n");
		return(DAT_00B73570);
	}

	switch (message) {
		case WM_SHOWWINDOW:
			return(0);

		case WM_PAINT:
			if (Game::SpecialDialog == 0) {
				if (Game::IsFocused == true || WindowedMode == true) {
					if (DAT_00887640 != NULL && VisibleSurface != NULL && HiddenSurface != NULL && CompositeSurface != NULL) {
						if (Unsorted::ScenarioStarted == true) {
							Update_Visible_Surface(DAT_00887640->Is_Captured(), CompositeSurface);
							Map.Blit_Sidebar(true);
							ValidateRect(hwnd, NULL);
							break;
						} else if (Game::IsMoviePlaying()) {
							Game::BlitMovie();
							ValidateRect(hwnd, NULL);
							break;
						} else if (!FUN_00774070()) {
							Update_Visible_Surface(DAT_00887640->Is_Captured(), HiddenSurface);
						}
					}
				}
			}
			ValidateRect(hwnd, NULL);
			break;

		case WM_ERASEBKGND:
			return(1);

		case WM_CREATE:
			ToolTips = new CCToolTip(hwnd);
			break;

		case WM_MOVE:
			if (WindowedMode == true && DAT_00887640 != NULL) {
				DAT_00887640->FUN_007B8960();
			}
			break;

		case WM_DESTROY:
			if (ToolTips != NULL) {
				delete ToolTips;
				ToolTips = NULL;
			}
			Game::hWnd = 0;
			switch (DAT_00A8EDD8) {
				default:
				case 1:
					DAT_00A8EDD8 = 2;
					break;

				case 0:
					break;
			}
			return(0);

		case WM_SYSCOMMAND:
			switch (wParam) {
				case SC_CLOSE:
					return(0);

				case SC_MINIMIZE:
					DAT_00A8ED81 = true;
					break;

				case SC_RESTORE:
					DAT_00A8ED81 = false;
					break;

				case SC_SCREENSAVE:
					return(0);
			}
			break;

		case WM_ACTIVATEAPP:
			if (hwnd == Game::hWnd && Game::IsFocused != (wParam != 0)) {
				Game::IsFocused = (wParam != 0);
				if (!Game::IsFocused) {
					Focus_Loss();
					FUN_004068E0("Focus lost\n");
				} else {
					Focus_Restore();
					FUN_004068E0("Focus gained\n");
				}
				DAT_008A00A0 = true;
			}
			return(0);

		case WM_RBUTTONUP:
			DAT_00884D34 = false;
			break;

		case WM_MOVING:
			return(On_WM_MOVING(hwnd, wParam, lParam));

		case WM_MOUSEWHEEL:
			if (!DAT_00B73561) {
				DAT_00B73561 = true;
				if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
					Execute_Command("SidebarDown");
				} else {
					Execute_Command("SidebarUp");
				}
				DAT_00B73561 = false;
			}
			break;
	}

	if (Keyboard->Message_Handler(hwnd, message, wParam, lParam)) {
		return(0);
	}

	if (message == WM_IME_CHAR) {
		int length = wcslen(DAT_00B730EC);
		char mb[4];
		int wc;
		if (wParam & 0xFF00) {
			mb[0] = (char)(wParam >> 8);
			mb[1] = (char)wParam;
			mb[2] = 0;
		} else {
			mb[0] = (char)wParam;
			mb[1] = 0;
		}
		int converted = MultiByteToWideChar(0, 0, mb, strlen(mb), (LPWSTR)&wc, 1);
		DAT_00B730EC[length] = (converted == 1) ? wc : 0;
		DAT_00B730EC[length + 1] = 0;
		return(0);
	}

	return(DefWindowProc(hwnd, message, wParam, lParam));
}

// FUNCTION: 0x00777AB0
void FUN_00777AB0(void)
{
}

// FUNCTION: 0x00777AC0
void Build_Date_String(char * buffer, int buflen)
{
	SYSTEMTIME systime;

	if (FileTimeToSystemTime ((LPFILETIME)(&BuildDate[28]), &systime) ) {
		sprintf(buffer, "%02d/%02d/%04d - %02d:%02d:%02d", systime.wMonth, systime.wDay, systime.wYear, systime.wHour, systime.wMinute, systime.wSecond);
	}
}

// FUNCTION: 0x00777B30
char *Build_Number_String(char * buffer, int buflen)
{
	sprintf (buffer, "%d", *(unsigned int*)(&BuildNumber[28]));
	return(buffer);
}

// FUNCTION: 0x00777B50
int FUN_00777B50(void)
{
	return(DAT_00822D1C);
}

// FUNCTION: 0x00777B60
char *Build_By_String(char * buffer, int buflen)
{
	strncpy(buffer, &BuildNumber[32], 32);
	return(buffer);
}

// FUNCTION: 0x00777B80
char *Build_By_Initials(char * buffer, int buflen)
{
	buffer[0] = BuildNumber[32];
	buffer[1] = 0;
	buffer[2] = 0;

	char *lastname = strchr(&BuildNumber[32], '_');
	if (lastname) {
		buffer[1] = lastname[1];
	}
	return(buffer);
}

// FUNCTION: 0x00777BB0
char *Build_Version_String(char * buffer, int buflen)
{
	char _buffer1[128];
	char _buffer2[128];

	sprintf(buffer, "%s-%s", Build_By_Initials(_buffer2, sizeof(_buffer2)), Build_Number_String(_buffer1, sizeof(_buffer1)));
	return(buffer);
}

extern HICON DAT_00B7354C;
extern HCURSOR DAT_00B73548;
extern int DAT_00B73544;
extern wchar_t DAT_00B73518;
void * __stdcall FUN_007C8A22(HWND window);
void * __stdcall FUN_007C8A1C(HWND window, void * context);
int __stdcall FUN_007C8A28(void * context, int action, int index, int value);
unsigned int __stdcall FUN_007C8A2E(void * context, unsigned int index, void * list, unsigned int length);
int __stdcall FUN_007C8A34(void * context, unsigned int type, void * buffer, unsigned int length);
extern "C" unsigned int __cdecl _mbsnccnt(const unsigned char * string, unsigned int count);

namespace OwnerDraw {
	extern wchar_t IMECompositionString[];
	extern int IMECompositionStringLength;
	extern int IMECompositionCursorPos;
	extern int IMEComposing;
	void CancelIMEComposition(void);
	void UpdateIMECompositionString(void);
}

// FUNCTION: 0x00777C30
void Create_Main_Window(HINSTANCE instance, int command_show, int width, int height)
{
	InitCommonControls();

	DAT_00B7354C = LoadIcon(instance, MAKEINTRESOURCE(93));
	DAT_00B73548 = LoadCursor(Game::hInstance, MAKEINTRESOURCE(104));

	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = Windows_Procedure;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = instance;
	wndclass.hIcon = DAT_00B7354C;
	wndclass.hCursor = DAT_00B73548;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "Yuri's Revenge";

	RegisterClass(&wndclass);

	if (WindowedMode) {
		Game::hWnd = CreateWindowEx(0, "Yuri's Revenge", "Yuri's Revenge", WS_SYSMENU|WS_CAPTION|WS_MINIMIZEBOX|WS_CLIPCHILDREN, 0, 0, 0, 0, NULL, NULL, instance, NULL);

		RECT rect;
		SetRect(&rect, 0, 0, width, height);
		AdjustWindowRectEx(&rect, GetWindowLong(Game::hWnd, GWL_STYLE), GetMenu(Game::hWnd) != 0, GetWindowLong(Game::hWnd, GWL_EXSTYLE));
		MoveWindow(Game::hWnd, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 1);
	} else {
		Game::hWnd = CreateWindowEx(WS_EX_TOPMOST, "Yuri's Revenge", "Yuri's Revenge", WS_POPUP|WS_CLIPCHILDREN, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, instance, NULL);
	}

	Game::hIMC = FUN_007C8A22(Game::hWnd);
	FUN_007C8A1C(Game::hWnd, NULL);

	ShowWindow(Game::hWnd, SW_NORMAL);
	DAT_00B73544 = command_show;
	UpdateWindow(Game::hWnd);
	SetFocus(Game::hWnd);

	RegisterHotKey(Game::hWnd, 1, MOD_ALT|MOD_CONTROL|MOD_SHIFT, 'M');

	SetCursor(LoadCursor(Game::hInstance, MAKEINTRESOURCE(104)));
}

// FUNCTION: 0x00777E00
void OwnerDraw::CancelIMEComposition(void)
{
	IMECompositionString[0] = 0;
	IMECompositionStringLength = 0;
	IMECompositionCursorPos = 0;
	IMEComposing = 0;
	DAT_00B730EC[0] = 0;
	if (Game::hIMC) {
		FUN_007C8A28(Game::hIMC, 0x15, 4, 0);
	}
}

struct IMECandidateList {
	unsigned long Size;
	unsigned long Style;
	unsigned long Count;
	unsigned long Selection;
	unsigned long PageStart;
	unsigned long PageSize;
	unsigned long Offset[1];
};

// FUNCTION: 0x00777E40
void __fastcall FUN_00777E40(void * context, unsigned int index, char * buffer, int length)
{
	if (context) {
		unsigned int size = FUN_007C8A2E(context, index, NULL, 0);
		if (size) {
			IMECandidateList * list = (IMECandidateList *)malloc(size);
			if (list) {
				if (list->Count && list->Selection < list->Count) {
					strncat(buffer, (char const *)list->Offset[list->Selection], length);
					buffer[length - 1] = 0;
				}
				free(list);
			}
		}
	}
}

// FUNCTION: 0x00777EA0
void OwnerDraw::UpdateIMECompositionString(void)
{
	IMECompositionString[0] = 0;
	IMECompositionStringLength = 0;
	IMECompositionCursorPos = 0;
	if (Game::hIMC) {
		char buffer[513];
		int size = FUN_007C8A34(Game::hIMC, 8, buffer, 512);
		if (size > 0) {
			buffer[size] = 0;
			int wide = MultiByteToWideChar(0, 0, buffer, -1, IMECompositionString, 256);
			DAT_00B73518 = 0;
			int length;
			if (wide < 0) {
				length = 0;
			} else {
				IMECompositionCursorPos = FUN_007C8A34(Game::hIMC, 0x80, NULL, 0) & 0xFFFF;
				length = wcslen(IMECompositionString);
			}
			int cursor = _mbsnccnt((unsigned char *)buffer, IMECompositionCursorPos);
			IMECompositionCursorPos = cursor;
			IMECompositionString[length] = 0;
			IMECompositionStringLength = length;
			if (cursor > length) {
				IMECompositionCursorPos = length;
			} else if (cursor < 0) {
				IMECompositionCursorPos = 0;
			}
		}
	}
}

// FUNCTION: 0x00777F90
void __fastcall FUN_00777F90(HWND window)
{
	if (Game::hIMC) {
		char buffer[513];
		int size = FUN_007C8A34(Game::hIMC, 0x800, buffer, 512);
		if (size > 0) {
			buffer[size] = 0;
			wchar_t wide[257];
			int count = MultiByteToWideChar(0, 0, buffer, strlen(buffer), wide, 256);
			if (count < 0) {
				count = 0;
			}
			wide[count] = 0;
			for (int i = 0; i < count; i++) {
				SendMessage(window, 0x4D9, wide[i], wide[i]);
			}
		}
	}
}

// FUNCTION: 0x00778030
int __fastcall OwnerDraw::ServiceIMEMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case 0x10F:
			UpdateIMECompositionString();
			DAT_00B73570 = DefWindowProc(hwnd, 0x10F, wParam, lParam);
			return(1);

		case 0x10E:
			UpdateIMECompositionString();
			IMEComposing = 0;
			DAT_00B73570 = DefWindowProc(hwnd, 0x10E, wParam, lParam);
			return(1);

		case 0x10D:
			UpdateIMECompositionString();
			DAT_00B730EC[0] = 0;
			IMEComposing = 1;
			DAT_00B73570 = DefWindowProc(hwnd, 0x10D, wParam, lParam);
			return(1);

		case 0x281:
			DAT_00B73570 = DefWindowProc(hwnd, 0x281, wParam, lParam);
			return(1);

		case 0x282:
		case 0x285:
		case 0x290:
		case 0x291:
			DAT_00B73570 = DefWindowProc(hwnd, message, wParam, lParam);
			return(1);
	}
	return(0);
}
