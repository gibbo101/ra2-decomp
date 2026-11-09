// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "dropship.h"
#include "_alpha.h"
#include "_keyboar.h"
#include "_surface.h"
#include "_zbuffer.h"
#include "bsurface.h"
#include "ccfile.h"
#include "ccrand.h"
#include "conquer.h"
#include "convert.h"
#include "dialog.h"
#include "draw.h"
#include "dsurface.h"
#include "fastmath.h"
#include "globals.h"
#include "house.h"
#include "infatype.h"
#include "keyboard.h"
#include "misc.h"
#include "scenario.h"
#include "scheme.h"
#include "shapebtn.h"
#include "stimer.h"
#include "swizzle.h"
#include "techtype.h"
#include "unittype.h"
#include "rgb.h"
#include "vector.h"
#include "weapon.h"
#include "bitfont.h"
#include <stdio.h>
#include <wchar.h>
#include "zbuffer.h"
#include <climits>
#include <cstring>

void Draw_Cost(int cost, int maxcost, Surface & surface, Point2D const & drawpoint, wchar_t const * title, bool center, ConvertClass & drawer, Blitter const * blitter);
void Draw_Unit_Info(Surface * surface, ConvertClass * drawer, TechnoTypeClass * techtype, bool allowed);

// SYNTHETIC: GAMEMD 0x004B67A0 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x004B67D0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x004B67F0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x004B6810 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x004B6830 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x004B6850 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x004B6870 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x004B68B0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x004B68E0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x004B6910 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x004B6940 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x004B6950 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x004B6970 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x004B6990 SYMBOL
// _$E41

// FUNCTION: GAMEMD 0x004B69B0
DropshipLoadoutClass::DropshipLoadoutClass(void) :
	CreationFrame(Frame),
	Unused2(INT_MAX),
	UnusedBool1(false),
	EntryCount(0),
	TotalCost(0)
{
}

// FUNCTION: GAMEMD 0x004B69D0
DropshipLoadoutClass::~DropshipLoadoutClass(void)
{
}

// FUNCTION: GAMEMD 0x004B69E0
bool DropshipLoadoutClass::Add(TechnoTypeClass * ttype)
{
	if (EntryCount < MAX_ENTRIES) {
		int usage_index = Get_Allowable_Index(ttype);

		if (usage_index != -1 && Scen->AllowableUnitCounts[usage_index] >= Scen->AllowableUnitMaximums[usage_index]) {
			return(false);
		}

		Entries[EntryCount++] = ttype;
		TotalCost += ttype->Raw_Cost();

		if (usage_index != -1) {
			Scen->AllowableUnitCounts[usage_index]++;
		}
		return(true);
	}

	return(false);
}

// FUNCTION: GAMEMD 0x004B6AB0
bool DropshipLoadoutClass::Remove(int index)
{
	if (EntryCount > 0 && index >= 0 && index < EntryCount) {
		int usage_index = Get_Allowable_Index(Entries[index]);

		if (usage_index != -1) {
			Scen->AllowableUnitCounts[usage_index]--;
		}
		TotalCost -= Entries[index]->Raw_Cost();

		for (int i = index; i < EntryCount - 1; ++i) {
			Entries[i] = Entries[i + 1];
		}

		EntryCount--;
		Entries[EntryCount] = NULL;

		return(true);
	}
	return(false);
}

// FUNCTION: GAMEMD 0x004B6B80
void DropshipLoadoutClass::Clear(void)
{
	UnusedBool1 = false;
	int count = EntryCount;
	while (count > 0) {
		Remove(0);
		count--;
	}
	EntryCount = 0;
	TotalCost = 0;
}

// FUNCTION: GAMEMD 0x004B6BB0
void DropshipLoadoutClass::Swizzle(void)
{
	for (int i = 0; i < EntryCount; i++) {
		Swizzle_Pointer(&Entries[i]);
	}
}

// FUNCTION: GAMEMD 0x004B6BE0
TechnoTypeClass * DropshipLoadoutClass::Fetch(int index)
{
	return(Entries[index]);
}

// FUNCTION: GAMEMD 0x004B6BF0
int DropshipLoadoutClass::Get_Allowable_Index(TechnoTypeClass *ttype)
{
	TypeList<TechnoTypeClass *> &units = Scen->AllowableUnits;
	if (!units.Count()) {
		return -1;
	}
	for (int index = 0; index < units.Count(); ++index) {
		if (units[index] == ttype && Scen->AllowableUnitMaximums[index] != -1) {
			return index;
		}
	}
	return -1;
}

#include "msanim.h"
#include "abuffer.h"
#include "gscreen.h"
#include "_xmouse.h"
#include <windows.h>
#undef LoadString
#include "stringtable.h"

struct FileSystem {
	static void const * LoadFile(char const *name, bool as_shape);
	static PaletteClass TEMPERAT_PAL;
	static ConvertClass * CAMEO_PAL;
};

struct ButtonFadeEffect
{
	unsigned int StartTime;
	int CandidateIndex;
	float Alpha;
	float Direction;
	bool StopAtLow;

	ButtonFadeEffect(unsigned int start_time, int candidate_index, float alpha, float direction, bool stop_at_low) :
		StartTime(start_time),
		CandidateIndex(candidate_index),
		Alpha(alpha),
		Direction(direction),
		StopAtLow(stop_at_low)
	{
	}
};

struct CrossDissolveEffect
{
	Rect FromRect;
	Rect ToRect;
	Rect DestRect;
	BSurface *FromSurface;
	BSurface *ToSurface;
	int Alpha;
	unsigned int StartTime;
	Blitter const *FromBlitter;
	Blitter const *ToBlitter;
	bool WasDrawn;

	CrossDissolveEffect(Rect const &from_rect, Rect const &to_rect, Rect const &dest_rect, BSurface *from_surface, BSurface *to_surface, Blitter const *from_blitter, Blitter const *to_blitter) :
		FromRect(from_rect),
		ToRect(to_rect),
		DestRect(dest_rect),
		FromSurface(from_surface),
		ToSurface(to_surface),
		Alpha(0),
		StartTime(::timeGetTime()),
		FromBlitter(from_blitter),
		ToBlitter(to_blitter),
		WasDrawn(false)
	{
	}
};

// FUNCTION: GAMEMD 0x004B6C30
void __fastcall Dropship_Screen(void)
{
	int i;
	int j;
	int k;

	int dropship_count = Scen->StartingDropships;
	int money = PlayerPtr->Available_Money();
	unsigned int money_display = 0;

	static const char *_drop_btn_names[] = {
		"DROP0001.SHP",
		"DROP0002.SHP",
		"DROP0003.SHP",
	};

	static const char *_green_btn_names[] = {
		"DGREEN1.SHP",
		"DGREEN2.SHP",
		"DGREEN3.SHP",
		"DGREEN4.SHP",
	};

	static const int _cameo_count = 8;
	static const int _cameo_x = 491;
	static const int _cameo_y = 25;
	static const int _cameo_width = 64;
	static const int _cameo_height = 48;
	static const int _cameo_gap_x = 4;
	static const int _cameo_gap_y = 2;
	static const int _up_x = 527;
	static const int _up_y = 229;
	static const int _down_x = 557;
	static const int _down_y = 229;
	static const int _up_index = 8;
	static const int _down_index = 9;
	static const int _slot_index = 10;
	static const int _slot_width = 64;
	static const int _slot_gap_x = 2;
	static const int _slot_x = 53;
	static const int _slot_height = 48;
	static const int _minimum_cost = 10;
	static const int _light_rate = 100;
	static const int _light_x = 371;
	static const int _fade_rate = 10;
	static const int _fade_low = 63;
	static const int _fade_scale = 70;
	static const int _loadout_rate = 250;
	static const int _loadout_x = 45;
	static const int _loadout_y = 0;
	static const int _dissolve_scale = 30;
	static const int _dissolve_rate = 7;
	static const int _money_x = 500;
	static const int _money_y = 385;
	static const int _money_rate = 50;
	static const int _click_delay = 300;
	static const int _button_width = 64;
	static const int _pilot_rate = 150;
	static const float _pilot_chance = 0.02f;
	static const int _pilot_x = 284;
	static const int _pilot_y = 151;
	static const int _pilot_delay = 5;

	static const int _loadout_slot_ys[3][3][DropshipLoadoutClass::MAX_ENTRIES] = {
		{
			{69, 69, 119, 119, 119},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0}
		},
		{
			{69, 69, 119, 119, 119},
			{209, 209, 259, 259, 259},
			{0, 0, 0, 0, 0}
		},
		{
			{39, 39, 89, 89, 89},
			{159, 159, 209, 209, 209},
			{279, 279, 329, 329, 329}
		}
	};

	static const int _green_light_ys[] = {
		10, 60, 110, 160
	};

	SHPReference *green_shapes[4];
	for (i = 0; i < 4; ++i) {
		CCFileClass file(_green_btn_names[i]);
		unsigned char *shape_buf = new unsigned char[file.Size()];
		file.Read(shape_buf, file.Size());
		green_shapes[i] = (SHPReference *)shape_buf;
	}

	SHPStruct *loadout_shape = (SHPStruct *)FileSystem::LoadFile("LOADOUT.SHP", false);
	SHPStruct *pilotlight_shape = (SHPStruct *)FileSystem::LoadFile("PILOTLIT.SHP", false);
	SHPStruct *no_cameo_shape = (SHPStruct *)FileSystem::LoadFile("XXICON.SHP", false);

	CCFileClass drop_shape_file(_drop_btn_names[dropship_count - 1]);
	SHPReference *drop_shape_buf = (SHPReference *)new unsigned char[drop_shape_file.Size()];
	drop_shape_file.Read(drop_shape_buf, drop_shape_file.Size());
	SHPStruct *drop_shape = drop_shape_buf;

	BSurface *background_surface = new BSurface(drop_shape->Get_Width(), drop_shape->Get_Height(), 1, drop_shape->GetPixels(0));

	int drop_height = drop_shape->Get_Height();
	int drop_width = drop_shape->Get_Width();
	Point2D screen_origin((HiddenSurface->Get_Width() - drop_width) / 2, (HiddenSurface->Get_Height() - drop_height) / 2);

	PaletteClass palette_data;
	memmove(&palette_data, FileSystem::LoadFile("DROPSHIP.PAL", false), sizeof(palette_data));
	for (i = 0; i < 256; ++i) {
		palette_data[i] = RGBClass(palette_data[i].Get_Red() * 4, palette_data[i].Get_Green() * 4, palette_data[i].Get_Blue() * 4);
	}

	ConvertClass *drawer_dropship = new ConvertClass(palette_data, FileSystem::TEMPERAT_PAL, *VisibleSurface, 53, false);
	ConvertClass *drawer_dropship_alpha = new ConvertClass(palette_data, FileSystem::TEMPERAT_PAL, *VisibleSurface, 1, false);

	memmove(&palette_data, FileSystem::LoadFile("CAMEO.PAL", false), sizeof(palette_data));
	for (i = 0; i < 256; ++i) {
		palette_data[i] = RGBClass(palette_data[i].Get_Red() * 4, palette_data[i].Get_Green() * 4, palette_data[i].Get_Blue() * 4);
	}
	ConvertClass *drawer_cameo = new ConvertClass(palette_data, FileSystem::TEMPERAT_PAL, *VisibleSurface, 1, false);

	Blitter const *plain_blitter = drawer_dropship->Blitter_From_Flags(SHAPE_NORMAL);
	Blitter const *cameo_alpha_blitter = drawer_cameo->Blitter_From_Flags(SHAPE_ALPHA_BLEND);
	Blitter const *drop_alpha_blitter = drawer_dropship_alpha->Blitter_From_Flags(SHAPE_ALPHA_BLEND);

	HiddenSurface->Fill(0);

	DynamicVectorClass<TechnoTypeClass *> candidates;
	int type_count = InfantryTypes.Count() + UnitTypes.Count();

	unsigned int ownable_mask = 1 << HouseTypes.ID(PlayerPtr->Class);

	if (Scen->AllowableUnits.Count() > 0) {
		for (i = 0; i < Scen->AllowableUnits.Count(); ++i) {
			TechnoTypeClass *techtype = Scen->AllowableUnits[i];
			if ((techtype->Ownable & ownable_mask) && Scen->AllowableUnitMaximums[i] != 0) {
				candidates.Add(techtype);
			}
		}
	} else {
		for (k = 0; k < type_count; ++k) {
			TechnoTypeClass *techtype = 0;
			if (k < InfantryTypes.Count()) {
				techtype = InfantryTypes[k];
			} else {
				techtype = UnitTypes[k - InfantryTypes.Count()];
			}

			if (techtype->Level > PlayerPtr->Control.TechLevel) {
				continue;
			}
			if (techtype->Fetch_RTTI() == RTTI_INFANTRYTYPE && techtype->Category == CATEGORY_CIVILIAN) {
				continue;
			}
			if (techtype->Level == -1) {
				continue;
			}
			if (techtype->Raw_Cost() <= _minimum_cost) {
				continue;
			}
			if ((techtype->Ownable & ownable_mask) == 0) {
				continue;
			}

			candidates.Add(techtype);
		}
	}

	int cameo_top = 0;
	bool can_scroll_up = false;
	int button_count = _cameo_count + 2 + dropship_count * DropshipLoadoutClass::MAX_ENTRIES;
	bool can_scroll_down = candidates.Count() > _cameo_count;
	ShapeButtonClass **buttons = new ShapeButtonClass *[button_count];
	ToggleClass *button_list = 0;

	int id = 1;
	for (i = 0; i < _cameo_count; ++i) {
		ShapeButtonClass *button = new ShapeButtonClass(id, i < candidates.Count() ? (ShapeSet const *)candidates[i]->Get_Cameo_Data() : (ShapeSet const *)no_cameo_shape, screen_origin.X + _cameo_x + (_cameo_width + _cameo_gap_x) * (i % 2), screen_origin.Y + _cameo_y + (_cameo_height + _cameo_gap_y) * (i / 2), 0, 0, true);
		button->ShapeDrawer = FileSystem::CAMEO_PAL;
		buttons[i] = button;
		if (i == 0) {
			button_list = buttons[0];
		} else {
			button->Add(*button_list);
		}
		++id;
	}

	ShapeButtonClass *up_button = new ShapeButtonClass(id, (ShapeSet const *)FileSystem::LoadFile("DROPUP.SHP", false), screen_origin.X + _up_x, screen_origin.Y + _up_y, 0, 0, false);
	up_button->ShapeDrawer = drawer_dropship;
	up_button->IsSticky = true;
	buttons[_up_index] = up_button;
	up_button->Add(*button_list);
	++id;

	ShapeButtonClass *down_button = new ShapeButtonClass(id, (ShapeSet const *)FileSystem::LoadFile("DROPDOWN.SHP", false), screen_origin.X + _down_x, screen_origin.Y + _down_y, 0, 0, false);
	down_button->ShapeDrawer = drawer_dropship;
	down_button->IsSticky = true;
	buttons[_down_index] = down_button;
	down_button->Add(*button_list);
	++id;

	for (i = 0; i < dropship_count; ++i) {
		for (j = 0; j < DropshipLoadoutClass::MAX_ENTRIES; ++j) {
			int xpos = j * (_slot_width + _slot_gap_x);
			ShapeButtonClass *button = new ShapeButtonClass(id, 0, screen_origin.X + _slot_x + (xpos < 2 * (_slot_width + _slot_gap_x) ? xpos : xpos - 2 * (_slot_width + _slot_gap_x)), screen_origin.Y + _loadout_slot_ys[dropship_count - 1][i][j], 0, 0, false);
			button->ShapeDrawer = FileSystem::CAMEO_PAL;
			buttons[id - 1] = button;
			button->Add(*button_list);
			++id;
		}
	}

	button_list->Turn_On();

	bool full_redraw = true;
	bool accept = false;
	bool recent_click = false;

	unsigned int light_frame[4] = {(unsigned)-1, (unsigned)-1, (unsigned)-1, (unsigned)-1};
	unsigned int light_start[4] = {0, 0, 0, 0};

	bool force_light_redraw = false;

	DynamicVectorClass<ButtonFadeEffect *> button_fades;

	unsigned int screen_start_time = ::timeGetTime();
	unsigned int money_display_time = screen_start_time;
	unsigned int last_input_time = 0;
	unsigned int pilot_start_time = 0;
	int pilot_frame = -1;

	CDTimerClass<SystemTimerClass> pilot_timer = _pilot_delay;
	int loadout_anim_frame = 0;

	DynamicVectorClass<CrossDissolveEffect *> cross_dissolves;
	bool force_info_redraw = false;

	TechnoTypeClass *last_hover_type = 0;
	int last_hover_slot = -1;
	int selected_count = 0;

	DynamicVectorClass<TechnoTypeClass *> selections;

	Rect alpha_area = AlphaBuffer->Bounds;
	Hide_Mouse();
	delete AlphaBuffer;
	AlphaBuffer = 0;
	AlphaBuffer = new ABuffer(Rect(0, 0, HiddenSurface->Get_Width(), HiddenSurface->Get_Height()));
	Show_Mouse();

	LogicalSurface = HiddenSurface;
	Keyboard->Clear();

	while (!accept) {
		bool redraw = false;

		if (SurfacesRestored) {
			SurfacesRestored = false;
			full_redraw = true;
		}

		if (full_redraw) {
			full_redraw = false;
			redraw = true;
			Draw_Shape(*HiddenSurface, *drawer_dropship, (ShapeSet const *)drop_shape, 0, Point2D(0, 0), Rect(screen_origin.X, screen_origin.Y, drop_shape->Get_Width(), drop_shape->Get_Height()), SHAPE_NORMAL);

			for (i = 0; i < button_count; ++i) {
				int usage_index = -1;
				if (i + cameo_top < candidates.Count()) {
					usage_index = DropshipLoadoutClass::Get_Allowable_Index(candidates[cameo_top + i]);
				}

				if (i >= _cameo_count || usage_index == -1 || Scen->AllowableUnitCounts[usage_index] < Scen->AllowableUnitMaximums[usage_index]) {
					AlphaBuffer->Fill(127, Rect(screen_origin.X + buttons[i]->X, screen_origin.Y + buttons[i]->Y, _cameo_width, _cameo_height));
				} else {
					AlphaBuffer->Fill(_fade_low, Rect(screen_origin.X + buttons[i]->X, screen_origin.Y + buttons[i]->Y, _cameo_width, _cameo_height));
				}
				buttons[i]->Draw_Me(0);
			}
		}

		Call_Back();

		int input = button_list->Input();
		if (!recent_click && (input & 0x8000) != 0) {
			input = input & ~0x8000;
			last_input_time = ::timeGetTime();

			if (input <= _cameo_count && selected_count < dropship_count * DropshipLoadoutClass::MAX_ENTRIES) {
				int candidate_index = cameo_top + input - 1;
				if (candidate_index >= 0 && candidate_index < candidates.Count()) {
					TechnoTypeClass *techtype = candidates[cameo_top + input - 1];

					int usage_index = DropshipLoadoutClass::Get_Allowable_Index(techtype);
					if (money - techtype->Raw_Cost() >= 0 && (usage_index == -1 || Scen->AllowableUnitCounts[usage_index] < Scen->AllowableUnitMaximums[usage_index])) {
						buttons[_slot_index + selected_count]->Set_Shape((ShapeSet const *)techtype->Get_Cameo_Data(), 0, 0);

						SHPStruct const *cameo_shape = (SHPStruct const *)techtype->Get_Cameo_Data();
						ShapeButtonClass *slot_button = buttons[_slot_index + selected_count];
						BSurface *cameo_surface = new BSurface(cameo_shape->Get_Width(), cameo_shape->Get_Height(), 1, cameo_shape->GetPixels(0));

						CrossDissolveEffect *dissolve_effect =
							new CrossDissolveEffect(Rect(slot_button->X - screen_origin.X, slot_button->Y - screen_origin.Y, slot_button->Width, slot_button->Height),
													Rect(0, 0, cameo_shape->Get_Width(), cameo_shape->Get_Height()),
													Rect(slot_button->X, slot_button->Y, slot_button->Width, slot_button->Height),
													background_surface,
													cameo_surface,
													drop_alpha_blitter,
													cameo_alpha_blitter);
						cross_dissolves.Add(dissolve_effect);

						if (usage_index != -1) {
							Scen->AllowableUnitCounts[usage_index]++;
						}
						selections.Add(techtype);

						redraw = true;
						selected_count++;
						recent_click = true;

						int light_row = (input - 1) / 2;
						if (light_frame[light_row] == (unsigned int)-1) {
							light_frame[light_row] = 0;
							light_start[light_row] = ::timeGetTime();
						}

						force_light_redraw = true;
						for (j = 0; ; ++j) {
							if (j >= button_fades.Count()) {
								ButtonFadeEffect *fade = new ButtonFadeEffect(::timeGetTime(), candidate_index, 127.0f, -1.0f, false);
								if (usage_index != -1 && Scen->AllowableUnitCounts[usage_index] >= Scen->AllowableUnitMaximums[usage_index]) {
									force_info_redraw = true;
									fade->StopAtLow = true;
								}
								button_fades.Add(fade);
								break;
							}
							if (button_fades[j]->CandidateIndex == candidate_index) {
								break;
							}
						}

						money -= techtype->Raw_Cost();
					}
				}
			} else if ((input == 9 && can_scroll_up) || (input == 10 && can_scroll_down)) {
				if (input == 9) {
					cameo_top -= 2;
				} else {
					cameo_top += 2;
				}

				for (i = 0; i < _cameo_count; ++i) {
					int candidate_index = cameo_top + i;
					buttons[i]->Set_Shape(candidate_index < candidates.Count() ? (ShapeSet const *)candidates[candidate_index]->Get_Cameo_Data() : (ShapeSet const *)no_cameo_shape, 0, 0);

					int usage_index = -1;
					if (candidate_index < candidates.Count()) {
						usage_index = DropshipLoadoutClass::Get_Allowable_Index(candidates[candidate_index]);
					}
					if (usage_index == -1 || Scen->AllowableUnitCounts[usage_index] < Scen->AllowableUnitMaximums[usage_index]) {
						AlphaBuffer->Fill(127, Rect(buttons[i]->X, buttons[i]->Y, _cameo_width, _cameo_height));
					} else {
						AlphaBuffer->Fill(_fade_low, Rect(buttons[i]->X, buttons[i]->Y, _cameo_width, _cameo_height));
					}
					buttons[i]->Draw_Me(1);
				}

				buttons[input - 1]->IsPressed = false;
				buttons[input - 1]->Draw_Me(1);

				can_scroll_up = cameo_top != 0;
				redraw = true;
				can_scroll_down = cameo_top < candidates.Count() - _cameo_count;
			} else if (input >= 11 && input < dropship_count * DropshipLoadoutClass::MAX_ENTRIES + 11) {
				int remove_index = input - 11;
				if (remove_index < selections.Count()) {
					int usage_index = DropshipLoadoutClass::Get_Allowable_Index(selections[remove_index]);
					if (usage_index != -1) {
						if (Scen->AllowableUnitCounts[usage_index] >= Scen->AllowableUnitMaximums[usage_index]) {
							for (j = 0; j < candidates.Count(); ++j) {
								if (candidates[j] == selections[remove_index]) {
									ButtonFadeEffect *fade = new ButtonFadeEffect(::timeGetTime(), j, 127.0f, -1.0f, false);
									fade->Direction = 1.0f;
									fade->Alpha = 63.0f;
									button_fades.Add(fade);
									break;
								}
							}
						}
						Scen->AllowableUnitCounts[usage_index]--;
					}

					money += selections[remove_index]->Raw_Cost();
					selections.Delete_Index(remove_index);
					redraw = true;

					for (i = 0; i < selected_count - remove_index - 1; ++i) {
						ShapeButtonClass *dest_slot = buttons[input + i - 1];

						SHPStruct const *old_shape = (SHPStruct const *)dest_slot->Get_Shape_Data();
						void *old_storage = ::operator new(sizeof(BSurface));
						BSurface *old_surface;
						if (old_storage) {
							int old_height = old_shape->Get_Height();
							old_surface = new (old_storage) BSurface(old_shape->Get_Width(), old_height, 1, old_shape->GetPixels(0));
						} else {
							old_surface = 0;
						}

						dest_slot->Set_Shape(buttons[input + i]->Get_Shape_Data(), 0, 0);

						SHPStruct const *new_shape = (SHPStruct const *)dest_slot->Get_Shape_Data();
						BSurface *new_surface = new BSurface(new_shape->Get_Width(), new_shape->Get_Height(), 1, new_shape->GetPixels(0));

						CrossDissolveEffect *dissolve_effect =
							new CrossDissolveEffect(Rect(0, 0, dest_slot->Width, dest_slot->Height),
													Rect(0, 0, dest_slot->Width, dest_slot->Height),
													Rect(dest_slot->X, dest_slot->Y, dest_slot->Width, dest_slot->Height),
													old_surface,
													new_surface,
													cameo_alpha_blitter,
													cameo_alpha_blitter);
						cross_dissolves.Add(dissolve_effect);
					}

					ShapeButtonClass *last_slot = buttons[selected_count + 9];
					SHPStruct const *last_shape = (SHPStruct const *)last_slot->Get_Shape_Data();
					BSurface *last_surface = new BSurface(last_shape->Get_Width(), last_shape->Get_Height(), 1, last_shape->GetPixels(0));

					CrossDissolveEffect *dissolve_effect =
						new CrossDissolveEffect(Rect(0, 0, last_shape->Get_Width(), last_shape->Get_Height()),
												Rect(last_slot->X - screen_origin.X, last_slot->Y - screen_origin.Y, last_slot->Width, last_slot->Height),
												Rect(last_slot->X, last_slot->Y, last_slot->Width, last_slot->Height),
												last_surface,
												background_surface,
												cameo_alpha_blitter,
												drop_alpha_blitter);
					cross_dissolves.Add(dissolve_effect);

					last_slot->Set_Shape(0, 0, 0);
					selected_count--;
				}
			}
		} else if (input == 32) {
			accept = true;
		}

		if (buttons[_up_index]->IsPressed && can_scroll_up && !recent_click) {
			buttons[_up_index]->Draw_Me(0);
			redraw = true;
		}
		if (buttons[_down_index]->IsPressed && can_scroll_down && !recent_click) {
			buttons[_down_index]->Draw_Me(0);
			redraw = true;
		}

		for (i = 0; i < 4; ++i) {
			if (light_frame[i] != (unsigned int)-1) {
				int frame = light_frame[i];
				int next_frame = (::timeGetTime() - light_start[i]) / _light_rate;
				if (next_frame != frame || force_light_redraw) {
					int light_y = screen_origin.Y + _green_light_ys[i];
					int light_x = screen_origin.X + _light_x;
					SHPStruct *shape = green_shapes[i];
					if (next_frame > shape->Get_Count()) {
						Blit_Block(
							*HiddenSurface,
							*drawer_dropship,
							*background_surface,
							Rect(light_x - screen_origin.X, light_y - screen_origin.Y, shape->Get_Width(), shape->Get_Height()),
							Point2D(0, 0),
							Rect(light_x, light_y, shape->Get_Width(), shape->Get_Height()),
							0,
							plain_blitter);
						light_frame[i] = (unsigned int)-1;
					} else {
						Draw_Shape(*HiddenSurface, *drawer_dropship, (ShapeSet const *)shape,
							frame,
							Point2D(0, 0),
							Rect(light_x, light_y, shape->Get_Width(), shape->Get_Height()), SHAPE_NORMAL);
						light_frame[i] = next_frame;
					}
					redraw = true;
				}
			}
		}

		for (i = button_fades.Count() - 1; i >= 0; --i) {
			ButtonFadeEffect *effect = button_fades[i];
			int alpha;
			if (effect->Direction < 0.0f) {
				alpha = 127 - (_fade_rate * ::timeGetTime() - _fade_rate * effect->StartTime) / _fade_scale;
				if (alpha < _fade_low) {
					alpha = _fade_low;
					effect->Direction = 1.0f;
					effect->StartTime = ::timeGetTime();
				}
			} else {
				alpha = (_fade_rate * ::timeGetTime() - _fade_rate * effect->StartTime) / _fade_scale + _fade_low;
				if (alpha > 127) {
					alpha = 127;
				}
			}

			float falpha = (float)alpha;
			if (falpha != effect->Alpha) {
				effect->Alpha = falpha;
				if (effect->CandidateIndex >= cameo_top && effect->CandidateIndex <= cameo_top + _cameo_count) {
					int screen_index = effect->CandidateIndex - cameo_top;
					AlphaBuffer->Fill((unsigned short)falpha, Rect(screen_origin.X + _cameo_x + (_cameo_width + _cameo_gap_x) * (screen_index % 2), screen_origin.Y + _cameo_y + (_cameo_height + _cameo_gap_y) * (screen_index / 2), _cameo_width, _cameo_height));
					buttons[screen_index]->Draw_Me(1);
					redraw = true;
				}

				if (effect->Alpha == 127.0f || (effect->StopAtLow && effect->Alpha == 63.0f)) {
					delete effect;
					button_fades.Delete_Index(i);
				}
			}
		}

		if (cross_dissolves.Count() != 0) {
			int dissolve_index = 0;
			if (dissolve_index < cross_dissolves.Count()) {
				int preceding_count = 0;
				do {
					CrossDissolveEffect *effect = cross_dissolves[dissolve_index];
					bool overlap_drawn = false;
					bool draw_background = true;

					j = dissolve_index - 1;
					for (int scan = preceding_count; scan >= 1; --scan) {
						CrossDissolveEffect *earlier = cross_dissolves[j];
						if (earlier->DestRect == effect->DestRect) {
							draw_background = false;
							overlap_drawn = overlap_drawn || earlier->WasDrawn;
						}
						--j;
					}

					int alpha = 255 < (_dissolve_rate * ::timeGetTime() - _dissolve_rate * effect->StartTime) / _dissolve_scale ? 255 : (_dissolve_rate * ::timeGetTime() - _dissolve_rate * effect->StartTime) / _dissolve_scale;

					if (alpha != effect->Alpha || overlap_drawn) {
						effect->Alpha = alpha;
						if (draw_background) {
							Blit_Block(*HiddenSurface, *drawer_dropship, *effect->FromSurface, effect->FromRect, Point2D(0, 0), effect->DestRect, 0, effect->FromBlitter);
						}
						AlphaBuffer->Fill(effect->Alpha, effect->DestRect);
						Blit_Block(*HiddenSurface, *drawer_cameo, *effect->ToSurface, effect->ToRect, Point2D(0, 0), effect->DestRect, 0, effect->ToBlitter);
						effect->WasDrawn = true;
						redraw = true;
					}

					if (alpha == 255) {
						if (effect->FromSurface != background_surface) {
							delete effect->FromSurface;
						}
						if (effect->ToSurface != background_surface) {
							delete effect->ToSurface;
						}
						delete effect;
						cross_dissolves.Delete_Index(dissolve_index);
						--dissolve_index;
						--preceding_count;
					}

					++dissolve_index;
					++preceding_count;
				} while (dissolve_index < cross_dissolves.Count());
			}
		}

		recent_click = (::timeGetTime() - last_input_time) < _click_delay;

		int loadout_count = loadout_shape->Get_Count();
		int next_loadout_frame = ((::timeGetTime() - screen_start_time) / _loadout_rate) % loadout_count;
		if (next_loadout_frame != loadout_anim_frame) {
			loadout_anim_frame = next_loadout_frame;
			Draw_Shape(*HiddenSurface, *drawer_dropship, (ShapeSet const *)loadout_shape, loadout_anim_frame, Point2D(0, 0), Rect(screen_origin.X + _loadout_x, screen_origin.Y, loadout_shape->Get_Width(), loadout_shape->Get_Height()), SHAPE_NORMAL);
			redraw = true;
		}

		unsigned int now = ::timeGetTime();
		if ((unsigned int)money != money_display) {
			unsigned int elapsed = now - money_display_time;
			if (elapsed >= _money_rate) {
				int previous_display = money_display;
				double distance = (double)abs(money - (int)money_display);
				double elapsed_time = (double)elapsed;
				double step = fastmath::sqrt(distance) * elapsed_time * 0.04 < distance ? fastmath::sqrt(distance) * elapsed_time * 0.04 : distance;

				int delta = (int)step;
				if (money < (int)money_display) {
					delta = -delta;
				}

				money_display += delta;
				Point2D money_point(screen_origin.X + _money_x, screen_origin.Y + _money_y);
				Draw_Cost(money_display, previous_display, *background_surface, money_point, StringTable::LoadString("TXT_CREDITS_COLON", 0, "D:\\ra2mdpost\\Dropship.cpp", 873), false, *drawer_dropship, plain_blitter);
				redraw = true;
				money_display_time = now;
			}
		} else {
			money_display_time = now;
		}

		if (pilot_frame >= 0) {
			int next_pilot_frame = (::timeGetTime() - pilot_start_time) / _pilot_rate;
			if (next_pilot_frame != pilot_frame) {
				if (next_pilot_frame < pilotlight_shape->Get_Count()) {
					pilot_frame = next_pilot_frame;
					Draw_Shape(*HiddenSurface, *drawer_dropship, (ShapeSet const *)pilotlight_shape, pilot_frame, Point2D(0, 0), Rect(screen_origin.X + _pilot_x, screen_origin.Y + _pilot_y, pilotlight_shape->Get_Width(), pilotlight_shape->Get_Height()), SHAPE_NORMAL);
					redraw = true;
				} else {
					pilot_frame = -1;
					Blit_Block(*HiddenSurface, *drawer_dropship, *background_surface, Rect(_pilot_x, _pilot_y, pilotlight_shape->Get_Width(), pilotlight_shape->Get_Height()), Point2D(0, 0), Rect(screen_origin.X + _pilot_x, screen_origin.Y + _pilot_y, pilotlight_shape->Get_Width(), pilotlight_shape->Get_Height()), 0, plain_blitter);
					pilot_timer = 60;
					redraw = true;
				}
			}
		} else {
			if (pilot_timer == 0) {
				if (Random_Pick(0, 0x7FFFFFFE) / (double)0x7FFFFFFE < _pilot_chance) {
					pilot_frame = 0;
					pilot_start_time = ::timeGetTime();
					Draw_Shape(*HiddenSurface, *drawer_dropship, (ShapeSet const *)pilotlight_shape, 0, Point2D(0, 0), Rect(screen_origin.X + _pilot_x, screen_origin.Y + _pilot_y, pilotlight_shape->Get_Width(), pilotlight_shape->Get_Height()), SHAPE_NORMAL);
					redraw = true;
				}

				pilot_timer = _pilot_delay;
			}
		}

		Point2D mouse_pos = MouseCursor->Get_Mouse_Point();
		int hover_slot;
		int previous_hover_slot;
		TechnoTypeClass *hover_type = 0;

		int grid_x = screen_origin.X + _cameo_x;
		int grid_y = screen_origin.Y + _cameo_y;
		if (mouse_pos.X >= grid_x && mouse_pos.X < grid_x + 2 * (_cameo_width + _cameo_gap_x) && mouse_pos.Y >= grid_y && mouse_pos.Y < grid_y + 4 * (_cameo_height + _cameo_gap_y)) {
			int col = (mouse_pos.X - screen_origin.X - _cameo_x) / (_cameo_width + _cameo_gap_x);
			int row = (mouse_pos.Y - screen_origin.Y - _cameo_y) / (_cameo_height + _cameo_gap_y);
			int left = screen_origin.X + (_cameo_width + _cameo_gap_x) * col + _cameo_x;
			int top = screen_origin.Y + (_cameo_height + _cameo_gap_y) * row + _cameo_y;
			if (mouse_pos.X >= left
				&& mouse_pos.X < left + _cameo_width
				&& mouse_pos.Y >= top
				&& mouse_pos.Y < top + _cameo_height
				&& cameo_top + col + 2 * row < candidates.Count()) {
				hover_type = candidates[cameo_top + col + 2 * row];
				previous_hover_slot = last_hover_slot;
				hover_slot = col + 2 * row;
				last_hover_slot = hover_slot;
			} else {
				hover_slot = -1;
				previous_hover_slot = last_hover_slot;
				last_hover_slot = -1;
			}
		} else {
			hover_slot = -1;
			previous_hover_slot = last_hover_slot;
			last_hover_slot = -1;
		}

		bool hover_allowed;
		int usage_index = DropshipLoadoutClass::Get_Allowable_Index(hover_type);
		if (usage_index != -1) {
			hover_allowed = Scen->AllowableUnitCounts[usage_index] < Scen->AllowableUnitMaximums[usage_index];
		} else {
			hover_allowed = true;
		}

		if (hover_type != last_hover_type || force_info_redraw) {
			last_hover_type = hover_type;
			Draw_Unit_Info(background_surface, drawer_dropship, hover_type, hover_allowed);
			redraw = true;
		}

		force_light_redraw = false;
		if (redraw) {
			if (hover_slot != previous_hover_slot && previous_hover_slot != -1) {
				buttons[previous_hover_slot]->Draw_Me(1);
			}

			if (hover_slot != -1) {
				int color = hover_allowed ? DSurface::Build_Hicolor_Pixel(0, 255, 0) : DSurface::Build_Hicolor_Pixel(255, 0, 0);
				HiddenSurface->Draw_Rect(Rect(screen_origin.X + _cameo_x + (_cameo_width + _cameo_gap_x) * (hover_slot % 2), screen_origin.Y + _cameo_y + (_cameo_height + _cameo_gap_y) * (hover_slot / 2), _cameo_width, _cameo_height), color);
			}

			Update_Visible_Surface(true, HiddenSurface, 0);
		}
	}

	for (i = 0; i < selections.Count(); ++i) {
		PlayerPtr->DropshipLoadouts[i / DropshipLoadoutClass::MAX_ENTRIES].Add(selections[i]);
	}

	delete drawer_dropship;
	delete drawer_cameo;
	delete drawer_dropship_alpha;
	delete background_surface;
	delete [] drop_shape_buf;
	for (i = 0; i < 4; ++i) {
		delete [] green_shapes[i];
	}
	for (i = 0; i < button_count; ++i) {
		delete buttons[i];
	}
	delete [] buttons;

	Hide_Mouse();

	delete AlphaBuffer;
	AlphaBuffer = 0;
	AlphaBuffer = new ABuffer(alpha_area);

	if (DepthBuffer) {
		delete DepthBuffer;
		DepthBuffer = 0;
	}
	DepthBuffer = new ZBuffer(alpha_area);

	Show_Mouse();
}

BitFont * __fastcall FUN_004A60D0(int flags);

static inline Point2D * Scratch(Point2D const & p) { return (Point2D *)&p; }

// FUNCTION: GAMEMD 0x004B96A0
void Draw_Cost(int cost, int maxcost, Surface & surface, Point2D const & drawpoint, wchar_t const * title, bool center, ConvertClass & drawer, Blitter const * blitter)
{
	static const int _width = 110;
	static const int _height = 15;

	static wchar_t cost_text[40];
	static wchar_t max_text[40];

	TextPrintType type = TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW);
	if (center) {
		type = TextPrintType(type|TPF_CENTER);
	}
	BitFont * font = FUN_004A60D0(type);

	swprintf(cost_text, L"%s%d", title, cost);
	swprintf(max_text, L"%s%d", title, maxcost);

	int x1, x2;

	if (center) {
		x1 = drawpoint.X - font->FUN_00433ED0(cost_text, 0) / 2;
		x2 = drawpoint.X - font->FUN_00433ED0(max_text, 0) / 2;
	} else {
		x1 = drawpoint.X;
		x2 = drawpoint.X;
	}

	Rect source = Union(Rect(x1, drawpoint.Y, _width, _height), Rect(x2, drawpoint.Y, _width, _height));
	Rect clip = source;
	clip.X -= (HiddenSurface->Get_Width() - surface.Get_Width()) / 2;
	clip.Y -= (HiddenSurface->Get_Height() - surface.Get_Height()) / 2;
	Blit_Block(*HiddenSurface, drawer, surface, clip, Point2D(0, 0), source, NULL, blitter);

	Fancy_Text_Print_Wide(Scratch(Point2D()), cost_text, HiddenSurface, &HiddenSurface->Get_Rect(), &drawpoint, Fetch_Scheme_By_Name("Yellow"), TBLACK, type);
}

// FUNCTION: GAMEMD 0x004B9890
void Draw_Unit_Info(Surface * surface, ConvertClass * drawer, TechnoTypeClass * techtype, bool allowed)
{
	static const int _xpos = 450;
	static const int _ypos = 300;
	static const int _width = 190;
	static const int _height = 80;
	static const int _offset = 15;

	Rect rect1 = Rect(_xpos, _ypos, _width, _height);
	Point2D point = Point2D((HiddenSurface->Get_Width() - surface->Get_Width()) / 2, (HiddenSurface->Get_Height() - surface->Get_Height()) / 2);

	Blit_Block(*HiddenSurface, *drawer, *surface, rect1, Point2D(0, 0), Rect(_xpos, _ypos, _width, _height) + point, NULL, drawer->Blitter_From_Flags(SHAPE_NORMAL));

	if (techtype != NULL) {
		wchar_t name[40];
		wchar_t cost[20];
		wchar_t armament[40];
		wchar_t armor[30];

		swprintf(name, L"Name: %s", (char *)techtype->GivenName);

		if (allowed) {
			swprintf(cost, L"Cost: %d", techtype->Raw_Cost());
		} else {
			swprintf(cost, L"Cost: N/A");
		}

		if (techtype->GetWeapon(0)->Weapon != NULL) {
			swprintf(armament, L"Armament: %s", (char *)techtype->GetWeapon(0)->Weapon->GivenName);
		} else {
			swprintf(armament, L"Armament: NONE");
		}

		swprintf(armor, L"Armor: %s", ArmorName[techtype->Armor]);

		ColorScheme * scheme;
		if (allowed) {
			scheme = Fetch_Scheme_By_Name("Green");
		} else {
			scheme = Fetch_Scheme_By_Name("Red");
		}

		Fancy_Text_Print_Wide(Scratch(Point2D()), name, HiddenSurface, &HiddenSurface->Get_Rect(), &(Point2D(_xpos, _ypos) + point), scheme, TBLACK, TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW));
		Fancy_Text_Print_Wide(Scratch(Point2D()), armament, HiddenSurface, &HiddenSurface->Get_Rect(), &(Point2D(_xpos, _ypos + _offset) + point), scheme, TBLACK, TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW));
		Fancy_Text_Print_Wide(Scratch(Point2D()), armor, HiddenSurface, &HiddenSurface->Get_Rect(), &(Point2D(_xpos, _ypos + _offset * 2) + point), scheme, TBLACK, TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW));
		Fancy_Text_Print_Wide(Scratch(Point2D()), cost, HiddenSurface, &HiddenSurface->Get_Rect(), &(Point2D(_xpos, _ypos + _offset * 3) + point), scheme, TBLACK, TextPrintType(TPF_6PT_GRAD|TPF_FULLSHADOW));
	}
}

// TEMPLATE: GAMEMD 0x004B9BE0
// ?Clear@?$DynamicVectorClass@PAUButtonFadeEffect@@@@UAEXXZ
template class DynamicVectorClass<TechnoTypeClass *>;

// TEMPLATE: GAMEMD 0x004B9C10
// ?ID@?$DynamicVectorClass@PAUButtonFadeEffect@@@@UAEHPBQAUButtonFadeEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9C30
// ?GetItem@?$VectorClass@PAUButtonFadeEffect@@@@UBEPAUButtonFadeEffect@@H@Z

// TEMPLATE: GAMEMD 0x004B9C40
// ?Clear@?$DynamicVectorClass@PAUCrossDissolveEffect@@@@UAEXXZ
template class DynamicVectorClass<ButtonFadeEffect *>;

// TEMPLATE: GAMEMD 0x004B9C70
// ?ID@?$DynamicVectorClass@PAUCrossDissolveEffect@@@@UAEHPBQAUCrossDissolveEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9C90
// ?GetItem@?$VectorClass@PAUCrossDissolveEffect@@@@UBEPAUCrossDissolveEffect@@H@Z

// TEMPLATE: GAMEMD 0x004B9CA0
// ??8?$VectorClass@PAUButtonFadeEffect@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x004B9CE0
// ?Resize@?$VectorClass@PAUButtonFadeEffect@@@@UAE_NHPBQAUButtonFadeEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9D90
// ?Clear@?$VectorClass@PAUButtonFadeEffect@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x004B9DC0
// ?ID@?$VectorClass@PAUButtonFadeEffect@@@@UAEHABQAUButtonFadeEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9E00
// ?ID@?$VectorClass@PAUButtonFadeEffect@@@@UAEHPBQAUButtonFadeEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9E20
// ?Resize@?$DynamicVectorClass@PAUButtonFadeEffect@@@@UAE_NHPBQAUButtonFadeEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9ED0
// ?ID@?$DynamicVectorClass@PAUButtonFadeEffect@@@@UAEHABQAUButtonFadeEffect@@@Z

struct CrossDissolveEffect;
template class DynamicVectorClass<CrossDissolveEffect *>;

// TEMPLATE: GAMEMD 0x004B9F00
// ??8?$VectorClass@PAUCrossDissolveEffect@@@@UBE_NABV0@@Z

// TEMPLATE: GAMEMD 0x004B9F40
// ?Resize@?$VectorClass@PAUCrossDissolveEffect@@@@UAE_NHPBQAUCrossDissolveEffect@@@Z

// TEMPLATE: GAMEMD 0x004B9FF0
// ?Clear@?$VectorClass@PAUCrossDissolveEffect@@@@UAEXXZ

// TEMPLATE: GAMEMD 0x004BA020
// ?ID@?$VectorClass@PAUCrossDissolveEffect@@@@UAEHABQAUCrossDissolveEffect@@@Z

// TEMPLATE: GAMEMD 0x004BA060
// ?ID@?$VectorClass@PAUCrossDissolveEffect@@@@UAEHPBQAUCrossDissolveEffect@@@Z

// TEMPLATE: GAMEMD 0x004BA080
// ?Resize@?$DynamicVectorClass@PAUCrossDissolveEffect@@@@UAE_NHPBQAUCrossDissolveEffect@@@Z

// TEMPLATE: GAMEMD 0x004BA130
// ?ID@?$DynamicVectorClass@PAUCrossDissolveEffect@@@@UAEHABQAUCrossDissolveEffect@@@Z

// SYNTHETIC: GAMEMD 0x004BA160
// ??_G?$DynamicVectorClass@PAVTechnoTypeClass@@@@UAEPAXI@Z
template class VectorClass<TechnoTypeClass *>;

// SYNTHETIC: GAMEMD 0x004BA1B0
// ??_G?$VectorClass@PAUButtonFadeEffect@@@@UAEPAXI@Z
template class VectorClass<ButtonFadeEffect *>;

// SYNTHETIC: GAMEMD 0x004BA200
// ??_G?$DynamicVectorClass@PAUButtonFadeEffect@@@@UAEPAXI@Z

// SYNTHETIC: GAMEMD 0x004BA250
// ??_G?$VectorClass@PAUCrossDissolveEffect@@@@UAEPAXI@Z
template class VectorClass<CrossDissolveEffect *>;

// SYNTHETIC: GAMEMD 0x004BA2A0
// ??_G?$DynamicVectorClass@PAUCrossDissolveEffect@@@@UAEPAXI@Z

// TEMPLATE: GAMEMD 0x004BA2F0
// ??0?$VectorClass@PAUButtonFadeEffect@@@@QAE@HPBQAUButtonFadeEffect@@@Z

// TEMPLATE: GAMEMD 0x004BA340
// ??0?$VectorClass@PAUCrossDissolveEffect@@@@QAE@HPBQAUCrossDissolveEffect@@@Z

// FUNCTION: GAMEMD 0x004B9BD0
// RGBClass::RGBClass
