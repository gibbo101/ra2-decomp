// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "facing.h"
#include <math.h>

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007E8828;
extern const double DAT_007E8830;
extern const double DAT_007E8890;

// FUNCTION: 0x004C81F0 $E2
double DAT_008B3A20 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004C8220 $E5
double DAT_008B3A70 = DAT_008B3A20 / DAT_007E8828;

// FUNCTION: 0x004C8240 $E8
double DAT_008B3A58 = DAT_007E1718 / DAT_008B3A70;

// FUNCTION: 0x004C8260 $E11
double DAT_008B3A50 = DAT_007E1720 * DAT_007E8830;

// FUNCTION: 0x004C8280 $E14
double DAT_008B3A28 = DAT_007E1728 * DAT_007E8830;

// FUNCTION: 0x004C82A0 $E17
double DAT_008A3A00 = DAT_007E1730 * DAT_007E8830;

// FUNCTION: 0x004C82C0 $E20
int DAT_008B3A78 = (int)(fastmath::tan(DAT_008A3A00 - DAT_008B3A28) * DAT_007E1738 * DAT_008B3A20);

// FUNCTION: 0x004C8300 $E23
double DAT_008B3A30 = fastmath::atan(DAT_008B3A78 * DAT_007E1740);

// FUNCTION: 0x004C8330 $E26
double DAT_008B3A48 = fastmath::atan((DAT_008B3A78 * 2) / DAT_008B3A20);

// FUNCTION: 0x004C8360 $E29
int DAT_008B3A6C = (int)(DAT_008B3A78 * 4 + DAT_007E1738);

struct FacingShortPair { short a; short b; FacingShortPair(void) { a = 0; b = 0; } };
struct FacingIntTriple { int a; int b; int c; FacingIntTriple(void) { a = 0; b = 0; c = 0; } };

// FUNCTION: 0x004C8390 $E32
FacingShortPair DAT_008B3A18;

// FUNCTION: 0x004C83A0 $E35
FacingIntTriple DAT_008B3A38;


struct FacingBias { int x; int y; int z; FacingBias(void) { z = 0; x = 0x80; y = 0x80; } };

// FUNCTION: 0x004C83C0 $E38
FacingBias DAT_008B3A60;


#include "expand.h"
#include "ccfile.h"
#include "_surface.h"
#include "dbgprint.h"
#include "language\language.h"
#include "ownrdraw.h"
#include "surface.h"
#include "version.h"
#include "win.h"
#include "windlg.h"
#include "winfix.h"
#include "data.h"
#include <conio.h>

#define EH_BUFFER_SIZE 65536

void __fastcall FUN_0053E3C0(HWND window);
void __fastcall FUN_0053E420(HWND window);
void __fastcall FUN_0053E6B0(void);

static char ExceptionText[EH_BUFFER_SIZE];
int ExceptionRecursions = -1;
bool UnusedEHBool = true;

static bool Any_Locked(void)
{
	if (VisibleSurface && VisibleSurface->Is_Locked() ||
		HiddenSurface && HiddenSurface->Is_Locked() ||
		CompositeSurface && CompositeSurface->Is_Locked() ||
		TileSurface && TileSurface->Is_Locked() ||
		SidebarSurface && SidebarSurface->Is_Locked() ||
		AlternateSurface && AlternateSurface->Is_Locked()) {
		return(true);
	}

	return(false);
}

void __fastcall Dump_Exception_Info(EXCEPTION_POINTERS * e_info);

// FUNCTION: 0x004C83E0
int CALLBACK Exception_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_INITDIALOG:
			DebugString("WM_INITDIALOG\n");

			SetDlgItemText(window, IDC_EXCEPT_DETAILS, ExceptionText);
			SetFocus(window);
			if (MainWindow) {
				Center_Window_Within_Window(window, MainWindow);
			}
			ShowWindow(window, SW_NORMAL);
			FUN_0053E3C0(window);
			break;

		case WM_CLOSE:
			EndDialog(window, IDC_EXCEPT_QUIT);
			FUN_0053E420(window);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDC_EXCEPT_SAVE:
					MessageBox(window, "Sorry, you can only attempt an emergency save if you were in the map editor when the exception occurred.\t", "Bummer", MB_ICONWARNING);
					break;

				case IDC_EXCEPT_QUIT:
					EndDialog(window, IDC_EXCEPT_QUIT);
					break;
			};
			break;

		case WM_MOVING:
			return(On_WM_MOVING(window, wparam, lparam));

		default:
			break;
	}

	return(0);
}

// FUNCTION: 0x004C84E0
int Exception_Dialog(void)
{
	int retval = 0;

	FUN_0053E6B0();
	switch (ExceptionRecursions) {

		default: {
			ShowCursor(TRUE);

			DLGTEMPLATE const * dialogt = (DLGTEMPLATE const *)Fetch_Resource(MAKEINTRESOURCE(IDD_EXCEPTION_SIMPLE), MAKEINTRESOURCE(5));
			if (dialogt != NULL) {
				DebugString("Resource found\n");
				retval = DialogBoxIndirectParam(ProgramInstance, dialogt, MainWindow, Exception_Proc, 0);
			} else {
				DebugString("Resource not found\n");
				retval = DialogBoxParam(ProgramInstance, MAKEINTRESOURCE(IDD_EXCEPTION_SIMPLE), MainWindow, Exception_Proc, 0);
			}
			ShowCursor(FALSE);
			break;
		}

		case 3:
			ExitProcess(EXIT_SUCCESS);
			retval = IDC_EXCEPT_QUIT;
			break;

		case 2:
			retval = IDC_EXCEPT_QUIT;
			break;

		case 1: {
			if (!AllocConsole()) {
				FreeConsole();
				AllocConsole();
			}
			cprintf("Error - Recursive exception detected. Program halted.\r\n See EXCEPT.TXT file for details\r\r\n");
			Sleep(4000);
			getch();
			FreeConsole();
			retval = IDC_EXCEPT_QUIT;
			break;
		}
	}

	return(retval);
}


// FUNCTION: 0x004C8FE0
int Exception_Handler(int exception_code, EXCEPTION_POINTERS *e_info)
{
	DebugString("Exception!\n");

	FUN_0053E6B0();
	ExceptionRecursions++;

	exception_code = exception_code;

	if (ExceptionRecursions == 0) {
		Dump_Exception_Info(e_info);

		HANDLE debug_file;
		DWORD	actual;
		debug_file = CreateFile("except.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (debug_file != INVALID_HANDLE_VALUE){
			WriteFile(debug_file, ExceptionText, lstrlen(ExceptionText), &actual, NULL);
			CloseHandle (debug_file);
		}
	}

	if (Any_Locked() && WinVersion.Is_Win9x() || GetCurrentThreadId() != (DWORD)MainThread) {
		DebugString("Can't bring up exception dialog due to Win16 mutex or threading issues\n");
		return(EXCEPTION_CONTINUE_SEARCH);
	}

	DebugString("Exception_Dialog()\n");
	if (Exception_Dialog() != IDC_EXCEPT_QUIT) {
		return(EXCEPTION_CONTINUE_SEARCH);
	}

	UnusedEHBool = false;

	if (ExceptionRecursions == 0) {
		ExceptionRecursions = -1;
	} else if (ExceptionRecursions == 2) {
		return(EXCEPTION_CONTINUE_SEARCH);
	}
	if (g_TopWindow) {
		OwnerDraw::End_Dialog(g_TopWindow);
	}

	return(EXCEPTION_EXECUTE_HANDLER);
}

// FUNCTION: 0x004C9150
int FUN_004C9150(void)
{
	return 0;
}

// FUNCTION: 0x004C9160
bool Expansion_Present(void)
{
	CCFileClass file("EXPAND.DAT");
	return(file.Is_Available());
}

// FUNCTION: 0x004C91B0
bool Expansion_Dialog(void)
{
	bool okval = true;
	return(okval);
}

// FUNCTION: 0x004C91C0
FacingClass::FacingClass(void) :
	DesiredFacing(DIR_N),
	StartFacing(DIR_N),
	ROT(DIR_N)
{
}

// FUNCTION: 0x004C91E0
FacingClass::FacingClass(int rate) :
	DesiredFacing(DIR_N),
	StartFacing(DIR_N),
	ROT(DIR_N)
{
	Set_ROT(rate);
}

// FUNCTION: 0x004C9220
bool FacingClass::Set_Desired(DirType const & facing)
{
	if (DesiredFacing != facing) {
		DirType cur;
		if (ROT.Facing > 0 && RotationTimer) {
			short diff = Turn_Arc();
			short rot = abs(diff) / ROT.Facing;
			DirType dir = DesiredFacing;

			if (rot > 0) {
				dir.Facing -= RotationTimer * (diff / rot);
			}
			cur = dir;
		} else {
			cur = DesiredFacing;
		}
		StartFacing = cur;
		DesiredFacing = facing;

		if (ROT.Facing > 0) {
			RotationTimer = abs(Turn_Arc()) / ROT.Facing;
		}
		return(true);
	}
	return(false);
}

// FUNCTION: 0x004C9300
bool FacingClass::Set(DirType const & facing)
{
	if (Current() != facing) {
		DesiredFacing = facing;
		StartFacing = facing;
		RotationTimer = 0;
		return(true);
	}

	RotationTimer = 0;
	return(false);
}

// FUNCTION: 0x004C93D0
DirType FacingClass::Current(void) const
{
	if (Is_Rotating()) {
		short diff = Turn_Arc();
		short rot = abs(diff) / ROT.Facing;
		DirType dir = DesiredFacing;

		if (rot > 0) {
			dir.Facing -= RotationTimer * (diff / rot);
		}
		return(dir);
	}
	return(DesiredFacing);
}

// FUNCTION: 0x004C9470
DirType FacingClass::Desired(void) const
{
	return(DesiredFacing);
}

// FUNCTION: 0x004C9480
bool FacingClass::Is_Rotating(void) const
{
	return(ROT.Facing > 0 && RotationTimer);
}

// FUNCTION: 0x004C94B0
bool FacingClass::Is_Rotating_CW(void) const
{
	if (Is_Rotating()) {
		return(Turn_Arc() > 0);
	}
	return(false);
}

// FUNCTION: 0x004C94F0
bool FacingClass::Is_Rotating_CCW(void) const
{
	if (Is_Rotating()) {
		short d = DesiredFacing.Facing - StartFacing.Facing;
		return(d < 0);
	}
	return(false);
}

// FUNCTION: 0x004C9530
DirType FacingClass::Difference(void)
{
	return(Desired() - Current());
}

// FUNCTION: 0x004C95D0
DirType FacingClass::Difference(DirType const & facing)
{
	return(facing - Current());
}

// FUNCTION: 0x004C9680
void FacingClass::Set_ROT(int rate)
{
	ROT.From_Dir256((Dir256)MIN(rate, DIR_S - 1));
}

// FUNCTION: 0x004C96A0 $E48
double DAT_008B3AD0 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x004C96D0 $E51
double DAT_008B3B20 = DAT_008B3AD0 / DAT_007E8890;

// FUNCTION: 0x004C96F0 $E54
double DAT_008B3B08 = DAT_007E1718 / DAT_008B3B20;
