// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "rect.h"
#include <math.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#undef LoadString
#include "ccfile.h"
#include "ccini.h"
#include "scenario.h"
#include "stringtable.h"

// SYNTHETIC: GAMEMD 0x0065F310 SYMBOL
// _$E2

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


struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0065F330 SYMBOL
// _$E18
double DAT_00B04CC8 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x0065F360 SYMBOL
// _$E21
double DAT_00B04D18 = DAT_00B04CC8 / DAT_007F0970;

// SYNTHETIC: GAMEMD 0x0065F380 SYMBOL
// _$E24
double DAT_00B04D00 = DAT_007E1718 / DAT_00B04D18;

// SYNTHETIC: GAMEMD 0x0065F3A0 SYMBOL
// _$E27
double DAT_00B04CF8 = DAT_007E1720 * DAT_007F0978;

// SYNTHETIC: GAMEMD 0x0065F3C0 SYMBOL
// _$E30
double DAT_00B04CD0 = DAT_007E1728 * DAT_007F0978;

// SYNTHETIC: GAMEMD 0x0065F3E0 SYMBOL
// _$E33
double DAT_00B04CA8 = DAT_007E1730 * DAT_007F0978;

// SYNTHETIC: GAMEMD 0x0065F400 SYMBOL
// _$E36
int DAT_00B04D20 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00B04CA8 - DAT_00B04CD0) * DAT_00B04CC8));

// SYNTHETIC: GAMEMD 0x0065F440 SYMBOL
// _$E39
double DAT_00B04CD8 = FUN_004CADE0(DAT_00B04D20 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x0065F470 SYMBOL
// _$E42
double DAT_00B04CF0 = FUN_004CADE0((DAT_00B04D20 * 2) / DAT_00B04CC8);

// SYNTHETIC: GAMEMD 0x0065F4A0 SYMBOL
// _$E45
int DAT_00B04D14 = (int)(DAT_00B04D20 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x0065F4D0 SYMBOL
// _$E48
ShortPair DAT_00B04CC0;

// SYNTHETIC: GAMEMD 0x0065F4E0 SYMBOL
// _$E51
IntTriple DAT_00B04CE0;

struct IntTriple2 {
	int A, B, C;
	IntTriple2() { A = 128; B = 128; C = 0; }
};

// SYNTHETIC: GAMEMD 0x0065F500 SYMBOL
// _$E54
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

// FUNCTION: GAMEMD 0x0065F520
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

// FUNCTION: GAMEMD 0x0065F710
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
