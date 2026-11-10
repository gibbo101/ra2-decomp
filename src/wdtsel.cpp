// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "language\language.h"

#include "_keyboar.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "data.h"
#include "globals.h"
#include "ini.h"
#include "keyboard.h"
#include "mschoice.h"
#include "msfont.h"
#include "newmenu.h"
#include "netshare.h"
#include "ownrdraw.h"
#include "pcx.h"
#include "session.h"
#include "geometry.h"
#include "theme.h"
#include "vector.h"
#include "wdtnet.h"
#include "worlddom.h"

#define IID_DEFINED
#include "wolapi\wolapi.h"

// SYNTHETIC: GAMEMD 0x0076C080 SYMBOL
// _$E8

namespace fastmath { double __cdecl sqrt(double x); double __cdecl tan(double x); double __cdecl atan(double x); }

using namespace WorldDominationTour;
// SYNTHETIC: GAMEMD 0x0076C0A0 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x0076C0D0 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x0076C0F0 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x0076C110 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x0076C130 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x0076C150 SYMBOL
// _$E46
// SYNTHETIC: GAMEMD 0x0076C170 SYMBOL
// _$E49
// SYNTHETIC: GAMEMD 0x0076C1B0 SYMBOL
// _$E52
// SYNTHETIC: GAMEMD 0x0076C1E0 SYMBOL
// _$E55
// SYNTHETIC: GAMEMD 0x0076C210 SYMBOL
// _$E58
// SYNTHETIC: GAMEMD 0x0076C240 SYMBOL
// _$E61
// SYNTHETIC: GAMEMD 0x0076C250 SYMBOL
// _$E64
// SYNTHETIC: GAMEMD 0x0076C270 SYMBOL
// _$E67

// FUNCTION: GAMEMD 0x0076C970
// WorldDominationTour::Selection::Init_Voices
void Selection::Init_Voices(INIClass const & ini, Wstring const & section)
{
	VoicePlayer.FUN_00770550(ini, section);
	{ Wstring key("WinningThreshold"); WinningThreshold = ini.Get_Int(section, key, WinningThreshold); }
	{ Wstring key("MegaWinningThreshold"); MegaWinningThreshold = ini.Get_Int(section, key, MegaWinningThreshold); }
	{ Wstring key("LosingThreshold"); LosingThreshold = ini.Get_Int(section, key, LosingThreshold); }
	{ Wstring key("MegaLosingThreshold"); MegaLosingThreshold = ini.Get_Int(section, key, MegaLosingThreshold); }
}

// FUNCTION: GAMEMD 0x0076CA60
// WorldDominationTour::Selection::Init_Sounds
void Selection::Init_Sounds(INIClass const & ini, Wstring const & section)
{
	Wstring buffer;

	int count = ini.Entry_Count(section);
	for (int i = 0; i < count; i++) {
		Wstring entry = ini.Get_Entry(section, i);
		if (entry != (char const *)NULL) {
			bool found;
			{
				Wstring none((char const *)NULL);
				found = ini.Get_String(section, entry, none, buffer) > 0;
			}
			if (found) {
				if (entry == "Theme") {
					ThemeName = buffer;
					break;
				}
				MSSfxEntry * sfx = new MSSfxEntry(entry, buffer);
				if (sfx->Get_Name() == (char const *)NULL) {
					delete sfx;
				} else {
					Add_Sfx_Entry(sfx);
				}
			}
		}
	}
}

// FUNCTION: GAMEMD 0x0076CBF0
// WorldDominationTour::Selection::Init_Target
void Selection::Init_Target(INIClass const & ini, Wstring const & section)
{
	Wstring buffer;
	int faction = TourCampaign.Get()->PlayerFaction;
	Wstring faction_name = *FUN_007AF2A0(faction);

	bool found;
	{
		Wstring none((char const *)NULL);
		found = ini.Get_String(section, faction_name + "ZoomingTarget", none, buffer) > 0;
	}
	if (found) {
		ZoomingTarget = new MSAnimEntry(buffer);
	}

	{
		Wstring none((char const *)NULL);
		ini.Get_String(section, faction_name + "TargetPalette", none, buffer);
	}
	TargetDrawer = Create_Drawer(buffer);

	{
		Wstring none((char const *)NULL);
		ini.Get_String(section, faction_name + "Target", none, buffer);
	}
	ThrobbingTarget = new MSAnimEntry(buffer);

	ThrobbingTargetDividingFrame = ini.Get_Int(section, faction_name + "TargetDividingFrame", 32);
}

// FUNCTION: GAMEMD 0x0076D470
// WorldDominationTour::Selection::Update_Hovered_Territory
void Selection::Update_Hovered_Territory(Point2D const & point)
{
	int id = WorldMap.Get()->FUN_00769330(point);
	if (id != HoveredTerritoryID) {
		HoveredTerritoryID = id;
		Select_Territory(WorldMap.Get()->FUN_0076F000(id));
	}
}

// FUNCTION: GAMEMD 0x0076DED0
// WorldDominationTour::Selection::Remove_Target_Anims
void Selection::Remove_Target_Anims(void)
{
	for (TERR_ITER it(WorldMap.Get()->Territories, 0); it; it++) {
		MSShapeAnim * anim = it->FUN_0076FF30();
		if (anim != NULL) {
			Remove_Anim(anim);
		}
	}
}

// FUNCTION: GAMEMD 0x0076DF50
// WorldDominationTour::Selection::Remove_Target_Anims2
void Selection::Remove_Target_Anims2(void)
{
	for (TERR_ITER it(WorldMap.Get()->Territories, 0); it; it++) {
		MSShapeAnim * anim = it->FUN_0076FF30();
		if (anim != NULL) {
			Remove_Anim(anim);
		}
	}
}

extern bool UserInputLocked;

class InputManagerClass {
public:
	static InputManagerClass * Instance;
	short FUN_0054F000(void);
	short FUN_0054F050(void);
	void FUN_0054F720(void);
};

typedef VectorCursor<MSSfxEntry *, DynamicVectorClass<MSSfxEntry *> > WDT_SFX_ITER;

static inline void Play_Sfx_Named(Selection * sel, char const * text)
{
	Wstring name(text);
	if (name != (char const *)NULL) {
		WDT_SFX_ITER it(sel->SfxEntries, 0);
		for (; it; it++) {
			if (!(name == it->Get_Name())) {
				break;
			}
		}
		if (it) {
			it->Play();
		}
	}
}

// FUNCTION: GAMEMD 0x0076DFD0
// WorldDominationTour::Selection::On_Map_Enter
void Selection::On_Map_Enter(void)
{
	Play_Sfx_Named(this, "MapEnter");
}

// FUNCTION: GAMEMD 0x0076E090
// WorldDominationTour::Selection::On_Map_Leave
void Selection::On_Map_Leave(void)
{
	Play_Sfx_Named(this, "MapLeave");
	HoveredTerritoryID = -1;
	rc_ptr<Territory> none;
	Select_Territory(none);
}

#undef LoadString
#include "stringtable.h"
WideWstring __fastcall FUN_007B7E40(int value, int digits);

// FUNCTION: GAMEMD 0x0076E180
// WorldDominationTour::Selection::On_Cancel_Enter
void Selection::On_Cancel_Enter(void)
{
	if (!WorldMap->CancelActive) {
		WorldMap->FUN_007693F0(this, true);
		Play_SFX("MouseCancel");
		Set_Tooltip(StringTable::LoadString("WOL:WDTTerritoryBack", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x37e));
	}
}

// FUNCTION: GAMEMD 0x0076E290
// WorldDominationTour::Selection::On_Cancel_Leave
void Selection::On_Cancel_Leave(void)
{
	if (WorldMap->CancelActive) {
		WorldMap->FUN_007693F0(this, false);
		Set_Tooltip(StringTable::LoadString("WDT:TerritorySelectToolTip", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x394));
	}
}

// FUNCTION: GAMEMD 0x0076E2F0
// WorldDominationTour::Selection::Do_Target_Selection
void Selection::Do_Target_Selection(int index)
{
	{
		WideWstring blank((wchar_t const *)NULL);
		Set_Text(blank, 0);
	}

	int current_tick = TourCampaign.Get()->Properties->FUN_0076BFE0();
	bool silent = true;
	int previous_tick = TourCampaign.Get()->Get_Previous_Tick(index);

	if (current_tick - previous_tick > 8) {
		previous_tick = current_tick - 8;
	}

	if (previous_tick != current_tick) {
		silent = !Present_History(previous_tick, current_tick);
	} else {
		bool mega = false;
		int outcome = Get_Outcome(current_tick, mega);
		VoicePlayer.FUN_007712C0(2, outcome, mega, true, 0);
	}

	{
		WideWstring blank((wchar_t const *)NULL);
		Set_Text(blank, 0);
	}
	Check_For_Breakout();

	Do_Target_Anim(TourCampaign.Get()->CycleHistory->FUN_00767BB0(current_tick), true, silent);
	TourCampaign.Get()->Write_INI(index);
}

// FUNCTION: GAMEMD 0x0076E430
// WorldDominationTour::Selection::Get_Outcome
int Selection::Get_Outcome(int tick, bool & mega)
{
	rc_ptr<State> state = TourCampaign->CycleHistory->FUN_00767BB0(tick);
	if (state.Get() != NULL) {
		int owned = state->FUN_0076F670(TourCampaign->PlayerFaction);
		if (owned <= LosingThreshold) {
			mega = owned <= MegaLosingThreshold;
			return(OUTCOME_LOSE);
		}
		if (owned >= WinningThreshold) {
			mega = owned >= MegaWinningThreshold;
			return(OUTCOME_WIN);
		}
	}

	mega = false;
	return(OUTCOME_DRAW);
}

// FUNCTION: GAMEMD 0x0076E4F0
// WorldDominationTour::Selection::Present_Ticks
bool Selection::Present_Ticks(int tick_from, int tick_to)
{
	int i = tick_from;
	bool first = true;
	int len = 0;

	while (i <= tick_to) {
		Check_For_Breakout();
		if (Brokeout) {
			break;
		}
		{
			WideWstring str1(StringTable::LoadString("WDT:ReviewingHistory", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x3e3));
			str1 += L"\n";
			if (first) {
				len = str1.length();
			}
			str1 += WideWstring(StringTable::LoadString("WDT:ReviewingHistoryDay", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x3eb)) + WideWstring(L" ") + FUN_007B7E40(i + 1, 2);
			Set_Text(str1, first == 0 ? len : 0);
			Do_Target_Anim(TourCampaign.Get()->CycleHistory->FUN_00767BB0(i), false, first);
			first = false;
			Wait_For_Anim(TextAnim, 300000);
			int loops = 5;
			while (!Brokeout) {
				if (loops-- == 0) {
					break;
				}
				Wait_Delay(5);
				Check_For_Breakout();
			}
			if (Brokeout) {
				break;
			}
		}
		i++;
	}

	return(first);
}

// FUNCTION: GAMEMD 0x0076E750
// WorldDominationTour::Selection::Presentation
void Selection::Presentation(int index)
{
	Set_Text(L"", 0);
	int current_tick = TourCampaign->Properties->FUN_0076BFE0();
	int previous_tick = TourCampaign->Get_Previous_Tick(index);
	if (current_tick - previous_tick > 8) {
		previous_tick = current_tick - 8;
	}

	rc_ptr<State> previous_state = TourCampaign->CycleHistory->FUN_00767BB0(previous_tick);
	if (previous_state.Get() != NULL) {
		int faction = TourCampaign->PlayerFaction;
		int owned = previous_state->FUN_0076F670(faction);
		if (owned >= MegaWinningThreshold) {
			VoicePlayer.FUN_007712C0(0, OUTCOME_WIN, owned >= 30, true, 0);
		} else if (owned <= MegaLosingThreshold) {
			VoicePlayer.FUN_007712C0(0, OUTCOME_LOSE, owned <= 0, true, 0);
		} else {
			VoicePlayer.FUN_007712C0(0, OUTCOME_DRAW, true, true, 0);
		}

		Present_Ticks(previous_tick, current_tick);
		while (VoicePlayer.FUN_007717F0() && !Brokeout) {
			Wait_Delay(5);
			Check_For_Breakout();
		}
	}
}

// FUNCTION: GAMEMD 0x0076E8B0
// WorldDominationTour::Selection::Present_History
bool Selection::Present_History(int tick_from, int tick_to)
{
	rc_ptr<State> before = TourCampaign->CycleHistory->FUN_00767BB0(tick_from);
	int faction = TourCampaign->PlayerFaction;
	int old_count = before->FUN_0076F670(faction);
	rc_ptr<State> after = TourCampaign->CycleHistory->FUN_00767BB0(tick_to);
	faction = TourCampaign->PlayerFaction;
	int new_count = after->FUN_0076F670(faction);
	int change = new_count - old_count;

	int outcome;
	bool emphasis;
	if (change < -1) {
		outcome = OUTCOME_LOSE;
		emphasis = change < -3 || new_count <= MegaLosingThreshold;
	} else if (change > 1) {
		outcome = OUTCOME_WIN;
		emphasis = change > 3 || new_count >= MegaWinningThreshold;
	} else {
		outcome = OUTCOME_DRAW;
		bool mega = false;
		emphasis = Get_Outcome(tick_to, mega) == OUTCOME_LOSE && mega;
	}

	VoicePlayer.FUN_007712C0(1, outcome, emphasis, true, 0);
	return(!Present_Ticks(tick_from, tick_to));
}

// FUNCTION: GAMEMD 0x0076E9D0
// WorldDominationTour::Selection::Check_For_Breakout
void Selection::Check_For_Breakout(void)
{
	Wait_For_Focus();
	while (InputManagerClass::Instance->FUN_0054F000()) {
		if (InputManagerClass::Instance->FUN_0054F050() == VK_ESCAPE) {
			Brokeout = true;
		}
	}
}

// FUNCTION: GAMEMD 0x0076EA20
// WorldDominationTour::Selection::Set_Text
void Selection::Set_Text(WideWstring const & text, int start_delay)
{
	if (TextAnim != NULL) {
		Remove_Anim(TextAnim);
		HiddenSurface->Blit_From(WorldMap->TextRect, *AlternateSurface, WorldMap->TextRect);
		Add_Update_Rect(WorldMap.Get()->TextRect);
		TextAnim = NULL;
	}

	if (text.length() > 0) {
		WideWstring copy(text);
		TextAnim = new MSBitPrintAnim(copy, WorldMap.Get()->TextRect, true);
		TextAnim->Set_Flag(true);
		Add_Animation(TextAnim);
	}
}

// FUNCTION: GAMEMD 0x0076EAF0
// WorldDominationTour::Selection::Set_Tooltip
void Selection::Set_Tooltip(WideWstring const & text)
{
	Rect rect = WorldMap.Get()->TooltipRect;
	if (TooltipAnim != NULL) {
		Remove_Anim(TooltipAnim);
		HiddenSurface->Blit_From(rect, *AlternateSurface, rect);
		Add_Update_Rect(rect);
		TooltipAnim = NULL;
	}

	if (text.length() > 0) {
		WideWstring copy(text);
		TooltipAnim = new MSBitPrintAnim(copy, rect, false);
		TooltipAnim->Set_Flag(true);
		Add_Animation(TooltipAnim);
	}
}

// FUNCTION: GAMEMD 0x0076EBE0
// WorldDominationTour::Selection::Play_SFX
void Selection::Play_SFX(Wstring const & name)
{
	if (name != (char const *)NULL) {
		SFX_ITER it(SfxEntries, 0);
		for (; it; it++) {
			if (!(name == it->Get_Name())) {
				break;
			}
		}
		if (it) {
			it->Play();
		}
	}
}

// FUNCTION: GAMEMD 0x0076EC80
// WorldDominationTour::Selection::Set_Long_Description
void Selection::Set_Long_Description(void)
{
	Set_Text(TourCampaign->Properties->LongDesc, 0);
	Set_Tooltip(StringTable::LoadString("WDT:TerritorySelectToolTip", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x4ce));
}

// FUNCTION: GAMEMD 0x0076ECD0
void __fastcall FUN_0076ECD0(rc_ptr<Campaign> * campaign, int theater)
{
	InputManagerClass::Instance->FUN_0054F720();
	Selection sel(*campaign, theater);
	sel.Presentation(theater);
}

// FUNCTION: GAMEMD 0x0076EEB0
// WorldDominationTour::Selection::FUN_0076EEB0
void Selection::FUN_0076EEB0(void)
{
	UserInputLocked = false;
	if (ThemeName != (char const *)NULL) {
		Theme.Play_Song(Theme.From_Name(ThemeName.get()));
		Theme.Set_Repeat(true);
	}
	MouseCursor->Hide_Mouse();
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	VisibleSurface->Blit_From(*HiddenSurface);
	InputManagerClass::Instance->FUN_0054F720();
	WorldMap.Get()->FUN_007692F0(this, &VoicePlayer);
	while (MouseCursor->Get_Mouse_State() < 0) {
		MouseCursor->Show_Mouse();
	}
}

// FUNCTION: GAMEMD 0x0076EF80
// WorldDominationTour::Selection::End
void Selection::End(void)
{
	MouseCursor->Hide_Mouse();
	Theme.Queue_Song((ThemeType)-3);
	AlternateSurface->Fill(0);
	HiddenSurface->Fill(0);
	while (MouseCursor->Get_Mouse_State() < 0) {
		MouseCursor->Show_Mouse();
	}
}

// TEMPLATE: GAMEMD 0x0076F000
// ?FUN_0076F000@Map@WorldDominationTour@@QAE?AV?$rc_ptr@VTerritory@WorldDominationTour@@@@H@Z
rc_ptr<Territory> Map::FUN_0076F000(int id)
{
	return(FUN_0076FF40(&Territories, id));
}

// TEMPLATE: GAMEMD 0x0076F020
// ?Get@?$rc_ptr@VCampaignProperties@WorldDominationTour@@@@QBEPAVCampaignProperties@WorldDominationTour@@XZ
template class rc_ptr<CampaignProperties>;

// TEMPLATE: GAMEMD 0x0076F030
// ??0?$rc_ptr@VTerritory@WorldDominationTour@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x0076F060
// ??1?$rc_ptr@VTerritory@WorldDominationTour@@@@UAE@XZ
// TEMPLATE: GAMEMD 0x0076F070
// ?Get@?$rc_ptr@VTerritory@WorldDominationTour@@@@QBEPAVTerritory@WorldDominationTour@@XZ
template class rc_ptr<Territory>;

// TEMPLATE: GAMEMD 0x0076F080
// ?Get@?$rc_ptr@VMap@WorldDominationTour@@@@QBEPAVMap@WorldDominationTour@@XZ
template class rc_ptr<Map>;

// TEMPLATE: GAMEMD 0x0076F090
// ??0?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@QAE@AAV?$DynamicVectorClass@PAVMSSfxEntry@@@@H@Z
// TEMPLATE: GAMEMD 0x0076F0B0
// ??B?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UBE_NXZ
// TEMPLATE: GAMEMD 0x0076F0C0
// ??0?$VectorCursor@V?$rc_ptr@VTerritory@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@@@QAE@AAV?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@H@Z
// TEMPLATE: GAMEMD 0x0076F0E0
// ??B?$VectorCursor@V?$rc_ptr@VTerritory@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@@@UBE_NXZ
// TEMPLATE: GAMEMD 0x0076F0F0
// ??B?$VectorCursor@PAVMSAnim@@V?$DynamicVectorClass@PAVMSAnim@@@@@@UBE_NXZ
// SYNTHETIC: GAMEMD 0x0076F100
// ??_G?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0076F120
// ??_G?$VectorCursor@V?$rc_ptr@VTerritory@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x0076F140
// ??_G?$VectorCursor@PAVMSAnim@@V?$DynamicVectorClass@PAVMSAnim@@@@@@UAEPAXI@Z
// TEMPLATE: GAMEMD 0x0076F160
// ??E?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F170
// ??F?$VectorCursor@PAVMSSfxEntry@@V?$DynamicVectorClass@PAVMSSfxEntry@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F180
// ??E?$VectorCursor@V?$rc_ptr@VTerritory@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F190
// ??F?$VectorCursor@V?$rc_ptr@VTerritory@WorldDominationTour@@@@V?$DynamicVectorClass@V?$rc_ptr@VTerritory@WorldDominationTour@@@@@@@@UAEAAV0@XZ
template class VectorCursor<rc_ptr<Territory>, DynamicVectorClass<rc_ptr<Territory> > >;
template class VectorCursor<MSAnim *, DynamicVectorClass<MSAnim *> >;
// TEMPLATE: GAMEMD 0x0076F1A0
// ??E?$VectorCursor@PAVMSAnim@@V?$DynamicVectorClass@PAVMSAnim@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F1B0
// ??F?$VectorCursor@PAVMSAnim@@V?$DynamicVectorClass@PAVMSAnim@@@@@@UAEAAV0@XZ
// TEMPLATE: GAMEMD 0x0076F1C0
// ??0?$VectorClass@PAVMSAnim@@@@QAE@HPBQAVMSAnim@@@Z
// TEMPLATE: GAMEMD 0x0076F210
// ??0?$VectorClass@PAVMSSfxEntry@@@@QAE@HPBQAVMSSfxEntry@@@Z
template class VectorClass<MSAnim *>;
template class VectorClass<MSSfxEntry *>;
template class VectorCursor<MSSfxEntry *, DynamicVectorClass<MSSfxEntry *> >;
