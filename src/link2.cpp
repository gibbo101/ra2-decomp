// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"
#include "geometry.h"
#include "rect.h"
#include "linetrail.h"
#include "object.h"
#include "goptions.h"
#include "_tactica.h"
#include "tactical.h"
#include "dsurface.h"
#include "globals.h"

// SYNTHETIC: GAMEMD 0x00556790 SYMBOL
// _$E2
// SYNTHETIC: GAMEMD 0x005567C0 SYMBOL
// _$E5
// SYNTHETIC: GAMEMD 0x005567E0 SYMBOL
// _$E8
// SYNTHETIC: GAMEMD 0x00556800 SYMBOL
// _$E11
// SYNTHETIC: GAMEMD 0x00556820 SYMBOL
// _$E14
// SYNTHETIC: GAMEMD 0x00556840 SYMBOL
// _$E17
// SYNTHETIC: GAMEMD 0x00556860 SYMBOL
// _$E20
// SYNTHETIC: GAMEMD 0x005568A0 SYMBOL
// _$E23
// SYNTHETIC: GAMEMD 0x005568D0 SYMBOL
// _$E26
// SYNTHETIC: GAMEMD 0x00556900 SYMBOL
// _$E29
// SYNTHETIC: GAMEMD 0x00556930 SYMBOL
// _$E32
// SYNTHETIC: GAMEMD 0x00556940 SYMBOL
// _$E35
// SYNTHETIC: GAMEMD 0x00556960 SYMBOL
// _$E38
// SYNTHETIC: GAMEMD 0x00556980 SYMBOL
// _$E41

// SYNTHETIC: GAMEMD 0x005569A0 SYMBOL
// _$E78
// SYNTHETIC: GAMEMD 0x005569E0 SYMBOL
// _$E76
DynamicVectorClass<LineTrail *> LineTrailArray(0);


// FUNCTION: GAMEMD 0x00556AD0
LineTrail::~LineTrail(void)
{
	if (Owner != NULL) {
		Owner->LineTrailer = NULL;
		Owner = NULL;
	}
	LineTrailArray.Delete(this);
}

// FUNCTION: GAMEMD 0x00556B30
void LineTrail::Release_Owner(void)
{
	if (Owner != NULL) {
		Owner->LineTrailer = NULL;
		Owner = NULL;
	}
}

// FUNCTION: GAMEMD 0x00556B50
void LineTrail::Set_Decrement(int value)
{
	if (!Options.DetailLevel) value <<= 1;
	Decrement = value;
}

// FUNCTION: GAMEMD 0x00556B70
void LineTrail::Update(void)
{
	if (Owner != NULL) {
		Coord position = Owner->Position;
		if (position != Trails[ActiveSlot].Position) {
			int & active = ActiveSlot;
			if (--active < 0) active = 31;
			LineTrailNode * node = &Trails[active];
			node->Position = position;
			node->Value = 255;
		}
	}

	int slot = ActiveSlot;
	do {
		LineTrailNode * fade = &Trails[slot];
		fade->Value -= Decrement;
		if (fade->Value < 0) fade->Value = 0;
		slot++;
		if (slot >= 32) slot = 0;
	} while (slot != ActiveSlot);
}

// FUNCTION: GAMEMD 0x00556C00
void LineTrail::Draw(void)
{
	int slot = ActiveSlot + 1;
	if (slot >= 32) slot = 0;

	LineTrailNode * to = &Trails[ActiveSlot];
	if (to->Position == COORD_NONE) return;

	for (;;) {
		LineTrailNode * from = to;
		to = &Trails[slot];
		if (from->Value == 0 || to->Position == COORD_NONE) break;

		Point2D start;
		Point2D end;
		TacticalMap->Coord_To_Pixel(from->Position, start);
		TacticalMap->Coord_To_Pixel(to->Position, end);
		int start_height = -2 - Tactical::Z_Lepton_To_Pixel(from->Position.Z);
		int end_height = -2 - Tactical::Z_Lepton_To_Pixel(to->Position.Z);
		DSurface::Composite->FUN_004BEAC0(DSurface::ViewBounds, start, end, Color, from->Value, start_height, end_height);

		slot++;
		if (slot >= 32) slot = 0;
		if (slot == ActiveSlot) break;
	}
}

// FUNCTION: GAMEMD 0x00556D20
bool LineTrail::Is_Expired(void) const
{
	return(Owner == NULL && Trails[ActiveSlot].Value == 0);
}

// FUNCTION: GAMEMD 0x00556D40
void LineTrail::Process_All(void)
{
	for (int index = LineTrailArray.Count() - 1; index >= 0; index--) {
		LineTrail * trail = LineTrailArray[index];
		trail->Update();
		if (trail->Is_Expired()) {
			delete trail;
		} else {
			trail->Draw();
		}
	}
}

// FUNCTION: GAMEMD 0x00556DF0
void LineTrail::Delete_All(void)
{
	for (int index = 0; index < LineTrailArray.Count(); index++) {
		LineTrail * trail = LineTrailArray[index];
		if (trail != NULL) {
			delete trail;
		}
	}
	LineTrailArray.Clear();
}

// TEMPLATE: GAMEMD 0x00556EB0
// ?Clear@?$DynamicVectorClass@PAVLineTrail@@@@UAEXXZ
// TEMPLATE: GAMEMD 0x00556EE0
// ?ID@?$DynamicVectorClass@PAVLineTrail@@@@UAEHPBQAVLineTrail@@@Z
// TEMPLATE: GAMEMD 0x00556F00
// ?GetItem@?$VectorClass@PAVLineTrail@@@@UBEPAVLineTrail@@H@Z
template class DynamicVectorClass<LineTrail *>;

// TEMPLATE: GAMEMD 0x00556F10
// ??8?$VectorClass@PAVLineTrail@@@@UBE_NABV0@@Z
// TEMPLATE: GAMEMD 0x00556F50
// ?Resize@?$VectorClass@PAVLineTrail@@@@UAE_NHPBQAVLineTrail@@@Z
// TEMPLATE: GAMEMD 0x00557000
// ?Clear@?$VectorClass@PAVLineTrail@@@@UAEXXZ
template class VectorClass<LineTrail *>;

// TEMPLATE: GAMEMD 0x00557030
// ?ID@?$VectorClass@PAVLineTrail@@@@UAEHABQAVLineTrail@@@Z
// TEMPLATE: GAMEMD 0x00557070
// ?ID@?$VectorClass@PAVLineTrail@@@@UAEHPBQAVLineTrail@@@Z
// TEMPLATE: GAMEMD 0x00557090
// ?Resize@?$DynamicVectorClass@PAVLineTrail@@@@UAE_NHPBQAVLineTrail@@@Z
// TEMPLATE: GAMEMD 0x00557140
// ?ID@?$DynamicVectorClass@PAVLineTrail@@@@UAEHABQAVLineTrail@@@Z
// SYNTHETIC: GAMEMD 0x00557170
// ??_G?$VectorClass@PAVLineTrail@@@@UAEPAXI@Z
// SYNTHETIC: GAMEMD 0x005571C0
// ??_G?$DynamicVectorClass@PAVLineTrail@@@@UAEPAXI@Z
