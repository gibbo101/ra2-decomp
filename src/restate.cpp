// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#undef LoadString
#include "ccfile.h"
#include "ccini.h"
#include "scenario.h"
#include "stringtable.h"

extern double __cdecl FUN_004CAC40(double);
extern double __cdecl FUN_004CAD50(double);
extern double __cdecl FUN_004CADE0(double);
extern const double DAT_007E1708;
extern const double DAT_007E1710;
extern const double DAT_007E1718;
extern const double DAT_007E1720;
extern const double DAT_007E1728;
extern const double DAT_007E1730;
extern const double DAT_007E1738;
extern const double DAT_007E1740;
extern const double DAT_007F0970;
extern const double DAT_007F0978;
extern const double DAT_007F0980;
extern const double DAT_007F0988;


struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// FUNCTION: 0x0065F330 $E14
double DAT_00B04CC8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0065F360 $E17
double DAT_00B04D18 = DAT_00B04CC8 / DAT_007F0970;

// FUNCTION: 0x0065F380 $E20
double DAT_00B04D00 = DAT_007E1718 / DAT_00B04D18;

// FUNCTION: 0x0065F3A0 $E23
double DAT_00B04CF8 = DAT_007E1720 * DAT_007F0978;

// FUNCTION: 0x0065F3C0 $E26
double DAT_00B04CD0 = DAT_007E1728 * DAT_007F0978;

// FUNCTION: 0x0065F3E0 $E29
double DAT_00B04CA8 = DAT_007E1730 * DAT_007F0978;

// FUNCTION: 0x0065F400 $E32
int DAT_00B04D20 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B04CA8 - DAT_00B04CD0) * DAT_00B04CC8));

// FUNCTION: 0x0065F440 $E35
double DAT_00B04CD8 = FUN_004CADE0(DAT_00B04D20 * DAT_007E1740);

// FUNCTION: 0x0065F470 $E38
double DAT_00B04CF0 = FUN_004CADE0((DAT_00B04D20 * 2) / DAT_00B04CC8);

// FUNCTION: 0x0065F4A0 $E41
int DAT_00B04D14 = (int)(DAT_00B04D20 * 4 + DAT_007E1738);

// FUNCTION: 0x0065F4D0 $E44
ShortPair DAT_00B04CC0;

// FUNCTION: 0x0065F4E0 $E47
IntTriple DAT_00B04CE0;

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { A = 128; B = 128; C = 0; }
};

// FUNCTION: 0x0065F500 $E50
IntTriple2 DAT_00B04D08;

namespace Unsorted { extern bool ScenarioStarted; }
extern bool UserInputLocked;
extern char * DAT_00839710;
extern char * DAT_00839714;
extern char DAT_00889F64[];
extern wchar_t const * DAT_00B04CA0;

namespace UI {
	int __fastcall StandardWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	HWND __fastcall BeginDialog(int resource, DLGPROC proc, int param);
	void __fastcall FocusOnWindow(HWND window);
	void __fastcall EndDialog(HWND window);
	char __fastcall Updated(void);
}
class InputManagerClass {
	public:
		static InputManagerClass * Instance;
		void DoSomething(void);
};
struct GScreenClassStub {
	void Flag_To_Redraw(int level);
};
extern GScreenClassStub Map;
extern int CALLBACK FUN_0065F710(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: 0x0065F520
void __fastcall FUN_0065F520(ScenarioClass * scen)
{
	bool started = Unsorted::ScenarioStarted;
	Unsorted::ScenarioStarted = false;

	CCFileClass file;
	CCINIClass ini;
	file.Close();
	file.Set_Name(DAT_00839710);
	if (file.Is_Available(0)) {
		ini.ReadCCFile(file, false, false);
		char const * section = scen->ScenarioName;
		bool found;
		if (DAT_00839714 == NULL) {
			found = ini.Is_Present(section);
		} else {
			found = ini.Is_Present(section, DAT_00839714);
		}
		wchar_t const * text;
		if (found) {
			char buffer[256];
			ini.Get_String(section, DAT_00839714, DAT_00889F64, buffer, sizeof(buffer));
			text = StringTable::LoadString(buffer, NULL, "D:\\ra2mdpost\\Restate.cpp", 0x87);
		} else {
			text = StringTable::LoadString("Brief:Error", NULL, "D:\\ra2mdpost\\Restate.cpp", 0x8a);
		}
		DAT_00B04CA0 = text;

		int result;
		HWND dialog = UI::BeginDialog(0x10c, FUN_0065F710, 0);
		UserInputLocked = true;
		InputManagerClass::Instance->DoSomething();
		if (dialog != NULL) {
			SetWindowLongA(dialog, DWL_USER, (LONG)&result);
			UI::FocusOnWindow(dialog);
			result = 0;
			while (true) {
				if (UI::Updated()) {
					result = 0x423;
					break;
				}
				if (result != 0) {
					break;
				}
			}
			UI::EndDialog(dialog);
		}
		InputManagerClass::Instance->DoSomething();
		UserInputLocked = Scen->IsInputLocked;
		Map.Flag_To_Redraw(2);
	}
	Unsorted::ScenarioStarted = started;
}

// FUNCTION: 0x0065F710
int CALLBACK FUN_0065F710(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = UI::StandardWndProc(window, message, wparam, lparam);
	if (rc != 0) {
		return rc;
	}
	switch (message) {
		case WM_COMMAND:
			if (GetDlgCtrlID((HWND)lparam) == 0x423 && HIWORD(wparam) == 0) {
				*(int *)GetWindowLong(window, DWL_USER) = 0x423;
			}
			break;
		case 0x497:
			{
				HWND item = GetDlgItem(window, 0x6e7);
				SendMessage(item, 0x4b2, 0, (LPARAM)DAT_00B04CA0);
			}
			break;
	}
	return 0;
}

// FUNCTION: 0x0065F7A0 $E54
double DAT_00B04D70 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// FUNCTION: 0x0065F7D0 $E57
double DAT_00B04DF0 = DAT_00B04D70 / DAT_007F0980;

// FUNCTION: 0x0065F7F0 $E60
double DAT_00B04DD0 = DAT_007E1718 / DAT_00B04DF0;

// FUNCTION: 0x0065F810 $E63
double DAT_00B04DC8 = DAT_007E1720 * DAT_007F0988;

// FUNCTION: 0x0065F830 $E66
double DAT_00B04D80 = DAT_007E1728 * DAT_007F0988;

// FUNCTION: 0x0065F850 $E69
double DAT_00B04D30 = DAT_007E1730 * DAT_007F0988;

// FUNCTION: 0x0065F870 $E72
int DAT_00B04DF8 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B04D30 - DAT_00B04D80) * DAT_00B04D70));

// FUNCTION: 0x0065F8B0 $E75
double DAT_00B04D90 = FUN_004CADE0(DAT_00B04DF8 * DAT_007E1740);

// FUNCTION: 0x0065F8E0 $E78
double DAT_00B04DC0 = FUN_004CADE0((DAT_00B04DF8 * 2) / DAT_00B04D70);

// FUNCTION: 0x0065F910 $E81
int DAT_00B04DEC = (int)(DAT_00B04DF8 * 4 + DAT_007E1738);

// FUNCTION: 0x0065F940 $E84
ShortPair DAT_00B04D68;

// FUNCTION: 0x0065F950 $E87
IntTriple DAT_00B04D98;

// FUNCTION: 0x0065F970 $E90
IntTriple2 DAT_00B04DE0;

struct IntQuad {
	int A, B, C, D;
	IntQuad() { A = 0; B = 0; C = 0; D = 0; }
};

// FUNCTION: 0x0065F990 $E93
IntQuad DAT_00B04D38;
