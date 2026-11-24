// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#define NDEBUG
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
#include "vcursor.h"
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

#include "bitfont.h"
#include "dsurface.h"
#undef LoadString
#include "stringtable.h"

namespace OwnerDraw { extern int PrimaryTextColor; }

static WideWstring Load_Selection_Text(char const * label, int source_line)
{
	return(WideWstring(StringTable::LoadString(label, NULL, "D:\\ra2mdpost\\WDTSel.cpp", source_line)));
}

// FUNCTION: GAMEMD 0x0076C290
// WorldDominationTour::Selection::Selection
Selection::Selection(rc_ptr<Campaign> const & campaign, int theater) :
	MSEngine(),
	TourCampaign(campaign),
	SelectedConflict(),
	VoicePlayer(campaign->Get_Faction()),
	Brokeout(false),
	LastPoint(-1, -1),
	StartTick(campaign->Properties->Get_Current_Tick()),
	LastHoverZone(-1),
	WorldMap(FUN_00767FB0(campaign->Properties->Get_Map_ID(), theater, campaign->Get_Faction())),
	HoveredTerritoryID(-1),
	MousedTerritory(),
	TargetDrawer(NULL),
	ZoomingTarget(NULL),
	ThrobbingTarget(NULL),
	ThrobbingTargetDividingFrame(32),
	TextAnim(NULL),
	TooltipAnim(NULL),
	Font(new MSFont(false)),
	CurrentState(FUN_0076F500()),
	ThemeName((char const *)NULL)
{
	int pixel = DSurface::Build_Hicolor_Pixel(GetRValue(OwnerDraw::PrimaryTextColor), GetGValue(OwnerDraw::PrimaryTextColor), GetBValue(OwnerDraw::PrimaryTextColor));
	TextColor = BitFont::Instance->SetColor(pixel);

	Add_Animation(VoicePlayer.FUN_00771800());

	CCFileClass file("WDTMAPMD.INI");
	INIClass ini;
	ini.Load(file, false);

	Init_Voices(ini, "VoiceOvers");
	Init_Sounds(ini, "SOUNDS");

	Wstring layout;
	ini.Get_String("LAYOUTS", Wstring_FromInt(campaign->Properties->Get_Layout()), "", layout);
	Init_Target(ini, layout);

	WorldMap->FUN_007681E0(ini, this, campaign->Is_Different(theater));

	WideWstring play_text = Load_Selection_Text("GUI:PlayWDT", 0x9d);
	MSBitPrintAnim * play = new MSBitPrintAnim(play_text, WorldMap->TitleRect, true);
	play->Set_Flag(true);
	Add_Animation(play);

	WideWstring back_text = Load_Selection_Text("TXT_BACK", 0xa4);
	MSBitPrintAnim * back = new MSBitPrintAnim(back_text, WorldMap->BackTextRect, true);
	back->Set_Flag(true);
	Add_Animation(back);

	Start();
}

// FUNCTION: GAMEMD 0x0076C6E0
// WorldDominationTour::Selection::~Selection
Selection::~Selection(void)
{
	End();

	MousedTerritory = NULL;

	Remove_Target_Anims();
	Remove_Target_Anims2();

	for (SFX_ITER sfx(SfxEntries); sfx; sfx++) {
		delete *sfx;
	}

	delete TargetDrawer;
	delete ZoomingTarget;
	delete ThrobbingTarget;
	delete Font;

	BitFont::Instance->SetColor((unsigned short)TextColor);
}


// FUNCTION: GAMEMD 0x0076C970
// WorldDominationTour::Selection::Init_Voices
void Selection::Init_Voices(INIClass const & ini, Wstring const & section)
{
	VoicePlayer.Init(ini, section);
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
	Wstring faction_name = *FUN_007AF2A0(TourCampaign->Get_Faction());

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

bool WOL_Wait_Callback(void);

// FUNCTION: GAMEMD 0x0076CDA0
// WorldDominationTour::Selection::Pick_Territory
rc_ptr<Territory> Selection::Pick_Territory(void)
{
	Set_Long_Description();
	bool canceled = false;
	bool selected = false;
	Keyboard->Clear();
	do {
		WOL_Wait_Callback();
		Wait_For_Focus();
		Point2D point(MouseCursor->Get_Mouse_X(), MouseCursor->Get_Mouse_Y());
		if (Keyboard->Check() != KN_NONE) {
			int key = Keyboard->Get();
			if (key == KN_LMOUSE) {
				Handle_Click(point, selected);
			} else if (key == (KN_LMOUSE | WWKEY_RLS_BIT)) {
				WorldMap->FUN_007693A0(this, false);
				if (WorldMap->FUN_00769440(point)) {
					Play_SFX("Canceled");
					canceled = true;
				}
			}
		} else if (point != LastPoint) {
			LastPoint = point;
			int zone = WorldMap->FUN_00769440(point) ? 2 : 1;
			if (LastHoverZone != zone) {
				switch (LastHoverZone) {
				case 1: On_Map_Leave(); break;
				case 2: On_Cancel_Leave(); break;
				}
			}
			switch (zone) {
			case 1:
				if (LastHoverZone != 1) On_Map_Enter();
				Update_Hovered_Territory(point);
				break;
			case 2:
				if (LastHoverZone != 2) On_Cancel_Enter();
				break;
			}
			LastHoverZone = zone;
		}
		Wait_Delay(1);
	} while (!canceled && !selected);
	return(selected ? MousedTerritory : rc_ptr<Territory>());
}

// FUNCTION: GAMEMD 0x0076D180
// WorldDominationTour::Selection::Handle_Click
void Selection::Handle_Click(Point2D const & point, bool & selected)
{
	int zone = WorldMap->FUN_00769440(point) ? 2 : 1;
	switch (zone) {
		case 1:
			WorldMap->FUN_007693A0(this, false);
			selected = Select_Territory_At(point);
			if (selected) {
				Play_SFX("ConflictSelected");
			}
			break;

		case 2:
			WorldMap->FUN_007693A0(this, true);
			On_Cancel_Enter();
			break;
	}
}

// FUNCTION: GAMEMD 0x0076D3A0
// WorldDominationTour::Selection::Select_Territory_At
bool Selection::Select_Territory_At(Point2D const & point)
{
	Update_Hovered_Territory(point);
	rc_ptr<Territory> territory = HoveredTerritoryID >= 0 ? WorldMap->FUN_0076F000(HoveredTerritoryID) : rc_ptr<Territory>();
	return(Select_Territory(territory));
}

// FUNCTION: GAMEMD 0x0076D470
// WorldDominationTour::Selection::Update_Hovered_Territory
void Selection::Update_Hovered_Territory(Point2D const & point)
{
	int id = WorldMap->FUN_00769330(point);
	if (id != HoveredTerritoryID) {
		HoveredTerritoryID = id;
		Select_Territory(WorldMap->FUN_0076F000(id));
	}
}

WideWstring __fastcall FUN_007B8090(wchar_t const * prefix, WideWstring const & text);

// FUNCTION: GAMEMD 0x0076D4D0
// WorldDominationTour::Selection::Do_Target_Anim
void Selection::Do_Target_Anim(rc_ptr<State> const & state, bool show_targets, bool silent)
{
	Remove_Target_Anims();

	MS_ANIM_LIST zoom_anims;
	MS_ANIM_LIST throb_anims;

	for (TERR_ITER it = Cursor_Of(WorldMap->Territories); it; it++) {
		rc_ptr<Territory> const & territory = it.Cur_Ref();
		int id = territory->ID;
		State * previous = CurrentState.operator->();
		int old_state = previous->Get_Territory_State(id);
		State * incoming = state.operator->();
		int new_state = incoming->Get_Territory_State(id);

		if (new_state != old_state) {
			Check_For_Breakout();
			it->Set_Owner_State(new_state, this, !Brokeout || silent);
		}

		if (!show_targets || new_state != 1) {
			continue;
		}
		Point2D target(WorldMap->Origin + it->Target);

		if (ZoomingTarget != NULL) {
			CCFileClass file(ZoomingTarget->Get_Filename());
			if (file.Is_Available()) {
				MSFadeAnim * fade = new MSFadeAnim(ZoomingTarget->Get_Filename(), target.X, target.Y, TargetDrawer, 3, SHAPE_CENTER, &Anims);
				zoom_anims.Add(fade);
			}
		}

		CCFileClass file(ThrobbingTarget->Get_Filename());
		if (file.Is_Available()) {
			MSShapeAnim * shape = new MSShapeAnim(ThrobbingTarget->Get_Filename(), target.X, target.Y, TargetDrawer, 5, true, SHAPE_CENTER);
			shape->Set_Stop_Frame(ThrobbingTargetDividingFrame - 1);
			shape->Set_Start_Frame(0);
			shape->Set_Frame(0);
			it->Set_Target_Anim(shape);
			throb_anims.Add(shape);
		}
	}

	MS_ANIM_ITER zoom_iter(zoom_anims);
	for (MS_ANIM_ITER throb_iter(throb_anims); throb_iter; throb_iter++) {
		Check_For_Breakout();

		if (zoom_iter) {
			if (Brokeout) {
				MSAnim * anim = zoom_iter.Cur();
				if (anim != NULL) {
					delete anim;
				}
			} else {
				Play_SFX("TargetZoom");
				Add_Animation(zoom_iter.Cur());
				Wait_For_Anim(zoom_iter.Cur(), 300000);
			}
			zoom_iter++;
		}

		Add_Animation(throb_iter.Cur());
	}

	CurrentState = state;
}

// FUNCTION: GAMEMD 0x0076DAD0
// WorldDominationTour::Selection::Select_Territory
bool Selection::Select_Territory(rc_ptr<Territory> const & territory)
{
	bool was_set = SelectedConflict.Get() != NULL;
	if (territory == MousedTerritory) {
		return(was_set);
	}
	if (MousedTerritory.Get() != NULL) {
		if (SelectedConflict.Get() != NULL) {
			SelectedConflict = NULL;
		}
		MousedTerritory->Set_Highlight(false, this);
		MSShapeAnim * anim = MousedTerritory->TargetAnim;
		if (anim != NULL) {
			anim->Set_Stop_Frame(ThrobbingTargetDividingFrame - 1);
			anim->Set_Start_Frame(0);
			anim->Set_Frame(0);
		}
	}
	MousedTerritory = territory;
	if (MousedTerritory.Get() == NULL) {
		Set_Long_Description();
		VoicePlayer.Queue(1, false, 30);
		return(was_set);
	}

	bool found = false;
	WideWstring buffer;
	WideWstring description;
	int id = MousedTerritory->ID;
	int territory_state = CurrentState->Get_Territory_State(id);
	int outcome = 2;
	if (territory_state != 1 && territory_state != 0) {
		outcome = TourCampaign->PlayerFaction != territory_state;
	}
	VoicePlayer.Queue(3, outcome, false, false, 30);
	switch (territory_state) {
	case 2:
		Set_Tooltip(Load_Selection_Text("WDT:AlliedHeldTooltip", 0x2ad));
		break;
	case 3:
		Set_Tooltip(Load_Selection_Text("WDT:SovietHeldTooltip", 0x2b1));
		break;
	case 1:
		Set_Tooltip(Load_Selection_Text("WDT:TerritorySelDispToolTip", 0x2b5));
		break;
	default:
		Set_Tooltip(Load_Selection_Text("WDT:TerritorySelectToolTip", 0x2b9));
		break;
	}
	MousedTerritory->Set_Highlight(true, this);
	if (TourCampaign->Properties->Get_Current_Tick() == StartTick) {
		CONFLICT_ITER it(TourCampaign->Conflicts);
		for (; it; it++) {
			rc_ptr<Conflict> const & handle = it.Cur_Ref();
			Conflict * conflict = handle.operator->();
			int id = MousedTerritory->ID;
			if (conflict->Get_Territory_Index() == id) {
				break;
			}
		}
		if (it) {
			found = true;
			SelectedConflict = it.Cur_Ref();
			buffer = it.Cur_Ref()->Process_Game_Options();
			description = buffer;
			MSShapeAnim * anim = territory->TargetAnim;
			if (anim != NULL) {
				anim->Set_Stop_Frame(-1);
				anim->Set_Start_Frame(ThrobbingTargetDividingFrame);
				anim->Set_Frame(ThrobbingTargetDividingFrame);
			}
		}
	}
	WideWstring dest = FUN_007B8090(StringTable::LoadString("WDT:TerritoryPrefix", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x2d7), MousedTerritory->Name)
		+ StringTable::LoadString("WDT:TerritorySuffix", NULL, "D:\\ra2mdpost\\WDTSel.cpp", 0x2d7);
	if (description.length() > 0) {
		dest += buffer;
	}
	Set_Text(dest, 0);
	return(found);
}

// FUNCTION: GAMEMD 0x0076DED0
// WorldDominationTour::Selection::Remove_Target_Anims
void Selection::Remove_Target_Anims(void)
{
	for (TERR_ITER it(WorldMap->Territories, 0); it; it++) {
		MSShapeAnim * anim = it->Remove_Target_Anim();
		if (anim != NULL) {
			Remove_Anim(anim);
		}
	}
}

// FUNCTION: GAMEMD 0x0076DF50
// WorldDominationTour::Selection::Remove_Target_Anims2
void Selection::Remove_Target_Anims2(void)
{
	for (TERR_ITER it(WorldMap->Territories, 0); it; it++) {
		MSShapeAnim * anim = it->Remove_Target_Anim();
		if (anim != NULL) {
			Remove_Anim(anim);
		}
	}
}

extern bool UserInputLocked;

// FUNCTION: GAMEMD 0x0076DFD0
// WorldDominationTour::Selection::On_Map_Enter
void Selection::On_Map_Enter(void)
{
	Play_SFX("MapEnter");
}

// FUNCTION: GAMEMD 0x0076E090
// WorldDominationTour::Selection::On_Map_Leave
void Selection::On_Map_Leave(void)
{
	Play_SFX("MapLeave");
	HoveredTerritoryID = -1;
	rc_ptr<Territory> const & none = rc_ptr<Territory>();
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
		Set_Tooltip(Load_Selection_Text("WOL:WDTTerritoryBack", 0x37e));
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

	int current_tick = TourCampaign.Get()->Properties->Get_Current_Tick();
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
		VoicePlayer.Queue(2, outcome, mega, true, 0);
	}

	{
		WideWstring blank((wchar_t const *)NULL);
		Set_Text(blank, 0);
	}
	Check_For_Breakout();

	Do_Target_Anim(TourCampaign.Get()->CycleHistory->Get_State(current_tick), true, silent);
	TourCampaign.Get()->Write_INI(index);
}

// FUNCTION: GAMEMD 0x0076E430
// WorldDominationTour::Selection::Get_Outcome
int Selection::Get_Outcome(int tick, bool & mega)
{
	rc_ptr<State> state = TourCampaign->CycleHistory->Get_State(tick);
	if (state.Get() != NULL) {
		int owned = state->Count_Owned_Territories(TourCampaign->PlayerFaction);
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
			Do_Target_Anim(TourCampaign.Get()->CycleHistory->Get_State(i), false, first);
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
	int current_tick = TourCampaign->Properties->Get_Current_Tick();
	int previous_tick = TourCampaign->Get_Previous_Tick(index);
	if (current_tick - previous_tick > 8) {
		previous_tick = current_tick - 8;
	}

	rc_ptr<State> previous_state = TourCampaign->CycleHistory->Get_State(previous_tick);
	if (previous_state.Get() != NULL) {
		int owned = previous_state->Count_Owned_Territories(TourCampaign->Get_Faction());
		if (owned >= MegaWinningThreshold) {
			VoicePlayer.Queue(0, OUTCOME_WIN, owned >= 30, true, 0);
		} else if (owned <= MegaLosingThreshold) {
			VoicePlayer.Queue(0, OUTCOME_LOSE, owned <= 0, true, 0);
		} else {
			VoicePlayer.Queue(0, OUTCOME_DRAW, true, true, 0);
		}

		Present_Ticks(previous_tick, current_tick);
		while (VoicePlayer.Playing() && !Brokeout) {
			Wait_Delay(5);
			Check_For_Breakout();
		}
	}
}

// FUNCTION: GAMEMD 0x0076E8B0
// WorldDominationTour::Selection::Present_History
bool Selection::Present_History(int tick_from, int tick_to)
{
	rc_ptr<State> before = TourCampaign->CycleHistory->Get_State(tick_from);
	int old_count = before->Count_Owned_Territories(TourCampaign->Get_Faction());
	rc_ptr<State> after = TourCampaign->CycleHistory->Get_State(tick_to);
	int new_count = after->Count_Owned_Territories(TourCampaign->Get_Faction());
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

	VoicePlayer.Queue(1, outcome, emphasis, true, 0);
	return(!Present_Ticks(tick_from, tick_to));
}

// FUNCTION: GAMEMD 0x0076E9D0
// WorldDominationTour::Selection::Check_For_Breakout
void Selection::Check_For_Breakout(void)
{
	Wait_For_Focus();
	while (Keyboard->Check()) {
		if (Keyboard->Get() == VK_ESCAPE) {
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
	Keyboard->Clear();
	Selection sel(*campaign, theater);
	sel.Presentation(theater);
}

// FUNCTION: GAMEMD 0x0076ED10
bool __fastcall FUN_0076ED10(rc_ptr<Campaign> * campaign, int theater)
{
	if (campaign->Get() == NULL) {
		return(false);
	}

	Selection sel(*campaign, theater);
	sel.Set_Long_Description();
	sel.Do_Target_Selection(theater);
	rc_ptr<Territory> selected = sel.Pick_Territory();
	bool valid = selected.Get() != NULL;
	if (valid) {
		int index = selected->ID;
		Session.WDTTerritory = index;
		rc_ptr<WDTTerritory> territory = WDT_Get_Territory(index);
		if (territory.Get() != NULL) {
			Session.Options.UnitCount = territory->UnitCount;
			BuildLevel = territory->TechLevel;
			Session.Options.Credits = territory->StartingCredits;
			Session.Options.AlliesAllowed = territory->AlliesAllowed;
			Session.Options.HarvTruce = false;
			Session.Options.Bases = territory->Bases;
			Session.Options.MCVRedeploy = territory->MCVRedeploy;
			Session.Options.FogOfWar = false;
			Session.Options.BridgeDestruction = true;
			Session.Options.Goodies = territory->Goodies;
			Session.Options.ShortGame = territory->ShortGame;
			Session.Options.SWAllowed = territory->BlueTiberium;
			Session.Options.BuildOffAlly = false;
			Session.Options.AIPlayers = 0;
			Session.Options.AIDifficulty = DIFF_NORMAL;
		} else {
			valid = false;
		}
	}
	return(valid);
}

// FUNCTION: GAMEMD 0x0076EEB0
// WorldDominationTour::Selection::Start
void Selection::Start(void)
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
	Keyboard->Clear();
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

// SYNTHETIC: GAMEMD 0x0076EFE0
// ??_GSelection@WorldDominationTour@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x0076F000
// ?FUN_0076F000@Map@WorldDominationTour@@QAE?AV?$rc_ptr@VTerritory@WorldDominationTour@@@@H@Z
rc_ptr<Territory> Map::FUN_0076F000(int id)
{
	return(Find_Territory_By_ID(&Territories, id));
}

// TEMPLATE: GAMEMD 0x0076F020
// ?Get@?$rc_ptr@VCampaignProperties@WorldDominationTour@@@@QBEPAVCampaignProperties@WorldDominationTour@@XZ
template class rc_ptr<CampaignProperties>;

// TEMPLATE: GAMEMD 0x0076F030
// ??0?$rc_ptr@VTerritory@WorldDominationTour@@@@QAE@ABV0@@Z
// TEMPLATE: GAMEMD 0x0076F060
// ??1?$rc_ptr@VTerritory@WorldDominationTour@@@@UAE@XZ
// TEMPLATE: GAMEMD 0x0076F070
// ??C?$rc_ptr@VTerritory@WorldDominationTour@@@@QBEPAVTerritory@WorldDominationTour@@XZ
template class rc_ptr<Territory>;

// TEMPLATE: GAMEMD 0x0076F080
// ??C?$rc_ptr@VMap@WorldDominationTour@@@@QBEPAVMap@WorldDominationTour@@XZ
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
