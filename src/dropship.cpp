// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "dropship.h"
#include "_alpha.h"
#include "_convert.h"
#include "_font.h"
#include "_keyboar.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "_zbuffer.h"
#include "bsurface.h"
#include "ccfile.h"
#include "ccrand.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "draw.h"
#include "dsurface.h"
#include "fastmath.h"
#include "font.h"
#include "globals.h"
#include "house.h"
#include "infatype.h"
#include "keyboard.h"
#include "language\language.h"
#include "misc.h"
#include "mixfile.h"
#include "mouse.h"
#include "rules.h"
#include "scenario.h"
#include "scheme.h"
#include "shapebtn.h"
#include "shapeset.h"
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

// FUNCTION: 0x004B69B0
DropshipLoadoutClass::DropshipLoadoutClass(void) :
	CreationFrame(Frame),
	Unused2(INT_MAX),
	UnusedBool1(false),
	EntryCount(0),
	TotalCost(0)
{
}

// FUNCTION: 0x004B69D0
DropshipLoadoutClass::~DropshipLoadoutClass(void)
{
}

// FUNCTION: 0x004B69E0
bool DropshipLoadoutClass::Add(TechnoTypeClass * ttype)
{
	if (EntryCount < MAX_ENTRIES) {
		int usage_index;

		if (!Scen->AllowableUnits.Count()) {
			usage_index = -1;
		} else {
			usage_index = Get_Allowable_Index(ttype);
		}

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

// FUNCTION: 0x004B6AB0
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

// FUNCTION: 0x004B6B80
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

// FUNCTION: 0x004B6BB0
void DropshipLoadoutClass::Swizzle(void)
{
	for (int i = 0; i < EntryCount; i++) {
		Swizzle_Pointer(&Entries[i]);
	}
}

// FUNCTION: 0x004B6BE0
TechnoTypeClass * DropshipLoadoutClass::Fetch(int index)
{
	return(Entries[index]);
}

// FUNCTION: 0x004B6BF0
int DropshipLoadoutClass::Get_Allowable_Index(TechnoTypeClass * ttype)
{
	if (Scen->AllowableUnits.Count()) {
		for (int i = 0; i < Scen->AllowableUnits.Count(); ++i) {
			if (Scen->AllowableUnits[i] == ttype && Scen->AllowableUnitMaximums[i] != -1) {
				return(i);
			}
		}
	}
	return(-1);
}

struct ButtonFadeEffect
{
	unsigned int StartTime;
	int CandidateIndex;
	float Alpha;
	float Direction;
	bool StopAtLow;
};

BitFont * __fastcall FUN_004A60D0(int flags);

static inline Point2D * Scratch(Point2D const & p) { return (Point2D *)&p; }

// FUNCTION: 0x004B96A0
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

// FUNCTION: 0x004B9890
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

// FUNCTION: 0x004B9BE0 ?$DynamicVectorClass@PAVTechnoTypeClass@@::Clear
template class DynamicVectorClass<TechnoTypeClass *>;

// FUNCTION: 0x004B9C10 ?$DynamicVectorClass@PAVTechnoTypeClass@@::ID

// FUNCTION: 0x004B9C30 ?$VectorClass@PAVTechnoTypeClass@@::GetItem

// FUNCTION: 0x004B9C40 ?$DynamicVectorClass@PAUButtonFadeEffect@@::Clear
template class DynamicVectorClass<ButtonFadeEffect *>;

// FUNCTION: 0x004B9C70 ?$DynamicVectorClass@PAUButtonFadeEffect@@::ID

// FUNCTION: 0x004B9C90 ?$VectorClass@PAUButtonFadeEffect@@::GetItem

// FUNCTION: 0x004B9CA0 ?$VectorClass@PAUButtonFadeEffect@@::operator==

// FUNCTION: 0x004B9CE0 ?$VectorClass@PAUButtonFadeEffect@@::Resize

// FUNCTION: 0x004B9D90 ?$VectorClass@PAUButtonFadeEffect@@::Clear

// FUNCTION: 0x004B9DC0 ?$VectorClass@PAUButtonFadeEffect@@::ID

// FUNCTION: 0x004B9E00 ?$VectorClass@PAUButtonFadeEffect@@::ID

// FUNCTION: 0x004B9E20 ?$DynamicVectorClass@PAUButtonFadeEffect@@::Resize

// FUNCTION: 0x004B9ED0 ?$DynamicVectorClass@PAUButtonFadeEffect@@::ID

struct CrossDissolveEffect;
template class DynamicVectorClass<CrossDissolveEffect *>;

// FUNCTION: 0x004B9F00 ?$VectorClass@PAUCrossDissolveEffect@@::operator==

// FUNCTION: 0x004B9F40 ?$VectorClass@PAUCrossDissolveEffect@@::Resize

// FUNCTION: 0x004B9FF0 ?$VectorClass@PAUCrossDissolveEffect@@::Clear

// FUNCTION: 0x004BA020 ?$VectorClass@PAUCrossDissolveEffect@@::ID

// FUNCTION: 0x004BA060 ?$VectorClass@PAUCrossDissolveEffect@@::ID

// FUNCTION: 0x004BA080 ?$DynamicVectorClass@PAUCrossDissolveEffect@@::Resize

// FUNCTION: 0x004BA130 ?$DynamicVectorClass@PAUCrossDissolveEffect@@::ID

// FUNCTION: 0x004BA160 ?$VectorClass@PAVTechnoTypeClass@@::~?$VectorClass@PAVTechnoTypeClass@@
template class VectorClass<TechnoTypeClass *>;

// FUNCTION: 0x004BA1B0 ?$VectorClass@PAUButtonFadeEffect@@::~?$VectorClass@PAUButtonFadeEffect@@
template class VectorClass<ButtonFadeEffect *>;

// FUNCTION: 0x004BA200 ?$DynamicVectorClass@PAUButtonFadeEffect@@::~?$DynamicVectorClass@PAUButtonFadeEffect@@

// FUNCTION: 0x004BA250 ?$VectorClass@PAUCrossDissolveEffect@@::~?$VectorClass@PAUCrossDissolveEffect@@
template class VectorClass<CrossDissolveEffect *>;

// FUNCTION: 0x004BA2A0 ?$DynamicVectorClass@PAUCrossDissolveEffect@@::~?$DynamicVectorClass@PAUCrossDissolveEffect@@

// FUNCTION: 0x004BA2F0 ?$VectorClass@PAUButtonFadeEffect@@::?$VectorClass@PAUButtonFadeEffect@@

// FUNCTION: 0x004BA340 ?$VectorClass@PAUCrossDissolveEffect@@::?$VectorClass@PAUCrossDissolveEffect@@

// FUNCTION: 0x004BA390
