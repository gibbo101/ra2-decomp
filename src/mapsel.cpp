// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "mapsel.h"

#include "addon.h"
#include "ccfile.h"
#include "convert.h"
#include "dbgprint.h"
#include "globals.h"
#include "houstype.h"
#include "msanim.h"
#include "mschoice.h"
#include "msengine.h"
#include "msfont.h"
#include "msgbox.h"
#include "gscreen.h"
#include "scenario.h"
#include "side.h"
#include "surface.h"
#include "stimer.h"
#include "theme.h"
#include "timer.h"
#include "stringtable.h"

#include <math.h>

// SYNTHETIC: GAMEMD 0x005ADB00 SYMBOL
// _$E10

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
extern const double DAT_007EDB38;
extern const double DAT_007EDB40;

struct ShortPair {
	unsigned short A, B;
	ShortPair() { A = 0; B = 0; }
};

struct IntTriple {
	int A, B, C;
	IntTriple() { A = 0; B = 0; C = 0; }
};

// SYNTHETIC: GAMEMD 0x005ADB20 SYMBOL
// _$E16
double DAT_00ABED78 = FUN_004CAC40(pow(DAT_007E1710, DAT_007E1708) * 2);

// SYNTHETIC: GAMEMD 0x005ADB50 SYMBOL
// _$E19
double DAT_00ABEDC8 = DAT_00ABED78 / DAT_007EDB38;

// SYNTHETIC: GAMEMD 0x005ADB70 SYMBOL
// _$E22
double DAT_00ABEDB0 = DAT_007E1718 / DAT_00ABEDC8;

// SYNTHETIC: GAMEMD 0x005ADB90 SYMBOL
// _$E25
double DAT_00ABEDA8 = DAT_007E1720 * DAT_007EDB40;

// SYNTHETIC: GAMEMD 0x005ADBB0 SYMBOL
// _$E28
double DAT_00ABED80 = DAT_007E1728 * DAT_007EDB40;

// SYNTHETIC: GAMEMD 0x005ADBD0 SYMBOL
// _$E31
double DAT_00ABED58 = DAT_007E1730 * DAT_007EDB40;

// SYNTHETIC: GAMEMD 0x005ADBF0 SYMBOL
// _$E34
int DAT_00ABEDD0 = (int)(DAT_007E1738 * (FUN_004CAD50(DAT_00ABED58 - DAT_00ABED80) * DAT_00ABED78));

// SYNTHETIC: GAMEMD 0x005ADC30 SYMBOL
// _$E37
double DAT_00ABED88 = FUN_004CADE0(DAT_00ABEDD0 * DAT_007E1740);

// SYNTHETIC: GAMEMD 0x005ADC60 SYMBOL
// _$E40
double DAT_00ABEDA0 = FUN_004CADE0((DAT_00ABEDD0 * 2) / DAT_00ABED78);

// SYNTHETIC: GAMEMD 0x005ADC90 SYMBOL
// _$E43
int DAT_00ABEDC4 = (int)(DAT_00ABEDD0 * 4 + DAT_007E1738);

// SYNTHETIC: GAMEMD 0x005ADCC0 SYMBOL
// _$E46
ShortPair DAT_00ABED70;

// SYNTHETIC: GAMEMD 0x005ADCD0 SYMBOL
// _$E49
IntTriple DAT_00ABED90;

class MapSelect : public MSEngine {
	public:
		bool Presentation(ScenarioClass * scenario);
		bool Advance_Progression(ScenarioClass * scen, Wstring const & map_name);
		bool SetNextScenario(ScenarioClass * scen);

	private:
		bool Init(ScenarioClass * scenario);
		void Deinit(void);

		void Play_Sound(Wstring const & name);
		void Queue_Voice(Wstring const & name, int delay);
		void Start_Voice(Wstring const & name);
		void Stop_Voice(bool fade);

		virtual void Do_Custom_Draw(Surface * surface);

		virtual void Process_Idle(void);
		const char * Process_Input(MapStage * stage);

	private:
		int XOffset;
		int YOffset;

		MapChoice Choices;

		ConvertClass * AnimDrawer;
		ConvertClass * OverlayDrawer;

		MSFont * Font;
		Rect TextRect;

		Wstring VoiceName;
		CDTimerClass<SystemTimerClass> VoiceTimer;
		int VoiceHandle;
};

void __cdecl FUN_004068E0(char const * format, ...);

// FUNCTION: GAMEMD 0x005ADCF0
void FUN_005ADCF0(void)
{
	extern int DAT_00ABEDC0;
	extern int DAT_00ABEDB8;
	extern int DAT_00ABEDBC;
	DAT_00ABEDC0 = 0;
	DAT_00ABEDB8 = 0x80;
	DAT_00ABEDBC = 0x80;
}

// FUNCTION: GAMEMD 0x005ADD10
const char * Map_Selection(ScenarioClass * scen)
{
	if (MapSelect().SetNextScenario(scen) == false) {
		WWMessageBox().Process(StringTable::LoadString("MSG:UnableMapSelect", NULL, "D:\\ra2mdpost\\MapSel.CPP", 0x86), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MapSel.CPP", 0x86), NULL, NULL);
		return(NULL);
	}

	FUN_004068E0("MapSelect: Scenario - %s, Stage - %ld\n", scen->ScenarioName, scen->Stage);

	return((strlen(scen->ScenarioName) == 0) ? NULL : scen->ScenarioName);
}

// FUNCTION: GAMEMD 0x005ADE20
const char * Map_Select_Advance(ScenarioClass * scenario, const char * map_name)
{
	if (MapSelect().Advance_Progression(scenario, map_name) == false) {
		WWMessageBox().Process(StringTable::LoadString("MSG:UnableMapSelect", NULL, "D:\\ra2mdpost\\MapSel.CPP", 0x98), StringTable::LoadString("TXT_OK", NULL, "D:\\ra2mdpost\\MapSel.CPP", 0x98), NULL, NULL);
		return(NULL);
	}

	return(map_name);
}

// FUNCTION: GAMEMD 0x005ADF20
bool MapSelect::Advance_Progression(ScenarioClass * scen, Wstring const & map_name)
{
	if ((scen == NULL) || (map_name == (char const *)NULL)) {
		return(false);
	}

	SideClass * side = Sides[HouseTypes[scen->PlayerHouse]->Side];
	if (side == NULL) {
		FUN_004068E0("MapSelect: Invalid side!\n");
		return(false);
	}

	FUN_004068E0("MapSelect: Side %s\n", (const char *)side->IniName);

	bool failed;
	{
		Wstring side_name((const char *)side->IniName);
		failed = (Choices.Initialize(side_name) == false);
	}
	if (failed) {
		Choices.Deinit();
		FUN_004068E0("MapSelect: Unable to initialize choices!\n");
		return(false);
	}

	MapStage * stage = Choices.Find_Stage_By_ID(scen->Stage);

	if (stage == NULL) {
		Choices.Deinit();
		FUN_004068E0("MapSelect: Invalid stage!\n");
		return(false);
	}

	scen->Set_Scenario_Name("");

	for (int index = 0; index < stage->Get_Selection_Count(); index++) {
		MapSelection * selection = stage->Get_Selection(index);

		if (selection != NULL) {
			MapStage * sel_stage = Choices.Find_Stage_By_Name(selection->Get_Stage_Label());
			Wstring name = sel_stage->Get_Scenario_Name();
			name.toLower();
			Wstring wanted = map_name;
			wanted.toLower();

			if (sel_stage != NULL && name == wanted) {
				scen->Set_Scenario_Name(sel_stage->Get_Scenario_Name().text());
				scen->Stage = Choices.Get_Stage_ID(sel_stage);
				break;
			}
		}
	}

	Choices.Deinit();
	return(strlen(scen->ScenarioName) > 0);
}

// FUNCTION: GAMEMD 0x005AE100
bool MapSelect::SetNextScenario(ScenarioClass * scen)
{
	if (Init(scen) == false) {
		return(false);
	}

	ScenarioClass::WasGameSaved = false;

	MapStage * stage = Choices.Find_Stage_By_Scenario(scen->ScenarioName);

	FUN_004068E0("MapSelect: Stage: %s\n", stage->Get_Stage_Label().c_str());

	if (stage->Get_Map_VQ_Name() != NULL) {
		Theme.Play_Song(Theme.From_Name("LOADING"));

		char const * label = stage->Get_Selection(0)->Get_Stage_Label().text();

		if (label != NULL) {
			FUN_004068E0("Selected %s\n", label);

			MapStage * next = Choices.Find_Stage_By_Name(label);
			Wstring name = next->Get_Scenario_Name();

			if (name.length() != 0) {
				scen->Set_Scenario_Name(name.get());
				scen->Stage = Choices.Get_Stage_ID(next);
			}
		}
	}

	Deinit();

	return(true);
}

// FUNCTION: GAMEMD 0x005AE220
bool MapSelect::Init(ScenarioClass * scenario)
{
	AnimDrawer = NULL;
	OverlayDrawer = NULL;
	Font = NULL;

	if (scenario == NULL) {
		FUN_004068E0("MapSelect: Invalid Scenario pointer!\n");
		return(false);
	}

	SideClass * side = Sides[HouseTypes[scenario->PlayerHouse]->Side];
	if (side == NULL) {
		FUN_004068E0("MapSelect: Invalid side!\n");
		return(false);
	}

	FUN_004068E0("MapSelect: Side %s\n", (const char *)side->IniName);

	bool failed;
	{
		Wstring side_name((const char *)side->IniName);
		failed = (Choices.Initialize(side_name) == false);
	}
	if (failed) {
		Choices.Deinit();
		FUN_004068E0("MapSelect: Unable to initialize choices!\n");
		return(false);
	}

	MapStage * map_stage = Choices.Find_Stage_By_Scenario(scenario->ScenarioName);

	if (map_stage == NULL) {
		FUN_004068E0("MapSelect: Invalid stage!\n");
		Deinit();
		return(false);
	}

	Font = new MSFont(true);

	if (Font == NULL) {
		FUN_004068E0("MapSelect: Unable to create font!\n");
		Deinit();
		return(false);
	}

	XOffset = ((HiddenSurface->Get_Width() - 640) / 2);
	YOffset = ((HiddenSurface->Get_Height() - 400) / 2);

	TextRect = *Choices.Get_Text_Rect();
	TextRect.X += XOffset;
	TextRect.Y += YOffset;

	int i = 0;

	do {
		WideWstring text = map_stage->Get_Description();
		MSPrintAnim::Word_Wrap(text, Font, TextRect.Width);
		i++;
		map_stage = Choices.Find_Stage_By_ID((unsigned short)i);
	} while (map_stage != NULL);

	VoiceName = (char const *)NULL;
	VoiceTimer = 0;
	VoiceHandle = -1;

	return(true);
}

// FUNCTION: GAMEMD 0x005AE460
void MapSelect::Deinit(void)
{
	if (Font != NULL) {
		delete Font;
		Font = NULL;
	}

	if (OverlayDrawer != NULL) {
		delete OverlayDrawer;
		OverlayDrawer = NULL;
	}

	if (AnimDrawer != NULL) {
		delete AnimDrawer;
		AnimDrawer = NULL;
	}

	Choices.Deinit();
}

// FUNCTION: GAMEMD 0x005AE4C0
void MapSelect::Process_Idle(void)
{
	if (VoiceName != (char const *)NULL && VoiceTimer == 0) {
		Start_Voice(VoiceName);
	}
}

// FUNCTION: GAMEMD 0x005AE4F0
const char * MapSelect::Process_Input(MapStage * stage)
{
	return(NULL);
}

// FUNCTION: GAMEMD 0x005AE500
void MapSelect::Play_Sound(Wstring const & name)
{
	if (name != (char const *)NULL) {
		MSSfxEntry * sfx = Choices.Find_Sound(name);

		if (sfx != NULL) {
			sfx->Play();
		}
	}
}

// FUNCTION: GAMEMD 0x005AE530
void MapSelect::Queue_Voice(Wstring const & name, int delay)
{
	VoiceName = name;
	VoiceTimer = delay;
}

// FUNCTION: GAMEMD 0x005AE570
void MapSelect::Start_Voice(Wstring const & name)
{
}

// FUNCTION: GAMEMD 0x005AE580
void MapSelect::Stop_Voice(bool fade)
{
}

// FUNCTION: GAMEMD 0x005AE590
void MapSelect::Do_Custom_Draw(Surface * surface)
{
}

// SYNTHETIC: GAMEMD 0x005AE5A0
// ??_GMapSelect@@UAEPAXI@Z
