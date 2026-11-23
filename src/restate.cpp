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
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0065F310 SYMBOL
// _$E2

// SYNTHETIC: GAMEMD 0x0065F330 SYMBOL
// _$E18
// SYNTHETIC: GAMEMD 0x0065F360 SYMBOL
// _$E21
// SYNTHETIC: GAMEMD 0x0065F380 SYMBOL
// _$E24
// SYNTHETIC: GAMEMD 0x0065F3A0 SYMBOL
// _$E27
// SYNTHETIC: GAMEMD 0x0065F3C0 SYMBOL
// _$E30
// SYNTHETIC: GAMEMD 0x0065F3E0 SYMBOL
// _$E33
// SYNTHETIC: GAMEMD 0x0065F400 SYMBOL
// _$E36
// SYNTHETIC: GAMEMD 0x0065F440 SYMBOL
// _$E39
// SYNTHETIC: GAMEMD 0x0065F470 SYMBOL
// _$E42
// SYNTHETIC: GAMEMD 0x0065F4A0 SYMBOL
// _$E45
// SYNTHETIC: GAMEMD 0x0065F4D0 SYMBOL
// _$E48
// SYNTHETIC: GAMEMD 0x0065F4E0 SYMBOL
// _$E51
// SYNTHETIC: GAMEMD 0x0065F500 SYMBOL
// _$E54

namespace Unsorted { extern bool ScenarioStarted; }
extern bool UserInputLocked;
extern char * MissionININame;
extern char * BriefingKey;
extern char DAT_00889F64[];
extern wchar_t const * RestateText;

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
extern int CALLBACK Restate_Mission_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

// FUNCTION: GAMEMD 0x0065F520
void __fastcall FUN_0065F520(ScenarioClass * scen)
{
	bool started = Unsorted::ScenarioStarted;
	Unsorted::ScenarioStarted = false;

	CCFileClass file;
	CCINIClass ini;
	file.Close();
	file.Set_Name(MissionININame);
	if (file.Is_Available(0)) {
		ini.ReadCCFile(file, false, false);
		char const * section = scen->ScenarioName;
		bool found;
		if (BriefingKey == NULL) {
			found = ini.Is_Present(section);
		} else {
			found = ini.Is_Present(section, BriefingKey);
		}
		wchar_t const * text;
		if (found) {
			char buffer[256];
			ini.Get_String(section, BriefingKey, DAT_00889F64, buffer, sizeof(buffer));
			text = StringTable::LoadString(buffer, NULL, "D:\\ra2mdpost\\Restate.cpp", 0x87);
		} else {
			text = StringTable::LoadString("Brief:Error", NULL, "D:\\ra2mdpost\\Restate.cpp", 0x8a);
		}
		RestateText = text;

		int result;
		HWND dialog = UI::BeginDialog(0x10c, Restate_Mission_Dialog_Proc, 0);
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
int CALLBACK Restate_Mission_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
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
				SendMessage(item, 0x4b2, 0, (LPARAM)RestateText);
			}
			break;
	}
	return 0;
}
