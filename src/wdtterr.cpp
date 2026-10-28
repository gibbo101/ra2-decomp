// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2026 ra2-decomp contributors
// Copyright 2025 Electronic Arts Inc.; 2026 OpenTS contributors
// Derived from Electronic Arts source code; EA's GPL Section 7 terms apply (COPYING.md).

#include "always.h"

#include <stddef.h>

#include "ccfile.h"
#include "ini.h"
#include "stringtable.h"
#include "wdtnet.h"
#include "rect.h"
#include "geometry.h"

// SYNTHETIC: GAMEMD 0x0076F700 SYMBOL
// _$E7
// SYNTHETIC: GAMEMD 0x0076F720 SYMBOL
// _$E10
// SYNTHETIC: GAMEMD 0x0076F750 SYMBOL
// _$E13
// SYNTHETIC: GAMEMD 0x0076F770 SYMBOL
// _$E16
// SYNTHETIC: GAMEMD 0x0076F790 SYMBOL
// _$E19
// SYNTHETIC: GAMEMD 0x0076F7B0 SYMBOL
// _$E22
// SYNTHETIC: GAMEMD 0x0076F7D0 SYMBOL
// _$E25
// SYNTHETIC: GAMEMD 0x0076F7F0 SYMBOL
// _$E28
// SYNTHETIC: GAMEMD 0x0076F830 SYMBOL
// _$E31
// SYNTHETIC: GAMEMD 0x0076F860 SYMBOL
// _$E34
// SYNTHETIC: GAMEMD 0x0076F890 SYMBOL
// _$E37
// SYNTHETIC: GAMEMD 0x0076F8C0 SYMBOL
// _$E40
// SYNTHETIC: GAMEMD 0x0076F8D0 SYMBOL
// _$E43
// SYNTHETIC: GAMEMD 0x0076F8F0 SYMBOL
// _$E46

using namespace WorldDominationTour;

struct Drawing {
	static int RenderWidth;
};

Wstring operator+(Wstring const & a, Wstring const & b);

// TEMPLATE: GAMEMD 0x0076F910
// ?FUN_0076F910@WorldDominationTour@@YI?AV?$rc_ptr@VTerritory@WorldDominationTour@@@@HABVINIClass@@ABVWstring@@11@Z
rc_ptr<Territory> __fastcall WorldDominationTour::FUN_0076F910(int id, INIClass const & ini, Wstring const & section, Wstring const & fileprefix, Wstring const & labelprefix)
{
	return rc_ptr<Territory>(new Territory(id, ini, section, fileprefix, labelprefix));
}

// FUNCTION: GAMEMD 0x0076F970
// WorldDominationTour::Territory::Territory
Territory::Territory(int id, INIClass const & ini, Wstring const & section, Wstring const & fileprefix, Wstring const & labelprefix) :
	ID(id),
	Target(0, 0),
	Origin(0, 0),
	OwnerAnim(NULL),
	HighlightAnim(NULL),
	TargetAnim(NULL)
{
	FileName = operator+(fileprefix, Wstring_FromInt(id, 2)) + ".shp";

	Wstring label = operator+(labelprefix, Wstring_FromInt(id, 2));
	Name = StringTable::LoadString(label.get(), NULL, "D:\\ra2mdpost\\WDTTerr.cpp", 0x27);

	Wstring width = Wstring_FromInt(Drawing::RenderWidth);
	Target = ini.Get_Point(section, "Target_" + width, Target);
	Origin = ini.Get_Point(section, "Origin_" + width, Origin);

	if (Drawing::RenderWidth > 800) {
		if (Target.X == 0 && Target.Y == 0) {
			Target = ini.Get_Point(section, "Target_800", Target);
		}
		if (Origin.X == 0 && Origin.Y == 0) {
			Origin = ini.Get_Point(section, "Origin_800", Origin);
		}
	}
}

// FUNCTION: GAMEMD 0x0076FB90
// WorldDominationTour::Territory::FUN_0076FB90
void Territory::FUN_0076FB90(Selection * engine, ConvertClass * drawer, Point2D const & pos)
{
	int x = Origin.X + pos.X;
	int y = Origin.Y + pos.Y;
	CCFileClass file(FileName);
	if (file.Is_Available(0)) {
		OwnerAnim = new MSOverlayAnim(FileName, x, y, drawer, 0, engine->Get_Anims(), true, -1, false);
		OwnerAnim->Set_Active(false);
		engine->Add_Animation(OwnerAnim);
	}
}

// FUNCTION: GAMEMD 0x0076FC50
// WorldDominationTour::Territory::FUN_0076FC50
void Territory::FUN_0076FC50(Selection * engine, ConvertClass * drawer, Point2D const & pos)
{
	int x = Origin.X + pos.X;
	int y = Origin.Y + pos.Y;
	CCFileClass file(FileName);
	if (file.Is_Available(0)) {
		HighlightAnim = new MSOverlayAnim(FileName, x, y, drawer, 0, engine->Get_Anims(), true, 0, false);
		HighlightAnim->Set_Active(false);
		engine->Add_Animation(HighlightAnim);
	}
}

// FUNCTION: GAMEMD 0x0076FD10
// WorldDominationTour::Territory::~Territory
Territory::~Territory(void)
{
}

// FUNCTION: GAMEMD 0x0076FD40
// WorldDominationTour::Territory::FUN_0076FD40
void Territory::FUN_0076FD40(int state, Selection * engine, bool wait)
{
	if (OwnerAnim != NULL) {
		int frame = -1;
		switch (state) {
			case 1:
				frame = 3;
				break;
			case 2:
				frame = 2;
				break;
			case 3:
				frame = 1;
				break;
		}
		Rect oldanimrect = OwnerAnim->Get_Rect();
		if (frame >= 0) {
			OwnerAnim->Set_Frame(frame);
			OwnerAnim->Set_Active(true);
			OwnerAnim->Restart();
		} else {
			OwnerAnim->Set_Active(false);
		}

		Rect newanimrect = OwnerAnim->Get_Rect();
		Rect rect = Union(newanimrect, oldanimrect);
		engine->Restore_Anims(rect);
		engine->Restore_And_Advance();
		engine->Wait_Delay(1);
	}
}

// FUNCTION: GAMEMD 0x0076FEB0
// WorldDominationTour::Territory::FUN_0076FEB0
void Territory::FUN_0076FEB0(bool active, Selection * engine)
{
	if (HighlightAnim != NULL) {
		HighlightAnim->Set_Active(active);
		Rect rect = HighlightAnim->Get_Rect();
		engine->Restore_Anims(rect);
		engine->Restore_And_Advance();
		engine->Wait_For_Anim(HighlightAnim);
	}
}

// FUNCTION: GAMEMD 0x0076FF20
// WorldDominationTour::Territory::FUN_0076FF20
void Territory::FUN_0076FF20(MSShapeAnim * anim)
{
	TargetAnim = anim;
}

// FUNCTION: GAMEMD 0x0076FF30
// WorldDominationTour::Territory::FUN_0076FF30
MSShapeAnim * Territory::FUN_0076FF30(void)
{
	MSShapeAnim * old = TargetAnim;
	TargetAnim = NULL;
	return(old);
}

// SYNTHETIC: GAMEMD 0x00770020
// ??_GTerritory@WorldDominationTour@@UAEPAXI@Z
