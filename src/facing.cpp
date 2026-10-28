// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "facing.h"
#include <math.h>

// SYNTHETIC: GAMEMD 0x004C81D0 SYMBOL
// _$E45

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

// SYNTHETIC: GAMEMD 0x004C81F0 SYMBOL
// _$E2
double DAT_008B3A20 = fastmath::sqrt(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x004C8220 SYMBOL
// _$E5
double DAT_008B3A70 = DAT_008B3A20 / DAT_007E8828;

// SYNTHETIC: GAMEMD 0x004C8240 SYMBOL
// _$E8
double DAT_008B3A58 = DAT_007E1718 / DAT_008B3A70;

// SYNTHETIC: GAMEMD 0x004C8260 SYMBOL
// _$E11
double DAT_008B3A50 = DAT_007E1720 * DAT_007E8830;

// SYNTHETIC: GAMEMD 0x004C8280 SYMBOL
// _$E14
double DAT_008B3A28 = DAT_007E1728 * DAT_007E8830;

// SYNTHETIC: GAMEMD 0x004C82A0 SYMBOL
// _$E17
double DAT_008A3A00 = DAT_007E1730 * DAT_007E8830;

// SYNTHETIC: GAMEMD 0x004C82C0 SYMBOL
// _$E20
int DAT_008B3A78 = (int)(fastmath::tan(DAT_008A3A00 - DAT_008B3A28) * DAT_007E1738 * DAT_008B3A20);

// SYNTHETIC: GAMEMD 0x004C8300 SYMBOL
// _$E23
double DAT_008B3A30 = fastmath::atan(DAT_008B3A78 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x004C8330 SYMBOL
// _$E26
double DAT_008B3A48 = fastmath::atan((DAT_008B3A78 * 2) / DAT_008B3A20);

// SYNTHETIC: GAMEMD 0x004C8360 SYMBOL
// _$E29
int DAT_008B3A6C = (int)(DAT_008B3A78 * 4 + DAT_007E1738);

struct FacingShortPair { short a; short b; FacingShortPair(void) { a = 0; b = 0; } };
struct FacingIntTriple { int a; int b; int c; FacingIntTriple(void) { a = 0; b = 0; c = 0; } };

// SYNTHETIC: GAMEMD 0x004C8390 SYMBOL
// _$E32
FacingShortPair DAT_008B3A18;

// SYNTHETIC: GAMEMD 0x004C83A0 SYMBOL
// _$E35
FacingIntTriple DAT_008B3A38;


struct FacingBias { int x; int y; int z; FacingBias(void) { z = 0; x = 0x80; y = 0x80; } };

// SYNTHETIC: GAMEMD 0x004C83C0 SYMBOL
// _$E38
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

void Dump_Exception_Info(EXCEPTION_POINTERS * e_info);

// FUNCTION: GAMEMD 0x004C83E0
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

// FUNCTION: GAMEMD 0x004C84E0
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


#include "getcpu.h"
#include "winstub.h"
#include "globals.h"
#include <imagehlp.h>

const char * FUN_004E62B0(void);

typedef BOOL  (WINAPI *SymCleanupType) (HANDLE hProcess);
typedef BOOL  (WINAPI *SymGetSymFromAddrType) (HANDLE hProcess, DWORD Address, LPDWORD Displacement, PIMAGEHLP_SYMBOL Symbol);
typedef BOOL  (WINAPI *SymInitializeType) (HANDLE hProcess, LPSTR UserSearchPath, BOOL fInvadeProcess);
typedef BOOL  (WINAPI *SymLoadModuleType) (HANDLE hProcess, HANDLE hFile, LPSTR ImageName, LPSTR ModuleName, DWORD BaseOfDll, DWORD SizeOfDll);
typedef DWORD (WINAPI *SymSetOptionsType) (DWORD SymOptions);
typedef BOOL  (WINAPI *SymUnloadModuleType) (HANDLE hProcess, DWORD BaseOfDll);
typedef BOOL  (WINAPI *StackWalkType) (DWORD MachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME StackFrame, LPVOID ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE TranslateAddress);
typedef LPVOID (WINAPI *SymFunctionTableAccessType) (HANDLE hProcess, DWORD AddrBase);
typedef DWORD (WINAPI *SymGetModuleBaseType) (HANDLE hProcess, DWORD dwAddr);

static SymCleanupType _SymCleanup = NULL;
static SymGetSymFromAddrType _SymGetSymFromAddr = NULL;
static SymInitializeType _SymInitialize = NULL;
static SymLoadModuleType _SymLoadModule = NULL;
static SymSetOptionsType _SymSetOptions = NULL;
static SymUnloadModuleType _SymUnloadModule = NULL;
static StackWalkType _StackWalk = NULL;
static SymFunctionTableAccessType _SymFunctionTableAccess = NULL;
static SymGetModuleBaseType _SymGetModuleBase = NULL;

static char const *ImagehelpFunctionNames[] =
{
	"SymCleanup",
	"SymGetSymFromAddr",
	"SymInitialize",
	"SymLoadModule",
	"SymSetOptions",
	"SymUnloadModule",
	"StackWalk",
	"SymFunctionTableAccess",
	"SymGetModuleBaseType",
	NULL
};

static void Add_Txt(char const *txt)
{
	if (lstrlen(ExceptionText) + lstrlen(txt) < EH_BUFFER_SIZE - 1) {
		lstrcat(ExceptionText, txt);
	}
}

// FUNCTION: GAMEMD 0x004C85E0
void Dump_Exception_Info(EXCEPTION_POINTERS *e_info)
{
	static const unsigned int _codes[] = {
		EXCEPTION_ACCESS_VIOLATION,
		EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
		EXCEPTION_BREAKPOINT,
		EXCEPTION_DATATYPE_MISALIGNMENT,
		EXCEPTION_FLT_DENORMAL_OPERAND,
		EXCEPTION_FLT_DIVIDE_BY_ZERO,
		EXCEPTION_FLT_INEXACT_RESULT,
		EXCEPTION_FLT_INVALID_OPERATION,
		EXCEPTION_FLT_OVERFLOW,
		EXCEPTION_FLT_STACK_CHECK,
		EXCEPTION_FLT_UNDERFLOW,
		EXCEPTION_ILLEGAL_INSTRUCTION,
		EXCEPTION_IN_PAGE_ERROR,
		EXCEPTION_INT_DIVIDE_BY_ZERO,
		EXCEPTION_INT_OVERFLOW,
		EXCEPTION_INVALID_DISPOSITION,
		EXCEPTION_NONCONTINUABLE_EXCEPTION,
		EXCEPTION_PRIV_INSTRUCTION,
		EXCEPTION_SINGLE_STEP,
		EXCEPTION_STACK_OVERFLOW,
		0xffffffff
	};

	static char const * _code_txt[] = {
		"Error code: EXCEPTION_ACCESS_VIOLATION\r\r\nDescription: The thread tried to read from or write to a virtual address for which it does not have the appropriate access.",
		"Error code: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\r\r\nDescription: The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.",
		"Error code: EXCEPTION_BREAKPOINT\r\r\nDescription: A breakpoint was encountered.",
		"Error code: EXCEPTION_DATATYPE_MISALIGNMENT\r\r\nDescription: The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.",
		"Error code: EXCEPTION_FLT_DENORMAL_OPERAND\r\r\nDescription: One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.",
		"Error code: EXCEPTION_FLT_DIVIDE_BY_ZERO\r\r\nDescription: The thread tried to divide a floating-point value by a floating-point divisor of zero.",
		"Error code: EXCEPTION_FLT_INEXACT_RESULT\r\r\nDescription: The result of a floating-point operation cannot be represented exactly as a decimal fraction.",
		"Error code: EXCEPTION_FLT_INVALID_OPERATION\r\r\nDescription: Some strange unknown floating point operation was attempted.",
		"Error code: EXCEPTION_FLT_OVERFLOW\r\r\nDescription: The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.",
		"Error code: EXCEPTION_FLT_STACK_CHECK\r\r\nDescription: The stack overflowed or underflowed as the result of a floating-point operation.",
		"Error code: EXCEPTION_FLT_UNDERFLOW\r\r\nDescription:\tThe exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.",
		"Error code: EXCEPTION_ILLEGAL_INSTRUCTION\r\r\nDescription:\tThe thread tried to execute an invalid instruction.",
		"Error code: EXCEPTION_IN_PAGE_ERROR\r\r\nDescription:\tThe thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.",
		"Error code: EXCEPTION_INT_DIVIDE_BY_ZERO\r\r\nDescription: The thread tried to divide an integer value by an integer divisor of zero.",
		"Error code: EXCEPTION_INT_OVERFLOW\r\r\nDescription: The result of an integer operation caused a carry out of the most significant bit of the result.",
		"Error code: EXCEPTION_INVALID_DISPOSITION\r\r\nDescription: An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.",
		"Error code: EXCEPTION_NONCONTINUABLE_EXCEPTION\r\r\nDescription: The thread tried to continue execution after a noncontinuable exception occurred.",
		"Error code: EXCEPTION_PRIV_INSTRUCTION\r\r\nDescription: The thread tried to execute an instruction whose operation is not allowed in the current machine mode.",
		"Error code: EXCEPTION_SINGLE_STEP\r\r\nDescription: A trace trap or other single-instruction mechanism signaled that one instruction has been executed.",
		"Error code: EXCEPTION_STACK_OVERFLOW\r\r\nDescription: The thread used up its stack.",
		"Error code: ?????\r\r\nDescription: Unknown exception."
	};

	DebugString("Dump exception info\n");

	char scrap[256];

	memset(ExceptionText, 0, sizeof(ExceptionText));

	HINSTANCE imagehelp = LoadLibrary("IMAGEHLP.DLL");

	if (imagehelp != NULL) {
		DebugString("Found IMAGEHLP.DLL - linking to required functions\n");
		char const *function_name = NULL;
		unsigned long *fptr = (unsigned long *)&_SymCleanup;
		int count = 0;

		do {
			function_name = ImagehelpFunctionNames[count];
			if (function_name) {
				*fptr = (unsigned long)GetProcAddress(imagehelp, function_name);
				fptr++;
				count++;
			}
		} while (function_name);
	} else {
		DebugString("Unable to load IMAGEHLP.DLL\n");
	}

	if (_SymSetOptions != NULL) {
		_SymSetOptions(SYMOPT_DEFERRED_LOADS);
	}

	int symload = 0;
	int symbols_available = false;

	if (_SymInitialize != NULL && _SymInitialize(GetCurrentProcess(), NULL, false)) {
		DebugString("Symbols are available\r\n\n");
		symbols_available = true;
	}

	if (!symbols_available) {
		DebugString("SymInitialize failed with code %d - %s\n", GetLastError(), Last_Error_Text());
	} else {
		if (_SymSetOptions != NULL) {
			_SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
		}

		char module_name[_MAX_PATH];
		GetModuleFileName(NULL, module_name, sizeof(module_name));

		if (_SymLoadModule != NULL) {
			symload = _SymLoadModule(GetCurrentProcess(), NULL, module_name, NULL, 0, 0);
		}

		if (!symload) {
			DebugString("SymLoad failed for module %s with code %d - %s\n", module_name, GetLastError(), Last_Error_Text());
		}
	}

	unsigned char symbol[256];
	unsigned long displacement;
	IMAGEHLP_SYMBOL *symptr = (IMAGEHLP_SYMBOL *)&symbol;

	CONTEXT *context = e_info->ContextRecord;

	int access_read_write = -1;
	unsigned long access_address = 0;

	if (e_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		DebugString("Exception is access violation\n");
		access_read_write = e_info->ExceptionRecord->ExceptionInformation[0];
		access_address = e_info->ExceptionRecord->ExceptionInformation[1];
	} else {
		DebugString("Exception code is %d\n", e_info->ExceptionRecord->ExceptionCode);
	}

	for (int i = 0; _codes[i] != 0xffffffff; i++) {
		if (_codes[i] == e_info->ExceptionRecord->ExceptionCode) {
			DebugString("Found exception description\n");
			break;
		}
	}
	Add_Txt(_code_txt[i]);
	Add_Txt("\r\n");

	if (e_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		wsprintf(scrap, "Access address:%08X ", access_address);
		Add_Txt(scrap);
		if (access_read_write) {
			Add_Txt("was written to.\r\n");
		} else {
			Add_Txt("was read from.\r\n");
		}
	}

	memset(symptr, 0, sizeof(IMAGEHLP_SYMBOL));
	symptr->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	symptr->MaxNameLength = 256 - sizeof(IMAGEHLP_SYMBOL);
	symptr->Size = 0;
	symptr->Address = context->Eip;

	if (!IsBadCodePtr((FARPROC)context->Eip)) {
		if (_SymGetSymFromAddr != NULL && _SymGetSymFromAddr(GetCurrentProcess(), context->Eip, &displacement, symptr)) {
			wsprintf(scrap, "Exception occurred at %08X - %s + %08X\r\n", context->Eip, symptr->Name, displacement);
		} else {
			DebugString("Failed to get symbol for EIP\r\n");
			if (_SymGetSymFromAddr != NULL) {
				DebugString("SymGetSymFromAddr failed with code %d - %s\n", GetLastError(), Last_Error_Text());
			}
			wsprintf(scrap, "Exception occurred at %08X\r\n", context->Eip);
		}
	} else {
		DebugString("context->Eip is bad code pointer\n");
	}

	Add_Txt(scrap);

	wsprintf(scrap, "\r\nVersion %s\r\n", Version_Name());
	Add_Txt(scrap);

	wsprintf(scrap, "Internal Version %s\r\n", VerNum.Version_Name());
	Add_Txt(scrap);

	char buildinfo[128];
	buildinfo[0] = 0;
	Build_Date_String(buildinfo, 128);

	char build_number[128];
	char build_name[128];

	wsprintf(scrap, "Release Build: %s by %s - %s\r\n", Build_Number_String(build_number, 128), Build_By_String(build_name, 128), buildinfo);
	Add_Txt(scrap);

	int cpu_type = 5;
	bool mmx = false;
	Get_CPU_Type(cpu_type, mmx, NULL, 0);
	wsprintf(scrap, "CPU %s, MMX %s, Vendor: %s\r\n", FUN_004E62B0(), mmx ? "Yes" : "No", "Not available");
	Add_Txt(scrap);

	Add_Txt("\r\nDetails:\r\n");

	DebugString("Register dump...\n");

	wsprintf(scrap, "Eip:%08X\tEsp:%08X\tEbp:%08X\r\n", context->Eip, context->Esp, context->Ebp);
	Add_Txt(scrap);
	wsprintf(scrap, "Eax:%08X\tEbx:%08X\tEcx:%08X\r\n", context->Eax, context->Ebx, context->Ecx);
	Add_Txt(scrap);
	wsprintf(scrap, "Edx:%08X\tEsi:%08X\tEdi:%08X\r\n", context->Edx, context->Esi, context->Edi);
	Add_Txt(scrap);
	wsprintf(scrap, "EFlags:%08X \r\n", context->EFlags);
	Add_Txt(scrap);
	wsprintf(scrap, "CS:%04x  SS:%04x  DS:%04x  ES:%04x  FS:%04x  GS:%04x\r\n", context->SegCs, context->SegSs, context->SegDs, context->SegEs, context->SegFs, context->SegGs);
	Add_Txt(scrap);

	DebugString("EIP bytes dump...\n");
	wsprintf(scrap, "\r\nBytes at CS:EIP (%08X)  : ", context->Eip);

	unsigned char *eip_ptr = (unsigned char *)(context->Eip);
	char bytestr[32];

	for (int c = 0; c < 32; c++) {
		if (IsBadReadPtr(eip_ptr, 1)) {
			lstrcat(scrap, "?? ");
		} else {
			wsprintf(bytestr, "%02X ", *eip_ptr);
			lstrcat(scrap, bytestr);
		}
		eip_ptr++;
	}

	lstrcat(scrap, "\r\n\r\n");
	Add_Txt(scrap);

	DebugString("Stack dump...\n");
	Add_Txt("Stack dump (* indicates possible code address) :\r\n");
	unsigned long *stackptr = (unsigned long *)context->Esp;

	for (int j = 0; j < 1024; j++) {
		if (IsBadReadPtr(stackptr, 4)) {
			wsprintf(scrap, "%08X: ", stackptr);
			lstrcat(scrap, "????????\r\n");
		} else {
			if (IsBadCodePtr((FARPROC)*stackptr)) {
				wsprintf(scrap, "%08X: %08X ", stackptr, *stackptr);
				lstrcat(scrap, "DATA_PTR\r\n");
			} else {
				wsprintf(scrap, "%08X: %08X", stackptr, *stackptr);

				if (symbols_available) {
					symptr->SizeOfStruct = sizeof(symbol);
					symptr->MaxNameLength = 128;
					symptr->Size = 0;
					symptr->Address = *stackptr;

					if (_SymGetSymFromAddr != NULL && _SymGetSymFromAddr(GetCurrentProcess(), *stackptr, &displacement, symptr)) {
						char symbuf[256];
						wsprintf(symbuf, " - %s + %08X", symptr->Name, displacement);
						lstrcat(scrap, symbuf);
					}
				} else {
					lstrcat(scrap, " *");
				}
				lstrcat(scrap, "\r\n");
			}
		}
		Add_Txt(scrap);
		stackptr++;
	}

	if (symbols_available) {
		if (_SymCleanup != NULL) {
			_SymCleanup(GetCurrentProcess());
		}

		if (symload) {
			if (_SymUnloadModule != NULL) {
				_SymUnloadModule(GetCurrentProcess(), NULL);
			}
		}
	}

	if (imagehelp) {
		FreeLibrary(imagehelp);
	}

	Add_Txt("\r\n\r\n");
}


// FUNCTION: GAMEMD 0x004C8FE0
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

// FUNCTION: GAMEMD 0x004C9150
int FUN_004C9150(void)
{
	return 0;
}

// FUNCTION: GAMEMD 0x004C9160
bool Expansion_Present(void)
{
	CCFileClass file("EXPAND.DAT");
	return(file.Is_Available());
}

// FUNCTION: GAMEMD 0x004C91B0
bool Expansion_Dialog(void)
{
	bool okval = true;
	return(okval);
}

// FUNCTION: GAMEMD 0x004C91C0
FacingClass::FacingClass(void) :
	DesiredFacing(DIR_N),
	StartFacing(DIR_N),
	ROT(DIR_N)
{
}

// FUNCTION: GAMEMD 0x004C91E0
FacingClass::FacingClass(int rate) :
	DesiredFacing(DIR_N),
	StartFacing(DIR_N),
	ROT(DIR_N)
{
	Set_ROT(rate);
}

// FUNCTION: GAMEMD 0x004C9220
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

// FUNCTION: GAMEMD 0x004C9300
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

// FUNCTION: GAMEMD 0x004C93D0
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

// FUNCTION: GAMEMD 0x004C9470
DirType FacingClass::Desired(void) const
{
	return(DesiredFacing);
}

// FUNCTION: GAMEMD 0x004C9480
bool FacingClass::Is_Rotating(void) const
{
	return(ROT.Facing > 0 && RotationTimer);
}

// FUNCTION: GAMEMD 0x004C94B0
bool FacingClass::Is_Rotating_CW(void) const
{
	if (Is_Rotating()) {
		return(Turn_Arc() > 0);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004C94F0
bool FacingClass::Is_Rotating_CCW(void) const
{
	if (Is_Rotating()) {
		short d = DesiredFacing.Facing - StartFacing.Facing;
		return(d < 0);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004C9530
DirType FacingClass::Difference(void)
{
	return(Desired() - Current());
}

// FUNCTION: GAMEMD 0x004C95D0
DirType FacingClass::Difference(DirType const & facing)
{
	return(facing - Current());
}

// FUNCTION: GAMEMD 0x004C9680
void FacingClass::Set_ROT(int rate)
{
	ROT.From_Dir256((Dir256)MIN(rate, DIR_S - 1));
}
